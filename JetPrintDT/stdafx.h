// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once
#include <SDKDDKVer.h>

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS
#include <afx.h>
#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

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


