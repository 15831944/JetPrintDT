#ifndef __HOST_INSTANCE_H
#define __HOST_INSTANCE_H

#include "SingletonTemplate.h"
#include "CPUInfoInstance.h"
class CHostInstance :public CSingletonTemplate<CHostInstance>
{
	friend class CSingletonTemplate<CHostInstance>;
private:
	CHostInstance();
	CHostInstance(const CHostInstance& hostInstance);
	CHostInstance& operator=(const CHostInstance& hostInstance);

	//private member
private:
	HWND m_hWnd;
	HDC m_hDC;
	int horzSizeMM;
	int vertSizeMM;
	int horzSizePixel;
	int vertSizePixel;
	double poundToMM;

	CString appPath;
	CPUInfo m_CPUInfo;
public:
	virtual ~CHostInstance(){
		::ReleaseDC(m_hWnd, m_hDC);
	}

	void InitializeHost(HWND hWnd);
	int GetHorizonSizeInMillimeter();
	int GetHorizonSizeInPixel();
	int GetVerticSizeInMillimeter();
	int GetVerticSizeInPixel();
	double GetPoundToMillimeter();
	CString GetAppPath();
private:
	void GenerateAppPath();
};


#endif //__HOST_INSTANCE_H