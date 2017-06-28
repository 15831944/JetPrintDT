// JetPrintPlacodeModifyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "JetPrintDT.h"
#include "JetPrintPlacodeModifyDlg.h"
#include "afxdialogex.h"


// CJetPrintPlacodeModifyDlg 对话框

IMPLEMENT_DYNAMIC(CJetPrintPlacodeModifyDlg, CDialogEx)

CJetPrintPlacodeModifyDlg::CJetPrintPlacodeModifyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CJetPrintPlacodeModifyDlg::IDD, pParent)
	, m_PlacodeLenth(0)
	, m_PlacodeWidth(0)
{

}

CJetPrintPlacodeModifyDlg::~CJetPrintPlacodeModifyDlg()
{
}

void CJetPrintPlacodeModifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDD_JETPRINTDT_PRINT_PLACODEMODIFY_PLACODELENTH_EDIT, m_PlacodeLenth);
	DDV_MinMaxDouble(pDX, m_PlacodeLenth, 0.0, 100.0);
	DDX_Text(pDX, IDD_JETPRINTDT_PRINT_PLACODEMODIFY_PLACODEWIDTH_EDIT, m_PlacodeWidth);
	DDV_MinMaxDouble(pDX, m_PlacodeWidth, 0.0, 100.0);
}


BEGIN_MESSAGE_MAP(CJetPrintPlacodeModifyDlg, CDialogEx)
END_MESSAGE_MAP()


// CJetPrintPlacodeModifyDlg 消息处理程序
