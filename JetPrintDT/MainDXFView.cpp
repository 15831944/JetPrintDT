// MainDXFScrollView.cpp : 实现文件
//

#include "stdafx.h"
#include "JetPrintDT.h"
#include "MainDXFView.h"


// CMainDXFView

IMPLEMENT_DYNCREATE(CMainDXFView, CScrollView)

CMainDXFView::CMainDXFView()
{

}

CMainDXFView::~CMainDXFView()
{
}


BEGIN_MESSAGE_MAP(CMainDXFView, CScrollView)
END_MESSAGE_MAP()


// CMainDXFView 绘图

void CMainDXFView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO:  计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CMainDXFView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码

	CRect rect;
	GetClientRect(&rect);
	CPoint point;
	point.x = (rect.Width() + 1) / 2;
	point.y = (rect.Height() + 1) / 2;

	rect.left = point.x - 100;
	rect.right = point.x + 100;
	rect.top = point.y - 100;
	rect.bottom = point.y + 100;
	pDC->Ellipse(rect);
}


// CMainDXFView 诊断

#ifdef _DEBUG
void CMainDXFView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CMainDXFView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMainDXFView 消息处理程序
