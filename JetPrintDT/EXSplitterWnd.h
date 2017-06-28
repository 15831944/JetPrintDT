
#if !defined(_EX_SPLITTERWND_H)
#define _EX_SPLITTERWND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#include <vector>
using namespace std;

#define LEFT_SIDE     0
#define RIGHT_SIDE    1


class CExSplitterWnd : public CSplitterWnd  
{
public:
	CExSplitterWnd(int nLevel = 0);
	virtual ~CExSplitterWnd();

	void SetInitialStatus(); // restore position and visibility
	inline void SetMinSize(int i, int x) {ASSERT(i >= 0 && i <= 2); m_nPaneMinSize[i] = x;};

	void SetRegistryKey(LPCSTR p) {m_sRegKey = p;};

   /* support for switching views */
	int AddView(int nSide, CRuntimeClass * pViewClass, CCreateContext* pContext);
	 void SwitchToView(int nSide,int nViewIX = -1);
	 /* support for switching views */
	bool HideView(int nRow, int nCol);
	bool ShowView(int nRow, int nCol, CWnd* pWnd);
	int m_iCurrentActiveWnd;

protected:   
	
	inline void SideToRowCol(int nSide, int* nRow, int*  nCol) 
	{
		*nRow = 0;
		*nCol = nSide;
	};

   void SaveToRegistry();
   void RememberSize();

   

	int m_nPaneSize[2];
	int m_nPaneMinSize[2];
   CString m_sRegKey;
   int m_nLevel;

public:
   /* support for switching views */
	vector<CWnd*> m_views[2];
	int m_nCurrentView[2]; // side as index
	

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#endif // _EX_SPLITTERWND_H
