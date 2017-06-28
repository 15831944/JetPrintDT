#ifndef _JETPRINTDT_FRAME_H
#define _JETPRINTDT_FRAME_H

#include "JetPrintDT.h"
#include "EXSplitterWnd.h"

class CJetPrintDTSplitInfo
{
public:
	int m_maxRow, m_maxCol;
	CExSplitterWnd m_wndSplitter;
	CJetPrintDTSplitInfo::CJetPrintDTSplitInfo();
	CJetPrintDTSplitInfo &operator=(const CJetPrintDTSplitInfo &SplitInfo);
};

class CJetPrintDTFrame : public CFrameWndEx
{
protected: // create from serialization only
	CJetPrintDTFrame();
	DECLARE_DYNCREATE(CJetPrintDTFrame)

	// Attributes
public:
	CMap <int, int, CJetPrintDTSplitInfo, CJetPrintDTSplitInfo&> m_SplitInfo;
public:

	// Operations
public:
	void ActivateView(CRuntimeClass *pViewClass);
	BOOL IsViewActive(CRuntimeClass *pViewClass);
	void UpdateResource(CJetPrintDTDocTemplate* pNewTemplate);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIFrameWnd)
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CJetPrintDTFrame();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSDIFrameWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //_JETPRINTDT_FRAME_H
