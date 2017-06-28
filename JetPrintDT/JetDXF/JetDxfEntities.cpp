#include "../stdafx.h"
#include "JetDxfEntities.h"
#include "JetDxfRead.h"
#include <afxpriv.h>


CJetDxfEntities::CJetDxfEntities(CJetDxfRead *pDxfRead) :CJetDxfBase(pDxfRead)
{
	m_pReadString = NULL;
	//memset(m_pReadString, 0, 1024 * sizeof(m_pReadString));
}


CJetDxfEntities::~CJetDxfEntities()
{

	if (NULL != m_pReadString)
	{
		delete []m_pReadString;
		m_pReadString = NULL;
	}
}

void CJetDxfEntities::SetEntityHeader(PDXFENTITYHEADER pEntityHeader)
{
	ZeroMemory(pEntityHeader, sizeof(DXFENTITYHEADER));
	pEntityHeader->Color = 256;					// Color (Default=BYLAYER)
	pEntityHeader->Thickness = 0;				// Thickness (default=0)
	pEntityHeader->LineTypeScale = 1.0;			// Linetype scale (default=1.0)
	pEntityHeader->ExtrusionDirection[0] = 0;	// Extrusion direction. (default = 0, 0, 1)
	pEntityHeader->ExtrusionDirection[1] = 0;
	pEntityHeader->ExtrusionDirection[2] = 1;
}


bool CJetDxfEntities::dxfReadData(HDXF hDxf)
{
	
	if (NULL == m_pDxfRead->pDxf)
		return FALSE;

	if (!dxfReadDataDirect(m_pDxfEntityHeader,pEntityData))
	{
		return FALSE;
	}

	
	return true;
}

bool CJetDxfEntities::dxfWriteData(HDXF hDxf)
{
	return true;
}

bool CJetDxfEntities::dxfReadDataDirect(PDXFENTITYHEADER pEntityHeader,LPVOID pEntityData)
{
	bool	result = false;
	CString strParam = _T("");
	CString strTemp = _T("");
	

	if (NULL == m_pDxfRead->pDxf)
		return false;

	// Check if current section is ENTITIES or BLOCKS
	if (m_pDxfRead->pDxf->Read.CurrentSection != SEC_ENTITIES)
	{
		if ((m_pDxfRead->pDxf->Read.CurrentSection != SEC_BLOCKS) || (!m_pDxfRead->pDxf->Read.isBlockOpen))
		{
			// Can not read entity data
			return false;
		}
	}

	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	m_pDxfRead->dxfStorePos();
	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	if (GCode != 0)
	{
		// Can not read entity data
		m_pDxfRead->dxfRestorePos();

		return false;
	}

	SetEntityHeader(pEntityHeader);	// Set Entities Header to Default Values
	result = FALSE;

	do {
		
		if (strcmp(CW2A(m_pReadString),"LINE") == 0)
		{
			ReadLineData(pEntityHeader, (PDXFENTLINE)pEntityData);
			result = TRUE;
			break;
		}
		else if (0 == strcmp(CW2A(m_pReadString),"POINT"))
		{
			ReadPointData(pEntityHeader, (PDXFENTPOINT)pEntityData);
			result = TRUE;
			break;
		}
		else if (0 == strcmp(CW2A(m_pReadString),"CIRCLE"))
		{
			ReadCircleData(pEntityHeader, (PDXFENTCIRCLE)pEntityData);
			result = TRUE;
			break;
		}
		else if (0 == strcmp(CW2A(m_pReadString),"ELLIPSE"))
		{
			ReadEllipseData(pEntityHeader, (PDXFENTELLIPSE)pEntityData);
			result = TRUE;
			break;
		}
		else if (0 == strcmp(CW2A(m_pReadString), "LWPOLYLINE"))
		{
			ReadLWPolylineData(pEntityHeader, (PDXFENTLWPOLYLINE)pEntityData);
			result =TRUE;
			break;
		}
		else if (0 == strcmp(CW2A(m_pReadString) ,"TEXT"))
		{
			ReadTextData(pEntityHeader, (PDXFENTTEXT)pEntityData);
			result = TRUE;
			break;
		}
		else if (0 == strcmp(CW2A(m_pReadString) ,"ARC"))
		{
			ReadArcData(pEntityHeader, (PDXFENTARC)pEntityData);
			result = TRUE;
			break;
		}
		else if (0 == strcmp(CW2A(m_pReadString) ,"SOLID"))
		{
			ReadSolidData(pEntityHeader, (PDXFENTSOLID)pEntityData);
			result = TRUE;
			break;
		}
		else if (0 == strcmp(CW2A(m_pReadString),"INSERT"))
		{
			ReadInsertData(pEntityHeader, (PDXFENTINSERT)pEntityData);
			result = TRUE;
			break;
		}
		//This code is implemented by tran duy dung
		//*******************************************************************
		else if (0 == strcmp(CW2A(m_pReadString) ,"POLYLINE")){
			ReadPolyLineData(pEntityHeader, (PDXFENTPOLYLINE)pEntityData);
			result = TRUE;
			break;
		}
		//*******************************************************************
		else if (0 == strcmp(CW2A(m_pReadString) ,"DIMENSION"))
		{
			ReadDimensionData(pEntityHeader, (PDXFENTDIMENSION)pEntityData);
			result = TRUE;
			break;
		}

		//******************************************************************
		else if (0 == strcmp(CW2A(m_pReadString) ,"SPLINE"))
		{
			ReadSplineData(pEntityHeader, (PDXFENTSPLINE)pEntityData);
			result = TRUE;
			break;
		}

		else
		{
			// Unknown entity
			// Looking for next entity
			do {
				memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
				m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
			} while (GCode != 0);
		}
	} while ((strcmp(CW2A(m_pReadString), "ENDBLK") != 0) && (strcmp(CW2A(m_pReadString), "ENDSEC") != 0) && (m_pDxfRead->pDxf->Read.CurrentPos < m_pDxfRead->pDxf->Read.FileSize));

	if ((m_pDxfRead->pDxf->Read.CurrentSection == SEC_BLOCKS) && (m_pDxfRead->pDxf->Read.isBlockOpen))
	{

		if ((GCode == 0) && (strcmp(CW2A(m_pReadString), "ENDBLK") == 0))
		{
			m_pDxfRead->pDxf->Read.isBlockOpen = FALSE;

			do{
				m_pDxfRead->dxfStorePos();
				memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
				m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
			} while (GCode != 0);


			if (strcmp(CW2A(m_pReadString), "ENDSEC") == 0)
				m_pDxfRead->pDxf->Read.CurrentSection = SEC_NOTSET; // Blocks section has been finished
			else // Reached to the next block
				m_pDxfRead->dxfRestorePos();
		}
		else
			m_pDxfRead->dxfRestorePos();
	}
	else
	{	
		if ((GCode == 0) && (strcmp(CW2A(m_pReadString), "ENDSEC") == 0))
			m_pDxfRead->pDxf->Read.CurrentSection = SEC_NOTSET; // Entities section has been finished
		else
			m_pDxfRead->dxfRestorePos();
	}

	return result;
}

BOOL CJetDxfEntities::ReadArcData(PDXFENTITYHEADER pEntityHeader, PDXFENTARC pArc)
{
	ZeroMemory(pArc, sizeof(DXFENTARC));

	pEntityHeader->EntityType = ENT_ARC;
	
	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, CW2A(m_pReadString));
			break;
		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CW2A(m_pReadString));
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = _wtof(m_pReadString);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = _wtof(m_pReadString);
			break;
		case 10:	// Center point X
			pArc->Point0.x = _wtof(m_pReadString);
			break;
		case 20:	// Center point Y
			pArc->Point0.y = _wtof(m_pReadString);
			break;
		case 30:	// Center point Z
			pArc->Point0.z = _wtof(m_pReadString);
			break;
		case 40:	// Radius
			pArc->Radius = _wtof(m_pReadString);
			break;
		case 50:	// Start angle
			pArc->StartAngle = _wtof(m_pReadString);
			break;
		case 51:	// End angle
			pArc->EndAngle = _wtof(m_pReadString);
			break;
		}

		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	}


	return TRUE;
}

BOOL CJetDxfEntities::ReadCircleData(PDXFENTITYHEADER pEntityHeader, PDXFENTCIRCLE pCircle)
{
	ZeroMemory(pCircle, sizeof(DXFENTCIRCLE));

	pEntityHeader->EntityType = ENT_CIRCLE;

	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));

	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, CT2A(m_pReadString));
			break;
		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);;
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CT2A(m_pReadString));
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = _wtof(m_pReadString);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = _wtof(m_pReadString);
			break;
		case 10:	// Center point X
			pCircle->Point0.x = _wtof(m_pReadString);
			break;
		case 20:	// Center point Y
			pCircle->Point0.y = _wtof(m_pReadString);
			break;
		case 30:	// Center point Z
			pCircle->Point0.z = _wtof(m_pReadString);
			break;
		case 40:	// Radius
			pCircle->Radius = _wtof(m_pReadString);
			break;
		}

		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);

	}

	return TRUE;
}

BOOL CJetDxfEntities::ReadDimensionData( PDXFENTITYHEADER pEntityHeader, PDXFENTDIMENSION pDimension)
{
	ZeroMemory(pDimension, sizeof(DXFENTDIMENSION));

	pEntityHeader->EntityType = ENT_DIMENSION;

	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));

	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, CT2A(m_pReadString));
			break;
		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);;
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CT2A(m_pReadString));
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = _wtof(m_pReadString);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = _wtof(m_pReadString);
			break;
		case 10:	// Definition point (in WCS) X
			pDimension->DimLineDefPoint.x = _wtof(m_pReadString);
			break;
		case 20:	// Definition point (in WCS) Y
			pDimension->DimLineDefPoint.y = _wtof(m_pReadString);
			break;
		case 30:	// Definition point (in WCS) Z
			pDimension->DimLineDefPoint.z = _wtof(m_pReadString);
			break;
		case 13:	// Definition point for linear and angular dimensions (in WCS) X
			pDimension->DefPoint3.x = _wtof(m_pReadString);
			break;
		case 23:	// Definition point for linear and angular dimensions (in WCS) Y
			pDimension->DefPoint3.y = _wtof(m_pReadString);
			break;
		case 33:	// Definition point for linear and angular dimensions (in WCS) Z
			pDimension->DefPoint3.z = _wtof(m_pReadString);
			break;
		case 14:	// Definition point for linear and angular dimensions (in WCS) X
			pDimension->DefPoint4.x = _wtof(m_pReadString);
			break;
		case 24:	// Definition point for linear and angular dimensions (in WCS) Y
			pDimension->DefPoint4.y = _wtof(m_pReadString);
			break;
		case 34:	// Definition point for linear and angular dimensions (in WCS) Z
			pDimension->DefPoint4.z = _wtof(m_pReadString);
			break;
		case 53:	// The rotation angle of the dimension text away from its default orientation (the direction of the dimension line)
			pDimension->DimRotationAngle = _wtof(m_pReadString);
			break;
		case 3:		// Dimension style name
			strcpy_s(pDimension->DimStyleName, CW2A(m_pReadString));
			break;
		case 1:		// Dimension text explicitly entered by the user. Optional; default is the measurement. If null or "<>", the dimension measurement is drawn as the text, if " " (one blank space), the text is suppressed.
			strcpy_s(pDimension->DimText, CW2A(m_pReadString));
			break;
		case 2:		// Name of the block that contains the entities that make up the dimension picture
			strcpy_s(pDimension->BlockName, CW2A(m_pReadString));
			break;
		}

		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	}

	return TRUE;
}

BOOL CJetDxfEntities::ReadEllipseData(PDXFENTITYHEADER pEntityHeader, PDXFENTELLIPSE pEllipse)
{
	ZeroMemory(pEllipse, sizeof(DXFENTELLIPSE));

	pEntityHeader->EntityType = ENT_ELLIPSE;

	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));

	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, CW2A(m_pReadString));
			break;
		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CW2A(m_pReadString));
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = _wtof(m_pReadString);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = _wtof(m_pReadString);
			break;
		case 10:	// Center point X
			pEllipse->CenterPoint.x = _wtof(m_pReadString);
			break;
		case 20:	// Center point Y
			pEllipse->CenterPoint.y = _wtof(m_pReadString);
			break;
		case 30:	// Center point Z
			pEllipse->CenterPoint.z = _wtof(m_pReadString);
			break;
		case 11:	// Endpoint of major axis X
			pEllipse->MajorAxisEndPoint.x = _wtof(m_pReadString);
			break;
		case 21:	// Endpoint of major axis Y
			pEllipse->MajorAxisEndPoint.y = _wtof(m_pReadString);
			break;
		case 31:	// Endpoint of major axis Z
			pEllipse->MajorAxisEndPoint.z = _wtof(m_pReadString);
			break;
		case 40:	// Ratio of minor axis to major axis
			pEllipse->MinorToMajorRatio = _wtof(m_pReadString);
			break;
		case 41:	// Start parameter (this value is 0.0 for a full ellipse)
			pEllipse->StartParam = _wtof(m_pReadString);
			break;
		case 42:	// End parameter (this value is 2pi for a full ellipse)
			pEllipse->EndParam = _wtof(m_pReadString);
			break;
		}

		m_pDxfRead->dxfStorePos();
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);

		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	return TRUE;
}

BOOL CJetDxfEntities::ReadInsertData( PDXFENTITYHEADER pEntityHeader, PDXFENTINSERT pInsert)
{
	ZeroMemory(pInsert, sizeof(DXFENTINSERT));

	pEntityHeader->EntityType = ENT_INSERT;
	pInsert->XScale = 1;
	pInsert->YScale = 1;
	pInsert->ZScale = 1;
	pInsert->RotationAngle = 0;

	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));

	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);

	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, CW2A(m_pReadString));
			break;
		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CW2A(m_pReadString));
			break;
		case 2:		// Block Name
			strcpy_s(pInsert->BlockHeaderName, CW2A(m_pReadString));
			break;
		case 10:	// Insertion point X
			pInsert->Point0.x = _wtof(m_pReadString);
			break;
		case 20:	// Insertion point Y
			pInsert->Point0.y = _wtof(m_pReadString);
			break;
		case 30:	// Insertion point Z
			pInsert->Point0.z = _wtof(m_pReadString);
			break;
		case 41:	// X scale factor (optional; default = 1)
			pInsert->XScale = _wtof(m_pReadString);
			break;
		case 42:	// Y scale factor (optional; default = 1)
			pInsert->YScale = _wtof(m_pReadString);
			break;
		case 43:	// Z scale factor (optional; default = 1)
			pInsert->ZScale = _wtof(m_pReadString);
			break;
		case 50:	// Rotation angle (optional; default = 0)
			pInsert->RotationAngle = _wtof(m_pReadString);
			break;
		}

		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	}

	return TRUE;
}

BOOL CJetDxfEntities::ReadLineData(PDXFENTITYHEADER pEntityHeader, PDXFENTLINE pLine)
{
	ZeroMemory(pLine, sizeof(DXFENTLINE));

	pEntityHeader->EntityType = ENT_LINE;

	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));

	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, CW2A(m_pReadString));
			break;
		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CW2A(m_pReadString));
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = _wtof(m_pReadString);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = _wtof(m_pReadString);
			break;
		case 10:	// Start point X
			pLine->Point0.x = _wtof(m_pReadString);
			break;
		case 20:	// Start point Y
			pLine->Point0.y = _wtof(m_pReadString);
			break;
		case 30:	// Start point Z
			pLine->Point0.z = _wtof(m_pReadString);
			break;
		case 11:	// End point X
			pLine->Point1.x = _wtof(m_pReadString);
			break;
		case 21:	// End point Y
			pLine->Point1.y = _wtof(m_pReadString);
			break;
		case 31:	// End point Z
			pLine->Point1.z = _wtof(m_pReadString);
			break;
		}

		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, sizeof(TCHAR) * 1024);
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	}

	return TRUE;
}

BOOL CJetDxfEntities::ReadPointData(PDXFENTITYHEADER pEntityHeader, PDXFENTPOINT pPoint)
{
	ZeroMemory(pPoint, sizeof(DXFENTPOINT));

	pEntityHeader->EntityType = ENT_POINT;

	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));

	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, CW2A(m_pReadString));
			break;
		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CW2A(m_pReadString));
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = _wtof(m_pReadString);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = _wtof(m_pReadString);
			break;
		case 10:	// Point location X
			pPoint->Point0.x = _wtof(m_pReadString);
			break;
		case 20:	// Point location Y
			pPoint->Point0.y = _wtof(m_pReadString);
			break;
		case 30:	// Point location Z
			pPoint->Point0.z = _wtof(m_pReadString);
			break;
		}

		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);

	}

	return TRUE;
}

BOOL CJetDxfEntities::ReadLWPolylineData(PDXFENTITYHEADER pEntityHeader, PDXFENTLWPOLYLINE pLWPolyline)
{
	pEntityHeader->EntityType = ENT_LWPOLYLINE;
	pLWPolyline->b_PolylineClose = false;
	pLWPolyline->pCoordinateX = NULL;
	pLWPolyline->pCoordinateY = NULL;

	ZeroMemory(pLWPolyline, sizeof(PDXFENTLWPOLYLINE));
	static UINT  vertexX = 0;
	static UINT  vertexY = 0;
	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));

	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, CW2A(m_pReadString));
			break;
		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CW2A(m_pReadString));
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = _wtof(m_pReadString);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = _wtof(m_pReadString);
			break;
		case 70:	// 是否闭合
			pLWPolyline->b_PolylineClose = (_wtoi(m_pReadString)>0 ? true : false);
			break;
			       //顶点的个数
		case 90:
			pLWPolyline->CountOfVertex = _wtoi(m_pReadString);

			if (NULL == pLWPolyline->pCoordinateX)
			{
				pLWPolyline->pCoordinateX = new double[pLWPolyline->CountOfVertex];
				if (NULL == pLWPolyline->pCoordinateX)
				{
					//内存分配失败，do something
				}
			}

			if (NULL == pLWPolyline->pCoordinateY)
			{
				pLWPolyline->pCoordinateY = new double[pLWPolyline->CountOfVertex];

				if (NULL == pLWPolyline->pCoordinateY)
				{
					//内存分配失败，do something
				}
			}
			break;

		case 10:
			if (NULL != pLWPolyline->pCoordinateX)
			{
				pLWPolyline->pCoordinateX[vertexX] = _wtof(m_pReadString);
				++vertexX;
			}
			break;

		case 20:
			if (NULL != pLWPolyline->pCoordinateY)
			{
				pLWPolyline->pCoordinateY[vertexY] = _wtof(m_pReadString);
				++vertexY;
			}
			break;
		}
		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, sizeof(TCHAR) * 1024);
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	}

	vertexX = 0;
	vertexY = 0;
	return TRUE;

}

BOOL CJetDxfEntities::ReadPolyLineData(PDXFENTITYHEADER pEntityHeader, PDXFENTPOLYLINE pPolyLine)
{
	ZeroMemory(pPolyLine, sizeof(DXFENTPOLYLINE));

	pEntityHeader->EntityType = ENT_POLYLINE;
	pPolyLine->Flag = 0;

	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));

	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, CW2A(m_pReadString));
			break;
		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CW2A(m_pReadString));
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = _wtof(m_pReadString);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = _wtof(m_pReadString);
			break;
		case 70:	// Polyline flag (bit-coded); default is 0
			pPolyLine->Flag = _wtoi(m_pReadString);
			break;
		}
		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	}

	// Reading Vertex Data
	SimpleList<DXFENTVERTEX> vertices;
	DXFENTVERTEX Vertex;
	while ((GCode == 0) && (wcscmp(m_pReadString, _T("VERTEX")) == 0))
	{
		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
		while (GCode != 0){
			switch (GCode)
			{
			case 10:	// Start point X
				Vertex.Point.x = _wtof(m_pReadString);
				break;
			case 20:	// Start point X
				Vertex.Point.y = _wtof(m_pReadString);
				break;
			case 30:	// Start point X
				Vertex.Point.z = _wtof(m_pReadString);
				break;
			}
			m_pDxfRead->dxfStorePos();
			memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
			m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
		}

		Vertex.Bulge = 0.0;
		vertices.add(Vertex);
	}

	pPolyLine->nVertex = vertices.getSize();
	pPolyLine->pVertex = new DXFENTVERTEX[pPolyLine->nVertex];
	int i = 0;
	vertices.start();
	while (vertices.next()) {
		pPolyLine->pVertex[i] = vertices.get();
		i++;
	}

	return TRUE;
}

BOOL CJetDxfEntities::ReadSplineData(PDXFENTITYHEADER pEntityHeader, PDXFENTSPLINE pSpline)
{
	CString strList = _T("");
	CString strTemp = _T("");
	DWORD dwLenth = 0;
	static UINT uKontIndex = 0;
	static UINT uControlPointIndex_X = 0;
	static UINT uControlPointIndex_Y = 0;
	int layerData = 0;

	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	pEntityHeader->EntityType = ENT_SPLINE;
	pSpline->uFlag = 0;
	pSpline->pControlPoint_X = NULL;
	pSpline->pControlPoint_Y = NULL;
	pSpline->pKontPoint = NULL;
	pSpline->SplineMatchPoint = 0;

	ZeroMemory(pSpline, sizeof(DXFENTPOLYLINE));

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);

	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			//strcpy_s(pEntityHeader->LayerName, _wtoi(strReadParam));
			layerData = _wtoi(m_pReadString);
			sprintf_s(pEntityHeader->LayerName,sizeof(layerData)/sizeof(int) +1, "%d",layerData);
			strTemp.Format(_T("LayerName %s"), pEntityHeader->LayerName);
			strList += strTemp;
			break;

		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);
			strTemp.Format(_T("Color %d"), pEntityHeader->Color);
			strList += strTemp;
			break;

		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CW2A(m_pReadString));
			strTemp.Format(_T("LineType %s"), pEntityHeader->LTypeName);
			strList += strTemp;
			break;

		case 39:	// Thickness
			pEntityHeader->Thickness = _wtof(m_pReadString);
			strTemp.Format(_T("Thickness %s"), pEntityHeader->Thickness);
			strList += strTemp;
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = _wtof(m_pReadString);
			strTemp.Format(_T("LineTypeScale %f"), pEntityHeader->LineTypeScale);
			strList += strTemp;
			break;
		case 70:	// Polyline flag (bit-coded); default is 0
			pSpline->uFlag = _wtoi(m_pReadString);
			strTemp.Format(_T("PolyLineType %d"), pSpline->uFlag);
			strList += strTemp;
			break;

		case 71:    //样条曲线的阶数
			pSpline->SplineRank = _wtoi(m_pReadString);
			strTemp.Format(_T("Rank %d"), pSpline->SplineRank);
			strList += strTemp;
			break;

		case  72:   //样条曲线的节点数
			pSpline->SplineKnots = _wtoi(m_pReadString);
			strTemp.Format(_T("Knots %d"), pSpline->SplineKnots);
			strList += strTemp;

			if (NULL == pSpline->pKontPoint)
			{
				pSpline->pKontPoint = new double[pSpline->SplineKnots];
				if (NULL == pSpline->pKontPoint)
				{
					//内存空间分配失败，do something
				}
			}
			break;

		case 73:
			pSpline->SplineControlPoints = _wtoi(m_pReadString);
			strTemp.Format(_T("ControlPoints %d"), pSpline->SplineControlPoints);
			strList += strTemp;
			if (NULL == pSpline->pControlPoint_X)
			{
				pSpline->pControlPoint_X = new double[pSpline->SplineControlPoints];
				if (NULL == pSpline->pControlPoint_X)
				{
					//内存空间分配失败，do something
				}
			}

			if (NULL == pSpline->pControlPoint_Y)
			{
				pSpline->pControlPoint_Y = new double[pSpline->SplineControlPoints];
				if (NULL == pSpline->pControlPoint_X)
				{
					//内存空间分配失败，do something
				}
			}
			break;

			//拟合点数
		case 74:
			pSpline->SplineMatchPoint = _wtoi(m_pReadString);
			strTemp.Format(_T("ControlPoints %d"), pSpline->SplineMatchPoint);
			strList += strTemp;
			break;

		case 40:
			pSpline->pKontPoint[uKontIndex] = _wtof(m_pReadString);
			++uKontIndex;
			break;

		case 10:
			pSpline->pControlPoint_X[uControlPointIndex_X] = _wtof(m_pReadString);
			++uControlPointIndex_X;
			break;

		case 20:
			pSpline->pControlPoint_Y[uControlPointIndex_Y] = _wtof(m_pReadString);
			++uControlPointIndex_Y;
			break;

		default:
			strTemp = _T("");
			break;

		}

		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	}

	uKontIndex = 0;
	uControlPointIndex_X = 0;
	uControlPointIndex_Y = 0;
	return TRUE;
}

BOOL CJetDxfEntities::ReadSolidData(PDXFENTITYHEADER pEntityHeader, PDXFENTSOLID pSolid)
{
	ZeroMemory(pSolid, sizeof(DXFENTSOLID));

	pEntityHeader->EntityType = ENT_SOLID;
	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));

	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, CW2A(m_pReadString));
			break;
		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CW2A(m_pReadString));
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = _wtof(m_pReadString);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = _wtof(m_pReadString);
			break;
		case 10:	// First corner X
			pSolid->Point0.x = _wtof(m_pReadString);
			break;
		case 20:	// First corner Y
			pSolid->Point0.y = _wtof(m_pReadString);
			break;
		case 30:	// First corner Z
			pSolid->Point0.z = _wtof(m_pReadString);
			break;
		case 11:	// Second corner X
			pSolid->Point1.x = _wtof(m_pReadString);
			break;
		case 21:	// Second corner Y
			pSolid->Point1.y = _wtof(m_pReadString);
			break;
		case 31:	// Second corner Z
			pSolid->Point1.z = _wtof(m_pReadString);
			break;
		case 12:	// Third corner X
			pSolid->Point2.x = _wtof(m_pReadString);
			break;
		case 22:	// Third corner Y
			pSolid->Point2.y = _wtof(m_pReadString);
			break;
		case 32:	// Third corner Z
			pSolid->Point2.z = _wtof(m_pReadString);
			break;
		case 13:	// Fourth corner X
			pSolid->Point3.x = _wtof(m_pReadString);
			break;
		case 23:	// Fourth corner Y
			pSolid->Point3.y = _wtof(m_pReadString);
			break;
		case 33:	// Fourth corner Z
			pSolid->Point3.z = _wtof(m_pReadString);
			break;
		}

		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	}

	return TRUE;
}


BOOL CJetDxfEntities::ReadTextData(PDXFENTITYHEADER pEntityHeader, PDXFENTTEXT pText)
{
	ZeroMemory(pText, sizeof(DXFENTTEXT));

	pEntityHeader->EntityType = ENT_TEXT;
	pText->TextData.Justification = 0;

	if (NULL == m_pReadString)
	{
		m_pReadString = new TCHAR[1024];
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
	}

	memset(m_pReadString, 0, 1024 * sizeof(TCHAR));

	m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, CW2A(m_pReadString));
			break;
		case 62:	// Color
			pEntityHeader->Color = _wtoi(m_pReadString);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, CW2A(m_pReadString));
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = _wtof(m_pReadString);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = _wtof(m_pReadString);
			break;
		case 7:		// Text Style Name
			strcpy_s(pText->TextData.TextStyleName, CW2A(m_pReadString));
			break;
		case 10:	// First alignment point (in OCS) X
			pText->Point0.x = _wtof(m_pReadString);
			break;
		case 20:	// First alignment point (in OCS) Y
			pText->Point0.y = _wtof(m_pReadString);
			break;
		case 30:	// First alignment point (in OCS) Z
			pText->Point0.z = _wtof(m_pReadString);
			break;
		case 72:	// Horizontal text justification type (default = 0)
			pText->TextData.Justification = (pText->TextData.Justification & 0xff00) | (char)_wtoi(m_pReadString);
			break;
		case 73:	// Vertical text justification type (default = 0)
			pText->TextData.Justification = (pText->TextData.Justification & 0x00ff) | (char)_wtoi(m_pReadString) << 8;
			break;
		case 11:	// Second alignment point (in OCS) X
			pText->TextData.SecondAlignmentPoint.x = _wtof(m_pReadString);
			break;
		case 21:	// Second alignment point (in OCS) Y
			pText->TextData.SecondAlignmentPoint.y = _wtof(m_pReadString);
			break;
		case 31:	// Second alignment point (in OCS) Z
			pText->TextData.SecondAlignmentPoint.z = _wtof(m_pReadString);
			break;
		case 40:	// Text Height
			pText->TextData.Height = _wtof(m_pReadString);
			break;
		case 41:	// Width Factor
			pText->TextData.WidthFactor = _wtof(m_pReadString);
			break;
		case 50:	// Text Rotation Angle
			pText->TextData.RotationAngle = _wtof(m_pReadString);
			break;
		case 1:		// Default value (the string itself)
			strcpy_s(pText->strText, CW2A(m_pReadString));
			break;
		}

		m_pDxfRead->dxfStorePos();
		memset(m_pReadString, 0, 1024 * sizeof(TCHAR));
		m_pDxfRead->ReadParamFromDxfFile(GCode, m_pReadString);
	}

	return TRUE;
}