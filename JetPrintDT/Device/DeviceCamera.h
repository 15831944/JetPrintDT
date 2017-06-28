#ifndef __DEVICE_CAMERA_H
#define __DEVICE_CAMERA_H
#include <tchar.h>
#include <afx.h>

#include "../DataStruct/JetPrintDTList.h"
#include "../SingleTon/SingletonTemplate.h"
#include "../SingleTon/CameraSharedBuffer.h"
#define  CAMERA_MESSAGE	WM_USER+100

typedef enum {
	WATCH_CAMERA,LOCATE_CAMERA,NULL_CAMERA
}CAMERA_TYPE;
class CDeviceCamera :public CSingletonTemplate<CDeviceCamera>{

	friend CSingletonTemplate<CDeviceCamera>;
private:
	CDeviceCamera();
	CDeviceCamera(const CDeviceCamera& camera);
	CDeviceCamera& operator=(const CDeviceCamera& camera);
	static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
	HWND GetHwndByProcessId(DWORD dwProcessId);
public:
	virtual ~CDeviceCamera();
	virtual bool InitializeCamera();
	
	bool GetWatchCameraStatus(){
		return m_bWatchCameraStatus;
	}

	bool GetLocateCameraStatus(){
		return m_bLocateCameraStatus;
	}
	
	//
	void CameraStartGrab();
	void CameraStopGrab();
	void CameraSwitchDevice();
	void CameraClose();
	//Exit Camera Server
	void CameraExit();

	//private members
private:
	bool m_bWatchCameraStatus;
	bool m_bLocateCameraStatus;
	
public:
	typedef struct{
		HWND cameraWnd;
		DWORD cameraProcessID;
	}WINDINFO;

	HWND hWnd;

	CAMERA_TYPE m_CurrentCameraType;
	bool m_bStartGrab;
};
#endif // !__DEVICE_CAMERA_H
