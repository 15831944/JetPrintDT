#pragma once

#include "DeviceADModulePort.h"
#include "../SingleTon/SingletonTemplate.h"

#define MODBUS_CONNECT		 1 //������
#define MODBUS_UNCONNECT	 0 //δ����

#define MODBUS_READSUC		 1 //��ȡ�ɹ�
#define MODBUS_WRITESUC		 2 //д���ݳɹ�

#define MODBUS_READFAIL		 0 //��ȡʧ��
#define MODBUS_WRITEFAIL	 0 //д��ʧ��

#define MODBUS_TIMEOUT		-1 //�����ݷ���
#define MODBUS_REQUSET  	-2 //ָ��ʹ���
#define MODBUS_REQUSETFAIL	-3 //ָ��ش���
#define MODBUS_PORTERR		-4 //����δ��
#define MODBUS_CRCERR		-5 //crcУ��ʧ��

class CDeviceHeatModule :public CSingletonTemplate<CDeviceHeatModule>
{
	friend CSingletonTemplate<CDeviceHeatModule>;
public:
	CDeviceHeatModule(void);
	virtual ~CDeviceHeatModule(void);

public:
	BOOL Init(unsigned long xBabd = 19200,//������
			  unsigned char xDataSize = 8, //����λ
		      unsigned char xParity = 0, //У��λ
		      unsigned char xStopBit = 0);//ֹͣλ
	BOOL UnInit();
	BOOL CheckConnect();
	BOOL WriteData(float voltage); //����ָ�����ֵ
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

