// JetDownloadFileThread.cpp : 实现文件
//

#include "stdafx.h"
#include "JetDownloadFileThread.h"
#include "MainFrm.h"
#include "PrintFileOperateView.h"
#include "SingleTon/ParamInitSingleton.h"
// CJetDownloadFileThread

IMPLEMENT_DYNCREATE(CJetDownloadFileThread, CWinThread)

CJetDownloadFileThread::CJetDownloadFileThread()
{
	m_lTotalBytes = 0;
	m_lDownloadedBytes = 0;
	m_lBeginTime = 0;
	m_lEndTime = 0;
	m_fExpendUnitTime = 0.0;
	//读配置文件
	CParamInitSingleton *pParam = CParamInitSingleton::Instance();
	if (pParam){
		m_fExpendUnitTime = pParam->GetDownloadSpeed();
	}
}

CJetDownloadFileThread::~CJetDownloadFileThread()
{
}

BOOL CJetDownloadFileThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	return TRUE;
}

int CJetDownloadFileThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	m_JetFileDownloadProgressDlg.DestroyWindow();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CJetDownloadFileThread, CWinThread)
	ON_THREAD_MESSAGE(UM_FILEDOWNLOAD,&CJetDownloadFileThread::OnInitThread)
END_MESSAGE_MAP()


// CJetDownloadFileThread 消息处理程序

bool CJetDownloadFileThread::CreateFileDownloadDlg()
{
	bool bResult = false;
	m_JetFileDownloadProgressDlg.Create(IDD_JETPRINTDT_PRINT_FILEDOWNLOAD_DIALOG/*,((CJetMainDlg*)(AfxGetMainWnd()))->m_pJetPrintDlg*/);
	
	if(NULL == m_JetFileDownloadProgressDlg.m_hWnd)
	{
		bResult = false;
		return bResult;
	}

	m_JetFileDownloadProgressDlg.ShowWindow(SW_HIDE);
	bResult = true;
	return bResult;
}

void CJetDownloadFileThread::OnInitThread(WPARAM wParam, LPARAM lParam)
{
	UINT nChioce = wParam;
	int nPos = 0;
	CString strText = _T("");
	int iRemainExpendTime = 0;

	CMainFrame* pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintCameraView *pCameraView = (CPrintCameraView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[RIGHT_SIDE][0]);
	CPrintFileOperateView *pFileView = nullptr;
	if (pMainFrame){
		pFileView = (CPrintFileOperateView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[0][FILEOPERATE_VIEW]);
	}
	switch(nChioce)
	{
	case 0:
		if(!CreateFileDownloadDlg())
		{
			AfxMessageBox(_T("创建文件加载窗口失败！"));
		}
		break;

	case 1:
		nPos = lParam;
		m_JetFileDownloadProgressDlg.m_DownloadProgress.SetPos(nPos);
		//估计剩余时间
		iRemainExpendTime = (int)((float)(m_lTotalBytes - m_lDownloadedBytes) / 1024.0 * m_fExpendUnitTime);
		if (m_lTotalBytes/1024 > 100)
		{
			strText.Format(_T("文件较大请耐心等待...估计剩余时间：%ds"), iRemainExpendTime);	
		}
		else
		{
			strText.Format(_T("文件加载中...估计剩余时间：%ds"), iRemainExpendTime);		
		}
		m_JetFileDownloadProgressDlg.GetDlgItem(IDD_JETPRINTDT_PRINT_FILEDOWNLOAD_PROGRESS_INDICATE_STATIC)->SetWindowText(strText);
		break;

	case 2:
		//文件加载结束		
		m_lEndTime = GetTickCount();		
		if (m_lTotalBytes > 10)
		{
			m_fExpendUnitTime = (float)((m_lEndTime - m_lBeginTime) * 1.024 / m_lTotalBytes);  
			CParamInitSingleton *pParam = CParamInitSingleton::Instance();
			if (pParam){
				pParam->SetDownloadSpeed(m_fExpendUnitTime);
			}
		}

		if (NULL != pFileView)
			pFileView->m_bIsFileDownloading = false;

		m_JetFileDownloadProgressDlg.ShowWindow(SW_HIDE);
		
		if (pMainFrame)
			pMainFrame->StartCheckTemp();

		
		if (pCameraView){
			pCameraView->StartGrabImage();
		}
		break;

	case 3:
		m_lDownloadedBytes = lParam;
		strText.Format(_T("%4d"), m_lDownloadedBytes);
		m_JetFileDownloadProgressDlg.m_DownloadedSizeEdit.SetWindowText(strText);
		break;

	case 4:
		m_lTotalBytes = lParam;
		strText.Format(_T("%4d"), m_lTotalBytes);
		m_JetFileDownloadProgressDlg.m_TotalSizeEdit.SetWindowText(strText);
		break;

	case 5:
		//文件超出内存范围，已停止加载
		if (m_JetFileDownloadProgressDlg.m_bCancelDownload)
		{
			CString text;
			text.Format(_T("文件取消加载中..."));
			m_JetFileDownloadProgressDlg.GetDlgItem(IDD_JETPRINTDT_PRINT_FILEDOWNLOAD_PROGRESS_INDICATE_STATIC)->SetWindowText(text);
			Sleep(2000);
		}
		else
		{
			CString text;
			text.Format(_T("文件超出内存范围,停止加载中...建议文件不超过100KB！"));
			m_JetFileDownloadProgressDlg.GetDlgItem(IDD_JETPRINTDT_PRINT_FILEDOWNLOAD_PROGRESS_INDICATE_STATIC)->SetWindowText(text);
			Sleep(2000);
		}
		m_JetFileDownloadProgressDlg.ShowWindow(SW_HIDE);
		if (NULL != pFileView)
			pFileView->m_bIsFileDownloading = false;

		if (pMainFrame)
			pMainFrame->StartCheckTemp();
		break;

	case 6:
		m_JetFileDownloadProgressDlg.ShowWindow(SW_SHOW);
		m_JetFileDownloadProgressDlg.CenterWindow();
		m_lBeginTime = GetTickCount();
		break;
	}
}


CString CJetDownloadFileThread::GetModuleDir()
{
	//获取Debug或Release所在路径
	HMODULE module = GetModuleHandle(0);
	char chFileName[MAX_PATH];
	LPTSTR lpstrFileName = (LPTSTR)chFileName;
	GetModuleFileName(module, lpstrFileName, MAX_PATH);

	CString strExePath(lpstrFileName);
	int nPos = strExePath.ReverseFind(_T('\\'));
	if (nPos < 0)
	{
		return CString("");
	}
	else
	{
		return strExePath.Left(nPos);
	}
}

BOOL CJetDownloadFileThread::ReadConfig()
{
	//读配置文件
	CString strConfigFilePath = GetModuleDir();
	if ("" == strConfigFilePath)
	{
		return FALSE;
	}

	strConfigFilePath += _T("\\Config");
	CFileFind filefind;
	if (!filefind.FindFile(strConfigFilePath))
	{
		return FALSE;
	}

	LPWSTR lpwstrValue = NULL;
	lpwstrValue = new WCHAR[MAX_PATH];
	memset(lpwstrValue, 0, sizeof(WCHAR)*MAX_PATH);
	GetPrivateProfileString(_T("DownLoadFile"), _T("ExpendUnitTime"), _T("READ_FAIL"), lpwstrValue, MAX_PATH, strConfigFilePath);
	if (StrCmpW(lpwstrValue, _T("READ_FAIL")) == 0)	return FALSE;
	m_fExpendUnitTime =(float) _wtof(lpwstrValue);

	delete [] lpwstrValue;
	lpwstrValue = NULL;

	return TRUE;
}

BOOL CJetDownloadFileThread::WriteConfig()
{
	//写配置文件
	CString strConfigFilePath = GetModuleDir();
	if ("" == strConfigFilePath)
	{
		return FALSE;
	}

	strConfigFilePath += _T("\\Config");
	CFileFind filefind;
	if (!filefind.FindFile(strConfigFilePath))
	{
		return FALSE;
	}

	CString strValue = _T("");
	strValue.Format(L"%f", m_fExpendUnitTime);
	WritePrivateProfileString(_T("DownLoadFile"), _T("ExpendUnitTime"), strValue, strConfigFilePath);

	return TRUE;
}