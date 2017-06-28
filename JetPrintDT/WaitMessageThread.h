#pragma once
#include "JetPrintWaittingDlg.h"


// CWaitMessageThread

class CWaitMessageThread : public CWinThread
{
	DECLARE_DYNCREATE(CWaitMessageThread)

protected:
	CWaitMessageThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CWaitMessageThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	CJetPrintWaittingDlg * m_pWaitMessageDlg;

private:
	afx_msg void OnInitThread(WPARAM wParam, LPARAM lParam);

protected:
	DECLARE_MESSAGE_MAP()
};


