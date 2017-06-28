#include "../stdafx.h"
#include "DeviceParker.h"
#include "afxwin.h"

CDeviceParker::CDeviceParker():m_pWnd(NULL)
{
	m_bIsParkerConnected = false;
	m_bIsMotorKilled = false;
	m_bWatchStatusIntial = false;
	m_strIpAddr = _T("");
	m_watchStatusThread = NULL;
	m_bIsParkerInitialized = false;
	statusMsg = 0;

	m_lAxisXPosition = 0;
	m_fAxisXSpeed = 0.0;
	m_lAxisYPosition = 0;
	m_fAxisYSpeed = 0.0;
	status_data.m_fXPos = 0.0;
	status_data.m_fXSpeed = 0.0;
	status_data.m_fYPos = 0.0;
	status_data.m_fYSpeed = 0.0;

	m_bIsMotorEnable = false;

	m_ThreadParam.m_bIsExitThread = false;
	m_ThreadParam.pJetParkerControl = NULL;

	m_bXJogPlus = false;
	m_bXJogMinus = false;
	m_bYJogPlus = false;
	m_bYjogMinus = false;

	m_bSpeedParamUpdated = false;

	m_bProgWrite = true;
	m_bIsJogMode = false;

	m_bSuspendThread = false;
	m_WatchStatusEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL);

	JetParkerInitialize();
	JetParkerConnect();
}


CDeviceParker::~CDeviceParker(void)
{
	//释放AlarmEvent事件
	//结束监视的线程
	m_ThreadParam.m_bIsExitThread = true;

	if(NULL != m_watchStatusThread)
	{
		if(NULL != m_watchStatusThread->m_hThread)
		{
			CloseHandle(m_watchStatusThread->m_hThread);

			m_watchStatusThread->m_hThread = NULL;
			delete m_watchStatusThread;
			m_watchStatusThread = NULL;
		}
	}

	CloseHandle(m_WatchStatusEvent);

	JetParkerDisConnect();
	JetParkerUnInitialize();
}

CComQIPtr<IChannel> CDeviceParker::GetControlChannel()const
{
	return m_controlChannel;
}

bool CDeviceParker::JetParkerInitialize()
{
	if(!m_bIsParkerInitialized)
	{
		CoInitializeEx(NULL,COINIT_MULTITHREADED);

		m_controlChannel.CoCreateInstance(L_COMACRSERVER_PROGID);
		if(m_controlChannel)
		{
			m_bIsParkerInitialized = true;
		}
		else
		{
			m_bIsParkerInitialized = false;
		}
	}
	


	return m_bIsParkerInitialized;
}

bool CDeviceParker::JetParkerIsInitialized()
{
	return m_bIsParkerInitialized;
}

bool CDeviceParker::JetParkerConnect()
{
	if(!m_bIsParkerConnected)
	{
		try
		{
			m_strIpAddr.Format(_T("192.168.10.40"));
			BSTR bstrIpAddr = ::SysAllocString(m_strIpAddr);
			if(NULL == bstrIpAddr)
			{
				m_bIsParkerConnected = false;
				return m_bIsParkerConnected;
			}

			m_controlChannel->put_bstrIP(bstrIpAddr);

			//用完之后要释放
			::SysFreeString(bstrIpAddr);

			m_controlChannel->put_bOnConnectTest(true);

			m_controlChannel->Connect(ETHERNET,0);
			m_bIsParkerConnected = m_controlChannel->TestConnect()?true:false;
		}
		catch (_com_error error)
		{
			return true;
		}
		
	}

	return m_bIsParkerConnected;
}

bool CDeviceParker::JetParkerDisConnect()
{
	try
	{
		if (nullptr == m_controlChannel)
			return false;

		if(m_controlChannel->TestConnect())
		{
			//监测电机是否处于使能状态
			m_controlChannel->Disconnect();
			m_bIsParkerConnected = m_controlChannel->TestConnect()?true:false;

			if(m_bIsParkerConnected)
			{
				AfxMessageBox(_T("未能断开连接！"));
				return false;
			}

			m_bWatchStatusIntial = false;
		}
	}
	catch(_com_error error)
	{

	}
	

	return !m_bIsParkerConnected;
}

void CDeviceParker::JetParkerKillMotor()
{
	if(m_bIsParkerConnected)
	{
		m_controlChannel->SetFlag(KILL_MOTOR_ZERO,m_bIsParkerConnected,m_bIsParkerConnected);
		m_controlChannel->SetFlag(KILL_MOTOR_ONE,m_bIsParkerConnected,m_bIsParkerConnected);

		m_bIsMotorKilled = true;
	}
}

void CDeviceParker::JetParkerEnableMotor()
{
	if(m_bIsParkerConnected)
	{
		//如果Kill All Motions
		if(JetParkerIsBitChecked(4360,19))
		{
			BOOL bSet = FALSE;
			m_controlChannel->SetFlag(KILL_MOTOR_ZERO,bSet,!bSet);
		}

		if(JetParkerIsBitChecked(4361,19))
		{
			BOOL bSet = FALSE;
			m_controlChannel->SetFlag(KILL_MOTOR_ONE,bSet,!bSet);
		}

		m_controlChannel->SetFlag(ENABLE_MOTOR_ZERO,m_bIsParkerConnected,m_bIsParkerConnected);
		m_controlChannel->SetFlag(ENABLE_MOTOR_ONE,m_bIsParkerConnected,m_bIsParkerConnected);

		m_bIsMotorEnable = true;
	}
}


void CDeviceParker::JetParkerDisableMotor()
{
	if(m_bIsParkerConnected)
	{
		//如果Kill All Motions
		m_controlChannel->SetFlag(ENABLE_MOTOR_ZERO,!m_bIsParkerConnected,m_bIsParkerConnected);
		m_controlChannel->SetFlag(ENABLE_MOTOR_ONE,!m_bIsParkerConnected,m_bIsParkerConnected);

		m_bIsMotorEnable = false;
	}
}

void CDeviceParker::JetParkerJogVelocitySet(const UINT &n_motor,const float& fSpeed)
{
	if(m_bIsParkerConnected)
	{
		switch(n_motor)
		{
		case 0:
			m_controlChannel->SetParmFloat(JOG_SPEED_ZERO_SET,fSpeed,m_bIsParkerConnected);
			break;

		case 1:
			m_controlChannel->SetParmFloat(JOG_SPEED_ONE_SET,fSpeed,m_bIsParkerConnected);
			break;
		}
	}

}

float CDeviceParker::JetParkerJogVelocityGet(const UINT &n_motor)
{

	float f_speed = 0.0;
	if(m_bIsParkerConnected)
	{
		switch(n_motor)
		{
		case 0:
			//m_controlChannel->
			f_speed = m_fAxisXSpeed;
			break;

		case 1:
			f_speed = m_fAxisYSpeed;
			break;
		}
	}

	return f_speed;
}

float CDeviceParker::JetParkerPositionGet(const UINT &motor)
{
	float fPosition = 0.0;
	if(m_bIsParkerConnected)
	{
		switch(motor)
		{
		case 0:
			fPosition = status_data.m_fXPos;
			break;

		case 1:
			fPosition = status_data.m_fYPos;
			break;
		}
	}

	return fPosition;
}

float CDeviceParker::JetParkerSpeedGet(const UINT &motor)
{
	float fSpeed = 0.0;
	if(m_bIsParkerConnected)
	{
		switch(motor)
		{
		case 0:
			fSpeed = status_data.m_fXSpeed;
			break;

		case 1:
			fSpeed = status_data.m_fYSpeed;
			break;
		}
	}

	return fSpeed;
}

float CDeviceParker::JetParkerMasterSpeedGet()
{
	float fSpeed = 0.0;
	SAFEARRAY* pSA;
	long lBound = 0; 
	CComVariant vItem;  // The CComVariant is found in the atlbase.h include file
	CComVariant status; // it will clean up SAFEARRAY when it goes out of scope.

	CString varText = _T("P8223");
	BSTR bstrText = ::SysAllocString(varText);
	status = m_controlChannel->GetACRCustom(bstrText);
	::SysFreeString(bstrText);

	if(status.vt & VT_ARRAY)
	{
		pSA = status.parray;
		SafeArrayGetElement(pSA, &lBound, &vItem);
		if(vItem.vt!=VT_EMPTY) 
			fSpeed = vItem.fltVal;
		else
			fSpeed = 0.0f;
	}


	return fSpeed;
}

void CDeviceParker::JetParkerJogAccSet(const UINT &n_motor,const float& fSpeed)
{
	if(m_bIsParkerConnected)
	{
		switch(n_motor)
		{
		case 0:
			m_controlChannel->SetParmFloat(JOG_ACC_ZERO_SET,fSpeed,m_bIsParkerConnected);
			break;

		case 1:
			m_controlChannel->SetParmFloat(JOG_ACC_ONE_SET,fSpeed,m_bIsParkerConnected);
			break;
		}
	}
}

void CDeviceParker::JetParkerJogAccGet()
{

}

void CDeviceParker::JetParkerXJogPlus()
{
	if(m_bIsParkerConnected)
	{
		m_controlChannel->SetFlag(JOG_X_PLUS,m_bIsParkerConnected,m_bIsParkerConnected);
		m_bXJogPlus = true;
	}
}

void CDeviceParker::JetParkerYJogPlus()
{
	if(m_bIsParkerConnected)
	{
		m_controlChannel->SetFlag(JOG_Y_PLUS,m_bIsParkerConnected,m_bIsParkerConnected);
		m_bYJogPlus = true;
	}
}

void CDeviceParker::JetParkerXJogMinus()
{
	if(m_bIsParkerConnected)
	{
		m_controlChannel->SetFlag(JOG_X_MINUS,m_bIsParkerConnected,m_bIsParkerConnected);
		m_bXJogMinus = true;
	}
}

void CDeviceParker::JetParkerYJogMinum()
{
	if(m_bIsParkerConnected)
	{
		m_controlChannel->SetFlag(JOG_Y_MINUS,m_bIsParkerConnected,m_bIsParkerConnected);
		m_bYjogMinus = true;
	}
}

void CDeviceParker::JetParkerJogHome()
{
	if(m_bIsParkerConnected)
	{
		CString homeText = _T("");
		homeText.Format(_T("JOG ABS X0 Y0 \r"));
		BSTR bstrHomeText = ::SysAllocString(homeText);

		m_controlChannel->Write(bstrHomeText);

		::SysFreeString(bstrHomeText);
	}
}

void CDeviceParker::JetParkerJogStop(const UINT & n_motor)
{
	if(m_bIsParkerConnected)
	{
		switch(n_motor)
		{
		case 0:
			m_controlChannel->SetFlag(JOG_X_STOP,m_bIsParkerConnected,m_bIsParkerConnected);
			m_bXJogMinus = false;
			m_bXJogPlus = false;
			break;

		case 1:
			m_controlChannel->SetFlag(JOG_Y_STOP,m_bIsParkerConnected,m_bIsParkerConnected);
			m_bYjogMinus = false;
			m_bYJogPlus = false;
			break;

		case 2:
			m_controlChannel->SetFlag(JOG_X_STOP,m_bIsParkerConnected,m_bIsParkerConnected);
			m_controlChannel->SetFlag(JOG_Y_STOP,m_bIsParkerConnected,m_bIsParkerConnected);
			m_bXJogMinus = false;
			m_bXJogPlus = false;
			m_bYjogMinus = false;
			m_bYJogPlus = false;
			break;
		}
	}

}

//Move Control
void CDeviceParker::JetParkerSetLimit(float plusPos,float minumPos)
{
	//JOG限位和移动限位一样
	if(m_bIsParkerConnected)
	{
		m_controlChannel->SetParmFloat(JOG_X_LIMIT_PLUS,plusPos,m_bIsParkerConnected);
		m_controlChannel->SetParmFloat(JOG_X_LIMIT_MINUM,minumPos,m_bIsParkerConnected);

		m_controlChannel->SetParmFloat(JOG_Y_LIMIT_PLUS,plusPos,m_bIsParkerConnected);
		m_controlChannel->SetParmFloat(JOG_Y_LIMIT_MINUS,minumPos,m_bIsParkerConnected);

		m_controlChannel->SetParmFloat(MOVE_X_LIMIT_PLUS,plusPos,m_bIsParkerConnected);
		m_controlChannel->SetParmFloat(MOVE_X_LIMIT_MINUS,minumPos,m_bIsParkerConnected);

		m_controlChannel->SetParmFloat(MOVE_Y_LIMIT_PLUS,plusPos,m_bIsParkerConnected);
		m_controlChannel->SetParmFloat(MOVE_Y_LIMIT_MINUS,plusPos,m_bIsParkerConnected);
	}
}

void CDeviceParker::JetParkerSuspendThread()
{
	if(NULL != m_watchStatusThread)
	{
		m_bSuspendThread = true;
	}

}

void CDeviceParker::JetParkerResumeThread()
{
	if(NULL != m_watchStatusThread)
	{
		m_bSuspendThread = false;
		SetEvent(m_WatchStatusEvent);
	}
}

bool CDeviceParker::JetParkerWatchStatusInit()
{
	//如果监视线程尚未开始
	if(!m_bWatchStatusIntial)
	{
		m_ThreadParam.pJetParkerControl = this;
		m_ThreadParam.m_bIsExitThread = false;

		m_watchStatusThread = AfxBeginThread(StatusInit,&m_ThreadParam,THREAD_PRIORITY_ABOVE_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_watchStatusThread)
		{
			AfxMessageBox(_T("启动监视失败！"));
			m_bWatchStatusIntial = false;

			return m_bWatchStatusIntial;
		}
		m_watchStatusThread->m_bAutoDelete = false;
		m_watchStatusThread->ResumeThread();

		SetEvent(m_WatchStatusEvent);
		m_bWatchStatusIntial = true;
	}

	return m_bWatchStatusIntial;
}

UINT CDeviceParker::StatusInit(void *param)
{
	THREAD_PARAM *pThreadParam = (THREAD_PARAM *)param;

	CComVariant status; // it will clean up SAFEARRAY when it goes out of scope.
	
	CComPtr<IUnknown> unknownChannel;
	CComQIPtr<IStatus> statusChannel;
	CoInitializeEx(NULL,COINIT_MULTITHREADED);

	statusChannel.CoCreateInstance(L_COMACRSERVER_PROGID);
	if(NULL == statusChannel)
	{
		//Do Something
	}
	CString strIp = _T("192.168.10.40");
	BSTR bstrStrIp = ::SysAllocString(strIp);
	statusChannel->put_bstrIP(bstrStrIp);
	::SysFreeString(bstrStrIp);

	statusChannel->Connect(pThreadParam->pJetParkerControl->ETHERNET,0);
	if(statusChannel->GetisOffline())
	{
		pThreadParam->m_bIsExitThread = true;
	}

	CString statusString = _T("");

	//AXIS0 Speed,AXIS1 Speed, AXIS0 Position, AXIS1 Position
	statusString.Format(_T("P12315,P12571,P12290,P12546"));
	BSTR bstrStatusString = ::SysAllocString(statusString);
	if(NULL == bstrStatusString)
	{
		return 0;
	}

	long msgID = statusChannel->AddACRCustom(bstrStatusString);
	::SysFreeString(bstrStatusString);

	while(!pThreadParam->m_bIsExitThread)
	{
		WaitForSingleObject(pThreadParam->pJetParkerControl->m_WatchStatusEvent,INFINITE);
		ResetEvent(pThreadParam->pJetParkerControl->m_WatchStatusEvent);

		while(true)
		{
			if(true == pThreadParam->m_bIsExitThread)
			{
				break;
			}

			if(true == pThreadParam->pJetParkerControl->m_bSuspendThread)
			{
				break;
			}
			Sleep(7); // Give Com some time to process, better data than if run with no Sleep()
			BSTR bstrStatusString = ::SysAllocString(statusString);
			try{status = statusChannel->GetACRCustom(bstrStatusString);}
			catch(_com_error)
			{

			};
			::SysFreeString(bstrStatusString);
			pThreadParam->pJetParkerControl->JetParkerUpdateState(status);
		}
	};

	//释放所要监测的变量
	if(statusChannel)
	{
		try
		{
			statusChannel->DelStatus(-1);
		}
		catch(_com_error *e )
		{
			e->Description();
		}

		statusChannel->Disconnect();
		//statusChannel.Release();
	}

	CoUninitialize();
	return 0;
}

void CDeviceParker::JetParkerUpdateState(CComVariant & varItem)
{
	SAFEARRAY* pStatusArray;
	long lBound = 0; 
	CComVariant vItem; 

	if(varItem.vt&VT_ARRAY)
	{
		pStatusArray = varItem.parray;
		SafeArrayGetElement(pStatusArray,&lBound,&vItem);
		//The Orders should be AXIS0 Speed,AXIS1 Speed, AXIS0 Position, AXIS1 Position
		//AXIS0 Speed
		if(vItem.vt != VT_EMPTY)
		{
			m_fAxisXSpeed = vItem.fltVal;
			++lBound;
		}

		//AXIS1 Speed
		SafeArrayGetElement(pStatusArray,&lBound,&vItem);
		if(vItem.vt != VT_EMPTY)
		{
			m_fAxisYSpeed = vItem.fltVal;
			++lBound;
		}

		//AXIS0 Position
		SafeArrayGetElement(pStatusArray,&lBound,&vItem);
		if(vItem.vt != VT_EMPTY)
		{
			m_lAxisXPosition = vItem.lVal;
			++lBound;
		}

		//AXIS1 Position
		SafeArrayGetElement(pStatusArray,&lBound,&vItem);
		if(vItem.vt != VT_EMPTY)
		{
			m_lAxisYPosition = vItem.lVal;
		}

		status_data.m_fXPos = (float)((m_lAxisXPosition*1.0)/(2000.0));
		status_data.m_fXSpeed = m_fAxisXSpeed;
		status_data.m_fYPos = (float)((m_lAxisYPosition*1.0)/(2000.0));
		status_data.m_fYSpeed = m_fAxisYSpeed;

		SendMessage(m_pWnd->m_hWnd,WM_STATUS_MESSAGE,(WPARAM)(&status_data),NULL);
	}
}

void CDeviceParker::JetParkerMove(float *pTargetPosition,bool bMoveAbsolute,int nAxis)
{
	//设置为增量运动的方式
	BOOL bMoveMode = bMoveAbsolute?TRUE:FALSE;
	m_controlChannel->put_bMoveAbsolute(bMoveMode);
	m_controlChannel->put_fMoveVEL(10.0);

	int nCountTarget = 0;
	if(nAxis == 0)
		nCountTarget = 1;
	if(nAxis == 1)
		nCountTarget = 1;
	if(nAxis == 2)
		nCountTarget = 2;

	VARIANT symbol;
	SAFEARRAY* pSAz;
	SAFEARRAYBOUND bound[1];
	bound[0].lLbound=0;
	bound[0].cElements=nCountTarget;
	pSAz = SafeArrayCreate(VT_VARIANT,1,bound);
	long itemNbr = 0;

	CComVariant *vItem;
	SafeArrayAccessData(pSAz,(void**)&vItem);
	for(int i=0;i<nCountTarget;i++)
	{
		vItem[i] = pTargetPosition[i];
	}

	SafeArrayUnaccessData(pSAz);

	VariantInit(&symbol);
	symbol.vt = VT_ARRAY | VT_VARIANT;
	symbol.parray = pSAz;

	//X Move
	if(nAxis == 0)
		nCountTarget = 1;
	if(nAxis == 1)
		nCountTarget = 2;
	if(nAxis == 2)
		nCountTarget = 3;

	m_controlChannel->Move(nCountTarget,symbol);

	SafeArrayDestroy(pSAz);
}

CString CDeviceParker::JetParkerGetIPAddr()const 
{
	return m_strIpAddr;
}

void CDeviceParker::JetParkerSetAglient(bool bStart)
{
	BOOL bStartState = bStart?TRUE:FALSE;
	m_controlChannel->SetFlag(33,bStartState,bStartState);
}

void CDeviceParker::JetParkerRunProgram()
{
	CString strSend = _T("");

	strSend.Format(_T("LRUN PROG0\r"));
	strSend += _T("\r");
	BSTR bstrCommand = ::SysAllocString(strSend);
	m_controlChannel->Write(bstrCommand);
	::SysFreeString(bstrCommand);
}

void CDeviceParker::JetParkerJogStep(double dbStep,bool bPositive,int nAxis)
{
	CString strSend = _T("");
	double stepLenth = 0.0;
	if(!m_bProgWrite)
	{
		JetParkerSetProg();

		m_bProgWrite = true;
	}

	//0表示X轴，1表示Y轴
	switch(nAxis)
	{
	case 0:
		if(bPositive)
		{
			stepLenth = dbStep;
			strSend.Format(_T("JOG INC X%.2f\r"),stepLenth);
			strSend += _T("\r");
			BSTR bstr_Send = ::SysAllocString(strSend);
			m_controlChannel->Write(bstr_Send);
			::SysFreeString(bstr_Send);

		}
		//反向移动
		else
		{
			stepLenth = -dbStep;
			strSend.Format(_T("JOG INC X%.2f \r"),stepLenth);
			strSend += _T("\r");
			BSTR bstr_Send = ::SysAllocString(strSend);
			m_controlChannel->Write(bstr_Send);
			::SysFreeString(bstr_Send);
		}
		break;

	case 1:
		if(bPositive)
		{
			stepLenth = dbStep;
			strSend.Format(_T("JOG INC Y%.2f \r"),stepLenth);
			strSend += _T("\r");
			BSTR bstrSend = ::SysAllocString(strSend);
			m_controlChannel->Write(bstrSend);
			::SysFreeString(bstrSend);
		}
		//反向移动
		else
		{
			stepLenth = -dbStep;
			strSend.Format(_T("JOG INC Y%.2f \r"),stepLenth);
			strSend += _T("\r");
			BSTR bstrSend = ::SysAllocString(strSend);
			m_controlChannel->Write(bstrSend);
			::SysFreeString(bstrSend);
		}
		break;
	}

}

bool CDeviceParker::JetParkerIsMotorEnabled() 
{
	bool bMotorXEnabled = false;
	bMotorXEnabled = JetParkerIsBitChecked(4360,28);
	bool bMotorYEnabled = false;
	bMotorYEnabled = JetParkerIsBitChecked(4361,28);

	if(bMotorXEnabled && bMotorYEnabled)
		m_bIsMotorEnable = true;
	else
		m_bIsMotorEnable = false;

	return m_bIsMotorEnable;
}

bool CDeviceParker::JetParkerSetProg()
{
	if(m_bIsParkerConnected)
	{
		CString strSend = _T("");
		strSend.Format(_T("%s"),_T("PROG0"));
		strSend += _T("\r");

		try{
			BSTR bstrSend = ::SysAllocString(strSend);
			m_controlChannel->Write(bstrSend);
			::SysFreeString(bstrSend);
		}
		catch(_com_error error)
		{
			//do something
			return false;
		}

		return true;
	}

	return false;
}


bool CDeviceParker::JetParkerIsXJogPlus() const
{
	return m_bXJogPlus;
}

bool CDeviceParker::JetParkerIsXJogMinus() const
{
	return m_bXJogMinus;
}

bool CDeviceParker::JetParkerIsYJogPlus() const
{
	return m_bYJogPlus;
}

bool CDeviceParker::JetParkerIsYJogMinus() const
{
	return m_bYjogMinus;
}

bool CDeviceParker::JetParkerIsBitChecked(int nGroup,int nIndex)
{
	bool bResult = false;
	int nItem = 0;
	if(nGroup<4097||nIndex<0)
	{
	bResult = false;
	return bResult;
	}

	CString strCheck = _T("");
	strCheck.Format(_T("P%d"),nGroup);
	BSTR bstrCommand = ::SysAllocString(strCheck);
	CComVariant variant;
	CComVariant vItem;
	SAFEARRAY *pSafeArray;
	long lBound = 0;
	try{variant = m_controlChannel->GetACRCustom(bstrCommand);}
	catch(_com_error){};
	::SysFreeString(bstrCommand);

	if(variant.vt & VT_ARRAY)
	{
		pSafeArray = variant.parray;
		SafeArrayGetElement(pSafeArray,&lBound,&vItem);

		if(vItem.vt != VT_EMPTY)
		{
		nItem = vItem.lVal;
		}
	}

	variant.Clear();

	BOOL bStatusChect = FALSE;
	try
	{
		bStatusChect = m_controlChannel->IsFlagSet(nItem,nIndex);
	}
	catch (_com_error* e)
	{
		CString errorMessage = _T("");
		errorMessage.Format(_T("%s"),e->ErrorMessage());
		TRACE(errorMessage);
	}
	

	bResult = bStatusChect?true:false;

	return bResult;
}

void CDeviceParker::JetParkerGetCustomParamLong(int nParam,long &result)
{
	long nResult = 0;

	CString strCheck = _T("");
	strCheck.Format(_T("P%d"),nParam);
	BSTR bstrCommamd = ::SysAllocString(strCheck);
	CComVariant variant;
	CComVariant vItem;
	SAFEARRAY *pSafeArray;
	long lBound = 0;
	variant = m_controlChannel->GetACRCustom(bstrCommamd);
	::SysFreeString(bstrCommamd);

	if(variant.vt & VT_ARRAY)
	{
		pSafeArray = variant.parray;
		SafeArrayGetElement(pSafeArray,&lBound,&vItem);

		if(vItem.vt != VT_EMPTY)
		{
			if(vItem.vt == VT_I4)
			{
				nResult = vItem.lVal;
			}
		}
	}

	result = nResult;
}

void CDeviceParker::JetParkerGetCustomParamFloat(int nParam,float &result)
{
	float fResult = 0;

	CString strCheck = _T("");
	strCheck.Format(_T("P%d"),nParam);
	BSTR bstrCommand = ::SysAllocString(strCheck);
	CComVariant variant;
	CComVariant vItem;
	SAFEARRAY *pSafeArray;
	long lBound = 0;
	variant = m_controlChannel->GetACRCustom(bstrCommand);
	::SysFreeString(bstrCommand);

	if(variant.vt & VT_ARRAY)
	{
		pSafeArray = variant.parray;
		SafeArrayGetElement(pSafeArray,&lBound,&vItem);

		if(vItem.vt != VT_EMPTY)
		{
			if(vItem.vt == VT_R4)
			{
				fResult = vItem.fltVal;
			}

			if(vItem.vt == VT_R8)
			{
				fResult = (float)(vItem.dblVal);
			}
		}
	}

	result = fResult;
}

bool CDeviceParker::JetParkerIsConnected()
{
	return m_controlChannel->TestConnect()?true:false;
}

bool CDeviceParker::JetParkerDownloadFile(CString fileName)
{
	bool bResult = false;
	if(!fileName.IsEmpty())
	{
		CString progDest = _T("");

		progDest.Format(_T("PROG0"));
		BSTR bstrProgDest = ::SysAllocString(progDest);
		BSTR bstrFileName = ::SysAllocString(fileName);
		m_controlChannel->DownloadFile(bstrProgDest,bstrFileName);
		::SysFreeString(bstrProgDest);
		::SysFreeString(bstrFileName);

		//需要检测文件加载到控制器的结果，使用一个多线程
		long m_total = 0;
		long m_byte = 0;
		bool bDownLoadStatus = true;
		CString m_Status = _T("");
		while(bDownLoadStatus)
		{
			long status = m_controlChannel->GetStatusDL(&m_total,&m_byte);

			switch(status){
			case 0:  
				m_Status = _T("No Transfer in progress.");
				bDownLoadStatus = false;
				bResult = true;
				break;
			case 1:
				bDownLoadStatus = true;
				m_Status = _T("Transfer in progress.");
				break;
			case 2:
				m_Status = _T("End of transfer.");
				bDownLoadStatus = false;
				bResult = true;
				break;
			case 3:
				m_Status = _T("User cancelled Transfer.");
				bDownLoadStatus = false;
				bResult = false;
				break;
			case 4:
				m_Status = _T("Error reading from file.");
				bDownLoadStatus = false;
				bResult = false;
				break;
			case 5:
				m_Status = _T("Too many errors during Transfer.");
				bDownLoadStatus = false;
				bResult = false;
				break;
			case 6:
				m_Status = _T("Transfer has timed out waiting for response.");
				bDownLoadStatus = false;
				bResult = false;
				break;
			case 7:
				m_Status = _T("The ACR OS failed to verify against the hardware description file (config image).");
				bDownLoadStatus = false;
				bResult = false;
				break;
			case 8:
				m_Status = _T("Save to flash of OS in progress.");
				bDownLoadStatus = false;
				bResult = false;
				break;
			case 9:
				m_Status = _T("Problem encountered in saving OS.");
				bDownLoadStatus = false;
				bResult = false;
				break;
			case 10:
				m_Status = _T("Problem encountered in saving OS, failed CRC check.");
				bDownLoadStatus = false;
				bResult = false;
				break;
			case 11:
				m_Status = _T("Problem encountered in saving OS, problem with flash.");
				bDownLoadStatus = false;
				bResult = false;
				break;
			default:
				char holdNbr[20];
				_ltoa_s(status,holdNbr,10);
				m_Status = _T("Some Problem downloading file. Error: ") + CString(holdNbr);
				bDownLoadStatus = false;
				bResult = false;
				break;
			}
		}

		return bResult;

	}

	return bResult;
}


void CDeviceParker::JetParkerMoveVelSet(double Speed)
{
	CString strSend = _T("");
	strSend.Format(_T("VEL %.2f"),Speed);
	strSend += _T("\r");

	try{
		BSTR bstrCommand = ::SysAllocString(strSend);
		m_controlChannel->Write(bstrCommand);
		::SysFreeString(bstrCommand);
	}
	catch(_com_error error)
	{
		//do something

	}

	
}

void CDeviceParker::JetParkerInitWnd(CWnd *pWnd)
{
	if(NULL != pWnd)
	{
		m_pWnd = pWnd;
	}
}

void CDeviceParker::JetParkerUnInitialize()
{
	if(NULL != m_controlChannel)
	{
		m_controlChannel.Release();
	}

	m_bIsParkerInitialized = false;
	//CoUninitialize();
}

void CDeviceParker::JetParkerTerminateWatchThread()
{
	m_ThreadParam.m_bIsExitThread = true;

	SetEvent(m_WatchStatusEvent);

}