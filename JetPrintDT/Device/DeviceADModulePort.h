#pragma once
#include "afx.h"

class CDeviceADModulePort{
public:
	CDeviceADModulePort(void);
	~CDeviceADModulePort(void);

public:
	BOOL IsOpen();
	BOOL OpenPort(unsigned long xPort,          //���ں�
				  unsigned long xBabd,          //������
				  unsigned char xDataSize,      //����λ
				  unsigned char xParity,        //У��λ
				  unsigned char xStopBit,       //ֹͣλ
				  unsigned long InputBuffSize,  //����buffer��С
				  unsigned long OutputBuffSize, //���buffer��С
				  unsigned long dwTimerOut);    //�ӳ�ʱ��
	void ClosePort();
	unsigned long ReadChar(unsigned long dwBufferLength, char *buff, unsigned long dwWaitTime = 20);
	unsigned long WriteChar(unsigned long dwBufferLength, char *buff);
	void SetTimerOut(unsigned long dwTimerOut=5000);  
	BOOL SetDBCParam(unsigned long xBabd,
		             unsigned char xDataSize, 
		             unsigned char xParity, 
					 unsigned char xStopBit);
	BOOL SetPortBuffSize(unsigned long InputBuffSize, unsigned long OutputBuffSize);
	void ClearBuffer();
	unsigned long GetInBufferCount();
	unsigned long GetOutBufferCount();
private:
	DCB dcb;
	COMMTIMEOUTS CommTimerOuts;
	HANDLE m_com;
	OVERLAPPED m_overlappedRead;
	OVERLAPPED m_OverlappedWrite;
};

