#pragma once

#include "ResizableFormView.h"
#include "JetPreviewWnd.h"
// CPrintMatchView ������ͼ

class CPrintMatchView : public CDialog
{
	DECLARE_DYNCREATE(CPrintMatchView)

public:
	CPrintMatchView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CPrintMatchView();

public:
	enum { IDD = IDD_JETPRINTDT_PRINT_MATCH_DIALOG };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	double m_dbXPrintOrignPos;
	double m_dbYPrintOrignPos;
	double m_dbXJetPos;
	double m_dbYJetPos;
	double m_dbXLocatePos;
	double m_dbYLocatePos;
	double m_dbStepLenth;

	CJetPreviewWnd m_PreviewWnd;
public:
	virtual BOOL OnInitDialog();


public:
	void UpdatePosition(double dbXPos,double dbYPos);
	bool InitSimulatePanel();
	afx_msg void OnBnClickedJetprintdtPrintMatchLocatecameraTocenterSetdistButton();
	afx_msg void OnBnClickedJetprintdtPrintMatchLocatecameraTocenterMovetoButton();
	afx_msg void OnBnClickedJetprintdtPrintMatchJetTocenterSetdistButton();
	afx_msg void OnBnClickedJetprintdtPrintMatchPrintorignYsteppositiveButton();
	afx_msg void OnBnClickedJetprintdtPrintMatchPrintorignXstepnegetiveButton();
	afx_msg void OnBnClickedJetprintdtPrintMatchPrintorignYstepnegetiveButton();
	afx_msg void OnBnClickedJetprintdtPrintMatchPrintorignXsteppositiveButton();
	afx_msg void OnBnClickedJetprintdtPrintMatchPrintorignSteplenthsetButton();
	afx_msg void OnBnClickedJetprintdtPrintMatchPrintorignCaculatePrintorignButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedJetprintdtPrintMatchPrintorignSetDecodefileButton();
};


