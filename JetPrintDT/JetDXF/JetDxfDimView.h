#pragma once
#include "JetDxfBaseView.h"

#define PI	3.1415926535

class CJetDxfDimView :public CJetDxfBaseView
{
public:
	CJetDxfDimView();
	~CJetDxfDimView();

	BOOL GetDimRect(
		PENTITYHEADER	pEntityHeader,	// pointer to entity data header
		PENTDIMENSION	pDim,			// pointer to dimension data structure
		REALRECT*		pRect			// pointer to entity boundary rectangle
		);

	BOOL dimDrawArrowHead(
		OBJHANDLE BlockObjhandle,		//Handle Object
		PENTITYHEADER pHeader,			//pointer to Header	
		double cx,
		double cy,
		double Angle,
		double ArrowSize,
		LPCTSTR ArrowHeadName,
		int BLKNum
		);

	BOOL DimLinear(
		OBJHANDLE		BlockObjhandle,	// handle of block that entity must be added in it. (NULL = Entity Section)
		PENTITYHEADER	pDimHeader,		// pointer to dim data header
		PENTDIMENSION	pDimension,		// pointer to dim data structure
		PDIMSTYLE		pDimStyle		// pointer of dim-style structure
		);

	BOOL IsDimInViewableArea(PVIEW pView, RECT *pDimRect);

	void PlotDimension(
		HDC				hdc,			// handle of DC for drawing
		PENTITYHEADER	pEntityHeader,	// pointer to entity data header
		PENTDIMENSION	pDimension,		// pointer to dimension data
		double			x,				// x position of insertion point (default = 0)
		double			y,				// y position of insertion point (default = 0)
		double			xScale,			// x scale factor (default = 1)
		double			yScale,			// y scale factor (default = 1)
		double			Rotation		// rotation angle (default = 0)
		);
};

