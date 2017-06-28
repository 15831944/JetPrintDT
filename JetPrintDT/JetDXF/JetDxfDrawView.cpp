// DrawingView.cpp : implementation file
//
#include "JetDxfDrawView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawingView
IMPLEMENT_DYNAMIC(CJetDxfDrawView,CStatic)

CJetDxfDrawView::CJetDxfDrawView()
{
	m_Pan = false;
	m_CurrentFile = _T("");
	graphType = NULL_TYPE;
	m_pJetBmpAnalysis = NULL;
	m_bDrawBorder = false;
	m_BorderRect.left = 0.0;
	m_BorderRect.right = 0.0;
	m_BorderRect.top = 0.0;
	m_BorderRect.bottom = 0.0;
	m_DrawBorderRect.left = 0.0;
	m_DrawBorderRect.right = 0.0;
	m_DrawBorderRect.top = 0.0;
	m_DrawBorderRect.bottom = 0.0;

	m_dbXLenth = 0.0;
	m_dbYLenth = 0.0;
	m_dbXOffSet = 0.0;
	m_dbYOffSet = 0.0;
	m_BorderType = BT_RECT;
}

CJetDxfDrawView::~CJetDxfDrawView()
{
}


BEGIN_MESSAGE_MAP(CJetDxfDrawView, CStatic)
	//{{AFX_MSG_MAP(CJetDxfDrawView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawingView message handlers

void CJetDxfDrawView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	if(NULL_TYPE != graphType)
	{
		
		
		if(DXF_TYPE == graphType)
		{
			if(NULL != m_Drawing.m_pJetDxfDimView)
			{
				if( m_Drawing.m_pJetDxfDimView->m_pDrawing != NULL)
				{
					if(m_bDrawBorder)
					{
						InitBaseBorderRect();
						ReCaculateDrawborder();
						CaculateBaseRect();
						m_Drawing.drwPaint(dc.m_hDC);

						CPen borderPen(PS_SOLID, 2, RGB(90, 127, 208));
						dc.SelectObject(borderPen);
						if(BT_ELLIPSE == m_BorderType)
							dc.Ellipse(static_cast<int>(m_DrawBorderRect.left), static_cast<int>(m_DrawBorderRect.top), static_cast<int>(m_DrawBorderRect.right), static_cast<int>(m_DrawBorderRect.bottom));
						else
							dc.Rectangle(static_cast<int>(m_DrawBorderRect.left), static_cast<int>(m_DrawBorderRect.top), static_cast<int>(m_DrawBorderRect.right), static_cast<int>(m_DrawBorderRect.bottom));
						DeleteObject(borderPen);
					}
					else
					{
						m_Drawing.drwPaint(dc.m_hDC);
					}
					

				}
			}
			
		}

		if(BMP_TYPE == graphType)
		{
			if(NULL != m_pJetBmpAnalysis)
			{
				if(m_bDrawBorder)
				{
					m_pJetBmpAnalysis->DrawBMPImageWithBorder(this,this->GetDC());
				}
				else
				{
					 m_pJetBmpAnalysis->DrawBMPImage(this,this->GetDC());
				}
			   
			}
		}
		
		
	}
	else
	{
		CRect r;
		GetClientRect(r);
		dc.SelectObject(GetStockObject(BLACK_BRUSH));
		dc.Rectangle(r);
	}
}

void CJetDxfDrawView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	PrevMousePnt = point;
	m_Pan = true;
	CStatic::OnLButtonDown(nFlags, point);
}

void CJetDxfDrawView::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_Pan = false;
	CStatic::OnLButtonUp(nFlags, point);
}

void CJetDxfDrawView::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	PrevMousePnt = point;
	m_Pan = true;
	CStatic::OnMButtonDown(nFlags, point);
}

void CJetDxfDrawView::OnMButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_Pan = false;
	CStatic::OnMButtonUp(nFlags, point);
}

void CJetDxfDrawView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_Pan && GetDC()->PtVisible(point))
	{
		if(NULL != m_Drawing.m_pJetDxfDimView->m_pDrawing)
		{
			double OnePixel;
			VIEW drwview;
			m_Drawing.drwGetViewProperties(&drwview);
			OnePixel = 1/(drwview.PPU*drwview.ZoomLevel);

			drwview.ViewLeft = drwview.ViewLeft - OnePixel*(point.x - PrevMousePnt.x);
			drwview.ViewBottom = drwview.ViewBottom + OnePixel*(point.y-PrevMousePnt.y);
			m_Drawing.drwSetViewProperties(&drwview);
			RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

			PrevMousePnt = point;
		}
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CJetDxfDrawView::DrawBMPImage(CWnd *pWnd)
{
	if(NULL == pWnd || NULL == m_pJetBmpAnalysis)
	{
		return ;
	}

	CDC *pDc = pWnd->GetDC();
	CBrush blackBrush(RGB(0,0,0));
	pDc->SelectObject(&blackBrush);

	CRect rect;
	pWnd->GetClientRect(&rect);

	pDc->FillRect(&rect,&blackBrush);
	DeleteObject(blackBrush);

	CPen redPen(PS_SOLID,1,RGB(255,0,0));
	pDc->SelectObject(redPen);
	CPoint ImageRange = m_pJetBmpAnalysis->RetriveImageRange();

	pDc->SetMapMode(MM_ANISOTROPIC);
	int ZoomScale = 3;

	pDc->SetWindowExt((ImageRange.x*ZoomScale)*DRAW_BMP_IMAGE,-(ImageRange.y*ZoomScale)*DRAW_BMP_IMAGE);
	pDc->SetViewportExt(rect.Width(),rect.Height());
	pDc->SetViewportOrg(rect.Width()/2,rect.Height()/2);

	
}

bool CJetDxfDrawView::GetDrawBorderState()
{
	return m_bDrawBorder;
}

void CJetDxfDrawView::EnableDrawBorder(bool bEnabled)
{
	m_bDrawBorder = bEnabled;
}

void CJetDxfDrawView::SetXOffSet(double dbXOffSet)
{
	m_dbXOffSet = dbXOffSet;
}

void CJetDxfDrawView::SetYOffSet(double dbYOffset)
{
	m_dbYOffSet = dbYOffset;
}

double CJetDxfDrawView::GetXOffSet() const
{
	return m_dbXOffSet;
}

double CJetDxfDrawView::GetYOffSet() const
{
	return m_dbYOffSet;
}

void CJetDxfDrawView::SetXLenth(double dbXLenth)
{
	m_dbXLenth = dbXLenth;
}

void CJetDxfDrawView::SetYLenth(double dbYLenth)
{
	m_dbYLenth = dbYLenth;
}

double CJetDxfDrawView::GetXLenth() const
{
	return m_dbXLenth;
}

double CJetDxfDrawView::GetYLenth() const
{
	return m_dbYLenth;
}

void CJetDxfDrawView::InitBaseBorderRect()
{
	double left = 0.0;
	double right = 0.0;
	double top = 0.0;
	double bottom = 0.0;

	left = m_Drawing.borderRect.left;
	right = m_Drawing.borderRect.right;
	top = m_Drawing.borderRect.top;
	bottom = m_Drawing.borderRect.bottom;

	double xCenter = (double)(left+right)/2.0;
	double yCenter = (double)(top+bottom)/2.0;

	double xBorderCenter = xCenter - m_dbXOffSet;
	double yBorderCenter = yCenter - m_dbYOffSet;

	REALRECT borderRect;
	borderRect.left = xBorderCenter - m_dbXLenth/2.0;
	borderRect.right = xBorderCenter+ m_dbXLenth/2.0;
	borderRect.top = yBorderCenter + m_dbYLenth/2.0;
	borderRect.bottom = yBorderCenter-m_dbYLenth/2.0;

	m_BorderRect.left = borderRect.left;
	m_BorderRect.right = borderRect.right;
	m_BorderRect.top = borderRect.top;
	m_BorderRect.bottom = borderRect.bottom;
}

void CJetDxfDrawView::ReCaculateDrawborder()
{
	REALRECT rect;
	double			ZoomLevelX = 0.0;
	double          ZoomLevelY = 0.0;
	m_Drawing.drwGetDrawingBorder(&rect);


	REALRECT tempRect;
	tempRect.left = rect.left < m_BorderRect.left ? rect.left : m_BorderRect.left;
	tempRect.right = rect.right < m_BorderRect.right ? m_BorderRect.right : rect.right;
	tempRect.top = rect.top < m_BorderRect.top ? m_BorderRect.top : rect.top;
	tempRect.bottom = rect.bottom < m_BorderRect.bottom ? rect.bottom : m_BorderRect.bottom;

	ZoomLevelX = (m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowBottom - m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowTop - 20) / \
		((tempRect.top - tempRect.bottom)*m_Drawing.m_pJetDxfDimView->m_pDrawing->View.PPU);

	ZoomLevelY = (m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowRight - m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowLeft - 20) / \
		((tempRect.right - tempRect.left)*m_Drawing.m_pJetDxfDimView->m_pDrawing->View.PPU);

	m_Drawing.m_pJetDxfDimView->m_pDrawing->View.ZoomLevel = (abs(ZoomLevelX) < abs(ZoomLevelY) ? abs(ZoomLevelX) : abs(ZoomLevelY));

	m_Drawing.m_pJetDxfDimView->m_pDrawing->View.ViewBottom = tempRect.bottom - ((m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowBottom - \
		m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowTop) / (m_Drawing.m_pJetDxfDimView->m_pDrawing->View.ZoomLevel*\
		m_Drawing.m_pJetDxfDimView->m_pDrawing->View.PPU) - (tempRect.top - tempRect.bottom)) / 2;

	m_Drawing.m_pJetDxfDimView->m_pDrawing->View.ViewLeft = tempRect.left - ((m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowRight - m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowLeft) / \
		(m_Drawing.m_pJetDxfDimView->m_pDrawing->View.ZoomLevel*m_Drawing.m_pJetDxfDimView->m_pDrawing->View.PPU) - (tempRect.right - tempRect.left)) / 2;

	m_Drawing.m_pJetDxfDimView->m_pDrawing->View.ViewTop = m_Drawing.m_pJetDxfDimView->m_pDrawing->View.ViewBottom + \
		(m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowBottom - m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowTop) / \
		m_Drawing.m_pJetDxfDimView->m_pDrawing->View.ZoomLevel / m_Drawing.m_pJetDxfDimView->m_pDrawing->View.PPU;

	m_Drawing.m_pJetDxfDimView->m_pDrawing->View.ViewRight = m_Drawing.m_pJetDxfDimView->m_pDrawing->View.ViewLeft + (m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowRight - \
		m_Drawing.m_pJetDxfDimView->m_pDrawing->View.WindowLeft) / m_Drawing.m_pJetDxfDimView->m_pDrawing->View.ZoomLevel / m_Drawing.m_pJetDxfDimView->m_pDrawing->View.PPU;
}

void CJetDxfDrawView::CaculateBaseRect()
{
	double dbLeft = m_BorderRect.left;
	double dbRight = m_BorderRect.right;
	double dbTop = m_BorderRect.top;
	double dbBottom = m_BorderRect.bottom;

	dbLeft = m_Drawing.m_pJetDxfDimView->UnitToHorizontalPixel(dbLeft);
	dbRight = m_Drawing.m_pJetDxfDimView->UnitToHorizontalPixel(dbRight);
	dbTop = m_Drawing.m_pJetDxfDimView->UnitToVerticalPixel(dbTop);
	dbBottom = m_Drawing.m_pJetDxfDimView->UnitToVerticalPixel(dbBottom);

	m_DrawBorderRect.left = dbLeft;
	m_DrawBorderRect.right = dbRight;
	m_DrawBorderRect.top = dbTop;
	m_DrawBorderRect.bottom = dbBottom;
}

void CJetDxfDrawView::SetBorderType(BORDERTYPE bordertype)
{
	m_BorderType = bordertype;
}