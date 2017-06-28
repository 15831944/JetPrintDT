#ifndef __JET_DXF_TEXT_VIEW_H
#define __JET_DXF_TEXT_VIEW_H

#include "JetDxfBaseView.h"

class CJetDxfTextView :
	public CJetDxfBaseView
{
public:
	CJetDxfTextView();
	~CJetDxfTextView();

	void PlotText(
		HDC				hdc,			// handle of DC for drawing
		PENTITYHEADER	pEntityHeader,	// pointer to entity data header
		PENTTEXT		pText,			// pointer to text data
		double			x,				// x position of insertion point (default = 0)
		double			y,				// y position of insertion point (default = 0)
		double			xScale,			// x scale factor (default = 1)
		double			yScale,			// y scale factor (default = 1)
		double			Rotation		// rotation angle (default = 0)
		);

	BOOL GetTextRect(PENTITYHEADER pEntityHeader, PENTTEXT pText, REALRECT* pRect);
	BOOL IsTextInViewableArea(PVIEW pView, RECT *pTextRect);
	UINT DecodeText(LPCTSTR TextIn, LPTSTR TextOut);

};


#endif // __JET_DXF_TEXT_VIEW_H

