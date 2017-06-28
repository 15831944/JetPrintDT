
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
// ����ö�ٴ��ڻص�����
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
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CJetPrintDTApp ����

CJetPrintDTApp::CJetPrintDTApp()
{
	m_bHiColorIcons = TRUE;
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO:  ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("JetPrintDT.AppID.NoVersion"));
}

// Ψһ��һ�� CJetPrintDTApp ����

CJetPrintDTApp theApp;


// CJetPrintDTApp ��ʼ��

BOOL CJetPrintDTApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CJetPrintDTWinApp::InitInstance();

	/*SkinH_Attach();
	SkinH_SetAero(FALSE);*/
	//SkinH_SetMenuAlpha(FALSE);
	//SkinH_AdjustHSV(-180, -100, -100);

	// �����ڴ���ǰ�ж�
	HWND oldHWnd = NULL;
	::EnumWindows(EnumWndProc, (LPARAM)&oldHWnd);    //ö���������еĴ���
	if (oldHWnd != NULL)
	{
		::MessageBox(NULL, TEXT("Ӧ�ó����Ѿ����С�"), TEXT("ע��"), MB_OK);

		::ShowWindow(oldHWnd, SW_NORMAL);     // ��ʾ
		::SetForegroundWindow(oldHWnd);       // ����
		return FALSE;
	}


	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	
	CSplashWindow::ShowSplashScreen(NULL, _T("Ӧ�ó���������..."));
	Sleep(200);
	//OpenCamera Moudle
	OpenCameraModule();
	Sleep(500);

	//initialize main param
	CParamInitSingleton* pParamSingleton = CParamInitSingleton::Instance();
	if (pParamSingleton){
		CSplashWindow::ShowSplashScreen(NULL, _T("��ʼ������..."));
		pParamSingleton->init();

		Sleep(200);
	}

	//initialize camera
	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera){

		CSplashWindow::ShowSplashScreen(NULL, _T("�����ʼ����..."));
		pCamera->InitializeCamera();
		Sleep(100);
		if (pCamera->GetWatchCameraStatus())
			CSplashWindow::ShowSplashScreen(NULL, _T("�۲������ʼ���ɹ�"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("�۲������ʼ��ʧ��"));

		Sleep(1000);

		if (pCamera->GetLocateCameraStatus())
			CSplashWindow::ShowSplashScreen(NULL, _T("��λ�����ʼ���ɹ�"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("��λ�����ʼ��ʧ��"));

		Sleep(1000);
	}

	//initialize parker
#if _DEBUGE_PARKER
	CDeviceParker *pParker = CDeviceParker::Instance();
	if (pParker){
		CSplashWindow::ShowSplashScreen(NULL, _T("��������ʼ����..."));
		pParker->JetParkerIsConnected();
		
		//Bit 33
		if (!pParker->JetParkerIsBitChecked(4097, 1)){
			pParker->JetParkerSetAglient(true);
			Sleep(35000);
		}
		
		CSplashWindow::ShowSplashScreen(NULL, _T("��������ʼ���ɹ�"));
		Sleep(1000);
	}
#endif

	//initialize visa
#if _DEBUGE_VISA
	CDeviceVisa *pVisa = CDeviceVisa::Instance();
	if (pVisa){
		CSplashWindow::ShowSplashScreen(NULL, _T("������������ʼ����..."));
		if(pVisa->InitInstrument())
			CSplashWindow::ShowSplashScreen(NULL, _T("������������ʼ���ɹ�"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("������������ʼ��ʧ��"));
		
		Sleep(1000);
	}
#endif

	//initialize pump
	CDeviceHarvardPump *pPump = CDeviceHarvardPump::Instance();
	CSplashWindow::ShowSplashScreen(NULL, _T("�����ó�ʼ����..."));
	if (pPump){
		pPump->Init(PUMP_PORT_NUMBER);
		if (pPump->IsHarvardOpen())
			CSplashWindow::ShowSplashScreen(NULL, _T("�����ó�ʼ���ɹ�"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("�����ó�ʼ��ʧ��"));

		Sleep(1000);
	}

	//initialize watch light port
	CDeviceWatchLightPort *pWatchLightPort = CDeviceWatchLightPort::Instance();
	CSplashWindow::ShowSplashScreen(NULL, _T("�۲��Դ��ʼ����..."));
	if (pWatchLightPort){
		pWatchLightPort->InitLightPort();
		Sleep(200);
		if (pWatchLightPort->IsPortOpen())
			CSplashWindow::ShowSplashScreen(NULL, _T("�۲��Դ��ʼ���ɹ�"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("�۲��Դ��ʼ��ʧ��"));

		Sleep(1000);
	}
	//initialize locate light port
	CDeviceLocateLightPort *pLocateLightPort = CDeviceLocateLightPort::Instance();
	CSplashWindow::ShowSplashScreen(NULL, _T("��λ��Դ��ʼ����..."));
	if (pLocateLightPort){
		pLocateLightPort->InitLightPort();
		Sleep(200);
		if (pLocateLightPort->IsPortOpen())
			CSplashWindow::ShowSplashScreen(NULL, _T("��λ��Դ��ʼ���ɹ�"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("��λ��Դ��ʼ��ʧ��"));

		Sleep(1000);
	}

	//initialize Heat module
	CSplashWindow::ShowSplashScreen(NULL, _T("����ģ���ʼ����..."));
	CDeviceHeatModule *pHeatMoudle = CDeviceHeatModule::Instance();
	if (pHeatMoudle){
		if (pHeatMoudle->Init()){
			CSplashWindow::ShowSplashScreen(NULL, _T("����ģ���ʼ���ɹ�"));
			pHeatMoudle->StopHeat();
		}
			
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("����ģ���ʼ��ʧ��"));
		
		Sleep(1000);
	}

	//initialize AD module
	CSplashWindow::ShowSplashScreen(NULL, _T("��ѹ��س�ʼ����..."));
	CDeviceADModule *pADMoudle = CDeviceADModule::Instance();
	if (pADMoudle){
		if(pADMoudle->Init(3))
			CSplashWindow::ShowSplashScreen(NULL, _T("��ѹ��س�ʼ���ɹ�"));
		else
			CSplashWindow::ShowSplashScreen(NULL, _T("��ѹ��س�ʼ��ʧ��"));

		
		Sleep(200);
	}

	//check host support information
	CHostInstance *pInstance = CHostInstance::Instance();
	if(pInstance){
		pInstance->InitializeHost(NULL);

		CSplashWindow::ShowSplashScreen(NULL, _T("�������֧����Ϣ..."));
		Sleep(200);
	}
	
	//initialize database
	CParamTable* pParamTable = CParamTable::Instance();
	CMotionTable* pMotionTable = CMotionTable::Instance();
	CSplashWindow::ShowSplashScreen(NULL, _T("��ʼ�����ݿ�..."));
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
	
	SetRegistryKey(_T("ME-JETDT ����"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ע��Ӧ�ó�����ĵ�ģ�塣  �ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	//CJetPrintDTDocTemplate* pDocTemplate;
	
	CJetPrintDTDocTemplate* pDocTemplate;
	pDocTemplate = new CJetPrintDTDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CJetPrintDTDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
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

	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ��������������ָ�������  ���
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
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
	//TODO:  �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);
	//CSingletonObserver::Instance()->RemoveAllAndDie();
	return CJetPrintDTWinApp::ExitInstance();
}

// CJetPrintDTApp ��Ϣ�������

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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CJetPrintDTApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CJetPrintDTApp �Զ������/���淽��

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

// CJetPrintDTApp ��Ϣ�������

void CAboutDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


BOOL CJetPrintDTApp::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
	ASSERT(pMsg);
	/// ����Ǽ������Ϣ
	if (CSplashWindow::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CJetPrintDTWinApp::PreTranslateMessage(pMsg);
}


BOOL CAboutDlg::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO:  �ڴ����ר�ô����/����û���

	return CDialogEx::Create(lpszTemplateName, pParentWnd);
}


BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���

	return CDialogEx::PreTranslateMessage(pMsg);
}
