
#include "JetGroupBox.h"

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#undef TRACE
#undef TRACERECT
#define TRACE __noop
#define TRACERECT __noop

//=============================================================================
// if you want to see the TRACE output, uncomment this line:
//#include "XTrace.h"
//=============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4996)			// disable bogus deprecation warning

//=============================================================================
// set resource handle (in case used in DLL)
//=============================================================================
#ifdef _USRDLL
#define AFXMANAGESTATE AfxGetStaticModuleState
#else
#define AFXMANAGESTATE AfxGetAppModuleState
#endif

//=============================================================================
BEGIN_MESSAGE_MAP(CJetGroupBox, CStatic)
	//=============================================================================
	//{{AFX_MSG_MAP(CJetGroupBox)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//=============================================================================
CJetGroupBox::CJetGroupBox() :
//=============================================================================
m_crText(GetSysColor(COLOR_BTNTEXT)),
m_crBackground(GetSysColor(COLOR_BTNFACE)),
m_crBorder(CLR_INVALID),
m_nYOffset(-1),
m_rect(CRect(0, 0, 0, 0)),
m_sizeText(CSize(0, 0)),
m_eBorderStyle(etched),
m_eControlAlignment(center),
m_filletRadius(0)
{
	TRACE(_T("in CJetGroupBox::CJetGroupBox\n"));
}

//=============================================================================
CJetGroupBox::~CJetGroupBox()
{
	if (m_font.GetSafeHandle())
		m_font.DeleteObject();
}

//=============================================================================
void CJetGroupBox::OnPaint()
{
	static BOOL bInPaint = FALSE;

	if (bInPaint)
		return;
	bInPaint = TRUE;
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetWindowRect(&rect);
	int windowWidth = rect.Width();
	int windowHeight = rect.Height();
	rect.left = 0;
	rect.top = 0;
	rect.right = windowWidth;
	rect.bottom = windowHeight;

	double fRadius = windowHeight*0.61*0.61*0.61;

	CString strRadius= _T("");
	strRadius.Format(_T("%.4f"),fRadius);
	m_filletRadius = _wtoi(strRadius);
	DrawItem(&dc, rect);

	bInPaint = FALSE;
}

//=============================================================================
void CJetGroupBox::DrawItem(CDC *pDC, CRect& rect)
{
	TRACE(_T("in CJetGroupBox::DrawItem\n"));

	ASSERT(pDC);
	if (!pDC)
		return;


	CRect rectItem(rect);
	CRect rectDraw(rectItem);
	TRACERECT(rectItem);
	CRect rectText(rectDraw);
	CRect rectFrame(rectDraw);

	if (m_sizeText.cx == 0)
	{
		// get string width
		CString strText = _T("");
		GetWindowText(strText);

		CFont *pOldFont = pDC->SelectObject(&m_font);
		if (!strText.IsEmpty())
			m_sizeText = pDC->GetTextExtent(strText);
		if (pOldFont)
			pDC->SelectObject(pOldFont);
    }

	m_nYOffset = m_sizeText.cy + 1;
	TRACE(_T("..... m_nYOffset=%d  m_sizeText.cy=%d  m_nIconSize=%d\n"), m_nYOffset, m_sizeText.cy);
	rectText.bottom = rectText.top + m_nYOffset;
	m_nYOffset = m_nYOffset / 2;
	rectFrame.top += m_nYOffset;

	// set up for double buffering
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rectDraw.Width(), rectDraw.Height());
	CBitmap *pOldBitmap = memDC.SelectObject(&bmp);

	// fill in entire window
	memDC.FillSolidRect(&rectItem, m_crBackground);


	TRACE(_T("groupbox\n"));
	// get parent DC and copy current contents - this will emulate 
	// transparent blt'ing

	CRect rectWindow;
	GetWindowRect(&rectWindow);
	GetParent()->ScreenToClient(&rectWindow);
	rectWindow.top += m_nYOffset * 2;

	CDC *pParentDC = GetParent()->GetDC();
	memDC.BitBlt(0, m_nYOffset * 2, rectWindow.Width(), rectWindow.Height(),
		pParentDC, rectWindow.left, rectWindow.top, SRCCOPY);
	VERIFY(ReleaseDC(pParentDC));


	// select NULL brush for border drawing
	HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(memDC.m_hDC, hBrush);


	// erase the old border that exists in the parent DC
	EraseBorder(&memDC, rectFrame, m_crBackground);
	m_rect = rectFrame;

	DrawBorder(&memDC, rectFrame);

	if (hOldBrush)
		::SelectObject(memDC.m_hDC, hOldBrush);

	//rectText.left = rectHeader.left;
	

	//DrawHeader(&memDC, rectText, rectIcon);
	DrawText(&memDC, rectText);

	// end double buffering
	pDC->BitBlt(0, 0, rectDraw.Width(), rectDraw.Height(),
		&memDC, 0, 0, SRCCOPY);

	// swap back the original bitmap
	if (pOldBitmap)
		memDC.SelectObject(pOldBitmap);
	if (bmp.GetSafeHandle())
		bmp.DeleteObject();

	memDC.DeleteDC();
}

//=============================================================================
void CJetGroupBox::EraseBorder(CDC *pDC, CRect& rect, COLORREF crBackground)
//=============================================================================
{
	if (!rect.IsRectEmpty())
	{
		CRect r(rect);
		CPen pen(PS_SOLID, 5, crBackground);
		CPen *pOldPen = pDC->SelectObject(&pen);
		r.DeflateRect(2, 2);
		pDC->Rectangle(&r);
		if (pOldPen)
			pDC->SelectObject(pOldPen);
	}
}

//=============================================================================
void CJetGroupBox::DrawEtchedRectangle(CDC *pDC, CRect& rect, COLORREF cr)
//=============================================================================
{
	TRACE(_T("in CJetGroupBox::DrawEtchedRectangle\n"));
	COLORREF crDark = IsWindowEnabled() ? cr : GetSysColor(COLOR_BTNSHADOW);
	CPen penDark(PS_SOLID, 3, crDark);
	CPen *pOldPen = pDC->SelectObject(&penDark);

	pDC->SelectObject(&penDark);

	EraseBorder(pDC, rect, pDC->GetBkColor());
	//pDC->RoundRect(rect.left, rect.top, rect.right, rect.bottom, m_filletRadius, m_filletRadius);
	pDC->Rectangle(&rect);

	if (pOldPen)
		pDC->SelectObject(pOldPen);
}


//=============================================================================
void CJetGroupBox::DrawBorder(CDC *pDC, CRect& rect)
//=============================================================================
{
	TRACE(_T("DrawBorder: groupbox\n"));

	// user color overrides themes
	if (m_crBorder != CLR_INVALID)
	{
		DrawEtchedRectangle(pDC, rect, m_crBorder);
	}
	else
	{
		// not themed and no user color
		DrawEtchedRectangle(pDC, rect, GetSysColor(COLOR_BTNSHADOW));
	}
}


//=============================================================================
int CJetGroupBox::DrawText(CDC *pDC, CRect& rect)
{
	int rc = rect.left;		// if no string just return start position

	CString strText = _T("");
	GetWindowText(strText);
	TRACE(_T("strText=<%s>\n"), strText);

	if (!strText.IsEmpty())
	{
		COLORREF crText = m_crText;
		if (!IsWindowEnabled())
			crText = GetSysColor(COLOR_GRAYTEXT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
		pDC->SetBkMode(OPAQUE);

		CFont *pOldFont = pDC->SelectObject(&m_font);

		// always left aligned within drawing rect
		UINT nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;

		CRect rectText(rect);
		// get size of text so we can update rect
		CSize size = pDC->GetTextExtent(strText);
		CRect clientRect;
		this->GetClientRect(&clientRect);
		if (m_eControlAlignment == right)
		{
			rectText.right = clientRect.right - m_filletRadius;
			rectText.left = rectText.right - size.cx;

		}
		else if (m_eControlAlignment == center)
		{
			rectText.left = clientRect.left + clientRect.Width() / 2 - size.cx / 2 ;
			rectText.right = rectText.left + size.cx;
		}
		else
		{
			rectText.left += m_filletRadius;
			rectText.right = rectText.left + size.cx;
		}



		pDC->DrawText(strText, rectText, nFormat);

		rc = rectText.right;

		if (pOldFont)
			pDC->SelectObject(pOldFont);
	}

	return rc;
}

//=============================================================================
BOOL CJetGroupBox::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;	
}


//=============================================================================
CFont * CJetGroupBox::GetSafeFont()
{
	// get current font
	CFont *pFont = CWnd::GetFont();

	if (pFont == 0)
	{
		// try to get parent font
		CWnd *pParent = GetParent();
		if (pParent && IsWindow(pParent->m_hWnd))
			pFont = pParent->GetFont();

		if (pFont == 0)
		{
			// no font, so get a system font
			HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
			if (hFont == 0)
				hFont = (HFONT)::GetStockObject(SYSTEM_FONT);
			if (hFont == 0)
				hFont = (HFONT)::GetStockObject(ANSI_VAR_FONT);
			if (hFont)
				pFont = CFont::FromHandle(hFont);
		}
	}

	return pFont;
}

//=============================================================================
int CJetGroupBox::GetFontHeight(int nPointSize)
{
	HDC hdc = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	ASSERT(hdc);
	int cyPixelsPerInch = ::GetDeviceCaps(hdc, LOGPIXELSY);
	::DeleteDC(hdc);

	int nHeight = -MulDiv(nPointSize, cyPixelsPerInch, 72);

	return nHeight;
}

//=============================================================================
CJetGroupBox& CJetGroupBox::SetBold(BOOL bBold,BOOL bRedraw /*= TRUE*/)

{
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));

	CFont* pFont = GetSafeFont();
	ASSERT(pFont);

	if (pFont)
	{
		ASSERT(pFont->GetSafeHandle());

		// create the font for this control
		pFont->GetLogFont(&lf);
		lf.lfCharSet = DEFAULT_CHARSET;
		if (m_font.GetSafeHandle())
			m_font.DeleteObject();
		VERIFY(m_font.CreateFontIndirect(&lf));
	}

	m_font.GetLogFont(&lf);

	lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;

	m_sizeText.cx = 0;		// force string width to be recalculated

	return SetFont(&lf, bRedraw);
}


//=============================================================================
CJetGroupBox& CJetGroupBox::SetFont(LPCTSTR lpszFaceName,int nPointSize,BOOL bRedraw /*= TRUE*/)
{
	// null face name is ok - we will use current font

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));

	if ((lpszFaceName == NULL) || (lpszFaceName[0] == _T('\0')))
		m_font.GetLogFont(&lf);
	else
		_tcsncpy(lf.lfFaceName, lpszFaceName, sizeof(lf.lfFaceName) / sizeof(TCHAR) - 1);

	lf.lfHeight = GetFontHeight(nPointSize);

	return SetFont(&lf, bRedraw);
}

//=============================================================================
CJetGroupBox& CJetGroupBox::SetFont(CFont *pFont,BOOL bRedraw /*= TRUE*/)
{
	ASSERT(pFont);

	if (!pFont)
		pFont = GetSafeFont();

	if (pFont)
	{
		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));

		pFont->GetLogFont(&lf);

		SetFont(&lf, bRedraw);
	}

	return *this;
}


CJetGroupBox& CJetGroupBox::SetFont(LOGFONT *pLogFont,BOOL bRedraw /*= TRUE*/)
{
	TRACE(_T("in CJetGroupBox::SetFont =================\n"));
	ASSERT(pLogFont);

	if (pLogFont)
	{
		if (m_font.GetSafeHandle())
			m_font.DeleteObject();
		VERIFY(m_font.CreateFontIndirect(pLogFont));

		m_sizeText.cx = 0;		// force string width to be recalculated

		if (bRedraw)
			RedrawWindow();
	}

	return *this;
}

//=============================================================================
CJetGroupBox& CJetGroupBox::SetWindowText(LPCTSTR lpszText, BOOL bRedraw /*= TRUE*/)
{
	AFX_MANAGE_STATE(AFXMANAGESTATE());

	CString strText = _T("");

	m_sizeText.cx = 0;

	if (lpszText)
	{
		// is this a text string or an id?
		if (HIWORD(lpszText) == 0)
		{
			// id
			UINT nId = LOWORD((UINT)(UINT_PTR)lpszText);
			VERIFY(strText.LoadString(nId));
		}
		else
		{
			// string
			strText = lpszText;
		}
	}

	CStatic::SetWindowText(strText);

	if (bRedraw)
		RedrawWindow();

	return *this;
}

CJetGroupBox& CJetGroupBox::SetTextColor(COLORREF cr, BOOL bRedraw /*= TRUE*/)
{
	m_crText = cr;

	if (bRedraw)
		RedrawWindow();

	return *this;
}

CJetGroupBox& CJetGroupBox::SetBorderColor(COLORREF cr, BOOL bRedraw /*= TRUE*/)
{
	m_crBorder = cr;

	if (bRedraw)
		RedrawWindow();

	return *this;
}

CJetGroupBox& CJetGroupBox::SetBorderStyle(BORDER_STYLE eStyle,
	BOOL bRedraw /*= TRUE*/)
{
	TRACE(_T("in CJetGroupBox::SetBorderStyle: %d\n"), eStyle);

	m_eBorderStyle = eStyle;

	if (bRedraw)
		RedrawWindow();

	return *this;
}


BOOL CJetGroupBox::EnableWindow(BOOL bEnable /*= TRUE*/,
	BOOL bRecurseChildren /*= FALSE*/)
{
	BOOL b_Enable = CStatic::EnableWindow(bEnable);

	return b_Enable;
}

CJetGroupBox& CJetGroupBox::SetAlignment(ALIGNMENT eAlign,BOOL bRedraw /*= TRUE*/)
{
	m_eControlAlignment = eAlign;

	if (bRedraw)
		RedrawWindow();

	return *this;
}