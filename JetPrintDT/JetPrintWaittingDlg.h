#pragma once

#include "resource.h"

// CJetPrintWaittingDlg 对话框

class CJetPrintWaittingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CJetPrintWaittingDlg)

public:
	CJetPrintWaittingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJetPrintWaittingDlg();

// 对话框数据
	enum { IDD = IDD_JETPRINTDT_PRINT_WAITTING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
