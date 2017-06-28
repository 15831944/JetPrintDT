
#ifndef __JET_DXF_VIEW_H
#define __JET_DXF_VIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../stdafx.h"

#include "JetDxfBaseView.h"
#include "JetDxfTextView.h"
#include "JetDxfDimView.h"


class CJetDxfView
{
public:
	CJetDxfView();
	~CJetDxfView();

	BOOL drwInitialize();

	BOOL drwInitView(
		int			x,					// horizontal position of drawing window
		int			y,					// vertical position of drawing window
		int			Width,				// width of drawing window
		int			Height				// height of drawing window
		);

	BOOL drwPaint(
		HDC			hdc					// handle to view window DC
		);

	BOOL GetLineRect(
		PENTITYHEADER pEntityHeader,    //pointer to EntityHeader
		PENTLINE pLine,                 //pointer to line
		REALRECT* pRect                 // Out Line Rect  
		);

	BOOL GetPointRect(
		PENTITYHEADER pEntityHeader,    //pointer to EntityHeader
		PENTPOINT pPoint,				//pointer to point object
		REALRECT* pRect					//out point rect	
		);

	BOOL GetCircleRect(
		PENTITYHEADER pEntityHeader,    //pointer to EntityHeader
		PENTCIRCLE pCircle,				//pointer to circle object	
		REALRECT* pRect					//out circle rect	
		);

	BOOL GetSolidRect(
		PENTITYHEADER pEntityHeader,    //pointer to EntityHeader
		PENTSOLID pSolid,				//pointer to solid object	
		REALRECT* pRect					//out circle rect
		);

	BOOL GetInsertBlockRect(
		PENTITYHEADER pEntityHeader,	//pointer to Entityheader
		PENTINSERT pInsert,				//pointer insert object
		REALRECT* pRect					//out block rect
		);

	BOOL GetPolylineRect(
		PENTITYHEADER pEntityHeader,	//pointer to EntityHeader
		PENTPOLYLINE pPolyline,			//pointer to Polyline
		REALRECT* pRect					//out Polyline rect	
		);

	BOOL GetLWPolylineRect(
		PENTITYHEADER pEntityHeader,
		PENTLWPOLYLINE pLWPolyline,
		REALRECT *pRect
		);

	BOOL GetSplineRect(
		PENTITYHEADER pEntityHeader,
		PENTSPLINE pSpline,
		REALRECT *pRect
		);

	BOOL GetArcRect(
		PENTITYHEADER pEntityHeader,
		PENTARC pArc,
		REALRECT *pRect
		);

	BOOL GetEllipseRect(
		PENTITYHEADER pEntityHeader,
		PENTELLIPSE pEllipse,
		REALRECT *pRect
		);

	BOOL GetEntityRect(
		PENTITYHEADER pEntityHeader,	//pointer to EntityHeader
		LPVOID pEntityData,				//pointer to Entity object
		REALRECT* pRect					//out Entity rect
		);

	void DrawLine(
		HDC hdc,
		COLORREF rgbcolor,
		int x1,
		int y1,
		int x2,
		int y2
		);

	void DrawDashLine(
		HDC hdc,
		LTYPE* pLType,
		double x1,
		double y1,
		double x2,
		double y2,
		COLORREF rgbcolor,
		double Scale,
		double PatternStartPoint,
		double x,
		double y,
		double xScale,
		double yScale,
		double Rotation
		);

	void PlotEntity(
		HDC hdc,
		PENTITYHEADER pEntityHeader,
		LPVOID pEntityData,
		double x = 0,
		double y = 0,
		double xScale = 1,
		double yScale = 1,
		double Rotation = 0
		);

	BOOL drwPlot(
		HDC hdc
		);

	VOID drwFindEntity(OBJHANDLE &objhandle,
		LPCTSTR strBlockName,
		PENTITYHEADER pEntityHeader,
		LPVOID pEntityData,
		DWORD dwFindType,
		DWORD* pdwEntityPos = NULL);

	// Functions for changing view properties
	BOOL drwGetViewProperties(
		PVIEW		pView				// pointer to view properties structure
		);

	BOOL drwSetViewProperties(
		PVIEW		pView				// pointer to view properties structure
		);

	// Zoom Functions
	BOOL drwGetDrawingBorder(
		PREALRECT	pRect				// pointer to drawing boundary rect
		);

	BOOL drwZoomExtents();

	BOOL SetZoomLevel(double dbZoomLevel);
	double GetZoomLevel();

public:
	CJetDxfDimView   *m_pJetDxfDimView;
	CJetDxfTextView  *m_pJetDxfTextView;
	REALRECT borderRect;
};

#endif // __JET_DXF_VIEW_H
