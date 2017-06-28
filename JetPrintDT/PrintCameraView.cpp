// MainPRINTRightView.cpp : 实现文件
//

#include "stdafx.h"
#include "JetPrintDT.h"
#include "PrintCameraView.h"
#include "SingleTon/CameraSharedBuffer.h"
#include "Device/DeviceCamera.h"

// CPrintCameraView

IMPLEMENT_DYNCREATE(CPrintCameraView, CScrollView)

CPrintCameraView::CPrintCameraView()
{
	m_pShowImageThread = nullptr;
	m_bGrabImage = false;
	m_iScrollHeight = 0;
	m_iScrollWidth = 0;

	m_lpBmi = nullptr;
}

CPrintCameraView::~CPrintCameraView()
{
	GdiplusShutdown(m_gdiplusToken);
}


BEGIN_MESSAGE_MAP(CPrintCameraView, CScrollView)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPrintCameraView 绘图

void CPrintCameraView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO:  计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

}

void InitializeWatchOverlay(CDC & NewDC, int iImageWidth, int iImageHeight)
{

	// Attach the device context.
	int center_x = iImageWidth / 2;
	int center_y = iImageHeight / 2;

	NewDC.SetBkMode(TRANSPARENT);

	// Create a blue pen. 
	CPen m_pen, *m_penOld;
	m_pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	m_penOld = NewDC.SelectObject(&m_pen);

	// Draw a cross in the overlay buffer. 
	NewDC.MoveTo(0, iImageHeight / 2);
	NewDC.LineTo(iImageWidth, iImageHeight / 2);
	NewDC.MoveTo(iImageWidth / 2, 0);
	NewDC.LineTo(iImageWidth / 2, iImageHeight);

	for (int i = 1; i <= 30; i++)
	{
		//左侧
		if (0 == (i) % 5)
		{
			NewDC.MoveTo(center_x - 10 * i, center_y);
			NewDC.LineTo(center_x - 10 * i, center_y + 25);

			NewDC.MoveTo(center_x - 10 * i, center_y);
			NewDC.LineTo(center_x - 10 * i, center_y - 25);

			if (0 == (i % 2))
			{
				CString rulerLabel = _T("");
				rulerLabel.Format(_T("%.2f um"), -150.0*(i / 5));
				NewDC.SetTextColor(RGB(255, 0, 0));
				NewDC.TextOut(center_x - 10 * i, center_y - 25, rulerLabel);
			}
			else
			{
				CString rulerLabel = _T("");
				rulerLabel.Format(_T("%.2f um"), -150.0*(i / 5));
				NewDC.SetTextColor(RGB(255, 0, 0));
				NewDC.TextOut(center_x - 10 * i, center_y + 25, rulerLabel);
			}

		}
		else
		{
			NewDC.MoveTo(center_x - 10 * i, center_y);
			NewDC.LineTo(center_x - 10 * i, center_y + 15);

			NewDC.MoveTo(center_x - 10 * i, center_y);
			NewDC.LineTo(center_x - 10 * i, center_y - 15);
		}

		//右侧
		if (0 == (i) % 5)
		{
			NewDC.MoveTo(center_x + 10 * i, center_y);
			NewDC.LineTo(center_x + 10 * i, center_y + 25);

			NewDC.MoveTo(center_x + 10 * i, center_y);
			NewDC.LineTo(center_x + 10 * i, center_y - 25);

			if (0 == (i % 2))
			{
				CString rulerLabel = _T("");
				rulerLabel.Format(_T("%.2f um"), 150.0*(i / 5));
				NewDC.SetTextColor(RGB(255, 0, 0));
				NewDC.TextOut(center_x + 10 * i, center_y - 25, rulerLabel);
			}
			else
			{
				CString rulerLabel = _T("");
				rulerLabel.Format(_T("%.2f um"), 150.0*(i / 5));
				NewDC.SetTextColor(RGB(255, 0, 0));
				NewDC.TextOut(center_x + 10 * i, center_y + 25, rulerLabel);
			}

		}
		else
		{
			NewDC.MoveTo(center_x + 10 * i, center_y);
			NewDC.LineTo(center_x + 10 * i, center_y + 15);

			NewDC.MoveTo(center_x + 10 * i, center_y);
			NewDC.LineTo(center_x + 10 * i, center_y - 15);
		}
	}

	for (int i = 1; i <= 80; i++)
	{
		//上侧
		if (0 == (i) % 5)
		{
			NewDC.MoveTo(center_x, center_y - 4 * i);
			NewDC.LineTo(center_x + 25, center_y - 4 * i);

			NewDC.MoveTo(center_x, center_y - 4 * i);
			NewDC.LineTo(center_x - 25, center_y - 4 * i);

			//TextOut
			if (0 == i % 2)
			{
				CString rulerLabel = _T("");
				rulerLabel.Format(_T("%d um"), -60 * (i / 5));
				NewDC.SetTextColor(RGB(255, 0, 0));
				NewDC.TextOut(center_x - 55, center_y - 4 * i, rulerLabel);
			}
			else
			{
				CString rulerLabel = _T("");
				rulerLabel.Format(_T("%d um"), -60 * (i / 5));
				NewDC.SetTextColor(RGB(255, 0, 0));
				NewDC.TextOut(center_x + 25, center_y - 4 * i, rulerLabel);
			}


		}
		else
		{
			NewDC.MoveTo(center_x, center_y - 4 * i);
			NewDC.LineTo(center_x + 15, center_y - 4 * i);

			NewDC.MoveTo(center_x, center_y - 4 * i);
			NewDC.LineTo(center_x - 15, center_y - 4 * i);
		}

		//下侧
		if (0 == (i) % 5)
		{
			NewDC.MoveTo(center_x, center_y + 4 * i);
			NewDC.LineTo(center_x + 25, center_y + 4 * i);

			NewDC.MoveTo(center_x, center_y + 4 * i);
			NewDC.LineTo(center_x - 25, center_y + 4 * i);

			//TextOut
			if (0 == i % 2)
			{
				CString rulerLabel = _T("");
				rulerLabel.Format(_T("%d um"), 60 * (i / 5));
				NewDC.SetTextColor(RGB(255, 0, 0));
				NewDC.TextOut(center_x - 55, center_y + 4 * i, rulerLabel);
			}
			else
			{
				CString rulerLabel = _T("");
				rulerLabel.Format(_T("%d um"), 60 * (i / 5));
				NewDC.SetTextColor(RGB(255, 0, 0));
				NewDC.TextOut(center_x + 25, center_y + 4 * i, rulerLabel);
			}


		}
		else
		{
			NewDC.MoveTo(center_x, center_y + 4 * i);
			NewDC.LineTo(center_x + 15, center_y + 4 * i);

			NewDC.MoveTo(center_x, center_y + 4 * i);
			NewDC.LineTo(center_x - 15, center_y + 4 * i);
		}
	}

	// Deselect and destroy the blue pen.
	NewDC.SelectObject(m_penOld);
	m_pen.DeleteObject();

}

void CPrintCameraView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码

	CDeviceCamera *pCamera = CDeviceCamera::Instance();
	if (pCamera->m_bStartGrab){
		if (pCamera->GetWatchCameraStatus() || pCamera->GetLocateCameraStatus()){

			IplImage *pImage = m_ShowVideo.GetImage();
			if (nullptr == pImage)
				return;

			if (m_lpBmi)
				free(m_lpBmi);

			m_lpBmi = m_ShowVideo.CtreateMapInfo(pImage, 1);

			CSize sizeTotal;
			sizeTotal = CSize(pImage->width, pImage->height);

			SetScrollSizes(MM_TEXT, sizeTotal);
			char *pBits = pImage->imageData;

			CRect clientRect;
			GetClientRect(&clientRect);

			int destX = (clientRect.Width() - pImage->width) / 2;
			destX = destX > 0 ? destX : 0;
			int destY = (clientRect.Height() - pImage->height) / 2;
			destY = destY > 0 ? destY : 0;

			StretchDIBits(pDC->m_hDC,
				destX, destY, pImage->width, pImage->height,
				0, 0, pImage->width, pImage->height,
				pBits, m_lpBmi, DIB_RGB_COLORS, SRCCOPY);
		}
	}
	
}


// CPrintCameraView 诊断

#ifdef _DEBUG
void CPrintCameraView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CPrintCameraView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPrintCameraView 消息处理程序


UINT CPrintCameraView::ShowImage(LPVOID pData){
	CPrintCameraView* pCameraView = (CPrintCameraView*)pData;
	CCameraSharedBuffer<char> * pImageBuffer = CCameraSharedBuffer<char>::Instance();

	
	return 0;
}

void CPrintCameraView::StopGrabImage(){
	KillTimer(TIMER_UPDATE_IMAGE);
}

void CPrintCameraView::StartGrabImage(){
	SetTimer(TIMER_UPDATE_IMAGE,1,NULL);
}

void CPrintCameraView::ZoomOutImage(){
	float fRatio = m_ShowVideo.fRatio;
	fRatio += 0.2f;
	if (fRatio > 1.5)
		fRatio = 1.5;

	m_ShowVideo.fRatio = fRatio;
}

void CPrintCameraView::ShowOrign(){
	m_ShowVideo.fRatio = 1.0;
}

void CPrintCameraView::ZoomInImage(){
	float fRatio = m_ShowVideo.fRatio;
	fRatio -= 0.2f;
	if (fRatio < 0.1)
		fRatio = 0.1;

	m_ShowVideo.fRatio = fRatio;
}

void CPrintCameraView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == TIMER_UPDATE_IMAGE){

		if (m_bGrabImage){
			CCameraSharedBuffer<char> * pImageBuffer = CCameraSharedBuffer<char>::Instance();
			CCameraParam *pCameraParam = CCameraParam::Instance();
			if (pImageBuffer){
				pImageBuffer->ReadData((char*)(pCameraParam), BUFFER_SIZE);
				m_ShowVideo.SetImageHeight(pCameraParam->m_iCurrentCameraHeight);
				m_ShowVideo.SetImageWidth(pCameraParam->m_iCurrentCameraWidth);

				if (m_iScrollHeight != pCameraParam->m_iCurrentCameraHeight*m_ShowVideo.fRatio || \
					m_iScrollWidth != pCameraParam->m_iCurrentCameraWidth*m_ShowVideo.fRatio){

					m_iScrollHeight = pCameraParam->m_iCurrentCameraHeight*m_ShowVideo.fRatio;
					m_iScrollWidth = pCameraParam->m_iCurrentCameraWidth*m_ShowVideo.fRatio;
				}

				m_ShowVideo.InitializeImage(pCameraParam->m_szImageBuffer);

				Invalidate(false);

			}
		}
		
	}

	CScrollView::OnTimer(nIDEvent);
}


void CPrintCameraView::ShowScale(){
	m_ShowVideo.m_bShowScale = !m_ShowVideo.m_bShowScale;
}

BOOL CPrintCameraView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	// TODO:  在此添加专用代码和/或调用基类
	CPoint scrollPosition = GetScrollPosition();
	CPoint deviceScrollPosition = GetDeviceScrollPosition();

	m_ShowVideo.ScrollImage(scrollPosition.x, scrollPosition.y);
	return CScrollView::OnScrollBy(sizeScroll, bDoScroll);
}
