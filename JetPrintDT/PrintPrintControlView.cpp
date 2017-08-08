// PrintPrintControlView.cpp : 实现文件
//

#include "stdafx.h"
#include "JetPrintDT.h"
#include "MainFrm.h"
#include "PrintPrintControlView.h"
#include "Device/DeviceParker.h"
#include "Device/DeviceVisa.h"
#include "Device/DeviceHarvardPump.h"
#include "SingleTon/ParamInitSingleton.h"
#include "PrintParamSetView.h"
#include "PrintMotionControlView.h"
#include "PrintFileOperateView.h"

// CPrintPrintControlView

IMPLEMENT_DYNCREATE(CPrintPrintControlView, CResizableFormView)

CPrintPrintControlView::CPrintPrintControlView()
: CResizableFormView(CPrintPrintControlView::IDD)
{
	m_dbXSpeed = 0.0;
	m_dbYSpeed = 0.0;
	m_dbXPosition = 0.0;
	m_dbYPosition = 0.0;

	m_bSetAgilentOutput = false;

	m_pRunProgThread = NULL;

	m_bRunThreadSuspended = true;

	m_hRunProgEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_bIsParamIntialized = false;

	m_pPrintPoint = NULL;
	m_strCurrentFileExt = _T("");
	m_pWaitMessageThread = NULL;

	m_dbPlacodeXLenth = 0.0;
	m_dbPlacodeYLenth = 0.0;
	m_dbXOffSet = 0.0;
	m_dbYOffSet = 0.0;
	m_PlacodeType = BT_ELLIPSE;
	m_bEnableUpdateView = false;
	m_bStopPrintFlag = false;
	m_bProgRunning = false;
	m_bCheckProgState = false;
	m_ProgState = STATE_NULL;
}

CPrintPrintControlView::~CPrintPrintControlView()
{
	if (NULL != m_JetDrawView.m_Drawing.m_pJetDxfDimView)
	{
		//m_JetDrawView.m_Drawing.drwUninitialize();
	}
}

void CPrintPrintControlView::DoDataExchange(CDataExchange* pDX)
{
	CResizableFormView::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_JETPRINTDT_PRINT_PRINTCONTROL_CURRENT_IMAGE_STATIC, m_JetDrawView);
	DDX_Control(pDX, IDC_JETPRINTDT_PRINT_PRINTCONTROL_CURRNET_IMAGE_PLACODESIZE_COMBO, m_PlacodeCombo);
}

BEGIN_MESSAGE_MAP(CPrintPrintControlView, CResizableFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SETSPEED_BUTTON, &CPrintPrintControlView::OnBnClickedJetprintdtPrintPrintcontrolSetspeedButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STARTPRINT_BUTTON, &CPrintPrintControlView::OnBnClickedJetprintdtPrintPrintcontrolStartprintButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STOPPRINT_BUTTON, &CPrintPrintControlView::OnBnClickedJetprintdtPrintPrintcontrolStopprintButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_PRINTCONTROL_HOME_BUTTON, &CPrintPrintControlView::OnBnClickedJetprintdtPrintPrintcontrolHomeButton)
	ON_CBN_SELCHANGE(IDC_JETPRINTDT_PRINT_PRINTCONTROL_CURRNET_IMAGE_PLACODESIZE_COMBO, &CPrintPrintControlView::OnSelchangeJetPrintPlacodeTypeCombo)
	ON_MESSAGE(WM_PROG_FINISHED, &CPrintPrintControlView::OnProgFinished)
	ON_MESSAGE(WM_FILE_LOADED, &CPrintPrintControlView::OnFileLoaded)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPrintPrintControlView 诊断

#ifdef _DEBUG
void CPrintPrintControlView::AssertValid() const
{
	CResizableFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPrintPrintControlView::Dump(CDumpContext& dc) const
{
	CResizableFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPrintPrintControlView 消息处理程序


BOOL CPrintPrintControlView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CResizableFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CPrintPrintControlView::OnInitialUpdate()
{

	//print control group
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_GROUP, CSize(0, 0), CSize(50, 50));
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SPEED_STATIC, CSize(20, 0), CSize(20, 20));
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SPEED_EDIT, CSize(20, 0), CSize(20, 0));
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SPEED_UNIT_STATIC, CSize(20, 0), CSize(20, 20));
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SETSPEED_BUTTON, CSize(0,5), CSize(50, 15));
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STARTPRINT_BUTTON, CSize(0,15), CSize(50, 25));
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STOPPRINT_BUTTON, CSize(0,25), CSize(50, 35));
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_HOME_BUTTON, CSize(0,35), CSize(50, 45));

	//trail group
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_REALTIME_TRACK_GROUP, CSize(50, 0), CSize(100, 50));

	//device status group
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_DEVICE_STATUS_GROUP, CSize(0, 50), CSize(50, 100));

	//current image group
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_CURRENT_IMAGE_GROUP, CSize(50, 50), CSize(100, 100));
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_CURRNET_IMAGE_PLACODESIZE_STATIC, CSize(50, 50), CSize(50, 50));
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_CURRNET_IMAGE_PLACODESIZE_COMBO, CSize(50, 50), CSize(50, 50));
	AddAnchor(IDC_JETPRINTDT_PRINT_PRINTCONTROL_CURRENT_IMAGE_STATIC, CSize(50, 50), CSize(100, 100));

	CResizableFormView::OnInitialUpdate();
	// TODO:  在此添加专用代码和/或调用基类
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	InitializeTrailGraph();
	InitializeDeviceGraph();
	InitializeImageGraph();
	InitializeCombo();
	//设置速度
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	m_dbXSpeed = pParker->JetParkerMasterSpeedGet();
#endif
	CString speedText = _T("");
	speedText.Format(_T("%.4f"), m_dbXSpeed);
	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SPEED_EDIT)->SetWindowText(speedText);

	if (NULL == m_pRunProgThread)
	{
		m_pRunProgThread = AfxBeginThread(StartPrint, this, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
		if (NULL == m_pRunProgThread)
		{
			AfxMessageBox(_T("开启运行监测线程失败！"));
		}
		else
		{
			m_pRunProgThread->m_bAutoDelete = false;
			m_pRunProgThread->ResumeThread();
			m_bRunThreadSuspended = true;
		}
	}

	if (NULL == m_pWaitMessageThread)
	{
		m_pWaitMessageThread = (CWaitMessageThread *)AfxBeginThread(RUNTIME_CLASS(CWaitMessageThread));
		if (NULL == m_pWaitMessageThread)
		{
			AfxMessageBox(_T("等待停止界面线程创建失败！"));
		}
	}

	UpdateOrignPos();
}


void CPrintPrintControlView::InitializeTrailGraph(){
	CRect chartRect;
	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_REALTIME_TRACK_GROUP)->GetClientRect(&chartRect);

	chartRect.left += 4;
	chartRect.top += 12;
	chartRect.bottom -= 4;
	chartRect.right -= 4;

	m_ChartCtrl.Create(GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_REALTIME_TRACK_GROUP), chartRect, IDC_JETPRINTDT_PRINT_PRINTCONTROL_TRAIL_GRAPH);
	if (NULL == m_ChartCtrl.m_hWnd)
	{
		return ;
	}

	CChartStandardAxis *pButomAxis = NULL;
	CChartStandardAxis *pLeftAxis = NULL;
	pButomAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	if (NULL == pButomAxis)
		return ;

	pLeftAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	if (NULL == pLeftAxis)
		return ;

	pButomAxis->SetAutomatic(true);
	pButomAxis->SetAutomaticMode(CChartAxis::FullAutomatic);
	pButomAxis->SetTextColor(RGB(255, 0, 0));
	pLeftAxis->SetAutomatic(true);
	pLeftAxis->SetAutomaticMode(CChartAxis::FullAutomatic);
	pLeftAxis->SetTextColor(RGB(255, 0, 0));
	pLeftAxis->EnableScrollBar(false);

	COLORREF BackColor = RGB(0, 0, 0);
	COLORREF GridColor = RGB(0, 180, 0);
	COLORREF TextColor = RGB(0, 180, 0);

	m_ChartCtrl.SetEdgeType(EDGE_SUNKEN);
	m_ChartCtrl.SetBackColor(BackColor);
	m_ChartCtrl.SetBorderColor(TextColor);
}

void CPrintPrintControlView::InitializeImageGraph(){
	CRect imageRect;
	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_CURRENT_IMAGE_GROUP)->GetClientRect(&imageRect);
	imageRect.left += 2;
	imageRect.right -= 2;
	imageRect.top += 24;
	imageRect.bottom -= 2;

	m_JetDrawView.Create(NULL, WS_CHILD | WS_VISIBLE, imageRect, GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_CURRENT_IMAGE_GROUP));
	if (nullptr == m_JetDrawView.m_hWnd)
		return;
}

void CPrintPrintControlView::InitializeDeviceGraph(){
	//Create GridList
	CRect gridRect;
	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_DEVICE_STATUS_GROUP)->GetClientRect(&gridRect);

	gridRect.left += 2;
	gridRect.right -= 2;
	gridRect.top += 12;
	gridRect.bottom -= 2;

	m_JetPrintGridList.Create(gridRect, GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_DEVICE_STATUS_GROUP), IDC_JETPRINTDT_PRINT_PRINTCONTROL_DEVICE_GRID);
	if (NULL == m_JetPrintGridList.m_hWnd)
	{
		//创建List失败
		return;
	}

	//初始化列表的各项
	int nFixedColum = 1;
	int nFixedRow = 1;
	int nColumCount = 2;
	int nRowCount = 12;

	m_JetPrintGridList.SetEditable(false);
	m_JetPrintGridList.EnableSelection(false);
	m_JetPrintGridList.SetAutoSizeStyle();
	try
	{
		m_JetPrintGridList.SetColumnCount(nColumCount);
		m_JetPrintGridList.SetRowCount(nRowCount);
		m_JetPrintGridList.SetFixedColumnCount(nFixedColum);
		m_JetPrintGridList.SetFixedRowCount(nFixedRow);

	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		return ;
	}

	UINT colorR = 0;
	UINT colorG = 0;
	UINT colorB = 0;
	int row = 0;
	CString strList = _T("");
	GV_ITEM item;
	item.crBkClr = RGB(128, 128, 128);
	item.mask = GVIF_TEXT;
	item.row = row;
	item.col = 0;

	strList.Format(_T("X位置"));
	item.strText = strList;
	colorR = 255;
	colorG = 0;
	colorB = 0;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	++row;
	strList.Format(_T("Y位置"));
	item.strText = strList;
	colorR = 224;
	colorG = 255;
	colorB = 255;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	++row;
	strList.Format(_T("X速度"));
	item.strText = strList;
	colorR = 255;
	colorG = 0;
	colorB = 0;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	++row;
	strList.Format(_T("Y速度"));
	item.strText = strList;
	colorR = 224;
	colorG = 255;
	colorB = 255;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	++row;
	strList.Format(_T("X电机状态"));
	item.strText = strList;
	colorR = 255;
	colorG = 0;
	colorB = 0;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	++row;
	strList.Format(_T("Y电机状态"));
	item.strText = strList;
	colorR = 224;
	colorG = 255;
	colorB = 255;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	++row;
	strList.Format(_T("流量泵流量"));
	item.strText = strList;
	colorR = 255;
	colorG = 0;
	colorB = 0;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	++row;
	strList.Format(_T("电压峰值"));
	item.strText = strList;
	colorR = 224;
	colorG = 255;
	colorB = 255;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	++row;
	strList.Format(_T("启动时间"));
	item.strText = strList;
	colorR = 255;
	colorG = 0;
	colorB = 0;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	++row;
	strList.Format(_T("当前时间"));
	item.strText = strList;
	colorR = 224;
	colorG = 255;
	colorB = 255;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	++row;
	strList.Format(_T("打印起点X"));
	item.strText = strList;
	colorR = 255;
	colorG = 0;
	colorB = 0;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	++row;
	strList.Format(_T("打印起点Y"));
	item.strText = strList;
	colorR = 224;
	colorG = 255;
	colorB = 255;
	item.row = row;
	item.crBkClr = RGB(colorR, colorG, colorB);
	item.mask |= GVIF_BKCLR;
	m_JetPrintGridList.SetItem(&item);

	CRect listRect;
	m_JetPrintGridList.GetClientRect(&listRect);
	int nListWidth = listRect.Width();
	m_JetPrintGridList.SetColumnWidth(0, (nListWidth) / 2);
	m_JetPrintGridList.SetColumnWidth(1, (nListWidth) / 2);

	/*for (int i = 0; i < m_JetPrintGridList.GetRowCount(); i++)
	{
		m_JetPrintGridList.SetRowHeight(i, listRect.Height() / m_JetPrintGridList.GetRowCount());
	}*/

	UpdateData(FALSE);
}

CString CPrintPrintControlView::GetProgPath()
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
void CPrintPrintControlView::InitializeCombo()
{
	CString comboText = _T("");

	comboText.Format(_T("1英寸硅片"));
	m_PlacodeCombo.AddString(comboText);
	comboText.Format(_T("2英寸硅片"));
	m_PlacodeCombo.AddString(comboText);
	comboText.Format(_T("4英寸硅片"));
	m_PlacodeCombo.AddString(comboText);
	comboText.Format(_T("自定义尺寸"));
	m_PlacodeCombo.AddString(comboText);

	m_PlacodeCombo.SetCurSel(0);
	OnSelchangeJetPrintPlacodeTypeCombo();
}

UINT CPrintPrintControlView::StartPrint(LPVOID pVoid)
{
	CPrintPrintControlView *param = (CPrintPrintControlView*)(pVoid);
	CDeviceParker *pParker = nullptr;

#if _DEBUGE_PARKER
	pParker = CDeviceParker::Instance();
#endif

	while (param->m_bRunProgThread)
	{
		WaitForSingleObject(param->m_hRunProgEvent, INFINITE);
		ResetEvent(param->m_hRunProgEvent);

		if (false == param->m_bRunProgThread)
		{
			break;
		}

		bool bProgRunning = true;
		while (bProgRunning)
		{
			if (false == param->m_bProgRunning)
			{
				param->m_bProgRunning = true;
				break;
			}

			if (!pParker->JetParkerIsBitChecked(4128, 0))
			{
				bProgRunning = false;

				param->m_bRunThreadSuspended = true;
				//::PostMessageA(param->m_hWnd,WM_PROG_FINISHED,0,0);
				param->OnProgFinished(0, 0);

				break;
			}
		}
	}

	
	return 0;
}

void CPrintPrintControlView::UpdateView(int nType)
{
	switch (nType)
	{
		//DXF Type
	case 0:
		m_JetDrawView.SetXOffSet(m_dbXOffSet);
		m_JetDrawView.SetYOffSet(m_dbYOffSet);
		m_JetDrawView.SetXLenth(m_dbPlacodeXLenth);
		m_JetDrawView.SetYLenth(m_dbPlacodeYLenth);
		m_JetDrawView.EnableDrawBorder(true);
		m_JetDrawView.SetBorderType(m_PlacodeType);
		m_JetDrawView.graphType = DXF_TYPE;

		m_JetDrawView.RedrawWindow();
		break;

		//BMP Type
	case 1:
		m_JetDrawView.m_pJetBmpAnalysis->SetXOffSet(m_dbXOffSet);
		m_JetDrawView.m_pJetBmpAnalysis->SetYOffSet(m_dbYOffSet);
		m_JetDrawView.m_pJetBmpAnalysis->SetXLenth(m_dbPlacodeXLenth);
		m_JetDrawView.m_pJetBmpAnalysis->SetYLenth(m_dbPlacodeYLenth);

		m_JetDrawView.m_pJetBmpAnalysis->SetBorderType(m_PlacodeType);
		m_JetDrawView.EnableDrawBorder(true);
		m_JetDrawView.graphType = BMP_TYPE;
		m_JetDrawView.RedrawWindow();
		break;
	}
}

void CPrintPrintControlView::UpdateGridList()
{
	bool bXStopState = false;  //X电机停止状态判断
	bool bYStopState = false;  //Y电机停止状态判断

	CDeviceParker* pParker = nullptr;
#if _DEBUGE_PARKER
	pParker = CDeviceParker::Instance();
#endif
	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);


	UINT nAxisChioce = 0;
	int col = 1;
	int row = 0;
	CString strList = _T("");
	GV_ITEM item;
	item.mask = GVIF_TEXT;
	UINT colorR = 0;
	UINT colorG = 0;
	UINT colorB = 0;
	item.col = col;

	//取得X电机的位置
	float fData = static_cast<float>(m_dbXPosition);
	strList.Format(_T("%.4f"), fData);
	m_JetPrintGridList.SetItemText(row, col, strList);

	//取得Y电机的位置
	++row;
	nAxisChioce = 1;
	fData = static_cast<float>(m_dbYPosition);
	strList.Format(_T("%.4f"), fData);
	m_JetPrintGridList.SetItemText(row, col, strList);

	//取得X电机的速度
	++row;
	nAxisChioce = 0;
	fData = static_cast<float>(m_dbXSpeed);
	fData = fData / 2000.00f;
	strList.Format(_T("%.4f"), fData);
	m_JetPrintGridList.SetItemText(row, col, strList);

	//取得Y电机的速度
	++row;
	nAxisChioce = 1;
	fData = static_cast<float>(m_dbYSpeed);
	fData = fData / 2000.00f;
	strList.Format(_T("%.4f"), fData);
	m_JetPrintGridList.SetItemText(row, col, strList);

	//取得X电机的状态
	++row;
	//检测电机是否处于Jog状态
#if _DEBUGE_PARKER
	if (pParker->JetParkerIsBitChecked(4120, 24) || pParker->JetParkerIsBitChecked(4128, 0))
	{
		strList.Format(_T("X电机启动"));
	}
	else
	{
		strList.Format(_T("X电机停止"));
		bXStopState = true;
	}
	m_JetPrintGridList.SetItemText(row, col, strList);
#endif

	//取得Y电机的状态
	++row;
#if _DEBUGE_PARKER
	if (pParker->JetParkerIsBitChecked(4121, 24) || pParker->JetParkerIsBitChecked(4128, 0))
	{
		strList.Format(_T("Y电机启动"));
	}
	else
	{
		strList.Format(_T("Y电机停止"));
		bYStopState = true;
	}
	m_JetPrintGridList.SetItemText(row, col, strList);
#endif

	CPrintParamSetView *pParamView = nullptr;
	if (pMainFrame){
		pParamView = (CPrintParamSetView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][PARAMSET_VIEW]);
	}
	
	//取得流量泵的流量
	++row;
	if (pParamView){
		strList.Format(_T("%.4f"), pParamView->GetInfuseRate());
		m_JetPrintGridList.SetItemText(row, col, strList);
	}
	
	

	//取得输出的峰值
	++row;
	if (pParamView->IsInACMode())
	{
		double dbAmplitude = pParamView->db_AC_Amplitude;
		double dbOffset = pParamView->db_AC_Offset;
		strList.Format(_T("%.4f"), (dbAmplitude / 2.0 + dbOffset));
	}
	else
	{
		double dbOffset = pParamView->db_DC_Offset;
		strList.Format(_T("%.4f"), dbOffset);
	}
	m_JetPrintGridList.SetItemText(row, col, strList);

	//取得启动时间
	++row;
	CParamInitSingleton *pParamSingleton = CParamInitSingleton::Instance();
	if (pParamSingleton)
		strList = pParamSingleton->GetStartTime();
	m_JetPrintGridList.SetItemText(row, col, strList);

	//取得当前时间
	++row;
	CTime currentTime = CTime::GetCurrentTime();
	strList = currentTime.Format(_T("%H:%M:%S"));
	m_JetPrintGridList.SetItemText(row, col, strList);

	//取得打印起点X
	CPrintMotionControlView *pMotionView = nullptr;
	if (pMainFrame){
		pMotionView = (CPrintMotionControlView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW]);
	}

	++row;
	if (pMotionView)
		strList.Format(_T("%.4f"), pMotionView->db_XPrintOrignPos);
	m_JetPrintGridList.SetItemText(row, col, strList);

	//取得打印起点Y
	++row;
	if (pMotionView)
		strList.Format(_T("%.4f"), pMotionView->db_YPrintOrignPos);
	m_JetPrintGridList.SetItemText(row, col, strList);

	m_JetPrintGridList.Refresh();
	UpdateData(FALSE);

	if (m_ProgState == STATE_STOPED)
	{
		if (m_bStopPrintFlag && NULL != m_pWaitMessageThread)
		{
			if (bXStopState && bYStopState)
			{
				//获得XY电机同时停止状态时,隐藏等待停止窗口
				m_ProgState = STATE_NULL;
				m_bStopPrintFlag = false;
				m_pWaitMessageThread->PostThreadMessage(UM_WAITMESSAGEDLG, 1, 0);
			}
		}
	}

	if (m_ProgState == STATE_RUN)
	{
		if (m_bCheckProgState)
		{
			if (bXStopState&&bYStopState)
			{
				m_ProgState = STATE_NULL;
				m_bCheckProgState = false;
				OnProgFinished(0, 0);
			}
		}
	}

}

void CPrintPrintControlView::OnSize(UINT nType, int cx, int cy)
{
	CResizableFormView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	if (NULL != m_ChartCtrl.m_hWnd)
	{
		CRect chartRect;
		GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_REALTIME_TRACK_GROUP)->GetClientRect(&chartRect);
		chartRect.left = chartRect.left + 4;
		chartRect.top = chartRect.top + 24;
		chartRect.bottom = chartRect.bottom - 4;
		chartRect.right = chartRect.right - 4;

		m_ChartCtrl.MoveWindow(chartRect);
	}

	if (NULL != m_JetPrintGridList.m_hWnd){
		CRect gridRect;
		GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_DEVICE_STATUS_GROUP)->GetClientRect(&gridRect);
		//ScreenToClient(gridRect);
		gridRect.left = gridRect.left + 4;
		gridRect.top = gridRect.top + 24;
		gridRect.bottom = gridRect.bottom - 4;
		gridRect.right = gridRect.right - 4;

		m_JetPrintGridList.MoveWindow(gridRect);
		m_JetPrintGridList.ExpandColumnsToFit(TRUE);
	}

	if (NULL != m_JetDrawView.m_hWnd){
		CRect imageRect;
		GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_CURRENT_IMAGE_GROUP)->GetClientRect(&imageRect);

		imageRect.left += 2;
		imageRect.right -= 2;
		imageRect.top += 48;
		imageRect.bottom -= 2;
		m_JetDrawView.MoveWindow(imageRect);
	}
}


void CPrintPrintControlView::OnBnClickedJetprintdtPrintPrintcontrolSetspeedButton()
{
	// TODO:  在此添加控件通知处理程序代码
	//首先设置为Prog0模式
	double xSpeed = 0.0;
	double ySpeed = 0.0;

	CString strEditValue = _T("");
	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SPEED_EDIT)->GetWindowText(strEditValue);
	xSpeed = _wtof(strEditValue);

	//检查速度是否超出限制
	if (xSpeed > 350.0)
	{
		xSpeed = 350.0;
	}
	m_dbXSpeed = xSpeed;

#if _DEBUGE_PARKER
	CDeviceParker* pParker = CDeviceParker::Instance();
	if (pParker->JetParkerSetProg())
	{
		pParker->JetParkerMoveVelSet(xSpeed);
	}
#endif

	CParamInitSingleton *pParam = CParamInitSingleton::Instance();
	if (pParam)
		pParam->SetPrintSpeed(xSpeed);

}


void CPrintPrintControlView::OnBnClickedJetprintdtPrintPrintcontrolStartprintButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintFileOperateView *pFileView = nullptr;
	if (pMainFrame){
		pFileView = (CPrintFileOperateView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW]);
	}

	if (false == pFileView->m_bIsFileDownloadSucceed)
	{
		AfxMessageBox(_T("文件未加载或已回零,请加载文件！"));
		return;
	}

	if (NULL != m_pPrintPoint)
		m_pPrintPoint->ClearSerie();

	if (m_strCurrentFileExt.MakeUpper() == _T("BMP"))
	{
		CChartAxis *pLeftAxis = m_ChartCtrl.GetLeftAxis();
		pLeftAxis->SetInverted(true);
		CChartAxis *pButtomAxis = m_ChartCtrl.GetBottomAxis();
		pButtomAxis->SetInverted(true);
	}
	else
	{
		CChartAxis *pLeftAxis = m_ChartCtrl.GetLeftAxis();
		pLeftAxis->SetInverted(true);
		CChartAxis *pButtomAxis = m_ChartCtrl.GetBottomAxis();
		pButtomAxis->SetInverted(true);
	}

	m_ChartCtrl.RemoveAllSeries();
	m_pPrintPoint = m_ChartCtrl.CreatePointsSerie();
	m_pPrintPoint->SetPointType(CChartPointsSerie::ptEllipse);
	m_pPrintPoint->SetPointSize(2, 2);
	m_pPrintPoint->SetBorderColor(RGB(255, 0, 0));
	m_pPrintPoint->SetVisible(true);
	m_pPrintPoint->SetSeriesOrdering(poNoOrdering);

	Sleep(10);

	SetTimer(TIME_PRINT, 1, NULL);

	m_bProgRunning = true;
	m_bRunProgThread = true;
	m_bRunThreadSuspended = true;

	Sleep(10);
#if _DEBUGE_VISA
	CDeviceVisa *pVisa = CDeviceVisa::Instance();
	if (pVisa)
		pVisa->SetOutput(FALSE);
#endif

	CDeviceParker *pParker = nullptr;
#if _DEBUGE_PARKER
	pParker = CDeviceParker::Instance();
#endif 
	pParker->JetParkerRunProgram();
	//SetEvent(m_hRunProgEvent);


	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SETSPEED_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STARTPRINT_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STOPPRINT_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_HOME_BUTTON)->EnableWindow(FALSE);

	//将文件操作界面两按钮置灰
	pFileView->GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEDOWNLOAD_BUTTON)->EnableWindow(FALSE);
	pFileView->GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEOPEN_BUTTON)->EnableWindow(FALSE);
}


void CPrintPrintControlView::OnBnClickedJetprintdtPrintPrintcontrolStopprintButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CDeviceParker *pParker = nullptr;
#if _DEBUGE_PARKER
	pParker = CDeviceParker::Instance();
#endif

	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	
	if (pParker->JetParkerIsBitChecked(4128, 0))
	{
		m_ProgState = STATE_STOPED;
		if (NULL != m_pWaitMessageThread)
		{
			m_pWaitMessageThread->PostThreadMessage(UM_WAITMESSAGEDLG, 0, 0);
		}

		GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STOPPRINT_BUTTON)->EnableWindow(FALSE);

		//停止程序的执行
		pParker->m_controlChannel->SetFlag(522, TRUE, TRUE);

		pParker->m_controlChannel->SetFlag(8467, TRUE, TRUE);
		pParker->m_controlChannel->SetFlag(8499, TRUE, TRUE);

		//结束程序运行的线程
		m_bProgRunning = false;
		Sleep(300);

		KillTimer(TIME_PRINT);
		m_bStopPrintFlag = true;

		//关闭函数发生器的输出
		//theApp.m_JetAglientFunc.SetOutput(FALSE);
		pParker->m_controlChannel->SetFlag(32, VARIANT_FALSE, VARIANT_TRUE);

	
		pMainFrame->StopCheckStatus();
	}
	else
	{
		//更新每个按钮的状态
		GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SETSPEED_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STARTPRINT_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STOPPRINT_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_HOME_BUTTON)->EnableWindow(TRUE);

		CPrintFileOperateView *pFileView = nullptr;
		if (pMainFrame){
			pFileView = (CPrintFileOperateView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW]);
		}

		//将文件操作界面两按钮置灰
		pFileView->GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEDOWNLOAD_BUTTON)->EnableWindow(TRUE);
		pFileView->GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEOPEN_BUTTON)->EnableWindow(TRUE);
	}

	//流量泵停止射流
	CDeviceHarvardPump *pPump = CDeviceHarvardPump::Instance();
	pPump->Stop();

#if _DEBUGE_VISA
	CDeviceVisa *pVisa = CDeviceVisa::Instance();
	if (pVisa)
		pVisa->SetOutput(FALSE);
#endif

	CPrintParamSetView *pParamView = nullptr;
	if (pMainFrame){
		pParamView = (CPrintParamSetView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][PARAMSET_VIEW]);
	}
	
	pParamView->SetFlowComponentState(true, true, true, true, false);
}


void CPrintPrintControlView::OnBnClickedJetprintdtPrintPrintcontrolHomeButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintFileOperateView *pFileView = nullptr;
	if (pMainFrame){
		pFileView = (CPrintFileOperateView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW]);
	}
	
	pFileView->m_bIsFileDownloadSucceed = false;

	CDeviceParker *pParker = nullptr;
#if _DEBUGE_PARKER
	pParker = CDeviceParker::Instance();
#endif

	bool bDownLoadResult = false;
	CString strFileName = _T("");
	CString strFilePath = _T("");
	strFilePath = GetProgPath();
	strFileName = _T("\\HOME.txt");
	strFilePath += strFileName;
	bDownLoadResult = pParker->JetParkerDownloadFile(strFilePath);

	if (bDownLoadResult)
	{
		pParker->JetParkerRunProgram();
	}
}


void CPrintPrintControlView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDeviceParker* pParker = nullptr;
	CDeviceVisa *pVisa = nullptr;
#if _DEBUGE_PARKER
	 pParker = CDeviceParker::Instance();
#endif

#if _DEBUGE_VISA
	pVisa = CDeviceVisa::Instance();
#endif

	if (TIME_PRINT == nIDEvent)
	{
		//如果线程处于挂起的状态，检测程序是否已经开始运行
		if (true == m_bRunThreadSuspended)
		{
			//如果检测到程序运行
			if (pParker->JetParkerIsBitChecked(4128, 0))
			{
				if (NULL != m_pRunProgThread)
				{
					//线程开始执行
					m_bRunThreadSuspended = false;
					SetEvent(m_hRunProgEvent);

					m_bCheckProgState = true;
					m_ProgState = STATE_RUN;
				}

				pVisa->SetOutput(TRUE);
				pParker->m_controlChannel->SetFlag(32, VARIANT_FALSE, VARIANT_TRUE);
			}

		}

		bool bIsBitSet = false;
		//Bit 34
		bIsBitSet = pParker->JetParkerIsBitChecked(4097, 0);
		if (bIsBitSet)
		{
			if (NULL != m_pPrintPoint)
			{
				m_pPrintPoint->AddPoint(m_dbXPosition, m_dbYPosition);

			}
		}
	}


	if (TIME_UPDATE_STATE == nIDEvent)
	{
		UpdateGridList();
	}
	CResizableFormView::OnTimer(nIDEvent);
}


LRESULT CPrintPrintControlView::OnProgFinished(WPARAM wParam, LPARAM lParam)
{
	KillTimer(TIME_PRINT);

	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SETSPEED_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STARTPRINT_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STOPPRINT_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_HOME_BUTTON)->EnableWindow(TRUE);
	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintFileOperateView *pFileView = nullptr;
	if (pMainFrame){
		pFileView = (CPrintFileOperateView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW]);
	}

	//将文件操作界面两按钮置灰
	pFileView->GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEDOWNLOAD_BUTTON)->EnableWindow(TRUE);
	pFileView->GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEOPEN_BUTTON)->EnableWindow(TRUE);

	//关闭函数发生器的输出
	CDeviceVisa *pVisa = nullptr;
#if _DEBUGE_VISA
	pVisa = CDeviceVisa::Instance();
	pVisa->SetOutput(FALSE);
#endif
	//m_PrintParker.m_controlChannel->SetFlag(35,VARIANT_FALSE,VARIANT_TRUE);
	
	pMainFrame->StopCheckStatus();

	//流量泵停止射流
	CDeviceHarvardPump *pPump = CDeviceHarvardPump::Instance();
	pPump->Stop();

	CPrintParamSetView *pParamView = nullptr;
	if (pMainFrame){
		pParamView = (CPrintParamSetView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][PARAMSET_VIEW]);
	}

	pParamView->SetFlowComponentState(true, true, true, true, false);

	AfxMessageBox(_T("文件打印完毕！"));

	return 0;
}

LRESULT CPrintPrintControlView::OnFileLoaded(WPARAM wParam, LPARAM lParam)
{
	int nChioce = (int)(lParam);
	switch (nChioce)
	{
		//DXF Type
	case 0:
	{
		m_JetDrawView.graphType = NULL_TYPE;

		m_JetDrawView.RedrawWindow();

		CJetDxfDimView *pDimView = (CJetDxfDimView*)(wParam);

		if (NULL == m_JetDrawView.m_Drawing.m_pJetDxfDimView)
		{
			m_JetDrawView.m_Drawing.drwInitialize();
		}

		if (NULL != pDimView)
		{
			memset(m_JetDrawView.m_Drawing.m_pJetDxfDimView->m_pDrawing, 0, sizeof(DRAWING));

			memcpy(m_JetDrawView.m_Drawing.m_pJetDxfDimView->m_pDrawing, pDimView->m_pDrawing, sizeof(DRAWING));
		}

		CRect viewRect;
		m_JetDrawView.GetClientRect(&viewRect);
		m_JetDrawView.m_Drawing.drwInitView(0, 0, viewRect.Width(), viewRect.Height());
		m_JetDrawView.m_Drawing.drwZoomExtents();


		m_JetDrawView.SetXOffSet(m_dbXOffSet);
		m_JetDrawView.SetYOffSet(m_dbYOffSet);
		m_JetDrawView.SetXLenth(m_dbPlacodeXLenth);
		m_JetDrawView.SetYLenth(m_dbPlacodeYLenth);
		m_JetDrawView.EnableDrawBorder(true);
		m_JetDrawView.SetBorderType(m_PlacodeType);
		m_JetDrawView.graphType = DXF_TYPE;

		if (m_JetDrawView.m_hWnd)
			m_JetDrawView.RedrawWindow();

		break;
	}


	//BMP Type
	case 1:
	{
		CJetBmpAnalysis *pBmpAnalysis = (CJetBmpAnalysis*)(wParam);
		if (NULL != pBmpAnalysis)
		{
			m_JetDrawView.m_pJetBmpAnalysis = pBmpAnalysis;
			m_JetDrawView.m_pJetBmpAnalysis->SetXOffSet(m_dbXOffSet);
			m_JetDrawView.m_pJetBmpAnalysis->SetYOffSet(m_dbYOffSet);
			m_JetDrawView.m_pJetBmpAnalysis->SetXLenth(m_dbPlacodeXLenth);
			m_JetDrawView.m_pJetBmpAnalysis->SetYLenth(m_dbPlacodeYLenth);

			m_JetDrawView.m_pJetBmpAnalysis->SetBorderType(m_PlacodeType);
			m_JetDrawView.EnableDrawBorder(true);
		}

		m_JetDrawView.graphType = BMP_TYPE;

		if (m_JetDrawView.m_hWnd)
			m_JetDrawView.RedrawWindow();
		break;
	}

	}

	m_bEnableUpdateView = true;
	return 0;
}



void CPrintPrintControlView::OnSelchangeJetPrintPlacodeTypeCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	int i = 0;
	i = m_PlacodeCombo.GetCurSel();

	if (i != CB_ERR)
	{
		switch (i)
		{
		case 0:
			m_dbPlacodeXLenth = 25.4;
			m_dbPlacodeYLenth = 25.4;
			m_PlacodeType = BT_ELLIPSE;
			break;
		case 1:
			m_dbPlacodeXLenth = 50.8;
			m_dbPlacodeYLenth = 50.8;
			m_PlacodeType = BT_ELLIPSE;
			break;
		case 2:
			m_dbPlacodeXLenth = 101.6;
			m_dbPlacodeYLenth = 101.6;
			m_PlacodeType = BT_ELLIPSE;
			break;
		case 3:
			if (IDOK == m_PlacodeModifyDlg.DoModal())
			{
				m_dbPlacodeXLenth = m_PlacodeModifyDlg.m_PlacodeLenth;
				m_dbPlacodeYLenth = m_PlacodeModifyDlg.m_PlacodeWidth;
				m_PlacodeType = BT_RECT;
			}
			break;
		}
	}

	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintMotionControlView *pMotionView = nullptr;
	if (pMainFrame){
		pMotionView = (CPrintMotionControlView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW]);
	}
	double dbXOffSet = pMotionView->db_XPrintOrignPos;
	double dbYOffSet = pMotionView->db_YPrintOrignPos;

	m_dbXOffSet = dbXOffSet;
	m_dbYOffSet = dbYOffSet;

	if (m_bEnableUpdateView)
	{
		if (m_strCurrentFileExt.MakeUpper() == _T("BMP"))
		{
			UpdateView(1);
		}
		else
		{
			UpdateView(0);
		}
	}
}

void CPrintPrintControlView::UpdatePosition(double xPos, double yPos)
{
	m_dbXPosition = xPos;
	m_dbYPosition = yPos;

	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintFileOperateView *pFileView = nullptr;
	if (pMainFrame){
		pFileView = (CPrintFileOperateView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW]);
	}
	if (nullptr != pFileView->m_pMatchView){
		pFileView->m_pMatchView->UpdatePosition(xPos, yPos);
	}
}

void CPrintPrintControlView::UpdateSpeed(double xSpeed, double ySpeed)
{
	m_dbXSpeed = xSpeed;
	m_dbYSpeed = ySpeed;
}

void CPrintPrintControlView::SetXMoveSpeed(double xSpeed)
{
	m_dbXSpeed = xSpeed;
}

void CPrintPrintControlView::SetYMoveSpeed(double ySpeed)
{
	m_dbYSpeed = ySpeed;
}

double CPrintPrintControlView::GetXMoveSpeed()
{
	return m_dbXSpeed;
}

double CPrintPrintControlView::GetYMoveSpeed()
{
	return m_dbYSpeed;
}

void CPrintPrintControlView::SetFileExt(CString fileExt)
{
	if (fileExt.IsEmpty())
		return;

	if (fileExt.MakeUpper() == _T("BMP"))
	{
		m_strCurrentFileExt = fileExt;
	}
	else
	{
		m_strCurrentFileExt = _T("");
	}
}

void CPrintPrintControlView::UpdateDeviceStatus(){
	SetTimer(TIME_UPDATE_STATE, 1000, NULL);
}

void CPrintPrintControlView::UpdateOrignPos()
{

	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintMotionControlView *pMotionView = nullptr;
	if (pMainFrame){
		pMotionView = (CPrintMotionControlView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][MOTIONCONTROL_VIEW]);
	}
	
	double dbXOffSet = pMotionView->db_XPrintOrignPos;
	double dbYOffSet = pMotionView->db_YPrintOrignPos;

	m_dbXOffSet = dbXOffSet;
	m_dbYOffSet = dbYOffSet;

	if (m_bEnableUpdateView)
	{
		if (m_strCurrentFileExt.MakeUpper() == _T("BMP"))
		{
			UpdateView(1);
		}
		else
		{
			UpdateView(0);
		}
	}
}

void CPrintPrintControlView::ExitWatchPrintThread()
{
	if (NULL != m_pRunProgThread)
	{
		m_bRunProgThread = false;
		SetEvent(m_hRunProgEvent);

		Sleep(10);
		WaitForSingleObject(m_pRunProgThread->m_hThread, INFINITE);
		CloseHandle(m_pRunProgThread->m_hThread);
		m_pRunProgThread = NULL;

		CloseHandle(m_hRunProgEvent);
	}

	KillTimer(TIME_UPDATE_STATE);
}

void CPrintPrintControlView::ExitWaitMessageUIThread()
{
	if (NULL != m_pWaitMessageThread)
	{
		m_pWaitMessageThread->ExitInstance();
		CloseHandle(m_pWaitMessageThread->m_hThread);
		m_pWaitMessageThread = NULL;
	}
}

void CPrintPrintControlView::OnDraw(CDC* /*pDC*/)
{
	// TODO:  在此添加专用代码和/或调用基类

}
