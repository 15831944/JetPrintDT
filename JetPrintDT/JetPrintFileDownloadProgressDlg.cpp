// JetPrintFileDownloadProgressDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "JetPrintDT.h"
#include "JetPrintFileDownloadProgressDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"


// CJetPrintFileDownloadProgressDlg �Ի���

IMPLEMENT_DYNAMIC(CJetPrintFileDownloadProgressDlg, CDialog)

CJetPrintFileDownloadProgressDlg::CJetPrintFileDownloadProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJetPrintFileDownloadProgressDlg::IDD, pParent)
{
	m_bCancelDownload = false;
}

CJetPrintFileDownloadProgressDlg::~CJetPrintFileDownloadProgressDlg()
{
}

void CJetPrintFileDownloadProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDD_JETPRINTDT_PRINT_FILEDOWNLOAD_PROGRESS_TOTAL_SIZE_EDIT, m_TotalSizeEdit);
	DDX_Control(pDX, IDD_JETPRINTDT_PRINT_FILEDOWNLOAD_PROGRESS_DOWNLOADED_SIZE_EDIT, m_DownloadedSizeEdit);
	DDX_Control(pDX, IDD_JETPRINTDT_PRINT_FILEDOWNLOAD_PROGRESS, m_DownloadProgress);
}


BEGIN_MESSAGE_MAP(CJetPrintFileDownloadProgressDlg, CDialog)
	ON_BN_CLICKED(IDD_JETPRINTDT_PRINT_FILEDOWNLOAD_PROGRESS_CANCEL_BUTTON, &CJetPrintFileDownloadProgressDlg::OnBnClickedJetprintdtPrintFiledownloadProgressCancelButton)
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CJetPrintFileDownloadProgressDlg ��Ϣ�������


void CJetPrintFileDownloadProgressDlg::OnBnClickedJetprintdtPrintFiledownloadProgressCancelButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_bCancelDownload = true;

	CMainFrame *pMainFram = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainFram)
		pMainFram->StartCheckTemp();

}


BOOL CJetPrintFileDownloadProgressDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (pMsg->hwnd == GetDlgItem(IDD_JETPRINTDT_PRINT_FILEDOWNLOAD_PROGRESS_TOTAL_SIZE_EDIT)->GetSafeHwnd())
		{
			return TRUE;
		}

		if (pMsg->hwnd == GetDlgItem(IDD_JETPRINTDT_PRINT_FILEDOWNLOAD_PROGRESS_DOWNLOADED_SIZE_EDIT)->GetSafeHwnd())
		{
			return TRUE;
		}
	}


	return CDialog::PreTranslateMessage(pMsg);
}


void CJetPrintFileDownloadProgressDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO:  �ڴ˴������Ϣ����������
}


void CJetPrintFileDownloadProgressDlg::OnNcDestroy()
{
	CDialog::OnNcDestroy();

	// TODO:  �ڴ˴������Ϣ����������
}


void CJetPrintFileDownloadProgressDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnClose();
}


BOOL CJetPrintFileDownloadProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitializeEdit();
	InitializeProgress();
	m_DownloadProgress.SetPos(0);
	this->CenterWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CJetPrintFileDownloadProgressDlg::InitializeEdit()
{
	COLORREF textColor = RGB(0, 0, 0);
	COLORREF backColor = RGB(148, 255, 255);
	COLORREF backColorFocus = RGB(205, 0, 0);
	m_DownloadedSizeEdit.SetBkClr(backColor);
	m_DownloadedSizeEdit.SetBkClrFocus(backColorFocus);
	m_DownloadedSizeEdit.SetTextFont(10, true, false, _T("Arial"));
	m_DownloadedSizeEdit.SetTextClr(textColor);
	m_DownloadedSizeEdit.EnableEditing(false);

	m_TotalSizeEdit.SetBkClr(backColor);
	m_TotalSizeEdit.SetBkClrFocus(backColorFocus);
	m_TotalSizeEdit.SetTextFont(10, true, false, _T("Arial"));
	m_TotalSizeEdit.SetTextClr(textColor);
	m_TotalSizeEdit.EnableEditing(false);
}

void CJetPrintFileDownloadProgressDlg::InitializeProgress()
{
	COLORREF startColor = COLORREF(RGB(255, 0, 0));
	COLORREF endColor = COLORREF(RGB(0, 0, 255));
	COLORREF backColor = COLORREF(RGB(0, 191, 255));
	COLORREF textColor = COLORREF(RGB(255, 255, 255));

	m_DownloadProgress.SetRange(0, 100);
	m_DownloadProgress.SetPos(1);
	m_DownloadProgress.ShowPercent(true);
	//m_DownloadProgress.SetBkColor(backColor);
	//m_DownloadProgress.SetStartColor(startColor);
	//m_DownloadProgress.SetEndColor(endColor);
	//m_DownloadProgress.SetTextColor(textColor);
}