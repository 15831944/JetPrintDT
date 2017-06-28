#pragma once

#include "resource.h"
// CJetPrintPlacodeModifyDlg 对话框

class CJetPrintPlacodeModifyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CJetPrintPlacodeModifyDlg)

public:
	CJetPrintPlacodeModifyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJetPrintPlacodeModifyDlg();

// 对话框数据
	enum { IDD = IDD_JETPRINTDT_PRINT_PLACODEMODIFY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_PlacodeLenth;
	double m_PlacodeWidth;
};
