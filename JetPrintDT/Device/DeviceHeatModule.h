#pragma once

#include "DeviceADModulePort.h"
#include "../SingleTon/SingletonTemplate.h"

#define MODBUS_CONNECT		 1 //已连接
#define MODBUS_UNCONNECT	 0 //未连接

#define MODBUS_READSUC		 1 //读取成功
#define MODBUS_WRITESUC		 2 //写数据成功

#define MODBUS_READFAIL		 0 //读取失败
#define MODBUS_WRITEFAIL	 0 //写数失败

#define MODBUS_TIMEOUT		-1 //无数据返回
#define MODBUS_REQUSET  	-2 //指令发送错误
#define MODBUS_REQUSETFAIL	-3 //指令返回错误
#define MODBUS_PORTERR		-4 //串口未打开
#define MODBUS_CRCERR		-5 //crc校验失败

class CDeviceHeatModule :public CSingletonTemplate<CDeviceHeatModule>
{
	friend CSingletonTemplate<CDeviceHeatModule>;
public:
	CDeviceHeatModule(void);
	virtual ~CDeviceHeatModule(void);

public:
	BOOL Init(unsigned long xBabd = 19200,//波特率
			  unsigned char xDataSize = 8, //数据位
		      unsigned char xParity = 0, //校验位
		      unsigned char xStopBit = 0);//停止位
	BOOL UnInit();
	BOOL CheckConnect();
	BOOL WriteData(float voltage); //参数指向计算值
	unsigned short CalcCrcFast(unsigned char* puchMsg,unsigned short usDataLen);

	bool SetTemperature(double dbTemp);
	double GetTemperature();
	double GetCurrentDestTemp();
	bool StartHeat();
	bool StopHeat();
	bool IsConnected(){
		return m_bIsConnected;
	}
public:
	CDeviceADModulePort m_comm;
	bool m_bIsConnected;
	bool m_bIsHeatting;
	enum {HEAT_PORT = 9};
};

