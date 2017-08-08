
#include "JetDxfWrite.h"
#include "JetDxfBaseView.h"

CJetDxfWrite::CJetDxfWrite() :m_pJetDxfBaseView(NULL)
{
	pStream = NULL;
	db_XOffset = 0.0;
	db_YOffSet = 0.0;
	db_XZoomFactor = 1.0;
	db_YZoomFactor = 1.0;
}


CJetDxfWrite::~CJetDxfWrite()
{
}

BOOL CJetDxfWrite::InitJetWriteOffset(const double& xOffset,const double &yOffset)
{
	db_XOffset = xOffset;
	db_YOffSet = yOffset;

	return TRUE;
}

BOOL CJetDxfWrite::InitJetWriteZoomFactor(const double &xzoomFactor,const double &yzoomFactor)
{
	if(xzoomFactor == 0.0)
		db_XZoomFactor = 1.0;
	else{
		db_XZoomFactor = xzoomFactor < 0.0 ? (-xzoomFactor) : xzoomFactor;
	}
	if (yzoomFactor == 0.0)
		db_YZoomFactor = 1.0;
	else{
		db_YZoomFactor = yzoomFactor < 0.0 ? (-yzoomFactor) : yzoomFactor;
	}

	
	return TRUE;
}


BOOL CJetDxfWrite::InitJetWriteView(CJetDxfBaseView *pJetDxfBaseView)
{
	if (NULL == pJetDxfBaseView)
	{
		m_pJetDxfBaseView = NULL;
		return FALSE;
	}

	m_pJetDxfBaseView = pJetDxfBaseView;
	return TRUE;
}

BOOL CJetDxfWrite::WriteFile()
{
	BOOL bResult = FALSE;
	CString strWriteLine = _T("");
	ENTITYHEADER	EntityHeader;
	char			EntityData[4096];

	memset(EntityData, 0, 4096);

	if (NULL == m_pJetDxfBaseView)
	{
		bResult = FALSE;
		return bResult;
	}

	if (NULL == pStream)
	{
		bResult = FALSE;
		return FALSE;
	}

	//Write Prog Header
	strWriteLine.Format(_T("%s"),_T("PROGRAM"));
	bResult = WriteParamString(strWriteLine);
	if(FALSE == bResult)
	{
		return bResult;
	}

	//Write Acc And Dec
	strWriteLine.Format(_T("ACC 2000 DEC 2000 STP20000 JRK20000"));
	bResult = WriteParamString(strWriteLine);
	if(FALSE == bResult)
	{
		return bResult;
	}

	if (m_pJetDxfBaseView->drwFindEntity_Direct(NULL, &EntityHeader, &EntityData, FIND_FIRST) > 0)
	{
		do{
			WriteEntities(&EntityHeader, &EntityData);
		} while (m_pJetDxfBaseView->drwFindEntity_Direct(NULL, &EntityHeader, &EntityData, FIND_NEXT) > 0);
	}
	else
	{
		bResult = FALSE;
		return bResult;
	}

	//Write Prog End
	strWriteLine.Format(_T("%s"),_T("ENDP"));
	bResult = WriteParamString(strWriteLine);
	if(FALSE == bResult)
	{
		return bResult;
	}
	bResult = TRUE;

	return bResult;

}

BOOL CJetDxfWrite::WriteEntities(PENTITYHEADER pEntityHeader, LPVOID pEntityData,double xOffset,double yOffset,double xScale,double yScale,double rotation)
{
	BOOL bResult = FALSE;

	if (pEntityHeader->Deleted)
	{
		return FALSE;
	}

	switch (pEntityHeader->EntityType)
	{
	case ENT_POINT:
		bResult = WritePoint((PENTPOINT)pEntityData,xOffset,yOffset,xScale,yScale,rotation);
		break;

	case ENT_LINE:
		bResult = WriteLine((PENTLINE)pEntityData,xOffset,yOffset,xScale,yScale,rotation);
		break;

	case ENT_ELLIPSE:
		break;

	case ENT_CIRCLE:
		bResult = WriteCircle((PENTCIRCLE)pEntityData,xOffset,yOffset,xScale,yScale,rotation);
		break;

	case ENT_LWPOLYLINE:
		bResult = WriteLWPolyline((PENTLWPOLYLINE)pEntityData,xOffset,yOffset,xScale,yScale,rotation);
		break;

	case ENT_SPLINE:
		bResult = WriteSpline((PENTSPLINE)pEntityData,xOffset,yOffset,xScale,yScale,rotation);
		break;

	case ENT_ARC:
		bResult = WriteArc((PENTARC)pEntityData,xOffset,yOffset,xScale,yScale,rotation);
		break;

	case ENT_INSERT:
		{
			BLOCKHEADER		BlockHeader;
			ENTITYHEADER	BlockEntityHeader;
			char			BlockEntityData[4096];
			OBJHANDLE       objHandle;
			BlockHeader.Objhandle = ((PENTINSERT)pEntityData)->BlockHeaderObjhandle;

			if (m_pJetDxfBaseView->drwFindBlock_Direct(FIND_BYHANDLE, &BlockHeader) > 0)
			{
				//			LAYER	BlockLayer;
				//			Layer.Objhandle = BlockHeader.LayerObjhandle;
				//			drwFindTableType(hDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer);

				objHandle = m_pJetDxfBaseView->drwFindEntity_Direct(CA2W(BlockHeader.Name), &BlockEntityHeader, &BlockEntityData, FIND_FIRST);
				//drwFindEntity(objHandle, CA2W(BlockHeader.Name), &BlockEntityHeader, &BlockEntityData, FIND_FIRST);
				if (objHandle > 0)
				{
					do{

						WriteEntities(
							&BlockEntityHeader,
							&BlockEntityData,
							((PENTINSERT)pEntityData)->Point0.x*xScale + xOffset,
							((PENTINSERT)pEntityData)->Point0.y*yScale + yOffset,
							((PENTINSERT)pEntityData)->XScale*xScale,
							((PENTINSERT)pEntityData)->YScale*yScale,
							((PENTINSERT)pEntityData)->RotationAngle + rotation
							);

						objHandle = m_pJetDxfBaseView->drwFindEntity_Direct(CA2W(BlockHeader.Name), &BlockEntityHeader, &BlockEntityData, FIND_NEXT);
						//drwFindEntity(objHandle, CA2W(BlockHeader.Name), &BlockEntityHeader, &BlockEntityData, FIND_NEXT);
					} while (objHandle > 0);
				}
			}

			break;
		}


	default:
		//其他的不要往文件之中写
		break;
	}

	return bResult;
}

BOOL CJetDxfWrite::WriteParamString(LPCTSTR	value)
{
	if (NULL == pStream)
		return false;

	if (fprintf(pStream, "%s\n",CW2A(value)) <= 0)
	{
		// file write error
		return FALSE;
	}

	return TRUE;
}

BOOL CJetDxfWrite::WriteSpline(PENTSPLINE pEntSpline,double xOffset,double yOffset,double xScale,double yScale,double rotation)
{
	BOOL bResult = FALSE;
	int nIndexOfControlPoint = 0;
	int nKnot = pEntSpline->SplineKnots;
	int nControlPoint = pEntSpline->SplineControlPoints;
	REALPOINT *pControlPoint = new REALPOINT[nControlPoint];
	if (NULL == pControlPoint)
	{
		bResult = FALSE;
		return bResult;
	}

	double *pKnot = new double[nKnot];
	if (NULL == pKnot)
	{
		bResult = FALSE;
		return bResult;
	}

	memset(pControlPoint, 0, sizeof(REALPOINT)*nControlPoint);
	memset(pKnot, 0, sizeof(double)*nKnot);

	for (int i = 0; i < nControlPoint; i++)
	{
		pControlPoint[i].x = pEntSpline->pControlPoint[i].x;
		pControlPoint[i].y = pEntSpline->pControlPoint[i].y;
		pControlPoint[i].z = 0.0;

		pControlPoint[i].x *= db_XZoomFactor;
		pControlPoint[i].y *= db_YZoomFactor;

		pControlPoint[i].x *= -1;
		pControlPoint[i].y *= -1;

		pControlPoint[i].x += db_XOffset;
		pControlPoint[i].y += db_YOffSet;

		

	}

	for (int j = 0; j < nKnot; j++)
	{
		pKnot[j] = pEntSpline->pKnotPoint[j];
	}

	CString strWrite = _T("");
	//移动至起始点
	strWrite.Format(_T("X %f Y %f\n"), pControlPoint[nIndexOfControlPoint].x, pControlPoint[nIndexOfControlPoint].y);
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"), _T("REM SPLINE"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"),_T("SET 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("NURB RANK %d\n"), pEntSpline->SplineRank);
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"), _T("NURB MODE 1"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	do 
	{

		strWrite.Format(_T("K %f X%f Y%f\n"), pKnot[nIndexOfControlPoint], pControlPoint[nIndexOfControlPoint].x,pControlPoint[nIndexOfControlPoint].y);
		bResult = WriteParamString(strWrite);
		if (FALSE == bResult)
		{
			return bResult;
		}

		nIndexOfControlPoint++;
	} while (nIndexOfControlPoint<nControlPoint);

	do 
	{
		strWrite.Format(_T("K %f \n"), pKnot[nIndexOfControlPoint]);
		bResult = WriteParamString(strWrite);
		if (FALSE == bResult)
		{
			return bResult;
		}

		nIndexOfControlPoint++;
	} while (nIndexOfControlPoint<nKnot);

	//End Of Nurb 
	strWrite.Format(_T("K %d\n"), -1);
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"),_T("CLR 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}
	//End Of Spline Block
	strWrite.Format(_T("REM ENDENTITY\n"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	if (NULL != pControlPoint)
	{
		delete[] pControlPoint;
		pControlPoint = NULL;
	}

	if (NULL != pKnot)
	{
		delete[] pKnot;
		pKnot = NULL;
	}

	bResult = TRUE;
	return bResult;
}

BOOL CJetDxfWrite::WriteLWPolyline(PENTLWPOLYLINE pEntLWPolyline,double xOffset,double yOffset,double xScale,double yScale,double rotation)
{
	BOOL bResult = FALSE;
	bool bClosed = pEntLWPolyline->b_PolylineClose;
	int nVertexIndex = 0;
	int nVertexCount = pEntLWPolyline->CountOfVertex;
	REALPOINT *pVertex = new REALPOINT[nVertexCount];
	if (NULL == pVertex)
	{
		bResult = FALSE;
		return bResult;
	}
	memset(pVertex, 0, nVertexCount*sizeof(REALPOINT));
	for (int i = 0; i < nVertexCount; i++)
	{
		double dbXtemp = WriteCaculateXPos(pEntLWPolyline->pVertexPoint[i].x,pEntLWPolyline->pVertexPoint[i].y,xOffset,yOffset,xScale,yScale,rotation) ;
		double dbYtemp = WriteCaculateYPos(pEntLWPolyline->pVertexPoint[i].x,pEntLWPolyline->pVertexPoint[i].y,xOffset,yOffset,xScale,yScale,rotation);
		dbXtemp *= db_XZoomFactor;
		dbYtemp *= db_YZoomFactor;
		dbXtemp *= -1;
		dbYtemp *= -1;

		pVertex[i].x = dbXtemp  + db_XOffset;
		pVertex[i].y = dbYtemp  + db_YOffSet;
	}

	CString strWrite = _T("");
	//移动至起始点
	strWrite.Format(_T("%s\n"),_T("CLR 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("X %f Y %f\n"), pVertex[nVertexIndex].x, pVertex[nVertexIndex].y);
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"), _T("REM LWPOLYLINE"));
	bResult = WriteParamString(strWrite);
	if (!bResult)
	{
		return bResult;
	}


	//等待一下
	strWrite.Format(_T("INH -516"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	//置位
	strWrite.Format(_T("%s\n"),_T("SET 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	do 
	{

		strWrite.Format(_T("X %f Y %f\n"), pVertex[nVertexIndex].x, pVertex[nVertexIndex].y);
		bResult = WriteParamString(strWrite);
		if (FALSE == bResult)
		{
			return bResult;
		}
		++nVertexIndex;
	} while (nVertexIndex<nVertexCount);

	//如果是关闭的
	if(bClosed)
	{
		strWrite.Format(_T("X %f Y %f\n"), pVertex[0].x, pVertex[0].y);
		bResult = WriteParamString(strWrite);
		if (FALSE == bResult)
		{
			return bResult;
		}
	}

	//在置位之前，移动至最后一个点
	if(bClosed)
	{
		strWrite.Format(_T("X %fY %f\n"), pVertex[0].x, pVertex[0].y);
		bResult = WriteParamString(strWrite);
		if (FALSE == bResult)
		{
			return bResult;
		}
	}
	else
	{
		strWrite.Format(_T("X %f Y %f\n"), pVertex[nVertexIndex-1].x, pVertex[nVertexIndex-1].y);
		bResult = WriteParamString(strWrite);
		if (FALSE == bResult)
		{
			return bResult;
		}
	}

	strWrite.Format(_T("INH -516"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}
	//等待一下
	strWrite.Format(_T("DWL0"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"),_T("CLR 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}
	strWrite.Format(_T("REM ENDENTITY\n"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	if (NULL != pVertex)
	{
		delete[] pVertex;
		pVertex = NULL;
	}

	bResult = TRUE;
	return bResult;
}

BOOL CJetDxfWrite::WriteLine(PENTLINE pEntLine,double xOffset,double yOffset,double xScale,double yScale,double rotation)
{
	BOOL bResult = FALSE;

	REALPOINT startPoint;

	//First Caculate Point
	startPoint.x = WriteCaculateXPos(pEntLine->Point0.x,pEntLine->Point0.y,xOffset,yOffset,xScale,yScale,rotation);
	startPoint.y = WriteCaculateYPos(pEntLine->Point0.x,pEntLine->Point0.y,xOffset,yOffset,xScale,yScale,rotation);
	startPoint.x *= db_XZoomFactor;
	startPoint.y *= db_YZoomFactor;
	startPoint.x *= -1;
	startPoint.y *= -1;
	startPoint.x = startPoint.x + db_XOffset;
	startPoint.y = startPoint.y + db_YOffSet;


	REALPOINT endPoint;
	//First Caculate Point
	endPoint.x = WriteCaculateXPos(pEntLine->Point1.x,pEntLine->Point1.y,xOffset,yOffset,xScale,yScale,rotation);
	endPoint.y = WriteCaculateYPos(pEntLine->Point1.x,pEntLine->Point1.y,xOffset,yOffset,xScale,yScale,rotation);
	endPoint.x *= db_XZoomFactor;
	endPoint.y *= db_YZoomFactor;
	endPoint.x *= -1;
	endPoint.y *= -1;

	endPoint.x = endPoint.x + db_XOffset;
	endPoint.y = endPoint.y + db_YOffSet;
	

	CString strWrite = _T("");
	//移动至起始点
	strWrite.Format(_T("%s\n"),_T("CLR 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("X %f Y %f\n"), startPoint.x, startPoint.y);
	bResult = WriteParamString(strWrite);
	if (!bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"), _T("REM LINE"));
	bResult = WriteParamString(strWrite);
	if (!bResult)
	{
		return bResult;
	}

	//置位之前移动至起始点
	//等待一下
	strWrite.Format(_T("INH -516"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"),_T("SET 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("X %f Y %f\n"), endPoint.x, endPoint.y);
	bResult = WriteParamString(strWrite);
	if (!bResult)
	{
		return bResult;
	}


	//等待一下
	strWrite.Format(_T("INH -516"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s"),_T("CLR 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("REM ENDENTITY\n"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	bResult = TRUE;
	return bResult;

}

BOOL CJetDxfWrite::WritePoint(PENTPOINT pEntPoint,double xOffset,double yOffset,double xScale,double yScale,double rotation)
{
	BOOL bResult = FALSE;
	REALPOINT centerPoint = pEntPoint->Point0;
	centerPoint.x = WriteCaculateXPos(pEntPoint->Point0.x,pEntPoint->Point0.y,xOffset,yOffset,xScale,yScale,rotation);
	centerPoint.y = WriteCaculateYPos(pEntPoint->Point0.x,pEntPoint->Point0.y,xOffset,yOffset,xScale,yScale,rotation);

	centerPoint.x *= db_XZoomFactor;
	centerPoint.y *= db_YZoomFactor;
	
	centerPoint.x *= -1;
	centerPoint.y *= -1;

	centerPoint.x = centerPoint.x + db_XOffset;
	centerPoint.y = centerPoint.y + db_YOffSet;

	CString strWrite = _T("");
	//移动至起始点
	strWrite.Format(_T("%s\n"),_T("CLR 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("X%f Y%f"),centerPoint.x,centerPoint.y);
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	//等待
	//等待一下
	strWrite.Format(_T("INH -516"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"),_T("Set 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	//等待一下
	strWrite.Format(_T("DWL (P1000)"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"),_T("CLR 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	bResult = TRUE;
	return bResult;

}

BOOL CJetDxfWrite::WriteCircle(PENTCIRCLE pEntCircle,double xOffset,double yOffset,double xScale,double yScale,double rotation)
{
	BOOL bResult = FALSE;

	REALPOINT centerPoint = pEntCircle->Point0;

	//First Caculate Point
	centerPoint.x = WriteCaculateXPos(pEntCircle->Point0.x,pEntCircle->Point0.y,xOffset,yOffset,xScale,yScale,rotation);
	centerPoint.y = WriteCaculateYPos(pEntCircle->Point0.x,pEntCircle->Point0.y,xOffset,yOffset,xScale,yScale,rotation);

	centerPoint.x *= db_XZoomFactor;
	centerPoint.y *= db_YZoomFactor;
	centerPoint.x *= -1;
	centerPoint.y *= -1;

	centerPoint.x = centerPoint.x + db_XOffset;
	centerPoint.y = centerPoint.y + db_YOffSet;

	double dbRadius = pEntCircle->Radius;
	dbRadius = dbRadius*(max(db_XZoomFactor, db_YZoomFactor));
	REALPOINT topPoint;
	topPoint.x = centerPoint.x;
	topPoint.y = centerPoint.y + dbRadius;

	CString strWrite = _T("");

	//首先移动至顶点
	strWrite.Format(_T("X %f Y%f\n"), topPoint.x, topPoint.y);
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("REM CIRCLE\n"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	//等待一下
	strWrite.Format(_T("INH -516"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"),_T("SET 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("CIRCCW X(%f,%f) Y(%f,%f)\n"), topPoint.x, centerPoint.x, topPoint.y, centerPoint.y);
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	//等待一下
	strWrite.Format(_T("INH -516"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"),_T("CLR 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("REM ENDENTITY\n"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	bResult = TRUE;
	return bResult;
}

BOOL CJetDxfWrite::WriteArc(PENTARC pEntArc,double xOffset,double yOffset,double xScale,double yScale,double rotation)
{
	BOOL bResult = FALSE;
	REALPOINT centerPoint;

	//First Caculate Point
	centerPoint.x = WriteCaculateXPos(pEntArc->Point0.x,pEntArc->Point0.y,xOffset,yOffset,xScale,yScale,rotation);
	centerPoint.y = WriteCaculateYPos(pEntArc->Point0.x,pEntArc->Point0.y,xOffset,yOffset,xScale,yScale,rotation);
	
	centerPoint.x *= db_XZoomFactor;
	centerPoint.y *= db_YZoomFactor;
	centerPoint.x *= -1;
	centerPoint.y *= -1;
	centerPoint.x = centerPoint.x + db_XOffset;
	centerPoint.y = centerPoint.y + db_YOffSet;

	double dbRadius = pEntArc->Radius;
	dbRadius = dbRadius*(max(db_XZoomFactor, db_YZoomFactor));
	REALPOINT startPoint;
	startPoint.x = 0.0;
	startPoint.y = 0.0;
	startPoint.z = 0.0;

	REALPOINT endPoint;
	endPoint.x = 0.0;
	endPoint.y = 0.0;
	endPoint.z = 0.0;

	double startAngle = pEntArc->StartAngle;
	double endAngle = pEntArc->EndAngle;

	startPoint.x = centerPoint.x + dbRadius * cos(startAngle / 180.0*PI);
	startPoint.y = centerPoint.y + dbRadius * sin(startAngle / 180.0*PI);

	endPoint.x = centerPoint.x + dbRadius*cos(endAngle / 180 * PI);
	endPoint.y = centerPoint.y + dbRadius*sin(endAngle / 180 * PI);

	CString strWrite = _T("");

	//首先移动至顶点
	strWrite.Format(_T("X %f Y%f\n"), startPoint.x, startPoint.y);
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("REM ARC\n"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	//等待一下
	strWrite.Format(_T("INH -516"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("%s\n"),_T("SET 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("CIRCCW X(%f,%f) Y(%f,%f)\n"), endPoint.x, centerPoint.x, endPoint.y, centerPoint.y);
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}


	//等待一下
	strWrite.Format(_T("INH -516"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}


	strWrite.Format(_T("%s\n"),_T("CLR 32"));
	bResult = WriteParamString(strWrite);
	if(FALSE == bResult)
	{
		return bResult;
	}

	strWrite.Format(_T("REM ENDENTITY\n"));
	bResult = WriteParamString(strWrite);
	if (FALSE == bResult)
	{
		return bResult;
	}

	bResult = TRUE;
	return bResult;
}


BOOL CJetDxfWrite::dxfCreateFile(LPCTSTR	FileName)
{
	if ((fopen_s(&pStream,CW2A(FileName), "wt")) == 0)
	{
		// Cannot create new file
		return TRUE;
	}


	return FALSE;


}


BOOL CJetDxfWrite::dxfCloseFile()
{

	if (NULL != pStream)
	{
		//WriteParamString(_T("EOF"));
	}

	// Closing DXF File -----------------
	if (fclose(pStream) != 0)
	{
		return FALSE;
	}

	return TRUE;
}

double CJetDxfWrite::WriteCaculateXPos(double xOrign,double yOrign,double xOffset,double yOffset,double xScale,double yScale,double rotation)
{
	double dbResult = 0.0;
	dbResult = xOffset + xOrign*xScale*cos(rotation*PI/180.0) - yOrign*yScale*sin(rotation*PI/180.0);
	return dbResult;
}

double CJetDxfWrite::WriteCaculateYPos(double xOrign,double yOrign,double xOffset,double yOffset,double xScale,double yScale,double rotation)
{
	double dbResult = 0.0;
	dbResult = yOffset + yOrign*yScale*cos(rotation*PI/180.0) + xOrign*xScale*sin(rotation*PI/180.0);
	return dbResult;
}