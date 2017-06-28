#pragma once

#include "ResizableFormView.h"
#include "JetPanelWnd.h"
#include "JetPrintGridCtrl\GridCtrl.h"

// CPrintMotionControlView 窗体视图

class CPrintMotionControlView : public CResizableFormView
{
	DECLARE_DYNCREATE(CPrintMotionControlView)

protected:
	CPrintMotionControlView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPrintMotionControlView();

public:
	enum { IDD = IDD_JETPRINTDT_PRINT_MOTIONCONTROL_DIALOG };
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
	CJetPanelWnd m_JetPanelWnd;
	enum { TIME_UPDATE_POS = 1, TIME_CHECK_STATE = 2 };
	enum { KILL_ALL_MOVE = 2, AXIS_ZERO_KILL = 0, AXIS_ONE_KILL = 1 };

	double db_XTargetPos;
	double db_YTargetPos;
	double db_XPos;
	double db_YPos;
	double db_XStepLenth;
	double db_YStepLenth;

	CGridCtrl m_MotionGridData;
	int m_nSelectedRow;
	bool m_bSuspendThread;
	int m_iDisCalBtnStateFlag;          //间距标定状态标识
	int m_iErrCalBtnStateFlag;			//误差标定状态标识
	float m_fSprayerCenPos_DisCal[2];   //间距标定 喷头中心位置
	float m_fCameraCenPos_DisCal[2];    //间距标定 相机中心位置
	float m_fCenDisSprayerAndCamera[2];	//喷头中心和相机中心距离
	float m_fSprayerCenPos_ErrCal[2];	//误差标定 喷头(墨点)中心位置
	float m_fCameraCenPos_ErrCal[2];	//误差标定 相机中心位置
	float m_fPrintErrOffset[2];			//打印误差补偿偏移

	//public members
public:
	CWinThread *m_pMotionCheckThread;
	HANDLE  m_hMotionEvent;
	double db_XPrintOrignPos;
	double db_YPrintOrignPos;
	double m_dbToLocateCameraXOffset;
	double m_dbToLocateCameraYOffset;
	bool m_bMotionThreadRun;
	//private member funcs
private:
	bool InitSimulatePanel();
	bool InitializeDataGrid();
	void InitializeParam();
	void SetDataGrid();
	CString GetProgPath();
	CString GetModuleDir();

	void EnableCheckThread();
	void CheckStatus(double xPos, double yPos);
	static UINT CheckMotionStatus(LPVOID pVoid);
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionTargetpossetButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionSteplenthsetButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionStepbackwardButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionStepleftButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionSteprightButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionStepforwardButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionLocatecenterCalibrateButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionLocatedistCalibrateButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionPrinterrorCalibrateButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionSetPrintorignposButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionMoveHomeButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionMovetoPrintorignposButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolMotionMovetoLocatecenterButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolDataApplyButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolDataAppendButton();
	afx_msg void OnBnClickedJetprintdtMotioncontrolDataDeleteButton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	LRESULT OnChangePosition(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeStepLenth(WPARAM wParam, LPARAM lParam);
	LRESULT OnMotionStatusChange(WPARAM wParam, LPARAM lParam);
	LRESULT OnHomeFailed(WPARAM wParam, LPARAM lParam);
	LRESULT OnMotionControlChange(WPARAM wParam, LPARAM lParam);
	LRESULT OnPeletteJog(WPARAM wParam, LPARAM lParam);
	LRESULT OnPeletteStopJog(WPARAM wParam, LPARAM lParam);
	void OnGridClick(NMHDR *pNotifyStruct, LRESULT *);

	void SuspendMotionStatusThread();
	void ResumeMotionStatusThread();
	void SetCurrentPos(double dbXpos, double dbYpos);
	void GetCurrentPos(float &fXpos, float &fYpos);
	
	void UpdateControlState();
	double GetXStepLenth(){ return db_XStepLenth; }
	double GetYStepLenth(){ return db_YStepLenth; }

	void SetStepLenth(double dbXStep, double dbYStep);
	void UpdateStepLenth(double dbXStepLenth, double dbYStepLenth);

	bool TerminateMotionThread();
	void UpdateLocateCameraPos(double dbXPos, double dbYPos);
};


