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
  bool        m_OpenFlag;     // 串口的连接状态标志
  HANDLE      m_hCom;	      // 串行口句柄
  OVERLAPPED  m_osRead;	      // 用于重叠读
  OVERLAPPED  m_osWrite;      // 用于重叠写
  int   m_nLightLuminence;    //从串口读取的数据
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

