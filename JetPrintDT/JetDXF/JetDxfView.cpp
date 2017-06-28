#include <float.h>
#include "JetDxfView.h"
#include "JetDxfColors.h"
#include "OpenSpline.h"

CJetDxfView::CJetDxfView()
{
	m_pJetDxfDimView = NULL;
	m_pJetDxfTextView = NULL;

	borderRect.left = 0.0;
	borderRect.right = 0.0;
	borderRect.top = 0.0;
	borderRect.bottom = 0.0;
}

CJetDxfView::~CJetDxfView()
{
	//do something
	if(NULL != m_pJetDxfDimView)
	{

		delete m_pJetDxfDimView;
		m_pJetDxfDimView = NULL;
	}

	if(NULL != m_pJetDxfTextView)
	{
		delete m_pJetDxfTextView;
		m_pJetDxfTextView = NULL;
	}
}

BOOL CJetDxfView::drwInitialize()
{
	m_pJetDxfDimView = new CJetDxfDimView;
	if(NULL != m_pJetDxfDimView)
	{
		m_pJetDxfDimView->drwCreate();
	}

	m_pJetDxfTextView = new CJetDxfTextView;
	if(NULL == m_pJetDxfTextView)
	{
		//do something
	}

	return TRUE;
}

/*-------------------------------------------------------------------*
|  GetLineRect                                                      |
|  Inputs:                                                          |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      PENTLINE pLine = pointer to line data structure              |
|      REALRECT* pRect = pointer to entity boundary rectangle       |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::GetLineRect(PENTITYHEADER pEntityHeader, PENTLINE pLine, REALRECT* pRect)
{
	pRect->top = __max(pLine->Point0.y, pLine->Point1.y);
	pRect->left = __min(pLine->Point0.x, pLine->Point1.x);
	pRect->bottom = __min(pLine->Point0.y, pLine->Point1.y);
	pRect->right = __max(pLine->Point0.x, pLine->Point1.x);
	return TRUE;
}

/*-------------------------------------------------------------------*
|  GetPointRect                                                     |
|  Inputs:                                                          |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      PENTPOINT pPoint = pointer to point data structure           |
|      REALRECT* pRect = pointer to entity boundary rectangle       |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::GetPointRect(PENTITYHEADER pEntityHeader, PENTPOINT pPoint, REALRECT* pRect)
{
	pRect->top = pPoint->Point0.y;
	pRect->left = pPoint->Point0.x;
	pRect->bottom = pPoint->Point0.y;
	pRect->right = pPoint->Point0.x;
	return TRUE;
}

/*-------------------------------------------------------------------*
|  GetCircleRect                                                    |
|  Inputs:                                                          |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      PENTCIRCLE pCircle = pointer to circle data structure        |
|      REALRECT* pRect = pointer to entity boundary rectangle       |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::GetCircleRect(PENTITYHEADER pEntityHeader, PENTCIRCLE pCircle, REALRECT* pRect)
{
	pRect->top = pCircle->Point0.y + pCircle->Radius;
	pRect->left = pCircle->Point0.x - pCircle->Radius;
	pRect->bottom = pCircle->Point0.y - pCircle->Radius;
	pRect->right = pCircle->Point0.x + pCircle->Radius;
	return TRUE;
}

/*-------------------------------------------------------------------*
|  GetSolidRect                                                     |
|  Inputs:                                                          |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      PENTSOLID pSolid = pointer to solid data structure           |
|      REALRECT* pRect = pointer to entity boundary rectangle       |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::GetSolidRect(PENTITYHEADER pEntityHeader, PENTSOLID pSolid, REALRECT* pRect)
{
	pRect->top = __max(pSolid->Point0.y, pSolid->Point1.y);
	pRect->top = __max(pRect->top, pSolid->Point2.y);
	pRect->top = __max(pRect->top, pSolid->Point3.y);
	pRect->left = __min(pSolid->Point0.x, pSolid->Point1.x);
	pRect->left = __min(pRect->left, pSolid->Point2.x);
	pRect->left = __min(pRect->left, pSolid->Point3.x);
	pRect->bottom = __min(pSolid->Point0.y, pSolid->Point1.y);
	pRect->bottom = __min(pRect->bottom, pSolid->Point2.y);
	pRect->bottom = __min(pRect->bottom, pSolid->Point3.y);
	pRect->right = __max(pSolid->Point0.x, pSolid->Point1.x);
	pRect->right = __max(pRect->right, pSolid->Point2.x);
	pRect->right = __max(pRect->right, pSolid->Point3.x);
	return TRUE;
}

/*-------------------------------------------------------------------*
|  GetInsertBlockRect                                               |
|  Inputs:                                                          |
|      PDRAWING pDrawing = pointer to the current DRAWING structure |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      PENTINSERT pInsert = pointer to insert data structure        |
|      REALRECT* pRect = pointer to entity boundary rectangle       |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::GetInsertBlockRect(PENTITYHEADER pEntityHeader, PENTINSERT pInsert, REALRECT* pRect)
{
	BLOCKHEADER		BlockHeader;
	ENTITYHEADER	BlockEntityHeader;
	char			BlockEntityData[4096];

	pRect->left = DBL_MAX;
	pRect->top = DBL_MIN;
	pRect->right = DBL_MIN;
	pRect->bottom = DBL_MAX;

	BlockHeader.Objhandle = pInsert->BlockHeaderObjhandle;
	if (m_pJetDxfDimView->drwFindBlock_Direct(FIND_BYHANDLE, &BlockHeader) > 0)
	{
		if (m_pJetDxfDimView->drwFindEntity_Direct(CA2W(BlockHeader.Name), &BlockEntityHeader, &BlockEntityData, FIND_FIRST) > 0)
		{
			do{
				REALRECT	Rect;
				if (GetEntityRect(&BlockEntityHeader, BlockEntityData, &Rect))
				{
					pRect->top = __max(Rect.top*pInsert->YScale + pInsert->Point0.y, pRect->top);
					pRect->left = __min(Rect.left*pInsert->XScale + pInsert->Point0.x, pRect->left);
					pRect->bottom = __min(Rect.bottom*pInsert->YScale + pInsert->Point0.y, pRect->bottom);
					pRect->right = __max(Rect.right*pInsert->XScale + pInsert->Point0.x, pRect->right);
				}
			} while (m_pJetDxfDimView->drwFindEntity_Direct(CA2W(BlockHeader.Name), &BlockEntityHeader, &BlockEntityData, FIND_NEXT) > 0);
		}
	}

	return TRUE;
}

/*-------------------------------------------------------------------*
|  GetPolylineRect                                                  |
|  Inputs:                                                          |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      PENTPOLYLINE pPolyline = pointer to polyline data structure  |
|      REALRECT* pRect = pointer to entity boundary rectangle       |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::GetPolylineRect(PENTITYHEADER pEntityHeader, PENTPOLYLINE pPolyline, REALRECT* pRect)
{
	pRect->top = pPolyline->pVertex[0].Point.y;
	pRect->left = pPolyline->pVertex[0].Point.x;
	pRect->bottom = pPolyline->pVertex[0].Point.y;
	pRect->right = pPolyline->pVertex[0].Point.x;

	for (UINT i = 1; i < pPolyline->nVertex; i++)
	{
		pRect->top = __max(pRect->top, pPolyline->pVertex[i].Point.y);
		pRect->left = __min(pRect->left, pPolyline->pVertex[i].Point.x);
		pRect->bottom = __min(pRect->bottom, pPolyline->pVertex[i].Point.y);
		pRect->right = __max(pRect->right, pPolyline->pVertex[i].Point.x);
	}

	return TRUE;
}

BOOL CJetDxfView::GetLWPolylineRect(PENTITYHEADER pEntityHeader, PENTLWPOLYLINE pLWPolyline, REALRECT *pRect)
{
	BOOL bResult = FALSE;

	pRect->top = pLWPolyline->pVertexPoint[0].y;
	pRect->left = pLWPolyline->pVertexPoint[0].x;
	pRect->bottom = pLWPolyline->pVertexPoint[0].y;
	pRect->right = pLWPolyline->pVertexPoint[0].x;

	int nCount = pLWPolyline->CountOfVertex;
	for (int i = 0; i < nCount; i++)
	{
		pRect->top = __max(pRect->top, pLWPolyline->pVertexPoint[i].y);
		pRect->left = __min(pRect->left, pLWPolyline->pVertexPoint[i].x);
		pRect->bottom = __min(pRect->bottom, pLWPolyline->pVertexPoint[i].y);
		pRect->right = __max(pRect->right, pLWPolyline->pVertexPoint[i].x);
	}

	bResult = TRUE;
	return bResult;
}

BOOL CJetDxfView::GetSplineRect(PENTITYHEADER pEntityHeader, PENTSPLINE pSpline, REALRECT *pRect)
{
	BOOL bResult = FALSE;

	pRect->top = pSpline->pControlPoint[0].y;
	pRect->left = pSpline->pControlPoint[0].x;
	pRect->bottom = pSpline->pControlPoint[0].y;
	pRect->right = pSpline->pControlPoint[0].x;

	int nCount = pSpline->SplineControlPoints;
	for (int i = 0; i < nCount; i++)
	{
		pRect->top = __max(pRect->top, pSpline->pControlPoint[i].y);
		pRect->left = __min(pRect->left, pSpline->pControlPoint[i].x);
		pRect->bottom = __min(pRect->bottom, pSpline->pControlPoint[i].y);
		pRect->right = __max(pRect->right, pSpline->pControlPoint[i].x);
	}

	bResult = TRUE;
	return bResult;

}

BOOL CJetDxfView::GetArcRect(PENTITYHEADER pEntityHeader, PENTARC pArc, REALRECT *pRect)
{
	BOOL bResult = FALSE;
	pRect->left = pArc->Point0.x;
	pRect->top = pArc->Point0.y;
	pRect->right = pArc->Point0.x;
	pRect->bottom = pArc->Point0.y;

	REALPOINT startPoint;
	startPoint.x = pArc->Point0.x + pArc->Radius * sin(pArc->StartAngle);
	startPoint.y = pArc->Point0.y + pArc->Radius * cos(pArc->StartAngle);
	startPoint.z = 0.0;

	REALPOINT endPoint;
	endPoint.x = pArc->Point0.x + pArc->Radius * sin(pArc->EndAngle);
	endPoint.y = pArc->Point0.y + pArc->Radius * cos(pArc->EndAngle);
	endPoint.z = 0.0;

	double angle = pArc->EndAngle - pArc->StartAngle;
	if (angle <= 90.0)
	{
		pRect->left = __min(pArc->Point0.x, __min(startPoint.x, endPoint.x));

	}

	pRect->left = pArc->Point0.x - pArc->Radius;
	pRect->right = pArc->Point0.x + pArc->Radius;
	pRect->top = pArc->Point0.y + pArc->Radius;
	pRect->bottom = pArc->Point0.y - pArc->Radius;

	bResult = TRUE;

	return bResult;
}

BOOL CJetDxfView::GetEllipseRect(PENTITYHEADER pEntityHeader, PENTELLIPSE pEllipse, REALRECT *pRect)
{
	BOOL bResult = FALSE;
	REALPOINT leftPoint;
	leftPoint.x= pEllipse->CenterPoint.x - pEllipse->MajorAxisEndPoint.x;
	leftPoint.y = pEllipse->CenterPoint.y - pEllipse->MajorAxisEndPoint.y;
	leftPoint.z = 0.0;

	REALPOINT rightPoint;
	rightPoint.x = pEllipse->CenterPoint.x + pEllipse->MajorAxisEndPoint.x;
	rightPoint.y = pEllipse->CenterPoint.y + pEllipse->MajorAxisEndPoint.y;
	rightPoint.z = 0.0;

	REALPOINT topPoint;
	topPoint.x = pEllipse->CenterPoint.x - pEllipse->MajorAxisEndPoint.y * pEllipse->MinorToMajorRatio;
	topPoint.y = pEllipse->CenterPoint.y + pEllipse->MajorAxisEndPoint.x * pEllipse->MinorToMajorRatio;
	topPoint.z = 0.0;

	REALPOINT bottomPoint;
	bottomPoint.x = pEllipse->CenterPoint.x + pEllipse->MajorAxisEndPoint.y * pEllipse->MinorToMajorRatio;
	bottomPoint.y = pEllipse->CenterPoint.y - pEllipse->MajorAxisEndPoint.x * pEllipse->MinorToMajorRatio;
	bottomPoint.z = 0.0;
	
	REALRECT rect;
	rect.left = __min(leftPoint.x, bottomPoint.x);
	rect.right = __max(rightPoint.x, topPoint.x);
	rect.top = __max(leftPoint.y, topPoint.y);
	rect.bottom = __min(rightPoint.y, bottomPoint.y);


	pRect->top = rect.top;
	pRect->bottom = rect.bottom;
	pRect->left = rect.left;
	pRect->right = rect.right;

	bResult = TRUE;
	return bResult;
}

/*-------------------------------------------------------------------*
|  GetEntityRect                                                    |
|  Descriptions:                                                    |
|      Calculates entity boundary rectangle                         |
|  Inputs:                                                          |
|      PDRAWING pDrawing = pointer to the current DRAWING structure |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure        |
|      REALRECT* pRect = pointer to entity boundary rectangle       |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::GetEntityRect(PENTITYHEADER pEntityHeader, LPVOID pEntityData, REALRECT* pRect)
{
	BOOL	result;

	switch (pEntityHeader->EntityType)
	{
	case ENT_LINE:
		result = GetLineRect(pEntityHeader, (PENTLINE)pEntityData, pRect);
		break;

	case ENT_POINT:
		result = GetPointRect(pEntityHeader, (PENTPOINT)pEntityData, pRect);
		break;

	case ENT_CIRCLE:
		result = GetCircleRect(pEntityHeader, (PENTCIRCLE)pEntityData, pRect);
		break;

	case ENT_ELLIPSE:
		result = GetEllipseRect(pEntityHeader, (PENTELLIPSE)pEntityData, pRect);
     	break;

		//	case ENT_TEXT:
		//		break;

     case ENT_ARC:
		 result = GetArcRect(pEntityHeader, (PENTARC)pEntityData, pRect);
		break;

	case ENT_SPLINE:
		result = GetSplineRect(pEntityHeader, (PENTSPLINE)pEntityData, pRect);
		break;

	case ENT_LWPOLYLINE:
		result = GetLWPolylineRect(pEntityHeader, (PENTLWPOLYLINE)pEntityData, pRect);
		break;

	case ENT_SOLID:
		result = GetSolidRect(pEntityHeader, (PENTSOLID)pEntityData, pRect);
		break;

	case ENT_INSERT:
		result = GetInsertBlockRect(pEntityHeader, (PENTINSERT)pEntityData, pRect);
		break;

	case ENT_POLYLINE:
		result = GetPolylineRect(pEntityHeader, (PENTPOLYLINE)pEntityData, pRect);
		break;

	case ENT_DIMENSION:
		result = m_pJetDxfDimView->GetDimRect(pEntityHeader, (PENTDIMENSION)pEntityData, pRect);
		break;

	default: // Entity type is not accepted
		result = FALSE;
	}

	return result;
}

/********************************************************************
* Functions for plot the drawing on DC                             *
********************************************************************/

// Units to pixel conversions


/*-------------------------------------------------------------------*
|  DrawLine                                                         |
|  Descriptions:                                                    |
|      Draws a line in view area                                    |
|  Inputs:                                                          |
|      PDRAWING pDrawing = pointer to the current DRAWING structure |
|      HDC hdc= handle of DC for drawing                            |
|      int x1 = x1 of line                                          |
|      int y1 = y1 of line                                          |
|      int x2 = x2 of line                                          |
|      int y2 = y2 of line                                          |
|  Output: Nothing                                                  |
*-------------------------------------------------------------------*/
void CJetDxfView::DrawLine(HDC hdc, COLORREF rgbcolor, int x1, int y1, int x2, int y2)
{
	double m;

	if (abs(x2 - x1) > abs(y2 - y1))
	{
		if (x1 > x2)
		{
			int dummy;
			dummy = x1; x1 = x2; x2 = dummy;
			dummy = y1; y1 = y2; y2 = dummy;
		}
		//		if((y2>pDrawing->View.WindowTop) && (y1<pDrawing->View.WindowBottom))
{
	m = (double)(y2 - y1) / (x2 - x1);
	for (int i = __max(x1, m_pJetDxfDimView->m_pDrawing->View.WindowLeft); i <= __min(x2, m_pJetDxfDimView->m_pDrawing->View.WindowRight); i++)
	{
		int j = int(m*(i - x1)) + y1;
		if ((j >= m_pJetDxfDimView->m_pDrawing->View.WindowTop) && (j <= m_pJetDxfDimView->m_pDrawing->View.WindowBottom))
			SetPixel(hdc, i, j, rgbcolor);
	}
}
	}
	else
	{
		if (y1 > y2)
		{
			int dummy;
			dummy = x1; x1 = x2; x2 = dummy;
			dummy = y1; y1 = y2; y2 = dummy;
		}
		//		if((x2>pDrawing->View.WindowLeft) && (x1<pDrawing->View.WindowRight))
{
	m = (double)(x2 - x1) / (y2 - y1);
	for (int j = __max(y1, m_pJetDxfDimView->m_pDrawing->View.WindowTop); j <= __min(y2, m_pJetDxfDimView->m_pDrawing->View.WindowBottom); j++)
	{
		int i = int(m*(j - y1)) + x1;
		if ((i >= m_pJetDxfDimView->m_pDrawing->View.WindowLeft) && (i <= m_pJetDxfDimView->m_pDrawing->View.WindowRight))
			SetPixel(hdc, i, j, rgbcolor);
	}
}
	}

}


/*-------------------------------------------------------------------*
|  DrawDashLine                                                     |
|  Descriptions:                                                    |
|      Draws a line in view area                                    |
|  Inputs:                                                          |
|      PDRAWING pDrawing = pointer to the current DRAWING structure |
|      HDC hdc= handle of DC for drawing                            |
|      int x1 = x1 of line                                          |
|      int y1 = y1 of line                                          |
|      int x2 = x2 of line                                          |
|      int y2 = y2 of line                                          |
|      double PatternStartPoint                                     |
|      double x = x position of insertion point (default = 0)       |
|      double y = y position of insertion point (default = 0)       |
|      double xScale = x scale factor (default = 1)                 |
|      double yScale = y scale factor (default = 1)                 |
|  Output: Nothing                                                  |
*-------------------------------------------------------------------*/
void CJetDxfView::DrawDashLine(HDC hdc, LTYPE* pLType,
	double x1, double y1, double x2,
	double y2, COLORREF rgbcolor,
	double Scale, double PatternStartPoint,
	double x, double y, double xScale,
	double yScale, double Rotation)
{
	double x11, y11;
	double x12, y12;
	double m;
	double cosalpha;
	double l1, l2 = 0;
	double xend, yend;
	double MoveDirection = 1;
	int ElementNum;

	if (fabs(x2 - x1) > fabs(y2 - y1))
	{
		if (x1 > x2)
			MoveDirection = -1;

		m = (y2 - y1) / (x2 - x1);
		cosalpha = 1 / sqrt(1 + m*m);

		x11 = x1;//__max(x1,pDrawing->View.ViewLeft);
		y11 = y1 + m*(x11 - x1);
		l1 = sqrt((x1 - x11)*(x1 - x11) + (y1 - y11)*(y1 - y11));
		l2 = fmod(PatternStartPoint + l1, pLType->PatternLength*Scale);
		for (ElementNum = 0; ElementNum < pLType->ElementsNumber; ElementNum++)
		{
			if (l2 < fabs(pLType->Elements[ElementNum] * Scale))
				break;
			l2 = l2 - fabs(pLType->Elements[ElementNum] * Scale);
		}

		x12 = x11 + MoveDirection * (fabs(pLType->Elements[ElementNum] * Scale) - l2)*(cosalpha);
		y12 = y11 + (x12 - x11)*(m);

		xend = x2;//__min(x2,pDrawing->View.ViewRight);
		while (MoveDirection*x12 < MoveDirection*xend)
		{
			if (pLType->Elements[ElementNum] >= 0)
				//				DrawLine( pDrawing, hdc, rgbcolor, U2PX(x11), U2PY(y11), U2PX(x12), U2PY(y12) );
				DrawLine(hdc, rgbcolor,
				m_pJetDxfDimView->UnitToXRotationPixel(x11, y11),
				m_pJetDxfDimView->UnitToYRotationPixel(x11, y11),
				m_pJetDxfDimView->UnitToXRotationPixel(x12, y12),
				m_pJetDxfDimView->UnitToYRotationPixel(x12, y12));

			ElementNum++;
			if (ElementNum >= pLType->ElementsNumber)
				ElementNum = 0;

			x11 = x12;
			y11 = y12;
			x12 = x11 + MoveDirection * fabs(pLType->Elements[ElementNum] * Scale)*(cosalpha);
			y12 = y11 + (x12 - x11)*(m);
		}
		if (pLType->Elements[ElementNum] >= 0)
			DrawLine(hdc, rgbcolor,
			m_pJetDxfDimView->UnitToXRotationPixel(x11, y11),
			m_pJetDxfDimView->UnitToYRotationPixel(x11, y11),
			m_pJetDxfDimView->UnitToXRotationPixel(x2, y2),
			m_pJetDxfDimView->UnitToYRotationPixel(x2, y2));
		//*****************************************************************************************
	}
	else
	{
		if (y1 > y2)
			MoveDirection = -1;

		m = (x2 - x1) / (y2 - y1);
		cosalpha = 1 / sqrt(1 + m*m);

		y11 = y1;//__max(y1,pDrawing->View.WindowBottom);
		x11 = x1 + m*(y11 - y1);
		l1 = sqrt((x1 - x11)*(x1 - x11) + (y1 - y11)*(y1 - y11));
		l2 = fmod(PatternStartPoint + l1, pLType->PatternLength*Scale);
		for (ElementNum = 0; ElementNum < pLType->ElementsNumber; ElementNum++)
		{
			if (l2 < fabs(pLType->Elements[ElementNum] * Scale))
				break;
			l2 = l2 - fabs(pLType->Elements[ElementNum] * Scale);
		}

		y12 = y11 + MoveDirection * (fabs(pLType->Elements[ElementNum] * Scale) - l2)*(cosalpha);
		x12 = x11 + (y12 - y11)*(m);

		yend = y2;//__min(y2,pDrawing->View.ViewTop);
		while (MoveDirection*y12 < MoveDirection*yend)
		{
			if (pLType->Elements[ElementNum] >= 0)
				//				DrawLine( pDrawing, hdc, rgbcolor, U2PX(x11), U2PY(y11), U2PX(x12), U2PY(y12) );
				DrawLine(hdc, rgbcolor,
				m_pJetDxfDimView->UnitToXRotationPixel(x11, y11),
				m_pJetDxfDimView->UnitToYRotationPixel(x11, y11),
				m_pJetDxfDimView->UnitToXRotationPixel(x12, y12),
				m_pJetDxfDimView->UnitToYRotationPixel(x12, y12));

			ElementNum++;
			if (ElementNum >= pLType->ElementsNumber)
				ElementNum = 0;

			y11 = y12;
			x11 = x12;
			y12 = y11 + MoveDirection * fabs(pLType->Elements[ElementNum] * Scale)*(cosalpha);
			x12 = x11 + (y12 - y11)*(m);
		}
		if (pLType->Elements[ElementNum] >= 0)
			//			DrawLine( pDrawing, hdc, rgbcolor, U2PX(x11), U2PY(y11), U2PX(x2), U2PY(y2) );
			DrawLine(hdc, rgbcolor,
			m_pJetDxfDimView->UnitToXRotationPixel(x11, y11),
			m_pJetDxfDimView->UnitToYRotationPixel(x11, y11),
			m_pJetDxfDimView->UnitToXRotationPixel(x2, y2),
			m_pJetDxfDimView->UnitToYRotationPixel(x2, y2));
	}
}

/*-------------------------------------------------------------------*
|  PlotEntity                                                       |
|  Descriptions:                                                    |
|      Plots an entity into given Drawing Context                   |
|  Inputs:                                                          |
|      PDRAWING pDrawing = pointer to the current DRAWING structure |
|      HDC hdc= handle of DC for drawing                            |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure        |
|      double x = x position of insertion point (default = 0)       |
|      double y = y position of insertion point (default = 0)       |
|      double xScale = x scale factor (default = 1)                 |
|      double yScale = y scale factor (default = 1)                 |
|      double Rotation = rotation angle (default = 0)               |
|  Output: Nothing                                                  |
*-------------------------------------------------------------------*/
void CJetDxfView::PlotEntity(HDC hdc, PENTITYHEADER pEntityHeader,
	LPVOID pEntityData, double x,
	double y, double xScale,
	double yScale, double Rotation)
{
	LAYER		Layer;
	LTYPE		LType;
	COLORREF	rgbcolor;
	short		color;
	HPEN		pen, oldpen;
	HBRUSH		brush, oldbrush;
	UINT			i;
	BOOL		isLTypeAvailable = FALSE;

	// Exit and do not plot, if this object is deleted
	if (pEntityHeader->Deleted)
		return;

	rgbcolor = RGB(255, 0, 0);
	if (pEntityHeader->EntityType != ENT_TEXT)
	{
		// Initialize LayerName & LineTypeName & Color
		color = pEntityHeader->Color;

		Layer.Objhandle = pEntityHeader->LayerObjhandle;
		if (m_pJetDxfDimView->drwFindTableType_Direct(TAB_LAYER, FIND_BYHANDLE, &Layer) > 0)
		{
			if ((pEntityHeader->Color == COLOR_BYBLOCK) || (pEntityHeader->Color == COLOR_BYLAYER))
				color = Layer.Color;
			else if (Layer.Color < 0)
			{
				// Layer is off
				return;
			}
		}
		else if (color >= COLOR_BYLAYER)
			color = 0;

		rgbcolor = CadColor[color];

		LType.Objhandle = pEntityHeader->LTypeObjhandle;
		isLTypeAvailable = m_pJetDxfDimView->drwFindTableType_Direct(TAB_LTYPE, FIND_BYHANDLE, &LType);
	}

	pen = CreatePen(PS_SOLID, 0, rgbcolor);
	oldpen = (HPEN)SelectObject(hdc, pen);

	// Plot entities data on DC
	switch (pEntityHeader->EntityType)
	{
	case ENT_LINE:
		/*		if( !(((x1*xScale+x<pDrawing->View.ViewLeft) && (x2*xScale+x<pDrawing->View.ViewLeft)) ||
				((x1*xScale+x>pDrawing->View.ViewRight) && (x2*xScale+x>pDrawing->View.ViewRight)) ||
				((y1*yScale+y<pDrawing->View.ViewBottom) && (y2*yScale+y<pDrawing->View.ViewBottom)) ||
				((y1*yScale+y>pDrawing->View.ViewTop) && (y2*yScale+y>pDrawing->View.ViewTop))) )
				*/		{
					double x1 = ((PENTLINE)pEntityData)->Point0.x;
					double y1 = ((PENTLINE)pEntityData)->Point0.y;
					double x2 = ((PENTLINE)pEntityData)->Point1.x;
					double y2 = ((PENTLINE)pEntityData)->Point1.y;

					if ((LType.PatternLength == 0) || (!isLTypeAvailable))
					{ // Solid Line
						DrawLine(hdc, rgbcolor,
							m_pJetDxfDimView->UnitToXRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
							m_pJetDxfDimView->UnitToYRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
							m_pJetDxfDimView->UnitToXRotationPixel(x2, y2,x,y,xScale,yScale,Rotation),
							m_pJetDxfDimView->UnitToYRotationPixel(x2, y2,x,y,xScale,yScale,Rotation));
					}
					else
					{ // Line has Pattern
						double scale = pEntityHeader->LineTypeScale;

						double linelen = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
						double PatternStartPoint = fabs(LType.Elements[0] * scale) / 2;
						double LinePatternStartPoint = fmod(linelen, LType.PatternLength*scale) / 2;

						if ((linelen < LType.PatternLength*scale) || (LType.PatternLength*scale*m_pJetDxfDimView->m_pDrawing->View.PPU*m_pJetDxfDimView->m_pDrawing->View.ZoomLevel < 5))
						{
							DrawLine(hdc, rgbcolor,
								m_pJetDxfDimView->UnitToXRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
								m_pJetDxfDimView->UnitToYRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
								m_pJetDxfDimView->UnitToXRotationPixel(x2, y2,x,y,xScale,yScale,Rotation),
								m_pJetDxfDimView->UnitToYRotationPixel(x2, y2,x,y,xScale,yScale,Rotation));
						}
						else
						{
							double m, cosalpha;
							double x11, y11;
							double x21, y21;

							if (fabs(x2 - x1) > fabs(y2 - y1))
							{
								if (x1 > x2)
								{
									double dummy;
									dummy = x1; x1 = x2; x2 = dummy;
									dummy = y1; y1 = y2; y2 = dummy;
								}
								m = (y2 - y1) / (x2 - x1);
								cosalpha = 1 / sqrt(1 + m*m);
								x11 = x1 + LinePatternStartPoint*cosalpha;
								y11 = y1 + (x11 - x1)*m;
								x21 = x2 - LinePatternStartPoint*cosalpha;
								y21 = y2 + (x21 - x2)*m;
							}
							else
							{
								if (y1 > y2)
								{
									double dummy;
									dummy = x1; x1 = x2; x2 = dummy;
									dummy = y1; y1 = y2; y2 = dummy;
								}
								m = (x2 - x1) / (y2 - y1);
								cosalpha = 1 / sqrt(1 + m*m);
								y11 = y1 + LinePatternStartPoint*cosalpha;
								x11 = x1 + (y11 - y1)*m;
								y21 = y2 - LinePatternStartPoint*cosalpha;
								x21 = x2 + (y21 - y2)*m;
							}

							DrawLine(hdc, rgbcolor,
								m_pJetDxfDimView->UnitToXRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
								m_pJetDxfDimView->UnitToYRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
								m_pJetDxfDimView->UnitToXRotationPixel(x11, y11,x,y,xScale,yScale,Rotation),
								m_pJetDxfDimView->UnitToYRotationPixel(x11, y11,x,y,xScale,yScale,Rotation));

							DrawDashLine(hdc, &LType, x11, y11, x21, y21, rgbcolor, scale, PatternStartPoint, x, y, xScale, yScale, Rotation);

							DrawLine(hdc, rgbcolor,
								m_pJetDxfDimView->UnitToXRotationPixel(x21, y21,x,y,xScale,yScale,Rotation),
								m_pJetDxfDimView->UnitToYRotationPixel(x21, y21,x,y,xScale,yScale,Rotation),
								m_pJetDxfDimView->UnitToXRotationPixel(x2, y2,x,y,xScale,yScale,Rotation),
								m_pJetDxfDimView->UnitToYRotationPixel(x2, y2,x,y,xScale,yScale,Rotation));
						}
					}
	}
				 break;

	case ENT_POINT:
		SetPixel(
			hdc,
			m_pJetDxfDimView->UnitToHorizontalPixel(((PENTPOINT)pEntityData)->Point0.x,x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToVerticalPixel(((PENTPOINT)pEntityData)->Point0.y,x,y,xScale,yScale,Rotation),
			rgbcolor
			);
		break;

	case ENT_CIRCLE:
		double x1, y1, x2, y2, xc, yc, r;
		x1 = ((PENTCIRCLE)pEntityData)->Point0.x - ((PENTCIRCLE)pEntityData)->Radius;
		y1 = ((PENTCIRCLE)pEntityData)->Point0.y - ((PENTCIRCLE)pEntityData)->Radius;
		x2 = ((PENTCIRCLE)pEntityData)->Point0.x + ((PENTCIRCLE)pEntityData)->Radius;
		y2 = ((PENTCIRCLE)pEntityData)->Point0.y + ((PENTCIRCLE)pEntityData)->Radius;
		xc = ((PENTCIRCLE)pEntityData)->Point0.x;
		yc = ((PENTCIRCLE)pEntityData)->Point0.y;
		r = ((PENTCIRCLE)pEntityData)->Radius;

		if ((LType.PatternLength == 0) || (!isLTypeAvailable))
		{ // Solid Line Circle
			Ellipse(
				hdc,
				m_pJetDxfDimView->UnitToXRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
				m_pJetDxfDimView->UnitToYRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
				m_pJetDxfDimView->UnitToXRotationPixel(x2, y2,x,y,xScale,yScale,Rotation),
				m_pJetDxfDimView->UnitToYRotationPixel(x2, y2,x,y,xScale,yScale,Rotation)
				);
		}
		else
		{ // Circle has Pattern
			double scale = pEntityHeader->LineTypeScale;

			double primeter = 2 * PI*r*xScale;

			if ((primeter < LType.PatternLength*scale) || (LType.PatternLength*scale*m_pJetDxfDimView->m_pDrawing->View.PPU*\
				m_pJetDxfDimView->m_pDrawing->View.ZoomLevel < 5))
			{
				Ellipse(
					hdc,
					m_pJetDxfDimView->UnitToXRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
					m_pJetDxfDimView->UnitToYRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
					m_pJetDxfDimView->UnitToXRotationPixel(x2, y2,x,y,xScale,yScale,Rotation),
					m_pJetDxfDimView->UnitToYRotationPixel(x2, y2,x,y,xScale,yScale,Rotation)
					);
			}

			else
			{
				double n, alpha;
				double x11, y11;
				double x21, y21;
				double PatternStartPoint = 0;
				double len = 0;

				scale = primeter / ceil(primeter / (LType.PatternLength*scale)) / LType.PatternLength;
				n = ceil((primeter*m_pJetDxfDimView->m_pDrawing->View.ZoomLevel*\
					m_pJetDxfDimView->m_pDrawing->View.PPU) / 5);
				if (n < 8)	n = 8;
				if (n > 100)
					n = 100;
				alpha = 2 * PI / n;

				x21 = xc + r;
				y21 = yc;
				for (int i = 1; i <= n; i++)
				{
					x11 = x21;
					y11 = y21;
					x21 = xc + r*cos(alpha*double(i));
					y21 = yc + r*sin(alpha*double(i));
					len = sqrt((x11 - x21)*(x11 - x21) + (y11 - y21)*(y11 - y21));
					DrawDashLine(hdc, &LType, x11, y11, x21, y21, rgbcolor, scale, PatternStartPoint, x, y, xScale, yScale, Rotation);
					PatternStartPoint = fmod(PatternStartPoint + len, LType.PatternLength*scale);
				}
			}
		}

		break;

	case ENT_ELLIPSE:
	{
		REALPOINT leftPoint;
		leftPoint.x = ((PENTELLIPSE)pEntityData)->CenterPoint.x - ((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.x;
		leftPoint.y = ((PENTELLIPSE)pEntityData)->CenterPoint.y - ((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.y;
		leftPoint.z = 0.0;

		REALPOINT rightPoint;
		rightPoint.x = ((PENTELLIPSE)pEntityData)->CenterPoint.x + ((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.x;
		rightPoint.y = ((PENTELLIPSE)pEntityData)->CenterPoint.y + ((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.y;
		rightPoint.z = 0.0;

		REALPOINT topPoint;
		topPoint.x = ((PENTELLIPSE)pEntityData)->CenterPoint.x - ((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.y * ((PENTELLIPSE)pEntityData)->MinorToMajorRatio;
		topPoint.y = ((PENTELLIPSE)pEntityData)->CenterPoint.y + ((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.x * ((PENTELLIPSE)pEntityData)->MinorToMajorRatio;
		topPoint.z = 0.0;

		REALPOINT bottomPoint;
		bottomPoint.x = ((PENTELLIPSE)pEntityData)->CenterPoint.x + ((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.y * ((PENTELLIPSE)pEntityData)->MinorToMajorRatio;
		bottomPoint.y = ((PENTELLIPSE)pEntityData)->CenterPoint.y - ((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.x * ((PENTELLIPSE)pEntityData)->MinorToMajorRatio;
		bottomPoint.z = 0.0;

		VIEW view;
		drwGetViewProperties(&view);

		RECT viewRect;
		viewRect.top = view.WindowTop;
		viewRect.bottom = view.WindowBottom;
		viewRect.left = view.WindowLeft;
		viewRect.right = view.WindowRight;

		int x_Center = m_pJetDxfDimView->UnitToHorizontalPixel(((PENTELLIPSE)pEntityData)->CenterPoint.x);
		int y_Center = m_pJetDxfDimView->UnitToVerticalPixel(((PENTELLIPSE)pEntityData)->CenterPoint.y);
		//int x_Center = viewRect.right - viewRect.left;
		//int y_Center = viewRect.bottom - viewRect.top;

		int nGraphicsMode = SetGraphicsMode(hdc, GM_ADVANCED);
		XFORM xform;
		double ellipse_angle = -tanh((((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.y) / (((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.x));
		if (ellipse_angle != 0.0)
		{
			double fangle = (double)ellipse_angle;
			xform.eM11 = (float)cos(fangle);
			xform.eM12 = (float)sin(fangle);
			xform.eM21 = (float)-sin(fangle);
			xform.eM22 = (float)cos(fangle);
			xform.eDx = (float)( x_Center- cos(fangle)*x_Center+ sin(fangle)*y_Center);
			xform.eDy = (float)(y_Center - cos(fangle)*y_Center - sin(fangle)*x_Center);

			SetWorldTransform(hdc, &xform);
		}

		Ellipse(
			hdc,
			m_pJetDxfDimView->UnitToHorizontalPixel(leftPoint.x,x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToVerticalPixel(topPoint.y,x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToHorizontalPixel(rightPoint.x,x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToVerticalPixel(bottomPoint.y,x,y,xScale,yScale,Rotation)
			);
		

	if (ellipse_angle != 0)
		{
			xform.eM11 = (float)1.0;
			xform.eM12 = (float)0;
			xform.eM21 = (float)0;
			xform.eM22 = (float)1.0;
			xform.eDx = (float)0;
			xform.eDy = (float)0;

			SetWorldTransform(hdc, &xform);
		}
		SetGraphicsMode(hdc, nGraphicsMode);
		break;
	}
	

	case ENT_TEXT:
		/*PlotText(
			hdc,
			pEntityHeader,
			(PENTTEXT)pEntityData,
			x, y,
			xScale, yScale,
			Rotation
			);*/
		break;

	case ENT_ARC:
		Arc(
			hdc,
			m_pJetDxfDimView->UnitToHorizontalPixel(((PENTARC)pEntityData)->Point0.x - ((PENTARC)pEntityData)->Radius,x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToVerticalPixel(((PENTARC)pEntityData)->Point0.y - ((PENTARC)pEntityData)->Radius,x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToHorizontalPixel(((PENTARC)pEntityData)->Point0.x + ((PENTARC)pEntityData)->Radius,x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToVerticalPixel(((PENTARC)pEntityData)->Point0.y + ((PENTARC)pEntityData)->Radius,x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToHorizontalPixel(((PENTARC)pEntityData)->Point0.x + ((PENTARC)pEntityData)->Radius * cos(((PENTARC)pEntityData)->StartAngle*PI / 180),x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToVerticalPixel(((PENTARC)pEntityData)->Point0.y + ((PENTARC)pEntityData)->Radius * sin(((PENTARC)pEntityData)->StartAngle*PI / 180),x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToHorizontalPixel(((PENTARC)pEntityData)->Point0.x + ((PENTARC)pEntityData)->Radius * cos(((PENTARC)pEntityData)->EndAngle*PI / 180),x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToVerticalPixel(((PENTARC)pEntityData)->Point0.y + ((PENTARC)pEntityData)->Radius * sin(((PENTARC)pEntityData)->EndAngle*PI / 180),x,y,xScale,yScale,Rotation)
			);
		break;

	case ENT_SOLID:
		POINT	p[4];

		if (Rotation == 0)
		{
			p[0].x = m_pJetDxfDimView->UnitToHorizontalPixel(((PENTSOLID)pEntityData)->Point0.x);
			p[0].y = m_pJetDxfDimView->UnitToVerticalPixel(((PENTSOLID)pEntityData)->Point0.y);
			p[1].x = m_pJetDxfDimView->UnitToHorizontalPixel(((PENTSOLID)pEntityData)->Point1.x);
			p[1].y = m_pJetDxfDimView->UnitToVerticalPixel(((PENTSOLID)pEntityData)->Point1.y);
			p[3].x = m_pJetDxfDimView->UnitToHorizontalPixel(((PENTSOLID)pEntityData)->Point2.x);
			p[3].y = m_pJetDxfDimView->UnitToVerticalPixel(((PENTSOLID)pEntityData)->Point2.y);
			p[2].x = m_pJetDxfDimView->UnitToHorizontalPixel(((PENTSOLID)pEntityData)->Point3.x);
			p[2].y = m_pJetDxfDimView->UnitToVerticalPixel(((PENTSOLID)pEntityData)->Point3.y);
		}
		else
		{
			p[0].x = m_pJetDxfDimView->UnitToXRotationPixel(((PENTSOLID)pEntityData)->Point0.x, ((PENTSOLID)pEntityData)->Point0.y);
			p[0].y = m_pJetDxfDimView->UnitToYRotationPixel(((PENTSOLID)pEntityData)->Point0.x, ((PENTSOLID)pEntityData)->Point0.y);
			p[1].x = m_pJetDxfDimView->UnitToXRotationPixel(((PENTSOLID)pEntityData)->Point1.x, ((PENTSOLID)pEntityData)->Point1.y);
			p[1].y = m_pJetDxfDimView->UnitToYRotationPixel(((PENTSOLID)pEntityData)->Point1.x, ((PENTSOLID)pEntityData)->Point1.y);
			p[3].x = m_pJetDxfDimView->UnitToXRotationPixel(((PENTSOLID)pEntityData)->Point2.x, ((PENTSOLID)pEntityData)->Point2.y);
			p[3].y = m_pJetDxfDimView->UnitToYRotationPixel(((PENTSOLID)pEntityData)->Point2.x, ((PENTSOLID)pEntityData)->Point2.y);
			p[2].x = m_pJetDxfDimView->UnitToXRotationPixel(((PENTSOLID)pEntityData)->Point3.x, ((PENTSOLID)pEntityData)->Point3.y);
			p[2].y = m_pJetDxfDimView->UnitToYRotationPixel(((PENTSOLID)pEntityData)->Point3.x, ((PENTSOLID)pEntityData)->Point3.y);
		}

		brush = CreateSolidBrush(rgbcolor);
		oldbrush = (HBRUSH)SelectObject(hdc, brush);
		Polygon(hdc, p, 4);
		SelectObject(hdc, oldbrush);
		DeleteObject(brush);
		break;

	case ENT_INSERT:
	{
		BLOCKHEADER		BlockHeader;
		ENTITYHEADER	BlockEntityHeader;
		char			BlockEntityData[4096];
		OBJHANDLE       objHandle;
		BlockHeader.Objhandle = ((PENTINSERT)pEntityData)->BlockHeaderObjhandle;
		if (m_pJetDxfDimView->drwFindBlock_Direct(FIND_BYHANDLE, &BlockHeader) > 0)
		{
			//			LAYER	BlockLayer;
			//			Layer.Objhandle = BlockHeader.LayerObjhandle;
			//			drwFindTableType(hDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer);

			objHandle = m_pJetDxfDimView->drwFindEntity_Direct(CA2W(BlockHeader.Name), &BlockEntityHeader, &BlockEntityData, FIND_FIRST);
			//drwFindEntity(objHandle, CA2W(BlockHeader.Name), &BlockEntityHeader, &BlockEntityData, FIND_FIRST);
			if (objHandle > 0)
			{
				do{
					if (BlockEntityHeader.Color == 0)
						BlockEntityHeader.Color = color;

					PlotEntity(
						hdc,
						&BlockEntityHeader,
						&BlockEntityData,
						((PENTINSERT)pEntityData)->Point0.x*xScale + x,
						((PENTINSERT)pEntityData)->Point0.y*yScale + y,
						((PENTINSERT)pEntityData)->XScale*xScale,
						((PENTINSERT)pEntityData)->YScale*yScale,
						((PENTINSERT)pEntityData)->RotationAngle + Rotation
						);

					objHandle = m_pJetDxfDimView->drwFindEntity_Direct(CA2W(BlockHeader.Name), &BlockEntityHeader, &BlockEntityData, FIND_NEXT);
					//drwFindEntity(objHandle, CA2W(BlockHeader.Name), &BlockEntityHeader, &BlockEntityData, FIND_NEXT);
				} while (objHandle > 0);
			}
		}
		break;
	}
		

	case ENT_SPLINE:
	{
		UINT nKnot = ((PENTSPLINE)pEntityData)->SplineKnots;
		int nRank = ((PENTSPLINE)pEntityData)->SplineRank;
		UINT nControlPoint = ((PENTSPLINE)pEntityData)->SplineControlPoints;
		int nMatchPoint = ((PENTSPLINE)pEntityData)->SplineMatchPoints;

		COpenList<CDxf3dPoint> controlList;
		COpenList<CDxf3dPoint> fitList;
		COpenList<double> knotList;

		CDxf3dPoint pt;
		for (i = 0; i < nControlPoint; i++)
		{
			pt.x = (float)((PENTSPLINE)pEntityData)->pControlPoint[i].x;
			pt.y = (float)((PENTSPLINE)pEntityData)->pControlPoint[i].y;
			pt.z = 0.0;

			controlList.AddTail(pt);
		}

		double dbKnot = 0.0;
		for (i = 0; i < nKnot; i++)
		{
			dbKnot = ((PENTSPLINE)pEntityData)->pKnotPoint[i];
			knotList.AddTail(dbKnot);
		}

		COpenSpline splineCurve;
		splineCurve.SetDegree(nRank);
		splineCurve.SetControlPoints(controlList);
		splineCurve.SetKnotVector(knotList);

		REALPOINT ptReal;
		ptReal.x = 0.0;
		ptReal.y = 0.0;
		ptReal.z = 0.0;

		for (double fStep = 0.0; fStep <= nMatchPoint-1; fStep += 0.01)
		{
			pt = splineCurve.GetPointAt(fStep);
			ptReal.x = pt.x;
			ptReal.y = pt.y;
			ptReal.z = pt.z;


			SetPixel(
				hdc,
				m_pJetDxfDimView->UnitToHorizontalPixel(ptReal.x, x, y, xScale, yScale, Rotation),
				m_pJetDxfDimView->UnitToVerticalPixel(ptReal.y, x, y, xScale, yScale, Rotation),
				rgbcolor
				);
		}
		break;
	}

	case ENT_LWPOLYLINE:
	{
		double lwpolyline_x1 = 0.0;
		double lwpolyline_y1 = 0.0;
		double lwpolyline_x2 = 0.0;
		double lwpolyline_y2 = 0.0;

		for (i = 0; i < ((PENTLWPOLYLINE)pEntityData)->CountOfVertex - 1; i++)
		{
			lwpolyline_x1 = ((PENTLWPOLYLINE)pEntityData)->pVertexPoint[i].x;
			lwpolyline_y1 = ((PENTLWPOLYLINE)pEntityData)->pVertexPoint[i].y;
			lwpolyline_x2 = ((PENTLWPOLYLINE)pEntityData)->pVertexPoint[i + 1].x;
			lwpolyline_y2 = ((PENTLWPOLYLINE)pEntityData)->pVertexPoint[i + 1].y;

			DrawLine(hdc, rgbcolor,
				m_pJetDxfDimView->UnitToXRotationPixel(lwpolyline_x1, lwpolyline_y1,x,y,xScale,yScale,Rotation),
				m_pJetDxfDimView->UnitToYRotationPixel(lwpolyline_x1, lwpolyline_y1,x,y,xScale,yScale,Rotation),
				m_pJetDxfDimView->UnitToXRotationPixel(lwpolyline_x2, lwpolyline_y2,x,y,xScale,yScale,Rotation),
				m_pJetDxfDimView->UnitToYRotationPixel(lwpolyline_x2, lwpolyline_y2,x,y,xScale,yScale,Rotation));
		}

		if (((PENTLWPOLYLINE)pEntityData)->b_PolylineClose)
		{
			lwpolyline_x1 = ((PENTLWPOLYLINE)pEntityData)->pVertexPoint[0].x;
			lwpolyline_y1 = ((PENTLWPOLYLINE)pEntityData)->pVertexPoint[0].y;
			lwpolyline_x2 = ((PENTLWPOLYLINE)pEntityData)->pVertexPoint[((PENTLWPOLYLINE)pEntityData)->CountOfVertex - 1].x;
			lwpolyline_y2 = ((PENTLWPOLYLINE)pEntityData)->pVertexPoint[((PENTLWPOLYLINE)pEntityData)->CountOfVertex - 1].y;

			DrawLine(hdc, rgbcolor,
				m_pJetDxfDimView->UnitToXRotationPixel(lwpolyline_x1, lwpolyline_y1,x,y,xScale,yScale,Rotation),
				m_pJetDxfDimView->UnitToYRotationPixel(lwpolyline_x1, lwpolyline_y1,x,y,xScale,yScale,Rotation),
				m_pJetDxfDimView->UnitToXRotationPixel(lwpolyline_x2, lwpolyline_y2,x,y,xScale,yScale,Rotation),
				m_pJetDxfDimView->UnitToYRotationPixel(lwpolyline_x2, lwpolyline_y2,x,y,xScale,yScale,Rotation));
		}

		/*if (NULL != ((PENTLWPOLYLINE)pEntityData)->pVertexPoint)
		{
			delete[]((PENTLWPOLYLINE)pEntityData)->pVertexPoint;
			((PENTLWPOLYLINE)pEntityData)->pVertexPoint = NULL;
		}*/
		break;
	}

	case ENT_POLYLINE:
		MoveToEx(
			hdc,
			m_pJetDxfDimView->UnitToHorizontalPixel(((PENTPOLYLINE)pEntityData)->pVertex[0].Point.x,x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToVerticalPixel(((PENTPOLYLINE)pEntityData)->pVertex[0].Point.y,x,y,xScale,yScale,Rotation),
			NULL
			);
		for (i = 1; i < ((PENTPOLYLINE)pEntityData)->nVertex; i++)
		{
			if (int(((PENTPOLYLINE)pEntityData)->pVertex[i].Bulge * 100) == 0)
				LineTo(
				hdc,
				m_pJetDxfDimView->UnitToHorizontalPixel(((PENTPOLYLINE)pEntityData)->pVertex[i].Point.x,x,y,xScale,yScale,Rotation),
				m_pJetDxfDimView->UnitToVerticalPixel(((PENTPOLYLINE)pEntityData)->pVertex[i].Point.y,x,y,xScale,yScale,Rotation)
				);
			else
			{ // Bulge is greater than zero
				double a1, a2, a3, a4;
				double x1, y1;
				double x2, y2;
				double x3, y3;
				double x4, y4;
				double xm, ym;
				double xc, yc, r;
				double bulge = ((PENTPOLYLINE)pEntityData)->pVertex[i].Bulge;

				x1 = ((PENTPOLYLINE)pEntityData)->pVertex[i].Point.x;
				x2 = ((PENTPOLYLINE)pEntityData)->pVertex[i - 1].Point.x;
				y1 = ((PENTPOLYLINE)pEntityData)->pVertex[i].Point.y;
				y2 = ((PENTPOLYLINE)pEntityData)->pVertex[i - 1].Point.y;
				a1 = (y1 - y2) / (x1 - x2);
				a2 = a1 - bulge * 2;
				a3 = -1 / a2;
				a4 = -1 / a1;

				xm = (x1 + x2) / 2;
				ym = (y1 + y2) / 2;

				xc = (ym - y1 + a3*x1 - a4*xm) / (a3 - a4);
				yc = a3*(xc - x1) + y1;
				r = sqrt((x1 - xc)*(x1 - xc) + (y1 - yc)*(y1 - yc));

				x3 = xc - r;
				y3 = yc - r;
				x4 = xc + r;
				y4 = yc + r;

				Arc(
					hdc,
					m_pJetDxfDimView->UnitToXRotationPixel(x3, y3,x,y,xScale,yScale,Rotation),
					m_pJetDxfDimView->UnitToYRotationPixel(x3, y3,x,y,xScale,yScale,Rotation),
					m_pJetDxfDimView->UnitToXRotationPixel(x4, y4,x,y,xScale,yScale,Rotation),
					m_pJetDxfDimView->UnitToYRotationPixel(x4, y4,x,y,xScale,yScale,Rotation),
					m_pJetDxfDimView->UnitToXRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
					m_pJetDxfDimView->UnitToYRotationPixel(x1, y1,x,y,xScale,yScale,Rotation),
					m_pJetDxfDimView->UnitToXRotationPixel(x2, y2,x,y,xScale,yScale,Rotation),
					m_pJetDxfDimView->UnitToYRotationPixel(x2, y2,x,y,xScale,yScale,Rotation)
					);
				/*				Ellipse(
								hdc,
								U2PXROT(x3,y3),
								U2PYROT(x3,y3),
								U2PXROT(x4,y4),
								U2PYROT(x4,y4)
								);*/
			}
		}
		if (((PENTPOLYLINE)pEntityData)->Flag & POLYLINE_CLOSED)
			LineTo(
			hdc,
			m_pJetDxfDimView->UnitToHorizontalPixel(((PENTPOLYLINE)pEntityData)->pVertex[0].Point.x,x,y,xScale,yScale,Rotation),
			m_pJetDxfDimView->UnitToVerticalPixel(((PENTPOLYLINE)pEntityData)->pVertex[0].Point.y,x,y,xScale,yScale,Rotation)
			);
		break;

	case ENT_DIMENSION:
		BLOCKHEADER		DimBlockHeader;
		ENTITYHEADER	DimBlockEntityHeader;
		char			DimBlockEntityData[4096];
		DimBlockHeader.Objhandle = ((PENTDIMENSION)pEntityData)->BlockheaderObjhandle;
		if (m_pJetDxfDimView->drwFindBlock_Direct(FIND_BYHANDLE, &DimBlockHeader) > 0)
		{
			//			LAYER	BlockLayer;
			//			Layer.Objhandle = BlockHeader.LayerObjhandle;
			//			drwFindTableType(hDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer);

			if (m_pJetDxfDimView->drwFindEntity_Direct(CA2W(DimBlockHeader.Name), &DimBlockEntityHeader, &DimBlockEntityData, FIND_FIRST) > 0)
			{
				do{
					if (DimBlockEntityHeader.Color == 0)
						DimBlockEntityHeader.Color = color;

					PlotEntity(
						hdc,
						&DimBlockEntityHeader,
						&DimBlockEntityData,
						0 + x,
						0 + y,
						1 * xScale,
						1 * yScale,
						0 + Rotation
						);
				} while (m_pJetDxfDimView->drwFindEntity_Direct(CA2W(DimBlockHeader.Name), &DimBlockEntityHeader, &DimBlockEntityData, FIND_NEXT) > 0);
			}
		}
		break;
	}

	if (pEntityHeader->EntityType != ENT_TEXT)
	{
		SelectObject(hdc, oldpen);
		DeleteObject(pen);
	}
}


VOID CJetDxfView::drwFindEntity(OBJHANDLE &objhandle, LPCTSTR strBlockName, PENTITYHEADER pEntityHeader, LPVOID pEntityData, DWORD dwFindType, DWORD* pdwEntityPos)
{
	objhandle = m_pJetDxfDimView->drwFindEntity_Direct(strBlockName, pEntityHeader, pEntityData, dwFindType, pdwEntityPos);
}

/*-------------------------------------------------------------------*
|  drwPlot                                                          |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      HDC hdc= handle of DC for drawing                            |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::drwPlot(HDC hdc)
{
	ENTITYHEADER	EntityHeader;
	char			EntityData[4096];

	// Initialize pDrawing ------------------
	if (NULL == m_pJetDxfDimView->m_pDrawing)
		return FALSE;

	if (m_pJetDxfDimView->m_pDrawing->View.Viewable)
	{
		// Entities Section ----------------------------------
		if (m_pJetDxfDimView->drwFindEntity_Direct(NULL, &EntityHeader, &EntityData, FIND_FIRST) > 0)
		{
			do{
				PlotEntity(hdc, &EntityHeader, &EntityData);
			} while (m_pJetDxfDimView->drwFindEntity_Direct(NULL, &EntityHeader, &EntityData, FIND_NEXT) > 0);
		}
	}

	// UnInitilize pDrawing -----------------
	return true;
}

/*-------------------------------------------------------------------*
|  drwInitView                                                      |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      int x = horizontal position of drawing window                |
|      int y = vertical position of drawing window                  |
|      int Width = width of drawing window                          |
|      int Height = height of drawing window                        |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::drwInitView(int x, int y, int Width, int Height)
{

	// Initialize pDrawing ------------------
	if (NULL == m_pJetDxfDimView->m_pDrawing)
		return FALSE;

	m_pJetDxfDimView->m_pDrawing->View.ZoomLevel = 1;
	m_pJetDxfDimView->m_pDrawing->View.PPU = 20;

	m_pJetDxfDimView->m_pDrawing->View.WindowLeft = x;
	m_pJetDxfDimView->m_pDrawing->View.WindowTop = y;
	m_pJetDxfDimView->m_pDrawing->View.WindowRight = x + Width;
	m_pJetDxfDimView->m_pDrawing->View.WindowBottom = y + Height;
	m_pJetDxfDimView->m_pDrawing->View.ViewLeft = 0;
	m_pJetDxfDimView->m_pDrawing->View.ViewBottom = 0;

	// UnInitilize pDrawing -----------------
	return TRUE;
}

/*-------------------------------------------------------------------*
|  drwPaint                                                         |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|	    HDC hdc	= handle to view window DC                           |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::drwPaint(HDC hdc)
{
	HRGN		ViewRgn;
	BOOL		result;

	// Initialize pDrawing ------------------
	if (NULL == m_pJetDxfDimView->m_pDrawing)
		return FALSE;

	SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	Rectangle(
		hdc,
		m_pJetDxfDimView->m_pDrawing->View.WindowLeft,
		m_pJetDxfDimView->m_pDrawing->View.WindowTop,
		m_pJetDxfDimView->m_pDrawing->View.WindowRight,
		m_pJetDxfDimView->m_pDrawing->View.WindowBottom
		);

	ViewRgn = CreateRectRgn(
		m_pJetDxfDimView->m_pDrawing->View.WindowLeft,
		m_pJetDxfDimView->m_pDrawing->View.WindowTop,
		m_pJetDxfDimView->m_pDrawing->View.WindowRight,
		m_pJetDxfDimView->m_pDrawing->View.WindowBottom
		);
	SelectObject(hdc, ViewRgn);

	SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
	result = drwPlot(hdc);

	DeleteObject(ViewRgn);

	return result;
}

/********************************************************************
* Functions for changing view properties                           *
********************************************************************/

/*-------------------------------------------------------------------*
|  drwGetViewProperties                                             |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      PVIEW pView = pointer to view properties structure           |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::drwGetViewProperties(PVIEW pView)
{
	if (NULL == m_pJetDxfDimView->m_pDrawing)
		return FALSE;

	memcpy(pView, &m_pJetDxfDimView->m_pDrawing->View, sizeof(VIEW));

	return TRUE;
}

/*-------------------------------------------------------------------*
|  drwSetViewProperties                                             |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      PVIEW pView = pointer to view properties structure           |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::drwSetViewProperties(PVIEW pView)
{

	if (NULL == m_pJetDxfDimView->m_pDrawing)
		return FALSE;

	memcpy(&m_pJetDxfDimView->m_pDrawing->View, pView, sizeof(VIEW));

	m_pJetDxfDimView->m_pDrawing->View.ViewTop = m_pJetDxfDimView->m_pDrawing->View.ViewBottom + \
		(m_pJetDxfDimView->m_pDrawing->View.WindowBottom - m_pJetDxfDimView->m_pDrawing->View.WindowTop) / m_pJetDxfDimView->m_pDrawing->View.ZoomLevel / m_pJetDxfDimView->m_pDrawing->View.PPU;
	m_pJetDxfDimView->m_pDrawing->View.ViewRight = m_pJetDxfDimView->m_pDrawing->View.ViewLeft + \
		(m_pJetDxfDimView->m_pDrawing->View.WindowRight - m_pJetDxfDimView->m_pDrawing->View.WindowLeft) / m_pJetDxfDimView->m_pDrawing->View.ZoomLevel / m_pJetDxfDimView->m_pDrawing->View.PPU;

	return TRUE;
}

/********************************************************************
* Zoom Functions                                                   *
********************************************************************/

/*-------------------------------------------------------------------*
|  drwGetDrawingBorder                                              |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      PREALRECT pRect = pointer to drawing boundary rect           |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::drwGetDrawingBorder(PREALRECT pRect)
{
	ENTITYHEADER	EntityHeader;
	char			EntityData[4096];
	REALRECT		Rect;
	double			x1 = DBL_MAX;
	double			y1 = DBL_MIN;
	double			x2 = DBL_MIN;
	double			y2 = DBL_MAX;

	// Initialize pDrawing ------------------
	if (NULL == m_pJetDxfDimView->m_pDrawing)
		return NULL;

	//	if(pDrawing->View.Viewable)
	{
		// Entities Section ----------------------------------
		if (m_pJetDxfDimView->drwFindEntity_Direct(NULL, &EntityHeader, &EntityData, FIND_FIRST) > 0)
		{
			do{
				if (GetEntityRect(&EntityHeader, EntityData, &Rect))
				{
					y1 = __max(Rect.top, y1);
					x1 = __min(Rect.left, x1);
					y2 = __min(Rect.bottom, y2);
					x2 = __max(Rect.right, x2);
				}
			} while (m_pJetDxfDimView->drwFindEntity_Direct(NULL, &EntityHeader, &EntityData, FIND_NEXT) > 0);
		}
	}

	pRect->left = x1;
	pRect->right = x2;
	pRect->bottom = y2;
	pRect->top = y1;


	borderRect.left = x1;
	borderRect.right = x2;
	borderRect.top = y1;
	borderRect.bottom = y2;
	return TRUE;
}

/*-------------------------------------------------------------------*
|  drwZoomExtents                                                   |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfView::drwZoomExtents()
{
	REALRECT		rect;
	double			ZoomLevelX = 0.0;
	double          ZoomLevelY = 0.0;

	// Initialize pDrawing ------------------
	if (NULL == m_pJetDxfDimView->m_pDrawing)
		return NULL;

	drwGetDrawingBorder(&rect);

	// Change Drawing View Properties
	ZoomLevelX = (m_pJetDxfDimView->m_pDrawing->View.WindowBottom - m_pJetDxfDimView->m_pDrawing->View.WindowTop - 20) / \
		((rect.top - rect.bottom)*m_pJetDxfDimView->m_pDrawing->View.PPU);

	ZoomLevelY = (m_pJetDxfDimView->m_pDrawing->View.WindowRight - m_pJetDxfDimView->m_pDrawing->View.WindowLeft - 20) / \
		((rect.right - rect.left)*m_pJetDxfDimView->m_pDrawing->View.PPU);

	m_pJetDxfDimView->m_pDrawing->View.ZoomLevel = (ZoomLevelX < ZoomLevelY ? ZoomLevelX : ZoomLevelY);

	m_pJetDxfDimView->m_pDrawing->View.ViewBottom = rect.bottom - ((m_pJetDxfDimView->m_pDrawing->View.WindowBottom - \
		m_pJetDxfDimView->m_pDrawing->View.WindowTop) / (m_pJetDxfDimView->m_pDrawing->View.ZoomLevel*\
		m_pJetDxfDimView->m_pDrawing->View.PPU) - (rect.top - rect.bottom)) / 2;

	m_pJetDxfDimView->m_pDrawing->View.ViewLeft = rect.left - ((m_pJetDxfDimView->m_pDrawing->View.WindowRight - m_pJetDxfDimView->m_pDrawing->View.WindowLeft) / \
		(m_pJetDxfDimView->m_pDrawing->View.ZoomLevel*m_pJetDxfDimView->m_pDrawing->View.PPU) - (rect.right - rect.left)) / 2;

	m_pJetDxfDimView->m_pDrawing->View.ViewTop = m_pJetDxfDimView->m_pDrawing->View.ViewBottom + \
		(m_pJetDxfDimView->m_pDrawing->View.WindowBottom - m_pJetDxfDimView->m_pDrawing->View.WindowTop) / \
		m_pJetDxfDimView->m_pDrawing->View.ZoomLevel / m_pJetDxfDimView->m_pDrawing->View.PPU;

	m_pJetDxfDimView->m_pDrawing->View.ViewRight = m_pJetDxfDimView->m_pDrawing->View.ViewLeft + (m_pJetDxfDimView->m_pDrawing->View.WindowRight - \
		m_pJetDxfDimView->m_pDrawing->View.WindowLeft) / m_pJetDxfDimView->m_pDrawing->View.ZoomLevel / m_pJetDxfDimView->m_pDrawing->View.PPU;

	return TRUE;
}


BOOL CJetDxfView::SetZoomLevel(double dbZoomLevel)
{
	VIEW drwview;
	if (drwGetViewProperties(&drwview))
	{
		double cx;
		double cy;
		cx = drwview.ViewLeft + ((drwview.WindowRight - drwview.WindowLeft) / 2)*(1 / (drwview.PPU*drwview.ZoomLevel));
		cy = drwview.ViewBottom + ((drwview.WindowBottom - drwview.WindowTop) / 2)*(1 / (drwview.PPU*drwview.ZoomLevel));

		drwview.ZoomLevel = dbZoomLevel;
		drwview.ViewLeft = cx - ((drwview.WindowRight - drwview.WindowLeft) / 2)*(1 / (drwview.PPU*drwview.ZoomLevel));
		drwview.ViewBottom = cy - ((drwview.WindowBottom - drwview.WindowTop) / 2)*(1 / (drwview.PPU*drwview.ZoomLevel));
		if (drwSetViewProperties(&drwview))
			return TRUE;
	}
	return FALSE;
}

double CJetDxfView::GetZoomLevel()
{
	VIEW drwview;
	if (drwGetViewProperties(&drwview))
		return drwview.ZoomLevel;
	else
		return 0;
}