// JetPrintDT.h : JetPrintDT Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
// CJetPrintDTApp:
// �йش����ʵ�֣������ JetPrintDT.cpp
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


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	enum { PUMP_PORT_NUMBER = 8 };
// ʵ��
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
