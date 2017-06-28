#ifndef XGROUPBOX_H
#define XGROUPBOX_H

#include "../stdafx.h"

//=============================================================================
class CJetGroupBox : public CStatic
//=============================================================================
{
// Construction
public:
	CJetGroupBox();
	virtual ~CJetGroupBox();

// Attributes
public:
	
	enum BORDER_STYLE { flat = 0, etched };

	enum ALIGNMENT { left = 0, center, right };

	CJetGroupBox& SetAlignment(ALIGNMENT eAlign, BOOL bRedraw = TRUE);
	CJetGroupBox& SetBold(BOOL bBold, BOOL bRedraw = TRUE);
	CJetGroupBox& SetBorderColor(COLORREF crBorder, BOOL bRedraw = TRUE);
	CJetGroupBox& SetBorderStyle(BORDER_STYLE eStyle, BOOL bRedraw = TRUE);
	CJetGroupBox& SetFont(CFont *pFont, BOOL bRedraw = TRUE);
	CJetGroupBox& SetFont(LOGFONT * pLogFont, BOOL bRedraw = TRUE);
	CJetGroupBox& SetFont(LPCTSTR lpszFaceName, int nPointSize, BOOL bRedraw = TRUE);
	CJetGroupBox& SetTextColor(COLORREF cr, BOOL bRedraw = TRUE);
	CJetGroupBox& SetWindowText(LPCTSTR lpszText, BOOL bRedraw = TRUE);
	BOOL EnableWindow(BOOL bEnable = TRUE, BOOL bRecurseChildren = FALSE);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJetGroupBox)
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFont			m_font;					// font used for text
	COLORREF		m_crText;				// text color
	COLORREF		m_crBackground;			// background color
	COLORREF		m_crBorder;				// user-specified border color
	
	CRect			m_rect;
	CSize			m_sizeText;				// size of text from GetTextExtent()
	
	BORDER_STYLE	m_eBorderStyle;			// border line style - flat or etched;
											// applies only if no border color
											// and no theme
	int				m_nYOffset;				// y offset of frame (text extends above frame)
	ALIGNMENT		m_eControlAlignment;
	int             m_filletRadius;


	void		DrawBorder(CDC *pDC, CRect& rect);
	void		DrawEtchedRectangle(CDC *pDC, CRect& rect, COLORREF cr);
	void		DrawItem(CDC *pDC, CRect& rect);
	int			DrawText(CDC *pDC, CRect& rect);
	void		EraseBorder(CDC *pDC, CRect& rect, COLORREF crBackground);
	int			GetFontHeight(int nPointSize);
	CFont *		GetSafeFont();

	// Generated message map functions
	//{{AFX_MSG(CJetGroupBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XGROUPBOX_H
