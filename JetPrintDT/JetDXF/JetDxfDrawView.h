#if !defined(AFX_DRAWINGVIEW_H__C7A18EB6_7630_491F_A48A_3C0137613E1A__INCLUDED_)
#define AFX_DRAWINGVIEW_H__C7A18EB6_7630_491F_A48A_3C0137613E1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrawingView.h : header file
//
#include "JetDxfView.h"
#include "../JetBmp/JetBmpAnalysis.h"
/////////////////////////////////////////////////////////////////////////////
// CDrawingView window

typedef enum _GRAPH_TYPE
{
	DXF_TYPE = 0,
	BMP_TYPE = 1,
	NULL_TYPE = 2,
}GRAPH_TYPE;

class CJetDxfDrawView : public CStatic
{
// Construction
	DECLARE_DYNAMIC(CJetDxfDrawView)
public:
	CJetDxfDrawView();
	virtual ~CJetDxfDrawView();
	void EnableDrawBorder(bool bEnabled);
	bool GetDrawBorderState();
	void SetXOffSet(double dbXOffSet);
	void SetYOffSet(double dbYOffset);
	double GetXOffSet() const;
	double GetYOffSet() const;
	void SetXLenth(double dbXLenth);
	void SetYLenth(double dbYLenth);
	double GetXLenth() const;
	double GetYLenth() const;
	void InitBaseBorderRect();
	void ReCaculateDrawborder();
	void CaculateBaseRect();
	void SetBorderType(BORDERTYPE bordertype);
public:
	CJetDxfView m_Drawing;
	CString m_CurrentFile;
	GRAPH_TYPE graphType;
	CJetBmpAnalysis *m_pJetBmpAnalysis;

	enum{DRAW_BMP_IMAGE = 10};

	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	void DrawBMPImage(CWnd *pWnd);
	//}}AFX_MSG

private:
	CPoint PrevMousePnt;
	bool m_Pan;
	bool m_bDrawBorder;
	REALRECT m_BorderRect;
	REALRECT m_DrawBorderRect;
	double m_dbXOffSet;
	double m_dbYOffSet;
	double m_dbXLenth;
	double m_dbYLenth;
	BORDERTYPE m_BorderType;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWINGVIEW_H__C7A18EB6_7630_491F_A48A_3C0137613E1A__INCLUDED_)
