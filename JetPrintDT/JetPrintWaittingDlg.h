#pragma once

#include "resource.h"

// CJetPrintWaittingDlg �Ի���

class CJetPrintWaittingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CJetPrintWaittingDlg)

public:
	CJetPrintWaittingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CJetPrintWaittingDlg();

// �Ի�������
	enum { IDD = IDD_JETPRINTDT_PRINT_WAITTING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
