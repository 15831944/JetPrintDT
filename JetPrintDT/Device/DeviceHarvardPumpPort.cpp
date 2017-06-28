#include "../stdafx.h"
#include "DeviceHarvardPumpPort.h"

/**
 *����:	���캯�����ڴ˳�ʼ������ͨ��ͬ������
 *����:	��
 *����:	��
**/
CDeviceFlowPort::CDeviceFlowPort(): m_SendOverlapEvent(FALSE, TRUE),	
				  m_RecvOverlapEvent(FALSE, TRUE),
				  m_WaitOverlapEvent(FALSE, TRUE)
{
	m_hComm				= INVALID_HANDLE_VALUE;
	m_bIsOpen			= FALSE;
	m_commType			= CommSync;
	m_nSendBufferSize	= 0;
	m_nRecvBufferSize	= 0;
}

/**
 *����:	����������û�йرմ��ڻ��ڴ˴��Զ��ر�
 *����:	��
 *����:	��
**/
CDeviceFlowPort::~CDeviceFlowPort()
{
	Close();
}

/************************************************************************/
/************************************************************************/
/* �򿪺͹ر�                                                                  */
/************************************************************************/
/************************************************************************/

/**
 *����:	�򿪶�Ӧ�Ĵ��ڲ����ö�Ӧ����
 *����:	nPort					--���ںţ�Ĭ��Ϊ1
		dwBaud					--�����ʣ�Ĭ��Ϊ9600
		stopbits				--ֹͣλ��Ĭ��Ϊ1λ
		commType				--ͨ�����ͣ�Ĭ��Ϊͬ��ͨ��
		parity					--��ż����λ��Ĭ������żУ��
		DataBits				--ͨ������λ��Ĭ��Ϊ8λ
		fc						--�����ƣ�Ĭ����������
		nSendBufferSize			--�������ݻ�������С��Ĭ��Ϊ1024
		nReceiveBufferSize		--�������ݻ�������С��Ĭ��Ϊ1024
 *����:	�򿪲����óɹ�ΪTRUE,����ΪFALSE
 *˵����һ����˵�����ͺͽ��ջ�������Ϊ1024���Ѿ��㹻�����Ԥ���ж�
		������һֵ��������Ϊ�����ֵ
**/
BOOL CDeviceFlowPort::Open( const int nPort				/*=1*/, 
				   const DWORD dwBaud			/*=9600*/,
				   const BYTE DataBits			/*=8*/,  
				   const StopBits stopbits		/*=OneStopBit*/, 
				   const CommType commType		/*=CommSync*/, 
				   const Parity parity			/*=NoParity*/, 
				   const FlowControl fc			/*=NoFlowControl*/, 
				   const int nSendBufferSize	/*=1024*/, 
				   const int nReceiveBufferSize /*=1024*/ )
{
	/************************************************************************/
	/* �򿪴���                                                                     */
	/************************************************************************/
	//У��˿ں�����
	ASSERT(nPort>0 && nPort<=255);

	//CreateFile����Ӧ����
	CString sPort;
	sPort.Format(_T("\\\\.\\COM%d"), nPort);
	m_hComm = CreateFile(sPort, 
						 GENERIC_READ | GENERIC_WRITE, 
						 0, 
						 NULL, 
						 OPEN_EXISTING, 
						 CommSync==commType ? 0 :FILE_FLAG_OVERLAPPED, 
						 NULL);
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return m_bIsOpen = FALSE;
	}

	/************************************************************************/
	/* ���ô��ڲ�����,У��λ,����λ,ֹͣλ��������                          */
	/************************************************************************/
	//���֮ǰ�Ĳ����б��Ա��޸�ָ���Ĳ�����������������
	DCB dcb;
	if (FALSE == GetState(dcb))
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;

		return m_bIsOpen = FALSE;
	}

	//������
	dcb.BaudRate = dwBaud; 

	//У��λ
	switch (parity)
	{
	case EvenParity:  dcb.Parity = EVENPARITY;  break;
	case MarkParity:  dcb.Parity = MARKPARITY;  break;
	case NoParity:    dcb.Parity = NOPARITY;    break;
	case OddParity:   dcb.Parity = ODDPARITY;   break;
	case SpaceParity: dcb.Parity = SPACEPARITY; break;
	default:          ASSERT(FALSE);            break;
	}

	//����λ
	dcb.ByteSize = DataBits;

	//ֹͣλ
	switch (stopbits)
	{
	case OneStopBit:           dcb.StopBits = ONESTOPBIT;   break;
	case OnePointFiveStopBits: dcb.StopBits = ONE5STOPBITS; break;
	case TwoStopBits:          dcb.StopBits = TWOSTOPBITS;  break;
	default:                   ASSERT(FALSE);               break;
	}

	//������
	dcb.fDsrSensitivity = FALSE;
	switch (fc)
	{
	case NoFlowControl:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
	case RtsCtsFlowControl:
		{
			dcb.fOutxCtsFlow = TRUE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
	case DtrDsrFlowControl:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = TRUE;
			dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
	case XonXoffFlowControl:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fOutX = TRUE;
			dcb.fInX = TRUE;
			dcb.XonChar = 0x11;
			dcb.XoffChar = 0x13;
			dcb.XoffLim = 100;
			dcb.XonLim = 100;
			break;
		}
	default:
		{
			ASSERT(FALSE);
			break;
		}
	}

	//����״̬
	if (FALSE == SetState(dcb))
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;

		return m_bIsOpen = FALSE;
	}

	/************************************************************************/
	/* ���÷������ݺͽ������ݻ�������С,����Ĭ�϶�д����������������        */
	/************************************************************************/
	if (FALSE == SetupComm(m_hComm, nReceiveBufferSize, nSendBufferSize)
		|| FALSE == Set0Timeout())
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;

		return m_bIsOpen = FALSE;
	}

	m_commType			= commType;
	m_nSendBufferSize	= nSendBufferSize;
	m_nRecvBufferSize	= nReceiveBufferSize;
	return m_bIsOpen	= TRUE;
}

/**
 *����:	�жϵ�ǰ�����Ƿ��Ѿ��򿪳ɹ�
 *����:	��
 *����:	��ΪTRUE��û�д�ΪFALSE
**/
BOOL CDeviceFlowPort::IsOpen()
{
	return m_bIsOpen;
}

/**
 *����:	�����ǰ�����Ѿ��򿪣��ر�
 *����:	��
 *����:	�����ѹر�ΪTRUE,����ΪFALSE
**/
BOOL CDeviceFlowPort::Close()
{
	if (IsOpen())
	{
		BOOL bSuccess = CloseHandle(m_hComm);
		
		if (TRUE == bSuccess)
		{
			m_hComm = INVALID_HANDLE_VALUE;
			m_commType	= CommSync;
			return !(m_bIsOpen = FALSE);
		}
		else
		{
			return !(m_bIsOpen = TRUE);
		}
	}

	return TRUE;
}

/************************************************************************/
/************************************************************************/
/* ���ͺͽ���                                                           */
/************************************************************************/
/************************************************************************/

/**
 *����:	����ָ��������,����ʹ�ַ������飬Ҳ�����Ǹ��ֽ�����(2/8/10/16)
 *����:	lpBuf			--����Ĵ��������ݻ�����,�ɱ����ַ�������
		dwCount			--�������ֽ���,�������д������ַ�(\n��\r��)
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
		ע�⣬�����첽ͨ��ʧ�ܿ����ǵ�ǰ���ڷ����У���Ҫ����GetLastErrorֵ
		�Ƿ�ΪERROR_IO_PENDING
 *˵�������Զ����ݵ�ǰ�򿪴���ʱָ����ͨ��������ȷ�Ϸ���ʱ��ͨ��ѡ��
**/
BOOL CDeviceFlowPort::Send(const void* lpBuf, const DWORD dwCount)
{
	ASSERT(IsOpen());

	DWORD dwBytesSend	= 0;		//�ɹ����͵��ֽ���
	BOOL bSuccess		= FALSE;

	switch(m_commType)
	{
	case CommSync:
		bSuccess = WriteFile(m_hComm, lpBuf, dwCount, &dwBytesSend, NULL);
		break;

	case CommAsync:
		//�����첽�����õ�ͬ���¼�����ʼ��Ϊ�����źš�״̬
		m_SendOverlapEvent.ResetEvent();
		ZeroMemory(&m_SendOverlap, sizeof(OVERLAPPED));
		m_SendOverlap.hEvent = m_SendOverlapEvent;

		//1.��������ɺ�ͬ���¼��ᱻ��Ϊ�����źš�״̬���ɵȴ������źš�״̬����֤�������
		//  �����WaitSendComplete�Ĺ���
		//2.������Ͳ���������ɣ���ʱ���ص�bSuccessΪFALSE��ʹ��GetLastError�õ�����
		//ERROR_IO_PENDING
		bSuccess = WriteFile(m_hComm, lpBuf, dwCount, &dwBytesSend, &m_SendOverlap);
		break;
	}

	return bSuccess;
}

/**
 *����:	����ָ����MFC�ַ�������
 *����:	strSend			--MFC Ctring�����ַ���
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
		ע�⣬�����첽ͨ��ʧ�ܿ����ǵ�ǰ���ڷ����У���Ҫ����GetLastErrorֵ
		�Ƿ�ΪERROR_IO_PENDING
 *˵�������Զ����ݵ�ǰ�򿪴���ʱָ����ͨ��������ȷ�Ϸ���ʱ��ͨ��ѡ��
**/
BOOL CDeviceFlowPort::Send( CString& strSend )
{
	CStringA strSendA;
	strSendA = strSend;//ע�������Unicode��Ascll��ת��

	return Send(strSendA.GetBuffer(0), strSendA.GetLength());
}

/**
 *����:	���Send����ʹ�ã��ȴ��첽�������
 *����:	��
 *����:	��
**/
void CDeviceFlowPort::WaitSendComplete()
{
	ASSERT(IsOpen());
	ASSERT(m_commType==CommAsync);

	WaitForSingleObject(m_SendOverlapEvent, INFINITE);
}

/**
 *����:	����ָ��������,����ʹ�ַ������飬Ҳ�����Ǹ��ֽ�����(2/8/10/16)
 *����:	lpBuf			--����Ĵ��������ݻ�����,�ɱ����ַ�������
		dwCount			--�������ֽ���,�������д������ַ�(\n��\r��)
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
		ע�⣬�����첽ͨ��ʧ�ܿ����ǵ�ǰ���ڽ����У���Ҫ����GetLastErrorֵ
		�Ƿ�ΪERROR_IO_PENDING
 *˵�������Զ����ݵ�ǰ�򿪴���ʱָ����ͨ��������ȷ�Ͻ���ʱ��ͨ��ѡ��
**/
BOOL CDeviceFlowPort::Recv(void* lpBuf, const DWORD dwCount)
{
	ASSERT(IsOpen());

	DWORD dwBytesRecv	= 0;		//�ɹ����յ��ֽ���
	BOOL bSuccess		= FALSE;

	switch(m_commType)
	{
	case CommSync:
		bSuccess = ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRecv, NULL);
		break;

	case CommAsync:
		//�����첽�����õ�ͬ���¼�����ʼ��Ϊ�����źš�״̬
		m_RecvOverlapEvent.ResetEvent();
		ZeroMemory(&m_RecvOverlap, sizeof(OVERLAPPED));
		m_RecvOverlap.hEvent = m_RecvOverlapEvent;

		//1.��������ɺ�ͬ���¼��ᱻ��Ϊ�����źš�״̬���ɵȴ������źš�״̬����֤�������
		//  �����WaitRecvComplete�Ĺ���
		//2.������ղ���������ɣ���ʱ���ص�bSuccessΪFALSE��ʹ��GetLastError�õ�����
		//ERROR_IO_PENDING
		bSuccess = ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRecv, &m_RecvOverlap);
		break;
	}

	return bSuccess;
}

/**
 *����:	����ָ��������,��䵽MFC�ַ�����
 *����:	strRecv			--MFC Ctring�����ַ���
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
		ע�⣬�����첽ͨ��ʧ�ܿ����ǵ�ǰ���ڽ����У���Ҫ����GetLastErrorֵ
		�Ƿ�ΪERROR_IO_PENDING
 *˵�������Զ����ݵ�ǰ�򿪴���ʱָ����ͨ��������ȷ�Ͻ���ʱ��ͨ��ѡ��
**/
BOOL CDeviceFlowPort::Recv( CString& strRecv )
{
	char* pRecvBuf = new char[m_nRecvBufferSize];
	ZeroMemory(pRecvBuf, m_nRecvBufferSize);

	if (FALSE == Recv(pRecvBuf, m_nRecvBufferSize))
	{
		return FALSE;
	}

	CStringA strRecvA;
	strRecvA = pRecvBuf;
	strRecv = strRecvA;//ASCLLת����Unicode���

	delete pRecvBuf;
	return TRUE;
}

/**
 *����:	���Recv����ʹ�ã��ȴ��첽�������
 *����:	��
 *����:	��
**/
void CDeviceFlowPort::WaitRecvComplete()
{
	ASSERT(IsOpen());
	ASSERT(m_commType==CommAsync);

	WaitForSingleObject(m_RecvOverlapEvent, INFINITE);
}

/**
 *����:	���Ϳ���ָ�ͬʱ��ȡ���ܵ��ķ���ֵ
 *����:	strSend			-- �����͵�MFC Ctring�����ַ���
		strRecv			-- ���ڽ��շ������ݵ�MFC Ctring�����ַ���
		nElapseTime		-- ���յ���һ���ַ��󣬵ȴ�һ��ʱ���Ա�������
						   Ĭ��Ϊ100ms
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
 *˵�������Զ����ݵ�ǰ�򿪴���ʱָ����ͨ��������ȷ�ϵ�ǰ��ͨ��ѡ��
**/
BOOL CDeviceFlowPort::GetResponse( CString& strSend, CString& strRecv, 
						  UINT nElapseTime/*=100*/)
{
	switch (m_commType)
	{
		//ͬ��
	case CommSync:
		//����
		if (!Send(strSend))
		{
			return FALSE;
		}

		//�ȴ���ȡ�ַ���Ϣ����
		SetEventToWait(EV_RXCHAR);
		StartWaitEvent();

		//��ȡ
		Sleep(nElapseTime);//�ȴ��������ݴ������
		if (!Recv(strRecv))
		{
			return FALSE;
		}
		break;

		//�첽
	case CommAsync:
		//����
		if (!Send(strSend) && ERROR_IO_PENDING!=GetLastError())
		{
			return FALSE;
		}
		
		//�ȴ���ȡ�ַ�
		SetEventToWait(EV_RXCHAR);
		StartWaitEvent();
		WaitEventArrive();

		//��ȡ
		Sleep(nElapseTime);//�ȴ��������ݴ������
		if (!Recv(strRecv) && ERROR_IO_PENDING!=GetLastError())
		{
			return FALSE;
		}
		WaitRecvComplete();
		break;
	}

	return TRUE;
}

/************************************************************************/
/************************************************************************/
/* �ȴ�ָ���¼�����                                                     */
/************************************************************************/
/************************************************************************/

/**
 *����:	���õ�ǰ�ȴ��������¼��������ַ������������ɵ�
 *����:	dwMask		--���룬���õ�ǰ�ȴ����¼����μ�MSDN SetCommMask
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
**/
BOOL CDeviceFlowPort::SetEventToWait(DWORD dwMask)
{
	ASSERT(m_hComm != INVALID_HANDLE_VALUE);

	return SetCommMask(m_hComm, dwMask);
}

/**
 *����:	��õ�ǰ�ȴ��������¼��������ַ������������ɵ�
 *����:	dwMask		--�����õ����룬�μ�MSDN GetCommMask
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
**/
BOOL CDeviceFlowPort::GetEventToWait(DWORD& dwMask)
{
	ASSERT(m_hComm != INVALID_HANDLE_VALUE);

	return GetCommMask(m_hComm, &dwMask);
}

/**
 *����:	�ȴ�SetEventToWait�����õ��¼�����
 *����:	��
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
 *˵����1.���������ʵ�е��ɬ������˵���þ���֪ͨ�ײ���ѯ��ǰ�¼�
	     �Ƿ񵽴һ������һ���¼���m_EventMask��Ϊ��Ӧ�¼����룬
		  m_WaitOverlap��Ϊ���ź�״̬��
	   2.����ͨ�����m_WaitOverlap�ź�״̬�ж��Ƿ����¼����
	     �����WaitEventArrive�����Ĺ�����
	   3.����ͨ�����m_EventMask�жϵ�ǰ�¼����ͣ�
	   4.ע����������Ƿ������ģ�ֻ֪ͨ�ײ���ѯ���������ȴ���
**/
BOOL CDeviceFlowPort::StartWaitEvent()
{
	ASSERT(IsOpen());

	BOOL bSuccess		= FALSE;

	if (CommSync == m_commType)
	{
		//ͬ��ͨ��ʱ�߳�����ֱ���ȴ��¼����
		bSuccess = WaitCommEvent(m_hComm, &m_EventMask, NULL);
	}
	else
	{
		m_WaitOverlapEvent.ResetEvent();//��ʼ���źţ�������ź�
		ZeroMemory(&m_WaitOverlap, sizeof(OVERLAPPED));
		m_WaitOverlap.hEvent = m_WaitOverlapEvent;

		//�첽ͨ��ʱ�������Ҫ�ȴ��¼���һ���¼������event���ź�
		bSuccess = WaitCommEvent(m_hComm, &m_EventMask, &m_WaitOverlap);
	}

	return bSuccess;	
}

/**
 *����:	�ȴ���Ӧ���¼���������SetEventToWait��StartWaitEvent���
 *����:	��
 *����:	��
**/
void CDeviceFlowPort::WaitEventArrive()
{
	ASSERT(IsOpen());
	ASSERT(m_commType==CommAsync);

	WaitForSingleObject(m_WaitOverlapEvent, INFINITE);
}

/************************************************************************/
/************************************************************************/
/* set��get                                                             */
/************************************************************************/
/************************************************************************/

/**
 *����:	���õ�ǰ���ڲ���
 *����:	dcb		--���ڲ����ṹ��
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
**/
BOOL CDeviceFlowPort::SetState(DCB& dcb)
{
	ASSERT(m_hComm != INVALID_HANDLE_VALUE);

	return SetCommState(m_hComm, &dcb);
}

/**
 *����:	��õ�ǰ���ڲ���
 *����:	dcb		--���ڲ����ṹ��
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
**/
BOOL CDeviceFlowPort::GetState(DCB& dcb)
{
	ASSERT(m_hComm != INVALID_HANDLE_VALUE);

	return GetCommState(m_hComm, &dcb);
}

/**
 *����:	���õ�ǰͨ�ų�ʱ
 *����:	timeouts		--��ʱ�ṹ��
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
**/
BOOL CDeviceFlowPort::SetTimeouts(COMMTIMEOUTS& timeouts)
{
	ASSERT(m_hComm != INVALID_HANDLE_VALUE);

	return SetCommTimeouts(m_hComm, &timeouts);
}

/**
 *����:	��ȡ��ǰͨ�ų�ʱ����
 *����:	timeouts		--��ʱ�ṹ��
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
**/
BOOL CDeviceFlowPort::GetTimeouts(COMMTIMEOUTS& timeouts)
{
	ASSERT(m_hComm != INVALID_HANDLE_VALUE);

	return GetCommTimeouts(m_hComm, &timeouts);
}

/**
 *����:	���ͺͽ��ն�������ִ��
 *����:	��
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
**/
BOOL CDeviceFlowPort::Set0Timeout()
{
	COMMTIMEOUTS Timeouts;
	ZeroMemory(&Timeouts, sizeof(COMMTIMEOUTS));

	Timeouts.ReadIntervalTimeout = MAXDWORD;
	Timeouts.ReadTotalTimeoutMultiplier = 0;
	Timeouts.ReadTotalTimeoutConstant = 0;
	Timeouts.WriteTotalTimeoutMultiplier = 0;
	Timeouts.WriteTotalTimeoutConstant = 0;

	return SetTimeouts(Timeouts);
}

/**
 *����:	��������ִ��
 *����:	��
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
**/
BOOL CDeviceFlowPort::Set0WriteTimeout()
{
	COMMTIMEOUTS Timeouts;

	GetTimeouts(Timeouts);
	Timeouts.WriteTotalTimeoutMultiplier = 0;
	Timeouts.WriteTotalTimeoutConstant = 0;

	return SetTimeouts(Timeouts);
}

/**
 *����:	��������ִ��
 *����:	��
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
**/
BOOL CDeviceFlowPort::Set0ReadTimeout()
{
	COMMTIMEOUTS Timeouts;

	GetTimeouts(Timeouts);
	Timeouts.ReadIntervalTimeout = MAXDWORD;
	Timeouts.ReadTotalTimeoutMultiplier = 0;
	Timeouts.ReadTotalTimeoutConstant = 0;

	return SetTimeouts(Timeouts);
}
