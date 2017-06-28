// PrintMotionControlView.cpp : 实现文件
//

#include "stdafx.h"
#include "JetPrintDT.h"
#include "PrintMotionControlView.h"
#include "SingleTon/ParamInitSingleton.h"
#include "Device/DeviceParker.h"
#include "Device/DeviceVisa.h"
#include "Database/MotionTable.h"
#include "MainFrm.h"
#include "PrintFileOperateView.h"
#include "PrintPrintControlView.h"
#include "PrintParamSetView.h"
// CPrintMotionControlView

IMPLEMENT_DYNCREATE(CPrintMotionControlView, CResizableFormView)

CPrintMotionControlView::CPrintMotionControlView()
: CResizableFormView(CPrintMotionControlView::IDD)
{
	db_XTargetPos = 0.0;
	db_YTargetPos = 0.0;
	db_XPos = 0.0;
	db_YPos = 0.0;
	db_XStepLenth = 0.0;
	db_YStepLenth = 0.0;

	db_XPrintOrignPos = 0.0;
	db_YPrintOrignPos = 0.0;

	m_hMotionEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_pMotionCheckThread = NULL;
	m_nSelectedRow = -1;

	m_bMotionThreadRun = true;

	m_bSuspendThread = false;

	m_dbToLocateCameraXOffset = 0.0;
	m_dbToLocateCameraYOffset = 0.0;

	m_iDisCalBtnStateFlag = 0;
	m_iErrCalBtnStateFlag = 0;
	for (int i = 0; i < 2; i++)
	{
		m_fSprayerCenPos_DisCal[i] = 0.0;
		m_fCameraCenPos_DisCal[i] = 0.0;
		m_fCenDisSprayerAndCamera[i] = 0.0;
		m_fSprayerCenPos_ErrCal[i] = 0.0;
		m_fCameraCenPos_ErrCal[i] = 0.0;
		m_fPrintErrOffset[i] = 0.0;
	}
}

CPrintMotionControlView::~CPrintMotionControlView()
{
}

void CPrintMotionControlView::DoDataExchange(CDataExchange* pDX)
{
	CResizableFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPrintMotionControlView, CResizableFormView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_TARGETPOSSET_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionTargetpossetButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPLENTHSET_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionSteplenthsetButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPBACKWARD_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionStepbackwardButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPLEFT_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionStepleftButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPRIGHT_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionSteprightButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPFORWARD_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionStepforwardButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_LOCATECENTER_CALIBRATE_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionLocatecenterCalibrateButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_LOCATEDIST_CALIBRATE_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionLocatedistCalibrateButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_PRINTERROR_CALIBRATE_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionPrinterrorCalibrateButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_SET_PRINTORIGNPOS_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionSetPrintorignposButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_MOVE_HOME_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionMoveHomeButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_MOVETO_PRINTORIGNPOS_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionMovetoPrintorignposButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_MOVETO_LOCATECENTER_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionMovetoLocatecenterButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_DATA_APPLY_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolDataApplyButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_DATA_APPEND_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolDataAppendButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_MOTIONCONTROL_DATA_DELETE_BUTTON, &CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolDataDeleteButton)
	ON_NOTIFY(NM_CLICK, IDC_JETPRINTDT_MOTIONCONTROL_DATA_GRID, &CPrintMotionControlView::OnGridClick)
	ON_MESSAGE(WM_MOTION_KILL, &CPrintMotionControlView::OnMotionStatusChange)
	ON_MESSAGE(WM_UPDATE_MOTIONSTATE, &CPrintMotionControlView::OnMotionControlChange)
	ON_MESSAGE(WM_CHANGE_STEP_LENTH, &CPrintMotionControlView::OnChangeStepLenth)
	ON_MESSAGE(WM_CHANGE_POSITION, &CPrintMotionControlView::OnChangePosition)
	ON_MESSAGE(WM_PALETTE_JOG, &CPrintMotionControlView::OnPeletteJog)
	ON_MESSAGE(WM_PALETTE_STOP_JOG, &CPrintMotionControlView::OnPeletteStopJog)
	ON_MESSAGE(WM_HOME_FAILED, &CPrintMotionControlView::OnHomeFailed)
END_MESSAGE_MAP()


// CPrintMotionControlView 诊断

#ifdef _DEBUG
void CPrintMotionControlView::AssertValid() const
{
	CResizableFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPrintMotionControlView::Dump(CDumpContext& dc) const
{
	CResizableFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPrintMotionControlView 消息处理程序


BOOL CPrintMotionControlView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CResizableFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CPrintMotionControlView::OnInitialUpdate()
{
	//Motion Group
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_GROUP, CSize(0, 0), CSize(100, 60));
	//Left Part
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XTARGETPOS_STATIC, CSize(0, 0),CSize(3,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XTARGETPOS_EDIT, CSize(3, 0),CSize(8,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XTARGETPOS_UNIT_STATIC, CSize(8, 0),CSize(10,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YTARGETPOS_STATIC, CSize(0, 3),CSize(3,3));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YTARGETPOS_EDIT, CSize(3, 3),CSize(8,3));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YTARGETPOS_UNIT_STATIC, CSize(8, 3),CSize(10,3));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_TARGETPOSSET_BUTTON, CSize(0, 6),CSize(10,11));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_STATIC, CSize(10, 0),CSize(13,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_EDIT, CSize(13, 0),CSize(18,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_UNIT_STATIC, CSize(18, 0),CSize(20,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_STATIC, CSize(10, 3),CSize(13,3));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_EDIT, CSize(13, 3),CSize(18,3));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_UNIT_STATIC, CSize(18, 3),CSize(20,3));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPLENTHSET_BUTTON, CSize(10, 6),CSize(20,11));

	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPBACKWARD_BUTTON, CSize(5, 15),CSize(15,20));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPLEFT_BUTTON, CSize(0, 20),CSize(10,25));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPRIGHT_BUTTON, CSize(10, 20),CSize(15,25));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPFORWARD_BUTTON, CSize(5, 25),CSize(15,30));

	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_LOCATECENTER_CALIBRATE_BUTTON, CSize(0, 35),CSize(5,41));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_LOCATEDIST_CALIBRATE_BUTTON, CSize(0, 42),CSize(5,48));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_PRINTERROR_CALIBRATE_BUTTON, CSize(0, 49),CSize(5,55));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_SET_PRINTORIGNPOS_BUTTON, CSize(10, 37),CSize(15,44));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_MOVETO_PRINTORIGNPOS_BUTTON, CSize(15, 37),CSize(20,44));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_MOVE_HOME_BUTTON, CSize(10, 46),CSize(15,53));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_MOVETO_LOCATECENTER_BUTTON, CSize(15, 46),CSize(20,53));

	//Right Part
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XCURRENTPOS_STATIC, CSize(40, 0),CSize(50,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XCURRENTPOS_EDIT, CSize(45, 0),CSize(60,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XCURRENTPOS_UNIT_STATIC, CSize(60, 0),CSize(70,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YCURRENTPOS_STATIC, CSize(60, 0),CSize(70,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YCURRENTPOS_EDIT, CSize(65, 0),CSize(80,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YCURRENTPOS_UNIT_STATIC, CSize(80, 0),CSize(90,0));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XNEGATIVE_BUTTON, CSize(20, 32),CSize(30,47));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XPOSITIVE_BUTTON, CSize(85, 32),CSize(95,47));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YNEGATIVE_BUTTON, CSize(51, 55),CSize(61,65));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YPOSITIVE_BUTTON, CSize(51, 10),CSize(61,20));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_SIMULATE_STATIC, CSize(30, 20),CSize(85,55));

	//Data Group
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_DATA_GROUP, CSize(0, 65), CSize(100, 100));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_DATA_LIST_STATIC, CSize(0, 65), CSize(90, 100));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_DATA_APPLY_BUTTON, CSize(90, 70), CSize(95, 80));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_DATA_APPEND_BUTTON, CSize(90, 80), CSize(95, 90));
	AddAnchor(IDC_JETPRINTDT_MOTIONCONTROL_DATA_DELETE_BUTTON, CSize(90, 90), CSize(95, 100));
	CResizableFormView::OnInitialUpdate();

	// TODO:  在此添加专用代码和/或调用基类
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	InitSimulatePanel();
	InitializeDataGrid();
	//InitializeParam
	InitializeParam();

	SetDataGrid();

	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_DATA_APPLY_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_DATA_APPEND_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_DATA_DELETE_BUTTON)->EnableWindow(FALSE);

	SetTimer(TIME_UPDATE_POS, 1000, NULL);
	SetTimer(TIME_CHECK_STATE, 1000, NULL);
}

bool CPrintMotionControlView::InitSimulatePanel()
{
	bool bResult = false;
	CRect panelRect;
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_SIMULATE_STATIC)->GetClientRect(&panelRect);

	if (NULL == m_JetPanelWnd.m_hWnd)
	{
		m_JetPanelWnd.Create(GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_SIMULATE_STATIC), panelRect, IDC_JETPRINTDT_MOTIONCONTROL_MOTION_SIMULATE_PANEL);
	}

	if (NULL != m_JetPanelWnd.m_hWnd)
	{
		//m_JetPanelWnd.SetBackColor(RGB(0,255,0));
		panelRect.left += 2;
		panelRect.right -= 2;
		panelRect.top += 2;
		panelRect.bottom -= 4;

		m_JetPanelWnd.MoveWindow(panelRect);

		m_JetPanelWnd.ShowWindow(SW_SHOW);

		bResult = true;
		return bResult;
	}

	return bResult;
}



void CPrintMotionControlView::OnSize(UINT nType, int cx, int cy)
{
	CResizableFormView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	if (NULL != m_JetPanelWnd.m_hWnd)
	{
		CRect chartRect;
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_SIMULATE_STATIC)->GetClientRect(&chartRect);
		chartRect.left = chartRect.left + 4;
		chartRect.top = chartRect.top + 4;
		chartRect.bottom = chartRect.bottom - 4;
		chartRect.right = chartRect.right - 4;

		m_JetPanelWnd.MoveWindow(chartRect);
	}

	if (NULL != m_MotionGridData.m_hWnd){
		CRect gridRect;
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_DATA_LIST_STATIC)->GetWindowRect(&gridRect);
		ScreenToClient(gridRect);
		gridRect.left = gridRect.left + 4;
		gridRect.top = gridRect.top + 4;
		gridRect.bottom = gridRect.bottom - 4;
		gridRect.right = gridRect.right - 4;

		m_MotionGridData.MoveWindow(gridRect);
		m_MotionGridData.ExpandColumnsToFit(TRUE);
	}
}

bool CPrintMotionControlView::InitializeDataGrid()
{
	bool bResult = false;

	CRect gridRect;
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_DATA_LIST_STATIC)->GetClientRect(&gridRect);

	m_MotionGridData.Create(gridRect, this, IDC_JETPRINTDT_MOTIONCONTROL_DATA_GRID);
	if (NULL == m_MotionGridData.m_hWnd)
	{
		bResult = false;
		return bResult;
	}

	int nFixedColum = 1;
	int nFixedRow = 1;
	int nColumCount = 8;
	int nRowCount = 50;
	m_MotionGridData.SetAutoSizeStyle();
	m_MotionGridData.SetEditable(false);
	m_MotionGridData.SetFixedColumnSelection(false);
	m_MotionGridData.ExpandColumnsToFit(true);
	try
	{
		m_MotionGridData.SetColumnCount(nColumCount);
		m_MotionGridData.SetFixedColumnCount(nFixedColum);
		m_MotionGridData.SetFixedRowCount(nFixedRow);
		m_MotionGridData.SetRowCount(nRowCount);

	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		bResult = false;
		return bResult;
	}
	//END_CATCH

	int col = 0;
	CString strList = _T("");
	GV_ITEM item;
	item.mask = GVIF_TEXT;
	item.row = 0;
	item.col = col;

	//Date Colum
	strList.Format(_T("%s"), _T("日期"));
	item.strText = strList;
	m_MotionGridData.SetItem(&item);

	//XMoveSpeed Colum
	++col;
	strList.Format(_T("%s"), _T("移动速度"));
	item.col = col;
	item.strText = strList;
	m_MotionGridData.SetItem(&item);

	//XPrintOrign
	++col;
	strList.Format(_T("%s"), _T("打印原点X"));
	item.col = col;
	item.strText = strList;
	m_MotionGridData.SetItem(&item);

	//YPrintOrign
	++col;
	strList.Format(_T("%s"), _T("打印原点Y"));
	item.col = col;
	item.strText = strList;
	m_MotionGridData.SetItem(&item);

	//XStepLenth
	++col;
	strList.Format(_T("%s"), _T("X步进步长"));
	item.col = col;
	item.strText = strList;
	m_MotionGridData.SetItem(&item);

	++col;
	strList.Format(_T("%s"), _T("Y步进步长"));
	item.col = col;
	item.strText = strList;
	m_MotionGridData.SetItem(&item);

	++col;
	strList.Format(_T("%s"), _T("X偏置"));
	item.col = col;
	item.strText = strList;
	m_MotionGridData.SetItem(&item);

	++col;
	strList.Format(_T("%s"), _T("Y偏置"));
	item.col = col;
	item.strText = strList;
	m_MotionGridData.SetItem(&item);

	UpdateData(FALSE);

	int nListWidth = 0;
	CRect listRect;
	m_MotionGridData.GetClientRect(&listRect);
	nListWidth = listRect.Width();
	for (int i = 0; i < 8; i++)
	{
		m_MotionGridData.SetColumnWidth(i, nListWidth / 8 + 2);
	}

	m_MotionGridData.SetColumnResize(TRUE);

	bResult = true;
	return bResult;
}

void CPrintMotionControlView::SetDataGrid(){
	CMotionTable* pTable = CMotionTable::Instance();

	if (pTable->open())
	{
		pTable->Query();
		pTable->MoveFirst();

		int col = 0;
		int row = 1;
		CString strList = _T("");
		GV_ITEM item;
		item.mask = GVIF_TEXT;
		UINT colorR = 0;
		UINT colorG = 0;
		UINT colorB = 0;
		while (!pTable->IsEOF())
		{
			colorR = (65 + 5 * row) < 255 ? ((65 + 5 * row)) : 255;
			colorG = (105 + 5 * row) < 255 ? (105 + 5 * row) : 255;
			colorB = (225 + 5 * row) < 255 ? (225 + 5 * row) : 255;

			item.row = row;
			item.strText = pTable->m_CurrentDate;
			item.col = 0;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;

			m_MotionGridData.SetItem(&item);

			item.row = row;
			item.col = 1;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_XOrignPos);
			m_MotionGridData.SetItem(&item);

			item.row = row;
			item.col = 2;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_YOrignPos);
			m_MotionGridData.SetItem(&item);

			item.row = row;
			item.col = 3;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_XStepLenth);
			m_MotionGridData.SetItem(&item);

			item.row = row;
			item.col = 4;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_YStepLenth);
			m_MotionGridData.SetItem(&item);

			item.row = row;
			item.col = 5;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_MoveSpeed);
			m_MotionGridData.SetItem(&item);

			item.row = row;
			item.col = 6;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_XOffSet);
			m_MotionGridData.SetItem(&item);

			item.row = row;
			item.col = 7;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_YOffSet);
			m_MotionGridData.SetItem(&item);

			++row;
			pTable->MoveNext();
		}
	}

	//关闭数据库
	pTable->Close();

}

void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionTargetpossetButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strTemp = _T("");
	double db_EditValue = 0.0;
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XTARGETPOS_EDIT)->GetWindowText(strTemp);
	db_EditValue = _wtof(strTemp);

	if (db_EditValue < -60.0)
	{
		db_EditValue = -60.0;
	}

	if (db_EditValue > 60.0)
	{
		db_EditValue = 60.0;
	}

	db_XTargetPos = db_EditValue;

	strTemp.Format(_T("%.4f"), db_EditValue);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XTARGETPOS_EDIT)->SetWindowText(strTemp);


	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YTARGETPOS_EDIT)->GetWindowText(strTemp);
	db_EditValue = _wtof(strTemp);

	if (db_EditValue < -65.0)
	{
		db_EditValue = -65.0;
	}

	if (db_EditValue > 65.0)
	{
		db_EditValue = 65.0;
	}

	db_YTargetPos = db_EditValue;

	strTemp.Format(_T("%.4f"), db_EditValue);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YTARGETPOS_EDIT)->SetWindowText(strTemp);


	//移动至目标位置
	float targetPos[2];
	memset(targetPos, 0, sizeof(targetPos));
	targetPos[0] = float(db_XTargetPos);
	targetPos[1] = float(db_YTargetPos);

	CDeviceParker *pPaker = nullptr;
#if _DEBUGE_PARKER
	 pPaker = CDeviceParker::Instance();

	if (pPaker->m_bIsJogMode)
	{
		//JOG TO X0 Y0
		pPaker->JetParkerJogHome();
		//Then Move To Target Pos
		pPaker->JetParkerMove(targetPos, true, 2);

		pPaker->m_bIsJogMode = false;
	}
	else
	{
		pPaker->JetParkerMove(targetPos, true, 2);

		pPaker->m_bIsJogMode = false;
	}
#endif
}

void CPrintMotionControlView::SetStepLenth(double dbXStep, double dbYStep){
	UpdateStepLenth(dbXStep, dbYStep);
}

void CPrintMotionControlView::UpdateStepLenth(double dbXStepLenth, double dbYStepLenth){

	CString strTemp;
	db_XStepLenth = dbXStepLenth;

	strTemp.Format(_T("%.4f"), dbXStepLenth);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_EDIT)->SetWindowText(strTemp);

	db_YStepLenth = dbYStepLenth;

	strTemp.Format(_T("%.4f"), dbYStepLenth);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_EDIT)->SetWindowText(strTemp);
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionSteplenthsetButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strTemp = _T("");
	double db_EditValue = 0.0;
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_EDIT)->GetWindowText(strTemp);
	db_EditValue = _wtof(strTemp);

	if (db_EditValue < 20.0)
	{
		db_EditValue =	20.0;
	}

	if (db_EditValue > 2000.0)
	{
		db_EditValue = 2000.0;
	}

	db_XStepLenth = db_EditValue;

	strTemp.Format(_T("%.4f"), db_EditValue);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_EDIT)->SetWindowText(strTemp);


	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_EDIT)->GetWindowText(strTemp);
	db_EditValue = _wtof(strTemp);

	if (db_EditValue < 20.0)
	{
		db_EditValue = 20.0;
	}

	if (db_EditValue > 2000.0)
	{
		db_EditValue = 2000.0;
	}

	db_YStepLenth = db_EditValue;

	strTemp.Format(_T("%.4f"), db_EditValue);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_EDIT)->SetWindowText(strTemp);
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionStepbackwardButton()
{
	// TODO:  在此添加控件通知处理程序代码
	double stepLenth = db_YStepLenth / 1000.0;
	bool bPositive = false;
	int  nAxisChioce = 1;

	bool bIsMotorEnabled = false;
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	bIsMotorEnabled = pParker->JetParkerIsMotorEnabled();
	if (false == bIsMotorEnabled)
	{

		pParker->JetParkerEnableMotor();
	}

#if 0
	if (pParker->m_bIsJogMode)
	{
		pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
		pParker->m_bIsJogMode = true;
	}
	else
	{

		//MoveHome First
		float targetPos[2];
		memset(targetPos, 0, sizeof(targetPos));
		targetPos[0] = float(0.0f);
		targetPos[1] = float(0.0f);

		pParker->JetParkerMove(targetPos, true, 2);

		pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
		pParker->m_bIsJogMode = true;
	}
#endif

	pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
	pParker->m_bIsJogMode = true;
#endif

}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionStepleftButton()
{
	// TODO:  在此添加控件通知处理程序代码
	double stepLenth = db_XStepLenth / 1000.0;
	bool bPositive = false;
	int  nAxisChioce = 0;

	bool bIsMotorEnabled = false;
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	bIsMotorEnabled = pParker->JetParkerIsMotorEnabled();
	if (false == bIsMotorEnabled)
	{

		pParker->JetParkerEnableMotor();

	}

#if 0
	if (pParker->m_bIsJogMode)
	{
		pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
		pParker->m_bIsJogMode = true;
	}
	else
	{

		//MoveHome First
		float targetPos[2];
		memset(targetPos, 0, sizeof(targetPos));
		targetPos[0] = float(0.0f);
		targetPos[1] = float(0.0f);

		pParker->JetParkerMove(targetPos, true, 2);

		pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
		pParker->m_bIsJogMode = true;
	}
#endif 

	pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
	pParker->m_bIsJogMode = true;
#endif
	
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionSteprightButton()
{
	// TODO:  在此添加控件通知处理程序代码
	double stepLenth = db_XStepLenth / 1000.0;
	bool bPositive = true;
	int  nAxisChioce = 0;

	bool bIsMotorEnabled = false;
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	bIsMotorEnabled = pParker->JetParkerIsMotorEnabled();
	if (false == bIsMotorEnabled)
	{

		pParker->JetParkerEnableMotor();
	}

#if 0
	if (pParker->m_bIsJogMode)
	{
		pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
		pParker->m_bIsJogMode = true;
	}
	else
	{

		//MoveHome First
		float targetPos[2];
		memset(targetPos, 0, sizeof(targetPos));
		targetPos[0] = float(0.0f);
		targetPos[1] = float(0.0f);

		pParker->JetParkerMove(targetPos, true, 2);

		pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
		pParker->m_bIsJogMode = true;
	}
#endif 

	pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
	pParker->m_bIsJogMode = true;
#endif
	
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionStepforwardButton()
{
	// TODO:  在此添加控件通知处理程序代码
	double stepLenth = db_YStepLenth / 1000.0;
	bool bPositive = true;
	int nAxisChioce = 1;

	bool bIsMotorEnabled = false;
#if _DEBUGE_PARKER
	CDeviceParker* pParker = CDeviceParker::Instance();
	bIsMotorEnabled = pParker->JetParkerIsMotorEnabled();
	if (false == bIsMotorEnabled)
	{

		pParker->JetParkerEnableMotor();
	}

#if 0
	if (pParker->m_bIsJogMode)
	{
		pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
		pParker->m_bIsJogMode = true;
	}
	else
	{
		//MoveHome First
		float targetPos[2];
		memset(targetPos, 0, sizeof(targetPos));
		targetPos[0] = float(0.0f);
		targetPos[1] = float(0.0f);

		pParker->JetParkerMove(targetPos, true, 2);

		pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
		pParker->m_bIsJogMode = true;
	}
#endif

	pParker->JetParkerJogStep(stepLenth, bPositive, nAxisChioce);
	pParker->m_bIsJogMode = true;
#endif
	
}

void CPrintMotionControlView::UpdateLocateCameraPos(double dbXPos, double dbYPos){
	m_dbToLocateCameraXOffset = dbXPos;
	m_dbToLocateCameraYOffset = dbYPos;


	CParamInitSingleton *pParam = CParamInitSingleton::Instance();
	if (pParam){
		pParam->SetXLocateCameraPos(m_dbToLocateCameraXOffset);
		pParam->SetYLocateCameraPos(m_dbToLocateCameraYOffset);
	}
}

void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionLocatecenterCalibrateButton()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateLocateCameraPos(db_XPos, db_YPos);
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionLocatedistCalibrateButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (0 == m_iDisCalBtnStateFlag)
	{
		if (IDOK == AfxMessageBox(_T("确定进行喷头和定位相机中心的距离标定吗？"), MB_OKCANCEL))
		{
			for (int i = 0; i < 2; i++)
			{
				m_fSprayerCenPos_DisCal[i] = 0.0;
				m_fCameraCenPos_DisCal[i] = 0.0;
				m_fCenDisSprayerAndCamera[i] = 0.0;
			}

			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_PRINTERROR_CALIBRATE_BUTTON)->EnableWindow(FALSE);
			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_LOCATEDIST_CALIBRATE_BUTTON)->SetWindowText(_T("打印墨点"));
			m_iDisCalBtnStateFlag = 1;
		}
	}
	else if (1 == m_iDisCalBtnStateFlag)
	{
		//控制高压放大器打印墨点,得到喷头中心位置
		GetCurrentPos(m_fSprayerCenPos_DisCal[0], m_fSprayerCenPos_DisCal[1]);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_LOCATEDIST_CALIBRATE_BUTTON)->SetWindowText(_T("中心对齐"));
		m_iDisCalBtnStateFlag = 2;
	}
	else if (2 == m_iDisCalBtnStateFlag)
	{
		//将相机中心和墨点中心对准,获取相机中心位置
		GetCurrentPos(m_fCameraCenPos_DisCal[0], m_fCameraCenPos_DisCal[1]);

		//计算间距,并写配置文件
		m_fCenDisSprayerAndCamera[0] = m_fCameraCenPos_DisCal[0] - m_fSprayerCenPos_DisCal[0];
		m_fCenDisSprayerAndCamera[1] = m_fCameraCenPos_DisCal[1] - m_fSprayerCenPos_DisCal[1];
		//WriteConfig();

		AfxMessageBox(_T("喷头和定位相机中心的距离标定成功！"));
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_LOCATEDIST_CALIBRATE_BUTTON)->SetWindowText(_T("喷头相机间距标定"));
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_PRINTERROR_CALIBRATE_BUTTON)->EnableWindow(TRUE);
		m_iDisCalBtnStateFlag = 0;
	}
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionPrinterrorCalibrateButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (0 == m_iErrCalBtnStateFlag)
	{
		if (IDOK == AfxMessageBox(_T("确定进行打印墨点的误差补偿标定吗？"), MB_OKCANCEL))
		{
			for (int i = 0; i < 2; i++)
			{
				m_fSprayerCenPos_ErrCal[i] = 0.0;
				m_fCameraCenPos_ErrCal[i] = 0.0;
			}

			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_LOCATEDIST_CALIBRATE_BUTTON)->EnableWindow(FALSE);
			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_PRINTERROR_CALIBRATE_BUTTON)->SetWindowText(_T("打印墨点"));
			m_iErrCalBtnStateFlag = 1;
		}
	}
	else if (1 == m_iErrCalBtnStateFlag)
	{
		//控制高压放大器打印一个墨点
		GetCurrentPos(m_fSprayerCenPos_ErrCal[0], m_fSprayerCenPos_ErrCal[1]);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_PRINTERROR_CALIBRATE_BUTTON)->SetWindowText(_T("视觉观察"));
		m_iErrCalBtnStateFlag = 2;
	}
	else if (2 == m_iErrCalBtnStateFlag)
	{
		//将相机以标定的间距移动到墨点位置
		float targetPos[2];
		memset(targetPos, 0, sizeof(targetPos));
		targetPos[0] = m_fSprayerCenPos_ErrCal[0] + m_fCenDisSprayerAndCamera[0];
		targetPos[1] = m_fSprayerCenPos_ErrCal[1] + m_fCenDisSprayerAndCamera[1];
#if _DEBUGE_PARKER
		CDeviceParker* pParker = CDeviceParker::Instance();
		if (pParker->m_bIsJogMode)
		{
			//JOG TO X0 Y0
			pParker->JetParkerJogHome();

			//Then Move To Target Pos
			pParker->JetParkerMove(targetPos, true, 2);
			pParker->m_bIsJogMode = false;
		}
		else
		{
			CDeviceParker* pParker = CDeviceParker::Instance();
			pParker->JetParkerMove(targetPos, true, 2);
			pParker->m_bIsJogMode = false;
		}

#endif
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_PRINTERROR_CALIBRATE_BUTTON)->SetWindowText(_T("中心对齐"));
		m_iErrCalBtnStateFlag = 3;
	}
	else if (3 == m_iErrCalBtnStateFlag)
	{
		//将视觉中心和墨点中心对准,获得相机中心位置
		GetCurrentPos(m_fCameraCenPos_ErrCal[0], m_fCameraCenPos_ErrCal[1]);

		//计算补偿误差
		m_fPrintErrOffset[0] = m_fCameraCenPos_ErrCal[0] - (m_fSprayerCenPos_ErrCal[0] + m_fCenDisSprayerAndCamera[0]);
		m_fPrintErrOffset[1] = m_fCameraCenPos_ErrCal[1] - (m_fSprayerCenPos_ErrCal[1] + m_fCenDisSprayerAndCamera[1]);

		AfxMessageBox(_T("打印墨点的误差补偿标定成功！"));
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_PRINTERROR_CALIBRATE_BUTTON)->SetWindowText(_T("打印误差标定"));
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_LOCATEDIST_CALIBRATE_BUTTON)->EnableWindow(TRUE);
		m_iErrCalBtnStateFlag = 0;
	}
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionSetPrintorignposButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (IDOK == AfxMessageBox(_T("是否使用标定的打印误差进行补偿？"), MB_OKCANCEL))
	{
		db_XPrintOrignPos = db_XPos - m_fPrintErrOffset[0];   //当前位置减去误差偏移
		db_YPrintOrignPos = db_YPos - m_fPrintErrOffset[1];
	}
	else
	{
		db_XPrintOrignPos = db_XPos;
		db_YPrintOrignPos = db_YPos;
	}

	CMainFrame *pMainFram = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintFileOperateView *pFileView = (CPrintFileOperateView*)(pMainFram->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW]);
	if (pFileView)
		pFileView->UpdatePrintOrignPos(db_XPrintOrignPos, db_YPrintOrignPos);

	CPrintPrintControlView *pPrintView = (CPrintPrintControlView*)(pMainFram->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][PRINTCONTROL_VIEW]);
	if (pPrintView)
		pPrintView->UpdateOrignPos();
	
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionMoveHomeButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame *pMainFram = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintFileOperateView *pFileView = (CPrintFileOperateView*)(pMainFram->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW]);
	if (pFileView)
		pFileView->m_bIsFileDownloadSucceed = false;

	bool bDownLoadResult = false;
	CString strFileName = _T("");
	CString strFilePath = _T("");
	strFilePath = GetProgPath();
	strFileName = _T("\\HOME.txt");
	strFilePath += strFileName;
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	bDownLoadResult = pParker->JetParkerDownloadFile(strFilePath);

	if (bDownLoadResult)
	{
		pParker->JetParkerRunProgram();
	}
#endif
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionMovetoPrintorignposButton()
{
	// TODO:  在此添加控件通知处理程序代码
	float targetPos[2];
	CString strTemp = _T("");
	strTemp.Format(_T("%.6f"), db_XPrintOrignPos);
	targetPos[0] = float(_wtof(strTemp));
	targetPos[1] = float(db_YPrintOrignPos);

#if _DEBUGE_PARKER
	CDeviceParker * pParker = CDeviceParker::Instance();
	if (!pParker->m_bIsJogMode)
	{
		pParker->JetParkerMove(targetPos, true, 2);
		pParker->m_bIsJogMode = false;
	}
	else
	{
		pParker->JetParkerJogHome();

		//Then Move To Target Pos
		pParker->JetParkerMove(targetPos, true, 2);
		pParker->m_bIsJogMode = false;
	}
#endif
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolMotionMovetoLocatecenterButton()
{
	// TODO:  在此添加控件通知处理程序代码
	float targetPos[2];
	memset(targetPos, 0, sizeof(targetPos));
	targetPos[0] = float(m_dbToLocateCameraXOffset);
	targetPos[1] = float(m_dbToLocateCameraYOffset);

#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	if (pParker->m_bIsJogMode)
	{
		//JOG TO X0 Y0
		pParker->JetParkerJogHome();

		//Then Move To Target Pos
		pParker->JetParkerMove(targetPos, true, 2);
		pParker->m_bIsJogMode = false;
	}
	else
	{
		pParker->JetParkerMove(targetPos, true, 2);
		pParker->m_bIsJogMode = false;
	}
#endif
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolDataApplyButton()
{
	// TODO:  在此添加控件通知处理程序代码
	int nCol = 0;
	nCol = m_MotionGridData.GetFocusCell().col;
	int nRow = 0;
	nRow = m_MotionGridData.GetFocusCell().row;

	CString itemText = _T("");
	itemText = m_MotionGridData.GetItemText(nRow, nCol);

	CMainFrame *pMainFram = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintFileOperateView *pFileView = (CPrintFileOperateView*)(pMainFram->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW]);
	CPrintPrintControlView *pPrintView = (CPrintPrintControlView*)(pMainFram->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][PRINTCONTROL_VIEW]);
	switch (nCol)
	{
		//XSpeed
		case 1:
		{
			double dbXSpeed = _wtof(itemText);
			pPrintView->SetXMoveSpeed(dbXSpeed);
			pPrintView->GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SPEED_EDIT)->SetWindowText(itemText);
			break;
		}

		//YSpeed
		case 2:
		{
			double dbYSpeed = _wtof(itemText);
			pPrintView->SetYMoveSpeed(dbYSpeed);
			break;
		}

		//XPrintOrign
		case 3:
		{
			double XPrintOrign = _wtof(itemText);
			db_XPrintOrignPos = XPrintOrign;
			break;
		}

		//YPrintOrign
		case 4:
		{
			double YPrintOrign = _wtof(itemText);
			db_YPrintOrignPos = YPrintOrign;
			break;
		}

		//XStep Lenth
		case 5:
		{
			double XStepLenth = _wtof(itemText);
			db_XStepLenth = XStepLenth;

			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_EDIT)->SetWindowText(itemText);
			break;
		}

		//YStepLenth
		case 6:
		{
			double YStepLenth = _wtof(itemText);
			db_YStepLenth = YStepLenth;

			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_EDIT)->SetWindowText(itemText);
			break;
		}
	}

	
	if (pFileView)
		pFileView->UpdatePrintOrignPos(db_XPrintOrignPos, db_YPrintOrignPos);
	
}


void CPrintMotionControlView::InitializeParam()
{
	CParamInitSingleton *pParamInstance = CParamInitSingleton::Instance();
	db_XTargetPos = pParamInstance->GetXTargetPos();
	db_YTargetPos = pParamInstance->GetYTargetPos();
	db_XStepLenth = pParamInstance->GetXStepLenth();
	db_YStepLenth = pParamInstance->GetYStepLenth();
	db_XPrintOrignPos = pParamInstance->GetXPrintOrignPos();
	db_YPrintOrignPos = pParamInstance->GetYPrintOrignPos();
	m_dbToLocateCameraXOffset = pParamInstance->GetXLocateCameraPos();
	m_dbToLocateCameraYOffset = pParamInstance->GetYLocateCameraPos();

	CString editText = _T("");
	editText.Format(_T("%.4f"), db_XTargetPos);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XTARGETPOS_EDIT)->SetWindowText(editText);
	editText.Format(_T("%.4f"), db_YTargetPos);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YTARGETPOS_EDIT)->SetWindowText(editText);
	editText.Format(_T("%.4f"), db_XStepLenth);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_EDIT)->SetWindowText(editText);
	editText.Format(_T("%.4f"), db_YStepLenth);
	GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_EDIT)->SetWindowText(editText);

	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintFileOperateView *pFileView = (CPrintFileOperateView *)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW]);
	if (pFileView)
		pFileView->UpdatePrintOrignPos(db_XPrintOrignPos, db_YPrintOrignPos);

}

void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolDataAppendButton()
{
	// TODO:  在此添加控件通知处理程序代码
	char szText[128] = { 0 };
	CMotionTable *pTable = CMotionTable::Instance();
	if (pTable->open())
	{
		pTable->AddNew();

		CTime m_Time;
		m_Time = CTime::GetCurrentTime();
		CString strTime = _T("");
		strTime = m_Time.Format(_T("%Y-%m-%d"));

		strcpy_s(szText, CW2A(strTime));
		pTable->m_CurrentDate = szText;
		//pTable->m_MoveSpeed = float(((CJetMainDlg*)AfxGetMainWnd())->m_pJetPrintDlg->GetXMoveSpeed());
		pTable->m_XOrignPos = float(db_XPrintOrignPos);
		pTable->m_YOrignPos = float(db_YPrintOrignPos);
		pTable->m_XStepLenth = float(db_XStepLenth);
		pTable->m_YStepLenth = float(db_YStepLenth);
		pTable->m_XOffSet = float(db_XPos);
		pTable->m_YOffSet = float(db_YPos);
		pTable->Update();
	}

	//关闭数据库
	pTable->Close();

	

	SetDataGrid();
	m_MotionGridData.UpdateData();
	m_MotionGridData.Refresh();
}


void CPrintMotionControlView::OnBnClickedJetprintdtMotioncontrolDataDeleteButton()
{
	// TODO:  在此添加控件通知处理程序代码
	int nRow = m_nSelectedRow;
	CMotionTable *pTable = CMotionTable::Instance();
	if (pTable->open())
	{
		pTable->Move(nRow);
		pTable->Delete();
		pTable->Query();

		pTable->Update();
	}

	pTable->Close();



	m_MotionGridData.DeleteRow(nRow);

	SetDataGrid();
	m_MotionGridData.UpdateData();
	m_MotionGridData.Refresh();
}


void CPrintMotionControlView::SetCurrentPos(double dbXpos, double dbYpos)
{
	db_XPos = dbXpos;
	db_YPos = dbYpos;
}

void CPrintMotionControlView::GetCurrentPos(float &fXpos, float &fYpos)
{
	fXpos = (float)db_XPos;
	fYpos = (float)db_YPos;
}

void CPrintMotionControlView::UpdateControlState()
{
	CDeviceParker* pParker = nullptr;
#if _DEBUGE_PARKER
	pParker = CDeviceParker::Instance();
	bool bState = pParker->JetParkerIsConnected();
	EnableWindow(bState);
#endif
}

CString CPrintMotionControlView::GetModuleDir()
{
	//获取Debug或Release所在路径
	HMODULE module = GetModuleHandle(0);
	char chFileName[MAX_PATH];
	LPTSTR lpstrFileName = (LPTSTR)chFileName;
	GetModuleFileName(module, lpstrFileName, MAX_PATH);

	CString strExePath(lpstrFileName);
	int nPos = strExePath.ReverseFind(_T('\\'));
	if (nPos < 0)
	{
		return CString("");
	}
	else
	{
		return strExePath.Left(nPos);
	}
}

CString CPrintMotionControlView::GetProgPath()
{
	CString strCurrentFolder = _T("");
	CString strProgFolder = _T("");
	TCHAR progFolder[MAX_PATH];
	memset(progFolder, 0x00, sizeof(progFolder));
	GetModuleFileName(AfxGetInstanceHandle(), progFolder, MAX_PATH);

	strCurrentFolder = progFolder;
	int nPos = strCurrentFolder.ReverseFind('\\');
	strCurrentFolder = strCurrentFolder.Left(nPos);
	strProgFolder = strCurrentFolder + _T("\\ParkerProg");

	return strProgFolder;
}

void CPrintMotionControlView::EnableCheckThread()
{
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	bool bMotorXEnabled = pParker->JetParkerIsBitChecked(4360, 28);
	bool bMotorYEnabled = pParker->JetParkerIsBitChecked(4361, 28);
	if (!bMotorXEnabled || !bMotorYEnabled)
	{
		pParker->JetParkerEnableMotor();
	}
#endif
	m_pMotionCheckThread = AfxBeginThread(CheckMotionStatus, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
	if (NULL == m_pMotionCheckThread)
	{
		AfxMessageBox(_T("创建检测线程失败！"));
		return;
	}

	m_pMotionCheckThread->m_bAutoDelete = false;
	m_pMotionCheckThread->ResumeThread();
	SetEvent(m_hMotionEvent);
}

void CPrintMotionControlView::CheckStatus(double xPos, double yPos)
{
	//检测电机所在的位置，然后启用或是禁用某些按钮
	if (xPos >= 58.0)
	{
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XPOSITIVE_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPRIGHT_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XNEGATIVE_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPLEFT_BUTTON)->EnableWindow(TRUE);
	}

	if (xPos <= -58.0)
	{
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XPOSITIVE_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPRIGHT_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XNEGATIVE_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPLEFT_BUTTON)->EnableWindow(FALSE);
	}

	if (xPos<58.0 && xPos>-58.0)
	{
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XPOSITIVE_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPRIGHT_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XNEGATIVE_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPLEFT_BUTTON)->EnableWindow(TRUE);
	}

	if (yPos >= 63.0)
	{
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YPOSITIVE_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPFORWARD_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YNEGATIVE_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPBACKWARD_BUTTON)->EnableWindow(TRUE);
	}

	if (yPos <= -63.0)
	{
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YPOSITIVE_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPFORWARD_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YNEGATIVE_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPBACKWARD_BUTTON)->EnableWindow(FALSE);
	}

	if (yPos > -63.0 && yPos < 63.0)
	{
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YPOSITIVE_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPFORWARD_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YNEGATIVE_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_STEPBACKWARD_BUTTON)->EnableWindow(TRUE);
	}
}

UINT CPrintMotionControlView::CheckMotionStatus(LPVOID pVoid)
{
	CPrintMotionControlView* pParam = (CPrintMotionControlView*)(pVoid);

#if _DEBUGE_PARKER
	//CDeviceParker *pParkerStatus = CDeviceParker::Instance();
	CDeviceParker *pParkerStatus = new CDeviceParker();
	
	while (pParam->m_bMotionThreadRun)
	{
		WaitForSingleObject(pParam->m_hMotionEvent, INFINITE);
		ResetEvent(pParam->m_hMotionEvent);

		bool bMotorInNormalState = true;
		while (bMotorInNormalState)
		{
			if (false == pParam->m_bMotionThreadRun)
			{
				break;
			}

			if (true == pParam->m_bSuspendThread)
			{

			}
			else
			{
				if (pParkerStatus->JetParkerIsBitChecked(4112, 10))
				{
					bMotorInNormalState = false;
					::PostMessageA(pParam->m_hWnd, WM_MOTION_KILL, 0, KILL_ALL_MOVE);
				}

				if (pParkerStatus->JetParkerIsBitChecked(4360, 19) || pParkerStatus->JetParkerIsBitChecked(4600, 4) || pParkerStatus->JetParkerIsBitChecked(4600, 5))
				{
					bMotorInNormalState = false;
					::PostMessageA(pParam->m_hWnd, WM_MOTION_KILL, 0, AXIS_ZERO_KILL);
				}

				if (pParkerStatus->JetParkerIsBitChecked(4361, 19) || pParkerStatus->JetParkerIsBitChecked(4601, 4) || pParkerStatus->JetParkerIsBitChecked(4601, 5))
				{
					bMotorInNormalState = false;
					::PostMessageA(pParam->m_hWnd, WM_MOTION_KILL, 0, AXIS_ONE_KILL);
				}

				if (pParkerStatus->JetParkerIsBitChecked(4600, 7) || pParkerStatus->JetParkerIsBitChecked(4601, 7))
				{
					bMotorInNormalState = false;
					::PostMessageA(pParam->m_hWnd, WM_HOME_FAILED, 0, 0);
				}
			}


		}
	}

	pParkerStatus->JetParkerDisConnect();

	if (NULL != pParkerStatus)
	{
		pParkerStatus->JetParkerUnInitialize();
		delete pParkerStatus;
		pParkerStatus = NULL;
	}

	if (NULL != pParam->m_hMotionEvent)
	{
		CloseHandle(pParam->m_hMotionEvent);
	}
#endif
	return 0;
}

LRESULT CPrintMotionControlView::OnChangePosition(WPARAM wParam, LPARAM lParam)
{
	int nChioce = wParam;
	switch (wParam)
	{
		//Y+
	case 0:
		OnBnClickedJetprintdtMotioncontrolMotionStepforwardButton();
		break;

		//X-
	case 1:
		OnBnClickedJetprintdtMotioncontrolMotionStepleftButton();
		break;
		//X+
	case 2:
		OnBnClickedJetprintdtMotioncontrolMotionSteprightButton();
		break;

		//Y-
	case 3:
		OnBnClickedJetprintdtMotioncontrolMotionStepbackwardButton();
		break;
	}

	return 0;
}

LRESULT CPrintMotionControlView::OnChangeStepLenth(WPARAM wParam, LPARAM lParam)
{
	int nChioce = wParam;

	double dbStepLenth = 0.0;
	CString editText = _T("");
	switch (nChioce)
	{
	case 0:
		if (dbStepLenth < 20.0)
		{
			dbStepLenth = 20.0;
		}

		if (dbStepLenth > 2000.0)
		{
			dbStepLenth = 2000.0;
		}

		db_XStepLenth = dbStepLenth;
		editText.Format(_T("%.4f"), dbStepLenth);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_EDIT)->SetWindowText(editText);
		break;
	case 1:
		if (dbStepLenth < 20.0)
		{
			dbStepLenth = 20.0;
		}

		if (dbStepLenth > 2000.0)
		{
			dbStepLenth = 2000.0;
		}

		db_XStepLenth = dbStepLenth;
		editText.Format(_T("%.4f"), dbStepLenth);
		GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_EDIT)->SetWindowText(editText);
		break;
	}
	return 0;
}

LRESULT CPrintMotionControlView::OnMotionStatusChange(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = lParam;

	CDeviceParker *pParker = nullptr;

	switch (uMsg)
	{
		//Kill Axis X
	case 0:
		//首先清空状态位
#if _DEBUGE_PARKER
		pParker = CDeviceParker::Instance();
		pParker->m_controlChannel->SetFlag(8467, FALSE, FALSE);
		pParker->JetParkerEnableMotor();
#endif
		Sleep(3000);
		SetEvent(m_hMotionEvent);
		break;

		//kill Axis Y
	case 1:
#if _DEBUGE_PARKER
		pParker = CDeviceParker::Instance();
		pParker->m_controlChannel->SetFlag(8499, FALSE, FALSE);
		pParker->JetParkerEnableMotor();
#endif
		Sleep(3000);
		SetEvent(m_hMotionEvent);
		break;

		//Kill All Move Request
	case 2:
#if _DEBUGE_PARKER
		pParker = CDeviceParker::Instance();
		pParker->m_controlChannel->SetFlag(522, FALSE, FALSE);
		pParker->JetParkerEnableMotor();
#endif
		Sleep(3000);
		SetEvent(m_hMotionEvent);
		break;
	}

	return 0;
}

LRESULT CPrintMotionControlView::OnHomeFailed(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedJetprintdtMotioncontrolMotionMoveHomeButton();
	SetEvent(m_hMotionEvent);
	return 0;
}

LRESULT CPrintMotionControlView::OnMotionControlChange(WPARAM wParam, LPARAM lParam)
{
#if _DEBUGE_VISA

	CDeviceVisa *pVisa = CDeviceVisa::Instance();
	if (pVisa->IsDeviceConnected())
	{
		EnableCheckThread();
	}
#endif
	return 0;
}

LRESULT CPrintMotionControlView::OnPeletteJog(WPARAM wParam, LPARAM lParam)
{
	int nChioce = wParam;
	CDeviceParker *pParker = nullptr;
#if _DEBUGE_PARKER
	pParker = CDeviceParker::Instance();
	switch (nChioce)
	{
		
	case 0:
		pParker->JetParkerXJogPlus();
		break;

	case 1:
		pParker->JetParkerXJogMinus();
		break;

	case 2:
		pParker->JetParkerYJogPlus();
		break;

	case 3:
		pParker->JetParkerYJogMinum();
		break;
	}
#endif
	return 0;
}

LRESULT CPrintMotionControlView::OnPeletteStopJog(WPARAM wParam, LPARAM lParam)
{
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	pParker->JetParkerJogStop(0);
	pParker->JetParkerJogStop(1);
#endif
	return 0;
}

void CPrintMotionControlView::OnGridClick(NMHDR *pNotifyStruct, LRESULT *)
{
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW*)pNotifyStruct;
	int nSelectColum = pItem->iColumn;
	int nSelectRow = pItem->iRow;

	m_nSelectedRow = nSelectRow;

	int nColumCount = m_MotionGridData.GetColumnCount();
	int nRowCount = m_MotionGridData.GetRowCount();

	if (nSelectColum >= 0 && nSelectColum < nColumCount)
	{
		if (nSelectRow >= 1 && nSelectRow < nRowCount)
		{
			//如果选择的是第一列的单元
			if (nSelectColum == 0)
			{
				GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_DATA_APPLY_BUTTON)->EnableWindow(FALSE);
				GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_DATA_APPEND_BUTTON)->EnableWindow(TRUE);
				GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_DATA_DELETE_BUTTON)->EnableWindow(TRUE);
			}
			else
			{
				GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_DATA_APPLY_BUTTON)->EnableWindow(TRUE);
				GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_DATA_APPEND_BUTTON)->EnableWindow(TRUE);
				GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_DATA_DELETE_BUTTON)->EnableWindow(FALSE);
			}
		}
	}
}

void CPrintMotionControlView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	UINT nAxisChioce = 0;

#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	float fXPos = pParker->JetParkerPositionGet(nAxisChioce);

	nAxisChioce = 1;

	float fYPos = pParker->JetParkerPositionGet(nAxisChioce);

	if (TIME_UPDATE_POS == nIDEvent)
	{
		CPoint indicatorPos;
		int nXpos = 0;
		int nYpos = 0;

		m_JetPanelWnd.DevicePointToLogicPoint(fXPos, fYPos, nXpos, nYpos);


		indicatorPos.x = nXpos;
		indicatorPos.y = nYpos;

		m_JetPanelWnd.updateIndicatorPoint(indicatorPos);
	}

	if (TIME_CHECK_STATE == nIDEvent)
	{
		CheckStatus(fXPos, fYPos);
	}
#endif
	CResizableFormView::OnTimer(nIDEvent);
}


BOOL CPrintMotionControlView::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	CString strTemp = _T("");
	double db_EditValue = 0.0;

	if (WM_KEYDOWN == pMsg->message&&VK_RETURN == pMsg->wParam)
	{
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XTARGETPOS_EDIT))
		{
			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XTARGETPOS_EDIT)->GetWindowText(strTemp);
			db_EditValue = _wtof(strTemp);

			if (db_EditValue < -60.0)
			{
				db_EditValue = -60.0;
			}

			if (db_EditValue > 60.0)
			{
				db_EditValue = 60.0;
			}

			db_XTargetPos = db_EditValue;

			strTemp.Format(_T("%.4f"), db_EditValue);

			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XTARGETPOS_EDIT)->SetWindowText(strTemp);
		}

		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YTARGETPOS_EDIT))
		{
			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YTARGETPOS_EDIT)->GetWindowText(strTemp);
			db_EditValue = _wtof(strTemp);

			if (db_EditValue < -65.0)
			{
				db_EditValue = -65.0;
			}

			if (db_EditValue > 65.0)
			{
				db_EditValue = 65.0;
			}

			db_YTargetPos = db_EditValue;

			strTemp.Format(_T("%.4f"), db_EditValue);

			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YTARGETPOS_EDIT)->SetWindowText(strTemp);
		}

		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_EDIT))
		{
			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_EDIT)->GetWindowText(strTemp);
			db_EditValue = _wtof(strTemp);


			if (db_EditValue < 20.0)
			{
				db_EditValue = 20.0;
			}

			if (db_EditValue > 2000.0)
			{
				db_EditValue = 2000.0;
			}

			db_XStepLenth = db_EditValue;

			strTemp.Format(_T("%.4f"), db_EditValue);

			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XSTEPLENTH_EDIT)->SetWindowText(strTemp);

		}

		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_EDIT))
		{
			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_EDIT)->GetWindowText(strTemp);
			db_EditValue = _wtof(strTemp);


			if (db_EditValue < 20.0)
			{
				db_EditValue = 20.0;
			}

			if (db_EditValue > 2000.0)
			{
				db_EditValue = 2000.0;
			}

			db_YStepLenth = db_EditValue;

			strTemp.Format(_T("%.4f"), db_EditValue);

			GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YSTEPLENTH_EDIT)->SetWindowText(strTemp);

		}

		return TRUE;
	}

	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XPOSITIVE_BUTTON)->m_hWnd)
		{
			//X Jog Positive
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			pParker->JetParkerXJogPlus();
			pParker->m_bIsJogMode = true;
#endif
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YPOSITIVE_BUTTON)->m_hWnd)
		{
			//Y Jog Positive
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			pParker->JetParkerYJogPlus();
			pParker->m_bIsJogMode = true;
#endif
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XNEGATIVE_BUTTON)->m_hWnd)
		{
			//X Jog Negative
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			pParker->JetParkerXJogMinus();
			pParker->m_bIsJogMode = true;
#endif
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YNEGATIVE_BUTTON)->m_hWnd)
		{
			//Y Jog Negative
#if _DEBUGE_PARKER
			CDeviceParker *pParker = CDeviceParker::Instance();
			pParker->JetParkerYJogMinum();
			pParker->m_bIsJogMode = true;
#endif 
		}

		return CResizableFormView::PreTranslateMessage(pMsg);
	}


	if (pMsg->message == WM_LBUTTONUP)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XPOSITIVE_BUTTON)->m_hWnd)
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

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YPOSITIVE_BUTTON)->m_hWnd)
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

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_XNEGATIVE_BUTTON)->m_hWnd)
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

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_MOTIONCONTROL_MOTION_YNEGATIVE_BUTTON)->m_hWnd)
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

		return CResizableFormView::PreTranslateMessage(pMsg);
	}
	return CResizableFormView::PreTranslateMessage(pMsg);
}

bool CPrintMotionControlView::TerminateMotionThread()
{
	bool bResult = false;

	m_bMotionThreadRun = false;
	SetEvent(m_hMotionEvent);

	//WaitForSingleObject(m_pMotionCheckThread->m_hThread,INFINITE);

	KillTimer(TIME_CHECK_STATE);
	KillTimer(TIME_UPDATE_POS);

	bResult = true;

	return bResult;
}