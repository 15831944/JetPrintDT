#include "../stdafx.h"
#include "JetDxfTable.h"
#include "JetDxfRead.h"

CJetDxfTable::CJetDxfTable(CJetDxfRead *pDxfRead) :CJetDxfBase(pDxfRead)
{
	GCode = 0;
}


CJetDxfTable::~CJetDxfTable()
{
}

/*-------------------------------------------------------------------*
|  ReadTableTypeName                                                |
|  Inputs:                                                          |
|      PDXF pDxf = pointer to the openning DXF file structure       |
|  Output: TableType Code else TAB_NOTSET                           |
*-------------------------------------------------------------------*/
DWORD CJetDxfTable::dxfReadTableTypeName()
{
	LPTSTR pTabValue = new TCHAR[256];
	CString strTabTemp = _T("");
	if (NULL == pTabValue)
	{
		return 0;
	}

	m_pDxfRead->ReadParamFromDxfFile(GCode, pTabValue);
	strTabTemp = pTabValue;

	if (strTabTemp == _T("LAYER"))
		m_pDxfRead->pDxf->Read.CurrentTableType = TAB_LAYER;

	else if (strTabTemp == _T("LTYPE"))
		m_pDxfRead->pDxf->Read.CurrentTableType = TAB_LTYPE;

	else if (strTabTemp == _T("STYLE"))
		m_pDxfRead->pDxf->Read.CurrentTableType = TAB_STYLE;

	else if (strTabTemp == _T("DIMSTYLE") )
		m_pDxfRead->pDxf->Read.CurrentTableType = TAB_DIMSTYLE;
	else
		m_pDxfRead->pDxf->Read.CurrentTableType = TAB_UNKNOWN;

	if (NULL != pTabValue)
	{
		delete []pTabValue;
		pTabValue = NULL;
	}

	return m_pDxfRead->pDxf->Read.CurrentTableType;
}

/*-------------------------------------------------------------------*
|  dxfReadTableTypeName                                             |
|  Inputs:                                                          |
|      HDXF hDxf = handle to the openning DXF file structure        |
|  Output: TableType Code else TAB_NOTSET                           |
*-------------------------------------------------------------------*/
bool CJetDxfTable::dxfReadData(HDXF hDxf)
{
	DWORD	result;

	// Initialize pDxf ------------------
	if ( NULL == m_pDxfRead->pDxf)
		return FALSE;

	// Check if current section is TABLES
	if (m_pDxfRead->pDxf->Read.CurrentSection != SEC_TABLES)
	{
		// Current section is not TABLES
		return FALSE;
	}

	m_pDxfRead->dxfStorePos();

	LPTSTR pTableName = new TCHAR[1024];
	if (NULL == pTableName)
	{
		//Do Something
	}
	memset(pTableName, 0, 1024 * sizeof(TCHAR));
	// Read TableType Name
	m_pDxfRead->dxfReadParam(GCode, pTableName);
	if ((GCode == 0) && (strcmp(CW2A(pTableName), "TABLE") == 0))
		dxfReadTableTypeName();
	else
		m_pDxfRead->dxfRestorePos(); // Restore Old Position

	result = m_pDxfRead->pDxf->Read.CurrentTableType;

	if (NULL != pTableName)
	{
		delete[] pTableName;
		pTableName = NULL;
	}
	return true;
}

/*-------------------------------------------------------------------*
|  dxfGetCurrentTableType                                           |
|  Inputs:                                                          |
|      HDXF hDxf = handle to the openning DXF file structure        |
|  Output: current table type code if everything is ok              |
*-------------------------------------------------------------------*/
DWORD CJetDxfTable::dxfGetCurrentTableType()
{
	DWORD	result;

	if (NULL == m_pDxfRead->pDxf)
		return FALSE;

	// Check if current section is TABLES
	if (m_pDxfRead->pDxf->Read.CurrentSection != SEC_TABLES)
	{
		// Current section is not TABLES
		return FALSE;
	}

	result = m_pDxfRead->pDxf->Read.CurrentTableType;

	return result;
}

/*-------------------------------------------------------------------*
|  dxfFindNextTableType                                             |
|  Inputs:                                                          |
|      HDXF hDxf = handle to the openning DXF file structure        |
|  Output: TableType Code else TAB_NOTSET                           |
*-------------------------------------------------------------------*/
DWORD CJetDxfTable::dxfFindNextTableType()
{
	DWORD	result;

	// Initialize pDxf ------------------
	if ( NULL == m_pDxfRead->pDxf)
		return FALSE;

	// Check if current section is TABLES
	if (m_pDxfRead->pDxf->Read.CurrentSection != SEC_TABLES)
	{
		// Current section is not TABLES
		return FALSE;
	}

	// Find TableType Name
	if (m_pDxfRead->FindParamFromDxfFile(0, _T("TABLE")))
	{
		// Read TableType Name
		dxfReadTableTypeName();
		result = m_pDxfRead->pDxf->Read.CurrentTableType;
	}
	else
		result = TAB_NOTSET;

	return result;
}

/*-------------------------------------------------------------------*
|  ReadDimStyleData                                                 |
|  Inputs:                                                          |
|      PDXF pDxf = pointer to the openning DXF file structure       |
|      PDXFDIMSTYLE pDimStyle = pointer to DimStyle structure       |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfTable::ReadDimStyleData(PDXFDIMSTYLE pDimStyle)
{

	ZeroMemory(pDimStyle, sizeof(DXFDIMSTYLE));
	LPTSTR pDimValue = new TCHAR[1024];
	if (NULL == pDimValue)
	{
		return FALSE;
	}

	m_pDxfRead->ReadParamFromDxfFile(GCode, pDimValue);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 2:		// DimStyle Name
			strcpy_s(pDimStyle->Name, CW2A(pDimValue));
			break;
		case 70:	// Standard flag values
			pDimStyle->StandardFlags = char(_wtoi(pDimValue));
			break;
		case 176:	// Dimension line & Arrow heads color
			pDimStyle->DIMCLRD = _wtoi(pDimValue);
			break;
		case 46:	// Dimension line size after Extensionline
			pDimStyle->DIMDLE = _wtoi(pDimValue);
			break;
		case 177:	// Extension line color
			pDimStyle->DIMCLRE = _wtoi(pDimValue);
			break;
		case 44:	// Extension line size after Dimline
			pDimStyle->DIMEXE = _wtoi(pDimValue);
			break;
		case 42:	// Offset from origin
			pDimStyle->DIMEXO = _wtoi(pDimValue);
			break;
		case 6:		// 1st Arrow head
			strcpy_s(pDimStyle->DIMBLK1, CW2A(pDimValue));
			break;
		case 7:		// 2nd Arrow head
			strcpy_s(pDimStyle->DIMBLK2, CW2A(pDimValue));
			break;
		case 41:	// Arrow size
			pDimStyle->DIMASZ = _wtof(pDimValue);
			break;
		case 340:	// Text style handle
			pDimStyle->DIMTXSTYObjhandle = _wtoi(pDimValue);
			break;
		case 178:	// Text color
			pDimStyle->DIMCLRT = _wtoi(pDimValue);
			break;
		case 140:	// Text height
			pDimStyle->DIMTXT =_wtof(pDimValue);
			break;
		case 77:	// Vertical Text Placement
			pDimStyle->DIMTAD = _wtoi(pDimValue);
			break;
		case 147:	// Offset from dimension line
			pDimStyle->DIMGAP = _wtof(pDimValue);
			break;
		}
		m_pDxfRead->dxfStorePos();
		m_pDxfRead->ReadParamFromDxfFile(GCode, pDimValue);
	}

	m_pDxfRead->dxfRestorePos();

	if (NULL != pDimValue)
	{
		delete [] pDimValue;
		pDimValue = NULL;
	}
	return TRUE;
}

/*-------------------------------------------------------------------*
|  ReadLayerData                                                    |
|  Inputs:                                                          |
|      PDXF pDxf = pointer to the openning DXF file structure       |
|      PDXFLAYER pLayer = pointer to Layer structure                |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfTable::ReadLayerData(PDXFLAYER pLayer)
{
	ZeroMemory(pLayer, sizeof(DXFLAYER));

	LPTSTR pLaylerData = new TCHAR[1024];
	if (NULL == pLaylerData)
	{
		return FALSE;
	}
	
	memset(pLaylerData, 0, 1024 * sizeof(TCHAR));

	m_pDxfRead->ReadParamFromDxfFile(GCode, pLaylerData);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 2:		// Layer Name
			strcpy_s(pLayer->Name, CW2A(pLaylerData));
			break;
		case 70:	// Standard flags
			pLayer->StandardFlags = char(_wtoi(pLaylerData));
			break;
		case 62:	// Layer Color
			pLayer->Color = _wtoi(pLaylerData);
			break;
		case 6:		// Linetype
			strcpy_s(pLayer->LineType, CW2A(pLaylerData));
			break;
		}
		m_pDxfRead->dxfStorePos();
		m_pDxfRead->ReadParamFromDxfFile(GCode, pLaylerData);
	}

	m_pDxfRead->dxfRestorePos();

	if (NULL != pLaylerData)
	{
		delete[] pLaylerData;
		pLaylerData = NULL;
	}
	return TRUE;
}

/*-------------------------------------------------------------------*
|  ReadLTypeData                                                    |
|  Inputs:                                                          |
|      PDXF pDxf = pointer to the openning DXF file structure       |
|      PDXFLTYPE pLType = pointer to LType structure                |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfTable::ReadLTypeData(PDXFLTYPE pLType)
{
	int		i=0;

	ZeroMemory(pLType, sizeof(DXFLTYPE));

	LPTSTR pLineType = new TCHAR[1024];
	if (NULL == pLineType)
	{

	}

	memset(pLineType, 0, 1024 * sizeof(TCHAR));
	m_pDxfRead->ReadParamFromDxfFile(GCode, pLineType);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 2:		// Linetype Name
			strcpy_s(pLType->Name, CW2A(pLineType));
			break;
		case 70:	// Standard flags
			pLType->StandardFlags = char(_wtoi(pLineType));
			break;
		case 3:		// Descriptive text for linetype
			strcpy_s(pLType->DescriptiveText, CW2A(pLineType));
			break;
		case 73:	// Number of linetype elements
			pLType->ElementsNumber = _wtoi(pLineType);
			break;
		case 40:	// Total pattern length
			pLType->TotalPatternLength = _wtof(pLineType);
			break;
		case 49:	// Dash, dot or space length
			pLType->Elements[i] = _wtof(pLineType);
			i++;
			break;
		}
		m_pDxfRead->dxfStorePos();
		m_pDxfRead->ReadParamFromDxfFile(GCode, pLineType);
	}

	m_pDxfRead->dxfRestorePos();

	if (NULL != pLineType)
	{
		delete[] pLineType;
		pLineType = NULL;
	}
	return TRUE;
}

/*-------------------------------------------------------------------*
|  ReadStyleData                                                    |
|  Inputs:                                                          |
|      PDXF pDxf = pointer to the openning DXF file structure       |
|      PDXFSTYLE pStyle = pointer to Style structure                |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfTable::ReadStyleData( PDXFSTYLE pStyle)
{

	ZeroMemory(pStyle, sizeof(DXFSTYLE));

	LPTSTR pStyleData = new TCHAR[1024];
	if (NULL == pStyleData)
	{

	}

	memset(pStyleData, 0, sizeof(TCHAR) * 1024);
	m_pDxfRead->ReadParamFromDxfFile(GCode, pStyleData);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 2:		// Style Name
			strcpy_s(pStyle->Name, CW2A(pStyleData));
			break;
		case 3:		// Primary Font Filename
			strcpy_s(pStyle->PrimaryFontFilename, CW2A(pStyleData));
			break;
		case 70:	// Standard flags
			pStyle->StandardFlags = char(_wtoi(pStyleData));
			break;
		case 71:	// Text generation flags -> 2=Text is backward  4=Text is upside down
			pStyle->TextGenerationFlags = _wtoi(pStyleData);
			break;
		case 40:	// Fixed text height
			pStyle->FixedTextHeight = _wtof(pStyleData);
			break;
		case 41:	// Width Factor
			pStyle->WidthFactor = _wtof(pStyleData);
			break;
		case 42:	// Height
			pStyle->Height = _wtof(pStyleData);
			break;
		case 50:	// Oblique angle
			pStyle->ObliqueAngle = _wtof(pStyleData);
			break;
		}
		m_pDxfRead->dxfStorePos();
		m_pDxfRead->ReadParamFromDxfFile(GCode, pStyleData);
	}

	m_pDxfRead->dxfRestorePos();

	if (NULL != pStyleData)
	{
		delete[] pStyleData;
		pStyleData = NULL;
	}
	return TRUE;
}

/*-------------------------------------------------------------------*
|  dxfReadTableType                                                 |
|  Inputs:                                                          |
|      HDXF hDxf = handle to the openning DXF file structure        |
|      LPVOID pTableType = pointer to table type structure          |
|  Output: TableType Code else TAB_NOTSET                           |
*-------------------------------------------------------------------*/
DWORD CJetDxfTable::dxfReadTableType(LPVOID pTableType)
{
	DWORD	result;

	// Initialize pDxf ------------------
	if (NULL == m_pDxfRead->pDxf)
		return FALSE;

	// Check if current section is TABLES
	if (m_pDxfRead->pDxf->Read.CurrentSection != SEC_TABLES)
	{
		// Current section is not TABLES
		return TAB_NOTSET;
	}

	// Looking for next table type if CurrentTableType==TAB_NOTSET
	if (m_pDxfRead->pDxf->Read.CurrentTableType == TAB_NOTSET)
	{
		if (dxfFindNextTableType() == TAB_NOTSET)
		{
			m_pDxfRead->pDxf->Read.CurrentSection = SEC_NOTSET;
			return TAB_NOTSET;
		}
	}

	// Read current table type data
	result = TAB_NOTSET;
	switch (m_pDxfRead->pDxf->Read.CurrentTableType)
	{
	case TAB_DIMSTYLE:
		if (!m_pDxfRead->FindParamFromDxfFile(0, _T("DIMSTYLE")))
			break;
		else
		{
			if (ReadDimStyleData((PDXFDIMSTYLE)pTableType))
				result = TAB_DIMSTYLE;
		}
		break;

	case TAB_LAYER:
		if (!m_pDxfRead->FindParamFromDxfFile(0, _T("LAYER")))
			break;
		else
		{
			if (ReadLayerData((PDXFLAYER)pTableType))
				result = TAB_LAYER;
		}
		break;

	case TAB_LTYPE:
		if (!m_pDxfRead->FindParamFromDxfFile(0, _T("LTYPE")))
			break;
		else
		{
			if (ReadLTypeData((PDXFLTYPE)pTableType))
				result = TAB_LTYPE;
		}
		break;

	case TAB_STYLE:
		if (!m_pDxfRead->dxfFindParam(0, _T("STYLE")))
			break;
		else
		{
			if (ReadStyleData((PDXFSTYLE)pTableType))
				result = TAB_STYLE;
		}
		break;
	}

	LPTSTR  pTableData = new TCHAR[1024];
	if (NULL == pTableData)
	{

	}

	memset(pTableData, 0, sizeof(TCHAR) * 1024);
	m_pDxfRead->dxfStorePos();
	m_pDxfRead->dxfReadParam(GCode, pTableData);

	if ((GCode == 0) && (strcmp(CW2A(pTableData), "ENDTAB") == 0))
	{
		m_pDxfRead->pDxf->Read.CurrentTableType = TAB_NOTSET;

		//		while(dxfReadTableTypeName(hDxf)==TAB_NOTSET)
		//		{
		m_pDxfRead->dxfStorePos();
		m_pDxfRead->dxfReadParam(GCode, pTableData);
		if ((GCode == 0) && (strcmp(CW2A(pTableData), "ENDSEC") == 0))
		{
			m_pDxfRead->pDxf->Read.CurrentSection = SEC_NOTSET; // Tables section has been finished
			//				break;
		}
		else
			m_pDxfRead->dxfRestorePos();
		//		}
	}
	else
		m_pDxfRead->dxfRestorePos();


	if (NULL != pTableData)
	{
		delete[] pTableData;
		pTableData = NULL;
	}
	return result;
}

bool CJetDxfTable::dxfWriteData(HDXF pDxf)
{
	return FALSE;
}