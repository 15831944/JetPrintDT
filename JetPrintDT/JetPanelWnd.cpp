// JetPanelWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "JetPanelWnd.h"
#include "afxwin.h"
#include "math.h"
// CJetPanelWnd

IMPLEMENT_DYNAMIC(CJetPanelWnd, CWnd)

CJetPanelWnd::CJetPanelWnd()
{
	RegisterWindowClass();
	//m_backgroundColor = GetSysColor(COLOR_BTNFACE);
	m_backgroundColor = RGB(72,118,255);
	m_PanelSize = CSize(0,0);
	m_LogicalSize = CSize(0,0);


	m_bMemDcCreated = false;
	m_bMouseInWnd = false;
	mousePosString = _T("");
	indicatorPoint = CPoint(0,0);
	mousePosInDevice = CPoint(0,0);
	mousePosInLogic = CPoint(0,0);

	n_TextHeight = 0;
	n_TextWidth = 0;

	m_startPos.x = 0;
	m_startPos.y = 0;

	m_endPos.x = 0;
	m_endPos.y = 0;
}

CJetPanelWnd::~CJetPanelWnd()
{
}


BEGIN_MESSAGE_MAP(CJetPanelWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()



// CJetPanelWnd 消息处理程序

CDC * CJetPanelWnd::GetDC()
{
	return &m_backgroundDC;
}

void CJetPanelWnd::SetBackColor(COLORREF newColor)
{
	m_backgroundColor = newColor;

	RefreshPanel();
}

void CJetPanelWnd::DrawBackground(CDC *pDc,CRect panelRect)
{

	CBrush backBrush;
	backBrush.CreateSolidBrush(m_backgroundColor);

	pDc->SetBkColor(m_backgroundColor);
	pDc->FillRect(panelRect,&backBrush);

	pDc->DrawEdge(panelRect,EDGE_SUNKEN,BF_RECT);
}

void CJetPanelWnd::DrawBackEdge(CDC *pDc,CRect panelRect)
{
	CPen SolidPen(PS_SOLID,0,m_borderColor);
	CPen* pOldPen = pDc->SelectObject(&SolidPen);

	pDc->MoveTo(panelRect.left,panelRect.top);
	pDc->LineTo(panelRect.right,panelRect.top);
	pDc->LineTo(panelRect.right,panelRect.bottom);
	pDc->LineTo(panelRect.left,panelRect.bottom);
	pDc->LineTo(panelRect.left,panelRect.top);

	pDc->SelectObject(pOldPen);
	DeleteObject(SolidPen);
}

//Refresh The PrintPanel;
void CJetPanelWnd::RefreshPanel()
{
	//窗口尚未创建成功
	if(!GetSafeHwnd())
		return ;

	int screenHeight = 0;
	int screenWidth = 0;
	int screenHeightPixs = 0;
	int screenWidthPixs = 0;
	double screenHeightPixsPerMM = 0.0;
	double screenWidthPixsPerMM = 0.0;

	CClientDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);


	if(!m_backgroundDC.GetSafeHdc())
	{
		CBitmap memBitmap;
		m_backgroundDC.CreateCompatibleDC(&dc) ;
		memBitmap.CreateCompatibleBitmap(&dc, clientRect.Width(), clientRect.Height()) ;
		m_backgroundDC.SelectObject(&memBitmap) ;
	}

	screenHeight = dc.GetDeviceCaps(VERTSIZE);
	screenWidth = dc.GetDeviceCaps(HORZSIZE);

	m_PanelSize.cx = 70;
	m_PanelSize.cy = 58;

	m_LogicalSize.cx = screenWidth;
	m_LogicalSize.cy = screenHeight;

	//设置映射的模式

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(m_PanelSize);
	//dc.SetViewportExt(clientRect.Width() / 2, clientRect.Height() / 2);
	dc.SetViewportExt(clientRect.Width()/2,-clientRect.Height()/2);

	//将视口的原点设为客户区的正中
	dc.SetViewportOrg(clientRect.Width()/2 , clientRect.Height()/2);

	//绘制背景
	DrawBackground(&m_backgroundDC,clientRect);

	//绘制边线
	DrawBackEdge(&m_backgroundDC,clientRect);

	//开始绘图
	drawCoordinate(&m_backgroundDC);

	Invalidate();
}


void CJetPanelWnd::drawRect(CPoint centerPos,CClientDC *pDc)
{
	CRect clientRect;
	GetClientRect(&clientRect);
	//pDc->SetMapMode(MM_ANISOTROPIC);
	//pDc->SetWindowExt(m_PanelSize);
	///dc.SetViewportExt(clientRect.Width() / 2, clientRect.Height() / 2);
	//pDc->SetViewportExt(clientRect.Width()/2,-clientRect.Height()/2);

	//将视口的原点设为客户区的正中
	//pDc->SetViewportOrg(clientRect.Width()/2 , clientRect.Height()/2);

	/*int x_Center = clientRect.Width()/2;
	int y_Center = clientRect.Height()/2;

	int nGraphicsMode = pDc->SetGraphicsMode(GM_ADVANCED);
	XFORM xform;
	float rotate_angle = 270.0f/180.0f*3.1415f;

	xform.eM11 = (float)cos(rotate_angle);
	xform.eM12 = (float)sin(rotate_angle);
	xform.eM21 = (float)(-sin(rotate_angle));
	xform.eM22 = (float)(cos(rotate_angle));

	xform.eDx = (float)(x_Center - cos(rotate_angle)*x_Center + sin(rotate_angle)*y_Center);
	xform.eDy = (float)(y_Center - cos(rotate_angle)*y_Center - sin(rotate_angle)*x_Center);

	pDc->SetWorldTransform(&xform);*/

	if(indicatorRect.IsRectEmpty())
	{
		indicatorRect.left = centerPos.x -RECT_WIDTH;
		indicatorRect.right = centerPos.x + RECT_WIDTH;
		indicatorRect.top = centerPos.y - RECT_WIDTH;
		indicatorRect.bottom = centerPos.y + RECT_WIDTH;
	}
	else
	{
		indicatorRect.SetRectEmpty();

		indicatorRect.left = centerPos.x - RECT_WIDTH;
		indicatorRect.right = centerPos.x + RECT_WIDTH;
		indicatorRect.top = centerPos.y - RECT_WIDTH;
		indicatorRect.bottom = centerPos.y + RECT_WIDTH;
	}
	
	CBrush rectBrush(RGB(0,0,0));

	pDc->FillRect(&indicatorRect,&rectBrush);

	/*xform.eM11 = (float)1.0;
	xform.eM12 = (float)0;
	xform.eM21 = (float)0;
	xform.eM22 = (float)1.0;

	xform.eDx = (float)0;
	xform.eDy = (float)0;

	pDc->SetWorldTransform(&xform);
	pDc->SetGraphicsMode(nGraphicsMode);*/
}

void CJetPanelWnd::setIndicatorPoint(CPoint rectPos)
{
	indicatorPoint = rectPos;
}

void CJetPanelWnd::updateIndicatorPoint(CPoint pos)
{
	setIndicatorPoint(pos);
	Invalidate();
}

void CJetPanelWnd::drawCoordinate(CDC *pDc)
{
	CPen coordNatePen(PS_SOLID,2,RGB(255,0,0));
	COLORREF arrowColor = RGB(255,0,0);
	CBrush arrowBrush;
	CPoint arrowPoint[3];
	CPoint leftArrowPoint,rightArrowPoint,topArrowPoint,bottomArrowPoint;
	CRgn arrowRgn;
	CRect clientRect;
	GetClientRect(&clientRect);

	pDc->SelectObject(&coordNatePen);
	
	//绘制横向坐标轴
	pDc->MoveTo(clientRect.left + 20 ,clientRect.top + clientRect.Height()/2);
	pDc->LineTo(clientRect.right - 20 , clientRect.top + clientRect.Height()/2);
	
	coordNatePen.DeleteObject();

    arrowBrush.CreateSolidBrush(arrowColor);
	pDc->SelectObject(&arrowBrush);
	//绘制左边的箭头
	arrowPoint[0].x = clientRect.left + 20;
	arrowPoint[0].y = clientRect.top + clientRect.Height()/2 - 4;
	arrowPoint[1].x = clientRect.left + 20;
	arrowPoint[1].y = clientRect.top + clientRect.Height()/2 + 4;
	arrowPoint[2].x = clientRect.left + 20 -8;
	arrowPoint[2].y = clientRect.top + clientRect.Height()/2 ;

	leftArrowPoint = arrowPoint[2];

	arrowRgn.CreatePolygonRgn(arrowPoint,3,ALTERNATE);

	pDc->FillRgn(&arrowRgn,&arrowBrush);
	pDc->MoveTo(arrowPoint[0]);
	pDc->LineTo(arrowPoint[1]);
	pDc->MoveTo(arrowPoint[1]);
	pDc->LineTo(arrowPoint[2]);
	pDc->MoveTo(arrowPoint[2]);
	pDc->LineTo(arrowPoint[0]);

	arrowRgn.DeleteObject();

	//绘制右边的箭头
	arrowPoint[0].x = clientRect.right - 20;
	arrowPoint[0].y = clientRect.top + clientRect.Height()/2 - 4;
	arrowPoint[1].x = clientRect.right - 20;
	arrowPoint[1].y = clientRect.top + clientRect.Height()/2 + 4;
	arrowPoint[2].x = clientRect.right - 20  + 8;
	arrowPoint[2].y = clientRect.top + clientRect.Height()/2 ;

	rightArrowPoint = arrowPoint[2];

	arrowRgn.CreatePolygonRgn(arrowPoint,3,ALTERNATE);
	pDc->FillRgn(&arrowRgn,&arrowBrush);
	pDc->MoveTo(arrowPoint[0]);
	pDc->LineTo(arrowPoint[1]);
	pDc->MoveTo(arrowPoint[1]);
	pDc->LineTo(arrowPoint[2]);
	pDc->MoveTo(arrowPoint[2]);
	pDc->LineTo(arrowPoint[0]);

	arrowRgn.DeleteObject();

	DeleteObject(arrowBrush);

	pDc->SelectObject(&coordNatePen);
	//绘制竖向坐标轴
	pDc->MoveTo(clientRect.left + clientRect.Width()/2 ,clientRect.top + 20);
	pDc->LineTo(clientRect.left + clientRect.Width()/2 , clientRect.bottom - 20);
	DeleteObject(coordNatePen);

	pDc->SelectObject(&arrowBrush);
	//绘制上边的箭头
	arrowPoint[0].x = clientRect.left + clientRect.Width()/2 -4;
	arrowPoint[0].y = clientRect.top +  20;
	arrowPoint[1].x = clientRect.left + clientRect.Width()/2 + 4;
	arrowPoint[1].y = clientRect.top + 20;
	arrowPoint[2].x = clientRect.left + clientRect.Width()/2;
	arrowPoint[2].y = clientRect.top + 20 - 8 ;

	topArrowPoint = arrowPoint[2];

	arrowRgn.CreatePolygonRgn(arrowPoint,3,ALTERNATE);
	pDc->FillRgn(&arrowRgn,&arrowBrush);
	pDc->MoveTo(arrowPoint[0]);
	pDc->LineTo(arrowPoint[1]);
	pDc->MoveTo(arrowPoint[1]);
	pDc->LineTo(arrowPoint[2]);
	pDc->MoveTo(arrowPoint[2]);
	pDc->LineTo(arrowPoint[0]);

	arrowRgn.DeleteObject();

	//绘制下边的箭头
	arrowPoint[0].x = clientRect.left + clientRect.Width()/2 -4;
	arrowPoint[0].y = clientRect.bottom -  20;
	arrowPoint[1].x = clientRect.left + clientRect.Width()/2 + 4;
	arrowPoint[1].y = clientRect.bottom - 20;
	arrowPoint[2].x = clientRect.left + clientRect.Width()/2;
	arrowPoint[2].y = clientRect.bottom - 20 + 8 ;

	bottomArrowPoint = arrowPoint[2];

	arrowRgn.CreatePolygonRgn(arrowPoint,3,WINDING);
	
	pDc->FillRgn(&arrowRgn,&arrowBrush);
	pDc->MoveTo(arrowPoint[0]);
	pDc->LineTo(arrowPoint[1]);
	pDc->MoveTo(arrowPoint[1]);
	pDc->LineTo(arrowPoint[2]);
	pDc->MoveTo(arrowPoint[2]);
	pDc->LineTo(arrowPoint[0]);

	arrowRgn.DeleteObject();

	DeleteObject(arrowBrush);

	pDc->SetBkMode(TRANSPARENT);
	pDc->SetTextColor(RGB(255,239,214));

	pDc->TextOutW(leftArrowPoint.x,leftArrowPoint.y,_T("向左"));
	pDc->TextOutW(rightArrowPoint.x - 32,rightArrowPoint.y,_T("向右"));
	pDc->TextOutW(topArrowPoint.x,topArrowPoint.y,_T("向后"));
	pDc->TextOutW(bottomArrowPoint.x ,bottomArrowPoint.y - 10 ,_T("向前"));
	

	coordNatePen.DeleteObject();
}

bool CJetPanelWnd::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, JETPANELWND, &wndcls)))
	{
		memset(&wndcls, 0, sizeof(WNDCLASS));   

		wndcls.hInstance		= hInst;
		wndcls.lpfnWndProc		= ::DefWindowProc;
		wndcls.hCursor			= NULL; //LoadCursor(NULL, IDC_ARROW);
		wndcls.hIcon			= 0;
		wndcls.lpszMenuName		= NULL;
		wndcls.hbrBackground	= (HBRUSH) ::GetStockObject(WHITE_BRUSH);
		wndcls.style			= CS_DBLCLKS; 
		wndcls.cbClsExtra		= 0;
		wndcls.cbWndExtra		= 0;
		wndcls.lpszClassName    = JETPANELWND;

		if (!RegisterClass(&wndcls))
		{
			//  AfxThrowResourceException();
			return false;
		}
	}

	return true;
}

bool CJetPanelWnd::Create(CWnd *pParent,const RECT &rect,UINT nID,DWORD dwStyle /* = WS_CHILD|WS_VISIBLE */)
{
	dwStyle |= WS_CLIPCHILDREN;
    int Result = CWnd::Create(JETPANELWND, _T(""), dwStyle, rect, pParent, nID);

	bool b_result = Result > 0 ? true : false;

	if (b_result)
		RefreshPanel();



	return b_result;
}

int CJetPanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	TEXTMETRIC tm;
	CDC *dc = CWnd::GetDC();
	dc->GetTextMetrics(&tm);
	n_TextHeight = tm.tmHeight + tm.tmExternalLeading;
	n_TextWidth = tm.tmAveCharWidth;


	ReleaseDC(dc);
	return 0;
}


void CJetPanelWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()

	if(!m_bMemDcCreated)
	{
		RefreshPanel();
		m_bMemDcCreated = true;
	}

	CRect clientRect;
	GetClientRect(&clientRect);

	dc.BitBlt(0,0,clientRect.Width(),clientRect.Height(),&m_backgroundDC,0,0,SRCCOPY);

	if(m_bMouseInWnd)
		TextCoordinate(mousePosInLogic,mousePosString);

	CClientDC clientDC(this);
	drawRect(indicatorPoint,&clientDC);

}


void CJetPanelWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	if (m_backgroundDC.GetSafeHdc() )
		m_backgroundDC.DeleteDC();

	CRect rect;
	GetClientRect(&rect);
	indicatorPoint.x = rect.Width()/2;
	indicatorPoint.y = rect.Height()/2;

	RefreshPanel();
}



void CJetPanelWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	indicatorPoint = point;
	
	Invalidate();

	CWnd::OnLButtonDblClk(nFlags, point);
}


void CJetPanelWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = this->m_hWnd;

	if(TrackMouseEvent(&tme))
	{
		m_bMouseInWnd = true;
	}

	CRect clientRect;
	GetClientRect(&clientRect);

	double pos_x = static_cast<double>(point.x);
	double pos_y = static_cast<double>(point.y);

	pos_x = pos_x - clientRect.Width()*1.0 / 2.0;
	pos_y = pos_y - clientRect.Height()*1.0 / 2.0;

	pos_x = pos_x * (m_PanelSize.cx*1.0) / (clientRect.Width()*1.0/2.0);
	pos_y = pos_y * (m_PanelSize.cy*1.0) / (clientRect.Height()*1.0/2.0*1.0);

	mousePosInDevice.x = static_cast<int>(pos_x);
	mousePosInDevice.y = static_cast<int>(pos_y);

	mousePosString.Format(_T("(%.2f,%.2f)"),pos_x, -pos_y);

	int n_CharCount = mousePosString.GetLength();

	if(point.x > clientRect.right - n_CharCount* n_TextWidth)
		point.x = clientRect.right - n_CharCount* n_TextWidth + 20;

	if(point.y > clientRect.bottom - n_TextHeight)
		point.y = clientRect.bottom - n_TextHeight;

	mousePosInLogic = point;
	
	//UpdateWindow();

	Invalidate();

	CWnd::OnMouseMove(nFlags, point);
}

void CJetPanelWnd::TextCoordinate(const CPoint& pt,const CString& text)
{
	CClientDC dc(this);
	dc.TextOutW(pt.x,pt.y,text);
}

BOOL CJetPanelWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//return CWnd::OnEraseBkgnd(pDC);

	return FALSE;
}


void CJetPanelWnd::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	//mousePosString = _T("");
	m_bMouseInWnd = false;

	Invalidate();

	CWnd::OnMouseLeave();
}


void CJetPanelWnd::SetPanelSize(double &db_x,double &db_y)
{
	m_PanelSize.cx = (long)db_x;
	m_PanelSize.cy = (long)db_y;
}

/************************************************************/
/*Input:    pt_x        point.x                             */
/*          pt_y        point.y                             */
/*Output:   pt_x        point.x                             */
/*          pt_y        point.y                             */
/************************************************************/
void CJetPanelWnd::DevicePointToLogicPoint(const float &fpt_x,const float &fpt_y,int& npt_x,int &npt_y)
{
	float fXpos = fpt_x;
	float fYpos = fpt_y;
	int xResult = DevicePointToLogicPointX(fXpos);
	int yResult = DevicePointToLogicPointY(fYpos);

	npt_x = xResult;
	npt_y = yResult;
}

int CJetPanelWnd::DevicePointToLogicPointX(float &pt_x)
{
	float fXPos = pt_x;
	int nResult = 0;
	CRect clientRect;
	GetClientRect(&clientRect);
	int nWidth = clientRect.Width();
	int nHeight = clientRect.Height();

	m_startPos.x = nWidth/2;
	m_startPos.y = nHeight/2;

	int n_xminValue = 0;
	int n_xmaxValue = 0;

	int nOffset = 0;
	
	n_xminValue = 0;
	n_xmaxValue = m_PanelSize.cx;

	m_endPos.x = nWidth;
	m_endPos.y = nHeight/2;

	nOffset = (int)floor((fXPos - n_xminValue)*(m_endPos.x - m_startPos.x)/(n_xmaxValue - n_xminValue));

	/*if(pt_x >= 0.0)
	{
	nResult = m_startPos.x + nOffset;
	}
	else
	{
	nResult = m_startPos.x - nOffset;
	}*/
	
	nResult = m_startPos.x + nOffset;

	return nResult;

}

int CJetPanelWnd::DevicePointToLogicPointY(float &pt_y)
{
	float fYPos = pt_y;
	int nResult = 0;
	CRect clientRect;
	GetClientRect(&clientRect);
	int nWidth = clientRect.Width();
	int nHeight = clientRect.Height();

	m_startPos.x = nWidth/2;
	m_startPos.y = nHeight/2;

	int n_yminValue = 0;
	int n_ymaxValue = 0;

	int nOffset = 0;

	n_yminValue = 0;
	n_ymaxValue = m_PanelSize.cy;

	m_endPos.x = nWidth/2;
	m_endPos.y = nHeight;

	nOffset = (int)floor((fYPos - n_yminValue)*(m_endPos.y - m_startPos.y)/(n_ymaxValue - n_yminValue));

	/*if(pt_y >= 0.0)
	{
	nResult = m_startPos.y + nOffset;
	}
	else
	{
	nResult = m_startPos.y - nOffset;
	}*/

	nResult = m_startPos.y - nOffset;
	return nResult;
}