// JetPrintDT.h : JetPrintDT 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
// CJetPrintDTApp:
// 有关此类的实现，请参阅 JetPrintDT.cpp
//
extern TCHAR g_strKSCoreAppName[];

class CJetPrintDTDocTemplate : public CSingleDocTemplate
{
	DECLARE_DYNAMIC(CJetPrintDTDocTemplate)
	// Constructors
public:
	CJetPrintDTDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass,
		int nSplitterID = -1, int nRow = 0, int nCol = 0);

	// Attributes
public:
	int m_nSplitterID;	// -1, no splitter else splitter ID (0, 1,...)
	int m_nRow;			// if splitter, row number (0, 1,...)
	int m_nCol;			// if splitter, col number (0, 1,...)

	CView* m_pView;		// view pointer

	// these overrides to retreive the view class and resource ID
	CRuntimeClass* GetViewClass()
	{
		return m_pViewClass;
	}
	UINT GetResourceID()
	{
		return m_nIDResource;
	}
};

//CJetPrintDTWinApp
class CJetPrintDTWinApp : public CWinAppEx
{
	DECLARE_DYNAMIC(CJetPrintDTWinApp)
public:
	CJetPrintDTWinApp();
	CJetPrintDTDocTemplate* GetTemplate(CView* pView);
	CJetPrintDTDocTemplate* GetTemplate(CRuntimeClass *pViewClass);
	CJetPrintDTDocTemplate* GetFirstTemplate();

protected:
	//int GetInt(LPCTSTR lpszEntry, int nDefault = 0);
};


//CJetPrintDTApp
class CJetPrintDTApp : public CJetPrintDTWinApp
{
public:
	CJetPrintDTApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	enum { PUMP_PORT_NUMBER = 8 };
// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	bool OpenCameraModule();
};

extern CJetPrintDTApp theApp;
