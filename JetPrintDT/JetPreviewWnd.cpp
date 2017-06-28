// JetPanelWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "JetPreviewWnd.h"
#include "afxwin.h"
#include "math.h"
// CJetPreviewWnd

IMPLEMENT_DYNAMIC(CJetPreviewWnd, CWnd)

CJetPreviewWnd::CJetPreviewWnd()
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

	locateCameraPoint = CPoint(0, 0);
	jetPoint = CPoint(0, 0);
	printOrignPoint = CPoint(0, 0);

	n_TextHeight = 0;
	n_TextWidth = 0;

	m_startPos.x = 0;
	m_startPos.y = 0;

	m_endPos.x = 0;
	m_endPos.y = 0;
}

CJetPreviewWnd::~CJetPreviewWnd()
{
}


BEGIN_MESSAGE_MAP(CJetPreviewWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()



// CJetPreviewWnd ��Ϣ�������

CDC * CJetPreviewWnd::GetDC()
{
	return &m_backgroundDC;
}

void CJetPreviewWnd::SetBackColor(COLORREF newColor)
{
	m_backgroundColor = newColor;

	RefreshPanel();
}

void CJetPreviewWnd::DrawBackground(CDC *pDc,CRect panelRect)
{

	CBrush backBrush;
	backBrush.CreateSolidBrush(m_backgroundColor);

	pDc->SetBkColor(m_backgroundColor);
	pDc->FillRect(panelRect,&backBrush);

	pDc->DrawEdge(panelRect,EDGE_SUNKEN,BF_RECT);
}

void CJetPreviewWnd::DrawBackEdge(CDC *pDc,CRect panelRect)
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
void CJetPreviewWnd::RefreshPanel()
{
	//������δ�����ɹ�
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

	m_PanelSize.cx = 55;
	m_PanelSize.cy = 55;

	m_LogicalSize.cx = screenWidth;
	m_LogicalSize.cy = screenHeight;

	//����ӳ���ģʽ

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(m_PanelSize);
	//dc.SetViewportExt(clientRect.Width() / 2, clientRect.Height() / 2);
	dc.SetViewportExt(clientRect.Width()/2,-clientRect.Height()/2);

	//���ӿڵ�ԭ����Ϊ�ͻ���������
	dc.SetViewportOrg(clientRect.Width()/2 , clientRect.Height()/2);

	//���Ʊ���
	DrawBackground(&m_backgroundDC,clientRect);

	//���Ʊ���
	DrawBackEdge(&m_backgroundDC,clientRect);

	//��ʼ��ͼ
	drawCoordinate(&m_backgroundDC);

	Invalidate();
}


void CJetPreviewWnd::drawRect(CPoint centerPos,CClientDC *pDc)
{
	CRect clientRect;
	GetClientRect(&clientRect);
	//pDc->SetMapMode(MM_ANISOTROPIC);
	//pDc->SetWindowExt(m_PanelSize);
	///dc.SetViewportExt(clientRect.Width() / 2, clientRect.Height() / 2);
	//pDc->SetViewportExt(clientRect.Width()/2,-clientRect.Height()/2);

	//���ӿڵ�ԭ����Ϊ�ͻ���������
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

void CJetPreviewWnd::setIndicatorPoint(CPoint rectPos)
{
	indicatorPoint = rectPos;
}

void CJetPreviewWnd::updateIndicatorPoint(CPoint pos)
{
	setIndicatorPoint(pos);
	Invalidate();
}

void CJetPreviewWnd::updatelocatecameraPoint(CPoint pos){
	locateCameraPoint = pos;
	Invalidate();
}

void CJetPreviewWnd::drawlocatecamera(CPoint pos, CClientDC* pDc){
	CBrush *pBrush = nullptr;
	CBrush redBrush;
	redBrush.CreateSolidBrush(RGB(255, 255, 0));

	pBrush = pDc->SelectObject(&redBrush);

	CRect rect;
	rect.left = pos.x - RECT_WIDTH;
	rect.right = pos.x + RECT_WIDTH;
	rect.top = pos.y - RECT_WIDTH;
	rect.bottom = pos.y + RECT_WIDTH;
	pDc->Ellipse(rect);

	pDc->SelectObject(pBrush);
	redBrush.DeleteObject();
	pBrush->DeleteObject();
}

void CJetPreviewWnd::updatejetPoint(CPoint pos){
	jetPoint = pos;
	Invalidate();
}

void CJetPreviewWnd::drawjetPoint(CPoint pos, CClientDC *pDc){
	CBrush *pBrush = nullptr;
	CBrush redBrush;
	redBrush.CreateSolidBrush(RGB(255, 0, 0));

	pBrush = pDc->SelectObject(&redBrush);

	CRect rect;
	rect.left = pos.x - RECT_WIDTH;
	rect.right = pos.x + RECT_WIDTH;
	rect.top = pos.y - RECT_WIDTH;
	rect.bottom = pos.y + RECT_WIDTH;
	pDc->Ellipse(rect);

	pDc->SelectObject(pBrush);
	redBrush.DeleteObject();
	pBrush->DeleteObject();
}

void CJetPreviewWnd::updateprintorignpos(CPoint pos){
	printOrignPoint = pos;
	Invalidate();
}

void CJetPreviewWnd::drawprintorignpoint(CPoint pos, CClientDC* pDc){
	CPen redPen(PS_SOLID, 2, RGB(255, 0, 0));
	pDc->SelectObject(redPen);
	pDc->MoveTo(CPoint(pos.x - 4, pos.y));
	pDc->LineTo(CPoint(pos.x + 4, pos.y));

	pDc->MoveTo(CPoint(pos.x, pos.y - 4));
	pDc->LineTo(CPoint(pos.x, pos.y + 4));

	DeleteObject(redPen);
}

void CJetPreviewWnd::drawCoordinate(CDC *pDc)
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
	
	//���ƺ���������
	pDc->MoveTo(clientRect.left + 20 ,clientRect.top + clientRect.Height()/2);
	pDc->LineTo(clientRect.right - 20 , clientRect.top + clientRect.Height()/2);
	
	coordNatePen.DeleteObject();

    arrowBrush.CreateSolidBrush(arrowColor);
	pDc->SelectObject(&arrowBrush);
	//������ߵļ�ͷ
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

	//�����ұߵļ�ͷ
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
	//��������������
	pDc->MoveTo(clientRect.left + clientRect.Width()/2 ,clientRect.top + 20);
	pDc->LineTo(clientRect.left + clientRect.Width()/2 , clientRect.bottom - 20);
	DeleteObject(coordNatePen);

	pDc->SelectObject(&arrowBrush);
	//�����ϱߵļ�ͷ
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

	//�����±ߵļ�ͷ
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

	coordNatePen.DeleteObject();
}

bool CJetPreviewWnd::RegisterWindowClass()
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

bool CJetPreviewWnd::Create(CWnd *pParent,const RECT &rect,UINT nID,DWORD dwStyle /* = WS_CHILD|WS_VISIBLE */)
{
	dwStyle |= WS_CLIPCHILDREN;
    int Result = CWnd::Create(JETPANELWND, _T(""), dwStyle, rect, pParent, nID);

	bool b_result = Result > 0 ? true : false;

	if (b_result)
		RefreshPanel();



	return b_result;
}

int CJetPreviewWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	TEXTMETRIC tm;
	CDC *dc = CWnd::GetDC();
	dc->GetTextMetrics(&tm);
	n_TextHeight = tm.tmHeight + tm.tmExternalLeading;
	n_TextWidth = tm.tmAveCharWidth;


	ReleaseDC(dc);
	return 0;
}


void CJetPreviewWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CWnd::OnPaint()

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

	drawlocatecamera(locateCameraPoint, &clientDC);
	drawjetPoint(jetPoint, &clientDC);
	drawprintorignpoint(printOrignPoint, &clientDC);
}


void CJetPreviewWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������

	if (m_backgroundDC.GetSafeHdc() )
		m_backgroundDC.DeleteDC();

	CRect rect;
	GetClientRect(&rect);
	indicatorPoint.x = rect.Width()/2;
	indicatorPoint.y = rect.Height()/2;

	RefreshPanel();
}



void CJetPreviewWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	indicatorPoint = point;
	
	Invalidate();

	CWnd::OnLButtonDblClk(nFlags, point);
}


void CJetPreviewWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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

void CJetPreviewWnd::TextCoordinate(const CPoint& pt,const CString& text)
{
	CClientDC dc(this);
	dc.TextOutW(pt.x,pt.y,text);
}

BOOL CJetPreviewWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	//return CWnd::OnEraseBkgnd(pDC);

	return FALSE;
}


void CJetPreviewWnd::OnMouseLeave()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	//mousePosString = _T("");
	m_bMouseInWnd = false;

	Invalidate();

	CWnd::OnMouseLeave();
}


void CJetPreviewWnd::SetPanelSize(double &db_x,double &db_y)
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
void CJetPreviewWnd::DevicePointToLogicPoint(const float &fpt_x,const float &fpt_y,int& npt_x,int &npt_y)
{
	float fXpos = fpt_x;
	float fYpos = fpt_y;
	int xResult = DevicePointToLogicPointX(fXpos);
	int yResult = DevicePointToLogicPointY(fYpos);

	npt_x = xResult;
	npt_y = yResult;
}

int CJetPreviewWnd::DevicePointToLogicPointX(float &pt_x)
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

int CJetPreviewWnd::DevicePointToLogicPointY(float &pt_y)
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