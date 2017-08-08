#include "../stdafx.h"
#include "DeviceVisa.h"

CDeviceVisa::CDeviceVisa(){
	memset(model, 0, sizeof(model));
	memset(address, 0, sizeof(address));
	memset(address, 0, sizeof(manufacturer));
	vi = 0;
	rm = 0;
	isConnected = false;


	//register call back funcs
	m_deviceList.RegisterInstance(this);
	m_deviceList.RegisterCopyDataFunc(&CDeviceVisa::CopyDataFunc);
	m_deviceList.RegisterCreateDataFunc(&CDeviceVisa::CreateDataFunc);
	m_deviceList.RegisterDestroyDataFunc(&CDeviceVisa::DestroyDataFunc);

}

CDeviceVisa::~CDeviceVisa(){

}

bool CDeviceVisa::InitInstrument(){
	bool bResult = false;
	bResult = FindInstruments();
	if (bResult){
		bResult = ConnectInstruments();
	}

	return bResult;
}

// brief: get all the visa devices that connected to the host
// param: void
// result: true
bool CDeviceVisa::FindInstruments(){
	
#if _DEBUGE_VISA
	if (rm != NULL)
		viClose(rm);
#endif

	ViStatus status = VI_SUCCESS;

#if _DEBUGE_VISA
	 status = viOpenDefaultRM(&rm);
#endif

	 CheckStatus(rm, status);
	/*-------Modify Array to configure searches-------*/
	LPSTR strSearch[] = { "?*INSTR", "?*SOCKET" }; //all types
	/*------------------------------------------------*/

	LPTSTR addresses = new TCHAR[MAX_PATH];
	memset(addresses, 0, sizeof(TCHAR)*MAX_PATH);

	ViChar buf[VI_FIND_BUFLEN];

	for (int i = 0; i <= ELEMLENTH(strSearch); i++)
	{
		__try
		{
			unsigned long count = 0;

#if _DEBUGE_VISA
			ViFindList findLst = 0;
			memset(&buf, 0, ELEMLENTH(buf));

			viFindRsrc(rm, strSearch[i], &findLst, &count, buf);
#endif

			if (count > 0) // Prevent Stack overflow
			{
				count--;
				CharToWchar(buf, addresses);
				m_deviceList.AddTail(addresses);
				memset(addresses, 0, sizeof(TCHAR)*MAX_PATH);
			}

			while (count--)
			{
				memset(&buf, 0, ELEMLENTH(buf));

				//buffer data overwritten with new data
#if _DEBUGE_VISA
				viFindNext(findLst, buf);
#endif
				CharToWchar(buf, addresses);
				m_deviceList.AddTail(addresses);
				memset(addresses, 0, sizeof(TCHAR)*MAX_PATH);
			}

#if _DEBUGE_VISA
			if (findLst != 0)
				viClose(findLst);
#endif
		}  // __try
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			SetLastError(GetExceptionCode());
			ErrorHandler();
		}
	} // next

	if (nullptr != addresses)
		delete[] addresses;
	return true;
}

// brief: check if status is ok 
// param: vi the associated virtual instrument
//		  status current status
// result:void
void CDeviceVisa::CheckStatus(ViSession vi, ViStatus status){
	static char buf[MAX_PATH] = { 0 };
	if (status < VI_SUCCESS)
	{
#if _DEBUGE_VISA
		viStatusDesc(vi, status, buf);
#endif
	}

}

void CDeviceVisa::ErrorHandlerEx(long wLine, LPSTR lpszFile)
{
	LPTSTR lpMessage = TEXT("");
	DWORD dwError = GetLastError();
	DWORD dwRtn = 0;
	TCHAR szBuffer[256] = { 0 };

	//Only want the file name, not the path
	int len = strlen(lpszFile);
	LPSTR c = lpszFile + len;
	for (int i = len + 1; (0 < i) && ('\\' != *c) && ('/' != *c); --c, --i);
	++c;
	TCHAR* szFile = new TCHAR[strlen(c) + 11]; // Want some slop so title won't be truncated
	memset(szFile, 0, sizeof(szFile));

	// Convert the filename to wide char 
	//  (Predefined preprocessor symbol __FILE__ is an Ascii string literal)
#ifdef _UNICODE
	MultiByteToWideChar(0, 0, (LPSTR)c, ELEMLENTH(szFile), szFile, ELEMLENTH(szFile));
#else
	strncpy(szFile, (LPSTR)c, NELEMS(szFile));
#endif

	wcscat_s(szFile, 256, _T("          "));

	// Display the error information along with the place the error happened.
	wsprintf(szBuffer, TEXT("Generic, Line=%d, File=%s"), wLine, szFile);

	// Allow FormatMessage() to look up the error code returned by GetLastError
	dwRtn = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwError,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		lpMessage,
		0,
		NULL);

	// Check to see if an error occurred calling FormatMessage()
	if (0 == dwRtn)
	{
		LPTSTR strErr = GetSEHErrorMessage(dwError);
		if (0 == _tcscmp(_T("Error Unknown"), strErr))
		{
			wsprintf(szBuffer,
				TEXT("An error occurred calling FormatMessage().")
				TEXT("Error Code %d"),
				dwError);
			MessageBox(NULL,
				szBuffer,
				TEXT("Generic"),
				MB_ICONSTOP | MB_ICONEXCLAMATION);
		}
		else
		{
			MessageBox(NULL, strErr, szBuffer, MB_ICONEXCLAMATION | MB_OK);
		}
		return;
	}

	if (nullptr != szFile)
		delete[] szFile;
	MessageBox(NULL, lpMessage, szBuffer, MB_ICONEXCLAMATION | MB_OK);
}

LPTSTR CDeviceVisa::GetSEHErrorMessage(DWORD dwError)
{
	static LPTSTR strErr = TEXT("");

	switch (dwError)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		strErr = TEXT("Access violation");
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		strErr = TEXT("Array bounds exceeded.");
		break;
	case EXCEPTION_BREAKPOINT:
		strErr = TEXT("A breakpoint is encountered.");
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		strErr = TEXT("Datatype misalignment.");
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		strErr = TEXT("Floating point operand is too small\nto represent a standerd floating point value.");
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		strErr = TEXT("Floating point devide by zero.");
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		strErr = TEXT("The result of a floating point operation\ncannot be represented exactly as a decimal fraction.");
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		strErr = TEXT("Invalid floating point operation.");
		break;
	case EXCEPTION_FLT_OVERFLOW:
		strErr = TEXT("Floating point overflow.");
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		strErr = TEXT("Floating point operation caused stack overflow or underflow.");
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		strErr = TEXT("Floating point operation caused stack underflow.");
		break;
	case EXCEPTION_GUARD_PAGE:
		strErr = TEXT("A thread has accessed memory allocated with the PAGE_GUARD modifier.");
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		strErr = TEXT("A thread has tried to execute an invalid instruction.");
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		strErr = TEXT("A thread has tried to access a page that is not present.");
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		strErr = TEXT("Integer divide by zero.");
		break;
	case EXCEPTION_INT_OVERFLOW:
		strErr = TEXT("Integer overflow.");
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		strErr = TEXT("The exception handler returned an invalid\ndisposition to the exception dispatcher.");
		break;
	case EXCEPTION_INVALID_HANDLE:
		strErr = TEXT("Invalid handle.");
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		strErr = TEXT("Attempted execution after a non-continuable exception occured.");
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		strErr = TEXT("Operation not allowed in the current computer mode.");
		break;
	case EXCEPTION_SINGLE_STEP:
		strErr = TEXT("A single instruction has executed.");
		break;
	case EXCEPTION_STACK_OVERFLOW:
		strErr = TEXT("Stack overflow.");
		break;
	default:
		strErr = TEXT("Error Unknown");
	}
	return strErr;
}

void CDeviceVisa::CharToWchar(const char *constCharString, LPTSTR result)
{
	int   nLen = strlen(constCharString) + 1;
	int   nwLen = MultiByteToWideChar(CP_ACP, 0, constCharString, nLen, NULL, 0);

	TCHAR   *wString = nullptr;
	wString = new TCHAR[nwLen];


	MultiByteToWideChar(CP_ACP, 0, constCharString, nLen, wString, nwLen);
	_tcscpy_s(result,nwLen, wString);//   wcscpy(outWchar,wString);


	delete[] wString;
}


void CDeviceVisa::WCharToChar(TCHAR *InWchar, char *OutStr)
{
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, InWchar, -1, NULL, 0, NULL, FALSE) + 1;
	WideCharToMultiByte(CP_OEMCP, NULL, InWchar, wcslen(InWchar), OutStr, dwNum, NULL, FALSE);
}

void CDeviceVisa::WriteLine(ViSession vi, LPSTR output)
{
	LPSTR strPut = new char[MAX_PATH];
	memset(strPut, 0, MAX_PATH*sizeof(char));
	sprintf_s(strPut, MAX_PATH, "%s%s", output, "\n");
	ViUInt32 actual = 0;

	ViStatus status = VI_SUCCESS;
#if _DEBUGE_VISA
	viWrite(vi, (ViBuf)strPut, strlen(strPut), &actual);
#endif

	CheckStatus(vi, status);

	delete[] strPut;
}


LPSTR CDeviceVisa::Read(ViSession vi, int bufferSize)
{
	ViChar *strRes = new ViChar[bufferSize];
	ViUInt32 actual = 0;

	// Read the results
	ViStatus status = VI_SUCCESS;
#if _DEBUG_VISA
	viRead(vi, (ViBuf)strRes, bufferSize, &actual);
#endif
	CheckStatus(vi, status);

	// NULL terminate the string
	strRes[actual] = 0;
	if (nullptr != strRes)
		delete[] strRes;

	return strRes;
}

void CDeviceVisa::GetDeviceParams(char* buffer){
	std::string result = "";
	result.assign(buffer);
	int nfirst = result.find(",");

	strcpy_s(manufacturer, result.substr(0, nfirst).c_str());

	int nSecond = result.find(",", nfirst + 1);
	strcpy_s(model, result.substr(nfirst + 1, nSecond).c_str());;
}

void CDeviceVisa::IdentifyInstrument(){
	char selected[MAX_PATH];
	memset(selected, 0, sizeof(char)*MAX_PATH);
	LPTSTR selectedAddress;
	
	__try
	{
		ViStatus status = VI_SUCCESS;
		ViSession device = 0;
		ViUInt32 actual = 0;
		ViChar response[MAX_PATH * 2];

		selectedAddress = m_deviceList.GetAt(0);
		WCharToChar(selectedAddress, selected);

#if _DEBUGE_VISA
		status = viOpen(rm, selected, 0, 500, &device);
#endif
		CheckStatus(device, status);

		WriteLine(device, "*IDN?");

#if _DEBUGE_VISA
		status = viRead(device, (ViBuf)response, sizeof(response), &actual);
#endif
		Sleep(1);

		if (status < VI_SUCCESS) //Read timed out or there was an error
		{
			if (status == VI_ERROR_TMO)
			{
				WriteLine(device, "ID?");
#if _DEBUGE_VISA
				status = viRead(device, (ViBuf)response, sizeof(response), &actual);
#endif
				Sleep(1);
				CheckStatus(device, status);

				if (0 != _stricmp(response, ""))
				{

					GetDeviceParams(response);
				}
				else
				{
					strcpy_s(model, "???");
					strcpy_s(manufacturer, "???");
				}
			}
			else
			{
				CheckStatus(device, status);
				__leave;
			}
		} //
		else
		{
			if (0 != _stricmp(response, ""))
			{
				GetDeviceParams(response);
			}
			else
			{
				strcpy_s(model, "???");
				strcpy_s(manufacturer, "???");
			}
		}
		SendGoToLocal(device);

#if _DEBUGE_VISA
		viClose(device);
#endif
	} //__try
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		SetLastError(GetExceptionCode());
		ErrorHandler();
	}

	if (selectedAddress)
		DestroyDataFunc(selectedAddress);
}

bool CDeviceVisa::ConnectInstruments(){
	ViStatus status = VI_SUCCESS;
	int timeout = 2000;
	int iSize = m_deviceList.GetSize();
	LPTSTR selectedAddress = nullptr;
	__try
	{
		if (iSize != 0)
		{
			selectedAddress = m_deviceList.GetAt(0);
			WCharToChar(selectedAddress, address);
			//open connection
#if _DEBUGE_VISA
			status = viOpen(rm, address, VI_NULL, timeout, &vi);
#endif
			CheckStatus(vi, status);
			isConnected = TRUE;
		}
	} //__try
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		SetLastError(GetExceptionCode());
		ErrorHandler();
	}

	if (selectedAddress)
		DestroyDataFunc(selectedAddress);

	return true;
}

std::wstring CDeviceVisa::GetCurrentDeviceAddress(){
	std::wstring deviceAddress ;
	LPTSTR address = m_deviceList.GetAt(0);
	deviceAddress.assign(address);

	delete [] address;

	return deviceAddress;
}

bool CDeviceVisa::DisConnectInstruments(){
#if _DEBUGE_VISA
	if (isConnected == TRUE)
	{
		//关闭到指定的USB接口控制的函数发生器的连接
		viClose(vi);

		//关闭总的资源管理器
		if (VI_SUCCESS == viClose(rm))
		{
			isConnected = FALSE;
			return TRUE;
		}
		else
		{
			isConnected = TRUE;
			return FALSE;
		}
	}

#endif
	return TRUE;
}

void CDeviceVisa::SendGoToLocal(ViSession vi)
{

	ViStatus status = VI_SUCCESS;
#if _DEBUGE_VISA
	status = viGpibControlREN(vi, VI_GPIB_REN_ADDRESS_GTL); // Go To Local
#endif
	CheckStatus(vi, status);
}

//设置波形为直流输出
//offset为默认初始电压
void CDeviceVisa::SetDCFunction(long offset/*=0*/)
{
	//clear
	WriteLine(vi, "*CLS\n");

	//set burst mode false
#if _DEBUGE_VISA
	viPrintf(vi, ":SOURce:BURSt:STATe %@1d\n", 0);

	//DC波形
	viPrintf(vi, ":SOURce:FUNCtion %s\n", "DC");

	//设置电压单位为V
	viPrintf(vi, ":SOURce:VOLTage:UNIT VPP\n");

	//电压上下限位-5~+5V
	viPrintf(vi, ":SOURce:VOLTage:LIMit:HIGH %@3lf\n", 5.0); //最大输出电压 
	viPrintf(vi, ":SOURce:VOLTage:LIMit:LOW %@3lf\n", -5.0); //最小输出电压 
	viPrintf(vi, ":SOURce:VOLTage:LIMit:STATe %@1d\n", 1);
#endif
	SetOffset(offset);
}

//设置波形为方波
void CDeviceVisa::SetSquareFunction()
{
	//clear
	WriteLine(vi, "*CLS\n");
#if _DEBUGE_VISA
	//方波波形
	viPrintf(vi, ":SOURce:FUNCtion %s\n", "SQUare");

	//设置电压单位为V
	viPrintf(vi, ":SOURce:VOLTage:UNIT VPP\n");

	//电压上下限位-5~+5V
	viPrintf(vi, ":SOURce:VOLTage:LIMit:HIGH %@3lf\n", 5.0); //最大输出电压 
	viPrintf(vi, ":SOURce:VOLTage:LIMit:LOW %@3lf\n", -5.0); //最小输出电压 
	viPrintf(vi, ":SOURce:VOLTage:LIMit:STATe %@1d\n", 1);

	//方波开启触发，门控模式
	viPrintf(vi, ":SOURce:BURSt:MODE GATed\n");
	viPrintf(vi, ":SOURce:BURSt:STATe %@1d\n", 1);

	//External Trigger,Slope Positive Trigger
	viPrintf(vi, ":TRIGger:SOURce EXTernal\n");
	viPrintf(vi, ":TRIGger:SLOPe POSitive\n");
	
#endif
}

//电压偏执为offset mV，范围为-5000~5000
void CDeviceVisa::SetOffset(long offset)
{
#if _DEBUGE_VISA
	viPrintf(vi, ":SOURce:VOLTage:OFFSet %@3ld mV\n", offset);
#endif
}

//频率(kHz) 
void CDeviceVisa::SetFrequency(double freq)
{
#if _DEBUGE_VISA
	viPrintf(vi, ":SOURce:FREQuency %@3lf HZ\n", freq);
#endif
}

//幅值,单位为V
void CDeviceVisa::SetAmplitude(double amp)
{
#if _DEBUGE_VISA
	viPrintf(vi, ":SOURce:VOLTage %@3lf mV\n", amp);
#endif
}

//方波波形占空比,%多少
void CDeviceVisa::SetDCycle(double dCycle)
{
#if _DEBUGE_VISA
	viPrintf(vi, ":SOURce:FUNCtion:SQUare:DCYCle %@3lf\n", dCycle);
#endif
}

//是否输出电压信号
void CDeviceVisa::SetOutput(BOOL isOutput)
{
#if _DEBUGE_VISA
	viPrintf(vi, ":OutPut %@1d\n", isOutput == TRUE ? 1 : 0);
#endif
}


//call back funcs
void CDeviceVisa::CreateDataFunc(LPTSTR& data){
	data = new TCHAR[MAX_PATH];
	if (nullptr == data)
		return;
	else{
		memset(data, 0, sizeof(TCHAR)*(MAX_PATH));
	}
}


void CDeviceVisa::CopyDataFunc(LPTSTR dest, LPTSTR source){
	assert(dest);
	assert(source);
	_tcsncpy_s(dest,MAX_PATH, source, MAX_PATH);
}


void CDeviceVisa::DestroyDataFunc(LPTSTR data){
	assert(data);

	delete[] data;
}