#include "../stdafx.h"
#include "DeviceHarvardPump.h"

CDeviceHarvardPump::CDeviceHarvardPump(void)
{
	m_nInfuseRate = 0;
}

CDeviceHarvardPump::~CDeviceHarvardPump(void)
{
}

/**
 *����:	��ʼ��������
 *����:	nPort		--������ռ�õĴ��ں�
 *����:	�ɹ�ΪTRUE��ʧ��ΪFALSE
**/
BOOL CDeviceHarvardPump::Init(const int nPort)
{
	//�򿪺ͳ�ʼ������
	if (FALSE == m_comm.Open(nPort, 115200, CDeviceFlowPort::TwoStopBits))
	{
		return FALSE;
	}

	//����PC�����������ӣ���������ʱ����\r->����:
	m_strSend = TEXT("\r");
	m_comm.GetResponse(m_strSend, m_strReceive);
	if (m_strReceive.GetAt(1) == TEXT(':'))//ʵ�ʷ�������Ϊ"\n:"
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 *����:	�����ÿ�ʼ�Զ�ע��
 *����:	��
 *����:	ʼ��ΪTRUE������ʵ��
**/
BOOL CDeviceHarvardPump::Run()
{
	m_strSend = TEXT("irun\r");
	m_comm.GetResponse(m_strSend, m_strReceive);

	return TRUE;
}

/**
 *����:	�����ÿ�ʼ�����Զ�ע��
 *����:	��
 *����:	ʼ��ΪTRUE������ʵ��
**/
BOOL CDeviceHarvardPump::RunBack()
{
	m_strSend = TEXT("wrun\r");
	m_comm.GetResponse(m_strSend, m_strReceive);

	return TRUE;
}

/**
 *����:	������ֹͣ�Զ�ע��
 *����:	��
 *����:	ʼ��ΪTRUE������ʵ��
**/
BOOL CDeviceHarvardPump::Stop()
{
	m_strSend = TEXT("stop\r");
	m_comm.GetResponse(m_strSend, m_strReceive);

	return TRUE;
}

/**
 *����:	�������л��Զ�ע��״̬�����ֶ���������ÿ�������ϵĿ�ʼ/ֹͣ��ťЧ��һ��
 *����:	��
 *����:	ʼ��ΪTRUE������ʵ��
**/
BOOL CDeviceHarvardPump::SwitchRunAndStop()
{
	m_strSend = TEXT("run\r");
	m_comm.GetResponse(m_strSend, m_strReceive);

	return TRUE;
}

/**
 *����:	���õ�ǰע������
 *����:	nRate		--������nl/min
 *����:	ʼ��ΪTRUE������ʵ��
**/
BOOL CDeviceHarvardPump::SetInfuseRate( const int nRate )
{
	m_strSend.Format(TEXT("irate %d nl/min\r"), nRate);
	m_comm.GetResponse(m_strSend, m_strReceive);

	m_nInfuseRate = nRate;
	return TRUE;
}

/**
 *����:	���õ�ǰ����ע������
 *����:	nRate		--������nl/min
 *����:	ʼ��ΪTRUE������ʵ��
 **/
BOOL CDeviceHarvardPump::SetWithdrawRate( const int nRate )
{
	m_strSend.Format(TEXT("wrate %d nl/min\r"), nRate);
	m_comm.GetResponse(m_strSend, m_strReceive);

	return TRUE;
}

/**
 *����:	���õ�ǰע������Ϊ���
 *����:	��
 *����:	ʼ��ΪTRUE������ʵ��
**/
BOOL CDeviceHarvardPump::SetMaxInfuseRate()
{
	m_strSend = TEXT("irate max\r");
	m_comm.GetResponse(m_strSend, m_strReceive);

	return TRUE;
}

/**
 *����:	���õ�ǰ����ע������Ϊ���
 *����:	��
 *����:	ʼ��ΪTRUE������ʵ��
**/
BOOL CDeviceHarvardPump::SetMaxWithdrawRate()
{
	m_strSend = TEXT("wrate max\r");
	m_comm.GetResponse(m_strSend, m_strReceive);

	return TRUE;
}

/**
 *����:	���õ�ǰע������Ϊ��С
 *����:	��
 *����:	ʼ��ΪTRUE������ʵ��
**/
BOOL CDeviceHarvardPump::SetMinInfuseRate()
{
	m_strSend = TEXT("irate min\r");
	m_comm.GetResponse(m_strSend, m_strReceive);

	return TRUE;
}

/**
 *����:	���õ�ǰ����ע������Ϊ��С
 *����:	��
 *����:	ʼ��ΪTRUE������ʵ��
**/
BOOL CDeviceHarvardPump::SetMinWithdrawRate()
{
	m_strSend = TEXT("wrate min\r");
	m_comm.GetResponse(m_strSend, m_strReceive);

	return TRUE;
}

/**
 *����:	���ע��������ע������
 *����:	��
 *����:	����ע��������nl/min
**/
int CDeviceHarvardPump::GetInfuseRate()
{
	int iRate=0;

	m_strSend = TEXT("irate\r");
	m_comm.GetResponse(m_strSend, m_strReceive);

	iRate = _wtoi(m_strReceive);
	return iRate;
}

double CDeviceHarvardPump::GetSyringeVolume()
{
	double dbVolume = 0;
	m_strSend = TEXT("svolume\r");
	m_comm.GetResponse(m_strSend,m_strReceive);

	dbVolume = _wtof(m_strReceive);

	return dbVolume;

}

/**
 *����:	���ע��������ע������
 *����:	��
 *����:	����ע��������nl/min
**/
int CDeviceHarvardPump::GetWithdrawRate()
{
	int wRate=0;

	m_strSend = TEXT("wrate\r");
	m_comm.GetResponse(m_strSend, m_strReceive);

	swscanf_s(m_strReceive, TEXT("\n%d "), &wRate);//safe function
	//_stscanf(m_strReceive, TEXT("\n%d "), &wRate);
	return wRate;
}

/**
 *����:	����ע����ֱ��
 *����:	dbDiameter		--ֱ����mm,Ĭ��Ϊ4.608mm
 *����:	ʼ��ΪTRUE������ʵ��
 *˵���� ע����������ע����ֱ���������������0����Ҫ��������
**/
BOOL CDeviceHarvardPump::SetSyringeDiameter( const double dbDiameter/*=4.608*/ )
{
	m_strSend.Format(TEXT("diameter %f mm\r"), dbDiameter);
	m_comm.GetResponse(m_strSend, m_strReceive);

	return TRUE;
}

BOOL CDeviceHarvardPump::SetSyringeVolume(double dbVolume)
{
	m_strSend.Format(TEXT("svolume %f ul\r"), dbVolume);
	m_comm.GetResponse(m_strSend, m_strReceive);

	return TRUE;
}

void CDeviceHarvardPump::CloseHarvardPump()
{
	m_comm.Close();
}