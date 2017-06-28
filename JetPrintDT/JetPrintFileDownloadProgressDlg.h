#pragma once

#include "CustomeControl\JetCustomeEdit.h"
#include "CustomeControl\JetInitProgressCtrl.h"
#include "CustomeControl\JetCustomeEdit.h"
#include "CustomeControl\JetInitProgressCtrl.h"
// CJetPrintFileDownloadProgressDlg �Ի���
#include "Resource.h"

class CJetPrintFileDownloadProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CJetPrintFileDownloadProgressDlg)

public:
	CJetPrintFileDownloadProgressDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CJetPrintFileDownloadProgressDlg();

// �Ի�������
	enum { IDD = IDD_JETPRINTDT_PRINT_FILEDOWNLOAD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CJetCustomeEdit m_TotalSizeEdit;
	CJetCustomeEdit m_DownloadedSizeEdit;
	CJetInitProgressCtrl m_DownloadProgress;
	bool m_bCancelDownload;

	enum{ TIME_DOWNLOAD = 0 };

private:

	void InitializeEdit();
	void InitializeProgress();

public:
	afx_msg void OnBnClickedJetprintdtPrintFiledownloadProgressCancelButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();

};
