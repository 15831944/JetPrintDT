#ifndef __JET_DXF_WRITE_H
#define __JET_DXF_WRITE_H

#include "JetDXFDef.h"
class CJetDxfBaseView;

class CJetDxfWrite
{
public:
	CJetDxfWrite();
	~CJetDxfWrite();

	BOOL InitJetWriteView(CJetDxfBaseView *pJetDxfBaseView);
	BOOL InitJetWriteOffset(const double& xOffset,const double &yOffset);
	BOOL InitJetWriteZoomFactor(const double &xzoomFactor,const double &yzoomFactor);
	BOOL WriteFile();
	BOOL WriteEntities(PENTITYHEADER pEntityHeader, LPVOID pEntityData,double xOffset = 0.0,double yOffset = 0.0,double xScale = 1.0,double yScale = 1.0,double rotation = 0.0);
	BOOL WriteLine(PENTLINE pEntLine,double xOffset = 0.0,double yOffset = 0.0,double xScale = 1.0,double yScale = 1.0,double rotation = 0.0);
	BOOL WriteCircle(PENTCIRCLE pEntCircle,double xOffset = 0.0,double yOffset = 0.0,double xScale = 1.0,double yScale = 1.0,double rotation = 0.0);
	BOOL WriteArc(PENTARC pEntArc,double xOffset = 0.0,double yOffset = 0.0,double xScale = 1.0,double yScale = 1.0,double rotation = 0.0);
	BOOL WriteLWPolyline(PENTLWPOLYLINE pEntLWPolyline,double xOffset = 0.0,double yOffset = 0.0,double xScale = 1.0,double yScale = 1.0,double rotation = 0.0);
	BOOL WriteSpline(PENTSPLINE pEntSpline,double xOffset = 0.0,double yOffset = 0.0,double xScale = 1.0,double yScale = 1.0,double rotation = 0.0);
	BOOL WritePoint(PENTPOINT pEntPoint,double xOffset = 0.0,double yOffset = 0.0,double xScale = 1.0,double yScale = 1.0,double rotation = 0.0);
	BOOL dxfCreateFile(LPCTSTR	FileName);

	BOOL dxfCloseFile();

	BOOL WriteParamString(LPCTSTR value);

private:
	double WriteCaculateXPos(double xOrign,
		                     double yOrign,
							 double xOffset = 0.0,
							 double yOffset = 0.0,
							 double xScale = 0.0,
							 double yScale = 0.0,
							 double rotation = 0.0);

	double WriteCaculateYPos(double xOrign,
							 double yOrign,
							 double xOffset = 0.0,
							 double yOffset = 0.0,
							 double xScale = 0.0,
							 double yScale = 0.0,
							 double rotation = 0.0);
public:
	FILE            *pStream;
	CJetDxfBaseView *m_pJetDxfBaseView;
	

private:
	double           db_XZoomFactor;
	double           db_YZoomFactor;
	double           db_XOffset;
	double           db_YOffSet;
};


#endif //__JET_DXF_WEITE_H

