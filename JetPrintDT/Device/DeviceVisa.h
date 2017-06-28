#ifndef __DEVICE_VISA_H
#define __DEVICE_VISA_H

#define ELEMLENTH(a) (sizeof(a) / sizeof((a)[0]))
#define ErrorHandler()  CDeviceVisa::ErrorHandlerEx(__LINE__, __FILE__)

#include <tchar.h>
#include <afx.h>

#include "visa.h"
#include "../DataStruct/JetPrintDTList.h"
#include "../SingleTon/SingletonTemplate.h"

#if _DEBUGE_VISA
#pragma comment(lib,"Device/visa32.lib")
#endif

class CDeviceVisa:public CSingletonTemplate<CDeviceVisa>{

	friend CSingletonTemplate<CDeviceVisa>;
private:
	CDeviceVisa();
	CDeviceVisa(const CDeviceVisa& visa);
	CDeviceVisa& operator=(const CDeviceVisa& visa);
public:
	virtual ~CDeviceVisa();
	//private members
private:
	char model[MAX_PATH];				///< The buffer for the model number
	char address[MAX_PATH];				///< The buffer for the instrument bus address
	char manufacturer[MAX_PATH];		///< The buffer for the instrument's manufacturer
	ULONG vi;							///< The VISA virtual instrument token
	ULONG rm;							///< The VISA resource manager token
	bool  isConnected;					///< The connected flag
	CJetPrintDTList<LPTSTR,CDeviceVisa> m_deviceList;
private:

	void CheckStatus(ViSession vi, ViStatus status);
	void SendGoToLocal(ViSession vi);

	void ErrorHandlerEx(long wLine, LPSTR lpszFile);
	LPTSTR GetSEHErrorMessage(DWORD dwError);

	void WriteLine(ViSession vi, LPSTR output);
	LPSTR Read(ViSession vi, int bufferSize);

	void GetDeviceParams(char* buffer);

public:
	bool IsDeviceConnected(){
		return isConnected;
	}

	bool InitInstrument();
	bool FindInstruments(void);
	void IdentifyInstrument();
	bool ConnectInstruments();
	bool DisConnectInstruments();
	std::wstring GetCurrentDeviceAddress();
	void CharToWchar(const char *constCharString, LPTSTR result);
	void WCharToChar(TCHAR *InWcharm, char* result);

	void SetDCFunction(long offset = 0);
	void SetSquareFunction();
	void SetOffset(long offset);
	void SetFrequency(double freq);
	void SetAmplitude(double amp);
	void SetDCycle(double dCycle);
	void SetOutput(BOOL isOutput);

	const char* GetModel()const{
		return model;
	}
	const char* GetManufacture() const{
		return manufacturer;
	}


	void CreateDataFunc(LPTSTR&);
	void CopyDataFunc(LPTSTR dest, LPTSTR source);
	void DestroyDataFunc(LPTSTR data);
};
#endif // !__DEVICE_VISA_H
