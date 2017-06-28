#pragma once

#ifndef __JET_DOWNLOAD_FILE_THREAD
#define __JET_DOWNLOAD_FILE_THREAD

#include "JetPrintFileDownloadProgressDlg.h"

// CJetDownloadFileThread

class CJetDownloadFileThread : public CWinThread
{
	DECLARE_DYNCREATE(CJetDownloadFileThread)

protected:
	CJetDownloadFileThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
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
	float m_fExpendUnitTime;    //�����ļ������ĵĵ�λʱ�� s/kb

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



