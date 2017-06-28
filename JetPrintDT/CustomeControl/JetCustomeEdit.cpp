// HMXEdit.cpp : implementation file
//

/********************************************************************

	created:	2001/10/25
	file:		HMXEdit
	author:		Massimo Colurcio
	homepage:	http://www.softhor.com/developmentarea
	email:		m.colurcio@softhor.com
	
	purpose:	new kind of CEdit class

	special thanks to Davide Calabro' (tooltip from BtnST)

*********************************************************************/
#include "../stdafx.h"
#include "JetCustomeEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJetCustomeEdit

CJetCustomeEdit::CJetCustomeEdit()
{
	m_clrBkGnd = GetSysColor(COLOR_WINDOW);
	m_clrText = GetSysColor(COLOR_WINDOWTEXT);
	m_clrBkGndFocus = m_clrBkGnd;
	m_clrTextFocus = m_clrText;
	m_brsBkGnd.CreateSolidBrush(m_clrBkGnd);
	m_brsBkGndFocus.CreateSolidBrush(m_clrBkGndFocus);
	m_bEnableEditing = true;
	m_bHasFocus = false;
}

CJetCustomeEdit::~CJetCustomeEdit()
{
	m_brsBkGnd.DeleteObject();
	m_brsBkGndFocus.DeleteObject();
	m_fntText.DeleteObject();
}


BEGIN_MESSAGE_MAP(CJetCustomeEdit, CEdit)
	//{{AFX_MSG_MAP(CJetCustomeEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillFocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetFocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJetCustomeEdit message handlers

HBRUSH CJetCustomeEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	pDC->SetBkColor(m_bHasFocus?m_clrBkGndFocus:m_clrBkGnd);
	pDC->SetTextColor(m_bHasFocus?m_clrTextFocus:m_clrText);
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return m_bHasFocus?(HBRUSH)m_brsBkGndFocus:(HBRUSH)m_brsBkGnd;
}

/********************************************************************

	created:	2001/10/25
	in:			clr, bRedraw
	out:		none
	return:		always true;
	
	purpose:	set background color

*********************************************************************/

bool CJetCustomeEdit::SetBkClr(COLORREF clr)
{
	m_clrBkGnd = clr;
	m_brsBkGnd.DeleteObject();
	m_brsBkGnd.CreateSolidBrush(clr);
	Invalidate();
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			none
	out:		clr
	return:		always true
	
	purpose:	return background color

*********************************************************************/

bool CJetCustomeEdit::GetBkClr(COLORREF & clr)
{
	clr = m_clrBkGnd;
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			clr, bRedraw
	out:		none
	return:		always true;
	
	purpose:	set Text color

*********************************************************************/

bool CJetCustomeEdit::SetTextClr(COLORREF clr)
{
	m_clrText = clr;
	Invalidate();

	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			none
	out:		clr
	return:		always true
	
	purpose:	get text color

*********************************************************************/

bool CJetCustomeEdit::GetTextClr(COLORREF & clr)
{
	clr = m_clrText;
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			clr, bRedraw
	out:		none
	return:		always true;
	
	purpose:	set Text color when control has focus

*********************************************************************/

bool CJetCustomeEdit::SetBkClrFocus(COLORREF clr)
{
	m_clrBkGndFocus = clr;
	m_brsBkGndFocus.DeleteObject();
	m_brsBkGndFocus.CreateSolidBrush(clr);
	Invalidate();

	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			none
	out:		clr
	return:		always true
	
	purpose:	get background color when control has focus

*********************************************************************/

bool CJetCustomeEdit::GetBkClrFocus(COLORREF & clr)
{
	clr = m_clrBkGndFocus;
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			clr, bRedraw
	out:		none
	return:		always true;
	
	purpose:	set Text color when control has focus

*********************************************************************/

bool CJetCustomeEdit::SetTextClrFocus(COLORREF clr)
{
	m_clrTextFocus = clr;
	Invalidate();

	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			none
	out:		clr
	return:		always true
	
	purpose:	get text color when control has focus

*********************************************************************/

bool CJetCustomeEdit::GetTextClrFocus(COLORREF & clr)
{
	clr = m_clrTextFocus;
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			bEditing
	out:		none
	return:		always true
	
	purpose:	enable / diable editing

*********************************************************************/

bool CJetCustomeEdit::EnableEditing(bool bEditing)
{
	m_bEnableEditing = bEditing;
	return true;
}

LRESULT CJetCustomeEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( !m_bEnableEditing )
		if( message == WM_PASTE || message == WM_KEYDOWN || message == WM_CHAR || message == WM_RBUTTONDOWN)
			return FALSE;

	return CEdit::WindowProc(message, wParam, lParam);
}

/********************************************************************

	created:	2001/10/25
	in:			nHeight, bBold, bItalic, sFaceName
	out:		none
	return:		always true
	
	purpose:	set the font for the control

*********************************************************************/

bool CJetCustomeEdit::SetTextFont( LONG nHeight, bool bBold, bool bItalic, const CString& sFaceName )
{
	LOGFONT lgfnt;

	lgfnt.lfHeight			= -MulDiv(nHeight, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);    
	lgfnt.lfWidth			= 0; 
	lgfnt.lfEscapement		= 0;    
	lgfnt.lfOrientation		= 0;    
	lgfnt.lfWeight			= bBold?FW_BOLD:FW_DONTCARE; 
	lgfnt.lfItalic			= bItalic?TRUE:FALSE;    
	lgfnt.lfUnderline		= FALSE;    
	lgfnt.lfStrikeOut		= FALSE;    
	lgfnt.lfCharSet			= DEFAULT_CHARSET; 
	lgfnt.lfOutPrecision	= OUT_DEFAULT_PRECIS;    
	lgfnt.lfClipPrecision	= CLIP_DEFAULT_PRECIS;    
	lgfnt.lfQuality			= DEFAULT_QUALITY; 
	lgfnt.lfPitchAndFamily	= DEFAULT_PITCH | FF_DONTCARE;    

	wcscpy_s(lgfnt.lfFaceName, sFaceName );

	return SetTextFont( lgfnt );

	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			lgFont
	out:		none
	return:		always true
	
	purpose:	set the font for the control

*********************************************************************/

bool CJetCustomeEdit::SetTextFont( const LOGFONT& lgfnt )
{
	m_fntText.DeleteObject();
	m_fntText.CreateFontIndirect( &lgfnt );
	SetFont( &m_fntText, TRUE );
	
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			none
	out:		lgFont
	return:		always true
	
	purpose:	get text font

*********************************************************************/

bool CJetCustomeEdit::GetTextFont( LOGFONT* plgfnt)
{
	GetFont()->GetObject( sizeof(LOGFONT), (void*)plgfnt);
	return true;
}

void CJetCustomeEdit::OnKillFocus()
{
	// TODO: Add your control notification handler code here
	m_bHasFocus = false;
	Invalidate();
	
	CEdit::OnKillFocus(NULL);
}

void CJetCustomeEdit::OnSetFocus()
{
	// TODO: Add your control notification handler code here
	m_bHasFocus = true;
	Invalidate();
	
	CEdit::OnSetFocus(NULL);
}

/********************************************************************

	created:	2001/10/25
	in:			bBold
	out:		none
	return:		always true
	
	purpose:	set font bold

*********************************************************************/

bool CJetCustomeEdit::SetFontBold( bool bBold )
{
	LOGFONT lgfnt;
	
	GetTextFont( &lgfnt );
	lgfnt.lfWeight	= bBold?FW_BOLD:FW_DONTCARE; 
	
	SetTextFont( lgfnt );
	
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			bItalic
	out:		none
	return:		always true
	
	purpose:	set the font italic

*********************************************************************/

bool CJetCustomeEdit::SetFontItalic( bool bItalic )
{
	LOGFONT lgfnt;
	
	GetTextFont( &lgfnt );
	lgfnt.lfItalic	= bItalic ? TRUE : FALSE; 
	
	SetTextFont( lgfnt );
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			nHeight
	out:		none
	return:		always true
	
	purpose:	set the font height

*********************************************************************/

bool CJetCustomeEdit::SetFontHeight( int nHeight )
{
	LOGFONT lgfnt;
	
	GetTextFont( &lgfnt );
	lgfnt.lfHeight	= -MulDiv(nHeight, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);    
	lgfnt.lfWidth	= 0; 
	
	SetTextFont( lgfnt );
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			sFaceName
	out:		none
	return:		always true
	
	purpose:	set the font face name

*********************************************************************/

bool CJetCustomeEdit::SetFontFaceName( const CString& sFaceName )
{
	LOGFONT lgfnt;
	
	GetTextFont( &lgfnt );
	wcscpy_s( lgfnt.lfFaceName, sFaceName);
	
	SetTextFont( lgfnt );
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			none
	out:		none
	return:		always true
	
	purpose:	init tooltip

*********************************************************************/

bool CJetCustomeEdit::InitToolTip()
{
	if (m_tt.m_hWnd == NULL) {
		m_tt.Create(this);
		m_tt.Activate(true);
		m_tt.SendMessage(TTM_SETMAXTIPWIDTH, 0, 400);
	}
	
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			sText, bActivate
	out:		none
	return:		always true
	
	purpose:	set tooltip text

*********************************************************************/

bool CJetCustomeEdit::SetToolTipText(const CString& sText, bool bActivate)
{
	InitToolTip(); 
	
	// If there is no tooltip defined then add it
	if (m_tt.GetToolCount() == 0)
	{
		CRect rect; 
		GetClientRect(rect);
		m_tt.AddTool(this, sText, rect, 1);
	}
	
	m_tt.UpdateTipText(sText, this, 1);
	m_tt.Activate(bActivate);
	
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			bActivate
	out:		none
	return:		always true
	
	purpose:	activate/deactivate tooltip

*********************************************************************/

bool CJetCustomeEdit::ActivateToolTip(bool bActivate)
{
	if (m_tt.GetToolCount() == 0)
		return false;
	
	// Activate tooltip
	m_tt.Activate(bActivate);
	
	return true;
}

/********************************************************************

	created:	2001/10/25
	in:			see CWnd::PretanslateMessage
	out:		see CWnd::PretanslateMessage
	return:		see CWnd::PretanslateMessage
	
	purpose:	let tooltip works

*********************************************************************/

BOOL CJetCustomeEdit::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_tt.RelayEvent(pMsg);
	
	return CEdit::PreTranslateMessage(pMsg);
} 

