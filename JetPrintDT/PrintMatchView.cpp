// PrintMatchView.cpp : 实现文件
//

#include "stdafx.h"
#include "JetPrintDT.h"
#include "PrintMatchView.h"
#include "SingleTon/ParamInitSingleton.h"
#include "Device/DeviceParker.h"
#include "PrintMotionControlView.h"
#include "PrintFileOperateView.h"
#include "MainFrm.h"
// CPrintMatchView

IMPLEMENT_DYNCREATE(CPrintMatchView, CDialog)

CPrintMatchView::CPrintMatchView()
: CDialog(CPrintMatchView::IDD)
{
	m_dbXPrintOrignPos = 0.0;
	m_dbYPrintOrignPos = 0.0;
	m_dbXLocatePos = 0.0;
	m_dbYLocatePos = 0.0;
	m_dbXJetPos = 0.0;
	m_dbYJetPos = 0.0;
}

CPrintMatchView::~CPrintMatchView()
{
}

void CPrintMatchView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPrintMatchView, CDialog)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_SETDIST_BUTTON, &CPrintMatchView::OnBnClickedJetprintdtPrintMatchLocatecameraTocenterSetdistButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_MOVETO_BUTTON, &CPrintMatchView::OnBnClickedJetprintdtPrintMatchLocatecameraTocenterMovetoButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_MATCH_JET_TOCENTER_SETDIST_BUTTON, &CPrintMatchView::OnBnClickedJetprintdtPrintMatchJetTocenterSetdistButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_YSTEPPOSITIVE_BUTTON, &CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignYsteppositiveButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_XSTEPNEGETIVE_BUTTON, &CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignXstepnegetiveButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_YSTEPNEGETIVE_BUTTON, &CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignYstepnegetiveButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_XSTEPPOSITIVE_BUTTON, &CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignXsteppositiveButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_STEPLENTHSET_BUTTON, &CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignSteplenthsetButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_CACULATE_PRINTORIGN_BUTTON, &CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignCaculatePrintorignButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_SET_DECODEFILE_BUTTON, &CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignSetDecodefileButton)
END_MESSAGE_MAP()


// CPrintMatchView 诊断

#ifdef _DEBUG
void CPrintMatchView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPrintMatchView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


BOOL CPrintMatchView::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CParamInitSingleton *pParam = CParamInitSingleton::Instance();
	if (pParam){
		m_dbXJetPos = pParam->GetXJetPos();
		m_dbYJetPos = pParam->GetYJetPos();
		m_dbXLocatePos = pParam->GetXLocateCameraPos();
		m_dbYLocatePos = pParam->GetYLocateCameraPos();
	}

	CString editText = _T("");
	editText.Format(_T("%.4f"), m_dbXLocatePos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_XPOS_EDIT)->SetWindowText(editText);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_REFERENCEDIST_XPOS_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), m_dbYLocatePos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_YPOS_EDIT)->SetWindowText(editText);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_REFERENCEDIST_YPOS_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), m_dbXJetPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_JET_TOCENTER_REFERENCEDIST_XPOS_EDIT)->SetWindowText(editText);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_JET_TOCENTER_NEWDIST_XPOS_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), m_dbYJetPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_JET_TOCENTER_REFERENCEDIST_YPOS_EDIT)->SetWindowText(editText);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_JET_TOCENTER_NEWDIST_YPOS_EDIT)->SetWindowText(editText);

	//set step lenth
	CMainFrame *pMainWnd = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainWnd){
		CPrintMotionControlView *pView = (CPrintMotionControlView *)pMainWnd->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW];
		if (pView){
			m_dbStepLenth = pView->GetXStepLenth();
		}
	}

	editText.Format(_T("%.4f"), m_dbStepLenth);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_STEPLENTHSET_EDIT)->SetWindowText(editText);


	editText.Format(_T("%.4f"), m_dbXPrintOrignPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_XPOS_EDIT)->SetWindowTextW(editText);
	editText.Format(_T("%.4f"), m_dbYPrintOrignPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_YPOS_EDIT)->SetWindowTextW(editText);

	InitSimulatePanel();

	CPoint indicatorPos;
	int nXpos = 0;
	int nYpos = 0;
	m_PreviewWnd.DevicePointToLogicPoint(m_dbXLocatePos, m_dbYLocatePos, nXpos, nYpos);
	indicatorPos.x = nXpos;
	indicatorPos.y = nYpos;
	m_PreviewWnd.updatelocatecameraPoint(indicatorPos);

	m_PreviewWnd.DevicePointToLogicPoint(m_dbXJetPos, m_dbYJetPos, nXpos, nYpos);
	indicatorPos.x = nXpos;
	indicatorPos.y = nYpos;
	m_PreviewWnd.updatejetPoint(indicatorPos);

	m_PreviewWnd.DevicePointToLogicPoint(m_dbXPrintOrignPos, m_dbYPrintOrignPos, nXpos, nYpos);
	indicatorPos.x = nXpos;
	indicatorPos.y = nYpos;
	m_PreviewWnd.updateprintorignpos(indicatorPos);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CPrintMatchView::UpdatePosition(double dbXPos, double dbYPos){
	CString editText = _T("");
	editText.Format(_T("%.4f"), dbXPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_XPOS_EDIT)->SetWindowText(editText);
	editText.Format(_T("%.4f"), dbYPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_YPOS_EDIT)->SetWindowText(editText);
}

void CPrintMatchView::OnBnClickedJetprintdtPrintMatchLocatecameraTocenterSetdistButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CString editText = _T("");
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_XPOS_EDIT)->GetWindowText(editText);
	m_dbXLocatePos = _wtof(editText);

	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_YPOS_EDIT)->GetWindowText(editText);
	m_dbYLocatePos = _wtof(editText);

	CMainFrame *pMainWnd = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainWnd){
		CPrintMotionControlView *pView = (CPrintMotionControlView *)pMainWnd->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW];
		if (pView){
			pView->UpdateLocateCameraPos(m_dbXLocatePos, m_dbYLocatePos);
		}
	}

	CPoint indicatorPos;
	int nXpos = 0;
	int nYpos = 0;
	m_PreviewWnd.DevicePointToLogicPoint(m_dbXLocatePos, m_dbYLocatePos, nXpos, nYpos);
	indicatorPos.x = nXpos;
	indicatorPos.y = nYpos;
	m_PreviewWnd.updatelocatecameraPoint(indicatorPos);
}


void CPrintMatchView::OnBnClickedJetprintdtPrintMatchLocatecameraTocenterMovetoButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame *pMainWnd = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainWnd){
		CPrintMotionControlView *pView = (CPrintMotionControlView *)pMainWnd->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW];
		if (pView){
			pView->OnBnClickedJetprintdtMotioncontrolMotionMovetoLocatecenterButton();
		}
	}
}


void CPrintMatchView::OnBnClickedJetprintdtPrintMatchJetTocenterSetdistButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CString editText = _T("");
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_XPOS_EDIT)->GetWindowText(editText);
	double dbXPos = _wtof(editText);

	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_YPOS_EDIT)->GetWindowText(editText);
	double dbYPos = _wtof(editText);


	m_dbXJetPos = m_dbXLocatePos - dbXPos;
	m_dbYJetPos = m_dbYLocatePos - dbYPos;

	editText.Format(_T("%.4f"), m_dbXJetPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_JET_TOCENTER_NEWDIST_XPOS_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), m_dbYJetPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_JET_TOCENTER_NEWDIST_YPOS_EDIT)->SetWindowText(editText);


	CParamInitSingleton *pParam = CParamInitSingleton::Instance();
	if (pParam){
		pParam->SetXJetPos(m_dbXJetPos);
		pParam->SetYJetPos(m_dbYJetPos);
	}

	CPoint indicatorPos;
	int nXpos = 0;
	int nYpos = 0;
	m_PreviewWnd.DevicePointToLogicPoint(m_dbXJetPos, m_dbYJetPos, nXpos, nYpos);
	indicatorPos.x = nXpos;
	indicatorPos.y = nYpos;
	m_PreviewWnd.updatejetPoint(indicatorPos);
}


void CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignYsteppositiveButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame *pMainWnd = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainWnd){
		CPrintMotionControlView *pView = (CPrintMotionControlView *)pMainWnd->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW];
		if (pView){
			pView->OnBnClickedJetprintdtMotioncontrolMotionStepforwardButton();
		}
	}
}


void CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignXstepnegetiveButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame *pMainWnd = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainWnd){
		CPrintMotionControlView *pView = (CPrintMotionControlView *)pMainWnd->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW];
		if (pView){
			pView->OnBnClickedJetprintdtMotioncontrolMotionStepleftButton();
		}
	}
}


void CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignYstepnegetiveButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame *pMainWnd = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainWnd){
		CPrintMotionControlView *pView = (CPrintMotionControlView *)pMainWnd->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW];
		if (pView){
			pView->OnBnClickedJetprintdtMotioncontrolMotionStepbackwardButton();
		}
	}
}


void CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignXsteppositiveButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame *pMainWnd = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainWnd){
		CPrintMotionControlView *pView = (CPrintMotionControlView *)pMainWnd->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW];
		if (pView){
			pView->OnBnClickedJetprintdtMotioncontrolMotionSteprightButton();
		}
	}
}


void CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignSteplenthsetButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CString editText = _T("");
	editText.Format(_T("%.4f"), m_dbStepLenth);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_STEPLENTHSET_EDIT)->GetWindowText(editText);
	m_dbStepLenth = _wtof(editText);

	CMainFrame *pMainWnd = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainWnd){
		CPrintMotionControlView *pView = (CPrintMotionControlView *)pMainWnd->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW];
		if (pView){
			pView->UpdateStepLenth(m_dbStepLenth,m_dbStepLenth);
		}
	}
}


void CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignCaculatePrintorignButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CString editText = _T("");
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_XPOS_EDIT)->GetWindowText(editText);
	double dbXPos = _wtof(editText);

	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_YPOS_EDIT)->GetWindowText(editText);
	double dbYPos = _wtof(editText);


	m_dbXPrintOrignPos = m_dbXLocatePos - dbXPos;
	m_dbYPrintOrignPos = m_dbYLocatePos - dbYPos;

	editText.Format(_T("%.4f"), m_dbXPrintOrignPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_XPOS_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), m_dbYPrintOrignPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_YPOS_EDIT)->SetWindowText(editText);

	CPoint indicatorPos;
	int nXpos = 0;
	int nYpos = 0;
	m_PreviewWnd.DevicePointToLogicPoint(m_dbXPrintOrignPos, m_dbYPrintOrignPos, nXpos, nYpos);
	indicatorPos.x = nXpos;
	indicatorPos.y = nYpos;
	m_PreviewWnd.updateprintorignpos(indicatorPos);
}

bool CPrintMatchView::InitSimulatePanel()
{
	bool bResult = false;
	CRect panelRect;
	GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_PREVIEW_STATIC)->GetClientRect(&panelRect);

	if (NULL == m_PreviewWnd.m_hWnd)
	{
		m_PreviewWnd.Create(GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_PREVIEW_STATIC), panelRect, IDC_JETPRINTDT_PRINT_MATCH_PRIVIEW_PANEL);
	}

	if (NULL != m_PreviewWnd.m_hWnd)
	{
		//m_JetPanelWnd.SetBackColor(RGB(0,255,0));
		panelRect.left += 2;
		panelRect.right -= 2;
		panelRect.top += 2;
		panelRect.bottom -= 4;

		m_PreviewWnd.MoveWindow(panelRect);

		m_PreviewWnd.ShowWindow(SW_SHOW);

		bResult = true;
		return bResult;
	}

	return bResult;
}

BOOL CPrintMatchView::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (WM_KEYDOWN == pMsg->message&&VK_RETURN == pMsg->wParam)
	{
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_REFERENCEDIST_XPOS_EDIT)){
			return TRUE;
		}
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_REFERENCEDIST_YPOS_EDIT)){
			return TRUE;
		}
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_XPOS_EDIT)){
			return TRUE;
		}
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_LOCATECAMERA_TOCENTER_NEWDIST_YPOS_EDIT)){
			return TRUE;
		}


		//jet
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_JET_TOCENTER_REFERENCEDIST_XPOS_EDIT)){
			return TRUE;
		}
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_JET_TOCENTER_REFERENCEDIST_YPOS_EDIT)){
			return TRUE;
		}
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_JET_TOCENTER_NEWDIST_XPOS_EDIT)){
			return TRUE;
		}
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_JET_TOCENTER_NEWDIST_YPOS_EDIT)){
			return TRUE;
		}

		//print orign
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_STEPLENTHSET_EDIT)){
			return TRUE;
		}
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_XPOS_EDIT)){
			return TRUE;
		}
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_PRINTORIGN_YPOS_EDIT)){
			return TRUE;
		}
	}

	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_XPOSITIVE_BUTTON)->m_hWnd)
		{
			//X Jog Positive
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			pParker->JetParkerXJogPlus();
			pParker->m_bIsJogMode = true;
#endif
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_YPOSITIVE_BUTTON)->m_hWnd)
		{
			//Y Jog Positive
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			pParker->JetParkerYJogPlus();
			pParker->m_bIsJogMode = true;
#endif
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_XNEGETIVE_BUTTON)->m_hWnd)
		{
			//X Jog Negative
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			pParker->JetParkerXJogMinus();
			pParker->m_bIsJogMode = true;
#endif
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_YNEGETIVE_BUTTON)->m_hWnd)
		{
			//Y Jog Negative
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			pParker->JetParkerYJogMinum();
			pParker->m_bIsJogMode = true;
#endif 
		}

		return CDialog::PreTranslateMessage(pMsg);
	}


	if (pMsg->message == WM_LBUTTONUP)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_XPOSITIVE_BUTTON)->m_hWnd)
		{
			//X Stop Jog Positive
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			if (pParker->JetParkerIsXJogPlus())
			{
				UINT nMotor = 0;
				pParker->JetParkerJogStop(nMotor);
			}
#endif
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_YPOSITIVE_BUTTON)->m_hWnd)
		{
			//Y Stop Jog Positive
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			if (pParker->JetParkerIsYJogPlus())
			{
				UINT nMotor = 1;
				pParker->JetParkerJogStop(nMotor);
			}
#endif
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_XNEGETIVE_BUTTON)->m_hWnd)
		{
			//X Stop Jog Negative
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			if (pParker->JetParkerIsXJogMinus())
			{
				UINT nMotor = 0;
				pParker->JetParkerJogStop(nMotor);
			}
#endif
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_MATCH_YNEGETIVE_BUTTON)->m_hWnd)
		{
			//Y Stop Jog Negative
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			if (pParker->JetParkerIsYJogMinus())
			{
				UINT nMotor = 1;
				pParker->JetParkerJogStop(nMotor);
			}
#endif
		}

		return CDialog::PreTranslateMessage(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CPrintMatchView::OnBnClickedJetprintdtPrintMatchPrintorignSetDecodefileButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame *pMainWnd = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainWnd){
		CPrintFileOperateView *pView = (CPrintFileOperateView *)pMainWnd->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW];
		if (pView){
			pView->MatchPrintDecodeFile(m_dbXPrintOrignPos, m_dbYPrintOrignPos,m_dbXJetPos,m_dbYJetPos);
		}
	}
}
