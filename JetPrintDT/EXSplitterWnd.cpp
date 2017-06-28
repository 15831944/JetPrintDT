

#include "stdafx.h"
#include "EXSplitterWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CExSplitterWnd, CSplitterWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


CExSplitterWnd::CExSplitterWnd(int nLevel/* = 0*/):
m_sRegKey(_T("SplitterWnd")) , m_nLevel(nLevel)
{
	m_nPaneSize[0]    = 100;
	m_nPaneSize[1]    = 100;
	m_nPaneMinSize[0] = 10;
	m_nPaneMinSize[1] = 10;
	m_nCurrentView[0] = 0;
	m_nCurrentView[1] = 0;
	m_iCurrentActiveWnd = 0;
}

CExSplitterWnd::~CExSplitterWnd()
{
   //SaveToRegistry();
}


void CExSplitterWnd::SetInitialStatus()
{
	//int c,r;
   CString sSubKey;
   sSubKey.Format(_T("Panes_%d"),m_nLevel);
   CString s;
	s = AfxGetApp()->GetProfileString(m_sRegKey,sSubKey,_T("-1 150 -1 150"));
	   //sscanf(s,_T("%d %d"),&m_nPaneSize[0],&m_nPaneSize[1]);
   
  
	SetColumnInfo(0,m_nPaneSize[0],m_nPaneMinSize[0]);
	SetColumnInfo(1,m_nPaneSize[1],m_nPaneMinSize[1]);
	 RecalcLayout();
}



void CExSplitterWnd::SaveToRegistry()
{
   CString s;
   CString sSubKey;
   sSubKey.Format(_T("Panes_%d"),m_nLevel);

   RememberSize();
   if (m_nPaneSize[0] == -1) m_nPaneSize[0] = 100;
   if (m_nPaneSize[1] == -1) m_nPaneSize[1] = 100;

   s.Format(_T("%d %d"),m_nPaneSize[0],m_nPaneSize[1]);
   AfxGetApp()->WriteProfileString(m_sRegKey,sSubKey,s);
}


void CExSplitterWnd::RememberSize()
{
  
	GetColumnInfo(0,m_nPaneSize[0],m_nPaneMinSize[0]);
	GetColumnInfo(1,m_nPaneSize[1],m_nPaneMinSize[1]);
}


bool CExSplitterWnd::HideView(int nRow, int nCol)
{
	if (this->m_hWnd){
		CWnd* pWnd = GetPane(nRow, nCol);
		if (!pWnd) 
			return(false);
		pWnd->SetDlgCtrlID(0);
		pWnd->ShowWindow(SW_HIDE);
		return(true);
	}
	
	return false;
}


bool CExSplitterWnd::ShowView(int nRow, int nCol, CWnd* pWnd)
{
	pWnd->SetDlgCtrlID(IdFromRowCol(nRow, nCol));
	pWnd->ShowWindow(SW_SHOW);
	return(true);
}

int CExSplitterWnd::AddView(int nSide, CRuntimeClass * pViewClass, CCreateContext* pContext)
{
	int nRow, nCol;
	SideToRowCol(nSide,&nRow,&nCol);

   // hide the current view of the pane if there is a view attached already
	if (GetDlgItem(IdFromRowCol(nRow, nCol))) {	
		HideView(nRow, nCol);
	}

	// create the new view, if fail, set the previous view current 
	if (CreateView(nRow, nCol, pViewClass, CSize(100,100), pContext) == 0) 
		return -1;

	// get and store the new view
	CWnd* pWnd = GetPane(nRow, nCol);
	m_views[nSide].push_back(pWnd);
	m_nCurrentView[nSide] = m_views[nSide].size() - 1;

	ShowView(nRow, nCol,pWnd);

	RedrawWindow();
	return(m_nCurrentView[nSide]);

}


void CExSplitterWnd::SwitchToView(int nSide, int nViewIX /* = -1 */)
{
	// if the View is -1 then just use the next view...
	if (nViewIX == -1) {
		nViewIX = m_nCurrentView[nSide] + 1;
		if (nViewIX > (int)m_views[nSide].size()||nViewIX == (int)m_views[nSide].size()) nViewIX = 0; // rollover to first view
	}
	
	CWnd* pWnd = m_views[nSide][nViewIX];

	int nRow, nCol;
	CWnd* pTempWnd = nullptr;
	for (int i = 0; i < (int)m_views[nSide].size(); i++){
		pTempWnd = m_views[nSide][i];
		pTempWnd->ShowWindow(SW_HIDE);
	}
	SideToRowCol(nSide,&nRow,&nCol);
	HideView(nRow, nCol);
	ShowView(nRow, nCol, pWnd);

	m_nCurrentView[nSide] = nViewIX;
	RecalcLayout();
	RedrawWindow();
}



void CExSplitterWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CWnd::OnMouseMove(nFlags, point);
}


void CExSplitterWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CWnd::OnLButtonDown(nFlags, point);
}


void CExSplitterWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CWnd::OnLButtonUp(nFlags, point);
}
