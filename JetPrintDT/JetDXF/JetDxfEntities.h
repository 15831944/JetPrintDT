#pragma once
#ifndef __JET_DXF_ENTITIES_H
#define __JET_DXF_ENTITIES_H
#include "JetDxfBase.h"

class CJetDxfEntities :
	public CJetDxfBase
{
public:
	CJetDxfEntities(CJetDxfRead *pDxfRead);
	virtual ~CJetDxfEntities();
	bool dxfReadData(HDXF hDxf);
	bool dxfWriteData(HDXF hDxf);

	virtual bool dxfReadDataDirect(PDXFENTITYHEADER pEntityHeader,LPVOID pEntityData);

	void SetEntityHeader(PDXFENTITYHEADER pEntityHeader);


private:
	BOOL ReadArcData(PDXFENTITYHEADER pEntityHeader, PDXFENTARC pArc);
	BOOL ReadCircleData(PDXFENTITYHEADER pEntityHeader, PDXFENTCIRCLE pCircle);
	BOOL ReadDimensionData(PDXFENTITYHEADER pEntityHeader, PDXFENTDIMENSION pDimension);
	BOOL ReadEllipseData(PDXFENTITYHEADER pEntityHeader, PDXFENTELLIPSE pEllipse);
	BOOL ReadInsertData(PDXFENTITYHEADER pEntityHeader, PDXFENTINSERT pInsert);
	BOOL ReadLineData(PDXFENTITYHEADER pEntityHeader, PDXFENTLINE pLine);
	BOOL ReadPointData(PDXFENTITYHEADER pEntityHeader, PDXFENTPOINT pPoint);
	BOOL ReadPolyLineData(PDXFENTITYHEADER pEntityHeader, PDXFENTPOLYLINE pPolyLine);
	BOOL ReadSolidData(PDXFENTITYHEADER pEntityHeader, PDXFENTSOLID pSolid);
	BOOL ReadTextData(PDXFENTITYHEADER pEntityHeader, PDXFENTTEXT pText);
	BOOL ReadSplineData(PDXFENTITYHEADER pEntityHeader, PDXFENTSPLINE pSpline);
	BOOL ReadLWPolylineData(PDXFENTITYHEADER pEntityHeader, PDXFENTLWPOLYLINE pLWPolyline);
public:
	PDXFENTITYHEADER m_pDxfEntityHeader;
	LPVOID  pEntityData;
	int		GCode;
	LPTSTR  m_pReadString;
};

#endif //__JET_DXF_ENTITIES_H



