#pragma once
#include "../SingleTon/SingletonTemplate.h"

class CDeviceWatchLightPort : public CSingletonTemplate<CDeviceWatchLightPort>
{

	friend CSingletonTemplate<CDeviceWatchLightPort>;
private:
	CDeviceWatchLightPort();
	~CDeviceWatchLightPort();

private:
  enum{ WATCH_LIGHT_PORT = 5 };
  DCB         dcb;
  bool        m_OpenFlag;     // ���ڵ�����״̬��־
  HANDLE      m_hCom;	      // ���пھ��
  OVERLAPPED  m_osRead;	      // �����ص���
  OVERLAPPED  m_osWrite;      // �����ص�д
  int   m_nLightLuminence;    //�Ӵ��ڶ�ȡ������
  int   m_nPortChannel;

public:
  bool  InitLightPort();
  void  ClosePort();
  bool  OpenPort(CString comPortName);

  DWORD WriteSerial(BYTE * chBuf, DWORD dwLength);
  DWORD ReadSerial(BYTE* chBuf, DWORD dwLength);
  bool  SendData(BYTE byAddress, BYTE byCommand, BYTE byValue, BYTE byTrigger);
  DWORD GetBufferLength();
  int   GetLightLuminence();
  int   GetPortNumber(CString portName);
  inline bool IsPortOpen() const
  {
    return m_OpenFlag;
  }

  void SetLightLuminence(int light){
	  m_nLightLuminence = light;
  }
};

