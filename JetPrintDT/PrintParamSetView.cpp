// MainPRINTLeftView.cpp : 实现文件
//
#include "stdafx.h"
#include "JetPrintDT.h"
#include "MainFrm.h"
#include "PrintParamSetView.h"
#include "SingleTon/ParamInitSingleton.h"
#include "Database/ParamTable.h"
#include "Device/DeviceHarvardPump.h"
#include "Device/DeviceVisa.h"
// CPrintParamSetView

IMPLEMENT_DYNCREATE(CPrintParamSetView, CResizableFormView)

CPrintParamSetView::CPrintParamSetView()
: CResizableFormView(CPrintParamSetView::IDD)
, m_VoltageModeRadio(1)
{
	m_bPumpOpen = false;
	db_InfuseRate = 0.0;
	m_dbNozzleDiameter = 4.606;

	b_AcMode = true;
	db_DC_Amplitude = 0.0;
	db_DC_Freq = 0.0;
	db_DC_Offset = 0.0;
	db_DC_Ratio = 0.0;

	db_AC_Amplitude = 0.0;
	db_AC_Freq = 0.0;
	db_AC_Offset = 0.0;
	db_AC_Ratio = 0.0;

	
	m_dbNozzleVolume = 0.0;
	m_bFlowVoltageOpenFlag = false;

	m_nSelectRow = -1;
	m_Material = _T("");
}

CPrintParamSetView::~CPrintParamSetView()
{
}

void CPrintParamSetView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_JETPRINTDT_PARAMSET_VOLTAGE_DCMODE_RADIO, m_VoltageModeRadio);
}

BEGIN_MESSAGE_MAP(CPrintParamSetView, CResizableFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_JETPRINTDT_PARAMSET_VOLTAGE_DCMODE_RADIO, &CPrintParamSetView::OnClickedJetprintdtParamsetVoltageDcmodeRadio)
	ON_BN_CLICKED(IDC_JETPRINTDT_PARAMSET_VOLTAGE_ACMODE_RADIO,&CPrintParamSetView::OnClickedJetprintdtParamsetVoltageDcmodeRadio)
	ON_BN_CLICKED(IDC_JETPRINTDT_PARAMSET_FLOW_STARTEJECT_BUTTON, &CPrintParamSetView::OnBnClickedJetprintdtParamsetFlowStartejectButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PARAMSET_FLOW_STOPEJECT_BUTTON, &CPrintParamSetView::OnBnClickedJetprintdtParamsetFlowStopejectButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PARAMSET_VOLTAGE_START_BUTTON, &CPrintParamSetView::OnBnClickedJetprintdtParamsetVoltageStartButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PARAMSET_VOLTAGE_STOP_BUTTON, &CPrintParamSetView::OnBnClickedJetprintdtParamsetVoltageStopButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PARAMSET_DATA_APPLY_BUTTON, &CPrintParamSetView::OnBnClickedJetprintdtParamsetDataApplyButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PARAMSET_DATA_MODIFY_BUTTON, &CPrintParamSetView::OnBnClickedJetprintdtParamsetDataModifyButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PARAMSET_DATA_APPEND_BUTTON, &CPrintParamSetView::OnBnClickedJetprintdtParamsetDataAppendButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PARAMSET_DATA_DELETE_BUTTON, &CPrintParamSetView::OnBnClickedJetprintdtParamsetDataDeleteButton)
	ON_NOTIFY(UDN_DELTAPOS, IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_SPIN, &CPrintParamSetView::OnDeltaposJetprintdtParamsetVoltageOffsetSpin)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_SPIN, &CPrintParamSetView::OnNMReleasedcaptureJetprintdtParamsetVoltageOffsetSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_SPIN, &CPrintParamSetView::OnDeltaposJetprintdtParamsetVoltageAmplitudeSpin)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_SPIN, &CPrintParamSetView::OnNMReleasedcaptureJetprintdtParamsetVoltageAmplitudeSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_SPIN, &CPrintParamSetView::OnDeltaposJetprintdtParamsetVoltageFreqSpin)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_SPIN, &CPrintParamSetView::OnNMReleasedcaptureJetprintdtParamsetVoltageFreqSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_SPIN, &CPrintParamSetView::OnDeltaposJetprintdtParamsetVoltageRatioSpin)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_SPIN, &CPrintParamSetView::OnNMReleasedcaptureJetprintdtParamsetVoltageRatioSpin)
	ON_NOTIFY(NM_CLICK, IDC_JETPRINTDT_PARAMSET_DATA_GRID, &CPrintParamSetView::OnGridClick)
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_JETPRINTDT_PARAMSET_DATA_GRID, &CPrintParamSetView::OnGridStartEdit)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_JETPRINTDT_PARAMSET_DATA_GRID, &CPrintParamSetView::OnGridEndEdit)
END_MESSAGE_MAP()


// CPrintParamSetView 诊断

#ifdef _DEBUG
void CPrintParamSetView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPrintParamSetView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPrintParamSetView 消息处理程序


BOOL CPrintParamSetView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CResizableFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


int CPrintParamSetView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CResizableFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	GetClientRect(&m_RectOrign);
	return 0;
}

void CPrintParamSetView::AdjustView(){
	CRect clientRect;
	GetClientRect(&clientRect);
	double dbXRate = 0.0;
	double dbYRate = 0.0;
	dbXRate = clientRect.Width() / (double)m_RectOrign.Width();
	dbYRate = clientRect.Height() / (double)m_RectOrign.Height();

	if (clientRect.Width() != 0 && clientRect.Height() != 0)
	{
		m_RectOrign = clientRect;

		CWnd* pWnd = NULL;
		pWnd = GetWindow(GW_CHILD);
		while (pWnd)
		{
			CString  strCaption = _T("");
			pWnd->GetWindowTextW(strCaption);
			OutputDebugString(TEXT("Caption:") + strCaption + TEXT("\n"));

			CRect  childRect;
			pWnd->GetWindowRect(&childRect);
			ScreenToClient(&childRect);

			childRect.left = (LONG)(childRect.left*dbXRate);
			childRect.top = (LONG)(childRect.top*dbYRate);
			childRect.right = (LONG)(childRect.right*dbXRate);
			childRect.bottom = (LONG)(childRect.bottom*dbYRate);

			pWnd->MoveWindow(&childRect, TRUE);

			pWnd = pWnd->GetWindow(GW_HWNDNEXT);
		}
	}
}

bool CPrintParamSetView::InitializeCharAxis(){
	CChartStandardAxis *pButomAxis = NULL;
	CChartStandardAxis *pLeftAxis = NULL;
	pButomAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	if (NULL == pButomAxis)
		return false;

	pLeftAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	if (NULL == pLeftAxis)
		return false;
	pLeftAxis->EnableScrollBar(FALSE);
	return false;
}

bool CPrintParamSetView::InitializeChart(){
	bool bResult = false;

	m_pLineSerial = m_ChartCtrl.CreateLineSerie();
	if (NULL == m_pLineSerial)
	{
		bResult = false;
		return bResult;
	}


	COLORREF BackColor = RGB(96, 156, 252);
	COLORREF GridColor = RGB(0, 180, 0);
	COLORREF TextColor = RGB(0, 180, 0);

	m_ChartCtrl.SetEdgeType(EDGE_SUNKEN);
	m_ChartCtrl.SetBackColor(BackColor);
	m_ChartCtrl.SetBorderColor(TextColor);
	bResult = true;
	return bResult;
}


void CPrintParamSetView::SetFlowComponentState(bool bEditEnabled, \
	bool bForwardEnabled,\
	bool bBackwardEnabled, \
	bool bRunEnabled, \
	bool bStopEnabled)
{
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FLOWSET_EDIT)->EnableWindow(bEditEnabled);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FASTFORWARD_BUTTON)->EnableWindow(bForwardEnabled);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FASTBACKWARD_BUTTON)->EnableWindow(bBackwardEnabled);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_STARTEJECT_BUTTON)->EnableWindow(bRunEnabled);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_STOPEJECT_BUTTON)->EnableWindow(bStopEnabled);
}

void CPrintParamSetView::DrawLine(){
	UpdateData(true);
	CChartAxis *pLeftAxis = m_ChartCtrl.GetLeftAxis();
	CChartAxis *pButtomAxis = m_ChartCtrl.GetBottomAxis();

	if (NULL == pLeftAxis || NULL == pButtomAxis)
		return;


	double db_yMax = 0.0;
	double db_yOffSet = 0.0;
	double db_xRatio = 0.0;
	double db__max = 0.0;
	//如果是交流模式
	if (b_AcMode)
	{
		db_yMax = db_AC_Amplitude;
		db_yOffSet = db_AC_Offset;
		db_xRatio = db_AC_Ratio;
		db__max = db_AC_Freq;
		double db_xOffset = (100.0 - db_AC_Ratio) / 100.0*db__max / 2.0;

		pLeftAxis->SetMinMax(-db_yMax + db_yOffSet, db_yMax + db_yOffSet);
		pButtomAxis->SetMinMax(0.0, db__max);

		double db_StartPos_x = 0 + db_xOffset;
		double db_StartPos_y = -db_yMax / 2.0 + db_yOffSet;

		double db_leftdownPos_x = db_xRatio / 100.0*db__max / 4.0*1.0 + db_xOffset;
		double db_leftDownPos_y = -db_yMax / 2.0 + db_yOffSet;

		double db_leftUpPos_x = db_xRatio / 100.0*db__max / 4.0*1.0 + db_xOffset;
		double db_leftUpPos_y = db_yMax / 2.0 + db_yOffSet;

		double db_rightUpPos_x = db_xRatio / 100.0*db__max / 4.0*3.0 + db_xOffset;
		double db_rightUpPos_y = db_yMax / 2.0 + db_yOffSet;

		double db_rightDownPos_x = db_xRatio / 100.0*db__max / 4.0*3.0 + db_xOffset;
		double db_rightDownPos_y = -db_yMax / 2.0 + db_yOffSet;

		double db_EndPos_x = db_xRatio / 100.0*db__max + db_xOffset;
		double db_EndPos_y = -db_yMax / 2.0 + db_yOffSet;

		m_pLineSerial->ClearSerie();

		m_pLineSerial->AddPoint(db_StartPos_x, db_StartPos_y);
		m_pLineSerial->AddPoint(db_leftdownPos_x, db_leftDownPos_y);
		m_pLineSerial->AddPoint(db_leftUpPos_x, db_leftUpPos_y);
		m_pLineSerial->AddPoint(db_rightUpPos_x, db_rightUpPos_y);
		m_pLineSerial->AddPoint(db_rightDownPos_x, db_rightDownPos_y);
		m_pLineSerial->AddPoint(db_EndPos_x, db_EndPos_y);

		TChartStringStream ssText;
		ssText << _T("(") << db_leftUpPos_x << _T(",") << db_leftUpPos_y << _T(")");
		m_pLineSerial->CreateBalloonLabel(2, ssText.str());

	}

	else
	{
		db_yMax = db_DC_Offset;
		db_yOffSet = db_DC_Offset;
		db_xRatio = db_DC_Ratio;
		db__max = 1000;

		pLeftAxis->SetMinMax(0.0, 2.0*db_yMax);
		pButtomAxis->SetMinMax(0.0, db__max);

		double db_StartPos_x = 0;
		double db_StartPos_y = db_yOffSet;
		double db_EndPos_x = 1000.00;
		double db_EndPos_y = db_yOffSet;

		m_pLineSerial->ClearSerie();

		m_pLineSerial->AddPoint(db_StartPos_x, db_StartPos_y);
		m_pLineSerial->AddPoint(db_EndPos_x, db_EndPos_y);
	}
}

void CPrintParamSetView::OnSize(UINT nType, int cx, int cy)
{
	CResizableFormView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	
	if (NULL != m_ChartCtrl.m_hWnd)
	{
		CRect chartRect;
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_GRAPH_STATIC)->GetClientRect(&chartRect);
		chartRect.left = chartRect.left + 4;
		chartRect.top = chartRect.top + 4;
		chartRect.bottom = chartRect.bottom - 4;
		chartRect.right = chartRect.right - 4;

		m_ChartCtrl.MoveWindow(chartRect);
	}

	if (NULL != m_DataGrid.m_hWnd){
		CRect gridRect;
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_GRID_STATIC)->GetWindowRect(&gridRect);
		ScreenToClient(gridRect);
		gridRect.left = gridRect.left + 4;
		gridRect.top = gridRect.top + 4;
		gridRect.bottom = gridRect.bottom - 4;
		gridRect.right = gridRect.right - 4;

		m_DataGrid.MoveWindow(gridRect);
		m_DataGrid.ExpandColumnsToFit(TRUE);
	}

}


void CPrintParamSetView::OnInitialUpdate()
{
	//FlowSet Group
	AddAnchor(IDC_JETPRINTDT_PARAMSET_FLOW_GROUP, TOP_LEFT, CSize(50,50));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_FLOW_FLOWSET_STATIC, CSize(20,0),CSize(20,20));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_FLOW_FLOWSET_EDIT,CSize(20,0),CSize(20,0));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_FLOW_FLOWSET_UNIT_STATIC, CSize(20,0),CSize(20,20));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_FLOW_FASTFORWARD_BUTTON, CSize(0,5),CSize(50,25));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_FLOW_FASTBACKWARD_BUTTON, CSize(0,15),CSize(50,35));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_FLOW_STARTEJECT_BUTTON, CSize(0,25),CSize(50,45));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_FLOW_STOPEJECT_BUTTON, CSize(0,35),CSize(50,55));


	//VoltageAdjust Group
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_GROUP, CSize(50,0), CSize(100,50));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_DCMODE_RADIO, CSize(50, 0));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_ACMODE_RADIO, CSize(100, 0));

	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_STATIC, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_SPIN, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_UNIT_STATIC, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_STATIC, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_SPIN, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_UNIT_STATIC, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_START_BUTTON, CSize(60, 5),CSize(100,5));

	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_STATIC, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_SPIN, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_UNIT_STATIC, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_STATIC, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_SPIN, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_UNIT_STATIC, CSize(50, 5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_STOP_BUTTON, CSize(60, 5),CSize(100,5));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_VOLTAGE_GRAPH_STATIC, CSize(50, 10),CSize(100,50));

	AddAnchor(IDC_JETPRINTDT_PARAMSET_DATA_GROUP, CSize(0,50), CSize(100,100));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_DATA_GRID_STATIC, CSize(0, 50), CSize(90, 100));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_DATA_APPLY_BUTTON, CSize(90, 55),CSize(100,65));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_DATA_MODIFY_BUTTON, CSize(90, 65),CSize(100,75));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_DATA_APPEND_BUTTON, CSize(90, 75),CSize(100,85));
	AddAnchor(IDC_JETPRINTDT_PARAMSET_DATA_DELETE_BUTTON, CSize(90, 85), CSize(100, 95));
	CResizableFormView::OnInitialUpdate();

	// TODO:  在此添加专用代码和/或调用基类

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();


	//Initialize Chart
	CRect chartRect;
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_GRAPH_STATIC)->GetClientRect(&chartRect);

	m_ChartCtrl.Create(GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_GRAPH_STATIC), chartRect, IDC_JETPRINTDT_PARAMSET_VOLTAGE_GRAPH_CHART);
	if (NULL == m_ChartCtrl.m_hWnd)
	{
		return ;
	}

	InitializeCharAxis();
	InitializeChart();
	InitializeDataGrid();

	//Initialize Param
	CParamInitSingleton *pParamInstance = CParamInitSingleton::Instance();
	db_InfuseRate = pParamInstance->GetFlow();

	db_DC_Amplitude = pParamInstance->GetDCPeak();
	db_DC_Freq = pParamInstance->GetDCFreq();
	db_DC_Offset = pParamInstance->GetDCOffset();
	db_DC_Ratio = pParamInstance->GetDCRatio();

	db_AC_Amplitude = pParamInstance->GetACPeak();
	db_AC_Freq = pParamInstance->GetACFreq();
	db_AC_Offset = pParamInstance->GetACOffset();
	db_AC_Ratio = pParamInstance->GetACRatio();

	CString editText = _T("");
	editText.Format(_T("%.4f"), db_InfuseRate);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FLOWSET_EDIT)->SetWindowText(editText);

	CString strEdit = _T("");
	//更新每个Edit框中的内容
	if (NULL != GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->m_hWnd)
	{
		strEdit.Format(_T("%.2f"), db_AC_Amplitude);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->SetWindowText(strEdit);
	}
	if (NULL != GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->m_hWnd)
	{
		strEdit.Format(_T("%.2f"), db_AC_Freq);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->SetWindowText(strEdit);
	}
	if (NULL != GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT)->m_hWnd)
	{
		strEdit.Format(_T("%.2f"), db_AC_Ratio);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT)->SetWindowText(strEdit);
	}
	if (NULL != GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->m_hWnd)
	{
		strEdit.Format(_T("%.2f"), db_AC_Offset);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->SetWindowText(strEdit);
	}

	DrawLine();

	SetDataGrid();

	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_APPLY_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_APPEND_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_DELETE_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_MODIFY_BUTTON)->EnableWindow(FALSE);

	OnClickedJetprintdtParamsetVoltageDcmodeRadio();
}


void CPrintParamSetView::OnClickedJetprintdtParamsetVoltageDcmodeRadio()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strTemp = _T("");
	CDeviceVisa* pVisa = nullptr;

	UpdateData(TRUE);
	switch (m_VoltageModeRadio)
	{
	case 1:
		b_AcMode = true;
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_STATIC)->EnableWindow(b_AcMode);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->EnableWindow(b_AcMode);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_SPIN)->EnableWindow(b_AcMode);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_UNIT_STATIC)->EnableWindow(b_AcMode);

		strTemp.Format(_T("%.2f"), db_AC_Amplitude);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->SetWindowText(strTemp);

		strTemp.Format(_T("%.2f"), db_AC_Freq);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->SetWindowTextW(strTemp);

		strTemp.Format(_T("%.2f"), db_AC_Offset);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->SetWindowTextW(strTemp);

		strTemp.Format(_T("%.2f"), db_AC_Ratio);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT)->SetWindowTextW(strTemp);

#if _DEBUGE_MODE
		pVisa = CDeviceVisa::Instance();
		pVisa->SetSquareFunction(); 
		pVisa->SetAmplitude(db_AC_Amplitude);
		pVisa->SetFrequency(db_AC_Freq);
		pVisa->SetDCycle(db_AC_Ratio);
		pVisa->SetOffset((long)db_AC_Offset);
#endif
		break;
	case 0:
		//Set to DC Mode
		b_AcMode = false;

		db_DC_Amplitude = 0.0;
		strTemp.Format(_T("%.2f"), db_DC_Amplitude);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->SetWindowText(strTemp);

		db_DC_Freq = 0.0;
		strTemp.Format(_T("%.2f"), db_DC_Freq);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->SetWindowTextW(strTemp);

		strTemp.Format(_T("%.2f"), db_DC_Offset);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->SetWindowTextW(strTemp);

		db_DC_Ratio = 100.0;
		strTemp.Format(_T("%.2f"), db_DC_Ratio);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT)->SetWindowTextW(strTemp);

		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_STATIC)->EnableWindow(!b_AcMode);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->EnableWindow(!b_AcMode);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_SPIN)->EnableWindow(!b_AcMode);
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_UNIT_STATIC)->EnableWindow(!b_AcMode);


#if _DEBUGE_MODE
		pVisa = CDeviceVisa::Instance();
		pVisa->SetDCFunction();
		pVisa->SetOffset((long)db_DC_Offset);
#endif
		break;
	}

	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_UNIT_STATIC)->EnableWindow(b_AcMode);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_STATIC)->EnableWindow(b_AcMode);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->EnableWindow(b_AcMode);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_SPIN)->EnableWindow(b_AcMode);

	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_STATIC)->EnableWindow(b_AcMode);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->EnableWindow(b_AcMode);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_SPIN)->EnableWindow(b_AcMode);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_UNIT_STATIC)->EnableWindow(b_AcMode);

	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_STATIC)->EnableWindow(b_AcMode);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT)->EnableWindow(b_AcMode);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_SPIN)->EnableWindow(b_AcMode);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_UNIT_STATIC)->EnableWindow(b_AcMode);



	DrawLine();
}


void CPrintParamSetView::OnBnClickedJetprintdtParamsetFlowStartejectButton()
{
	// TODO:  在此添加控件通知处理程序代码
#if _DEBUGE_MODE
	CDeviceHarvardPump *pPump = CDeviceHarvardPump::Instance();
	bool bOpen = pPump->m_comm.IsOpen()?true:false;
	if (bOpen)
	{
		CString infuseText = _T("");
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FLOWSET_EDIT)->GetWindowText(infuseText);

		db_InfuseRate = _wtof(infuseText);

		pPump->SetInfuseRate(static_cast<int>(db_InfuseRate));
		pPump->Run();
		m_bPumpOpen = true;

		CParamInitSingleton *pParam = CParamInitSingleton::Instance();
		pParam->SetFlow(db_InfuseRate);
	}
#endif

	SetFlowComponentState(false, false, false, false, true);
}


void CPrintParamSetView::OnBnClickedJetprintdtParamsetFlowStopejectButton()
{
	// TODO:  在此添加控件通知处理程序代码
#if _DEBUGE_MODE
	if (m_bPumpOpen)
	{
		CDeviceHarvardPump *pPump = CDeviceHarvardPump::Instance();
		pPump->Stop();
		m_bPumpOpen = false;
	}
#endif

	SetFlowComponentState(true, true, true, true, false);
}


void CPrintParamSetView::OnBnClickedJetprintdtParamsetVoltageStartButton()
{
	// TODO:  在此添加控件通知处理程序代码
#if _DEBUGE_VISA
	CDeviceVisa *pVisa = CDeviceVisa::Instance();

	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);

	CParamInitSingleton *pParam = CParamInitSingleton::Instance();
	if (b_AcMode)
	{
		CString dataText = _T("");

		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->GetWindowText(dataText);
		db_AC_Amplitude = _wtof(dataText);
		pVisa->SetAmplitude(db_AC_Amplitude);
		pParam->SetACPeak(db_AC_Amplitude);

		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->GetWindowText(dataText);
		db_AC_Offset = _wtof(dataText);
		pVisa->SetOffset((long)db_AC_Offset);
		pParam->SetACOffset(db_AC_Offset);

		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->GetWindowText(dataText);
		db_AC_Freq = _wtof(dataText);
		pVisa->SetFrequency(db_AC_Freq);
		pParam->SetACFreq(db_AC_Freq);

		
		::PostMessageA(pMainFrame->m_hWnd, WM_FREQ_CHANGED, 0, LPARAM(db_AC_Freq));

		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT)->GetWindowText(dataText);
		db_AC_Ratio = _wtof(dataText);
		pVisa->SetDCycle(db_AC_Ratio);
		pParam->SetACRatio(db_AC_Ratio);

	}
	else
	{
		CString dataText = _T("");
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->GetWindowText(dataText);
		db_DC_Offset = _wtof(dataText);
		pVisa->SetOffset((long)db_DC_Offset);
	}

	DrawLine();

	pVisa->SetOutput(TRUE);

	if (b_AcMode)
	{
		//SetBit 32

		::PostMessageA(pMainFrame->m_hWnd, WM_SET_OUTPUT, 0, 1);
	}

	m_bFlowVoltageOpenFlag = true;

	if (b_AcMode)
	{
		if ((db_AC_Offset > 100 || db_AC_Amplitude > 100))
		{
			pMainFrame->StartCheckStatus();
		}
	}
	else{
		if (db_DC_Offset > 400)
		{
			pMainFrame->StartCheckStatus();
		}
	}
#endif
	
}


void CPrintParamSetView::OnBnClickedJetprintdtParamsetVoltageStopButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
#if _DEBUGE_MODE
	CDeviceVisa *pVisa = CDeviceVisa::Instance();
	pVisa->SetOutput(FALSE);
	if (b_AcMode)
	{
		//ClrBit 32

		::PostMessageA(pMainFrame->m_hWnd, WM_SET_OUTPUT, 0, 0);
	}

#endif
	m_bFlowVoltageOpenFlag = false;

	
	pMainFrame->StopCheckStatus();
}


BOOL CPrintParamSetView::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CString strTemp = _T("");
	double db_EditValue = 0.0;
	CParamInitSingleton *pParam = CParamInitSingleton::Instance();
	if (WM_KEYDOWN == pMsg->message&&VK_RETURN == pMsg->wParam)
	{
		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT))
		{
			UpdateData(TRUE);
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->GetWindowText(strTemp);

			db_EditValue = _wtof(strTemp);

			if (db_EditValue >= 0.00 && db_EditValue <= 5000.0)
			{
				if (b_AcMode){
					db_AC_Amplitude = db_EditValue;
					pParam->SetACPeak(db_AC_Amplitude);
				}
				else{
					db_DC_Amplitude = db_EditValue;
					pParam->SetDCPeak(db_DC_Amplitude);
				}
					
			}

			else
			{
				db_EditValue = 0.0;

				if (b_AcMode){
					db_AC_Amplitude = db_EditValue;
					pParam->SetACPeak(db_AC_Amplitude);
				}
					
				else{
					db_DC_Amplitude = db_EditValue;
					pParam->SetDCPeak(db_DC_Amplitude);
				}
					
			}

			strTemp.Format(_T("%.2f"), db_EditValue);
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->SetWindowText(strTemp);

			if (b_AcMode)
			{
#if _DEBUGE_MODE
				CDeviceVisa* pVisa = CDeviceVisa::Instance();
				pVisa->SetAmplitude(db_AC_Amplitude);
#endif
			}
			UpdateData(false);
			DrawLine();
		}


		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT))
		{
			UpdateData(TRUE);
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->GetWindowText(strTemp);

			db_EditValue = _wtof(strTemp);

			if (db_EditValue >= 0.00 && db_EditValue <= 10000.00)
			{
				if (b_AcMode)
					db_AC_Freq = db_EditValue;
				else
					db_DC_Freq = db_EditValue;
			}

			else
			{
				db_EditValue = 0.0;

				if (b_AcMode)
					db_AC_Freq = db_EditValue;
				else
					db_DC_Freq = db_EditValue;

				strTemp.Format(_T("%.2f"), db_EditValue);

				GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->SetWindowText(strTemp);
			}

			if (b_AcMode)
			{
#if _DEBUGE_MODE
				CDeviceVisa* pVisa = CDeviceVisa::Instance();
				pVisa->SetFrequency(db_AC_Freq);

				::PostMessageA(pMainFrame->m_hWnd, WM_FREQ_CHANGED, 0, LPARAM(db_AC_Freq));
#endif
			}

			UpdateData(false);
			DrawLine();

		}

		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT))
		{
			UpdateData(TRUE);
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->GetWindowText(strTemp);

			db_EditValue = _wtof(strTemp);

			if (db_EditValue >= 0.00 && db_EditValue <= 3000.00)
			{
				if (b_AcMode)
					db_AC_Offset = db_EditValue;
				else
					db_DC_Offset = db_EditValue;
			}

			else
			{
				db_EditValue = 0.0;

				if (b_AcMode)
					db_AC_Offset = db_EditValue;
				else
					db_DC_Offset = db_EditValue;

				strTemp.Format(_T("%.2f"), db_EditValue);

				GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->SetWindowText(strTemp);
			}

			if (b_AcMode)
			{
#if _DEBUGE_MODE
				CDeviceVisa* pVisa = CDeviceVisa::Instance();
				
				pVisa->SetOffset((long)db_AC_Offset);
#endif
			}
			else
			{
#if _DEBUGE_MODE
				CDeviceVisa* pVisa = CDeviceVisa::Instance();
				
				pVisa->SetOffset((long)db_DC_Offset);
#endif
			}

			UpdateData(false);
			DrawLine();
		}

		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT))
		{
			UpdateData(TRUE);
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT)->GetWindowText(strTemp);

			db_EditValue = _wtof(strTemp);

			if (db_EditValue >= 0.00 && db_EditValue <= 100.00)
			{
				if (b_AcMode)
					db_AC_Ratio = db_EditValue;
				else
					db_DC_Ratio = db_EditValue;
			}

			else
			{
				db_EditValue = 0.0;

				if (b_AcMode)
					db_AC_Ratio = db_EditValue;
				else
					db_DC_Ratio = db_EditValue;

				strTemp.Format(_T("%.2f"), db_EditValue);

				GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT)->SetWindowText(strTemp);
			}

			if (b_AcMode)
			{
#if _DEBUGE_MODE
				CDeviceVisa* pVisa = CDeviceVisa::Instance();
				pVisa->SetDCycle(db_AC_Ratio);
#endif
			}
			UpdateData(false);
			DrawLine();

		}

		if (GetFocus() == GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FLOWSET_EDIT))
		{
			UpdateData(TRUE);
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FLOWSET_EDIT)->GetWindowText(strTemp);

			db_EditValue = _wtof(strTemp);

			if (db_EditValue <= 10.0)
			{
				db_EditValue = 10.0;
				db_InfuseRate = db_EditValue;
				strTemp.Format(_T("%.4f"), db_EditValue);
			}

			if (db_EditValue >= 8000.0)
			{
				db_EditValue = 8000.0;
				db_InfuseRate = db_EditValue;
				strTemp.Format(_T("%.4f"), db_EditValue);
			}

			GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FLOWSET_EDIT)->SetWindowText(strTemp);

			UpdateData(false);

			db_InfuseRate = db_EditValue;
#if _DEBUGE_MODE
			CDeviceHarvardPump* pPumu = CDeviceHarvardPump::Instance();
		
			pPumu->SetInfuseRate((int)db_InfuseRate);
#endif
		}

		return TRUE;
	}


	//if (WM_LBUTTONDBLCLK == pMsg->message)
	//{
	//	return TRUE;
	//}

	//if (WM_LBUTTONDBLCLK == pMsg->message)
	//{
	//	return TRUE;
	//}

	if (pMsg->message == WM_LBUTTONDOWN)
	{
		CWnd* pWnd = WindowFromPoint(pMsg->pt);
		if (pWnd == GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FASTFORWARD_BUTTON))
		{
#if _DEBUGE_MODE
			CDeviceHarvardPump* pPumu = CDeviceHarvardPump::Instance();
			db_InfuseRate = pPumu->GetInfuseRate();
			pPumu->SetMaxInfuseRate();

			pPumu->Run();
			SetFlowComponentState(true, true, false, false, false);
#endif
		}

		if (pWnd == GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FASTBACKWARD_BUTTON))
		{
#if _DEBUGE_MODE
			CDeviceHarvardPump* pPumu = CDeviceHarvardPump::Instance();
			db_InfuseRate = pPumu->GetWithdrawRate();
			pPumu->SetMaxWithdrawRate();

			pPumu->RunBack();
			SetFlowComponentState(true, false, true, false, false);
#endif
		}
	}

	if (pMsg->message == WM_LBUTTONUP)
	{
#if _DEBUGE_MODE
		CWnd* pWnd = WindowFromPoint(pMsg->pt);
		if(pWnd == GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FASTBACKWARD_BUTTON) || \
			pWnd == GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FASTFORWARD_BUTTON)){
			CDeviceHarvardPump* pPumu = CDeviceHarvardPump::Instance();
			pPumu->Stop();

			//再设为最开始的参数
			pPumu->SetInfuseRate(static_cast<int>(db_InfuseRate));
			SetFlowComponentState(true, true, true, true, false);
		}
		
#endif
	}

	if (WM_LBUTTONDOWN == pMsg->message || WM_LBUTTONDBLCLK == pMsg->message)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_DCMODE_RADIO)->GetSafeHwnd())
		{
			CRect winRect;
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_DCMODE_RADIO)->GetWindowRect(&winRect);
			HDC hdc = GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_DCMODE_RADIO)->GetDC()->GetSafeHdc();
			TEXTMETRIC tm;
			GetTextMetrics(hdc, &tm);
			int iOffSetX = tm.tmHeight * 2 / 3;
			int iOffSetY = (winRect.Height() - (tm.tmHeight - tm.tmInternalLeading)) / 2;

			CPoint pt = pMsg->pt;
			if (pt.x < winRect.left || pt.x > winRect.left + iOffSetX ||
				pt.y < winRect.top + iOffSetY || pt.y > winRect.bottom - iOffSetY)
			{
				return TRUE;
			}
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_ACMODE_RADIO)->GetSafeHwnd())
		{
			CRect winRect;
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_ACMODE_RADIO)->GetWindowRect(&winRect);
			HDC hdc = GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_ACMODE_RADIO)->GetDC()->GetSafeHdc();
			TEXTMETRIC tm;
			GetTextMetrics(hdc, &tm);
			int iOffSetX = tm.tmHeight * 2 / 3;
			int iOffSetY = (winRect.Height() - (tm.tmHeight - tm.tmInternalLeading)) / 2;

			CPoint pt = pMsg->pt;
			if (pt.x < winRect.left || pt.x > winRect.left + iOffSetX ||
				pt.y < winRect.top + iOffSetY || pt.y > winRect.bottom - iOffSetY)
			{
				return TRUE;
			}
		}
	}

	
	return CResizableFormView::PreTranslateMessage(pMsg);
}

bool CPrintParamSetView::InitializeDataGrid()
{
	bool bResult = false;

	//Create GridList
	CRect gridRect;
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_GRID_STATIC)->GetWindowRect(&gridRect);
	//ScreenToClient(&gridRect);

	m_DataGrid.Create(gridRect, this, IDC_JETPRINTDT_PARAMSET_DATA_GRID);
	if (NULL == m_DataGrid.m_hWnd)
	{
		//创建List失败
		bResult = false;
		return bResult;
	}

	//初始化列表的各项
	int nFixedColum = 1;
	int nFixedRow = 1;
	int nColumCount = 8;
	int nRowCount = 50;

	m_DataGrid.SetAutoSizeStyle();
	m_DataGrid.SetEditable(true);
	m_DataGrid.SetFixedColumnSelection(false);
	m_DataGrid.ExpandColumnsToFit(true);
	try
	{
		m_DataGrid.SetColumnCount(nColumCount);
		m_DataGrid.SetRowCount(nRowCount);
		m_DataGrid.SetFixedColumnCount(nFixedColum);
		m_DataGrid.SetFixedRowCount(nFixedRow);

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
	item.crBkClr = RGB(128, 128, 128);
	item.mask = GVIF_TEXT;
	item.row = 0;
	item.col = col;

	//Date Colum
	strList.Format(_T("%s"), _T("日期"));
	item.strText = strList;
	m_DataGrid.SetItem(&item);

	//Material Colum
	++col;
	strList.Format(_T("%s"), _T("材料名称"));
	item.col = col;
	item.strText = strList;
	m_DataGrid.SetItem(&item);

	//Flow Colum
	++col;
	strList.Format(_T("%s"), _T("流量"));
	item.col = col;
	item.strText = strList;
	m_DataGrid.SetItem(&item);

	//AC/DC Mode
	++col;
	strList.Format(_T("%s"), _T("DC_偏置"));
	item.col = col;
	item.strText = strList;
	m_DataGrid.SetItem(&item);

	//Amplitude
	++col;
	strList.Format(_T("%s"), _T("AC_峰值"));
	item.col = col;
	item.strText = strList;
	m_DataGrid.SetItem(&item);

	//Freq
	++col;
	strList.Format(_T("%s"), _T("AC_频率"));
	item.col = col;
	item.strText = strList;
	m_DataGrid.SetItem(&item);

	//Offset
	++col;
	strList.Format(_T("%s"), _T("AC_偏置"));
	item.col = col;
	item.strText = strList;
	m_DataGrid.SetItem(&item);

	//Ratio
	++col;
	strList.Format(_T("%s"), _T("AC_占空比"));
	item.col = col;
	item.strText = strList;
	m_DataGrid.SetItem(&item);

	CRect listRect;
	m_DataGrid.GetClientRect(&listRect);
	int nListWidth = listRect.Width();
	m_DataGrid.SetColumnWidth(0, (nListWidth - 8) / 8 + 10);
	m_DataGrid.SetColumnWidth(1, (nListWidth - 8) / 8 + 4);

	for (int i = 2; i < 8; i++)
	{
		m_DataGrid.SetColumnWidth(i, (nListWidth - 8) / 8 + 2);
	}

	UpdateData(FALSE);

	bResult = true;
	return bResult;
}

void CPrintParamSetView::SetDataGrid(){
	CParamTable* pTable = CParamTable::Instance();

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
			//m_DataGrid.InsertRow(strList,row);
			colorR = (65 + 5 * row) < 255 ? ((65 + 5 * row)) : 255;
			colorG = (105 + 5 * row) < 255 ? (105 + 5 * row) : 255;
			colorB = (225 + 5 * row) < 255 ? (225 + 5 * row) : 255;

			item.row = row;
			item.strText = pTable->m_CurrentDate;
			item.col = 0;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;

			m_DataGrid.SetItem(&item);

			item.row = row;
			item.col = 1;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText = pTable->m_Material;
			m_DataGrid.SetItem(&item);

			item.row = row;
			item.col = 2;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_FlowRate);
			m_DataGrid.SetItem(&item);

			item.row = row;
			item.col = 3;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_DCOffSet);
			m_DataGrid.SetItem(&item);

			item.row = row;
			item.col = 4;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_ACPeak);
			m_DataGrid.SetItem(&item);

			item.row = row;
			item.col = 5;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_ACFreq);
			m_DataGrid.SetItem(&item);

			item.row = row;
			item.col = 6;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_ACOffset);
			m_DataGrid.SetItem(&item);

			item.row = row;
			item.col = 7;
			item.crBkClr = RGB(colorR, colorG, colorB);
			item.mask |= GVIF_BKCLR;
			item.strText.Format(_T("%.2f"), pTable->m_ACRatio);
			m_DataGrid.SetItem(&item);

			++row;
			pTable->MoveNext();
		}
	}

	//关闭数据库
	pTable->Close();

}

void CPrintParamSetView::OnBnClickedJetprintdtParamsetDataApplyButton()
{
	// TODO:  在此添加控件通知处理程序代码
	int nCol = 0;
	nCol = m_DataGrid.GetFocusCell().col;
	int nRow = 0;
	nRow = m_DataGrid.GetFocusCell().row;

	CString itemText = _T("");
	itemText = m_DataGrid.GetItemText(nRow, nCol);

	switch (nCol)
	{
		//流量
	case 2:
	{
		int nFlowRate = _wtoi(itemText);
		db_InfuseRate = nFlowRate*1.0;
		GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FLOWSET_EDIT)->SetWindowText(itemText);
#if _DEBUGE_MODE
		CDeviceHarvardPump* pPump = CDeviceHarvardPump::Instance();
		pPump->SetInfuseRate(nFlowRate);
#endif
		break;
	}

	//直流偏置
	case 3:
	{
		double dbOffset = _wtof(itemText);
		db_DC_Offset = dbOffset;

		if (!b_AcMode)
		{
#if _DEBUGE_VISA
			CDeviceVisa *pVisa = CDeviceVisa::Instance();
			pVisa->SetOffset((long)db_DC_Offset);
#endif
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->SetWindowText(itemText);

			DrawLine();
		}
		break;
	}

	//AC峰值
	case 4:
	{
		double dbAmplitude = _wtof(itemText);
		db_AC_Amplitude = dbAmplitude;

		if (b_AcMode)
		{
#if _DEBUGE_VISA
			CDeviceVisa* pVisa = CDeviceVisa::Instance();
			pVisa->SetAmplitude(db_AC_Amplitude);
#endif
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->SetWindowText(itemText);

			DrawLine();
		}
		break;
	}

	//AC 频率
	case 5:
	{
		double dbFreq = _wtof(itemText);
		db_AC_Freq = dbFreq;

		if (b_AcMode)
		{
#if _DEBUGE_VISA
			CDeviceVisa *pVisa = CDeviceVisa::Instance();
			pVisa->SetFrequency(db_AC_Freq);
#endif
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->SetWindowText(itemText);

			DrawLine();
		}
		break;
	}

	//AC Offset
	case 6:
	{
		double dbOffset = _wtof(itemText);
		db_AC_Offset = dbOffset;

		if (b_AcMode)
		{
#if _DEBUGE_VISA
			CDeviceVisa* pVisa = CDeviceVisa::Instance();
			pVisa->SetOffset((long)db_AC_Offset);
#endif
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->SetWindowText(itemText);

			DrawLine();
		}
		break;
	}

	//AC占空比
	case 7:
	{
		double dbRatio = _wtof(itemText);
		db_AC_Ratio = dbRatio;

		if (b_AcMode)
		{
#if _DEBUGE_VISA
			CDeviceVisa* pVisa = CDeviceVisa::Instance();
			pVisa->SetDCycle(db_AC_Ratio);
#endif
			GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->SetWindowText(itemText);

			DrawLine();
		}
		break;
	}
	}
}


void CPrintParamSetView::OnBnClickedJetprintdtParamsetDataModifyButton()
{
	// TODO:  在此添加控件通知处理程序代码
	int nCol = m_DataGrid.GetFocusCell().col;
	int nRow = m_DataGrid.GetFocusCell().row;

	CString itemText = _T("");
	itemText = m_DataGrid.GetItemText(nRow, nCol);

	CParamTable* pTable = CParamTable::Instance();
	if (pTable->open())
	{
		pTable->Move(nRow);
		pTable->Edit();

		switch (nCol)
		{
		case 1:
			pTable->m_Material = (LPSTR)(LPCTSTR)itemText;
			break;

		case 2:
			pTable->m_FlowRate = (float)(_wtof(itemText));
			break;

		case 3:
			pTable->m_DCOffSet = float(_wtof(itemText));
			break;

		case 4:
			pTable->m_ACPeak = float(_wtof(itemText));
			break;

		case 5:
			pTable->m_ACFreq = float(_wtof(itemText));
			break;

		case 6:
			pTable->m_ACOffset = float(_wtof(itemText));
			break;

		case 7:
			pTable->m_ACRatio = float(_wtof(itemText));
			break;
		}

		//数据库更新数据
		pTable->Update();

		//关闭数据库
		pTable->Close();

		
		SetDataGrid();
		m_DataGrid.UpdateData();
		m_DataGrid.Refresh();
	}
}


void CPrintParamSetView::OnBnClickedJetprintdtParamsetDataAppendButton()
{
	// TODO:  在此添加控件通知处理程序代码
	char szText[128] = { 0 };
	CParamTable* pTable = CParamTable::Instance();
	if (pTable->open())
	{
		pTable->AddNew();

		//首先获取当前的时间
		CTime m_Time;
		m_Time = CTime::GetCurrentTime();
		CString strTime = _T("");
		strTime = m_Time.Format(_T("%Y-%m-%d"));

		strcpy_s(szText, CW2A(strTime));
		pTable->m_CurrentDate = szText;
		memset(szText, 0, 128);
		strcpy_s(szText, CW2A(m_Material));
		pTable->m_Material = szText;
		pTable->m_FlowRate = float(db_InfuseRate);
		pTable->m_ACPeak = float(db_AC_Amplitude);
		pTable->m_ACFreq = float(db_AC_Freq);
		pTable->m_ACOffset = float(db_AC_Offset);
		pTable->m_ACRatio = float(db_AC_Ratio);
		pTable->m_DCOffSet = float(db_DC_Offset);

		pTable->Update();
	}

	pTable->Close();

	SetDataGrid();
	m_DataGrid.UpdateData();
	m_DataGrid.Refresh();
}


void CPrintParamSetView::OnBnClickedJetprintdtParamsetDataDeleteButton()
{
	// TODO:  在此添加控件通知处理程序代码
	int nRow = m_nSelectRow;
	int nIndex = 0;

	CParamTable *pTable = CParamTable::Instance();
	if (pTable->open())
	{
		pTable->Move(nRow);
		pTable->Delete();
		pTable->Query();

		pTable->Update();
	}

	pTable->Close();

	
	m_DataGrid.DeleteRow(nRow);

	SetDataGrid();
	m_DataGrid.UpdateData();
	m_DataGrid.Refresh();
}


void CPrintParamSetView::OnDeltaposJetprintdtParamsetVoltageOffsetSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	CString editText = _T("");
	double db_EditValue = 0.0;
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->GetWindowText(editText);

	CString leftData = _T("");
	CString rightData = _T("");
	int iPos = editText.Find('.');

	leftData = editText.Left(iPos);
	rightData = editText.Right(editText.GetLength()-(iPos));

	int iLeftData = _wtoi(leftData);
	double fRightData = _wtof(rightData);
	int iRightDataLenth = editText.GetLength() - (iPos+1);
	double dbStep = 0.0;
	if (fRightData > 0.0){
		for (int i = 1; i <= iRightDataLenth; i++){
			if (fRightData*pow(10, i)>=1.0f){
				dbStep = 1.0 / (pow(10, i));
				break;
			}
		}
	}
	else{
		int iTemp = iLeftData;
		for (int i = 1; i <= iPos; i++){
			if (iTemp % ((int)pow(10, i)) > 0){
				dbStep = pow(10, i-1);

				break;
			}
		}
	}

	if (pNMUpDown->iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往上的箭头
	{

		db_EditValue = _wtof(editText);
		if (db_EditValue <= 0.01)
			return;

		db_EditValue += dbStep;

		editText.Format(_T("%.2f"), db_EditValue);
	}

	else if (pNMUpDown->iDelta == 1) // 如果此值为1, 说明点击了Spin的往下的箭头
	{
		db_EditValue = _wtof(editText);

		if (db_EditValue >= 8000.00)
			return;

		db_EditValue -= dbStep;

		editText.Format(_T("%.2f"), db_EditValue);
	}

	if (b_AcMode)
	{
		db_AC_Offset = db_EditValue;
#if _DEBUGE_MODE
		CDeviceVisa* pVisa = CDeviceVisa::Instance();
		pVisa->SetOffset((long)db_AC_Offset);
#endif
	}
	else
	{
		db_DC_Offset = db_EditValue;
#if _DEBUGE_MODE
		CDeviceVisa* pVisa = CDeviceVisa::Instance();
		pVisa->SetOffset((long)db_DC_Offset);
#endif

	}
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->SetWindowText(editText);

	DrawLine();

	*pResult = 0;
}


void CPrintParamSetView::OnNMReleasedcaptureJetprintdtParamsetVoltageOffsetSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}


void CPrintParamSetView::OnDeltaposJetprintdtParamsetVoltageAmplitudeSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	CString editText = _T("");
	double db_EditValue = 0.0;
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->GetWindowText(editText);

	CString leftData = _T("");
	CString rightData = _T("");
	int iPos = editText.Find('.');

	leftData = editText.Left(iPos);
	rightData = editText.Right(editText.GetLength() - (iPos));

	int iLeftData = _wtoi(leftData);
	double fRightData = _wtof(rightData);
	int iRightDataLenth = editText.GetLength() - (iPos + 1);
	double dbStep = 0.0;
	if (fRightData > 0.0){
		for (int i = 1; i <= iRightDataLenth; i++){
			if (fRightData*pow(10, i) >= 1.0f){
				dbStep = 1.0 / (pow(10, i));
				break;
			}
		}
	}
	else{
		int iTemp = iLeftData;
		for (int i = 1; i <= iPos; i++){
			if (iTemp % ((int)pow(10, i)) > 0){
				dbStep = pow(10, i - 1);

				break;
			}
		}
	}

	if (pNMUpDown->iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往上的箭头
	{

		db_EditValue = _wtof(editText);
		if (db_EditValue <= 0.01)
			return;

		db_EditValue += dbStep;

		editText.Format(_T("%.2f"), db_EditValue);
	}

	else if (pNMUpDown->iDelta == 1) // 如果此值为1, 说明点击了Spin的往上的箭头
	{
		db_EditValue = _wtof(editText);

		if (db_EditValue >= 8000.00)
			return;

		db_EditValue -= dbStep;

		editText.Format(_T("%.2f"), db_EditValue);
	}

	if (b_AcMode)
	{
		db_AC_Amplitude = db_EditValue;
#if _DEBUGE_MODE
		CDeviceVisa* pVisa = CDeviceVisa::Instance();
		pVisa->SetAmplitude(db_AC_Amplitude);
#endif
	}
	else
	{
		db_DC_Amplitude = db_EditValue;
#if _DEBUGE_MODE
		//theApp.m_JetAglientFunc.SetAmplitude(db_DC_Amplitude);
#endif

	}
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->SetWindowText(editText);

	DrawLine();

	*pResult = 0;
}


void CPrintParamSetView::OnNMReleasedcaptureJetprintdtParamsetVoltageAmplitudeSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}


void CPrintParamSetView::OnDeltaposJetprintdtParamsetVoltageFreqSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	CString editText = _T("");
	double db_EditValue = 0.0;
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->GetWindowText(editText);

	CString leftData = _T("");
	CString rightData = _T("");
	int iPos = editText.Find('.');

	leftData = editText.Left(iPos);
	rightData = editText.Right(editText.GetLength() - (iPos));

	int iLeftData = _wtoi(leftData);
	double fRightData = _wtof(rightData);
	int iRightDataLenth = editText.GetLength() - (iPos + 1);
	double dbStep = 0.0;
	if (fRightData > 0.0){
		for (int i = 1; i <= iRightDataLenth; i++){
			if (fRightData*pow(10, i) >= 1.0f){
				dbStep = 1.0 / (pow(10, i));
				break;
			}
		}
	}
	else{
		int iTemp = iLeftData;
		for (int i = 1; i <= iPos; i++){
			if (iTemp % ((int)pow(10, i)) > 0){
				dbStep = pow(10, i - 1);

				break;
			}
		}
	}

	if (pNMUpDown->iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往上的箭头
	{

		db_EditValue = _wtof(editText);
		if (db_EditValue <= 0.01)
			return;

		db_EditValue += dbStep;

		editText.Format(_T("%.2f"), db_EditValue);
	}

	else if (pNMUpDown->iDelta == 1) // 如果此值为1, 说明点击了Spin的往下的箭头
	{
		db_EditValue = _wtof(editText);

		if (db_EditValue >= 10000.00)
			return;

		db_EditValue -= dbStep;

		editText.Format(_T("%.2f"), db_EditValue);
	}

	if (b_AcMode)
	{
		db_AC_Freq = db_EditValue;
#if _DEBUGE_MODE
		CDeviceVisa* pVisa = CDeviceVisa::Instance();
		pVisa->SetFrequency(db_AC_Freq);
#endif
	}
	else
	{
		db_DC_Freq = db_EditValue;
#if _DEBUGE_MODE
		//theApp.m_JetAglientFunc.SetAmplitude(db_DC_Amplitude);
#endif

	}
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->SetWindowText(editText);

	DrawLine();
	*pResult = 0;
}


void CPrintParamSetView::OnNMReleasedcaptureJetprintdtParamsetVoltageFreqSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}


void CPrintParamSetView::OnDeltaposJetprintdtParamsetVoltageRatioSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	CString editText = _T("");
	double db_EditValue = 0.0;
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT)->GetWindowText(editText);

	CString leftData = _T("");
	CString rightData = _T("");
	int iPos = editText.Find('.');

	leftData = editText.Left(iPos);
	rightData = editText.Right(editText.GetLength() - (iPos));

	int iLeftData = _wtoi(leftData);
	double fRightData = _wtof(rightData);
	int iRightDataLenth = editText.GetLength() - (iPos + 1);
	double dbStep = 0.0;
	if (fRightData > 0.0){
		for (int i = 1; i <= iRightDataLenth; i++){
			if (fRightData*pow(10, i) >= 1.0f){
				dbStep = 1.0 / (pow(10, i));
				break;
			}
		}
	}
	else{
		int iTemp = iLeftData;
		for (int i = 1; i <= iPos; i++){
			if (iTemp % ((int)pow(10, i)) > 0){
				dbStep = pow(10, i - 1);

				break;
			}
		}
	}

	if (pNMUpDown->iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往上的箭头
	{

		db_EditValue = _wtof(editText);
		if (db_EditValue <= 0.01)
			return;

		db_EditValue += dbStep;

		editText.Format(_T("%.2f"), db_EditValue);
	}

	else if (pNMUpDown->iDelta == 1) // 如果此值为1, 说明点击了Spin的往下的箭头
	{
		db_EditValue = _wtof(editText);

		if (db_EditValue >= 100.00)
			return;

		db_EditValue -= dbStep;

		editText.Format(_T("%.2f"), db_EditValue);
	}

	if (b_AcMode)
	{
		db_AC_Ratio = db_EditValue;
#if _DEBUGE_MODE
		CDeviceVisa *pVisa = CDeviceVisa::Instance();
		pVisa->SetDCycle(db_AC_Ratio);
		
#endif
	}
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT)->SetWindowText(editText);

	DrawLine();
	*pResult = 0;
}


void CPrintParamSetView::OnNMReleasedcaptureJetprintdtParamsetVoltageRatioSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}

void CPrintParamSetView::OnGridClick(NMHDR *pNotifyStruct, LRESULT *)
{
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW*)pNotifyStruct;
	int nSelectColum = pItem->iColumn;
	int nSelectRow = pItem->iRow;

	m_nSelectRow = nSelectRow;

	int nColumCount = m_DataGrid.GetColumnCount();
	int nRowCount = m_DataGrid.GetRowCount();

	if (nSelectColum >= 0 && nSelectColum < nColumCount)
	{
		if (nSelectRow >= 1 && nSelectRow < nRowCount)
		{
			//如果选择的是第一列的单元
			if (nSelectColum == 0)
			{
				GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_APPLY_BUTTON)->EnableWindow(FALSE);
				GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_APPEND_BUTTON)->EnableWindow(TRUE);
				GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_DELETE_BUTTON)->EnableWindow(TRUE);
				GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_MODIFY_BUTTON)->EnableWindow(FALSE);
			}
			else
			{
				GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_APPLY_BUTTON)->EnableWindow(TRUE);
				GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_APPEND_BUTTON)->EnableWindow(TRUE);
				GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_DELETE_BUTTON)->EnableWindow(FALSE);
				GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_MODIFY_BUTTON)->EnableWindow(FALSE);
			}
		}
	}
}

void CPrintParamSetView::OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_APPLY_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_APPEND_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_DELETE_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_MODIFY_BUTTON)->EnableWindow(TRUE);

	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_MODIFY_BUTTON)->SetFocus();
}

void CPrintParamSetView::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW*)pNotifyStruct;
	//m_DataGrid.SetFocusCell(pItem->iColumn, pItem->iRow);

	//ModifyData
	//OnBnClickedJetprintdtParamsetDataModifyButton();

	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_APPLY_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_APPEND_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_DELETE_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_JETPRINTDT_PARAMSET_DATA_MODIFY_BUTTON)->EnableWindow(FALSE);
}

bool CPrintParamSetView::IsInACMode() const
{
	return b_AcMode;
}

inline double CPrintParamSetView::GetNozzleDiameter() const
{
	return m_dbNozzleDiameter;
}

inline double CPrintParamSetView::GetNozzleVolume() const
{
	return m_dbNozzleVolume;
}

double CPrintParamSetView::GetInfuseRate() const
{
	return db_InfuseRate;
}

 void CPrintParamSetView::SetNozzleDiameter(double dbDiameter)
{
	m_dbNozzleDiameter = dbDiameter;

	CDeviceHarvardPump *pPump = CDeviceHarvardPump::Instance();
	if (pPump){
		pPump->SetSyringeDiameter(m_dbNozzleDiameter);
		pPump->SetInfuseRate(static_cast<int>(db_InfuseRate));
	}
	
}

 void CPrintParamSetView::UpdateFlowControlState(bool bEnabled)
 {
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FLOWSET_EDIT)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FASTFORWARD_BUTTON)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_FASTBACKWARD_BUTTON)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_FLOW_STARTEJECT_BUTTON)->EnableWindow(bEnabled);
	 //GetDlgItem(IDC_JET_FLOW_STOP_INJECT_BTN)->EnableWindow(bEnabled);
 }

 void CPrintParamSetView::UpdateAgilentControlState(bool bEnabled)
 {
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_DCMODE_RADIO)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_ACMODE_RADIO)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_START_BUTTON)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_STOP_BUTTON)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_EDIT)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_OFFSET_SPIN)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_EDIT)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_AMPLITUDE_SPIN)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_EDIT)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_FREQ_SPIN)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_EDIT)->EnableWindow(bEnabled);
	 GetDlgItem(IDC_JETPRINTDT_PARAMSET_VOLTAGE_RATIO_SPIN)->EnableWindow(bEnabled);
 }

void CPrintParamSetView::SetNozzleVolume(double dbVolume)
{
	m_dbNozzleVolume = dbVolume;
	CDeviceHarvardPump *pPump = CDeviceHarvardPump::Instance();
	if (pPump){
		pPump->SetSyringeVolume(dbVolume);
		pPump->SetInfuseRate(static_cast<int>(db_InfuseRate));
	}
	
}
