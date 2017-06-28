// MainFrm.h : CMainFrame 类的接口
//
#pragma once
#include "Resource.h"
#include "JetPrintDTFrame.h"
#include "PrintParamSetView.h"
#include "PrintCameraView.h"

#define MIN_VOL  1.0e-3

typedef enum VIEW_TYPE{PARAMSET_VIEW = 3,MOTIONCONTROL_VIEW = 2,FILEOPERATE_VIEW = 1,PRINTCONTROL_VIEW = 0};

class COutlookBar : public CMFCOutlookBar
{
public:
	virtual BOOL AllowShowOnPaneMenu() const { return FALSE; }
	virtual void GetPaneName(CString& strName) const { BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR); ASSERT(bNameValid); if (!bNameValid) strName.Empty(); }
	virtual BOOL CanBeResized() const { return FALSE; }
	virtual BOOL OnBeforeAnimation(int nPage){
		 return CMFCOutlookBar::OnBeforeAnimation(nPage);
	}
};

class CNoMenuApplicationButton :public CMFCRibbonApplicationButton{
public:
	CNoMenuApplicationButton();
	~CNoMenuApplicationButton();

	virtual void OnLButtonDown(CPoint point);
	virtual void OnLButtonDblClk(CPoint point);
};

class CMainFrame : public CJetPrintDTFrame
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:
	enum { TIME_CHECK_STATUS = 1, TIME_CHECK_TEMP = 2,TIME_CHECK_DEVICE = 3,TIME_DELAY_GATHER = 200 };
// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CMFCCaptionBar    m_wndCaptionBar;
	
	VIEW_TYPE m_PrintViewType;
	int index;
// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnRibbonCategoryChanged(WPARAM, LPARAM);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	virtual void ActivateFrame(int nCmdShow = -1);
	afx_msg void OnNcPaint();
	DECLARE_MESSAGE_MAP()

	BOOL CreateCaptionBar();

	int FindFocusedOutlookWnd(CMFCOutlookBarTabCtrl** ppOutlookWnd);
	void ResizeSplitterSize(int cx,int cy);
	CMFCOutlookBarTabCtrl* FindOutlookParent(CWnd* pWnd);
	CMFCOutlookBarTabCtrl* m_pCurrOutlookWnd;
	CMFCOutlookBarPane*    m_pCurrOutlookPage;

	
public:

private:
	void SwitchMainView(int nChioce);
	
public:
	//
	afx_msg void OnClose();

	//Print 
	afx_msg void OnPrintParamsetButton();
	afx_msg void OnUpdatePrintParamsetButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintMotioncontrolButton();
	afx_msg void OnUpdatePrintMotioncontrolButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintFileoperateButton();
	afx_msg void OnUpdatePrintFileoperateButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintPrintcontrolButton();
	afx_msg void OnUpdatePrintPrintcontrolButton(CCmdUI *pCmdUI);
	
	
	
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	
	afx_msg void OnPrintStartgrabButton();
	afx_msg void OnUpdatePrintStartgrabButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintStopgrabButton();
	afx_msg void OnUpdatePrintStopgrabButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintSaveimageButton();
	afx_msg void OnUpdatePrintSaveimageButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintZoomoutButton();
	afx_msg void OnUpdatePrintZoomoutButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintZoominButton();
	afx_msg void OnUpdatePrintZoominButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintShoworignButton();
	afx_msg void OnUpdatePrintShoworignButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintTurnonlightButton();
	afx_msg void OnUpdatePrintTurnonlightButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintTurnofflightButton();
	afx_msg void OnUpdatePrintTurnofflightButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintDisplayscaleButton();
	afx_msg void OnUpdatePrintDisplayscaleButton(CCmdUI *pCmdUI);
	
	afx_msg void OnPrintLightLuminanceSlider();
	afx_msg void OnUpdatePrintLightLuminanceSlider(CCmdUI *pCmdUI);

	afx_msg LRESULT OnStatusChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateGridData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitializeFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitDestroy(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetOutput(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFreqChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void StartCheckStatus();
	void StopCheckStatus();
	void StartWatchParkerStatus();
	double ReadDCVoltage();
	void UpdateWindowState();
	void StartCheckTemp();
	void StopCheckTemp();
	void UpdateLightSlider();
	void UpdateTempSlider();

	afx_msg void OnPrintSwitchcameraButton();
	afx_msg void OnUpdatePrintSwitchcameraButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintTurnonheatButton();
	afx_msg void OnUpdatePrintTurnonheatButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintTemperatureAdjustSlider();
	afx_msg void OnUpdatePrintTemperatureAdjustSlider(CCmdUI *pCmdUI);
	afx_msg void OnPrintTurnonpresurreButton();
	afx_msg void OnUpdatePrintTurnonpresurreButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintTurnoffpressureButton();
	afx_msg void OnUpdatePrintTurnoffpressureButton(CCmdUI *pCmdUI);
	afx_msg void OnPrintTurnoffheatButton();
	afx_msg void OnUpdatePrintTurnoffheatButton(CCmdUI *pCmdUI);
};


