// WaitMessageThread.cpp : 实现文件
//

#include "stdafx.h"
#include "WaitMessageThread.h"
#include "PrintPrintControlView.h"
#include "MainFrm.h"
#include "PrintFileOperateView.h"

// CWaitMessageThread

IMPLEMENT_DYNCREATE(CWaitMessageThread, CWinThread)

CWaitMessageThread::CWaitMessageThread()
{

	m_pWaitMessageDlg = NULL;
}

CWaitMessageThread::~CWaitMessageThread()
{

}

BOOL CWaitMessageThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	return TRUE;
}

int CWaitMessageThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	if(NULL != m_pWaitMessageDlg)
		m_pWaitMessageDlg->DestroyWindow();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CWaitMessageThread, CWinThread)
	ON_THREAD_MESSAGE(UM_WAITMESSAGEDLG, &CWaitMessageThread::OnInitThread)
END_MESSAGE_MAP()


// CWaitMessageThread 消息处理程序
void CWaitMessageThread::OnInitThread(WPARAM wParam, LPARAM lParam)
{
	UINT u_Num = wParam;
	switch(u_Num)
	{
	case 0:
		if(NULL == m_pWaitMessageDlg)
		{
			this->m_pWaitMessageDlg = new CJetPrintWaittingDlg;
			if (NULL != m_pWaitMessageDlg)
			{
				this->m_pWaitMessageDlg->Create(IDD_JETPRINTDT_PRINT_WAITTING_DIALOG);
			}	
		}
		if(NULL != m_pWaitMessageDlg)
		{
			if(m_pWaitMessageDlg->m_hWnd)
			{
				this->m_pWaitMessageDlg->ShowWindow(SW_SHOW);
				this->m_pWaitMessageDlg->CenterWindow();
			}	
		}	
		break;
	case 1:
		this->m_pWaitMessageDlg->ShowWindow(SW_HIDE);
		//更新按钮的状态
		CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
		CPrintPrintControlView *pPrintView = nullptr;
		CPrintFileOperateView *pFileView = nullptr;
		if (pMainFrame){
			pPrintView = (CPrintPrintControlView *)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][PRINTCONTROL_VIEW]);
			pFileView = (CPrintFileOperateView  *)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[LEFT_SIDE][FILEOPERATE_VIEW]);
		}
		if (pPrintView){
			pPrintView->GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_SETSPEED_BUTTON)->EnableWindow(TRUE);
			pPrintView->GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_HOME_BUTTON)->EnableWindow(TRUE);
			pPrintView->GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STARTPRINT_BUTTON)->EnableWindow(TRUE);
			pPrintView->GetDlgItem(IDC_JETPRINTDT_PRINT_PRINTCONTROL_STOPPRINT_BUTTON)->EnableWindow(TRUE);
		}
		
		if (pFileView){
			pFileView->GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEOPEN_BUTTON)->EnableWindow(TRUE);
			pFileView->GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEDOWNLOAD_BUTTON)->EnableWindow(TRUE);
		}
		
		break;
	}
}