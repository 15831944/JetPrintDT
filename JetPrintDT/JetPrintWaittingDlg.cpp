// JetPrintWaittingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "JetPrintDT.h"
#include "JetPrintWaittingDlg.h"
#include "afxdialogex.h"


// CJetPrintWaittingDlg 对话框

IMPLEMENT_DYNAMIC(CJetPrintWaittingDlg, CDialogEx)

CJetPrintWaittingDlg::CJetPrintWaittingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CJetPrintWaittingDlg::IDD, pParent)
{

}

CJetPrintWaittingDlg::~CJetPrintWaittingDlg()
{
}

void CJetPrintWaittingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CJetPrintWaittingDlg, CDialogEx)
END_MESSAGE_MAP()


// CJetPrintWaittingDlg 消息处理程序
