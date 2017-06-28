// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���  
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�  
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://go.microsoft.com/fwlink/?LinkId=238214��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once
#include <SDKDDKVer.h>

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS
#include <afx.h>
#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

#define PI	3.1415926535
#define _DEBUGE_MODE 0
#define _DEBUGE_VISA 0
#define _DEBUGE_PARKER 0

#define VISA_OUTPUT    32
#define TIMER_UPDATE_IMAGE 0
#define WM_STATUS_MESSAGE			WM_USER+3
#define WM_BUTTON_FIRST_SELECT		WM_USER+4
#define UM_FILEDOWNLOAD				WM_USER+5

#define WM_PROG_FINISHED			WM_USER+7
#define WM_MOTION_KILL				WM_USER+8
#define WM_SET_OUTPUT				WM_USER+13
#define WM_FILE_LOADED              WM_USER+14
#define WM_FREQ_CHANGED             WM_USER+15
#define UM_WAITMESSAGEDLG			WM_USER+20
#define WM_UPDATE_MOTIONSTATE       WM_USER+21
#define WM_CHANGE_STEP_LENTH        WM_USER+28
#define WM_CHANGE_POSITION          WM_USER+29
#define WM_PALETTE_JOG              WM_USER+30
#define WM_PALETTE_STOP_JOG         WM_USER+31
#define WM_HOME_FAILED              WM_USER+32

#include   "GdiPlus.h" 
using   namespace   Gdiplus;
#pragma   comment(lib,   "gdiplus.lib")
#include "SkinH.h"
#pragma comment(lib,"SkinH.lib")

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


