// PrintFileOperateView.cpp : 实现文件
//

#include "stdafx.h"
#include "JetPrintDT.h"
#include "PrintFileOperateView.h"
#include "SingleTon/ParamInitSingleton.h"
#include "Device/DeviceParker.h"
#include "Device/DeviceCamera.h"
#include "PrintCameraView.h"
#include "MainFrm.h"
#include "PrintMotionControlView.h"
#include "PrintPrintControlView.h"
// CPrintFileOperateView

IMPLEMENT_DYNCREATE(CPrintFileOperateView, CResizableFormView)

CPrintFileOperateView::CPrintFileOperateView()
: CResizableFormView(CPrintFileOperateView::IDD)
{
	m_dbImageHeight = 10.0;
	m_dbImageWidth = 10.0;
	m_dbSpace = 0.5;
	m_Threshold = 128;

	m_dbDXFImageOrignWidth = 0.0;
	m_dbDXFImageOrignHeight = 0.0;
	m_dbDXFImageDestWidth = 0.0;
	m_dbDXFImageDestHeight = 0.0;

	m_bFileOpenedOK = false;
	m_bIsFileDownloading = false;
	m_CurrentFileName = _T("");
	m_CurrentFileExt = _T("");
	m_strFilePath = _T("");
	m_strFileName = _T("");

	m_bIsFileDownloadSucceed = false;
	m_pDownLoadFileThread = NULL;
	
	db_XOrignPos = 0.0;
	db_YOrignPos = 0.0;

	m_pMatchView = nullptr;
}

CPrintFileOperateView::~CPrintFileOperateView()
{
}

void CPrintFileOperateView::DoDataExchange(CDataExchange* pDX)
{
	CResizableFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_JETPRINTDT_PRINT_FILEOPERATE_DESTIMAGE_SHOW_PICTURE, m_JetDrawView);
}

BEGIN_MESSAGE_MAP(CPrintFileOperateView, CResizableFormView)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DECODE_BUTTON, &CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperateDxfDecodeButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_PARAMSET_BUTTON, &CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperateBmpParamsetButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEOPEN_BUTTON, &CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperateFileFileopenButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEDOWNLOAD_BUTTON, &CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperateFileFiledownloadButton)
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_FILEOPERATE_PRINTORIGN_BUTTON, &CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperatePrintorignButton)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_BN_CLICKED(IDC_JETPRINTDT_PRINT_FILEOPERATE_MATCHPRINT_BUTTON, &CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperateMatchprintButton)
END_MESSAGE_MAP()


// CPrintFileOperateView 诊断

#ifdef _DEBUG
void CPrintFileOperateView::AssertValid() const
{
	CResizableFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPrintFileOperateView::Dump(CDumpContext& dc) const
{
	CResizableFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPrintFileOperateView 消息处理程序


BOOL CPrintFileOperateView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CResizableFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CPrintFileOperateView::OnInitialUpdate()
{
	//DXF Group
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_GROUP, CSize(0, 0), CSize(50, 50));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNHEIGHT_STATIC, CSize(25, 5),CSize(25,5));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNHEIGHT_EDIT, CSize(25, 5),CSize(30,5));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNHEIGHT_UNIT_STATIC, CSize(30, 5),CSize(30,10));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNWIDTH_STATIC, CSize(25, 10),CSize(25,10));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNWIDTH_EDIT, CSize(25, 10),CSize(30,10));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNWIDTH_UNIT_STATIC, CSize(30,10),CSize(30,10));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTHEIGHT_STATIC, CSize(25, 15),CSize(25,15));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTHEIGHT_EDIT, CSize(25, 15),CSize(30,15));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTHEIGHT_UNIT_STATIC, CSize(30, 15),CSize(30,15));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTWIDTH_STATIC, CSize(25, 20),CSize(25,20));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTWIDTH_EDIT, CSize(25, 20),CSize(30,20));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTWIDTH_UNIT_STATIC, CSize(30, 20),CSize(30,20));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DECODE_BUTTON, CSize(30, 25),CSize(30,30));

	//BMP Group
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_GROUP, CSize(0, 50), CSize(50, 80));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEWIDTH_STATIC, CSize(25, 50),CSize(25,50));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEWIDTH_EDIT, CSize(25, 50),CSize(30,50));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEWIDTH_UNIT_STATIC, CSize(30,50),CSize(30,50));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEHEIGHT_STATIC, CSize(25, 55), CSize(25, 55));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEHEIGHT_EDIT, CSize(25, 55),CSize(30,55));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEHEIGHT_UNIT_STATIC, CSize(30, 55),CSize(30,55));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_DROPDIST_STATIC, CSize(25, 60),CSize(25,60));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_DROPDIST_EDIT, CSize(25, 60), CSize(30, 60));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_DROPDIST_UNIT_STATIC, CSize(30, 60),CSize(30,60));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_THRESHOLD_STATIC, CSize(25, 65),CSize(25,65));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_THRESHOLD_EDIT, CSize(25, 65),CSize(30,65));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_THRESHOLD_RANGE_STATIC, CSize(30, 65), CSize(30, 65));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_PARAMSET_BUTTON, CSize(30, 70),CSize(30,75));

	//PRINT ORING GROUP
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_PRINTORIGN_GROUP, CSize(0, 80), CSize(50, 100));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_XPRINTORIGN_STATIC, CSize(25, 80), CSize(25, 80));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_XPRINTORIGN_EDIT, CSize(25, 80), CSize(30, 80));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_XPRINTORIGN_UNIT_STATIC, CSize(30, 80), CSize(30, 80));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_YPRINTORIGN_STATIC, CSize(25, 85), CSize(25, 85));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_YPRINTORIGN_EDIT, CSize(25, 85), CSize(30, 85));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_YPRINTORIGN_UNIT_STATIC, CSize(30, 85), CSize(30, 85));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_PRINTORIGN_BUTTON, CSize(10, 90), CSize(30, 95));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_MATCHPRINT_BUTTON, CSize(40, 90), CSize(60, 95));

	//Dest Image
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DESTIMAGE_GROUP, CSize(50, 0), CSize(100, 55));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_DESTIMAGE_SHOW_PICTURE, CSize(50, 5), CSize(100, 55));

	//File Group
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_GROUP, CSize(50, 60), CSize(100, 100));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEPATH_STATIC, CSize(65, 72),CSize(65,74));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEPATH_EDIT, CSize(65, 72),CSize(65,74));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEOPEN_BUTTON, CSize(65, 72),CSize(65,74));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILENAME_STATIC, CSize(65, 86),CSize(65,88));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILENAME_EDIT, CSize(65, 86),CSize(65,88));
	AddAnchor(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEDOWNLOAD_BUTTON, CSize(65, 86),CSize(65,88));


	CResizableFormView::OnInitialUpdate();

	// TODO:  在此添加专用代码和/或调用基类
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	//initialize param
	CParamInitSingleton *pParamInstance = CParamInitSingleton::Instance();
	m_dbDXFImageDestHeight = pParamInstance->GetDXFDestHeight();
	m_dbDXFImageDestWidth = pParamInstance->GetDXFDestWidth();
	m_dbDXFImageOrignHeight = pParamInstance->GetDXFOrignHeight();
	m_dbDXFImageOrignWidth = pParamInstance->GetDXFOrignWidth();

	m_dbImageHeight = pParamInstance->GetBMPHeight();
	m_dbImageWidth = pParamInstance->GetBMPWidth();
	m_dbSpace = pParamInstance->GetBMPDropDist();
	m_Threshold = (int)pParamInstance->GetBMPThreshold();


	CString editText = _T("");
	editText.Format(_T("%.4f"), m_dbDXFImageDestHeight);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTHEIGHT_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), m_dbDXFImageDestWidth);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTWIDTH_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), m_dbDXFImageOrignHeight);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNHEIGHT_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), m_dbDXFImageOrignWidth);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNWIDTH_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), m_dbImageHeight);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEHEIGHT_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), m_dbImageWidth);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEWIDTH_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), m_dbSpace);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_DROPDIST_EDIT)->SetWindowText(editText);

	editText.Format(_T("%4d"), m_Threshold);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_THRESHOLD_EDIT)->SetWindowText(editText);


	if (NULL == m_pDownLoadFileThread)
	{
		m_pDownLoadFileThread = (CJetDownloadFileThread *)AfxBeginThread(RUNTIME_CLASS(CJetDownloadFileThread));
		if (NULL == m_pDownLoadFileThread)
		{
			DWORD dwErrorCode = GetLastError();
			AfxMessageBox(_T("文件加载界面进程创建失败!"));
		}
	}

	if (NULL != m_pDownLoadFileThread)
		m_pDownLoadFileThread->PostThreadMessage(UM_FILEDOWNLOAD, 0, 0);
}


void CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperateDxfDecodeButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_CurrentFileExt.MakeUpper() == _T("DXF"))
	{
		CString strTempFile = _T("");
		CString strFileFolder = _T("");
		CString FileName = m_CurrentFileName;
		CString strTemp = _T("");
		strFileFolder = this->SetProgPath();
		int nPos = FileName.ReverseFind('\\');
		strTempFile = FileName.Right(FileName.GetLength() - 1 - nPos);
		nPos = strTempFile.Find('.');
		FileName = strTempFile.Left(nPos);
		strTemp = _T("\\") + FileName;
		strFileFolder += strTemp;
		FileName = strFileFolder;
		FileName += _T(".txt");
		double left = 0.0;
		double right = 0.0;
		double top = 0.0;
		double bottom = 0.0;

		left = m_JetDrawView.m_Drawing.borderRect.left;
		right = m_JetDrawView.m_Drawing.borderRect.right;
		top = m_JetDrawView.m_Drawing.borderRect.top;
		bottom = m_JetDrawView.m_Drawing.borderRect.bottom;

		double dbDestHeight = 0.0;
		double dbDestWidth = 0.0;
		CString editText = _T("");
		GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTHEIGHT_EDIT)->GetWindowText(editText);
		dbDestHeight = _wtof(editText);
		if (dbDestHeight > 60.0 || dbDestHeight < 0.0){
			dbDestHeight = m_dbDXFImageOrignHeight;
		}
		editText.Format(_T("%.4f"), dbDestHeight);
		GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTHEIGHT_EDIT)->SetWindowText(editText);

		GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTWIDTH_EDIT)->GetWindowText(editText);
		dbDestWidth = _wtof(editText);
		if (dbDestWidth > 60.0 || dbDestWidth < 0.0){
			dbDestWidth = m_dbDXFImageOrignWidth;
		}
		editText.Format(_T("%.4f"), dbDestWidth);
		GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTWIDTH_EDIT)->SetWindowText(editText);

		double xOffset = (double)(right + left) / 2.0;
		double yOffset = (double)(top + bottom) / 2.0;

		xOffset = xOffset - db_XOrignPos;
		yOffset = yOffset - db_YOrignPos;

		double xScale = 0.0;
		if (IsEqual(0.0 ,m_dbDXFImageOrignWidth))
			xScale = 0.0;
		else
			xScale = dbDestWidth / m_dbDXFImageOrignWidth;

		double yScale = 0.0;
		if (IsEqual(0.0,m_dbDXFImageOrignHeight))
			yScale = 0.0;
		else
			yScale = dbDestHeight / m_dbDXFImageOrignHeight;

		m_JetDxfWriteProg.InitJetWriteOffset(xOffset, yOffset);
		m_JetDxfWriteProg.InitJetWriteZoomFactor(xScale, yScale);
		m_JetDxfWriteProg.InitJetWriteView(m_JetDrawView.m_Drawing.m_pJetDxfDimView);
		m_JetDxfWriteProg.dxfCreateFile(FileName);

		m_JetDxfWriteProg.WriteFile();
		m_JetDxfWriteProg.dxfCloseFile();
	}
}


void CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperateBmpParamsetButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strEdit = _T("");
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEHEIGHT_EDIT)->GetWindowText(strEdit);
	m_dbImageHeight = _wtof((strEdit));

	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEWIDTH_EDIT)->GetWindowText(strEdit);
	m_dbImageWidth = _wtof(strEdit);

	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_THRESHOLD_EDIT)->GetWindowText(strEdit);
	m_Threshold = _wtoi(strEdit);

	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_DROPDIST_EDIT)->GetWindowText(strEdit);
	m_dbSpace = _wtof(strEdit);

	if (m_dbSpace < 0.00 || m_dbImageHeight / m_dbSpace < 5.0 || m_dbImageWidth / m_dbSpace < 5.0)
	{
		AfxMessageBox(_T("参数设置不合理，请重新设置！"));
	}

	//重新解析
	if (m_CurrentFileExt.MakeUpper() == _T("BMP")){

		m_JetDrawView.m_pJetBmpAnalysis = &m_JetBmpAnalysis;
		m_JetDrawView.graphType = BMP_TYPE;

		m_JetBmpAnalysis.m_dbImageHeight = m_dbImageHeight;
		m_JetBmpAnalysis.m_dbImageWidth = m_dbImageWidth;
		m_JetBmpAnalysis.m_dbSpace = m_dbSpace;
		m_JetBmpAnalysis.m_uThreashold = m_Threshold;
		m_JetBmpAnalysis.SetImageRangeEmpty();

		m_JetBmpAnalysis.SetXOffSet(db_XOrignPos);
		m_JetBmpAnalysis.SetYOffSet(db_YOrignPos);
		if (m_JetBmpAnalysis.BmpAnalysis(m_CurrentFileName, m_dbImageWidth, m_dbImageHeight, m_Threshold, m_dbSpace))
		{
			m_bFileOpenedOK = true;
			CString strTempFile = _T("");
			CString strFileFolder = _T("");
			strFileFolder = this->SetProgPath();
			int nPos = m_strFileName.Find('.');
			strTempFile = m_strFileName.Left(nPos);
			strTempFile = strFileFolder + _T("\\") + strTempFile;
			strTempFile += _T(".txt");

			m_JetBmpAnalysis.WriteProg(strTempFile);

		}
		else
		{
			m_bFileOpenedOK = false;
			AfxMessageBox(_T("BMP文件解析失败!"));
		}

		CMainFrame* pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
		CPrintPrintControlView *pPrintView = nullptr;
		if (pMainFrame){
			pPrintView = (CPrintPrintControlView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[0][PRINTCONTROL_VIEW]);
		}

		if (NULL != pPrintView)
		{
			if (NULL != pPrintView->m_hWnd)
			{
				::PostMessageA(pPrintView->m_hWnd, WM_FILE_LOADED, WPARAM(m_JetDrawView.m_pJetBmpAnalysis), LPARAM(1));
			}
		}
	}
}


void CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperateFileFileopenButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_bIsFileDownloading)
	{
		return;
	}

	// TODO: 在此添加控件通知处理程序代码
	CFileDialog OpenDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_EXPLORER, _T("DXF file format(*.dxf) |*.dxf|BMP file format(*.bmp)|*.bmp|CDW file format(*.cdw)|*.cdw|All Files (*.*)|*.*||"), NULL);
	OpenDlg.m_ofn.lpstrTitle = _T("Open DXF、BMP And CDW File");
	//OpenDlg.HideControl(IDOK); 

	if (OpenDlg.DoModal() == IDOK)
	{
		m_strFileName = OpenDlg.GetFileName();
		m_strFilePath = OpenDlg.GetFolderPath();
		m_CurrentFileExt = OpenDlg.GetFileExt();
		m_CurrentFileName = m_strFilePath + _T("\\") + m_strFileName;

		GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEPATH_EDIT)->SetWindowText(m_strFilePath);
		GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILENAME_EDIT)->SetWindowText(m_strFileName);

		AnalysisAndDraw();
	}
}


void CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperateFileFiledownloadButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_bFileOpenedOK)
	{
		AfxMessageBox(_T("请先打开文件!"));
		return;
	}

	if (m_bIsFileDownloading)
	{
		return;
	}

	if (m_strFilePath.IsEmpty() || m_strFileName.IsEmpty())
	{
		AfxMessageBox(_T("文件路径为空,请先打开文件!"));
		return;
	}

	m_bIsFileDownloading = true;
	//取得预打印文件的路径
	CString strTempFile = _T("");
	CString strFileFolder = _T("");
	strFileFolder = this->SetProgPath();
	int nPos = m_strFileName.Find('.');
	strTempFile = m_strFileName.Left(nPos);
	strTempFile = strFileFolder + _T("\\") + strTempFile;
	strTempFile += _T(".txt");

	m_CurrentDownloadFile = strTempFile;

	CMainFrame *pMainFram = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainFram)
		pMainFram->StopCheckTemp();

	CPrintCameraView *pCameraView = (CPrintCameraView*)(pMainFram->m_SplitInfo[0].m_wndSplitter.m_views[RIGHT_SIDE][0]);
	if (pCameraView){
		pCameraView->StopGrabImage();
	}
	
	Sleep(200);
	//创建文件加载界面
	if (NULL != m_pDownLoadFileThread)
	{
		m_pDownLoadFileThread->PostThreadMessage(UM_FILEDOWNLOAD, 6, 0);
		DownloadFile(); //开始文件下载
	}
}

void CPrintFileOperateView::MatchPrintDecodeFile(double dbXOrignPos, double dbYOrignPos,double dbXJet,double dbYJet){
	CMainFrame* pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);;
	CPrintMotionControlView *pView = nullptr;

	if (m_CurrentFileExt.MakeUpper() == _T("DXF"))
	{
		m_JetDrawView.graphType = DXF_TYPE;
		CRect viewRect;
		m_JetDrawView.GetClientRect(&viewRect);
		m_JetDrawView.m_Drawing.drwInitView(0, 0, viewRect.Width(), viewRect.Height());
		m_JetDrawView.m_Drawing.drwZoomExtents();
		m_JetDrawView.RedrawWindow();

		CString strTempFile = _T("");
		CString strFileFolder = _T("");
		CString FileName = m_CurrentFileName;
		CString strTemp = _T("");
		strFileFolder = this->SetProgPath();
		int nPos = FileName.ReverseFind('\\');
		strTempFile = FileName.Right(FileName.GetLength() - 1 - nPos);
		nPos = strTempFile.Find('.');
		FileName = strTempFile.Left(nPos);
		strTemp = _T("\\") + FileName;
		strFileFolder += strTemp;
		FileName = strFileFolder;
		FileName += _T(".txt");
		double left = 0;
		double right = 0;
		double top = 0;
		double bottom = 0;

		left = m_JetDrawView.m_Drawing.borderRect.left;
		right = m_JetDrawView.m_Drawing.borderRect.right;
		top = m_JetDrawView.m_Drawing.borderRect.top;
		bottom = m_JetDrawView.m_Drawing.borderRect.bottom;

		double xWidth = abs((double)(right - left) / 2.0);
		double yHeight = -abs((double)(top - bottom) / 2.0);
		double xOffset = (double)(right + left) / 2.0;
		double yOffset = (double)(top + bottom) / 2.0;

		xOffset = xOffset+xWidth - dbXJet;
		yOffset = yOffset+yHeight - dbYJet;

		m_JetDxfWriteProg.InitJetWriteOffset(xOffset, yOffset);
		m_JetDxfWriteProg.InitJetWriteView(m_JetDrawView.m_Drawing.m_pJetDxfDimView);
		m_JetDxfWriteProg.dxfCreateFile(FileName);

		m_JetDxfWriteProg.WriteFile();
		m_JetDxfWriteProg.dxfCloseFile();

		CPrintPrintControlView *pPrintView = nullptr;
		if (pMainFrame){
			pPrintView = (CPrintPrintControlView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[0][PRINTCONTROL_VIEW]);
		}
		if (NULL != pPrintView)
		{
			if (NULL != pPrintView->m_hWnd)
			{
				::PostMessageA(pPrintView->m_hWnd, WM_FILE_LOADED, WPARAM(m_JetDrawView.m_Drawing.m_pJetDxfDimView), LPARAM(0));
			}
		}
	}

	if (m_CurrentFileExt.MakeUpper() == _T("BMP"))
	{
		m_JetDrawView.m_pJetBmpAnalysis = &m_JetBmpAnalysis;
		m_JetDrawView.graphType = BMP_TYPE;

		CString strTempFile = _T("");
		CString strFileFolder = _T("");
		strFileFolder = this->SetProgPath();
		int nPos = m_strFileName.Find('.');
		strTempFile = m_strFileName.Left(nPos);
		strTempFile = strFileFolder + _T("\\") + strTempFile;
		strTempFile += _T(".txt");

		double xoffset = db_XOrignPos  - dbXJet;
		double yoffset = db_YOrignPos  - dbYJet;

		m_JetBmpAnalysis.SetXOffSet(xoffset);
		m_JetBmpAnalysis.SetYOffSet(yoffset);
		m_JetBmpAnalysis.WriteProg(strTempFile);

		CPrintPrintControlView *pPrintView = nullptr;
		if (pMainFrame){
			pPrintView = (CPrintPrintControlView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[0][PRINTCONTROL_VIEW]);
		}
		if (NULL != pPrintView)
		{
			if (NULL != pPrintView->m_hWnd)
			{
				::PostMessageA(pPrintView->m_hWnd, WM_FILE_LOADED, WPARAM(m_JetDrawView.m_pJetBmpAnalysis), LPARAM(1));
			}
		}
	}
}


void CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperatePrintorignButton()
{
	// TODO:  在此添加控件通知处理程序代码
	CString editText = _T("");
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_XPRINTORIGN_EDIT)->GetWindowText(editText);
	db_XOrignPos = _wtof(editText);

	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_YPRINTORIGN_EDIT)->GetWindowText(editText);
	db_YOrignPos = _wtof(editText);

	CMainFrame* pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);;
	CPrintMotionControlView *pView = nullptr;
	if (pMainFrame){
		pView = (CPrintMotionControlView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[0][MOTIONCONTROL_VIEW]);
	}
	if (pView){
		pView->db_XPrintOrignPos = db_XOrignPos;
		pView->db_YPrintOrignPos = db_YOrignPos;
	}


	if (m_CurrentFileExt.MakeUpper() == _T("DXF"))
	{
		m_JetDrawView.graphType = DXF_TYPE;
		CRect viewRect;
		m_JetDrawView.GetClientRect(&viewRect);
		m_JetDrawView.m_Drawing.drwInitView(0, 0, viewRect.Width(), viewRect.Height());
		m_JetDrawView.m_Drawing.drwZoomExtents();
		m_JetDrawView.RedrawWindow();

		CString strTempFile = _T("");
		CString strFileFolder = _T("");
		CString FileName = m_CurrentFileName;
		CString strTemp = _T("");
		strFileFolder = this->SetProgPath();
		int nPos = FileName.ReverseFind('\\');
		strTempFile = FileName.Right(FileName.GetLength() - 1 - nPos);
		nPos = strTempFile.Find('.');
		FileName = strTempFile.Left(nPos);
		strTemp = _T("\\") + FileName;
		strFileFolder += strTemp;
		FileName = strFileFolder;
		FileName += _T(".txt");
		double left = 0;
		double right = 0;
		double top = 0;
		double bottom = 0;

		left = m_JetDrawView.m_Drawing.borderRect.left;

		right = m_JetDrawView.m_Drawing.borderRect.right;
		top = m_JetDrawView.m_Drawing.borderRect.top;
		bottom = m_JetDrawView.m_Drawing.borderRect.bottom;

		double xOffset = (double)(right + left) / 2.0;
		double yOffset = (double)(top + bottom) / 2.0;

		xOffset = xOffset - db_XOrignPos;
		yOffset = yOffset - db_YOrignPos;

		m_JetDxfWriteProg.InitJetWriteOffset(xOffset, yOffset);
		m_JetDxfWriteProg.InitJetWriteView(m_JetDrawView.m_Drawing.m_pJetDxfDimView);
		m_JetDxfWriteProg.dxfCreateFile(FileName);

		m_JetDxfWriteProg.WriteFile();
		m_JetDxfWriteProg.dxfCloseFile();

		CPrintPrintControlView *pPrintView = nullptr;
		if (pMainFrame){
			pPrintView = (CPrintPrintControlView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[0][PRINTCONTROL_VIEW]);
		}
		if (NULL != pPrintView)
		{
			if (NULL != pPrintView->m_hWnd)
			{
				::PostMessageA(pPrintView->m_hWnd, WM_FILE_LOADED, WPARAM(m_JetDrawView.m_Drawing.m_pJetDxfDimView), LPARAM(0));
			}
		}
	}

	if (m_CurrentFileExt.MakeUpper() == _T("BMP"))
	{
		m_JetDrawView.m_pJetBmpAnalysis = &m_JetBmpAnalysis;
		m_JetDrawView.graphType = BMP_TYPE;

		CString strTempFile = _T("");
		CString strFileFolder = _T("");
		strFileFolder = this->SetProgPath();
		int nPos = m_strFileName.Find('.');
		strTempFile = m_strFileName.Left(nPos);
		strTempFile = strFileFolder + _T("\\") + strTempFile;
		strTempFile += _T(".txt");

		m_JetBmpAnalysis.SetXOffSet(db_YOrignPos);
		m_JetBmpAnalysis.SetYOffSet(db_YOrignPos);
		m_JetBmpAnalysis.WriteProg(strTempFile);

		CPrintPrintControlView *pPrintView = nullptr;
		if (pMainFrame){
			pPrintView = (CPrintPrintControlView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[0][PRINTCONTROL_VIEW]);
		}
		if (NULL != pPrintView)
		{
			if (NULL != pPrintView->m_hWnd)
			{
				::PostMessageA(pPrintView->m_hWnd, WM_FILE_LOADED, WPARAM(m_JetDrawView.m_pJetBmpAnalysis), LPARAM(1));
			}
		}
	}
}


void CPrintFileOperateView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CResizableFormView::OnPaint()
}


void CPrintFileOperateView::OnNcPaint()
{
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CResizableFormView::OnNcPaint()
}

CString CPrintFileOperateView::SetProgPath()
{
	CString strCurrentFolder = _T("");
	CString strProgFolder = _T("");
	TCHAR progFolder[MAX_PATH];
	memset(progFolder, 0x00, sizeof(progFolder));
	GetModuleFileName(AfxGetInstanceHandle(), progFolder, MAX_PATH);

	strCurrentFolder = progFolder;
	int nPos = strCurrentFolder.ReverseFind('\\');
	strCurrentFolder = strCurrentFolder.Left(nPos);
	strProgFolder = strCurrentFolder + _T("\\ParkerProg");

	if (!PathIsDirectory(strProgFolder))
	{
		CreateDirectory(strProgFolder, 0);
	}

	return strProgFolder;
}

bool CPrintFileOperateView::LoadAndImportFile(CString FileName)
{
	bool bResult = false;
	CString strFileFolder = _T("");
	CString strTemp = _T("");
	m_JetDrawView.m_Drawing.drwInitialize();

	m_JetDxfRead.InitView(&m_JetDrawView.m_Drawing);

	// Load data from dxf file
	if (!FileName.IsEmpty())
	{
		m_JetDxfRead.LoadDataFromFile(NULL, FileName);
	}
	else
	{
		bResult = false;
		return bResult;
	}

	CRect viewRect;
	m_JetDrawView.GetClientRect(&viewRect);
	m_JetDrawView.m_Drawing.drwInitView(0, 0, viewRect.Width(), viewRect.Height());
	m_JetDrawView.m_Drawing.drwZoomExtents();
	m_JetDrawView.RedrawWindow();


	//原图的宽度和高度
	double left = 0.0;
	double right = 0.0;
	double top = 0.0;
	double bottom = 0.0;

	left = m_JetDrawView.m_Drawing.borderRect.left;

	right = m_JetDrawView.m_Drawing.borderRect.right;
	top = m_JetDrawView.m_Drawing.borderRect.top;
	bottom = m_JetDrawView.m_Drawing.borderRect.bottom;
	double iWidth = abs(right - left);
	double iHeight= abs(top - bottom);

	CString textEdit = _T("");
	textEdit.Format(_T("%.4f"), iHeight);
	m_dbDXFImageOrignHeight = iHeight;
	m_dbDXFImageDestHeight = iHeight;
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNHEIGHT_EDIT)->SetWindowTextW(textEdit);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTHEIGHT_EDIT)->SetWindowTextW(textEdit);

	textEdit.Format(_T("%.4f"), iWidth);
	m_dbDXFImageOrignWidth = iWidth;
	m_dbDXFImageDestWidth = iWidth;
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNWIDTH_EDIT)->SetWindowTextW(textEdit);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTWIDTH_EDIT)->SetWindowTextW(textEdit);

	//写入文件
	CString strTempFile = _T("");
	strFileFolder = this->SetProgPath();
	int nPos = FileName.ReverseFind('\\');
	strTempFile = FileName.Right(FileName.GetLength() - 1 - nPos);
	nPos = strTempFile.Find('.');
	FileName = strTempFile.Left(nPos);
	strTemp = _T("\\") + FileName;
	strFileFolder += strTemp;
	FileName = strFileFolder;
	FileName += _T(".txt");
	

	double xOffset = (double)(right + left) / 2.0;
	double yOffset = (double)(top + bottom) / 2.0;

	
	xOffset = xOffset - db_XOrignPos;
	yOffset = yOffset - db_YOrignPos;

	m_JetDxfWriteProg.InitJetWriteOffset(xOffset, yOffset);
	m_JetDxfWriteProg.InitJetWriteView(m_JetDrawView.m_Drawing.m_pJetDxfDimView);
	m_JetDxfWriteProg.dxfCreateFile(FileName);

	if (m_JetDxfWriteProg.WriteFile())
	{
		//AfxMessageBox(_T("文件写入成功"));
	}


	m_JetDxfWriteProg.dxfCloseFile();
	bResult = true;

	return bResult;
}

BOOL CPrintFileOperateView::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNHEIGHT_EDIT)->GetSafeHwnd())
		{

			return TRUE;
		}
		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_ORIGNWIDTH_EDIT)->GetSafeHwnd())
		{

			return TRUE;
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTHEIGHT_EDIT)->GetSafeHwnd())
		{

			CString strEdit = _T("");
			GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTHEIGHT_EDIT)->GetWindowText(strEdit);
			m_dbDXFImageDestHeight = _wtof(strEdit);

			return TRUE;
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTWIDTH_EDIT)->GetSafeHwnd())
		{

			CString strEdit = _T("");
			GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DXF_DESTWIDTH_EDIT)->GetWindowText(strEdit);
			m_dbDXFImageDestWidth = _wtof(strEdit);

			return TRUE;
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEWIDTH_EDIT)->GetSafeHwnd())
		{

			CString strEdit = _T("");
			GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEWIDTH_EDIT)->GetWindowText(strEdit);
			m_dbImageWidth = _wtof(strEdit);

			return TRUE;
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEHEIGHT_EDIT)->GetSafeHwnd())
		{
			CString strEdit = _T("");
			GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_IMAGEHEIGHT_EDIT)->GetWindowText(strEdit);
			m_dbImageHeight = _wtof((strEdit));

			return TRUE;
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_THRESHOLD_EDIT)->GetSafeHwnd())
		{
			CString strEdit = _T("");
			GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_THRESHOLD_EDIT)->GetWindowText(strEdit);
			m_Threshold = _wtoi(strEdit);

			return TRUE;
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_DROPDIST_EDIT)->GetSafeHwnd())
		{
			CString strEdit = _T("");

			GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_BMP_DROPDIST_EDIT)->GetWindowText(strEdit);
			m_dbSpace = _wtof(strEdit);

			return TRUE;
		}
		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_XPRINTORIGN_EDIT)->GetSafeHwnd())
		{
			CString strEdit = _T("");

			GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_XPRINTORIGN_EDIT)->GetWindowText(strEdit);
			db_XOrignPos = _wtof(strEdit);

			return TRUE;
		}

		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_YPRINTORIGN_EDIT)->GetSafeHwnd())
		{
			CString strEdit = _T("");

			GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_YPRINTORIGN_EDIT)->GetWindowText(strEdit);
			db_YOrignPos = _wtof(strEdit);

			return TRUE;
		}


		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEPATH_EDIT)->GetSafeHwnd())
		{
			return TRUE;
		}
		if (pMsg->hwnd == GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_FILE_FILEPATH_EDIT)->GetSafeHwnd())
		{
			return TRUE;
		}

	}
	return CResizableFormView::PreTranslateMessage(pMsg);
}

CString CPrintFileOperateView::GetModuleDir()
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

BOOL CPrintFileOperateView::ReadConfig()
{
	//读配置文件
	CString strConfigFilePath = GetModuleDir();
	if ("" == strConfigFilePath)
	{
		//MessageBox(_T("Release所在路径为空"));
		return FALSE;
	}

	strConfigFilePath += _T("\\Config");
	CFileFind filefind;
	if (!filefind.FindFile(strConfigFilePath))
	{
		//MessageBox(_T("未发现Config文件"));
		return FALSE;
	}

	char chValue[MAX_PATH];
	memset(chValue, 0, MAX_PATH);
	GetPrivateProfileString(_T("PrintFile"), _T("filePath"), _T("READ_FAIL"), (LPTSTR)chValue, MAX_PATH, strConfigFilePath);
	if (strcmp(chValue, "READ_FAIL") == 0)	return FALSE;
	m_strFilePath = (LPTSTR)chValue;

	memset(chValue, 0, MAX_PATH);
	GetPrivateProfileString(_T("PrintFile"), _T("fileName"), _T("READ_FAIL"), (LPTSTR)chValue, MAX_PATH, strConfigFilePath);
	if (strcmp(chValue, "READ_FAIL") == 0)	return FALSE;
	m_strFileName = (LPTSTR)chValue;

	memset(chValue, 0, MAX_PATH);
	GetPrivateProfileString(_T("PrintFile"), _T("fileExtName"), _T("READ_FAIL"), (LPTSTR)chValue, MAX_PATH, strConfigFilePath);
	if (strcmp(chValue, "READ_FAIL") == 0)	return FALSE;
	m_CurrentFileExt = (LPTSTR)chValue;

	return TRUE;
}

BOOL CPrintFileOperateView::WriteConfig()
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

	WritePrivateProfileString(_T("PrintFile"), _T("filePath"), m_strFilePath, strConfigFilePath);
	WritePrivateProfileString(_T("PrintFile"), _T("fileName"), m_strFileName, strConfigFilePath);
	WritePrivateProfileString(_T("PrintFile"), _T("fileExtName"), m_CurrentFileExt, strConfigFilePath);

	return TRUE;
}

void CPrintFileOperateView::AnalysisAndDraw()
{
	CMainFrame* pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CPrintPrintControlView *pPrintView = nullptr;
	if (pMainFrame){
		pPrintView = (CPrintPrintControlView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[0][PRINTCONTROL_VIEW]);
	}
	//pPrintView->SetFileExt(m_CurrentFileExt);

	m_JetDrawView.graphType = NULL_TYPE;
	m_JetDrawView.RedrawWindow();

	//首先清空
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_DESTIMAGE_SHOW_PICTURE)->Invalidate();

	if (m_CurrentFileExt.MakeUpper() == _T("DXF"))
	{
		m_JetDrawView.graphType = DXF_TYPE;
		if (LoadAndImportFile(m_CurrentFileName))
		{
			m_bFileOpenedOK = true;
		}
		else
		{
			m_bFileOpenedOK = false;
			AfxMessageBox(_T("DXF文件解析失败!"));
		}

		if (NULL != pPrintView)
		{
			if (NULL != pPrintView->m_hWnd)
			{
				::PostMessageA(pPrintView->m_hWnd, WM_FILE_LOADED, WPARAM(m_JetDrawView.m_Drawing.m_pJetDxfDimView), LPARAM(0));
			}
		}
	}

	if (m_CurrentFileExt.MakeUpper() == _T("BMP"))
	{
		m_JetDrawView.m_pJetBmpAnalysis = &m_JetBmpAnalysis;
		m_JetDrawView.graphType = BMP_TYPE;

		m_JetBmpAnalysis.m_dbImageHeight = m_dbImageHeight;
		m_JetBmpAnalysis.m_dbImageWidth = m_dbImageWidth;
		m_JetBmpAnalysis.m_dbSpace = m_dbSpace;
		m_JetBmpAnalysis.m_uThreashold = m_Threshold;
		m_JetBmpAnalysis.SetImageRangeEmpty();

		m_JetBmpAnalysis.SetXOffSet(db_XOrignPos);
		m_JetBmpAnalysis.SetYOffSet(db_YOrignPos);
		if (m_JetBmpAnalysis.BmpAnalysis(m_CurrentFileName, m_dbImageWidth, m_dbImageHeight, m_Threshold, m_dbSpace))
		{
			m_bFileOpenedOK = true;
			CString strTempFile = _T("");
			CString strFileFolder = _T("");
			strFileFolder = this->SetProgPath();
			int nPos = m_strFileName.Find('.');
			strTempFile = m_strFileName.Left(nPos);
			strTempFile = strFileFolder + _T("\\") + strTempFile;
			strTempFile += _T(".txt");

			m_JetBmpAnalysis.WriteProg(strTempFile);

		}
		else
		{
			m_bFileOpenedOK = false;
			AfxMessageBox(_T("BMP文件解析失败!"));
		}

		if (NULL != pPrintView)
		{
			if (NULL != pPrintView->m_hWnd)
			{
				::PostMessageA(pPrintView->m_hWnd, WM_FILE_LOADED, WPARAM(m_JetDrawView.m_pJetBmpAnalysis), LPARAM(1));
			}
		}
	}
}


void CPrintFileOperateView::DownloadFile()
{
	CDeviceParker *pParker = nullptr;

#if _DEBUGE_PARKER
	pParker = CDeviceParker::Instance();
#endif

	if (!m_CurrentDownloadFile.IsEmpty())
	{
		if (m_pDownLoadFileThread->m_JetFileDownloadProgressDlg.m_hWnd != NULL)
		{
			m_pDownLoadFileThread->m_JetFileDownloadProgressDlg.m_bCancelDownload = false;
		}

		m_bIsFileDownloadSucceed = false;

		int nPos = 0;
		long nSize = 0;
		CString progDest = _T("");

		progDest.Format(_T("PROG0"));
		BSTR bstrProgDest = ::SysAllocString(progDest);
		BSTR bstrFileName = ::SysAllocString(m_CurrentDownloadFile);
		pParker->m_controlChannel->DownloadFile(bstrProgDest, bstrFileName);
		::SysFreeString(bstrProgDest);
		::SysFreeString(bstrFileName);

		//需要检测文件加载到控制器的结果
		long m_total = 0;
		long m_byte = 0;
		long m_currentByte = 0;
		bool bDownLoadStatus = true;
		CString m_Status = _T("");
		while (bDownLoadStatus)
		{
			if (m_pDownLoadFileThread->m_JetFileDownloadProgressDlg.m_hWnd != NULL &&
				m_pDownLoadFileThread->m_JetFileDownloadProgressDlg.m_bCancelDownload == true)
			{
				//若取消加载
				pParker->m_controlChannel->StopDownload();

				Sleep(1000);
				CMainFrame* pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
				CPrintCameraView *pCameraView = (CPrintCameraView*)(pMainFrame->m_SplitInfo[0].m_wndSplitter.m_views[RIGHT_SIDE][0]);
				if (pCameraView){
					pCameraView->StartGrabImage();
				}
			}

			long status = pParker->m_controlChannel->GetStatusDL(&m_total, &m_byte);

			m_pDownLoadFileThread->PostThreadMessage(UM_FILEDOWNLOAD, 3, m_byte);
			if (m_total > m_currentByte)
			{
				m_pDownLoadFileThread->PostThreadMessage(UM_FILEDOWNLOAD, 4, m_total);
				m_currentByte = m_total;
			}


			if (m_total != 0)
				nPos = static_cast<int>((m_byte*1.0f) / (m_total*1.0f)*100.0f);

			m_pDownLoadFileThread->PostThreadMessage(UM_FILEDOWNLOAD, 1, nPos);

			switch (status)
			{
			case 0:
				m_Status = _T("No Transfer in progress.");
				bDownLoadStatus = false;
				break;
			case 1:
				bDownLoadStatus = true;
				m_Status = _T("Transfer in progress.");
				break;
			case 2:
				m_Status = _T("End of transfer.");
				m_pDownLoadFileThread->PostThreadMessage(UM_FILEDOWNLOAD, 2, nPos);
				m_bIsFileDownloadSucceed = true;
				bDownLoadStatus = false;
				break;
			case 3:
				m_pDownLoadFileThread->PostThreadMessage(UM_FILEDOWNLOAD, 5, nPos);
				bDownLoadStatus = false;
				break;
			default:
				pParker->m_controlChannel->StopDownload();
				m_pDownLoadFileThread->PostThreadMessage(UM_FILEDOWNLOAD, 5, nPos);
				bDownLoadStatus = false;
				break;
			}
		}
	}
}

void CPrintFileOperateView::ExitDownloadUIThread()
{
	//结束文件加载的用户界面线程
	if (NULL != m_pDownLoadFileThread)
	{
		m_pDownLoadFileThread->ExitInstance();
		CloseHandle(m_pDownLoadFileThread->m_hThread);
		m_pDownLoadFileThread->m_hThread = NULL;
		m_pDownLoadFileThread = NULL;
	}
}


void CPrintFileOperateView::UpdatePrintOrignPos(double dbXPos, double dbYPos){
	db_XOrignPos = dbXPos;
	db_YOrignPos = dbYPos;

	CString editText = _T("");

	editText.Format(_T("%.4f"), db_XOrignPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_XPRINTORIGN_EDIT)->SetWindowText(editText);

	editText.Format(_T("%.4f"), db_YOrignPos);
	GetDlgItem(IDC_JETPRINTDT_PRINT_FILEOPERATE_YPRINTORIGN_EDIT)->SetWindowText(editText);
}

bool CPrintFileOperateView::IsEqual(double dbLeft, double dbRight){
	bool bResult = false;
	bResult = (abs(dbLeft - dbRight) < DOUBLE_ERROR) ? true : false;

	return bResult;
}

void CPrintFileOperateView::OnBnClickedJetprintdtPrintFileoperateMatchprintButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (nullptr == m_pMatchView){
		m_pMatchView = new CPrintMatchView;
		if (nullptr != m_pMatchView)
			m_pMatchView->Create(IDD_JETPRINTDT_PRINT_MATCH_DIALOG);
	}
	m_pMatchView->ShowWindow(SW_SHOW);
}
