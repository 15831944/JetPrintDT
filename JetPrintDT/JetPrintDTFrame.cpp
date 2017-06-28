

#include "stdafx.h"
#include "JetPrintDTFrame.h"
#include "PrintCameraView.h"
#include "PrintParamSetView.h"
#include "PrintMotionControlView.h"
#include "PrintFileOperateView.h"
#include "PrintPrintControlView.h"

CJetPrintDTSplitInfo::CJetPrintDTSplitInfo()
{
	m_maxRow = m_maxCol = 0;
}

CJetPrintDTSplitInfo &CJetPrintDTSplitInfo::operator=(const CJetPrintDTSplitInfo &SplitInfo)
{
	// function only for CMap
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// CJetPrintDTFrame

IMPLEMENT_DYNCREATE(CJetPrintDTFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CJetPrintDTFrame, CFrameWndEx)
	//{{AFX_MSG_MAP(CJetPrintDTFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJetPrintDTFrame construction/destruction

CJetPrintDTFrame::CJetPrintDTFrame()
{
}

CJetPrintDTFrame::~CJetPrintDTFrame()
{
}

BOOL CJetPrintDTFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	CView* pView;
	CJetPrintDTApp* pApp = (CJetPrintDTApp*)AfxGetApp();
	ASSERT_KINDOF(CJetPrintDTApp, pApp);

	// hold informations from DocTemplates to create splitter
	POSITION TmpPos = pApp->GetFirstDocTemplatePosition();
	while (TmpPos)
	{
		CJetPrintDTDocTemplate* pTemplate = (CJetPrintDTDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
		ASSERT_KINDOF(CJetPrintDTDocTemplate, pTemplate);
		if (pTemplate->m_nSplitterID != -1)
		{
			// find or create the splitter ID in the splitter map
			
			int nID = pTemplate->m_nSplitterID;
			m_SplitInfo[nID].m_maxRow = max(m_SplitInfo[nID].m_maxRow, pTemplate->m_nRow + 1);
			m_SplitInfo[nID].m_maxCol = max(m_SplitInfo[nID].m_maxCol, pTemplate->m_nCol + 1);
		}
	}

	if (!m_SplitInfo.IsEmpty()){
		// create the splitter and the views in the splitter
		POSITION MapPos = m_SplitInfo.GetStartPosition();
		while (MapPos)
		{
			int nID = 0;
			CJetPrintDTSplitInfo SplitInfo;
			m_SplitInfo.GetNextAssoc(MapPos, nID, SplitInfo);
			if (m_SplitInfo[nID].m_maxRow && m_SplitInfo[nID].m_maxCol)
			{
				CRect rect; GetClientRect(&rect);
				int cx = rect.Width(); int cy = rect.Height();

				// normaly always ToolBar & StatusBar
				cy = cy - 56;
				if (cy < 56)
					cy = 0;
				//Outlook Bar
				if (cx < 0)
					cx = 0;

				if (m_SplitInfo[nID].m_maxCol)
					cx /= m_SplitInfo[nID].m_maxCol;
				if (m_SplitInfo[nID].m_maxRow)
					cy /= m_SplitInfo[nID].m_maxRow;
				CSize sizeDefault(cx, cy);

				m_SplitInfo[nID].m_wndSplitter.CreateStatic(this, m_SplitInfo[nID].m_maxRow, m_SplitInfo[nID].m_maxCol);
				// disable view ID to use for next splitter
				m_SplitInfo[nID].m_wndSplitter.SetDlgCtrlID(0);

				TmpPos = pApp->GetFirstDocTemplatePosition();
				while (TmpPos)
				{
					CJetPrintDTDocTemplate* pTemplate = (CJetPrintDTDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
					if (pTemplate->m_nSplitterID == nID)
					{
						// create view
						int nRow = pTemplate->m_nRow, nCol = pTemplate->m_nCol;
						pContext->m_pNewDocTemplate = pTemplate;
						pContext->m_pNewViewClass = pTemplate->GetViewClass();

						if (nID == 0){
							if (nCol == 1)
								m_SplitInfo[nID].m_wndSplitter.AddView(RIGHT_SIDE, RUNTIME_CLASS(CPrintCameraView), pContext);
							else{
								m_SplitInfo[nID].m_wndSplitter.AddView(LEFT_SIDE, RUNTIME_CLASS(CPrintPrintControlView), pContext);  //Page First
								m_SplitInfo[nID].m_wndSplitter.AddView(LEFT_SIDE, RUNTIME_CLASS(CPrintFileOperateView), pContext);   //Page Second
								m_SplitInfo[nID].m_wndSplitter.AddView(LEFT_SIDE, RUNTIME_CLASS(CPrintMotionControlView), pContext); //Page Third
								m_SplitInfo[nID].m_wndSplitter.AddView(LEFT_SIDE, RUNTIME_CLASS(CPrintParamSetView), pContext);	   //Page Forth
							}
							m_SplitInfo[nID].m_wndSplitter.SetColumnInfo(nCol, cx, cx);
						}
						/*if (!m_SplitInfo[nID].m_wndSplitter.CreateView(nRow, nCol,
						pContext->m_pNewViewClass, sizeDefault, pContext))
						return FALSE;*/
						pTemplate->m_pView = (CView*)m_SplitInfo[nID].m_wndSplitter.GetPane(nRow, nCol);
					}
				}
				// disable view ID to use for next splitter view
				TmpPos = pApp->GetFirstDocTemplatePosition();
				while (TmpPos)
				{
					CJetPrintDTDocTemplate* pTemplate = (CJetPrintDTDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
					if (pTemplate->m_nSplitterID == nID)
						pTemplate->m_pView->SetDlgCtrlID(0);
				}
			}
		}
	}


	// create the other views (no splitter)
	TmpPos = pApp->GetFirstDocTemplatePosition();
	while (TmpPos)
	{
		CJetPrintDTDocTemplate* pTemplate = (CJetPrintDTDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
		if (pTemplate->m_nSplitterID == -1)
		{
			pContext->m_pNewViewClass = pTemplate->GetViewClass();
			pView = (CView*)CreateView(pContext, AFX_IDW_PANE_FIRST);
			ASSERT(pView != NULL);
			pTemplate->m_pView = pView;
			// disable view ID to use for next view
			pView->SetDlgCtrlID(0);
		}
	}
	SetActiveView(NULL);

	CJetPrintDTDocTemplate *pTemplate = NULL;

	/*pTemplate = pApp->GetTemplate(RUNTIME_CLASS(CMainDXFView));
	if (pTemplate) 
		ActivateView(pTemplate->GetViewClass());*/

	// activate the first doctemplate view
	//pTemplate = pApp->GetFirstTemplate();
	TmpPos = pApp->GetFirstDocTemplatePosition();
	if (TmpPos){
		do{
			pTemplate = (CJetPrintDTDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
		} while (pTemplate->m_nSplitterID == -1);

		ActivateView(pTemplate->GetViewClass());

		m_SplitInfo[0].m_wndSplitter.SwitchToView(LEFT_SIDE, 0);
	}
	
	return TRUE;
}

void CJetPrintDTFrame::ActivateView(CRuntimeClass* pViewClass)
{
	CJetPrintDTApp* pApp = (CJetPrintDTApp*)AfxGetApp();
	CView* pActiveView = GetActiveView();
	CJetPrintDTDocTemplate* pNewTemplate = pApp->GetTemplate(pViewClass);
	CJetPrintDTDocTemplate* pActiveTemplate = pApp->GetTemplate(pActiveView);

	// the view class doesn't exist
	if (pNewTemplate == NULL) 
		return;

	// the new template is the active template, do nothing
	if (pNewTemplate == pActiveTemplate) 
		return;

	if (pActiveView)
		pActiveView->ShowWindow(SW_HIDE);
	
	if (pActiveTemplate != NULL)
	{
		// the active and new template are in the same splitter, change the active view
		if (pActiveTemplate->m_nSplitterID != -1 &&
			pActiveTemplate->m_nSplitterID == pNewTemplate->m_nSplitterID)
		{
			UpdateResource(pNewTemplate);
			SetActiveView(pNewTemplate->m_pView);
			return;
		}

		// the active view is in a splitter, hide all views in the splitter and the splitter
		if (pActiveTemplate->m_nSplitterID != -1)
		{
			POSITION TmpPos;
			CJetPrintDTDocTemplate* pTemplate;
			int nID = pActiveTemplate->m_nSplitterID;
			TmpPos = pApp->GetFirstDocTemplatePosition();
			while (TmpPos)
			{
				pTemplate = (CJetPrintDTDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
				if (pTemplate->m_nSplitterID == nID)
				{
					pTemplate->m_pView->ShowWindow(SW_HIDE);
					pTemplate->m_pView->SetDlgCtrlID(0);
				}
			}
			// hide the splitter
			m_SplitInfo[nID].m_wndSplitter.SetDlgCtrlID(0);
			m_SplitInfo[nID].m_wndSplitter.ShowWindow(SW_HIDE);
		}
		// the active view is not in a splitter, hide this view
		else
		{
			pActiveTemplate->m_pView->SetDlgCtrlID(0);
			pActiveTemplate->m_pView->ShowWindow(SW_HIDE);
		}
	}

	// the new view is in a splitter, show all splitter views and the splitter
	if (pNewTemplate->m_nSplitterID != -1)
	{
		POSITION TmpPos;
		CJetPrintDTDocTemplate* pTemplate;
		int nID = pNewTemplate->m_nSplitterID;
		TmpPos = pApp->GetFirstDocTemplatePosition();
		while (TmpPos)
		{
			pTemplate = (CJetPrintDTDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
			if (pTemplate->m_nSplitterID == nID)
			{
				int nRow = pTemplate->m_nRow;
				int nCol = pTemplate->m_nCol;
				int nDlgID = m_SplitInfo[nID].m_wndSplitter.IdFromRowCol(nRow, nCol);
				pTemplate->m_pView->SetDlgCtrlID(nDlgID);
				pTemplate->m_pView->ShowWindow(SW_SHOW);
			}
		}
		// show the splitter
		m_SplitInfo[nID].m_wndSplitter.SetDlgCtrlID(AFX_IDW_PANE_FIRST);
		m_SplitInfo[nID].m_wndSplitter.ShowWindow(SW_SHOW);
		if (nID == 1){
			//m_SplitInfo[nID].m_wndSplitter.ShowActiveView();
		}
	}
	// the new view is not in a splitter, active the new view
	else
	{
		pNewTemplate->m_pView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);
		pNewTemplate->m_pView->ShowWindow(SW_SHOW);
	}

	SetActiveView(NULL);
	UpdateResource(pNewTemplate);

	// display and update the new view
	SetActiveView(pNewTemplate->m_pView);
	RecalcLayout();

	pNewTemplate->m_pView->Invalidate();
}

BOOL CJetPrintDTFrame::IsViewActive(CRuntimeClass* pViewClass)
{
	CView* pActiveView = GetActiveView();
	if (pActiveView == NULL) return FALSE;
	return (pActiveView->GetRuntimeClass() == pViewClass);
}

void CJetPrintDTFrame::UpdateResource(CJetPrintDTDocTemplate *pNewTemplate)
{
	CJetPrintDTApp* pApp = (CJetPrintDTApp*)AfxGetApp();

	// support for view context menu, accelerator and help ID like MDI
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(pNewTemplate->GetResourceID()), RT_MENU);

	// if menu is not the default menu, destroy it
	HMENU hMenu = ::GetMenu(m_hWnd);
	if (hMenu != m_hMenuDefault) ::DestroyMenu(hMenu);
	// load menu from doctemplate
	hMenu = ::LoadMenu(hInst, MAKEINTRESOURCE(pNewTemplate->GetResourceID()));
	// if no menu, get default
	if (hMenu == NULL) hMenu = m_hMenuDefault;
	// load menu
	::SetMenu(m_hWnd, hMenu);

	// load new accelerator table
	HACCEL hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(pNewTemplate->GetResourceID()));
	// if no table for this template, load default
	if (hAccel == NULL) ::LoadAccelerators(hInst, MAKEINTRESOURCE(pApp->GetFirstTemplate()->GetResourceID()));

	// chage help ID for this view
	m_nIDHelp = pNewTemplate->GetResourceID();

	// change the title of the document
	pNewTemplate->GetDocString(m_strTitle, CDocTemplate::windowTitle);
	OnUpdateFrameTitle(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CJetPrintDTFrame message handlers
