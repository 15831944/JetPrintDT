#include "../stdafx.h"
#include "DeviceADModulePort.h"


CDeviceADModulePort::CDeviceADModulePort(void)
{
	m_com = INVALID_HANDLE_VALUE;
}


CDeviceADModulePort::~CDeviceADModulePort(void)
{
}

BOOL CDeviceADModulePort::IsOpen()
{
	return m_com != INVALID_HANDLE_VALUE;
}



//���ö�д��ʱ�ṹ
void CDeviceADModulePort::SetTimerOut( unsigned long dwTimerOut/*=5000*/ )
{
	if(!IsOpen())
	{
		return;
	}

	//��һ�λ����������ݺ���������
	CommTimerOuts.ReadIntervalTimeout = MAXWORD;
	CommTimerOuts.ReadTotalTimeoutConstant = 0;
	CommTimerOuts.ReadTotalTimeoutMultiplier = 0;

	//����д��ʱ����
	CommTimerOuts.WriteTotalTimeoutConstant = dwTimerOut;
	CommTimerOuts.WriteTotalTimeoutMultiplier = 0;

	//���ó�ʱ
	SetCommTimeouts(m_com,&CommTimerOuts);
}

//����dcb����
BOOL CDeviceADModulePort::SetDBCParam( 
	unsigned long xBabd, 
	unsigned char xDataSize,
	unsigned char xParity,
	unsigned char xStopBit )
{
	if(!IsOpen())
	{
		return FALSE;
	}

	if(!GetCommState(m_com,&dcb))
	{
		ClosePort();
		return FALSE;
	}

	//����ͨѶ����
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = xBabd;
	dcb.ByteSize = xDataSize;
	dcb.Parity = xParity;
	dcb.StopBits = xStopBit;

	if(!SetCommState(m_com,&dcb))
	{
		/*CString str;
		str.Format(TEXT("%d"),GetLastError());
		AfxMessageBox(str);*/
		ClosePort();
		return FALSE;
	}

	return TRUE;
}

//���ö˿ڻ�������С
BOOL CDeviceADModulePort::SetPortBuffSize( unsigned long InputBuffSize, unsigned long OutputBuffSize )
{
	if(!IsOpen())
	{
		return FALSE;
	}
	if(! SetupComm(m_com,InputBuffSize,OutputBuffSize))
	{
		ClosePort();
		return FALSE;
	}

	return TRUE;
}

//��������
void CDeviceADModulePort::ClearBuffer()
{
	if(! IsOpen())
	{
		return;
	}

	PurgeComm(m_com,PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
}

//��ǰ���ջ������ֽ���Ŀ
unsigned long CDeviceADModulePort::GetInBufferCount()
{
	if(! IsOpen())
	{
		return (0);
	}

	DWORD dwErrorFlags;
	COMSTAT Comstat;

	ClearCommError(m_com,&dwErrorFlags,&Comstat);
	return Comstat.cbInQue;
}

//��ǰ���ͻ������ֽ���Ŀ
unsigned long CDeviceADModulePort::GetOutBufferCount()
{
	if(! IsOpen())
	{
		return (0);
	}

	DWORD dwErrorFlags;
	COMSTAT Comstat;

	ClearCommError(m_com,&dwErrorFlags,&Comstat);
	return Comstat.cbOutQue;
}

//�򿪴���
BOOL CDeviceADModulePort::OpenPort( unsigned long xPort, 
	/*���ں� */ unsigned long xBabd, 
	/*������ */ unsigned char xDataSize, 
	/*����λ*/ unsigned char xParity, 
	/*У��λ*/ unsigned char xStopBit, 
	/*ֹͣλ*/ unsigned long InputBuffSize,
	/*����buffer��С */ unsigned long OutputBuffSize,
	/*���buffer��С */ unsigned long dwTimerOut )
{
	if(IsOpen())
	{
		ClosePort();
	}
	//�����¼�
	memset(&m_overlappedRead,0,sizeof(OVERLAPPED));
	m_overlappedRead.hEvent = CreateEvent(NULL,/*FALSE*/FALSE,TRUE,TEXT("portread"));
	ASSERT(m_overlappedRead.hEvent != INVALID_HANDLE_VALUE);

	memset(&m_OverlappedWrite,0,sizeof(OVERLAPPED));
	m_OverlappedWrite.hEvent = CreateEvent(NULL,/*FALSE*/FALSE,TRUE,TEXT("portwrite"));
	ASSERT(m_OverlappedWrite.hEvent != INVALID_HANDLE_VALUE);

	//ȡ�ô����ַ�
	CString csPort;
	csPort.Format(TEXT("\\\\.\\COM%d"),xPort);

	m_com = CreateFile(csPort,
		               GENERIC_READ | GENERIC_WRITE,
		               0,    
		               NULL, 
		               OPEN_EXISTING, 
		               FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,    
		               NULL);
	if(m_com == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	SetPortBuffSize(InputBuffSize,OutputBuffSize);
	SetDBCParam(xBabd,xDataSize,xParity,xStopBit);
	SetTimerOut(dwTimerOut);

	//��������
	PurgeComm(m_com, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	return TRUE;

}

void CDeviceADModulePort::ClosePort()
{
	if(IsOpen())
	{
		CloseHandle(m_com);
		m_com = INVALID_HANDLE_VALUE;
	}

	if(m_overlappedRead.hEvent != NULL)
	{
		CloseHandle(m_overlappedRead.hEvent);
	}

	if(m_OverlappedWrite.hEvent != NULL)
	{
		CloseHandle(m_OverlappedWrite.hEvent);
	}
}


//�첽������
unsigned long CDeviceADModulePort::ReadChar( unsigned long dwBufferLength, 
	                           char *buff,
	                           unsigned long dwWaitTime )
{
	if(!IsOpen())
	{
		return 0;
	}

	DWORD dwError;
	COMSTAT comStat;
	if(ClearCommError(m_com,&dwError,&comStat) && dwError > 0)  //�������
	{
		PurgeComm(m_com,PURGE_RXABORT | PURGE_RXCLEAR);  //������뻺����
		return 0;
	}
	if(! comStat.cbInQue)  //������������
	{
		return 0;
	}

	unsigned long uReadLength = 0;
	dwBufferLength = dwBufferLength-1 > comStat.cbInQue ? comStat.cbInQue : dwBufferLength-1;

	if(! ::ReadFile(m_com,buff,dwBufferLength,&uReadLength,&m_overlappedRead))
	{
		if(GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_overlappedRead.hEvent,dwWaitTime);
			if(! GetOverlappedResult(m_com,&m_overlappedRead,&uReadLength,FALSE))
			{
				if(GetLastError() != ERROR_IO_INCOMPLETE)
				{
					uReadLength = 0;
				}
			}
		}
		else
		{
			uReadLength = 0;
		}
	}

	return uReadLength;
}

unsigned long CDeviceADModulePort::WriteChar( unsigned long dwBufferLength, char *buff )
{
	if(! IsOpen())
	{
		return 0;
	}

	DWORD dwError;
	if(ClearCommError(m_com,&dwError,NULL) && dwError > 0)
	{
		PurgeComm(m_com, PURGE_TXABORT | PURGE_TXCLEAR);
	}

	unsigned long uWriteLength = 0;
	if(! WriteFile(m_com,buff,dwBufferLength,&uWriteLength,&m_OverlappedWrite))
	{
		if(GetLastError() ==ERROR_IO_PENDING )
		{
			DWORD tmp = 0;
			tmp = WaitForSingleObject(m_OverlappedWrite.hEvent,1000);
			if(tmp == WAIT_TIMEOUT || tmp == WAIT_ABANDONED)
			{
				return 0;
			}
			else if (tmp == WAIT_OBJECT_0)
			{
				if(! GetOverlappedResult(m_com,&m_OverlappedWrite,&uWriteLength,FALSE))
				{
					return 0;
				}
				else
				{
					return uWriteLength;
				}
			}

			uWriteLength = 0;
		}
	}

	return uWriteLength;
}



