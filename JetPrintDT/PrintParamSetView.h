#pragma once

#include "ResizableFormView.h"
#include "JetPrintGridCtrl/GridCtrl.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartCtrl.h"
#include "ChartCtrl/ChartAxis.h"
// CPrintParamSetView 窗体视图

class CPrintParamSetView : public CResizableFormView
{
	DECLARE_DYNCREATE(CPrintParamSetView)

protected:
	CPrintParamSetView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPrintParamSetView();

public:
	enum { IDD = IDD_JETPRINTDT_PRINT_PARAMSET_DIALOG };
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
	CRect m_RectOrign;

	//Flow Param
	bool m_bPumpOpen;     //流量泵是否开启
	double db_InfuseRate; //注射流量
	double m_dbNozzleDiameter;
	double m_dbNozzleVolume;

	//Voltage 
	int m_VoltageModeRadio;
	CChartCtrl m_ChartCtrl;
	CChartLineSerie *m_pLineSerial;
	bool b_AcMode;					//处于交流模式


	//data grid
	CGridCtrl   m_DataGrid;
	int         m_nSelectRow;
	CString		m_Material;

	//public members
public:
	//直流的四个参数
	double db_DC_Freq;
	double db_DC_Amplitude;
	double db_DC_Ratio;
	double db_DC_Offset;

	//交流的四个参数
	double db_AC_Offset;
	double db_AC_Freq;
	double db_AC_Amplitude;
	double db_AC_Ratio;

	bool m_bFlowVoltageOpenFlag; //电压调节打开标志

	//private member funcs
private:
	void AdjustView();
	bool InitializeCharAxis();
	bool InitializeChart();
	bool InitializeDataGrid();
	void SetDataGrid();
	void DrawLine();


	inline double round(const double& db, const size_t &count){
		return (long long)((long double)db*pow(10, count)) / pow(10, count);
	}
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg void OnClickedJetprintdtParamsetVoltageDcmodeRadio();
	afx_msg void OnBnClickedJetprintdtParamsetFlowStartejectButton();
	afx_msg void OnBnClickedJetprintdtParamsetFlowStopejectButton();
	afx_msg void OnBnClickedJetprintdtParamsetVoltageStartButton();
	afx_msg void OnBnClickedJetprintdtParamsetVoltageStopButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedJetprintdtParamsetFlowFastforwardButton();
	afx_msg void OnBnClickedJetprintdtParamsetDataApplyButton();
	afx_msg void OnBnClickedJetprintdtParamsetDataModifyButton();
	afx_msg void OnBnClickedJetprintdtParamsetDataAppendButton();
	afx_msg void OnBnClickedJetprintdtParamsetDataDeleteButton();
	afx_msg void OnDeltaposJetprintdtParamsetVoltageOffsetSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureJetprintdtParamsetVoltageOffsetSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposJetprintdtParamsetVoltageAmplitudeSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureJetprintdtParamsetVoltageAmplitudeSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposJetprintdtParamsetVoltageFreqSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureJetprintdtParamsetVoltageFreqSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposJetprintdtParamsetVoltageRatioSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureJetprintdtParamsetVoltageRatioSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT * pResult);
	afx_msg void OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT *pResult);

	bool IsInACMode() const;
	double GetNozzleDiameter() const;
	double GetNozzleVolume() const;
	double GetInfuseRate() const;
	void SetNozzleDiameter(double dbDiameter);
	void SetNozzleVolume(double dbVolume);
	void UpdateFlowControlState(bool bEnabled);
	void UpdateAgilentControlState(bool bEnabled);
	void SetFlowComponentState(bool bEditEnabled, bool bForwardEnabled, bool bBackwardEnabled, bool bRunEnabled, bool bStopEnabled);
};


