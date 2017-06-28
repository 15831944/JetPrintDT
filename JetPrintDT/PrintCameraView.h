#pragma once


#include "Device/cvShowVideo.h"
// CPrintCameraView 视图
class CPrintCameraView : public CScrollView
{
	DECLARE_DYNCREATE(CPrintCameraView)

protected:
	CPrintCameraView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPrintCameraView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	virtual void OnInitialUpdate();     // 构造后的第一次

	DECLARE_MESSAGE_MAP()

public:
	GdiplusStartupInput   m_gdiplusStartupInput;
	ULONG_PTR   m_gdiplusToken;
	bool        m_bGrabImage;
	CCVShowVideo m_ShowVideo;
	CWinThread *m_pShowImageThread;
	
	int m_iScrollHeight;
	int m_iScrollWidth;

	typedef struct{
		HWND cameraWnd;
		DWORD cameraProcessID;
	}WINDINFO;


	LPBITMAPINFO m_lpBmi;

public:
	void StopGrabImage();
	void StartGrabImage();
	void ZoomOutImage();
	void ZoomInImage();
	void ShowOrign();

	static UINT ShowImage(LPVOID pData);
	void ShowScale();
	bool EnableScale(){
		return m_ShowVideo.m_bShowScale;
	}
	void SaveImage(char* szFileName){
		m_ShowVideo.SaveImage(szFileName);
	}
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
};


