
#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "JetPrintDT.h"
#include "MainFrm.h"

#include "JetPrintDTDoc.h"
#include "PrintParamSetView.h"
#include "PrintCameraView.h"
#include "MainDXFView.h"

#include "SplashWindow.h"
#include "SingleTon/HostInstance.h"
#include "Database/ParamTable.h"
#include "Database/MotionTable.h"
#include "Device/DeviceHarvardPump.h"
#include "Device/DeviceVisa.h"
#include "Device/DeviceParker.h"
#include "Device/DeviceADModule.h"
#include "Device/DeviceCamera.h"
#include "Device/DeviceWatchLightPort.h"
#include "Device/DeviceLocateLightPort.h"
#include "Device/DeviceHeatModule.h"

#include "SingleTon/ParamInitSingleton.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


TCHAR g_strKSCoreAppName[] = _T("AFX_KSInstall_CPP__12036F8B_8301_46e2_ADC5_A14A44A85877__");
HANDLE g_hValue = (HANDLE)2055;
// 定义枚举窗口回调函数
BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lParam)
{
	//TCHAR str[200] = {0};
	//::GetWindowText(hwnd, str, 200);
	HANDLE h = GetProp(hwnd, g_strKSCoreAppName);
	if (h == g_hValue)
	{
		*(HWND*)lParam = hwnd;
		return FALSE;
	}
	return TRUE;
}


IMPLEMENT_DYNAMIC(CJetPrintDTDocTemplate, CSingleDocTemplate)

CJetPrintDTDocTemplate::CJetPrintDTDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass,
int nSplitterID, int nRow, int nCol) :
CSingleDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
	m_nSplitterID = nSplitterID;
	m_nRow = nRow;
	m_nCol = nCol;
	m_pView = NULL;
}

//CJetPrintDTWinApp
IMPLEMENT_DYNAMIC(CJetPrintDTWinApp, CWinApp)

CJetPrintDTWinApp::CJetPrintDTWinApp()
{
}

CJetPrintDTDocTemplate* CJetPrintDTWinApp::GetTemplate(CView* pView)
{
	POSITION pos;
	CJetPrintDTDocTemplate* pTemplate = NULL;

	pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		pTemplate = (CJetPrintDTDocTemplate*)GetNextDocTemplate(pos);
		if (pTemplate->m_pView == pView) return pTemplate;
	}
	return NULL;
}

CJetPrintDTDocTemplate* CJetPrintDTWinApp::GetTemplate(CRuntimeClass *pViewClass)
{
	POSITION pos;
	CJetPrintDTDocTemplate* pTemplate = NULL;

	pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		pTemplate = (CJetPrintDTDocTemplate*)GetNextDocTemplate(pos);
		if (pTemplate->m_pView->IsKindOf(pViewClass)) 
			return pTemplate;
	}
	return NULL;
}

CJetPrintDTDocTemplate* CJetPrintDTWinApp::GetFirstTemplate()
{
	POSITION pos;
	CJetPrintDTDocTemplate* pTemplate = NULL;
	pos = GetFirstDocTemplatePosition();
	pTemplate = (CJetPrintDTDocTemplate*)GetNextDocTemplate(pos);
	ASSERT(pTemplate != NULL);
	return pTemplate;
}


//int CJetPrintDTWinApp::GetInt(LPCTSTR lpszEntry, int nDefault = 0){
//	return CWinApp::
//}
// CJetPrintDTApp

BEGIN_MESSAGE_MAP(CJetPrintDTApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CJetPrintDTApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CJetPrintDTApp 构造

CJetPrintDTApp::CJetPrintDTApp()
{
	m_bHiColorIcons = TRUE;
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO:  将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("JetPrintDT.AppID.NoVersion"));
}

// 唯一的一个 CJetPrintDTApp 对象

CJetPrintDTApp theApp;


// CJetPrintDTApp 初始化

BOOL CJetPrintDTApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CJetPrintDTWinApp::InitInstance();

	/*SkinH_Attach();
	SkinH_SetAero(FALSE);*/
	//SkinH_SetMenuAlpha(FALSE);
	//SkinH_AdjustHSV(-180, -100, -100);

	// 主窗口创建前判断
	HWND oldHWnd = NULL;
	::EnumWindows(EnumWndProc, (LPARAM)&oldHWnd);    //枚举所有运行的窗口
	if (oldHWnd != NULL)
	{
		::MessageBox(NULL, TEXT("应用程序已经运行。"), TEXT("注意"), MB_OK);

		::ShowWindow(oldHWnd, SW_NORMAL);     // 显示
		::SetForegroundWindow(oldHWnd);       // 激活
		return FALSE;
	}


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	
	CSplashWindow::ShowSplashScreen(NULL, _T("应用程序启动中..."));
	Sleep(200);
	//OpenCamera Moudle
	OpenCameraModule();
	Sleep(500);

	//initialize main param
	CParamInitSingleton* pParamSingleton = CParamInitSingleton::Instance();
	if (pParamSingleton){
		CSplashWindow::ShowSplashScreen(NULL, _T("初始化参数..."));
		pParamSingleton->init();

		Sleep(200);
	}

	//initialize camera
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){

		CSplashWindow::ShowSplashScreen(NULL, _T("相机初始化中..."));
		pCamera->InitializeCamera();
		Sleep(100);
		if (pCamera->GetWatchCameraStatus())
			CSplashWindow::ShowSplashScreen(NULL, _T("观测相机初始化成功"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("观测相机初始化失败"));

		Sleep(1000);

		if (pCamera->GetLocateCameraStatus())
			CSplashWindow::ShowSplashScreen(NULL, _T("定位相机初始化成功"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("定位相机初始化失败"));

		Sleep(1000);
	}

	//initialize parker
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	if (pParker){
		CSplashWindow::ShowSplashScreen(NULL, _T("控制器初始化中..."));
		pParker->JetParkerIsConnected();
		
		//Bit 33
		if (!pParker->JetParkerIsBitChecked(4097, 1)){
			pParker->JetParkerSetAglient(true);
			Sleep(35000);
		}
		
		CSplashWindow::ShowSplashScreen(NULL, _T("控制器初始化成功"));
		Sleep(1000);
	}
#endif

	//initialize visa
#if _DEBUGE_VISA
	CDeviceVisa *pVisa = CDeviceVisa::Instance();
	if (pVisa){
		CSplashWindow::ShowSplashScreen(NULL, _T("函数发生器初始化中..."));
		if(pVisa->InitInstrument())
			CSplashWindow::ShowSplashScreen(NULL, _T("函数发生器初始化成功"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("函数发生器初始化失败"));
		
		Sleep(1000);
	}
#endif

	//initialize pump
	CDeviceHarvardPump *pPump = CDeviceHarvardPump::Instance();
	CSplashWindow::ShowSplashScreen(NULL, _T("流量泵初始化中..."));
	if (pPump){
		pPump->Init(PUMP_PORT_NUMBER);
		if (pPump->IsHarvardOpen())
			CSplashWindow::ShowSplashScreen(NULL, _T("流量泵初始化成功"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("流量泵初始化失败"));

		Sleep(1000);
	}

	//initialize watch light port
	CDeviceWatchLightPort *pWatchLightPort = CDeviceWatchLightPort::Instance();
	CSplashWindow::ShowSplashScreen(NULL, _T("观测光源初始化中..."));
	if (pWatchLightPort){
		pWatchLightPort->InitLightPort();
		Sleep(200);
		if (pWatchLightPort->IsPortOpen())
			CSplashWindow::ShowSplashScreen(NULL, _T("观测光源初始化成功"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("观测光源初始化失败"));

		Sleep(1000);
	}
	//initialize locate light port
	CDeviceLocateLightPort *pLocateLightPort = CDeviceLocateLightPort::Instance();
	CSplashWindow::ShowSplashScreen(NULL, _T("定位光源初始化中..."));
	if (pLocateLightPort){
		pLocateLightPort->InitLightPort();
		Sleep(200);
		if (pLocateLightPort->IsPortOpen())
			CSplashWindow::ShowSplashScreen(NULL, _T("定位光源初始化成功"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("定位光源初始化失败"));

		Sleep(1000);
	}

	//initialize Heat module
	CSplashWindow::ShowSplashScreen(NULL, _T("加热模块初始化中..."));
	CDeviceHeatModule *pHeatMoudle = CDeviceHeatModule::Instance();
	if (pHeatMoudle){
		if (pHeatMoudle->Init()){
			CSplashWindow::ShowSplashScreen(NULL, _T("加热模块初始化成功"));
			pHeatMoudle->StopHeat();
		}
			
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("加热模块初始化失败"));
		
		Sleep(1000);
	}

	//initialize AD module
	CSplashWindow::ShowSplashScreen(NULL, _T("电压监控初始化中..."));
	CDeviceADModule *pADMoudle = CDeviceADModule::Instance();
	if (pADMoudle){
		if(pADMoudle->Init(3))
			CSplashWindow::ShowSplashScreen(NULL, _T("电压监控初始化成功"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("电压监控初始化失败"));

		
		Sleep(200);
	}

	//check host support information
	CHostInstance *pInstance = CHostInstance::Instance();
	if(pInstance){
		pInstance->InitializeHost(NULL);

		CSplashWindow::ShowSplashScreen(NULL, _T("检查主机支持信息..."));
		Sleep(200);
	}
	
	//initialize database
	CParamTable* pParamTable = CParamTable::Instance();
	CMotionTable* pMotionTable = CMotionTable::Instance();
	CSplashWindow::ShowSplashScreen(NULL, _T("初始化数据库..."));
	if (pParamTable&&pMotionTable){
		pParamTable->open();
		pMotionTable->open();

		if (pParamTable->IsOpen() && pMotionTable->IsOpen()){
			pParamTable->Close();
			pMotionTable->Close();

			Sleep(200);
		}
	}

	CSplashWindow::HideSplashScreen();
	
	SetRegistryKey(_T("ME-JETDT 钟瑞"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	//CJetPrintDTDocTemplate* pDocTemplate;
	
	CJetPrintDTDocTemplate* pDocTemplate;
	pDocTemplate = new CJetPrintDTDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CJetPrintDTDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CMainDXFView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CJetPrintDTDocTemplate(
		IDR_NEWITEM,
		RUNTIME_CLASS(CJetPrintDTDoc),
		RUNTIME_CLASS(CMainFrame),        // PRINTLeftView Combine With PRINTRightView
		RUNTIME_CLASS(CPrintParamSetView), 0, 0, 0);
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CJetPrintDTDocTemplate(
		IDR_NEWITEM,
		RUNTIME_CLASS(CJetPrintDTDoc),
		RUNTIME_CLASS(CMainFrame),        // PRINTRightView Combine With PRINTLeftView
		RUNTIME_CLASS(CPrintCameraView), 0, 0, 1);
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pMainWnd->ShowWindow(SHOW_FULLSCREEN);
	//m_pMainWnd->CenterWindow();
	m_pMainWnd->UpdateWindow();
	
	::SetProp(m_pMainWnd->m_hWnd, g_strKSCoreAppName, g_hValue);

	CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	if (pMainFrame){
		pMainFrame->StartWatchParkerStatus();
		pMainFrame->UpdateWindowState();
		pMainFrame->StartCheckTemp();
		pMainFrame->UpdateLightSlider();
		pMainFrame->UpdateTempSlider();
	}
		
	
	return TRUE;
}

int CJetPrintDTApp::ExitInstance()
{
	//TODO:  处理可能已添加的附加资源
	AfxOleTerm(FALSE);
	//CSingletonObserver::Instance()->RemoveAllAndDie();
	return CJetPrintDTWinApp::ExitInstance();
}

// CJetPrintDTApp 消息处理程序

bool CJetPrintDTApp::OpenCameraModule(){
	CString cameraExeName = _T("");
	CString cameraExePath = _T("");
	TCHAR progFolder[MAX_PATH];
	memset(progFolder, 0x00, sizeof(progFolder));
	GetModuleFileName(AfxGetInstanceHandle(), progFolder, MAX_PATH);

	cameraExeName = progFolder;
	int nPos = cameraExeName.ReverseFind('\\');
	cameraExeName = cameraExeName.Left(nPos);
	cameraExePath = cameraExeName + _T("\\");
	cameraExeName = cameraExeName + _T("\\CameraServer.exe");

	SHELLEXECUTEINFO sei = { 0 };

	const ULONG fMask =
		SEE_MASK_FLAG_DDEWAIT |
		SEE_MASK_FLAG_NO_UI |
		SEE_MASK_NOCLOSEPROCESS;

	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.hwnd = NULL;
	sei.fMask = fMask;
	sei.lpVerb = TEXT("open");
	sei.lpFile = cameraExeName;
	sei.lpParameters = NULL;
	sei.lpDirectory = cameraExePath;
	sei.nShow = SW_SHOWNORMAL;

	if (::ShellExecuteEx(&sei)){
		return true;
	}

	return false;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CJetPrintDTApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CJetPrintDTApp 自定义加载/保存方法

void CJetPrintDTApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CJetPrintDTApp::LoadCustomState()
{
}

void CJetPrintDTApp::SaveCustomState()
{
}

// CJetPrintDTApp 消息处理程序

void CAboutDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


BOOL CJetPrintDTApp::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	ASSERT(pMsg);
	/// 如果是激活窗口消息
	if (CSplashWindow::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CJetPrintDTWinApp::PreTranslateMessage(pMsg);
}


BOOL CAboutDlg::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CDialogEx::Create(lpszTemplateName, pParentWnd);
}


BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CDialogEx::PreTranslateMessage(pMsg);
}
