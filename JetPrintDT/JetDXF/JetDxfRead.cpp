#include "JetDxfRead.h"
#include "JetDxfBlock.h"
#include "JetDxfEntities.h"
#include "JetDxfTable.h"
#include "JetDxfView.h"

CJetDxfRead::CJetDxfRead()
{
	hDxf = NULL;
	pDxf = NULL;
	pStream = NULL;
	m_pDxfTable = new CJetDxfTable(this);
	m_pDxfBlock = new CJetDxfBlock(this);
	m_pDxfEntity = new CJetDxfEntities(this);
}


CJetDxfRead::~CJetDxfRead()
{
	if(NULL != m_pDxfTable)
	{
		delete m_pDxfTable;
		m_pDxfTable = NULL;
	}

	if(NULL != m_pDxfBlock)
	{
		delete m_pDxfBlock;
		m_pDxfBlock = NULL;
	}

	if(NULL != m_pDxfEntity)
	{
		delete m_pDxfEntity;
		m_pDxfEntity = NULL;
	}
}


bool CJetDxfRead::InitView(CJetDxfView *pView)
{
	if (NULL != pView)
	{
		m_pDxfView = pView;

		return true;
	}

	return false;
}

HDXF CJetDxfRead::dxfOpenFile(LPCTSTR FileName)
{
	if ((0!=_wfopen_s(&pStream,FileName, _T("rb"))))
	{
		// Cannot create new file
		return NULL;
	}

	hDxf = (HDXF)GlobalAlloc(GHND, sizeof(DXF));
	if (hDxf == NULL)
	{
		// GlobalAlloc error
		return NULL;
	}

	pDxf = (PDXF)GlobalLock(hDxf);
	if (pDxf == NULL)
	{
		// GlobalLock error
		GlobalFree(hDxf);
		return NULL;
	}
	pDxf->pStream = pStream;
	pDxf->Size = sizeof(DXF);
	pDxf->isWriteMode = FALSE;

	pDxf->Read.CurrentSection = SEC_NOTSET;
	pDxf->Read.CurrentTableType = TAB_NOTSET;
	pDxf->Read.isBlockOpen = FALSE;
	pDxf->Read.StoredStreamPos = 0;

	fseek(pStream, 0, SEEK_END);
	pDxf->Read.FileSize = ftell(pStream);
	rewind(pStream);

	pDxf->Read.CurrentPos = 0;
	pDxf->Read.BufferPos = 0;
	pDxf->Read.BufferDataSize = fread(pDxf->Read.ReadBuffer, 1, READBUFFERSIZE, pDxf->pStream);

	GlobalUnlock(hDxf);

	return hDxf;
}

bool CJetDxfRead::dxfCloseFile()
{
    if(NULL != pStream)
	{
		fclose(pStream);
	}

	return true;
}

/********************************************************************
* Sections Functions                                               *
********************************************************************/

/*-------------------------------------------------------------------*
|  dxfFindNextSection                                               |
|  Inputs:                                                          |
|      HDXF hDxf = handle to the openning DXF file structure        |
|  Output: section code if everything is ok else SEC_NOTSET         |
*-------------------------------------------------------------------*/
DWORD CJetDxfRead::dxfFindNextSection()
{
	int		GCode = 0;
	CString	SectionName = _T("");
	CString strSection = _T("SECTION");
	
	// Initialize pDxf ------------------
	if (NULL == pDxf)
	{
		return FALSE;
	}
	

	// Set CurrentSection to SEC_NOTSET
	pDxf->Read.CurrentSection = SEC_NOTSET;

	// Search for Section
	if (!dxfFindParam(0, strSection))
	{
		// Invalid Dxf file format
		return SEC_NOTSET;
	}

	LPTSTR pSectionName = new TCHAR[1024];

	if (NULL == pSectionName)
	{
		return FALSE;
	}

	if (pDxf->Read.CurrentPos<pDxf->Read.FileSize)
	{
		dxfReadParam(GCode, pSectionName);
		SectionName = pSectionName;
		if (SectionName == _T("HEADER"))
			pDxf->Read.CurrentSection = SEC_HEADER;
		else if (SectionName==_T("TABLES"))
			pDxf->Read.CurrentSection = SEC_TABLES;
		else if (SectionName== _T ("BLOCKS"))
			pDxf->Read.CurrentSection = SEC_BLOCKS;
		else if (SectionName==_T("ENTITIES"))
			pDxf->Read.CurrentSection = SEC_ENTITIES;
		else
			pDxf->Read.CurrentSection = SEC_UNKNOWN;
	}

	if (NULL != pSectionName)
	{
		delete []pSectionName;
		pSectionName = NULL;
	}

	return pDxf->Read.CurrentSection;
}

/*-------------------------------------------------------------------*
|  dxfGetCurrentSection                                             |
|  Inputs:                                                          |
|      HDXF hDxf = handle to the openning DXF file structure        |
|  Output: current section code if everything is ok                 |
*-------------------------------------------------------------------*/
DWORD CJetDxfRead::dxfGetCurrentSection()
{

	DWORD	result;

	// Initialize pDxf ------------------
	if (NULL == pDxf)
	{
		return FALSE;
	}
	

	result = pDxf->Read.CurrentSection;

	
	return result;
}

PDXF CJetDxfRead::InitilizePDXF(HDXF hDxf)
{

	if (hDxf == NULL)
	{
		// No DXF file is open
		return NULL;
	}

	pDxf = (PDXF)GlobalLock(hDxf);
	if (pDxf == NULL)
	{
		// GlobalLock error
		return NULL;
	}

	return pDxf;
}


BOOL CJetDxfRead::UnInitilizePDXF(HDXF hDxf)
{
	// Unlock hDxf ----------------------
	GlobalUnlock(hDxf);
	if (!GlobalUnlock(hDxf) && (GetLastError() != NO_ERROR))
	{
		// memory error
		return FALSE;
	}

	return TRUE;
}

void CJetDxfRead::ReadLine(LPTSTR valuestr)
{
	char ch;
	int  i = 0;

	while (pDxf->Read.CurrentPos<pDxf->Read.FileSize)
	{
		ch = pDxf->Read.ReadBuffer[pDxf->Read.CurrentPos - pDxf->Read.BufferPos];
		if (ch == 0x0d)
		{
			valuestr[i] = 0;
			break;
		}

		valuestr[i++] = ch;

		pDxf->Read.CurrentPos++;
		if ((pDxf->Read.CurrentPos - pDxf->Read.BufferPos) >= pDxf->Read.BufferDataSize)
		{
			pDxf->Read.BufferPos += pDxf->Read.BufferDataSize;
			pDxf->Read.BufferDataSize = fread(pDxf->Read.ReadBuffer, 1, READBUFFERSIZE, pDxf->pStream);
		}
	}

	pDxf->Read.CurrentPos++;
	if ((pDxf->Read.CurrentPos - pDxf->Read.BufferPos) >= pDxf->Read.BufferDataSize)
	{
		pDxf->Read.BufferPos += pDxf->Read.BufferDataSize;
		pDxf->Read.BufferDataSize = fread(pDxf->Read.ReadBuffer, 1, READBUFFERSIZE, pDxf->pStream);
	}

	ch = pDxf->Read.ReadBuffer[pDxf->Read.CurrentPos - pDxf->Read.BufferPos];
	if (ch == 0x0a)
	{
		pDxf->Read.CurrentPos++;
		if ((pDxf->Read.CurrentPos - pDxf->Read.BufferPos) >= pDxf->Read.BufferDataSize)
		{
			pDxf->Read.BufferPos += pDxf->Read.BufferDataSize;
			pDxf->Read.BufferDataSize = fread(pDxf->Read.ReadBuffer, 1, READBUFFERSIZE, pDxf->pStream);
		}
	}
}

/*-------------------------------------------------------------------*
|  ReadParamFromDxfFile                                             |
|  Inputs:                                                          |
|      PDXF pDxf = pointer to the openning DXF file structure       |
|      int& GroupCode = group code                                  |
|      LPTSTR valuestr = pointer to null terminated string          |
|                       to put data in it                           |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfRead::ReadParamFromDxfFile(int&	GroupCode, LPTSTR valuestr)
{
	LPTSTR strGroupCode = NULL;
	strGroupCode = new TCHAR[1024];
	if (NULL == strGroupCode)
	{
		return FALSE;
	}

	ReadLine(strGroupCode);
	GroupCode = _wtoi(strGroupCode);
	ReadLine(valuestr);


	if (NULL != strGroupCode)
	{
		delete []strGroupCode;
		strGroupCode = NULL;
	}

	return TRUE;
}


/*-------------------------------------------------------------------*
|  dxfReadParam                                                     |
|  Inputs:                                                          |
|      HDXF hDxf = handle to the openning DXF file structure        |
|      int& GroupCode = group code                                  |
|      LPTSTR valuestr = pointer to null terminated string          |
|                       to put data in it                           |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfRead::dxfReadParam(int& GroupCode, LPTSTR valuestr)
{

	if (hDxf == NULL)
	{
		// No DXF file is open
		return FALSE;
	}

	if (pDxf == NULL)
	{
		// GlobalLock error
		return FALSE;
	}

	LPTSTR strGroupCode = NULL;
	strGroupCode = new TCHAR[1024];
	if (NULL == strGroupCode)
	{
		return FALSE;
	}

	ReadLine(strGroupCode);
	GroupCode = _wtoi(strGroupCode);
	ReadLine(valuestr);


	if (NULL != strGroupCode)
	{
		delete []strGroupCode;
		strGroupCode = NULL;
	}

	return TRUE;
}

/*-------------------------------------------------------------------*
|  FindParamFromDxfFile                                             |
|  Inputs:                                                          |
|      PDXF pDxf = pointer to the openning DXF file structure       |
|      int GroupCode = group code                                   |
|      LPCTSTR valuestr = pointer to null terminated string to find |
|  Output: TRUE if found else FALSE                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfRead::FindParamFromDxfFile(int GroupCode, LPCTSTR valuestr)
{
	int		groupcode = 0;
	BOOL	found = FALSE;
	CString strValue = valuestr;
	CString strTemp = _T("");

	LPTSTR pGroupCode = new TCHAR[256];
	if (NULL == pGroupCode)
	{
		return FALSE;
	}

	dxfStorePos();
	while (pDxf->Read.CurrentPos<pDxf->Read.FileSize)
	{
		/*		if(fscanf(pDxf->pStream, "%d", &groupcode)<=0)
		{
		// file read error
		break;
		}

		ReadLine(pDxf, value);*/

		ReadLine(pGroupCode);
		groupcode = _wtoi(pGroupCode);
		ReadLine(pGroupCode);

		strTemp = pGroupCode;
		if ((groupcode == GroupCode) && (strTemp == strValue))
		{
			found = TRUE;
			break;
		}
	}

	if (!found)
		dxfRestorePos();

	if (NULL != pGroupCode)
	{
		delete[] pGroupCode;
		pGroupCode = NULL;
	}

	return found;
}

/*-------------------------------------------------------------------*
|  dxfFindParam                                                     |
|  Inputs:                                                          |
|      HDXF hDxf = handle to the openning DXF file structure        |
|      int GroupCode = group code                                   |
|      LPCTSTR valuestr = pointer to null terminated string to find |
|  Output: TRUE if found else FALSE                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfRead::dxfFindParam(int GroupCode, LPCTSTR valuestr)
{
	int		groupcode = 0;
	BOOL	found = false;
	CString strValue = _T("");
	CString strTemp = valuestr;
	// Initialize pDxf ------------------
	if (pDxf== NULL)
		return FALSE;
	
	found = FALSE;
	dxfStorePos();

	LPTSTR pGroupCode = new TCHAR[256];
	if (NULL == pGroupCode)
		return false;

	while (pDxf->Read.CurrentPos<pDxf->Read.FileSize)
	{
		
		ReadLine(pGroupCode);
		groupcode = _wtoi(pGroupCode);
		ReadLine(pGroupCode);

		strValue.Format(_T("%s"), pGroupCode);
		//strOut = pGroupCode;


		if ((groupcode == GroupCode) && (strValue.Compare(strTemp) == 0))
		{
			found = TRUE;
			
			break;
		}

	}

	if (!found)
		dxfRestorePos();
	//释放内存
	if (NULL != pGroupCode)
	{
		memset(pGroupCode, 0, sizeof(pGroupCode));
		delete []pGroupCode;
		pGroupCode = NULL;
	}


	return found;
}

/*-------------------------------------------------------------------*
|  dxfStorePos                                                      |
|  Inputs:                                                          |
|      PDXF pDxf = pointer to the openning DXF file structure       |
|  Output: TRUE if found else FALSE                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfRead::dxfStorePos()
{
	if (pDxf == NULL)
		return FALSE;

	pDxf->Read.StoredStreamPos = pDxf->Read.CurrentPos/*ftell(pDxf->pStream)*/;

	return TRUE;
}

/*-------------------------------------------------------------------*
|  dxfRestorePos                                                    |
|  Inputs:                                                          |
|      PDXF pDxf = pointer to the openning DXF file structure       |
|  Output: TRUE if found else FALSE                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfRead::dxfRestorePos()
{
	if (pDxf == NULL)
		return FALSE;

	pDxf->Read.CurrentPos = pDxf->Read.StoredStreamPos;
	if (((pDxf->Read.CurrentPos - pDxf->Read.BufferPos)<0) || ((pDxf->Read.CurrentPos - pDxf->Read.BufferPos) >= pDxf->Read.BufferDataSize))
	{
		fseek(pDxf->pStream, pDxf->Read.StoredStreamPos, SEEK_SET);
		pDxf->Read.BufferDataSize = fread(pDxf->Read.ReadBuffer, 1, READBUFFERSIZE, pDxf->pStream);
		pDxf->Read.BufferPos = pDxf->Read.StoredStreamPos;
	}

	return TRUE;
}

BOOL CJetDxfRead::LoadDataFromFile(OBJHANDLE BlockObjhandle,LPCTSTR strFileName)
{
	
	DWORD		dwCurrentSection;
	DWORD		dwCurrentTableType;

	DXFDIMSTYLE	DxfDimStyle;
	DXFLAYER	DxfLayer;
	DXFLTYPE	DxfLType;
	DXFSTYLE	DxfStyle;
	DIMSTYLE	DimStyle;
	LAYER		Layer;
	LTYPE		LType;
	STYLE		Style;


	if (strFileName == NULL)
	{
		// Filename can not be NULL
		return FALSE;
	}

	// Open current dxf file
	hDxf = dxfOpenFile(strFileName);

	// Initialize pDxf ------------------
	if (NULL == pDxf)
	{
		if (NULL == (pDxf=InitilizePDXF(hDxf)))
		     return FALSE;
	}

	// Set Progress Percent to 0

	dwCurrentSection = dxfFindNextSection();
	while (dwCurrentSection != SEC_NOTSET)
	{
		switch (dwCurrentSection)
		{
		case SEC_HEADER:	// Header Section
			break;

		case SEC_TABLES:	// Tables Section
			do{
				if ((dwCurrentTableType = m_pDxfTable->dxfFindNextTableType()) == TAB_NOTSET)
					break;
				switch (dwCurrentTableType)
				{
				case TAB_DIMSTYLE:	// DimStyle TableType
					do{
						if (m_pDxfTable->dxfReadTableType(&DxfDimStyle) != TAB_DIMSTYLE)
							break;

						strcpy_s(DimStyle.Name, DxfDimStyle.Name);
						DimStyle.StandardFlags = DxfDimStyle.StandardFlags;
						DimStyle.dimasz = DxfDimStyle.DIMASZ;
						strcpy_s(DimStyle.dimblk1, DxfDimStyle.DIMBLK1);
						strcpy_s(DimStyle.dimblk2, DxfDimStyle.DIMBLK2);
						DimStyle.dimclrd = DxfDimStyle.DIMCLRD;
						DimStyle.dimclre = DxfDimStyle.DIMCLRE;
						DimStyle.dimclrt = DxfDimStyle.DIMCLRT;
						DimStyle.dimdle = DxfDimStyle.DIMDLE;
						DimStyle.dimexe = DxfDimStyle.DIMEXE;
						DimStyle.dimexo = DxfDimStyle.DIMEXO;
						DimStyle.dimgap = DxfDimStyle.DIMGAP;
						DimStyle.dimtxt = DxfDimStyle.DIMTXT;
						DimStyle.dimtad = DxfDimStyle.DIMTAD;

						strcpy_s(Style.Name, DxfDimStyle.DIMTXSTY);
						
						if (m_pDxfView->m_pJetDxfDimView->drwFindTableType_Direct(TAB_STYLE, FIND_BYNAME, &Style) > 0)
							DimStyle.dimtxstyObjhandle = Style.Objhandle;
						else
						{
							// Text style is unavailable
							// Using default text style
						}

						m_pDxfView->m_pJetDxfDimView->drwAddTableType(TAB_DIMSTYLE, &DimStyle);

					} while (m_pDxfTable->dxfGetCurrentTableType() == TAB_DIMSTYLE);
					break;

				case TAB_LAYER:		// Layer TableType
					do{
						if (m_pDxfTable->dxfReadTableType(&DxfLayer) != TAB_LAYER)
							break;

						strcpy_s(Layer.Name, DxfLayer.Name);
						Layer.StandardFlags = DxfLayer.StandardFlags;
						Layer.Color = DxfLayer.Color;
						//						Layer.LineWeight = DxfLayer.LineWeight;
						//						Layer.PlotFlag = DxfLayer.PlotFlag;
						//						Layer.PlotStyleObjhandle;

						strcpy_s(LType.Name, DxfLayer.LineType);
					
						if (m_pDxfView->m_pJetDxfDimView->drwFindTableType_Direct(TAB_LTYPE, FIND_BYNAME, &LType) > 0)
							Layer.LineTypeObjhandle = LType.Objhandle;
						else
						{
							// Line type is unavailable
							//dxfCloseFile(hDxf);
							//GlobalUnlock(hDrawing);
							return FALSE;
						}
						m_pDxfView->m_pJetDxfDimView->drwAddTableType(TAB_LAYER, &Layer);
					} while (m_pDxfTable->dxfGetCurrentTableType() == TAB_LAYER);
					break;

				case TAB_LTYPE:		// LType TableType
					do{
						if (m_pDxfTable->dxfReadTableType(&DxfLType) != TAB_LTYPE)
							break;

						strcpy_s(LType.Name, DxfLType.Name);
						LType.StandardFlags = DxfLType.StandardFlags;
						strcpy_s(LType.DescriptiveText, DxfLType.DescriptiveText);
						LType.ElementsNumber = DxfLType.ElementsNumber;
						memcpy(LType.Elements, DxfLType.Elements, sizeof(double) * 30);			// Line Elements (Max=30)
						LType.PatternLength = DxfLType.TotalPatternLength;

						m_pDxfView->m_pJetDxfDimView->drwAddTableType(TAB_LTYPE, &LType);
					} while (m_pDxfTable->dxfGetCurrentTableType() == TAB_LTYPE);
					break;

				case TAB_STYLE:		// Style TableType
					do{
						if (m_pDxfTable->dxfReadTableType(&DxfStyle) != TAB_STYLE)
							break;

						strcpy_s(Style.Name, DxfStyle.Name);
						Style.StandardFlags = DxfStyle.StandardFlags;
						Style.TextGenerationFlags = DxfStyle.TextGenerationFlags;
						strcpy_s(Style.PrimaryFontFilename, DxfStyle.PrimaryFontFilename);
						//						strcpy_s(Style.BigFontFilename, DxfStyle.BigFontFilename);
						Style.ObliqueAngle = DxfStyle.ObliqueAngle;
						Style.FixedTextHeight = DxfStyle.FixedTextHeight;
						Style.WidthFactor = DxfStyle.WidthFactor;
						Style.LastHeightUsed = DxfStyle.Height;

						m_pDxfView->m_pJetDxfDimView->drwAddTableType(TAB_STYLE, &Style);
					} while (m_pDxfTable->dxfGetCurrentTableType() == TAB_STYLE);
					break;
				}

			} while (dxfGetCurrentSection() == SEC_TABLES);
			break;

		case SEC_BLOCKS:	// Blocks Section
			DXFBLOCKHEADER	DxfBlockHeader;
			BLOCKHEADER		BlockHeader;
			OBJHANDLE       BlockHandle;

			do{
				if (m_pDxfBlock->dxfFindBlockHeader(&DxfBlockHeader))
				{
					// Adding new block
					ZeroMemory(&BlockHeader, sizeof(BLOCKHEADER));
					strcpy_s(BlockHeader.Name, DxfBlockHeader.Name);
					BlockHeader.Flags = DxfBlockHeader.Flags;
					BlockHeader.BasePoint = DxfBlockHeader.BasePoint;

					strcpy_s(Layer.Name, DxfBlockHeader.LayerName);
					if (m_pDxfView->m_pJetDxfDimView->drwFindTableType_Direct(TAB_LAYER, FIND_BYNAME, &Layer) > 0)
						BlockHeader.LayerObjhandle = Layer.Objhandle;
					else
					{
						// Layer is unavailable
						// Using default layer "0"
					}

					BlockHandle = m_pDxfView->m_pJetDxfDimView->drwAddBlock_Direct(&BlockHeader);

					// Add new block's entities data
					ReadEntitiesDataFromFile(BlockHandle);
				}
				else
					break;
			} while (dxfGetCurrentSection() == SEC_BLOCKS);
			break;

		case SEC_ENTITIES:	// Entities Section
			ReadEntitiesDataFromFile(BlockObjhandle);
			break;
		}

		dwCurrentSection = dxfFindNextSection();
	}

	// Set Progress Percent to 100
	
	// UnInitilize hDxf -----------------
	UnInitilizePDXF(hDxf);

	dxfCloseFile();

	return true;
}

void CJetDxfRead::ReadEntitiesDataFromFile(OBJHANDLE BlockObjHandle)
{
	UINT nPointIndex = 0;
	DXFENTITYHEADER	DxfEntityHeader;
	ENTITYHEADER	EntityHeader;
	char			DxfEntityData[4096];
	DIMSTYLE		DimStyle;

	LAYER			Layer;
	LTYPE			LType;
	STYLE			Style;

	ENTLINE			Line;
	ENTARC			Arc;
	ENTCIRCLE		Circle;
	ENTELLIPSE		Ellipse;
	ENTDIMENSION	Dimension;
	ENTINSERT		Insert;
	ENTPOINT		Point;
	ENTSOLID		Solid;
	ENTTEXT			Text;
	ENTPOLYLINE		Polyline;
	ENTLWPOLYLINE   LWPolyline;
	ENTSPLINE       Spline;
	UINT i = 0;
	char			buf[512];

	while (m_pDxfEntity->dxfReadDataDirect(&DxfEntityHeader,&DxfEntityData))
	{
		ZeroMemory(&EntityHeader, sizeof(ENTITYHEADER));
		EntityHeader.EntityType = DxfEntityHeader.EntityType;
		EntityHeader.Color = DxfEntityHeader.Color;
		EntityHeader.Thickness = DxfEntityHeader.Thickness;
		EntityHeader.LineTypeScale = DxfEntityHeader.LineTypeScale;
		memcpy(EntityHeader.ExtrusionDirection, DxfEntityHeader.ExtrusionDirection, sizeof(double) * 3);
		EntityHeader.LineWeight = DxfEntityHeader.LineWeight;

		// Looking for layer name
		if (strlen(DxfEntityHeader.LayerName)>0)
		{
			strcpy_s(Layer.Name, DxfEntityHeader.LayerName);
			if (m_pDxfView->m_pJetDxfDimView->drwFindTableType_Direct(TAB_LAYER, FIND_BYNAME, &Layer) > 0)
				EntityHeader.LayerObjhandle = Layer.Objhandle;
			else
			{
				//Layler is unavailable
			}
		}
		else
		{
			// Set to default layer "0"
		}

		// Looking for line type name
		if (strlen(DxfEntityHeader.LTypeName)>0)
		{
			strcpy_s(LType.Name, DxfEntityHeader.LTypeName);
			if (m_pDxfView->m_pJetDxfDimView->drwFindTableType_Direct(TAB_LTYPE, FIND_BYNAME, &LType) > 0)
				EntityHeader.LTypeObjhandle = LType.Objhandle;
			else
			{
				// LType is unavailable
			}
		}
		else
		{
			// Set to default ltype
		}

		switch (DxfEntityHeader.EntityType)
		{
		case ENT_LINE:
			Line.Point0 = ((PDXFENTLINE)&DxfEntityData)->Point0;
			Line.Point1 = ((PDXFENTLINE)&DxfEntityData)->Point1;
	        
			m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &Line);
			break;

		case ENT_POINT:
			Point.Point0 = ((PDXFENTPOINT)&DxfEntityData)->Point0;
			m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &Point);
			break;

		case ENT_CIRCLE:
			Circle.Point0 = ((PDXFENTCIRCLE)&DxfEntityData)->Point0;
			Circle.Radius = ((PDXFENTCIRCLE)&DxfEntityData)->Radius;
			m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &Circle);
			break;

	    case ENT_ELLIPSE:
			Ellipse.CenterPoint = ((PDXFENTELLIPSE)&DxfEntityData)->CenterPoint;
			Ellipse.MajorAxisEndPoint = ((PDXFENTELLIPSE)&DxfEntityData)->MajorAxisEndPoint;
			Ellipse.MinorToMajorRatio = ((PDXFENTELLIPSE)&DxfEntityData)->MinorToMajorRatio;
			Ellipse.StartParam = ((PDXFENTELLIPSE)&DxfEntityData)->StartParam;
			Ellipse.EndParam = ((PDXFENTELLIPSE)&DxfEntityData)->EndParam;
			m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &Ellipse);
			break;


		case ENT_TEXT:
			ZeroMemory(&Text, sizeof(ENTTEXT));
			Text.Point0 = ((PDXFENTTEXT)&DxfEntityData)->Point0;
			strcpy_s(Text.strText, ((PDXFENTTEXT)&DxfEntityData)->strText);
			Text.TextData.Height = ((PDXFENTTEXT)&DxfEntityData)->TextData.Height;
			Text.TextData.RotationAngle = ((PDXFENTTEXT)&DxfEntityData)->TextData.RotationAngle;
			Text.TextData.WidthFactor = ((PDXFENTTEXT)&DxfEntityData)->TextData.WidthFactor;
			Text.TextData.Oblique = ((PDXFENTTEXT)&DxfEntityData)->TextData.Oblique;
			Text.TextData.GenerationFlag = ((PDXFENTTEXT)&DxfEntityData)->TextData.GenerationFlag;
			Text.TextData.Justification = ((PDXFENTTEXT)&DxfEntityData)->TextData.Justification;
			Text.TextData.SecondAlignmentPoint = ((PDXFENTTEXT)&DxfEntityData)->TextData.SecondAlignmentPoint;
			// Looking for text style name
			if (strlen(((PDXFENTTEXT)&DxfEntityData)->TextData.TextStyleName)>0)
			{
				strcpy_s(Style.Name, ((PDXFENTTEXT)&DxfEntityData)->TextData.TextStyleName);
				if (m_pDxfView->m_pJetDxfDimView->drwFindTableType_Direct(TAB_STYLE, FIND_BYNAME, &Style) > 0)
					Text.TextData.TextStyleObjhandle = Style.Objhandle;
				else
				{
					//style is unavailable
				}
			}
			else
			{
				// Set to default style
			}

			m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &Text);
			break;

		case ENT_ARC:
			Arc.Point0 = ((PDXFENTARC)&DxfEntityData)->Point0;
			Arc.Radius = ((PDXFENTARC)&DxfEntityData)->Radius;
			Arc.StartAngle = ((PDXFENTARC)&DxfEntityData)->StartAngle;
			Arc.EndAngle = ((PDXFENTARC)&DxfEntityData)->EndAngle;
			m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &Arc);
			break;

		case ENT_SOLID:
			Solid.Point0 = ((PDXFENTSOLID)&DxfEntityData)->Point0;
			Solid.Point1 = ((PDXFENTSOLID)&DxfEntityData)->Point1;
			Solid.Point2 = ((PDXFENTSOLID)&DxfEntityData)->Point2;
			Solid.Point3 = ((PDXFENTSOLID)&DxfEntityData)->Point3;
			m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &Solid);
			break;

		case ENT_INSERT:
			BLOCKHEADER	BlockHeader;
			Insert.Point0 = ((PDXFENTINSERT)&DxfEntityData)->Point0;
			Insert.XScale = ((PDXFENTINSERT)&DxfEntityData)->XScale;
			Insert.YScale = ((PDXFENTINSERT)&DxfEntityData)->YScale;
			Insert.ZScale = ((PDXFENTINSERT)&DxfEntityData)->ZScale;
			Insert.RotationAngle = ((PDXFENTINSERT)&DxfEntityData)->RotationAngle;

			strcpy_s(BlockHeader.Name, ((PDXFENTINSERT)&DxfEntityData)->BlockHeaderName);
			if ((Insert.BlockHeaderObjhandle = m_pDxfView->m_pJetDxfDimView->drwFindBlock_Direct( FIND_BYNAME, &BlockHeader)) > 0)
				m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &Insert);
			else
			{
				// Block name not found
			}
			break;

		case ENT_POLYLINE:
			Polyline.nVertex = ((PENTPOLYLINE)&DxfEntityData)->nVertex;
			Polyline.pVertex = ((PENTPOLYLINE)&DxfEntityData)->pVertex;
			Polyline.Flag = ((PENTPOLYLINE)&DxfEntityData)->Flag;
			
			m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &Polyline);
			delete ((PENTPOLYLINE)&DxfEntityData)->pVertex;
			break;

		case ENT_LWPOLYLINE:
			
			LWPolyline.b_PolylineClose = ((PDXFENTLWPOLYLINE)&DxfEntityData)->b_PolylineClose;
			LWPolyline.CountOfVertex = ((PDXFENTLWPOLYLINE)&DxfEntityData)->CountOfVertex;
			nPointIndex = LWPolyline.CountOfVertex;

			LWPolyline.pVertexPoint = new REALPOINT[nPointIndex];
			if (NULL == LWPolyline.pVertexPoint)
			{
				//内存分配失败，do something
			}
			memset(LWPolyline.pVertexPoint, 0, sizeof(REALPOINT)*nPointIndex);

			for (i = 0; i < nPointIndex; i++)
			{
				LWPolyline.pVertexPoint[i].x = ((PDXFENTLWPOLYLINE)&DxfEntityData)->pCoordinateX[i];
				LWPolyline.pVertexPoint[i].y = ((PDXFENTLWPOLYLINE)&DxfEntityData)->pCoordinateY[i];
				LWPolyline.pVertexPoint[i].z = 0;
			}

			m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &LWPolyline);

			if (NULL != ((PDXFENTLWPOLYLINE)&DxfEntityData)->pCoordinateX)
			{
				delete[]((PDXFENTLWPOLYLINE)&DxfEntityData)->pCoordinateX;
				((PDXFENTLWPOLYLINE)&DxfEntityData)->pCoordinateX = NULL;
			}

			if (NULL != ((PDXFENTLWPOLYLINE)&DxfEntityData)->pCoordinateY)
			{
				delete[]((PDXFENTLWPOLYLINE)&DxfEntityData)->pCoordinateY;
				((PDXFENTLWPOLYLINE)&DxfEntityData)->pCoordinateY = NULL;
			}
			break;

		case ENT_SPLINE:
			Spline.SplineRank = ((PDXFENTSPLINE)&DxfEntityData)->SplineRank;
			Spline.SplineKnots = ((PDXFENTSPLINE)&DxfEntityData)->SplineKnots;
			Spline.SplineControlPoints = ((PDXFENTSPLINE)&DxfEntityData)->SplineControlPoints;
			Spline.SplineMatchPoints = ((PDXFENTSPLINE)&DxfEntityData)->SplineMatchPoint;
			Spline.pKnotPoint = new double[Spline.SplineKnots];
			if (NULL == Spline.pKnotPoint)
			{
				//内存分配失败，do something;
			}
			for (i = 0; i < Spline.SplineKnots; i++)
			{
				Spline.pKnotPoint[i] = ((PDXFENTSPLINE)&DxfEntityData)->pKontPoint[i];
			}


			Spline.pControlPoint = new REALPOINT[Spline.SplineControlPoints];
			if (NULL == Spline.pControlPoint)
			{
				//内存分配失败，do something;
			}
			for (i = 0; i < Spline.SplineControlPoints; i++)
			{
				Spline.pControlPoint[i].x = ((PDXFENTSPLINE)&DxfEntityData)->pControlPoint_X[i];
				Spline.pControlPoint[i].y = ((PDXFENTSPLINE)&DxfEntityData)->pControlPoint_Y[i];
				Spline.pControlPoint[i].z = 0.0;
			}

			m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &Spline);
			if (NULL != ((PDXFENTSPLINE)&DxfEntityData)->pControlPoint_X)
			{
				delete[]((PDXFENTSPLINE)&DxfEntityData)->pControlPoint_X;
				((PDXFENTSPLINE)&DxfEntityData)->pControlPoint_X = NULL;
			}

			if (NULL != ((PDXFENTSPLINE)&DxfEntityData)->pControlPoint_Y)
			{
				delete[]((PDXFENTSPLINE)&DxfEntityData)->pControlPoint_Y;
				((PDXFENTSPLINE)&DxfEntityData)->pControlPoint_Y = NULL;
			}

			if (NULL != ((PDXFENTSPLINE)&DxfEntityData)->pKontPoint)
			{
				delete[]((PDXFENTSPLINE)&DxfEntityData)->pKontPoint;
				((PDXFENTSPLINE)&DxfEntityData)->pKontPoint = NULL;
			}

			break;

		case ENT_DIMENSION:
			BLOCKHEADER	DimBlockHeader;
			ZeroMemory(&Dimension, sizeof(ENTDIMENSION));
			Dimension.DefPoint3 = ((PDXFENTDIMENSION)&DxfEntityData)->DefPoint3;
			Dimension.DefPoint4 = ((PDXFENTDIMENSION)&DxfEntityData)->DefPoint4;
			Dimension.DimRotationAngle = ((PDXFENTDIMENSION)&DxfEntityData)->DimRotationAngle;
			Dimension.DimLineDefPoint = ((PDXFENTDIMENSION)&DxfEntityData)->DimLineDefPoint;
			strcpy_s(Dimension.DimText, ((PDXFENTDIMENSION)&DxfEntityData)->DimText);

			strcpy_s(DimBlockHeader.Name, ((PDXFENTDIMENSION)&DxfEntityData)->BlockName);
			
			Dimension.BlockheaderObjhandle = m_pDxfView->m_pJetDxfDimView->drwFindBlock_Direct(FIND_BYNAME, &DimBlockHeader);
			strcpy_s(buf, &((PDXFENTDIMENSION)&DxfEntityData)->BlockName[2]);
			unsigned long n = atol(buf);
			if (n > m_pDxfView->m_pJetDxfDimView->m_pDrawing->LastDimBlkNum)
				m_pDxfView->m_pJetDxfDimView->m_pDrawing->LastDimBlkNum = n;

			// Looking for dim style name
			if (strlen(((PDXFENTDIMENSION)&DxfEntityData)->DimStyleName) > 0)
			{
				strcpy_s(DimStyle.Name, ((PDXFENTDIMENSION)&DxfEntityData)->DimStyleName);
				if (m_pDxfView->m_pJetDxfDimView->drwFindTableType_Direct(TAB_DIMSTYLE, FIND_BYNAME, &DimStyle) > 0)
					Dimension.DimStyleObjhandle = DimStyle.Objhandle;
				else
				{
					// DimStyle is unavailable
					// Set to default dim style
				}
			}
			else
			{
				// Set to default dim style
			}

			m_pDxfView->m_pJetDxfDimView->drwAddEntity_Direct(BlockObjHandle, &EntityHeader, &Dimension);
			//drwAddEntity_Direct(m_pDrawing, BlockObjhandle, &EntityHeader, &Dimension);
			break;
		}

	}
}