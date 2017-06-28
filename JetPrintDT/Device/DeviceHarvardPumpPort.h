#ifndef CDeviceFlowPort_H
#define CDeviceFlowPort_H

#include <afxmt.h>

class CDeviceFlowPort
{
// ��������
public:
	enum FlowControl			//������
	{
		NoFlowControl,			//��������
		RtsCtsFlowControl,		//Rts/CtsӲ��������
		DtrDsrFlowControl,		//Dtr/DsrӲ��������
		XonXoffFlowControl		//Xon/Xoff���������
	};

	enum Parity					//��żУ��
	{    
		EvenParity,				//żУ��
		MarkParity,				//���
		NoParity,				//����żУ��
		OddParity,				//��У��
		SpaceParity				//�ո�
	};

	enum StopBits				//ֹͣλ
	{		
		OneStopBit,				//1
		OnePointFiveStopBits,	//1.5
		TwoStopBits				//2
	};

	enum
	{
		NoHandshaking = 0,
		XonXoff = 1,
		RtsCts = 2,
		XonXoffAndRtsCts = 3
	}HandshakeConstants;

	enum CommType				//ͨ������
	{	
		CommSync = 0,			//ͬ������
		CommAsync = 1,			//�첽����
	};

// ����
public:
	CDeviceFlowPort();
	~CDeviceFlowPort();
	BOOL Open(const int nPort				=1, 
		      const DWORD dwBaud			=9600, 
			  const BYTE DataBits			=6,
			  const StopBits stopbits		=OneStopBit,
			  const CommType commType		=CommSync,
			  const Parity parity			=NoParity,   
			  const FlowControl fc			=NoFlowControl, 
			  const int nSendBufferSize		=1024,
			  const int nReceiveBufferSize	=1024);
	BOOL IsOpen();
	BOOL Close();

	BOOL Send(const void* lpBuf, const DWORD dwCount);
	BOOL Recv(void* lpBuf, const DWORD dwCount);
	BOOL Send(CString& strSend);
	BOOL Recv(CString& strRecv);
	void WaitSendComplete();
	void WaitRecvComplete();
	BOOL GetResponse(CString& strSend, CString& strRecv, UINT nElapseTime=100);

	BOOL SetEventToWait(DWORD dwMask);
	BOOL GetEventToWait(DWORD& dwMask);
	BOOL StartWaitEvent();
	void WaitEventArrive();

	BOOL SetState(DCB& dcb);
	BOOL GetState(DCB& dcb);
	BOOL SetTimeouts(COMMTIMEOUTS& timeouts);
	BOOL GetTimeouts(COMMTIMEOUTS& timeouts);
	BOOL Set0Timeout();
	BOOL Set0WriteTimeout();
	BOOL Set0ReadTimeout();

protected:
	HANDLE		m_hComm;			//���ھ��
	BOOL		m_bIsOpen;			//�����Ƿ��
	CommType	m_commType;			//��ǰͨѶ����
	int			m_nSendBufferSize;
	int			m_nRecvBufferSize;

	OVERLAPPED	m_SendOverlap;		//�����첽�ṹ��
	CEvent		m_SendOverlapEvent;	//�����첽�ṹ���¼�
	OVERLAPPED	m_RecvOverlap;		//�����첽�ṹ��
	CEvent		m_RecvOverlapEvent;	//�����첽�ṹ���¼�
	OVERLAPPED	m_WaitOverlap;		//�ȴ��첽�ṹ��
	CEvent		m_WaitOverlapEvent;	//�ȴ��첽�ṹ���¼�
	DWORD		m_EventMask;		//�ȴ��¼�Mask
};

#endif