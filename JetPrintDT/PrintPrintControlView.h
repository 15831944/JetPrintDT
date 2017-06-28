#pragma once
#include "JetDXF/JetDxfRead.h"
#include "JetDXF/JetDxfDrawView.h"
#include "ResizableFormView.h"
#include "JetPrintGridCtrl\GridCtrl.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartCtrl.h"
#include "ChartCtrl/ChartAxis.h"
#include "ChartCtrl/ChartPointsSerie.h"

#include "JetDownloadFileThread.h"
#include "WaitMessageThread.h"
#include "JetPrintPlacodeModifyDlg.h"

// CPrintPrintControlView 窗体视图

class CPrintPrintControlView : public CResizableFormView
{
	DECLARE_DYNCREATE(CPrintPrintControlView)

protected:
	CPrintPrintControlView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPrintPrintControlView();

public:
	enum { IDD = IDD_JETPRINTDT_PRINT_PRINTCONTROL_DIALOG };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	//private members
private:
	typedef enum{ STATE_NULL = 0, STATE_RUN = 1, STATE_STOPED = 2 }PROG_STATE;

	CJetDxfDrawView m_JetDrawView;

	CChartCtrl m_ChartCtrl;
	CChartPointsSerie *m_pPrintPoint;

	CGridCtrl m_JetPrintGridList;

	double m_dbXSpeed;
	double m_dbYSpeed;

	double m_dbXPosition;
	double m_dbYPosition;

	CString m_strCurrentFileExt;
	bool m_bSetAgilentOutput;
	enum { TIME_PRINT = 0, TIME_UPDATE_STATE = 2 };
	bool m_bRunThreadSuspended;
	HANDLE m_hRunProgEvent;

	bool m_bIsParamIntialized;

	double m_dbPlacodeXLenth;
	double m_dbPlacodeYLenth;

	double m_dbXOffSet;
	double m_dbYOffSet;
	BORDERTYPE m_PlacodeType;
	bool m_bEnableUpdateView;
	PROG_STATE m_ProgState;

	//public members
public:
	CWinThread *m_pRunProgThread;
	bool m_bRunProgThread;
	CComboBox m_PlacodeCombo;
	CWaitMessageThread * m_pWaitMessageThread;
	CJetPrintPlacodeModifyDlg m_PlacodeModifyDlg;
	bool m_bStopPrintFlag;
	bool m_bProgRunning;
	bool m_bCheckProgState;
	
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	//private member func
private:
	void InitializeTrailGraph();
	void InitializeDeviceGraph();
	void InitializeImageGraph();
	CString GetProgPath();
	void InitializeCombo();
	static UINT StartPrint(LPVOID pVoid);
	void UpdateView(int nType);
	void UpdateGridList();

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedJetprintdtPrintPrintcontrolSetspeedButton();
	afx_msg void OnBnClickedJetprintdtPrintPrintcontrolStartprintButton();
	afx_msg void OnBnClickedJetprintdtPrintPrintcontrolStopprintButton();
	afx_msg void OnBnClickedJetprintdtPrintPrintcontrolHomeButton();
	afx_msg LRESULT OnProgFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFileLoaded(WPARAM wParam, LPARAM lParam);

	afx_msg void OnSelchangeJetPrintPlacodeTypeCombo();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void UpdatePosition(double xPos, double yPos);
	void UpdateSpeed(double xSpeed, double ySpeed);
	void SetXMoveSpeed(double xSpeed);
	void SetYMoveSpeed(double ySpeed);
	void InitializePrintParker();
	void DestroyPrintParker();
	double GetXMoveSpeed();
	double GetYMoveSpeed();
	void SetFileExt(CString fileExt);
	void UpdateDeviceStatus();
	void UpdateOrignPos();
	void ExitWatchPrintThread();
	void ExitWaitMessageUIThread();
	virtual void OnDraw(CDC* /*pDC*/);
	
};


