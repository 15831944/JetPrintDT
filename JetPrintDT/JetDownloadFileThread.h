#pragma once

#ifndef __JET_DOWNLOAD_FILE_THREAD
#define __JET_DOWNLOAD_FILE_THREAD

#include "JetPrintFileDownloadProgressDlg.h"

// CJetDownloadFileThread

class CJetDownloadFileThread : public CWinThread
{
	DECLARE_DYNCREATE(CJetDownloadFileThread)

protected:
	CJetDownloadFileThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CJetDownloadFileThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	//public members
public:
	CJetPrintFileDownloadProgressDlg m_JetFileDownloadProgressDlg;

private:
	long m_lTotalBytes;
	long m_lDownloadedBytes;
	long m_lBeginTime;
	long m_lEndTime;
	float m_fExpendUnitTime;    //加载文件所消耗的单位时间 s/kb

	//private functions
private:
	bool CreateFileDownloadDlg();
	afx_msg void OnInitThread(WPARAM wParam, LPARAM lParam);
	CString GetModuleDir();
	BOOL ReadConfig();
	BOOL WriteConfig();

protected:
	DECLARE_MESSAGE_MAP()
};


#endif  //__JET_DOWNLOAD_FILE_THREAD



