#include "../stdafx.h"
#include <afxwin.h>
#include "DeviceCamera.h"
#include "../SingleTon/ParamInitSingleton.h"

CDeviceCamera::CDeviceCamera(){
	m_bWatchCameraStatus = false;
	m_bLocateCameraStatus = false;
	hWnd = nullptr;
	m_CurrentCameraType = NULL_CAMERA;
	m_bStartGrab = false;
}

CDeviceCamera::~CDeviceCamera(){

}

bool CDeviceCamera::InitializeCamera(){
	bool bResult = true;
	int icount = 0;
	CCameraSharedBuffer<char> *pCameraBuffer = CCameraSharedBuffer<char>::Instance();
	CCameraParam *pCameraParam = CCameraParam::Instance();
	if (bResult){
		if (pCameraBuffer){
			if (pCameraParam){
				pCameraBuffer->ReadData((char*)(pCameraParam), BUFFER_SIZE);
			}
			
			CParamInitSingleton* pFileParam = CParamInitSingleton::Instance();
			int iwatchCameraID = pFileParam->GetWatchCameraID();
			int ilocateCameraID = pFileParam->GetLocateCameraID();

			if (pCameraParam){
				pCameraParam->m_lWatchCameraSerial = iwatchCameraID;
				pCameraParam->m_lLocateCameraSerial = ilocateCameraID;
			
				unsigned long long iResult = pCameraBuffer->WriteData((char*)(pCameraParam), BUFFER_SIZE);
				
				Sleep(10);
				pCameraBuffer->ReadData((char*)(pCameraParam), BUFFER_SIZE);
				while (pCameraParam->m_lWatchCameraSerial != iwatchCameraID && pCameraParam->m_lLocateCameraSerial != ilocateCameraID){
					pCameraParam->m_lWatchCameraSerial = iwatchCameraID;
					pCameraParam->m_lLocateCameraSerial = ilocateCameraID;

				    pCameraBuffer->WriteData((char*)(pCameraParam), BUFFER_SIZE);
					Sleep(100);
					pCameraBuffer->ReadData((char*)(pCameraParam),BUFFER_SIZE);
					++icount;

					if (icount > 10)
						break;
				}
				
				if (iResult < BUFFER_SIZE)
					return false;
			}

			if (nullptr == hWnd)
				hWnd = GetHwndByProcessId(pCameraParam->m_iCurrentProcessID);
			if (nullptr == hWnd)
				hWnd = ::FindWindow(NULL, _T("CameraServer"));
			if (hWnd){

				::SendMessage(hWnd, CAMERA_MESSAGE, 0, CAMERA_OPEN);

				Sleep(100);
				pCameraBuffer->ReadData((char*)(pCameraParam), BUFFER_SIZE);

				icount = 0;
				while (pCameraParam->m_bMessageArrived == false){
					::PostMessage(hWnd, CAMERA_MESSAGE, 0, CAMERA_OPEN);
					Sleep(200);
					pCameraBuffer->ReadData((char*)(pCameraParam), BUFFER_SIZE);
					++icount;
					if (icount>5)
						break;
				}

				if (false == pCameraParam->m_bMessageArrived){
					pCameraParam->m_bEnableOpenCamera = true;
					pCameraBuffer->WriteData((char*)(pCameraParam), BUFFER_SIZE);
					Sleep(500);
					pCameraBuffer->ReadData((char*)(pCameraParam), BUFFER_SIZE);
					icount = 1;
					while (pCameraParam->m_bEnableOpenCamera == true || pCameraParam->m_bEnableCloseCamera == false)
					{
						pCameraParam->m_bEnableOpenCamera = true;
						pCameraBuffer->WriteData((char*)(pCameraParam), BUFFER_SIZE);
						Sleep(500);
						pCameraBuffer->ReadData((char*)(pCameraParam), BUFFER_SIZE);
						++icount;
						if (icount >5)
							break;
					}

				}
			}

			Sleep(1000);
		}
	}
	
	icount = 0;
	do{
		++icount;
		Sleep(20);
		pCameraBuffer->ReadData((char*)(pCameraParam), BUFFER_SIZE);
		if (icount > 2000)
			break;
	} while (false == pCameraParam->m_bCameraOpenFlag);

	m_bWatchCameraStatus = pCameraParam->m_bOpenWatchCamera;
	m_bLocateCameraStatus = pCameraParam->m_bOpenLocateCamera;

	if (m_bWatchCameraStatus)
		m_CurrentCameraType = WATCH_CAMERA;
	else if (false == m_bWatchCameraStatus&&m_bLocateCameraStatus)
		m_CurrentCameraType = LOCATE_CAMERA;
	else
		m_CurrentCameraType = NULL_CAMERA;

	bResult = pCameraParam->m_iCurrentProcessID > 0;

	pCameraParam->m_bCameraOpenFlag = true;
	pCameraBuffer->WriteData((char*)(pCameraParam), BUFFER_SIZE);
	Sleep(50);

	return bResult;
}



BOOL CALLBACK CDeviceCamera::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	WINDINFO* pInfo = (WINDINFO*)lParam;
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId);

	if (dwProcessId == pInfo->cameraProcessID)
	{
		pInfo->cameraWnd = hWnd;
		return FALSE;
	}
	return TRUE;
}

HWND CDeviceCamera::GetHwndByProcessId(DWORD dwProcessId)
{
	WINDINFO info = { 0 };
	info.cameraWnd = NULL;
	info.cameraProcessID = dwProcessId;
	EnumWindows(EnumWindowsProc, (LPARAM)&info);
	return info.cameraWnd;
}


void CDeviceCamera::CameraStartGrab(){
	CCameraSharedBuffer<char> *pCameraBuffer = CCameraSharedBuffer<char>::Instance();
	CCameraParam *pCameraParam = CCameraParam::Instance();
	if (pCameraBuffer){
		if (pCameraParam){
			switch (m_CurrentCameraType){
			case WATCH_CAMERA:
				pCameraParam->m_bLocateCameraGrab = false;
				pCameraParam->m_bWatchCameraGrab = true;
				break;

			case LOCATE_CAMERA:
				pCameraParam->m_bLocateCameraGrab = true;
				pCameraParam->m_bWatchCameraGrab = false;
				break;

			case NULL_CAMERA:
				pCameraParam->m_bLocateCameraGrab = false;
				pCameraParam->m_bWatchCameraGrab = false;
				break;
			}
		}

		pCameraBuffer->WriteData((char*)(pCameraParam), BUFFER_SIZE);
	}

	Sleep(1000);
	if (nullptr == hWnd)
		hWnd = GetHwndByProcessId(pCameraParam->m_iCurrentProcessID);
	if (hWnd){
		::SendMessage(hWnd, CAMERA_MESSAGE, 0, CAMERA_START_GRAB);

		m_bStartGrab = true;
	}
}
void CDeviceCamera::CameraStopGrab(){
	CCameraSharedBuffer<char> *pCameraBuffer = CCameraSharedBuffer<char>::Instance();
	CCameraParam *pCameraParam = CCameraParam::Instance();
	if (pCameraBuffer&&pCameraParam){
		pCameraParam->m_bLocateCameraGrab = false;
		pCameraParam->m_bWatchCameraGrab = false;
	}

	pCameraBuffer->WriteData((char*)(pCameraParam), BUFFER_SIZE);
	if (nullptr == hWnd)
		hWnd = GetHwndByProcessId(pCameraParam->m_iCurrentProcessID);
	if (hWnd){
		::SendMessage(hWnd, CAMERA_MESSAGE, 0, CAMERA_START_GRAB);

		m_bStartGrab = false;
	}
}

void CDeviceCamera::CameraSwitchDevice(){
	CCameraSharedBuffer<char> *pCameraBuffer = CCameraSharedBuffer<char>::Instance();
	CCameraParam *pCameraParam = CCameraParam::Instance();
	if (pCameraBuffer&&pCameraParam){
		if (m_CurrentCameraType == WATCH_CAMERA){
			//switch to locate camera
			pCameraParam->m_bLocateCameraGrab = true;
			pCameraParam->m_bWatchCameraGrab = false;

			m_CurrentCameraType = LOCATE_CAMERA;
		}
		else if (m_CurrentCameraType == LOCATE_CAMERA){
			//switch to watch camera
			pCameraParam->m_bLocateCameraGrab = false;
			pCameraParam->m_bWatchCameraGrab = true;

			m_CurrentCameraType = WATCH_CAMERA;
		}
		
	}

	pCameraBuffer->WriteData((char*)(pCameraParam), BUFFER_SIZE);
	if (nullptr == hWnd)
		hWnd = GetHwndByProcessId(pCameraParam->m_iCurrentProcessID);
	if (hWnd){
		::SendMessage(hWnd, CAMERA_MESSAGE, 0, CAMERA_START_GRAB);

		m_bStartGrab = true;
	}
}

void CDeviceCamera::CameraClose(){
	if (hWnd){
		::SendMessage(hWnd, CAMERA_MESSAGE, 0, CAMERA_CLOSE);

		m_bStartGrab = false;
	}
}


//Exit Camera Server
void CDeviceCamera::CameraExit(){
	if (hWnd){
		::SendMessage(hWnd, CAMERA_MESSAGE, 0, CAMERA_EXIT);
	}
}