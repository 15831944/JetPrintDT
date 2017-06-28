#include "HostInstance.h"

CHostInstance::CHostInstance() :CSingletonTemplate<CHostInstance>(){
	m_hWnd = nullptr;
	m_hDC = nullptr;
	horzSizeMM = 0;
	vertSizeMM = 0;
	horzSizePixel = 0;
	vertSizePixel = 0;
	poundToMM = 0.35; //National Standard
	appPath = _T("");

	GenerateAppPath();
}

void CHostInstance::InitializeHost(HWND hWnd){
	m_hWnd = hWnd;
	m_hDC = ::GetDC(m_hWnd);
}

int CHostInstance::GetHorizonSizeInMillimeter(){
	if (nullptr == m_hDC)
		return horzSizeMM;
	horzSizeMM = ::GetDeviceCaps(m_hDC, HORZSIZE);
	return horzSizeMM;
}
int CHostInstance::GetHorizonSizeInPixel(){
	if (nullptr == m_hDC)
		return horzSizePixel;
	horzSizePixel = ::GetDeviceCaps(m_hDC, HORZRES);
	return horzSizePixel;
}

int CHostInstance::GetVerticSizeInMillimeter(){
	if (nullptr == m_hDC)
		return vertSizeMM;
	vertSizeMM = ::GetDeviceCaps(m_hDC, VERTSIZE);
	return vertSizeMM;
}

int CHostInstance::GetVerticSizeInPixel(){
	if (nullptr == m_hDC)
		return vertSizePixel;
	vertSizePixel = ::GetDeviceCaps(m_hDC, VERTRES);
	return vertSizePixel;
}

double CHostInstance::GetPoundToMillimeter(){
	return poundToMM;
}

void CHostInstance::GenerateAppPath(){
	TCHAR modulePath[MAX_PATH];
	GetModuleFileName(NULL, modulePath, MAX_PATH);
	appPath = modulePath;
	appPath = appPath.Left(appPath.ReverseFind(_T('\\')));
}

CString CHostInstance::GetAppPath(){
	return appPath;
}