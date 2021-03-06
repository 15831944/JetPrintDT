
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESIZABLELAYOUT_H_INCLUDED_)
#define AFX_RESIZABLELAYOUT_H_INCLUDED_

#include <afxtempl.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// useful compatibility constants (the only one required is NOANCHOR)

const CSize NOANCHOR(-1,-1),
	TOP_LEFT(0,0), TOP_CENTER(50,0), TOP_RIGHT(100,0),
	MIDDLE_LEFT(0,50), MIDDLE_CENTER(50,50), MIDDLE_RIGHT(100,50),
	BOTTOM_LEFT(0,100), BOTTOM_CENTER(50,100), BOTTOM_RIGHT(100,100);


class CResizableLayout
{
	class LayoutInfo
	{
	public:
		HWND hWnd;
		UINT nCallbackID;

		BOOL bAdjHScroll;
		BOOL bNeedRefresh;

		// upper-left corner
		SIZE sizeTypeTL;
		SIZE sizeMarginTL;
		
		// bottom-right corner
		SIZE sizeTypeBR;
		SIZE sizeMarginBR;
	
	public:
		LayoutInfo()
		{ ZeroMemory(this, sizeof(LayoutInfo)); }

		LayoutInfo(HWND hwnd, SIZE tl_t, SIZE tl_m, 
			SIZE br_t, SIZE br_m, BOOL hscroll, BOOL refresh)
		{
			hWnd = hwnd;
			nCallbackID = 0;

			bAdjHScroll = hscroll;
			bNeedRefresh = refresh;

			sizeTypeTL = tl_t;
			sizeMarginTL = tl_m;
			
			sizeTypeBR = br_t;
			sizeMarginBR = br_m;
		}
	};

	// list of repositionable controls (in 2 parts: anchors, callbacks)
	CArray<LayoutInfo, LayoutInfo&> m_arrLayout;
	int m_iFirstCallback;		// index of first callback

	void EnumAndClipChildWindow(HWND hWnd, CDC* pDC);

protected:
	virtual BOOL LikesClipping(HWND hWnd);
	virtual BOOL NeedsRefresh(HWND hWnd);

	// exclude child windows from the clipping region
	void ClipChildren(CDC *pDC);
	
	// override for scrollable or expanding parent windows
	virtual void GetTotalClientRect(LPRECT lpRect);

	// add anchors to a control, given its HWND
	void AddAnchor(HWND hWnd, CSize sizeTypeTL, CSize sizeTypeBR = NOANCHOR);

	// add anchors to a control, given its ID
	void AddAnchor(UINT nID, CSize sizeTypeTL, CSize sizeTypeBR = NOANCHOR)
	{
		AddAnchor(::GetDlgItem(GetResizableWnd()->GetSafeHwnd(), nID),
			sizeTypeTL, sizeTypeBR);
	}

	// add a callback (control ID or HWND is unknown or may change)
	void AddAnchorCallback(UINT nCallbackID);

	// adjust children's layout, when parent's size changes
	void ArrangeLayout();

	// override to provide dynamic control's layout info
	virtual BOOL ArrangeLayoutCallback(LayoutInfo& layout);

	// reset layout content
	void RemoveAllAnchors()
	{
		m_arrLayout.RemoveAll();
		m_iFirstCallback = 0;
	}

	virtual CWnd* GetResizableWnd() = 0;

public:
	CResizableLayout() { m_iFirstCallback = 0; }

	virtual ~CResizableLayout()
	{
		// just for safety
		RemoveAllAnchors();
	}
};

#endif // !defined(AFX_RESIZABLELAYOUT_H_INCLUDED_)
