#pragma once

#include "resource.h"
// CJetPrintPlacodeModifyDlg �Ի���

class CJetPrintPlacodeModifyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CJetPrintPlacodeModifyDlg)

public:
	CJetPrintPlacodeModifyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CJetPrintPlacodeModifyDlg();

// �Ի�������
	enum { IDD = IDD_JETPRINTDT_PRINT_PLACODEMODIFY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_PlacodeLenth;
	double m_PlacodeWidth;
};
