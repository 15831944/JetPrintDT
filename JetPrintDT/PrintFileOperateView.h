#pragma once

#include "ResizableFormView.h"

#include "JetDXF/JetDxfRead.h"
#include "JetDXF/JetDxfDrawView.h"
#include "JetDXF/JetDxfWrite.h"
#include "JetBmp/JetBmpAnalysis.h"

#include "CustomeControl/JetCustomeEdit.h"
#include "CustomeControl/JetGroupBox.h"
// CPrintFileOperateView 窗体视图

#include "JetDownloadFileThread.h"
#include "CustomeControl\JetGroupBox.h"
#include "CustomeControl\JetCustomeEdit.h"
#include "afxwin.h"
#include "JetDXF\JetDxfDrawView.h"
#include "PrintMatchView.h"
#define DOUBLE_ERROR 0.0000001

class CPrintFileOperateView : public CResizableFormView
{
	DECLARE_DYNCREATE(CPrintFileOperateView)

protected:
	CPrintFileOperateView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPrintFileOperateView();

public:
	enum { IDD = IDD_JETPRINTDT_PRINT_FILEOPERATE_DIALOG };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CJetDxfRead m_JetDxfRead;
	CJetDxfWrite m_JetDxfWriteProg;

	//bmp
	double m_dbImageHeight;
	double m_dbImageWidth;
	double m_dbSpace;
	int    m_Threshold;

	//dxf
	double m_dbDXFImageOrignWidth;
	double m_dbDXFImageOrignHeight;
	double m_dbDXFImageDestWidth;
	double m_dbDXFImageDestHeight;
	double m_dbxzoomfactor;
	double m_dbyzoomfactor;
	//file load
	bool m_bFileOpenedOK;
	
	CString m_CurrentFileName;
	CString m_CurrentFileExt;
	CString m_strFilePath;
	CString m_strFileName;

	double db_XOrignPos;        //x print orign pos
	double db_YOrignPos;		//y print orign pos	

	CString m_CurrentDownloadFile;
public:
	CJetBmpAnalysis m_JetBmpAnalysis;
	CJetDxfDrawView m_JetDrawView;
	bool m_bIsFileDownloading;
	bool m_bIsFileDownloadSucceed;
	CJetDownloadFileThread *m_pDownLoadFileThread;
	CPrintMatchView *m_pMatchView;
private:
	CString SetProgPath();
	CString GetCurrentFolder();
	bool LoadAndImportFile(CString FileName);
	bool AdjustScreen(CRect &rc_Orign, CRect &rc_Inst);
	void UpdateFileList();
	void DownloadFile();
	CString GetModuleDir();
	BOOL ReadConfig();
	BOOL WriteConfig();
	bool IsEqual(double dbLeft, double dbRight);
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnBnClickedJetprintdtPrintFileoperateDxfDecodeButton();
	afx_msg void OnBnClickedJetprintdtPrintFileoperateBmpParamsetButton();
	afx_msg void OnBnClickedJetprintdtPrintFileoperateFileFileopenButton();
	afx_msg void OnBnClickedJetprintdtPrintFileoperateFileFiledownloadButton();
	afx_msg void OnBnClickedJetprintdtPrintFileoperatePrintorignButton();
	afx_msg void OnBnClickedJetprintdtPrintFileoperateMatchprintButton();
	CString GetCurrentFileName();
	CString GetCurrentFileExt();
	void AnalysisAndDraw();     //解析并显示文件
	void ExitDownloadUIThread();
	void UpdatePrintOrignPos(double dbXPos, double dbYPos);
	void MatchPrintDecodeFile(double dbXOrignPos, double dbYOrignPos,double dbXJet,double dbYJet);
};


