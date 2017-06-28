
#include "DeviceWatchLightPort.h"


CDeviceWatchLightPort::CDeviceWatchLightPort()
{
	m_OpenFlag = false;
	m_nLightLuminence = 0;
	m_nPortChannel = 0;
}


CDeviceWatchLightPort::~CDeviceWatchLightPort()
{
	ClosePort();
}

void CDeviceWatchLightPort::ClosePort()
{
	if (this->m_OpenFlag)
	{
		//SetCommMask(m_hCom, 0);
		if (m_hCom != NULL)
			CloseHandle(m_hCom);
		m_OpenFlag = false;
	}
}

bool CDeviceWatchLightPort::OpenPort(CString comPortName)
{
	m_nPortChannel = GetPortNumber(comPortName);
	comPortName = _T("\\\\.\\") + comPortName;
	m_hCom = CreateFile(comPortName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (m_hCom == INVALID_HANDLE_VALUE)
	{
		m_OpenFlag = false;
		return m_OpenFlag;
	}

	GetCommState(m_hCom, &dcb);
	dcb.BaudRate = 9600;    //波特率为9600
	dcb.StopBits = 0;       //一个停止位为0，两个是1
	dcb.ByteSize = 8;       //每个字节有8位
	dcb.Parity = NOPARITY;  //无奇偶校验位

	SetCommState(m_hCom, &dcb);

	if(SendData(0,5,0,1))
	{
		m_OpenFlag = true;
	}
	else
	{
		m_OpenFlag = false;
	}
	return m_OpenFlag;
}

DWORD CDeviceWatchLightPort::WriteSerial(BYTE * chBuf, DWORD dwLength)
{
  memset(&m_osWrite, 0, sizeof(OVERLAPPED));
  BOOL bState;
  COMSTAT comStat;
  DWORD dwErrorFlags;
  ClearCommError(m_hCom, &dwErrorFlags, &comStat);
  bState = WriteFile(m_hCom, chBuf, dwLength, &dwLength, &m_osWrite);
  if (!bState)
  {
    if (GetLastError() == ERROR_IO_PENDING)
    {
      GetOverlappedResult(m_hCom, &m_osWrite, &dwLength, TRUE);// 等待
    }
    else
    {
      dwLength = 0;
    }
  }
  return dwLength;
}

DWORD CDeviceWatchLightPort::ReadSerial(BYTE* chBuf, DWORD dwLength)
{
  DWORD nLen;
  COMSTAT comStat;
  DWORD dwErrorFlags;
  ClearCommError(m_hCom, &dwErrorFlags, &comStat);
  nLen = min(dwLength, comStat.cbInQue);
  ReadFile(m_hCom, chBuf, nLen, &nLen, &m_osRead);
  return nLen;
}

bool CDeviceWatchLightPort::SendData(BYTE byAddress, BYTE byCommand, BYTE byValue, BYTE byTrigger)
{
  PurgeComm(this->m_hCom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

  BYTE buf[6] = { 0x24 };
  BYTE reciveBuf[20];
  DWORD readLen = 0;
  buf[0] = 0x24;
  buf[1] = byAddress;
  buf[2] = byCommand;
  buf[3] = byValue;
  buf[4] = byTrigger;
  buf[5] = buf[0] ^ buf[1] ^ buf[2] ^ buf[3] ^ buf[4];

  if (WriteSerial(buf, 6) <= 0)
  {
    return false;
  }

  Sleep(50);
  int getReciveLength = 0, readBufferLength = 0, c = 0;
  CString strShow;
  for (c = 0; c<10; c++)
  {
    getReciveLength = GetBufferLength();
    if (getReciveLength>0)
    {
      /*strShow.Format("length=%d",getReciveLength);
      this->m_List.AddString(strShow);*/
      readBufferLength = ReadSerial(reciveBuf, GetBufferLength());
      if (readBufferLength == 2)//为什么是两个字节的长度
      {
        if (reciveBuf[1] == 0x24)
        {
          return true;
        }
        else if (reciveBuf[1] == 0x26)
        {
          return false;
        }
        else
        {
          return false;
        }
      }
      else if (readBufferLength>2)
      {
        //读取
        if (reciveBuf[0] == 0x24)
        {
          CString strValue;
          if (reciveBuf[1] >= 0 && reciveBuf[1] <= 15)
          {
            if (reciveBuf[2] == 5)
            {
              m_nLightLuminence = reciveBuf[3];
            }
            else
            {
              strShow.Format(_T("Command is not to read data Command =%d"), reciveBuf[2]);
            }
          }
          else
          {
	          return false;
	        }
        }
        else
        {
	        //SetDlgItemText(IDC_COM_STATE_SHOW_STATIC, _T("Data format error"));
          return false;
        }
      }
      else
      {
        strShow.Format(_T("Data length error readBufferLength=%d"), readBufferLength);
        //SetDlgItemText(IDC_COM_STATE_SHOW_STATIC, strShow);
        return false;
      }
      break;
    }
    Sleep(30);
  }
  if (c >= 10)
  {
    return false;
  }
  return true;
}

DWORD CDeviceWatchLightPort::GetBufferLength()
{
  memset(&m_osRead, 0, sizeof(OVERLAPPED));
  COMSTAT comStat;
  DWORD dwErrorFlags;
  ClearCommError(m_hCom, &dwErrorFlags, &comStat);
  return comStat.cbInQue;
}

bool CDeviceWatchLightPort::InitLightPort()
{
  CString portName = _T("");
  portName.Format(_T("COM%d"),WATCH_LIGHT_PORT);
  bool bResult = false;

  bResult = OpenPort(portName);
  return true;
}

int CDeviceWatchLightPort::GetLightLuminence()
{
	return m_nLightLuminence;
}

int CDeviceWatchLightPort::GetPortNumber(CString portName)
{
	int nResult = -1;
	if(portName.IsEmpty())
		return nResult;

	CString port = _T("COM");
	CString portNumber = _T("");
	portNumber = portName.Right(portName.GetLength() - port.GetLength());

	nResult = _wtoi(portNumber);

	return nResult;

}