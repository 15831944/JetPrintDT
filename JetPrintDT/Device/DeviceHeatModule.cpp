
#include "DeviceHeatModule.h"

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <math.h>

CDeviceHeatModule::CDeviceHeatModule(void)
{
	m_bIsConnected = false;
	m_bIsHeatting = false;
}


CDeviceHeatModule::~CDeviceHeatModule(void)
{
}

BOOL CDeviceHeatModule::Init(unsigned long xBabd /*= 9600*/,/*波特率 */ 
							unsigned char xDataSize /*= 8*/, /*数据位*/ 
							unsigned char xParity /*= 0*/,/*校验位*/ 
							unsigned char xStopBit /*= 0*/ )/*停止位*/ 
{
	//打开串口
	if(FALSE == m_comm.OpenPort(HEAT_PORT,xBabd,xDataSize,xParity,xStopBit,4096,4096,1000))
	{
		return FALSE;
	}
	//检测通信
	if(FALSE == CheckConnect())
	{
		return FALSE;
	}

	m_bIsConnected = true;
	return TRUE;
}

BOOL CDeviceHeatModule::UnInit()
{
	m_comm.ClosePort();
	return TRUE;
}


BOOL CDeviceHeatModule::WriteData(float voltage)
{
	unsigned short int crc16;
	unsigned short int crctmp;
	unsigned short int vol_ox;
	unsigned short int temp;
	
	unsigned char buff2[256];

	memset(buff2, 0x00, 255);
	buff2[0] = 0x01;
	buff2[1] = 0x06;
	buff2[2] = 0x00;
	buff2[3] = 0x0A;

	//计算电压值并转换成16进制数
	vol_ox = static_cast<unsigned short>(voltage * 4096 / 10);
	temp = vol_ox & 0xFF00;  //取低位数据
	temp = temp >> 8;    //取高位数据并右移8位
	buff2[4] = (unsigned char)temp;

	temp = vol_ox & 0x00FF;
	buff2[5] = (unsigned char)(temp);

	//计算校验位
	crc16 = CalcCrcFast(buff2,6);
	buff2[6] = (unsigned char)(crc16 >> 8);
	buff2[7] = (unsigned char)crc16;

	//发送数据
	unsigned long strlen;
	if(FALSE == m_comm.IsOpen())
	{
		return FALSE;
	}
	
	//发送数据
	strlen = m_comm.WriteChar(8,(char*)buff2);
	if(strlen != 8)
	{
		return FALSE;
	}
	
	//读数据
	memset(buff2,0x00,255);
	Sleep(50);

	strlen= m_comm.ReadChar(255, (char *)buff2, 1000);
	if(strlen == 0)
	{
		return FALSE;				
	}

	//返回长度有效,解析接收缓冲区
	crc16= CalcCrcFast(buff2, 6);
	crctmp= buff2[strlen-2];
	crctmp= crctmp << 8 | buff2[strlen-1];
	if(crc16 != crctmp )
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CDeviceHeatModule::CheckConnect()
{
	unsigned short int crc16;
	unsigned char buff[256];
	memset(buff, 0x00, 255);

	buff[0]= 0x01;
	buff[1]= 0x03;
	buff[2]= 0x00;
	buff[3]= 0x00;
	buff[4]= 0x00;
	buff[5]= 0x04;
	crc16= CalcCrcFast(buff, 6);
	buff[6] = (unsigned char)crc16;
	buff[7]= (unsigned char)(crc16 >> 8);


	
	//检查串口是否打开
	if(FALSE == m_comm.IsOpen())
	{
		return FALSE;
	}

	//发送数据
	if(8 != m_comm.WriteChar(8, (char *)buff))
	{
		return FALSE;
	}

	//读数据
	memset(buff, 0x00, 255);
	Sleep(100);

	unsigned long strlen = m_comm.ReadChar(255, (char *)buff,1000);
		
	//计算返回长度是否正确
	if(strlen != 13)
	{
		return FALSE;
	}
				
	return TRUE;
}


unsigned short CDeviceHeatModule::CalcCrcFast(unsigned char*puchMsg, unsigned short usDataLen)
{
	int j;
	unsigned int reg_crc = 0Xffff;
	while (usDataLen--)
	{
		reg_crc ^= *puchMsg++;
		for (j = 0; j < 8; j++)
		{
			if (reg_crc & 0x01) reg_crc = (reg_crc >> 1) ^ 0Xa001;
			else reg_crc = reg_crc >> 1;
		}
	}
	return reg_crc;// 返回CRC校验结果
}

bool CDeviceHeatModule::SetTemperature(double dbTemp){
	unsigned short int crc16;
	unsigned char buff[256];
	bool bResult = false;
	unsigned int data = (unsigned int)(dbTemp*10.0);
	char chHigh = data >> 8;
	char chLow = data & 0xff;

	memset(buff, 0x00, 255);

	for (char index = 0x0A; index < 0x0e; index++){
		buff[0] = 0x01;
		buff[1] = 0x06;
		buff[2] = 0x00;
		buff[3] = index;
		buff[4] = chHigh;
		buff[5] = chLow;
		crc16 = CalcCrcFast(buff, 6);
		buff[6] = (unsigned char)crc16;
		buff[7] = (unsigned char)(crc16 >> 8);

		if (FALSE == m_comm.IsOpen())
		{
			bResult = false;
			return bResult;
		}

		//发送数据
		if (8 != m_comm.WriteChar(8, (char *)buff))
		{
			bResult = false;
			return bResult;
		}

		memset(buff, 0x00, 255);
		Sleep(100);

		unsigned long strlen = m_comm.ReadChar(255, (char *)buff, 1000);
		if (strlen != 8){
			bResult = false;
			return bResult;
		}
	}
	
	return true;
}

double CDeviceHeatModule::GetTemperature(){
	unsigned short int crc16;
	unsigned char buff[256];
	double dbTemp[4] = { 0.0 };
	unsigned int data = 0;
	unsigned int temp = 0xff;
	memset(buff, 0x00, 255);

	buff[0] = 0x01;
	buff[1] = 0x03;
	buff[2] = 0x00;
	buff[3] = 0x00;
	buff[4] = 0x00;
	buff[5] = 0x04;
	crc16 = CalcCrcFast(buff, 6);
	buff[6] = (unsigned char)crc16;
	buff[7] = (unsigned char)(crc16 >> 8);



	//检查串口是否打开
	if (FALSE == m_comm.IsOpen())
	{
		return FALSE;
	}

	//发送数据
	if (8 != m_comm.WriteChar(8, (char *)buff))
	{
		return FALSE;
	}

	//读数据
	memset(buff, 0x00, 255);
	Sleep(100);

	unsigned long strlen = m_comm.ReadChar(255, (char *)buff, 1000);

	//计算返回长度是否正确
	if (strlen == 13)
	{
		for (int i = 3; i < 3 + 2 * 4; i += 2){
			data |= buff[i];
			data = data << 8;
			temp &= buff[i+1];
			data |= temp;

			dbTemp[(i - 3) / 2] = data / 10.0;

			data = 0;
			temp = 0xff;
		}
		
	}

	double averageTemp = 0.0;
	for (int i = 0; i < 4; i++)
		averageTemp += dbTemp[i];

	averageTemp /= 4.0;

	return averageTemp;
}

bool CDeviceHeatModule::StartHeat(){
	unsigned short int crc16;
	unsigned char buff[256];
	bool bResult = false;

	unsigned int data = 418;
	char chHigh = data >> 8;
	char chLow = data & 0xff;

	memset(buff, 0x00, 255);

	buff[0] = 0x01;
	buff[1] = 0x06;
	buff[2] = chHigh;
	buff[3] = chLow;
	buff[4] = 0x00;
	buff[5] = 0x0f;
	crc16 = CalcCrcFast(buff, 6);
	buff[6] = (unsigned char)crc16;
	buff[7] = (unsigned char)(crc16 >> 8);

	if (FALSE == m_comm.IsOpen())
	{
		bResult = false;
		return bResult;
	}

	//发送数据
	if (8 != m_comm.WriteChar(8, (char *)buff))
	{
		bResult = false;
		return bResult;
	}

	memset(buff, 0x00, 255);
	Sleep(100);

	unsigned long strlen = m_comm.ReadChar(255, (char *)buff, 1000);
	if (strlen != 8){
		bResult = false;
		return bResult;
	}

	data = 447;
	chHigh = data >> 8;
	chLow = data & 0xff;

	memset(buff, 0x00, 255);

	buff[0] = 0x01;
	buff[1] = 0x06;
	buff[2] = chHigh;
	buff[3] = chLow;
	buff[4] = 0x00;
	buff[5] = 0x0F;
	crc16 = CalcCrcFast(buff, 6);
	buff[6] = (unsigned char)crc16;
	buff[7] = (unsigned char)(crc16 >> 8);

	if (8 != m_comm.WriteChar(8, (char *)buff))
	{
		bResult = false;
		return bResult;
	}

	memset(buff, 0x00, 255);
	Sleep(100);

	strlen = m_comm.ReadChar(255, (char *)buff, 1000);
	if (strlen != 8){
		bResult = false;
		return bResult;
	}

	m_bIsHeatting = true;

	return true;
}


bool CDeviceHeatModule::StopHeat(){
	unsigned short int crc16;
	unsigned char buff[256];
	bool bResult = false;

	unsigned int data = 418;
	char chHigh = data >> 8;
	char chLow = data & 0xff;

	memset(buff, 0x00, 255);

	buff[0] = 0x01;
	buff[1] = 0x06;
	buff[2] = chHigh;
	buff[3] = chLow;
	buff[4] = 0x00;
	buff[5] = 0x0f;
	crc16 = CalcCrcFast(buff, 6);
	buff[6] = (unsigned char)crc16;
	buff[7] = (unsigned char)(crc16 >> 8);

	if (FALSE == m_comm.IsOpen())
	{
		bResult = false;
		return bResult;
	}

	//发送数据
	if (8 != m_comm.WriteChar(8, (char *)buff))
	{
		bResult = false;
		return bResult;
	}

	memset(buff, 0x00, 255);
	Sleep(100);

	unsigned long strlen = m_comm.ReadChar(255, (char *)buff, 1000);
	if (strlen != 8){
		bResult = false;
		return bResult;
	}

	data = 447;
	chHigh = data >> 8;
	chLow = data & 0xff;

	memset(buff, 0x00, 255);

	buff[0] = 0x01;
	buff[1] = 0x06;
	buff[2] = chHigh;
	buff[3] = chLow;
	buff[4] = 0x00;
	buff[5] = 0x00;
	crc16 = CalcCrcFast(buff, 6);
	buff[6] = (unsigned char)crc16;
	buff[7] = (unsigned char)(crc16 >> 8);

	if (8 != m_comm.WriteChar(8, (char *)buff))
	{
		bResult = false;
		return bResult;
	}

	memset(buff, 0x00, 255);
	Sleep(100);

	strlen = m_comm.ReadChar(255, (char *)buff, 1000);
	if (strlen != 8){
		bResult = false;
		return bResult;
	}

	m_bIsHeatting = false;
	return true;
}

double CDeviceHeatModule::GetCurrentDestTemp(){
	unsigned short int crc16;
	unsigned char buff[256];
	bool bResult = false;
	double dbTemp[4] = { 0.0 };
	unsigned int data = 0;
	unsigned int temp = 0xff;
	memset(buff, 0x00, 255);

	buff[0] = 0x01;
	buff[1] = 0x04;
	buff[2] = 0x00;
	buff[3] = 0x0A;
	buff[4] = 0x00;
	buff[5] = 0x04;
	crc16 = CalcCrcFast(buff, 6);
	buff[6] = (unsigned char)crc16;
	buff[7] = (unsigned char)(crc16 >> 8);

	if (FALSE == m_comm.IsOpen())
	{
		bResult = false;
		return bResult;
	}

	//发送数据
	if (8 != m_comm.WriteChar(8, (char *)buff))
	{
		bResult = false;
		return bResult;
	}

	memset(buff, 0x00, 255);
	Sleep(100);

	unsigned long strlen = m_comm.ReadChar(255, (char *)buff, 1000);


	//计算返回长度是否正确
	if (strlen == 13)
	{
		for (int i = 3; i < 3 + 2 * 4; i += 2){
			data |= buff[i];
			data = data << 8;
			temp &= buff[i + 1];
			data |= temp;

			dbTemp[(i - 3) / 2] = data / 10.0;

			data = 0;
			temp = 0xff;
		}

	}

	double averageTemp = 0.0;
	for (int i = 0; i < 4; i++)
		averageTemp += dbTemp[i];

	averageTemp /= 4.0;

	return averageTemp;
}