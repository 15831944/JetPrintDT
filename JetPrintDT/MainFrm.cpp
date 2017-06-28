// MainFrm.cpp : CMainFrame 类的实现
//
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "JetPrintDT.h"
#include "MainFrm.h"
#include "assert.h"
#include "PrintMotionControlView.h"
#include "PrintFileOperateView.h"
#include "PrintPrintControlView.h"
#include "Device/DeviceParker.h"
#include "Device/DeviceADModule.h"
#include "Device/DeviceHarvardPump.h"
#include "Device/DeviceVisa.h"
#include "Device/DeviceCamera.h"
#include "Device/DeviceWatchLightPort.h"
#include "Device/DeviceLocateLightPort.h"
#include "Device/DeviceHeatModule.h"
#include "SingleTon/CameraSharedBuffer.h"
#include "SingleTon/ParamInitSingleton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CNoMenuApplicationButton::CNoMenuApplicationButton(){

}

CNoMenuApplicationButton::~CNoMenuApplicationButton(){

}

void CNoMenuApplicationButton::OnLButtonDown(CPoint point){
	return;
}

void CNoMenuApplicationButton::OnLButtonDblClk(CPoint point){
	return;
}
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CJetPrintDTFrame)

BEGIN_MESSAGE_MAP(CMainFrame, CJetPrintDTFrame)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_REGISTERED_MESSAGE(AFX_WM_ON_CHANGE_RIBBON_CATEGORY, OnRibbonCategoryChanged)



	ON_COMMAND(ID_PRINT_PARAMSET_BUTTON, &CMainFrame::OnPrintParamsetButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_PARAMSET_BUTTON, &CMainFrame::OnUpdatePrintParamsetButton)
	ON_COMMAND(ID_PRINT_MOTIONCONTROL_BUTTON, &CMainFrame::OnPrintMotioncontrolButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_MOTIONCONTROL_BUTTON, &CMainFrame::OnUpdatePrintMotioncontrolButton)
	ON_COMMAND(ID_PRINT_FILEOPERATE_BUTTON, &CMainFrame::OnPrintFileoperateButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_FILEOPERATE_BUTTON, &CMainFrame::OnUpdatePrintFileoperateButton)
	ON_COMMAND(ID_PRINT_PRINTCONTROL_BUTTON, &CMainFrame::OnPrintPrintcontrolButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_PRINTCONTROL_BUTTON, &CMainFrame::OnUpdatePrintPrintcontrolButton)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCHITTEST()
	ON_WM_NCPAINT()


	ON_COMMAND(ID_FILE_SAVE, &CMainFrame::OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CMainFrame::OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CMainFrame::OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CMainFrame::OnUpdateFileSaveAs)

	ON_COMMAND(ID_PRINT_STARTGRAB_BUTTON, &CMainFrame::OnPrintStartgrabButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_STARTGRAB_BUTTON, &CMainFrame::OnUpdatePrintStartgrabButton)
	ON_COMMAND(ID_PRINT_STOPGRAB_BUTTON, &CMainFrame::OnPrintStopgrabButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_STOPGRAB_BUTTON, &CMainFrame::OnUpdatePrintStopgrabButton)
	ON_COMMAND(ID_PRINT_SAVEIMAGE_BUTTON, &CMainFrame::OnPrintSaveimageButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_SAVEIMAGE_BUTTON, &CMainFrame::OnUpdatePrintSaveimageButton)
	ON_COMMAND(ID_PRINT_ZOOMOUT_BUTTON, &CMainFrame::OnPrintZoomoutButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_ZOOMOUT_BUTTON, &CMainFrame::OnUpdatePrintZoomoutButton)
	ON_COMMAND(ID_PRINT_ZOOMIN_BUTTON, &CMainFrame::OnPrintZoominButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_ZOOMIN_BUTTON, &CMainFrame::OnUpdatePrintZoominButton)
	ON_COMMAND(ID_PRINT_SHOWORIGN_BUTTON, &CMainFrame::OnPrintShoworignButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_SHOWORIGN_BUTTON, &CMainFrame::OnUpdatePrintShoworignButton)
	ON_COMMAND(ID_PRINT_TURNONLIGHT_BUTTON, &CMainFrame::OnPrintTurnonlightButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_TURNONLIGHT_BUTTON, &CMainFrame::OnUpdatePrintTurnonlightButton)
	ON_COMMAND(ID_PRINT_TURNOFFLIGHT_BUTTON, &CMainFrame::OnPrintTurnofflightButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_TURNOFFLIGHT_BUTTON, &CMainFrame::OnUpdatePrintTurnofflightButton)
	ON_COMMAND(ID_PRINT_DISPLAYSCALE_BUTTON, &CMainFrame::OnPrintDisplayscaleButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_DISPLAYSCALE_BUTTON, &CMainFrame::OnUpdatePrintDisplayscaleButton)
	ON_WM_CLOSE()
	ON_COMMAND(ID_PRINT_LIGHT_LUMINANCE_SLIDER, &CMainFrame::OnPrintLightLuminanceSlider)
	ON_UPDATE_COMMAND_UI(ID_PRINT_LIGHT_LUMINANCE_SLIDER, &CMainFrame::OnUpdatePrintLightLuminanceSlider)
	ON_WM_TIMER()

	ON_MESSAGE(WM_STATUS_MESSAGE, &CMainFrame::OnStatusChange)
	ON_MESSAGE(WM_SET_OUTPUT, &CMainFrame::OnSetOutput)
	ON_MESSAGE(WM_FREQ_CHANGED, &CMainFrame::OnFreqChanged)
	ON_COMMAND(ID_PRINT_SWITCHCAMERA_BUTTON, &CMainFrame::OnPrintSwitchcameraButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_SWITCHCAMERA_BUTTON, &CMainFrame::OnUpdatePrintSwitchcameraButton)
	ON_COMMAND(ID_PRINT_TURNONHEAT_BUTTON, &CMainFrame::OnPrintTurnonheatButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_TURNONHEAT_BUTTON, &CMainFrame::OnUpdatePrintTurnonheatButton)
	ON_COMMAND(ID_PRINT_TEMPERATURE_ADJUST_SLIDER, &CMainFrame::OnPrintTemperatureAdjustSlider)
	ON_UPDATE_COMMAND_UI(ID_PRINT_TEMPERATURE_ADJUST_SLIDER, &CMainFrame::OnUpdatePrintTemperatureAdjustSlider)
	ON_COMMAND(ID_PRINT_TURNONPRESURRE_BUTTON, &CMainFrame::OnPrintTurnonpresurreButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_TURNONPRESURRE_BUTTON, &CMainFrame::OnUpdatePrintTurnonpresurreButton)
	ON_COMMAND(ID_PRINT_TURNOFFPRESSURE_BUTTON, &CMainFrame::OnPrintTurnoffpressureButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_TURNOFFPRESSURE_BUTTON, &CMainFrame::OnUpdatePrintTurnoffpressureButton)
	ON_COMMAND(ID_PRINT_TURNOFFHEAT_BUTTON, &CMainFrame::OnPrintTurnoffheatButton)
	ON_UPDATE_COMMAND_UI(ID_PRINT_TURNOFFHEAT_BUTTON, &CMainFrame::OnUpdatePrintTurnoffheatButton)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO:  在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
	m_PrintViewType = PRINTCONTROL_VIEW;
	index = 0;
}

CMainFrame::~CMainFrame()
{

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CJetPrintDTFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	CMFCRibbonApplicationButton *pApplicationButton = m_wndRibbonBar.GetApplicationButton();
	delete pApplicationButton;

	CNoMenuApplicationButton* pNoMenuApplicationButton = new CNoMenuApplicationButton;
	pNoMenuApplicationButton->SetImage(IDB_MAIN);
	m_wndRibbonBar.SetApplicationButton(pNoMenuApplicationButton, CSize(45, 45));

	CMFCRibbonQuickAccessToolBar *pToolBar = m_wndRibbonBar.GetQuickAccessToolbar();
	pToolBar->RemoveAll();

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 导航窗格将创建在左侧，因此将暂时禁用左侧的停靠: 
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);


	// 创建标题栏: 
	if (!CreateCaptionBar())
	{
		TRACE0("未能创建标题栏\n");
		return -1;      // 未能创建
	}

	HICON m_hIcon = AfxGetApp()->LoadIcon(IDR_MAIN_ICON);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// 已创建 Outlook 栏，应允许在左侧停靠。
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CJetPrintDTFrame::PreCreateWindow(cs))
		return FALSE;
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("未能创建标题栏\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	m_wndCaptionBar.ShowWindow(SW_HIDE);

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CJetPrintDTFrame::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CJetPrintDTFrame::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	///m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	m_wndCaptionBar.ShowWindow(FALSE);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}


void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // 强制关闭“打印预览”模式
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

void CMainFrame::ResizeSplitterSize(int cx, int cy){
	if (cx == 0 || cy == 0)
		return;

	CJetPrintDTApp* pApp = (CJetPrintDTApp*)AfxGetApp();
	ASSERT_KINDOF(CJetPrintDTApp, pApp);

	POSITION TmpPos = pApp->GetFirstDocTemplatePosition();

	POSITION MapPos = m_SplitInfo.GetStartPosition();
	while (MapPos)
	{
		int nID = 0;
		int tempCx = cx;
		CJetPrintDTSplitInfo SplitInfo;
		m_SplitInfo.GetNextAssoc(MapPos, nID, SplitInfo);
		if (m_SplitInfo[nID].m_maxRow > 0 && m_SplitInfo[nID].m_maxCol > 0)
		{

			cy = cy - 56;
			if (cy < 56)
				cy = 0;
			//Outlook Bar
			if (tempCx < 0)
				tempCx = 0;

			if (m_SplitInfo[nID].m_maxCol)
				tempCx /= m_SplitInfo[nID].m_maxCol;
			if (m_SplitInfo[nID].m_maxRow)
				cy /= m_SplitInfo[nID].m_maxRow;

			TmpPos = pApp->GetFirstDocTemplatePosition();
			while (TmpPos)
			{
				CJetPrintDTDocTemplate* pTemplate = (CJetPrintDTDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
				if (pTemplate->m_pView){
					if (pTemplate->m_nSplitterID == nID)
					{
						// create view
						int nRow = pTemplate->m_nRow, nCol = pTemplate->m_nCol;

						if (m_SplitInfo[nID].m_wndSplitter.m_hWnd)
							m_SplitInfo[nID].m_wndSplitter.SetColumnInfo(nCol, tempCx, tempCx / 2);
					}
				}

			}
		}
	}
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CJetPrintDTFrame::OnCreateClient(lpcs, pContext);
}


void CMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CJetPrintDTFrame::OnMouseMove(nFlags, point);
}


void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CJetPrintDTFrame::OnLButtonDown(nFlags, point);
}


void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CJetPrintDTFrame::OnLButtonUp(nFlags, point);
}


LRESULT CMainFrame::OnRibbonCategoryChanged(WPARAM, LPARAM){
	CMFCRibbonCategory *pCategory = m_wndRibbonBar.GetActiveCategory();
	int nCategory = m_wndRibbonBar.GetCategoryIndex(pCategory);

	SwitchMainView(nCategory);

	return 0;
}

void CMainFrame::SwitchMainView(int nChioce){
	assert(nChioce > 0);
	assert(nChioce < 6);
	switch (nChioce)
	{
	case 1:
		break;

	case 2:
		break;

	case 3:
		break;

	case 4:{

			   ActivateView(RUNTIME_CLASS(CPrintCameraView));
			   m_SplitInfo[0].m_wndSplitter.SwitchToView(LEFT_SIDE, m_PrintViewType);
			   //UpdateWindow();

			   break;
	}
	default:
		break;
	}


}

void CMainFrame::OnPrintParamsetButton()
{
	// TODO:  在此添加命令处理程序代码
	m_PrintViewType = PARAMSET_VIEW;
	m_SplitInfo[0].m_wndSplitter.SwitchToView(LEFT_SIDE, PARAMSET_VIEW);
}


void CMainFrame::OnUpdatePrintParamsetButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_PrintViewType == PARAMSET_VIEW);
}


void CMainFrame::OnPrintMotioncontrolButton()
{
	// TODO:  在此添加命令处理程序代码
	m_PrintViewType = MOTIONCONTROL_VIEW;
	m_SplitInfo[0].m_wndSplitter.SwitchToView(LEFT_SIDE, MOTIONCONTROL_VIEW);
}


void CMainFrame::OnUpdatePrintMotioncontrolButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_PrintViewType == MOTIONCONTROL_VIEW);
}


void CMainFrame::OnPrintFileoperateButton()
{
	// TODO:  在此添加命令处理程序代码
	m_PrintViewType = FILEOPERATE_VIEW;
	m_SplitInfo[0].m_wndSplitter.SwitchToView(LEFT_SIDE, FILEOPERATE_VIEW);
}


void CMainFrame::OnUpdatePrintFileoperateButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_PrintViewType == FILEOPERATE_VIEW);
}


void CMainFrame::OnPrintPrintcontrolButton()
{
	// TODO:  在此添加命令处理程序代码
	m_PrintViewType = PRINTCONTROL_VIEW;
	m_SplitInfo[0].m_wndSplitter.SwitchToView(LEFT_SIDE, PRINTCONTROL_VIEW);
}


void CMainFrame::OnUpdatePrintPrintcontrolButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_PrintViewType == PRINTCONTROL_VIEW);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CJetPrintDTFrame::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码

	ResizeSplitterSize(cx, cy);
}


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);

	if (lpMMI->ptMinTrackSize.x <= cx)
	{
		lpMMI->ptMinTrackSize.x = cx;
	}

	if (lpMMI->ptMinTrackSize.y <= cy)
	{
		lpMMI->ptMinTrackSize.y = cy;
	}


	CJetPrintDTFrame::OnGetMinMaxInfo(lpMMI);
}


void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	//CJetPrintDTFrame::OnLButtonDblClk(nFlags, point);
}


void CMainFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nHitTest != HTCAPTION)
		CJetPrintDTFrame::OnNcLButtonDblClk(nHitTest, point);
	//CJetPrintDTFrame::OnNcLButtonDblClk(nHitTest, point);
}


void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	//if (nHitTest != HTCAPTION)
	CJetPrintDTFrame::OnNcLButtonDown(nHitTest, point);
}


LRESULT CMainFrame::OnNcHitTest(CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (CWnd::OnNcHitTest(point) == HTRIGHT \
		|| CWnd::OnNcHitTest(point) == HTLEFT \
		|| CWnd::OnNcHitTest(point) == HTTOP \
		|| CWnd::OnNcHitTest(point) == HTBOTTOM\
		|| CWnd::OnNcHitTest(point) == HTTOPLEFT\
		|| CWnd::OnNcHitTest(point) == HTTOPRIGHT\
		|| CWnd::OnNcHitTest(point) == HTBOTTOMLEFT\
		|| CWnd::OnNcHitTest(point) == HTBOTTOMRIGHT)
		return FALSE;

	//return CWnd::OnNcHitTest(point);
	return CJetPrintDTFrame::OnNcHitTest(point);
}


void CMainFrame::ActivateFrame(int nCmdShow)
{
	// TODO:  在此添加专用代码和/或调用基类
	nCmdShow = SW_SHOWMAXIMIZED;
	CJetPrintDTFrame::ActivateFrame(nCmdShow);
}


void CMainFrame::OnNcPaint()
{
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用
	ShowWindow(SW_SHOWMAXIMIZED);
	CJetPrintDTFrame::OnNcPaint();
}


void CMainFrame::OnFileSave()
{
	// TODO:  在此添加命令处理程序代码
	CMFCRibbonCategory *pCategory = m_wndRibbonBar.GetActiveCategory();
	int nCategory = m_wndRibbonBar.GetCategoryIndex(pCategory);

	//DXF View
	if (nCategory == 1){

	}

	//BMP View
	if (nCategory == 2){

	}
}


void CMainFrame::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	CMFCRibbonCategory *pCategory = m_wndRibbonBar.GetActiveCategory();
	int nCategory = m_wndRibbonBar.GetCategoryIndex(pCategory);
	bool bEnable = true;

	if (nCategory == 1 || nCategory == 2){
		pCmdUI->SetCheck(bEnable);
	}
	else{
		bEnable = false;
		pCmdUI->SetCheck(bEnable);
	}


}


void CMainFrame::OnFileSaveAs()
{
	// TODO:  在此添加命令处理程序代码
}


void CMainFrame::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
}


void CMainFrame::OnPrintStartgrabButton()
{
	// TODO:  在此添加命令处理程序代码
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){
		pCamera->CameraStartGrab();
	}

	Sleep(1000);
	CPrintCameraView *pCameraView = (CPrintCameraView*)(m_SplitInfo[0].m_wndSplitter.m_views[RIGHT_SIDE][0]);
	if (pCameraView){
		pCameraView->StartGrabImage();
		pCameraView->m_bGrabImage = true;
	}
}


void CMainFrame::OnUpdatePrintStartgrabButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	bool bEnabled = false;
	if (pCamera){
		bEnabled = pCamera->GetLocateCameraStatus() || pCamera->GetWatchCameraStatus();
	}

	pCmdUI->Enable(bEnabled);
}


void CMainFrame::OnPrintStopgrabButton()
{
	// TODO:  在此添加命令处理程序代码
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){
		pCamera->CameraStopGrab();
	}

	CPrintCameraView *pCameraView = (CPrintCameraView*)(m_SplitInfo[0].m_wndSplitter.m_views[RIGHT_SIDE][0]);
	if (pCameraView){
		pCameraView->m_bGrabImage = false;
		pCameraView->StopGrabImage();
	}
}


void CMainFrame::OnUpdatePrintStopgrabButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
}


void CMainFrame::OnPrintSaveimageButton()
{
	// TODO:  在此添加命令处理程序代码
	CFileDialog dlg(
		false,			// SaveAs
		TEXT("bmp"),	// bitmap-files
		0,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		TEXT("Windows Bitmap-File (*.bmp)|*.bmp|All Files (*.*)|*.*||"),
		this
		);
	if (IDOK == dlg.DoModal()){
		char szPath[255] = { 0 };
		strcpy(szPath, CW2A(dlg.GetPathName().GetBuffer()));

		CPrintCameraView *pCameraView = (CPrintCameraView*)(m_SplitInfo[0].m_wndSplitter.m_views[RIGHT_SIDE][0]);
		if (pCameraView)
			pCameraView->SaveImage(szPath);
	}
}


void CMainFrame::OnUpdatePrintSaveimageButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码

}


void CMainFrame::OnPrintZoomoutButton()
{
	// TODO:  在此添加命令处理程序代码
	CPrintCameraView *pCameraView = (CPrintCameraView*)(m_SplitInfo[0].m_wndSplitter.m_views[RIGHT_SIDE][0]);
	if (pCameraView)
		pCameraView->ZoomOutImage();
}


void CMainFrame::OnUpdatePrintZoomoutButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
}


void CMainFrame::OnPrintZoominButton()
{
	// TODO:  在此添加命令处理程序代码
	CPrintCameraView *pCameraView = (CPrintCameraView*)(m_SplitInfo[0].m_wndSplitter.m_views[RIGHT_SIDE][0]);
	if (pCameraView)
		pCameraView->ZoomInImage();
}


void CMainFrame::OnUpdatePrintZoominButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
}


void CMainFrame::OnPrintShoworignButton()
{
	// TODO:  在此添加命令处理程序代码
	CPrintCameraView *pCameraView = (CPrintCameraView*)(m_SplitInfo[0].m_wndSplitter.m_views[RIGHT_SIDE][0]);
	if (pCameraView)
		pCameraView->ShowOrign();
}


void CMainFrame::OnUpdatePrintShoworignButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
}


void CMainFrame::OnPrintTurnonlightButton()
{
	// TODO:  在此添加命令处理程序代码
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){
		if (pCamera->m_CurrentCameraType == WATCH_CAMERA){
			CDeviceWatchLightPort *pWatchLight = CDeviceWatchLightPort::Instance();
			if (pWatchLight){
				if (pWatchLight->IsPortOpen())
					pWatchLight->SendData(0, 1, pWatchLight->GetLightLuminence(), 1);
			}
		}

		if (pCamera->m_CurrentCameraType == LOCATE_CAMERA){
			CDeviceLocateLightPort *pLocateLight = CDeviceLocateLightPort::Instance();
			if (pLocateLight){
				if (pLocateLight->IsPortOpen())
					pLocateLight->SendData(0, 1, pLocateLight->GetLightLuminence(), 1);
			}
		}
	}
}


void CMainFrame::OnUpdatePrintTurnonlightButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){
		if (pCamera->m_CurrentCameraType == WATCH_CAMERA){
			CDeviceWatchLightPort *pWatchLight = CDeviceWatchLightPort::Instance();
			if (pWatchLight){
				pCmdUI->Enable(pWatchLight->IsPortOpen());
			}
		}

		if (pCamera->m_CurrentCameraType == LOCATE_CAMERA){
			CDeviceLocateLightPort *pLocateLight = CDeviceLocateLightPort::Instance();
			if (pLocateLight){
				pCmdUI->Enable(pLocateLight->IsPortOpen());
			}
		}

		if (pCamera->m_CurrentCameraType == NULL_CAMERA)
			pCmdUI->Enable(false);
	}
}


void CMainFrame::OnPrintTurnofflightButton()
{
	// TODO:  在此添加命令处理程序代码
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){
		if (pCamera->m_CurrentCameraType == WATCH_CAMERA){
			CDeviceWatchLightPort *pWatchLight = CDeviceWatchLightPort::Instance();
			if (pWatchLight){
				if (pWatchLight->IsPortOpen())
					pWatchLight->SendData(0, 2, pWatchLight->GetLightLuminence(), 1);
			}
		}

		if (pCamera->m_CurrentCameraType == LOCATE_CAMERA){
			CDeviceLocateLightPort *pLocateLight = CDeviceLocateLightPort::Instance();
			if (pLocateLight){
				if (pLocateLight->IsPortOpen())
					pLocateLight->SendData(0, 2, pLocateLight->GetLightLuminence(), 1);
			}
		}
	}
}


void CMainFrame::OnUpdatePrintTurnofflightButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){
		if (pCamera->m_CurrentCameraType == WATCH_CAMERA){
			CDeviceWatchLightPort *pWatchLight = CDeviceWatchLightPort::Instance();
			if (pWatchLight){
				pCmdUI->Enable(pWatchLight->IsPortOpen());
			}
		}

		if (pCamera->m_CurrentCameraType == LOCATE_CAMERA){
			CDeviceLocateLightPort *pLocateLight = CDeviceLocateLightPort::Instance();
			if (pLocateLight){
				pCmdUI->Enable(pLocateLight->IsPortOpen());
			}
		}

		if (pCamera->m_CurrentCameraType == NULL_CAMERA)
			pCmdUI->Enable(false);
	}
}


void CMainFrame::OnPrintDisplayscaleButton()
{
	// TODO:  在此添加命令处理程序代码
	CPrintCameraView *pCameraView = (CPrintCameraView*)(m_SplitInfo[0].m_wndSplitter.m_views[RIGHT_SIDE][0]);
	if (pCameraView)
		pCameraView->ShowScale();
}


void CMainFrame::OnUpdatePrintDisplayscaleButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	CPrintCameraView *pCameraView = (CPrintCameraView*)(m_SplitInfo[0].m_wndSplitter.m_views[RIGHT_SIDE][0]);
	pCmdUI->SetCheck(pCameraView->EnableScale());
}


void CMainFrame::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	if (AfxMessageBox(_T("正在打印中，是否停止打印并退出？"), MB_OKCANCEL) == IDOK){
		if (IDOK == AfxMessageBox(_T("为避免高压放电打坏喷头, 建议将喷头上移至适合高度！"), MB_OK)){
			//SaveFile
			CParamInitSingleton *pParam = CParamInitSingleton::Instance();
			if (pParam){
				pParam->WriteXMLFile();
			}
			
			//stop check temperature
			StopCheckTemp();

			//close temperature port
			CDeviceHeatModule *pHeatMoudle = CDeviceHeatModule::Instance();
			if (pHeatMoudle){
				pHeatMoudle->StopHeat();
				pHeatMoudle->UnInit();
			}

			//stop grab image
			OnPrintStopgrabButton();

			//close camera and exit
			CDeviceCamera *pCamera = CDeviceCamera::Instance();
			if (pCamera){
				pCamera->CameraExit();
			}

			//close light port
			CDeviceWatchLightPort *pWatchLight = CDeviceWatchLightPort::Instance();
			if (pWatchLight){
				if (pWatchLight->IsPortOpen())
					pWatchLight->SendData(0, 2, pWatchLight->GetLightLuminence(), 1);
			}

			CDeviceLocateLightPort *pLocateLight = CDeviceLocateLightPort::Instance();
			if (pLocateLight){
				if (pLocateLight->IsPortOpen())
					pLocateLight->SendData(0, 2, pLocateLight->GetLightLuminence(), 1);
			}


			//stop pump and close
			CDeviceHarvardPump *pPump = CDeviceHarvardPump::Instance();
			if (pPump){
				pPump->Stop();
				pPump->CloseHarvardPump();
			}

			//stop visa and close
			CDeviceVisa *pVisa = CDeviceVisa::Instance();
			if (pVisa){
				pVisa->SetOutput(FALSE);
				pVisa->DisConnectInstruments();
			}

			Sleep(200);

			//clear motion thread
			CPrintMotionControlView *pMotionView = (CPrintMotionControlView*)(m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW]);
			if (pMotionView->m_pMotionCheckThread){
				pMotionView->m_bMotionThreadRun = false;
				pMotionView->TerminateMotionThread();
			}

			//clear parker check thread
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			if (pParker)
				pParker->JetParkerTerminateWatchThread();

			Sleep(300);

			if (pParker){
				pParker->JetParkerSetAglient(false);

				Sleep(1000);

				pParker->JetParkerDisableMotor();
				pParker->JetParkerDisConnect();
			}
#endif
			CPrintFileOperateView *pFileView = (CPrintFileOperateView*)(m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW]);
			if (pFileView){
				pFileView->ExitDownloadUIThread();

				Sleep(100);
			}

			CPrintPrintControlView *pPrintView = (CPrintPrintControlView*)(m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][PRINTCONTROL_VIEW]);
			if (pPrintView){
				pPrintView->ExitWatchPrintThread();

				Sleep(100);

				pPrintView->ExitWaitMessageUIThread();
				Sleep(100);
			}

		}
		else
			return;
	}
	else
		return;

	::RemoveProp(AfxGetApp()->m_pMainWnd->m_hWnd, g_strKSCoreAppName);

	CJetPrintDTFrame::OnClose();
}


void CMainFrame::OnPrintLightLuminanceSlider()
{
	// TODO:  在此添加命令处理程序代码
	CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, m_wndRibbonBar.FindByID(ID_PRINT_LIGHT_LUMINANCE_SLIDER));
	int iPos = pSlider->GetPos();
	CString editText = _T("");
	editText.Format(_T("%4d"), iPos);

	CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_PRINT_LIGHT_LUMINANCE_EDIT));
	pEdit->SetEditText(editText);

	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){
		if (pCamera->m_CurrentCameraType == WATCH_CAMERA){
			CDeviceWatchLightPort *pWatchLight = CDeviceWatchLightPort::Instance();
			if (pWatchLight){
				if (pWatchLight->IsPortOpen())
					pWatchLight->SetLightLuminence(iPos);
				pWatchLight->SendData(0, 3, pWatchLight->GetLightLuminence(), 1);
			}
		}

		if (pCamera->m_CurrentCameraType == LOCATE_CAMERA){
			CDeviceLocateLightPort *pLocateLight = CDeviceLocateLightPort::Instance();
			if (pLocateLight){
				if (pLocateLight->IsPortOpen())
					pLocateLight->SetLightLuminence(iPos);
				pLocateLight->SendData(0, 3, pLocateLight->GetLightLuminence(), 1);
			}
		}
	}
}


void CMainFrame::OnUpdatePrintLightLuminanceSlider(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){
		if (pCamera->m_CurrentCameraType == WATCH_CAMERA){
			CDeviceWatchLightPort *pWatchLight = CDeviceWatchLightPort::Instance();
			if (pWatchLight){
				pCmdUI->Enable(pWatchLight->IsPortOpen());
			}
		}

		if (pCamera->m_CurrentCameraType == LOCATE_CAMERA){
			CDeviceLocateLightPort *pLocateLight = CDeviceLocateLightPort::Instance();
			if (pLocateLight){
				pCmdUI->Enable(pLocateLight->IsPortOpen());
			}
		}
		if (pCamera->m_CurrentCameraType == NULL_CAMERA)
			pCmdUI->Enable(false);
	}
}


LRESULT CMainFrame::OnStatusChange(WPARAM wParam, LPARAM lParam)
{
	STATUS_DATA *pStatusData = (STATUS_DATA*)wParam;
	float fSpeed = 0.0f;
	float fXPosition = 0.0f;
	float fYPosition = 0.0f;

	float fXMoveSpeed = 0.0f;
	float fYMoveSpeed = 0.0f;

	float fXJogSpeed = 0.0f;
	float fYJogSpeed = 0.0f;

	CString strSet = _T("");

	CPrintMotionControlView *pMotionView = (CPrintMotionControlView*)(m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW]);
	//参数标定界面的显示参数更新
	if (NULL != pMotionView)
	{
		fXPosition = pStatusData->m_fXPos;
		strSet.Format(_T("%.4f"), fXPosition);
		pMotionView->GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XCURRENTPOS_EDIT)->SetWindowText(strSet);

		fYPosition = pStatusData->m_fYPos;
		strSet.Format(_T("%.4f"), fYPosition);
		pMotionView->GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YCURRENTPOS_EDIT)->SetWindowText(strSet);

		pMotionView->SetCurrentPos(fXPosition, fYPosition);

	}

	CPrintPrintControlView *pPrintView = (CPrintPrintControlView*)(m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][PRINTCONTROL_VIEW]);
	if (NULL != pPrintView)
	{
		fXPosition = pStatusData->m_fXPos;
		fYPosition = pStatusData->m_fYPos;
		fXMoveSpeed = pStatusData->m_fXSpeed;
		fYMoveSpeed = pStatusData->m_fYSpeed;

		pPrintView->UpdatePosition(fXPosition, fYPosition);
		pPrintView->UpdateSpeed(fXMoveSpeed, fYMoveSpeed);
	}

	return 0;
}

LRESULT CMainFrame::OnSetOutput(WPARAM wParam, LPARAM lParam)
{

	CDeviceParker *pParker = nullptr;
#if _DEBUGE_PARKER
	pParker = CDeviceParker::Instance();

	if (lParam)
	{
		pParker->m_controlChannel->SetFlag(VISA_OUTPUT, VARIANT_TRUE, VARIANT_TRUE);
	}
	else
	{
		pParker->m_controlChannel->SetFlag(VISA_OUTPUT, VARIANT_FALSE, VARIANT_TRUE);
	}
#endif
	return 0;
}

LRESULT CMainFrame::OnFreqChanged(WPARAM wParam, LPARAM lParam)
{
	double dbFreq = (double)(lParam);
	double dwlTime = 1.0 / dbFreq*1.1;

	CString writeParam = _T("");
	writeParam.Format(_T("P1000 = %.2f \r"), dwlTime);
	BSTR bstrWrite = ::SysAllocString(writeParam);

#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	pParker->m_controlChannel->Write(bstrWrite);
#endif 
	::SysFreeString(bstrWrite);


	return 0;
}

double CMainFrame::ReadDCVoltage()
{
	char chCommand[8] = { 0 };
	chCommand[0] = 0x01;
	chCommand[1] = 0x04;
	chCommand[2] = 0x00;
	chCommand[3] = 0x00;
	chCommand[4] = 0x00;
	chCommand[5] = 0x01;
	chCommand[6] = 0x31;
	chCommand[7] = 0xCA;


	unsigned int lenth = 0;
	unsigned int data = 0;
	unsigned int temp = 0xff;
	double result = 0.0;

	CDeviceADModule *pADMoudle = CDeviceADModule::Instance();

	if (pADMoudle->m_comm.IsOpen())
	{
		lenth = pADMoudle->m_comm.WriteChar(8, chCommand);
		Sleep(100);
		lenth = pADMoudle->m_comm.ReadChar(8, chCommand);

		if (lenth > 0)
		{
			data |= chCommand[3];
			data = data << 8;
			temp &= chCommand[4];
			data |= temp;

			result = data *10.0 / 4096.0;
		}
	}

	return result;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	if (nIDEvent == TIME_CHECK_STATUS)
	{
		double dbVolt = ReadDCVoltage();
		if (dbVolt < MIN_VOL)
		{
			StopCheckStatus();
			AfxMessageBox(_T("高压放大器处于断电保护状态，请重启操作软件"));
		}

	}

	if (nIDEvent == TIME_CHECK_DEVICE){
		switch (index){
		case 0:{
#if _DEBUGE_PARKER
				   CDeviceParker *pParker = CDeviceParker::Instance();
				   if (pParker){
					   CMFCRibbonLabel *pLable = (CMFCRibbonLabel *)m_wndStatusBar.GetElement(0);
					   if (pParker->JetParkerIsConnected()){
						   pLable->SetText(_T("控制卡正常"));
					   }
					   else
						   pLable->SetText(_T("控制卡异常"));

				   }
#endif
				   break;
		}
		case 1:{
#if _DEBUGE_VISA
				   CDeviceVisa *pVisa = CDeviceVisa::Instance();
				   if (pVisa){
					   CMFCRibbonLabel *pLable = (CMFCRibbonLabel *)m_wndStatusBar.GetElement(0);
					   if (pVisa->IsDeviceConnected()){
						   pLable->SetText(_T("函数发生器正常"));
					   }
					   else
						   pLable->SetText(_T("函数发生器异常"));
				   }
#endif
				   break;
		}
		case 2:{
				   CDeviceHarvardPump *pPump = CDeviceHarvardPump::Instance();
				   if (pPump){
					   CMFCRibbonLabel *pLable = (CMFCRibbonLabel *)m_wndStatusBar.GetElement(0);
					   if (pPump->IsHarvardOpen()){
						   pLable->SetText(_T("流量泵正常"));
					   }
					   else
						   pLable->SetText(_T("流量泵异常"));
				   }
				   break;
		}
		case 3:{
				   CDeviceCamera *pCamera = CDeviceCamera::Instance();
				   if (pCamera){
					   CMFCRibbonLabel *pLable = (CMFCRibbonLabel *)m_wndStatusBar.GetElement(0);
					   if (pCamera->GetWatchCameraStatus()){
						   pLable->SetText(_T("观测相机正常"));
					   }
					   else
						   pLable->SetText(_T("观测相机异常"));
					   break;
				   }
		}
		case 4:{
				   CDeviceCamera *pCamera = CDeviceCamera::Instance();
				   if (pCamera){
					   CMFCRibbonLabel *pLable = (CMFCRibbonLabel *)m_wndStatusBar.GetElement(0);
					   if (pCamera->GetLocateCameraStatus()){
						   pLable->SetText(_T("定位相机正常"));
					   }
					   else
						   pLable->SetText(_T("定位相机异常"));
					   break;
				   }
		}
		}
		++index;
		index = index % 5;
	}

	if (nIDEvent == TIME_CHECK_TEMP){
#if _DEBUGE_MODE
		CDeviceHeatModule *pHeatMoudle = CDeviceHeatModule::Instance();
		if (pHeatMoudle){
			if (pHeatMoudle->IsConnected()){
				double dbTemp = pHeatMoudle->GetTemperature();
				CString editText;
				editText.Format(_T("当前温度%.4f"), dbTemp);

				CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_PRINT_TEMPERATURE_EDIT));
				pEdit->SetEditText(editText);
			}

		}
#endif
	}

	CJetPrintDTFrame::OnTimer(nIDEvent);
}

void CMainFrame::StartCheckStatus()
{
	//SetTimer(TIME_CHECK_STATUS, TIME_DELAY_GATHER, NULL);
}

void CMainFrame::StopCheckStatus()
{
	KillTimer(TIME_CHECK_STATUS);
}

void CMainFrame::StartWatchParkerStatus(){
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	if (pParker){
		pParker->JetParkerInitWnd(this);
		pParker->JetParkerWatchStatusInit();
	}
#endif
}

void CMainFrame::StartCheckTemp(){
	SetTimer(TIME_CHECK_TEMP, 2000, NULL);
}

void CMainFrame::StopCheckTemp(){
	KillTimer(TIME_CHECK_TEMP);
}

void CMainFrame::UpdateTempSlider(){
	CDeviceHeatModule *pHeatMoudle = CDeviceHeatModule::Instance();
	if (pHeatMoudle){
		if (pHeatMoudle->IsConnected()){
			double dbCurrentDestTemp = pHeatMoudle->GetCurrentDestTemp();

			CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, m_wndRibbonBar.FindByID(ID_PRINT_TEMPERATURE_ADJUST_SLIDER));
			pSlider->SetPos(int(dbCurrentDestTemp));

			CString editText = _T("");
			editText.Format(_T("目标温度%.4f"), dbCurrentDestTemp);
			CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_PRINT_TEMPERATURESET_EDIT));
			pEdit->SetEditText(editText);
		}
	}
}

void CMainFrame::UpdateLightSlider(){
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){
		if (pCamera->m_CurrentCameraType == WATCH_CAMERA){
			CDeviceWatchLightPort *pWatchLight = CDeviceWatchLightPort::Instance();
			if (pWatchLight){
				int iPos = pWatchLight->GetLightLuminence();

				//Set Slider Pos
				CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, m_wndRibbonBar.FindByID(ID_PRINT_LIGHT_LUMINANCE_SLIDER));
				pSlider->SetPos(iPos);

				CString editText = _T("");
				editText.Format(_T("当前亮度%d"), iPos);
				CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_PRINT_LIGHT_LUMINANCE_EDIT));
				pEdit->SetEditText(editText);
			}
		}
		else if (pCamera->m_CurrentCameraType == LOCATE_CAMERA){
			CDeviceLocateLightPort *pLocateLight = CDeviceLocateLightPort::Instance();
			if (pLocateLight){
				int iPos = pLocateLight->GetLightLuminence();

				//Set Slider Pos
				CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, m_wndRibbonBar.FindByID(ID_PRINT_LIGHT_LUMINANCE_SLIDER));
				pSlider->SetPos(iPos);

				CString editText = _T("");
				editText.Format(_T("当前亮度%d"), iPos);
				CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_PRINT_LIGHT_LUMINANCE_EDIT));
				pEdit->SetEditText(editText);
			}
		}
		else{
			int iPos = 0;

			//Set Slider Pos
			CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, m_wndRibbonBar.FindByID(ID_PRINT_LIGHT_LUMINANCE_SLIDER));
			pSlider->SetPos(iPos);

			CString editText = _T("");
			editText.Format(_T("当前亮度%d"), iPos);
			CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_PRINT_LIGHT_LUMINANCE_EDIT));
			pEdit->SetEditText(editText);
		}

	}
}

void CMainFrame::UpdateWindowState()
{
	CPrintParamSetView *pParamView = (CPrintParamSetView*)m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][PARAMSET_VIEW];
	if (NULL != pParamView)
	{
		if (NULL != pParamView->m_hWnd)
		{
			CDeviceHarvardPump *pPump = CDeviceHarvardPump::Instance();
			pParamView->UpdateFlowControlState(pPump->IsHarvardOpen());
#if _DEBUGE_VISA
			CDeviceVisa *pVisa = CDeviceVisa::Instance();
			pParamView->UpdateAgilentControlState(pVisa->IsDeviceConnected());
#endif
		}
	}

	CPrintMotionControlView *pMotionView = (CPrintMotionControlView*)m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW];
	if (NULL != pMotionView)
	{
		if (NULL != pMotionView->m_hWnd)
		{
			pMotionView->UpdateControlState();

			::PostMessageA(pMotionView->m_hWnd, WM_UPDATE_MOTIONSTATE, 0, 0);
		}
	}

	CPrintFileOperateView *pFileView = (CPrintFileOperateView*)m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW];
	if (NULL != pFileView)
	{
		pFileView->AnalysisAndDraw();
	}

	CPrintPrintControlView *pPrintView = (CPrintPrintControlView*)(m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][PRINTCONTROL_VIEW]);
	if (pPrintView)
		pPrintView->UpdateDeviceStatus();

	SetTimer(TIME_CHECK_DEVICE, 1000, NULL);
}

void CMainFrame::OnPrintSwitchcameraButton()
{
	// TODO:  在此添加命令处理程序代码
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){
		pCamera->CameraSwitchDevice();
	}

	UpdateLightSlider();
}


void CMainFrame::OnUpdatePrintSwitchcameraButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	bool bResult = false;
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){
		bResult = (pCamera->GetLocateCameraStatus() && pCamera->GetWatchCameraStatus());
	}

	pCmdUI->Enable(bResult);
}


void CMainFrame::OnPrintTurnonheatButton()
{
	// TODO:  在此添加命令处理程序代码
	CDeviceHeatModule *pHeatMoudle = CDeviceHeatModule::Instance();
	if (pHeatMoudle){
		pHeatMoudle->StartHeat();
	}
}


void CMainFrame::OnUpdatePrintTurnonheatButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	CDeviceHeatModule *pHeatMoudle = CDeviceHeatModule::Instance();
	bool bEnabled = false;
	if (pHeatMoudle){
		if (pHeatMoudle->IsConnected() && false == pHeatMoudle->m_bIsHeatting)
			bEnabled = true;
		else
			bEnabled = false;
	}

	pCmdUI->Enable(true);
}


void CMainFrame::OnPrintTurnoffheatButton()
{
	// TODO:  在此添加命令处理程序代码
	CDeviceHeatModule *pHeatMoudle = CDeviceHeatModule::Instance();
	if (pHeatMoudle){
		pHeatMoudle->StopHeat();
	}
}


void CMainFrame::OnUpdatePrintTurnoffheatButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	CDeviceHeatModule *pHeatMoudle = CDeviceHeatModule::Instance();
	bool bEnabled = false;
	if (pHeatMoudle){
		if (pHeatMoudle->IsConnected() && pHeatMoudle->m_bIsHeatting)
			bEnabled = true;
		else
			bEnabled = false;
	}

	pCmdUI->Enable(true);
}


void CMainFrame::OnPrintTemperatureAdjustSlider()
{
	// TODO:  在此添加命令处理程序代码
	CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, m_wndRibbonBar.FindByID(ID_PRINT_TEMPERATURE_ADJUST_SLIDER));
	int iPos = pSlider->GetPos();

	CString editText = _T("");
	editText.Format(_T("目标温度%d"), iPos);
	CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_PRINT_TEMPERATURESET_EDIT));
	pEdit->SetEditText(editText);

	CDeviceHeatModule *pHeatMoudle = CDeviceHeatModule::Instance();
	if (pHeatMoudle){
		pHeatMoudle->SetTemperature(iPos);
	}
}


void CMainFrame::OnUpdatePrintTemperatureAdjustSlider(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	CDeviceHeatModule *pHeatMoudle = CDeviceHeatModule::Instance();
	bool bEnabled = false;
	if (pHeatMoudle){
		bEnabled = pHeatMoudle->IsConnected();
	}

	pCmdUI->Enable(bEnabled);
}


void CMainFrame::OnPrintTurnonpresurreButton()
{
	// TODO:  在此添加命令处理程序代码
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	if (pParker){
		pParker->m_controlChannel->SetFlag(34, VARIANT_TRUE, VARIANT_TRUE);
	}
#endif
}


void CMainFrame::OnUpdatePrintTurnonpresurreButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	bool bEnabled = false;
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	if (pParker){
		bEnabled = pParker->JetParkerIsConnected();
	}
#endif
	pCmdUI->Enable(bEnabled);
}


void CMainFrame::OnPrintTurnoffpressureButton()
{
	// TODO:  在此添加命令处理程序代码
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	if (pParker){
		pParker->m_controlChannel->SetFlag(34, VARIANT_FALSE, VARIANT_TRUE);
	}
#endif
}


void CMainFrame::OnUpdatePrintTurnoffpressureButton(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	bool bEnabled = false;
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	if (pParker){
		bEnabled = pParker->JetParkerIsConnected();
	}
#endif
	pCmdUI->Enable(bEnabled);
}


