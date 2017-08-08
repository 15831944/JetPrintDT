#include "JetDxfBaseView.h"
#include <math.h>
CJetDxfBaseView::CJetDxfBaseView()
{
	m_pDrawing = NULL;
}


CJetDxfBaseView::~CJetDxfBaseView()
{
	if (NULL != m_pDrawing)
	{
		delete m_pDrawing;
		m_pDrawing = NULL;
	}
}

/*-------------------------------------------------------------------*
|  drwCreate                                                        |
|  Inputs:                                                          |
|      Nothing!                                                     |
|  Output: Handle to new created drawing ( NULL=Error )             |
*-------------------------------------------------------------------*/
BOOL CJetDxfBaseView::drwCreate()
{

	if (NULL != m_pDrawing){
		if (NULL != m_pDrawing->Entities.pEntityHeap){
			m_pDrawing->Tables.NumLayers = 0;
			m_pDrawing->Tables.NumLTypes = 0;
			m_pDrawing->Tables.NumStyles = 0;
			m_pDrawing->Tables.NumDimStyles = 0;

			m_pDrawing->Tables.CurrentLayer = 0;
			m_pDrawing->Tables.CurrentLType = 0;
			m_pDrawing->Tables.CurrentStyle = 0;
			m_pDrawing->Tables.CurrentDimStyle = 0;

			m_pDrawing->BlocksNumber = 0;
			m_pDrawing->CurrentBlock = 0;

			m_pDrawing->LastObjhandle = 0;
			m_pDrawing->LastDimBlkNum = 0;

			ZeroMemory(m_pDrawing->Entities.pEntityHeap, HeapSize(GetProcessHeap(), HEAP_NO_SERIALIZE, m_pDrawing->Entities.pEntityHeap));

			m_pDrawing->Entities.TotalSize = HeapSize(GetProcessHeap(), HEAP_NO_SERIALIZE, m_pDrawing->Entities.pEntityHeap);
			m_pDrawing->Entities.FreePos = 0;
			m_pDrawing->Entities.LastEntityPos = 0;
			m_pDrawing->Entities.CurrentEntityPos = 0;
			m_pDrawing->Entities.EntitiesNumber = 0;

			m_pDrawing->View.Viewable = TRUE;

			return TRUE;
		}
		else
		{
			delete m_pDrawing;
			m_pDrawing = NULL;
		}
	}


	m_pDrawing = new DRAWING;
	if (m_pDrawing == NULL)
	{
		return FALSE;
	}

	ZeroMemory(m_pDrawing, sizeof(DRAWING));

	//	m_pDrawing->Tables.CurrentLayerHandle = 0;	// Layer 0
	//	m_pDrawing->Tables.CurrentTextStyleHandle = 0;// Standard
	//	m_pDrawing->Tables.CurrentDimStyleHandle = 0;	// Default
	//	m_pDrawing->Tables.CurrentLineTypeHandle = 0;	// ByLayer
	//	m_pDrawing->Tables.CurrentColor = 256;		// ByLayer
	//	m_pDrawing->Tables.CurrentLineWeight = 0;		// ByLayer

	m_pDrawing->Tables.NumLayers = 0;
	m_pDrawing->Tables.NumLTypes = 0;
	m_pDrawing->Tables.NumStyles = 0;
	m_pDrawing->Tables.NumDimStyles = 0;

	m_pDrawing->Tables.CurrentLayer = 0;
	m_pDrawing->Tables.CurrentLType = 0;
	m_pDrawing->Tables.CurrentStyle = 0;
	m_pDrawing->Tables.CurrentDimStyle = 0;

	m_pDrawing->BlocksNumber = 0;
	m_pDrawing->CurrentBlock = 0;

	m_pDrawing->LastObjhandle = 0;
	m_pDrawing->LastDimBlkNum = 0;


	// Initilize Entities Section
	m_pDrawing->Entities.pEntityHeap = (PBYTE)HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, HEAP_ENTITY_SIZE);
	if (m_pDrawing->Entities.pEntityHeap == NULL)
	{
		HeapFree(GetProcessHeap(), 0, m_pDrawing->Entities.pEntityHeap);
		return FALSE;
	}

	m_pDrawing->Entities.TotalSize = HeapSize(GetProcessHeap(), HEAP_NO_SERIALIZE, m_pDrawing->Entities.pEntityHeap);
	m_pDrawing->Entities.FreePos = 0;
	m_pDrawing->Entities.LastEntityPos = 0;
	m_pDrawing->Entities.CurrentEntityPos = 0;
	m_pDrawing->Entities.EntitiesNumber = 0;

	m_pDrawing->View.Viewable = TRUE;

	return TRUE;
}

/*-------------------------------------------------------------------*
|  drwDestroy                                                       |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the drawing to be destroyed    |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfBaseView::drwDestroy()
{
	ENTITYHEADER	EntityHeader;
	char			EntityData[4096];
	BLOCKHEADER		BlockHeader;
	PENTPOLYLINE	pPLine;

	if (NULL == m_pDrawing)
	{
		return TRUE;
	}

	m_pDrawing->View.Viewable = FALSE;

	// Free all allocated memory by polylines before destroy entire drawing
	// Entities Section
	if (drwFindEntity_Direct(NULL, &EntityHeader, &EntityData, FIND_FIRST) > 0)
	{
		do{
			if (EntityHeader.EntityType == ENT_POLYLINE)
			{
				pPLine = ((PENTPOLYLINE)&EntityData);
				delete ((PENTPOLYLINE)&EntityData)->pVertex;
			}
		} while (drwFindEntity_Direct(NULL, &EntityHeader, &EntityData, FIND_NEXT) > 0);
	}
	// Blocks Section
	if (drwFindBlock_Direct(FIND_FIRST, &BlockHeader) > 0)
	{
		do{
			if (drwFindEntity_Direct(CA2W(BlockHeader.Name), &EntityHeader, &EntityData, FIND_FIRST) > 0)
			{
				do{
					if (EntityHeader.EntityType == ENT_POLYLINE)
						delete ((PENTPOLYLINE)&EntityData)->pVertex;
				} while (drwFindEntity_Direct(CA2W(BlockHeader.Name), &EntityHeader, &EntityData, FIND_NEXT) > 0);
			}
		} while (drwFindBlock_Direct(FIND_NEXT, &BlockHeader) > 0);
	}

	if (m_pDrawing->Entities.pEntityHeap){
		// Free Entities Memory
		HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, m_pDrawing->Entities.pEntityHeap);
		m_pDrawing->Entities.pEntityHeap = NULL;
	}
	

	// Free Blocks Memory
	for (unsigned int i = 0; i < m_pDrawing->BlocksNumber; i++){
		HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, m_pDrawing->Blocks[i].Entities.pEntityHeap);
		m_pDrawing->Blocks[i].Entities.pEntityHeap = NULL;
	}
		


	return TRUE;
}



/*-------------------------------------------------------------------*
|  drwFindTableType_Direct                                          |
|  Inputs:                                                          |
|      m_pDrawing m_pDrawing = pointer to the current DRAWING structure |
|      DWORD dwTableType = table type to get data                   |
|                        ( TAB_NOTSET = search in all table types ) |
|      DWORD dwFindType = find type                                 |
|      LPVOID pTableType = pointer to table type structure          |
|  Output: Handle of found table type (0=Error or Not found)        |
*-------------------------------------------------------------------*/
OBJHANDLE CJetDxfBaseView::drwFindTableType_Direct(DWORD dwTableType, DWORD dwFindType, LPVOID pTableType)
{
	OBJHANDLE	result = 0;

	if (dwFindType & FIND_FIRST)
	{
		switch (dwTableType)
		{
		case TAB_DIMSTYLE:
			if (m_pDrawing->Tables.NumDimStyles <= 0)
				break;
			m_pDrawing->Tables.CurrentDimStyle = 0;
			CopyMemory(pTableType, &m_pDrawing->Tables.DimStyle[m_pDrawing->Tables.CurrentDimStyle], sizeof(DIMSTYLE));
			result = m_pDrawing->Tables.DimStyle[m_pDrawing->Tables.CurrentDimStyle].Objhandle;
			break;
		case TAB_LAYER:
			if (m_pDrawing->Tables.NumLayers <= 0)
				break;
			m_pDrawing->Tables.CurrentLayer = 0;
			CopyMemory(pTableType, &m_pDrawing->Tables.Layer[m_pDrawing->Tables.CurrentLayer], sizeof(LAYER));
			result = m_pDrawing->Tables.Layer[m_pDrawing->Tables.CurrentLayer].Objhandle;
			break;
		case TAB_LTYPE:
			if (m_pDrawing->Tables.NumLTypes <= 0)
				break;
			m_pDrawing->Tables.CurrentLType = 0;
			CopyMemory(pTableType, &m_pDrawing->Tables.LType[m_pDrawing->Tables.CurrentLType], sizeof(LTYPE));
			result = m_pDrawing->Tables.LType[m_pDrawing->Tables.CurrentLType].Objhandle;
			break;
		case TAB_STYLE:
			if (m_pDrawing->Tables.NumStyles <= 0)
				break;
			m_pDrawing->Tables.CurrentStyle = 0;
			CopyMemory(pTableType, &m_pDrawing->Tables.Style[m_pDrawing->Tables.CurrentStyle], sizeof(STYLE));
			result = m_pDrawing->Tables.Style[m_pDrawing->Tables.CurrentStyle].Objhandle;
			break;
		}
	}
	else if (dwFindType & FIND_LAST)
	{
		switch (dwTableType)
		{
		case TAB_DIMSTYLE:
			if (m_pDrawing->Tables.NumDimStyles <= 0)
				break;
			m_pDrawing->Tables.CurrentDimStyle = m_pDrawing->Tables.NumDimStyles - 1;
			CopyMemory(pTableType, &m_pDrawing->Tables.DimStyle[m_pDrawing->Tables.CurrentDimStyle], sizeof(DIMSTYLE));
			result = m_pDrawing->Tables.DimStyle[m_pDrawing->Tables.CurrentDimStyle].Objhandle;
			break;
		case TAB_LAYER:
			if (m_pDrawing->Tables.NumLayers <= 0)
				break;
			m_pDrawing->Tables.CurrentLayer = m_pDrawing->Tables.NumLayers - 1;
			CopyMemory(pTableType, &m_pDrawing->Tables.Layer[m_pDrawing->Tables.CurrentLayer], sizeof(LAYER));
			result = m_pDrawing->Tables.Layer[m_pDrawing->Tables.CurrentLayer].Objhandle;
			break;
		case TAB_LTYPE:
			if (m_pDrawing->Tables.NumLTypes <= 0)
				break;
			m_pDrawing->Tables.CurrentLType = m_pDrawing->Tables.NumLTypes - 1;
			CopyMemory(pTableType, &m_pDrawing->Tables.LType[m_pDrawing->Tables.CurrentLType], sizeof(LTYPE));
			result = m_pDrawing->Tables.LType[m_pDrawing->Tables.CurrentLType].Objhandle;
			break;
		case TAB_STYLE:
			if (m_pDrawing->Tables.NumStyles <= 0)
				break;
			m_pDrawing->Tables.CurrentStyle = m_pDrawing->Tables.NumStyles - 1;
			CopyMemory(pTableType, &m_pDrawing->Tables.Style[m_pDrawing->Tables.CurrentStyle], sizeof(STYLE));
			result = m_pDrawing->Tables.Style[m_pDrawing->Tables.CurrentStyle].Objhandle;
			break;
		}
	}
	else if (dwFindType & FIND_PREVIOUS)
	{
		switch (dwTableType)
		{
		case TAB_DIMSTYLE:
			if (m_pDrawing->Tables.CurrentDimStyle <= 0)
				break;
			m_pDrawing->Tables.CurrentDimStyle--;
			CopyMemory(pTableType, &m_pDrawing->Tables.DimStyle[m_pDrawing->Tables.CurrentDimStyle], sizeof(DIMSTYLE));
			result = m_pDrawing->Tables.DimStyle[m_pDrawing->Tables.CurrentDimStyle].Objhandle;
			break;
		case TAB_LAYER:
			if (m_pDrawing->Tables.CurrentLayer <= 0)
				break;
			m_pDrawing->Tables.CurrentLayer--;
			CopyMemory(pTableType, &m_pDrawing->Tables.Layer[m_pDrawing->Tables.CurrentLayer], sizeof(LAYER));
			result = m_pDrawing->Tables.Layer[m_pDrawing->Tables.CurrentLayer].Objhandle;
			break;
		case TAB_LTYPE:
			if (m_pDrawing->Tables.CurrentLType <= 0)
				break;
			m_pDrawing->Tables.CurrentLType--;
			CopyMemory(pTableType, &m_pDrawing->Tables.LType[m_pDrawing->Tables.CurrentLType], sizeof(LTYPE));
			result = m_pDrawing->Tables.LType[m_pDrawing->Tables.CurrentLType].Objhandle;
			break;
		case TAB_STYLE:
			if (m_pDrawing->Tables.CurrentStyle <= 0)
				break;
			m_pDrawing->Tables.CurrentStyle--;
			CopyMemory(pTableType, &m_pDrawing->Tables.Style[m_pDrawing->Tables.CurrentStyle], sizeof(STYLE));
			result = m_pDrawing->Tables.Style[m_pDrawing->Tables.CurrentStyle].Objhandle;
			break;
		}
	}
	else if (dwFindType & FIND_NEXT)
	{
		switch (dwTableType)
		{
		case TAB_DIMSTYLE:
			if (m_pDrawing->Tables.CurrentDimStyle >= m_pDrawing->Tables.NumDimStyles - 1)
				break;
			m_pDrawing->Tables.CurrentDimStyle++;
			CopyMemory(pTableType, &m_pDrawing->Tables.DimStyle[m_pDrawing->Tables.CurrentDimStyle], sizeof(DIMSTYLE));
			result = m_pDrawing->Tables.DimStyle[m_pDrawing->Tables.CurrentDimStyle].Objhandle;
			break;
		case TAB_LAYER:
			if (m_pDrawing->Tables.CurrentLayer >= m_pDrawing->Tables.NumLayers - 1)
				break;
			m_pDrawing->Tables.CurrentLayer++;
			CopyMemory(pTableType, &m_pDrawing->Tables.Layer[m_pDrawing->Tables.CurrentLayer], sizeof(LAYER));
			result = m_pDrawing->Tables.Layer[m_pDrawing->Tables.CurrentLayer].Objhandle;
			break;
		case TAB_LTYPE:
			if (m_pDrawing->Tables.CurrentLType >= m_pDrawing->Tables.NumLTypes - 1)
				break;
			m_pDrawing->Tables.CurrentLType++;
			CopyMemory(pTableType, &m_pDrawing->Tables.LType[m_pDrawing->Tables.CurrentLType], sizeof(LTYPE));
			result = m_pDrawing->Tables.LType[m_pDrawing->Tables.CurrentLType].Objhandle;
			break;
		case TAB_STYLE:
			if (m_pDrawing->Tables.CurrentStyle >= m_pDrawing->Tables.NumStyles - 1)
				break;
			m_pDrawing->Tables.CurrentStyle++;
			CopyMemory(pTableType, &m_pDrawing->Tables.Style[m_pDrawing->Tables.CurrentStyle], sizeof(STYLE));
			result = m_pDrawing->Tables.Style[m_pDrawing->Tables.CurrentStyle].Objhandle;
			break;
		}
	}
	else if (dwFindType & (FIND_BYHANDLE | FIND_BYNAME))
	{
		if ((dwTableType == TAB_NOTSET) || (dwTableType == TAB_DIMSTYLE))
			for (UINT i = 0; i<m_pDrawing->Tables.NumDimStyles; i++)
				if (((m_pDrawing->Tables.DimStyle[i].Objhandle == ((PDIMSTYLE)pTableType)->Objhandle) && (dwFindType & FIND_BYHANDLE))
					|| ((strcmp(m_pDrawing->Tables.DimStyle[i].Name, ((PDIMSTYLE)pTableType)->Name) == 0) && (dwFindType & FIND_BYNAME)))
				{
					dwTableType = TAB_DIMSTYLE;
					CopyMemory(pTableType, &m_pDrawing->Tables.DimStyle[i], sizeof(DIMSTYLE));
					result = m_pDrawing->Tables.DimStyle[i].Objhandle;
				}

		if ((dwTableType == TAB_NOTSET) || (dwTableType == TAB_LAYER))
			for (UINT i = 0; i<m_pDrawing->Tables.NumLayers; i++)
				if (((m_pDrawing->Tables.Layer[i].Objhandle == ((PLAYER)pTableType)->Objhandle) && (dwFindType & FIND_BYHANDLE))
					|| ((strcmp(m_pDrawing->Tables.Layer[i].Name, ((PLAYER)pTableType)->Name) == 0) && (dwFindType & FIND_BYNAME)))
				{
					dwTableType = TAB_LAYER;
					CopyMemory(pTableType, &m_pDrawing->Tables.Layer[i], sizeof(LAYER));
					result = m_pDrawing->Tables.Layer[i].Objhandle;
				}

		if ((dwTableType == TAB_NOTSET) || (dwTableType == TAB_LTYPE))
			for (UINT i = 0; i<m_pDrawing->Tables.NumLTypes; i++)
				if (((m_pDrawing->Tables.LType[i].Objhandle == ((PLTYPE)pTableType)->Objhandle) && (dwFindType & FIND_BYHANDLE))
					|| ((strcmp(m_pDrawing->Tables.LType[i].Name, ((PLTYPE)pTableType)->Name) == 0) && (dwFindType & FIND_BYNAME)))
				{
					dwTableType = TAB_LTYPE;
					CopyMemory(pTableType, &m_pDrawing->Tables.LType[i], sizeof(LTYPE));
					result = m_pDrawing->Tables.LType[i].Objhandle;
				}

		if ((dwTableType == TAB_NOTSET) || (dwTableType == TAB_STYLE))
			for (UINT i = 0; i<m_pDrawing->Tables.NumStyles; i++)
				if (((m_pDrawing->Tables.Style[i].Objhandle == ((PSTYLE)pTableType)->Objhandle) && (dwFindType & FIND_BYHANDLE))
					|| ((strcmp(m_pDrawing->Tables.Style[i].Name, ((PSTYLE)pTableType)->Name) == 0) && (dwFindType & FIND_BYNAME)))
				{
					dwTableType = TAB_STYLE;
					CopyMemory(pTableType, &m_pDrawing->Tables.Style[i], sizeof(STYLE));
					result = m_pDrawing->Tables.Style[i].Objhandle;
				}
	}

	return result;
}

/*-------------------------------------------------------------------*
|  drwFindTableType                                                 |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      DWORD dwTableType = table type to get data                   |
|                        ( TAB_NOTSET = search in all table types ) |
|      DWORD dwFindType = find type                                 |
|      LPVOID pTableType = pointer to table type structure          |
|  Output: Handle of found table type (0=Error or Not found)        |
*-------------------------------------------------------------------*/
OBJHANDLE CJetDxfBaseView::drwFindTableType(DWORD dwTableType, DWORD dwFindType, LPVOID pTableType)
{
	OBJHANDLE	result;

	result = drwFindTableType_Direct(dwTableType, dwFindType, pTableType);

	return result;
}

/*-------------------------------------------------------------------*
|  drwAddTableType                                                  |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      DWORD dwTableType = table type to be added                   |
|      LPVOID pTableType = pointer to table type structure to add   |
|  Output: Handle of new table type (0=Error)                       |
*-------------------------------------------------------------------*/
OBJHANDLE CJetDxfBaseView::drwAddTableType(DWORD dwTableType, LPVOID pTableType)
{
	int			CurrentTType = 0;
	OBJHANDLE	NewTableTypeObjhandle;

	
	// Check for duplication
	if (drwFindTableType_Direct(dwTableType, FIND_BYNAME, pTableType) != 0)
	{
		return 0;
	}

	NewTableTypeObjhandle = m_pDrawing->LastObjhandle + 1;

	switch (dwTableType)
	{
	case TAB_DIMSTYLE:
		if (m_pDrawing->Tables.NumDimStyles >= MAX_DIMSTYLES)
		{
			return 0;
		}

		STYLE		Style;
		Style.Objhandle = ((PDIMSTYLE)pTableType)->dimtxstyObjhandle;
		if (drwFindTableType(TAB_STYLE, FIND_BYHANDLE, &Style) <= 0)
		{
			return 0;
		}

		CurrentTType = m_pDrawing->Tables.NumDimStyles;
		CopyMemory(&m_pDrawing->Tables.DimStyle[CurrentTType], pTableType, sizeof(DIMSTYLE));
		m_pDrawing->Tables.DimStyle[CurrentTType].Objhandle = NewTableTypeObjhandle;
		m_pDrawing->Tables.NumDimStyles++;
		break;

	case TAB_LAYER:
		if (m_pDrawing->Tables.NumLayers >= MAX_LAYERS)
		{
			// Cannot add more Layers
			return 0;
		}

		LTYPE	LType;
		LType.Objhandle = ((PLAYER)pTableType)->LineTypeObjhandle;
		if (drwFindTableType(TAB_LTYPE, FIND_BYHANDLE, &LType) <= 0)
		{
			// Cannot find linetype used for this layer
			return 0;
		}

		CurrentTType = m_pDrawing->Tables.NumLayers;
		CopyMemory(&m_pDrawing->Tables.Layer[CurrentTType], pTableType, sizeof(LAYER));
		m_pDrawing->Tables.Layer[CurrentTType].Objhandle = NewTableTypeObjhandle;
		m_pDrawing->Tables.NumLayers++;
		break;

	case TAB_LTYPE:
		if (m_pDrawing->Tables.NumLTypes >= MAX_LTYPES)
		{
			// Cannot add more LineTypes
			return 0;
		}

		CurrentTType = m_pDrawing->Tables.NumLTypes;
		CopyMemory(&m_pDrawing->Tables.LType[CurrentTType], pTableType, sizeof(LTYPE));
		m_pDrawing->Tables.LType[CurrentTType].Objhandle = NewTableTypeObjhandle;
		m_pDrawing->Tables.NumLTypes++;
		break;

	case TAB_STYLE:
		if (m_pDrawing->Tables.NumStyles >= MAX_STYLES)
		{
			// Cannot add more TextStyles
			return 0;
		}

		CurrentTType = m_pDrawing->Tables.NumStyles;
		CopyMemory(&m_pDrawing->Tables.Style[CurrentTType], pTableType, sizeof(STYLE));
		m_pDrawing->Tables.Style[CurrentTType].Objhandle = NewTableTypeObjhandle;
		m_pDrawing->Tables.NumStyles++;
		break;

	default:
		// Unknown Table Type
		return 0;
	}

	m_pDrawing->LastObjhandle++;

	return NewTableTypeObjhandle;
}

/*-------------------------------------------------------------------*
|  drwDeleteTableType                                               |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      DWORD dwTableType = table type to be removed                 |
|      OBJHANDLE TableTypeObjhandle = handle of table type to remove|
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfBaseView::drwDeleteTableType(DWORD dwTableType, OBJHANDLE TableTypeObjhandle)
{
	BOOL		result = FALSE;
	UINT		i = 0;
	UINT        j = 0;

	// Initialize m_pDrawing ------------------
	if (m_pDrawing  == NULL)
		return FALSE;

	switch (dwTableType)
	{
	case TAB_DIMSTYLE:
		for (i = 0; i<m_pDrawing->Tables.NumDimStyles; i++)
			if (m_pDrawing->Tables.DimStyle[i].Objhandle == TableTypeObjhandle)
			{
				for (j = m_pDrawing->Tables.NumDimStyles - 1; j>i; j--)
					m_pDrawing->Tables.DimStyle[j] = m_pDrawing->Tables.DimStyle[j - 1];
				result = TRUE;
			}
		m_pDrawing->Tables.NumDimStyles--;
		break;

	case TAB_LAYER:
		for (i = 0; i<m_pDrawing->Tables.NumLayers; i++)
			if (m_pDrawing->Tables.Layer[i].Objhandle == TableTypeObjhandle)
			{
				for (j = m_pDrawing->Tables.NumLayers - 1; j>i; j--)
					m_pDrawing->Tables.Layer[j] = m_pDrawing->Tables.Layer[j - 1];
				result = TRUE;
			}
		m_pDrawing->Tables.NumLayers--;
		break;

	case TAB_LTYPE:
		for (i = 0; i<m_pDrawing->Tables.NumLTypes; i++)
			if (m_pDrawing->Tables.LType[i].Objhandle == TableTypeObjhandle)
			{
				for (j = m_pDrawing->Tables.NumLTypes - 1; j>i; j--)
					m_pDrawing->Tables.LType[j] = m_pDrawing->Tables.LType[j - 1];
				result = TRUE;
			}
		m_pDrawing->Tables.NumLTypes--;
		break;

	case TAB_STYLE:
		for (i = 0; i<m_pDrawing->Tables.NumStyles; i++)
			if (m_pDrawing->Tables.Style[i].Objhandle == TableTypeObjhandle)
			{
				for (j = m_pDrawing->Tables.NumStyles - 1; j>i; j--)
					m_pDrawing->Tables.Style[j] = m_pDrawing->Tables.Style[j - 1];
				result = TRUE;
			}
		m_pDrawing->Tables.NumStyles--;
		break;

	default: // Table Type is not accepted
		return FALSE;
	}

	if (!result)
	{
		// Table type is not found
		return FALSE;
	}

	// UnInitilize m_pDrawing -----------------
	return true;
}


/********************************************************************
* BLOCKS Section                                                   *
********************************************************************/

/*-------------------------------------------------------------------*
|  drwFindBlock_Direct                                              |
|  Inputs:                                                          |
|      m_pDrawing m_pDrawing = pointer to the current DRAWING structure |
|      DWORD dwFindType = find type                                 |
|      PBLOCKHEADER pBlockHeader = pointer to block header data     |
|  Output: Handle of found block (0=Error or Not found)             |
*-------------------------------------------------------------------*/
OBJHANDLE CJetDxfBaseView::drwFindBlock_Direct(DWORD dwFindType, PBLOCKHEADER pBlockHeader)
{
	OBJHANDLE		result;
	bool			isBlockFound = false;
	unsigned int	i = 0;

	if (m_pDrawing->BlocksNumber <= 0)
	{
		// There is no block to find
		return 0;
	}

	switch (dwFindType)
	{
	case FIND_FIRST:
		m_pDrawing->CurrentBlock = 0;
		isBlockFound = true;
		break;
	case FIND_LAST:
		m_pDrawing->CurrentBlock = m_pDrawing->BlocksNumber - 1;
		isBlockFound = true;
		break;
	case FIND_PREVIOUS:
		if (m_pDrawing->CurrentBlock <= 0)
			break;
		m_pDrawing->CurrentBlock--;
		isBlockFound = true;
		break;
	case FIND_NEXT:
		if (m_pDrawing->CurrentBlock >= m_pDrawing->BlocksNumber - 1)
			break;
		m_pDrawing->CurrentBlock++;
		isBlockFound = true;
		break;
	case FIND_BYHANDLE:
		for (i = 0; i<m_pDrawing->BlocksNumber; i++)
			if (m_pDrawing->Blocks[i].Objhandle == pBlockHeader->Objhandle)
			{
				m_pDrawing->CurrentBlock = i;
				isBlockFound = true;
				break;
			}
		break;
	case FIND_BYNAME:
		for (i = 0; i<m_pDrawing->BlocksNumber; i++)
			if (strcmp(m_pDrawing->Blocks[i].Name, pBlockHeader->Name) == 0)
			{
				m_pDrawing->CurrentBlock = i;
				isBlockFound = true;
				break;
			}
		break;
	}

	if (isBlockFound)
	{
		CopyMemory(pBlockHeader, &m_pDrawing->Blocks[m_pDrawing->CurrentBlock], sizeof(BLOCKHEADER));
		result = m_pDrawing->Blocks[m_pDrawing->CurrentBlock].Objhandle;
	}
	else
		result = 0;

	return result;
}

/*-------------------------------------------------------------------*
|  drwFindBlock                                                     |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      DWORD dwFindType = find type                                 |
|      PBLOCKHEADER pBlockHeader = pointer to block header data     |
|  Output: Handle of found block (0=Error or Not found)             |
*-------------------------------------------------------------------*/
OBJHANDLE CJetDxfBaseView::drwFindBlock(DWORD dwFindType, PBLOCKHEADER pBlockHeader)
{
	OBJHANDLE		result;

	// Initialize m_pDrawing ------------------
	if (m_pDrawing  == NULL)
		return FALSE;

	result = drwFindBlock_Direct(dwFindType, pBlockHeader);


	return result;
}

/*-------------------------------------------------------------------*
|  drwGetBlocksNum                                                  |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|  Output: number of blocks in current drawing (lower than 0=Error) |
*-------------------------------------------------------------------*/
int CJetDxfBaseView::drwGetBlocksNum()
{
	int			result;

	// Initialize m_pDrawing ------------------
	if (m_pDrawing == NULL)
		return -1;

	result = m_pDrawing->BlocksNumber;

	return result;
}

/*-------------------------------------------------------------------*
|  FindBlockPosByHandle                                             |
|  Inputs:                                                          |
|      m_pDrawing m_pDrawing = pointer to the current drawing structure |
|      OBJHANDLE BlockObjhandle = handle of block to find           |
|  Output: Position of block in Blocks list. (-1=block not found)   |
*-------------------------------------------------------------------*/
int CJetDxfBaseView::FindBlockPosByHandle(OBJHANDLE BlockObjhandle)
{
	for (unsigned int i = 0; i<m_pDrawing->BlocksNumber; i++)
		if (m_pDrawing->Blocks[i].Objhandle == BlockObjhandle)
			return i;

	return -1;
}

/*-------------------------------------------------------------------*
|  FindBlockPosByName                                               |
|  Inputs:                                                          |
|      m_pDrawing m_pDrawing = pointer to the current drawing structure |
|      LPCTSTR strBlockName = block name to be found                |
|  Output: Position of block in Blocks list. (-1=block not found)   |
*-------------------------------------------------------------------*/
int CJetDxfBaseView::FindBlockPosByName(LPCTSTR strBlockName)
{
	for (unsigned int i = 0; i<m_pDrawing->BlocksNumber; i++)
		if (strcmp(m_pDrawing->Blocks[i].Name, CW2A(strBlockName)) == 0)
			return i;

	return -1;
}

/*-------------------------------------------------------------------*
|  drwAddBlock_Direct                                               |
|  Inputs:                                                          |
|      m_pDrawing m_pDrawing = pointer to the current DRAWING structure |
|      PBLOCKHEADER pBlockHeader = pointer to blockheader structure |
|  Output: Handle of the new block (0=Error)                        |
*-------------------------------------------------------------------*/
OBJHANDLE CJetDxfBaseView::drwAddBlock_Direct(PBLOCKHEADER pBlockHeader)
{
	DWORD		BlockNumber = 0;
	OBJHANDLE	NewBlockObjhandle;

	// Check for current blocks number
	if (m_pDrawing->BlocksNumber >= 256)
	{
		// No more blocks can be added
		return 0;
	}

	if (FindBlockPosByName(CA2W(pBlockHeader->Name)) >= 0)
	{
		// The block already exists
		return 0;
	}

	// Initilize new block
	NewBlockObjhandle = m_pDrawing->LastObjhandle + 1;
	BlockNumber = m_pDrawing->BlocksNumber;
	CopyMemory(&m_pDrawing->Blocks[BlockNumber], pBlockHeader, sizeof(BLOCKHEADER));
	m_pDrawing->Blocks[BlockNumber].Objhandle = NewBlockObjhandle;

	// Initialize Entities Section of new block
	m_pDrawing->Blocks[BlockNumber].Entities.pEntityHeap = (PBYTE)HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, HEAP_BLOCK_SIZE);
	if (m_pDrawing->Blocks[BlockNumber].Entities.pEntityHeap == NULL)
	{
		// GlobalAlloc error
		return 0;
	}

	m_pDrawing->Blocks[BlockNumber].Entities.TotalSize = HeapSize(GetProcessHeap(), HEAP_NO_SERIALIZE, m_pDrawing->Blocks[BlockNumber].Entities.pEntityHeap);
	m_pDrawing->Blocks[BlockNumber].Entities.FreePos = 0;
	m_pDrawing->Blocks[BlockNumber].Entities.LastEntityPos = 0;
	m_pDrawing->Blocks[BlockNumber].Entities.CurrentEntityPos = 0;
	m_pDrawing->Blocks[BlockNumber].Entities.EntitiesNumber = 0;

	m_pDrawing->LastObjhandle++;
	m_pDrawing->BlocksNumber++;

	return NewBlockObjhandle;
}

/*-------------------------------------------------------------------*
|  drwAddBlock                                                      |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      PBLOCKHEADER pBlockHeader = pointer to blockheader structure |
|  Output: Handle of the new block (0=Error)                        |
*-------------------------------------------------------------------*/
OBJHANDLE CJetDxfBaseView::drwAddBlock(PBLOCKHEADER pBlockHeader)
{
	OBJHANDLE	result;

	// Initialize m_pDrawing ------------------
	if (m_pDrawing== NULL)
		return 0;

	result = drwAddBlock_Direct(pBlockHeader);
	if (result == 0)
	{
		return 0;
	}

	return result;
}

/*-------------------------------------------------------------------*
|  drwDeleteBlock                                                   |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      OBJHANDLE BlockObjhandle = handle of block to Delete         |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfBaseView::drwDeleteBlock(OBJHANDLE BlockObjhandle)
{
	DWORD		BlockNumber = 0;

	// Initialize m_pDrawing ------------------
	if (m_pDrawing == NULL)
		return FALSE;

	BlockNumber = FindBlockPosByHandle(BlockObjhandle);
	if (BlockNumber < 0)
	{
		// The block cannot be found
		return FALSE;
	}

	// Free entites memory related to this block
	if (FALSE == HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, m_pDrawing->Blocks[BlockNumber].Entities.pEntityHeap))
	{
		// error in GlobalFree function
		return FALSE;
	}

	m_pDrawing->BlocksNumber--;

	// UnInitilize m_pDrawing -----------------
	return true;
}


/********************************************************************
* ENTITIES Section                                                 *
********************************************************************/

/*-------------------------------------------------------------------*
|  AddToEntitiesList                                                |
|  Inputs:                                                          |
|      PENTITIES pEntities = pointer of ENTITIES structure          |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure to add |
|      WORD EntitySize = size of entity data to be added            |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfBaseView::AddToEntitiesList(PENTITIES pEntities, PENTITYHEADER pEntityHeader, LPVOID pEntityData, WORD EntitySize)
{
	if (pEntities->pEntityHeap == NULL)
	{
		return FALSE;
	}

	// Check if we need more memory space for entities data
	if ((pEntities->FreePos + sizeof(ENTITYHEADER) + EntitySize) >= pEntities->TotalSize)
	{
		pEntities->pEntityHeap = (PBYTE)HeapReAlloc(
			GetProcessHeap(),
			HEAP_NO_SERIALIZE,
			pEntities->pEntityHeap,
			pEntities->TotalSize + HEAP_ENTITY_SIZE);

		pEntities->TotalSize = HeapSize(GetProcessHeap(), HEAP_NO_SERIALIZE, pEntities->pEntityHeap);
		if ((pEntities->FreePos + sizeof(ENTITYHEADER) + EntitySize) >= pEntities->TotalSize)
		{
			// Memory limitation error
			return FALSE;
		}
	}

	// Init EntityHeader for previous entity position
	pEntityHeader->PreviousEntityPos = pEntities->LastEntityPos;
	pEntityHeader->NextEntityPos = pEntities->FreePos + sizeof(ENTITYHEADER) + EntitySize;

	// Copy entity data to memory
	CopyMemory((LPVOID)((DWORD)pEntities->pEntityHeap + pEntities->FreePos), pEntityHeader, sizeof(ENTITYHEADER));
	CopyMemory((LPVOID)((DWORD)pEntities->pEntityHeap + pEntities->FreePos + sizeof(ENTITYHEADER)), (LPVOID)pEntityData, EntitySize);

	// Update pointers
	pEntities->LastEntityPos = pEntities->FreePos;
	pEntities->FreePos += sizeof(ENTITYHEADER) + EntitySize;


	return TRUE;
}

/*-------------------------------------------------------------------*
|  AddEntity                                                        |
|  Inputs:                                                          |
|      m_pDrawing m_pDrawing = pointer to the current DRAWING structure |
|      PENTITIES pEntities = pointer of ENTITIES structure          |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure to add |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfBaseView::AddEntity(PENTITIES pEntities, PENTITYHEADER pEntityHeader, LPVOID pEntityData)
{
	BOOL result = false;
	OBJHANDLE		BlockObjhandle1;

	switch (pEntityHeader->EntityType)
	{
	case ENT_LINE:
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTLINE));
		break;

	case ENT_POINT:
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTPOINT));
		break;

	case ENT_CIRCLE:
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTCIRCLE));
		break;

	case ENT_ELLIPSE:
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTELLIPSE));
		break;

	case ENT_TEXT:
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTTEXT));
		break;

	case ENT_ARC:
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTARC));
		break;

	case ENT_SOLID:
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTSOLID));
		break;

	case ENT_INSERT:
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTINSERT));
		break;

	case ENT_LWPOLYLINE:
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTLWPOLYLINE));
		break;

	case ENT_SPLINE:
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTSPLINE));
		break;

	case ENT_POLYLINE:
		PENTVERTEX pVertex;
		pVertex = new ENTVERTEX[((PENTPOLYLINE)pEntityData)->nVertex];
		memcpy(pVertex, ((PENTPOLYLINE)pEntityData)->pVertex, sizeof(ENTVERTEX)*((PENTPOLYLINE)pEntityData)->nVertex);
		((PENTPOLYLINE)pEntityData)->pVertex = pVertex;
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTPOLYLINE));
		break;

	case ENT_DIMENSION:
		BlockObjhandle1 = ((PENTDIMENSION)pEntityData)->BlockheaderObjhandle;

		if (BlockObjhandle1 == NULL)
		{
			DIMSTYLE		DimStyle;
			BLOCKHEADER		BlockHeader;
			OBJHANDLE		BlockObjhandle;
			char			buf[32];
			// Looking for dim style
			DimStyle.Objhandle = PENTDIMENSION(pEntityData)->DimStyleObjhandle;
			if (drwFindTableType_Direct(TAB_DIMSTYLE, FIND_BYHANDLE, &DimStyle)>0)
			{
				// Create a new Dimension Block
				ZeroMemory(&BlockHeader, sizeof(BLOCKHEADER));
				strcpy_s(BlockHeader.Name, "*D");
				_ltoa_s(++m_pDrawing->LastDimBlkNum, buf, 10);
				strcat_s(BlockHeader.Name, buf);
				BlockHeader.Flags = 1;
				BlockHeader.BasePoint.x = 0;
				BlockHeader.BasePoint.y = 0;
				BlockHeader.BasePoint.z = 0;
				BlockHeader.LayerObjhandle = pEntityHeader->LayerObjhandle;
				BlockObjhandle = drwAddBlock_Direct(&BlockHeader);

				if (BlockObjhandle != NULL)
				{
					//DimLinear(m_pDrawing, BlockObjhandle, pEntityHeader, PENTDIMENSION(pEntityData), &DimStyle);
					PENTDIMENSION(pEntityData)->BlockheaderObjhandle = BlockObjhandle;
				}
			}

		}
		result = AddToEntitiesList(pEntities, pEntityHeader, pEntityData, sizeof(ENTDIMENSION));
		break;

	default: // Entity is not accepted
		result = FALSE;
	}

	if (!result)
		return FALSE;
	else
		pEntities->EntitiesNumber++;

	return TRUE;
}

/*-------------------------------------------------------------------*
|  drwAddEntity_Direct                                              |
|  Inputs:                                                          |
|      m_pDrawing m_pDrawing = pointer to the current DRAWING structure |
|      OBJHANDLE BlockObjhandle = handle of block that entity must  |
|                        be added in it. (NULL = Entity Section)    |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure to add |
|  Output: Handle of the new entity (0=Error)                       |
*-------------------------------------------------------------------*/
OBJHANDLE CJetDxfBaseView::drwAddEntity_Direct(OBJHANDLE BlockObjhandle, PENTITYHEADER pEntityHeader, LPVOID pEntityData)
{
	PENTITIES		pEntities;
	//	ENTITYHEADER	EntityHeader;
	OBJHANDLE		NewEntityObjhandle;

	if (BlockObjhandle != NULL)
	{ // Search for block name
		DWORD i = FindBlockPosByHandle(BlockObjhandle);
		if (i<0)
		{
			// The block cannot be found
			return 0;
		}
		else
			pEntities = &m_pDrawing->Blocks[i].Entities;
	}
	else
		pEntities = &m_pDrawing->Entities;

	// Initialize EntityHeader
	NewEntityObjhandle = m_pDrawing->LastObjhandle + 1;
	pEntityHeader->Objhandle = NewEntityObjhandle;
	pEntityHeader->Deleted = FALSE;
	//	EntityHeader.EntityType = usEntityType;
	//	EntityHeader.LayerObjhandle = m_pDrawing->Tables.CurrentLayerHandle;
	//	EntityHeader.LTypeObjhandle = m_pDrawing->Tables.CurrentLineTypeHandle;
	//	EntityHeader.Color = m_pDrawing->Tables.CurrentColor;

	// Add Entity to list
	if (!AddEntity(pEntities, pEntityHeader, pEntityData))
	{
		return 0;
	}

	m_pDrawing->LastObjhandle++;

	return NewEntityObjhandle;
}

/*-------------------------------------------------------------------*
|  drwAddEntity                                                     |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      OBJHANDLE BlockObjhandle = handle of block that entity must  |
|                        be added in it. (NULL = Entity Section)    |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure to add |
|  Output: Handle of the new entity (0=Error)                       |
*-------------------------------------------------------------------*/
OBJHANDLE CJetDxfBaseView::drwAddEntity(OBJHANDLE BlockObjhandle, PENTITYHEADER pEntityHeader, LPVOID pEntityData)
{
	OBJHANDLE		NewEntityObjhandle;

	// Initialize m_pDrawing ------------------
	if (m_pDrawing  == NULL)
		return 0;

	NewEntityObjhandle = drwAddEntity_Direct(BlockObjhandle, pEntityHeader, pEntityData);
	if (NewEntityObjhandle == 0)
	{
		return 0;
	}

	return NewEntityObjhandle;
}

/*-------------------------------------------------------------------*
|  drwDeleteEntity                                                  |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      LPCTSTR strBlockName = block name that entity must be changed|
|                             in it. (NULL = Entities Section)      |
|      OBJHANDLE EntityHandle = handle of entity to be deleted      |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfBaseView::drwDeleteEntity(LPCTSTR strBlockName, OBJHANDLE EntityHandle)
{
	ENTITYHEADER	EntityHeader;
	char			EntityData[4096];

	// Initialize m_pDrawing ------------------
	if (m_pDrawing  == NULL)
		return 0;

	// Find the entity and mark it as "deleted"
	EntityHeader.Objhandle = EntityHandle;
	if (drwFindEntity_Direct(strBlockName, &EntityHeader, EntityData, FIND_BYHANDLE) == 0)
	{
		return FALSE;
	}

	EntityHeader.Deleted = TRUE;
	if (drwChangeEntity_Direct(strBlockName, &EntityHeader, &EntityData) == FALSE)
	{
		return FALSE;
	}

	// UnInitilize m_pDrawing -----------------
	return TRUE;
}

/*-------------------------------------------------------------------*
|  GetCurrentEntityData                                             |
|  Inputs:                                                          |
|      PENTITIES pEntities = pointer of ENTITIES structure          |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure        |
|  Output: current entity data size (0 = error occured)             |
*-------------------------------------------------------------------*/
DWORD CJetDxfBaseView::GetCurrentEntityData(PENTITIES pEntities, PENTITYHEADER pEntityHeader, LPVOID pEntityData)
{
	DWORD	EntityDataSize;

	// Initialize pEntitiesData ----------
	if (pEntities->pEntityHeap == NULL)
	{
		// GlobalLock error
		return 0;
	}

	CopyMemory(pEntityHeader, (LPVOID)((DWORD)pEntities->pEntityHeap + pEntities->CurrentEntityPos), sizeof(ENTITYHEADER));

	EntityDataSize = pEntityHeader->NextEntityPos - (pEntities->CurrentEntityPos + sizeof(ENTITYHEADER));
	CopyMemory(pEntityData, (LPVOID)((DWORD)pEntities->pEntityHeap + pEntities->CurrentEntityPos + sizeof(ENTITYHEADER)), EntityDataSize);

	return EntityDataSize;
}

/*-------------------------------------------------------------------*
|  SetEntityData                                                    |
|  Inputs:                                                          |
|      PENTITIES pEntities = pointer of ENTITIES structure          |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure        |
|      DWORD dwEntityPos = position of entity to set data on it     |
|  Output: current entity data size (0 = error occured)             |
*-------------------------------------------------------------------*/
DWORD CJetDxfBaseView::SetEntityData(PENTITIES pEntities, PENTITYHEADER pEntityHeader, LPVOID pEntityData, DWORD dwEntityPos)
{
	DWORD	EntityDataSize;

	// Initialize pEntitiesData ----------
	if (pEntities->pEntityHeap == NULL)
	{
		// GlobalLock error
		return 0;
	}

	CopyMemory((LPVOID)((DWORD)pEntities->pEntityHeap + dwEntityPos), pEntityHeader, sizeof(ENTITYHEADER));

	EntityDataSize = pEntityHeader->NextEntityPos - (dwEntityPos + sizeof(ENTITYHEADER));
	CopyMemory((LPVOID)((DWORD)pEntities->pEntityHeap + dwEntityPos + sizeof(ENTITYHEADER)), pEntityData, EntityDataSize);

	return EntityDataSize;
}

/*-------------------------------------------------------------------*
|  drwFindEntity_Direct                                             |
|  Inputs:                                                          |
|      m_pDrawing m_pDrawing = pointer to the current DRAWING structure |
|      LPCTSTR strBlockName = block name that entity must be found  |
|                             in it. (NULL = Entity Section)        |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure        |
|      DWORD dwFindType = find type                                 |
|      DWORD* pdwEntityPos = position of found entity               |
|  Output: Handle of found entity (0=Error or Not found)            |
*-------------------------------------------------------------------*/
OBJHANDLE CJetDxfBaseView::drwFindEntity_Direct(LPCTSTR strBlockName, PENTITYHEADER pEntityHeader, LPVOID pEntityData, DWORD dwFindType, DWORD* pdwEntityPos)
{
	PENTITIES		pEntities;
	OBJHANDLE		result;

	if (pEntityHeader == NULL)
	{
		// pEntityHeader can not be NULL
		return 0;
	}

	if (pEntityData == NULL)
	{
		// pEntityData can not be NULL
		return 0;
	}

	if (strBlockName != NULL)
	{ // Search for block name
		DWORD i = FindBlockPosByName(strBlockName);
		if (i<0)
		{
			// The block cannot be found
			return 0;
		}
		else
			pEntities = &m_pDrawing->Blocks[i].Entities;
	}
	else
		pEntities = &m_pDrawing->Entities;

	// Check number of entities
	if (pEntities->EntitiesNumber<1)
	{
		// There is no entity to find
		return 0;
	}

	result = 0;

	if (dwFindType & FIND_BYHANDLE)
	{
		ENTITYHEADER tempEntityHeader;
		char tempEntityData[4096];
		DWORD dwEntitySize;

		pEntities->CurrentEntityPos = 0;
		while (pEntities->CurrentEntityPos<pEntities->FreePos)
		{
			dwEntitySize = GetCurrentEntityData(pEntities, &tempEntityHeader, tempEntityData);
			if (pdwEntityPos != NULL)
				*pdwEntityPos = pEntities->CurrentEntityPos;

			pEntities->CurrentEntityPos = tempEntityHeader.NextEntityPos;
			if (pEntityHeader->Objhandle == tempEntityHeader.Objhandle)
			{
				CopyMemory(pEntityHeader, &tempEntityHeader, sizeof(ENTITYHEADER));
				CopyMemory(pEntityData, tempEntityData, dwEntitySize);
				result = pEntityHeader->Objhandle;
				break;
			}
		}
	}
	else
	{
		if (dwFindType & FIND_FIRST)
		{
			pEntities->CurrentEntityPos = 0;
			GetCurrentEntityData(pEntities, pEntityHeader, pEntityData);
			if (pdwEntityPos != NULL)
				*pdwEntityPos = pEntities->CurrentEntityPos;

			pEntities->CurrentEntityPos = pEntityHeader->NextEntityPos;
			result = pEntityHeader->Objhandle;
		}
		else if (dwFindType & FIND_LAST)
		{
			pEntities->CurrentEntityPos = pEntities->LastEntityPos;
			GetCurrentEntityData(pEntities, pEntityHeader, pEntityData);
			if (pdwEntityPos != NULL)
				*pdwEntityPos = pEntities->CurrentEntityPos;

			pEntities->CurrentEntityPos = pEntities->FreePos;
			result = pEntityHeader->Objhandle;
		}
		else if (dwFindType & FIND_PREVIOUS)
		{
			//		if(pEntities->CurrentEntityPos<=0)
			//			break;
			//		GetCurrentEntityData(pEntities, pEntityHeader, pEntityData);
			//		if(pdwEntityPos!=NULL)
			//			*pdwEntityPos = pEntities->CurrentEntityPos;

			//		pEntities->CurrentEntityPos = pEntityHeader->PreviousEntityPos;
			//		GetCurrentEntityData(pEntities, pEntityHeader, pEntityData);
			//		result = pEntityHeader->Objhandle;
		}
		else if (dwFindType & FIND_NEXT)
		{
			if (pEntities->CurrentEntityPos<pEntities->FreePos)
			{
				GetCurrentEntityData(pEntities, pEntityHeader, pEntityData);
				if (pdwEntityPos != NULL)
					*pdwEntityPos = pEntities->CurrentEntityPos;

				pEntities->CurrentEntityPos = pEntityHeader->NextEntityPos;
				result = pEntityHeader->Objhandle;
			}
		}
	}

	return result;
}

/*-------------------------------------------------------------------*
|  drwFindEntity                                                    |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      LPCTSTR strBlockName = block name that entity must be found  |
|                             in it. (NULL = Entities Section)      |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure        |
|      DWORD dwFindType = find type                                 |
|  Output: Handle of found entity (0=Error or Not found)            |
*-------------------------------------------------------------------*/
OBJHANDLE CJetDxfBaseView::drwFindEntity( LPCTSTR strBlockName, PENTITYHEADER pEntityHeader, LPVOID pEntityData, DWORD dwFindType)
{
	OBJHANDLE		result;

	// Initialize m_pDrawing ------------------
	if (m_pDrawing  == NULL)
		return FALSE;

	if ((result = drwFindEntity_Direct(strBlockName, pEntityHeader, pEntityData, dwFindType)) == 0)
	{
		return 0;
	}

	return result;
}

/*-------------------------------------------------------------------*
|  drwChangeEntity_Direct                                           |
|  Inputs:                                                          |
|      m_pDrawing m_pDrawing = pointer to the current DRAWING structure |
|      LPCTSTR strBlockName = block name that entity must be changed|
|                             in it. (NULL = Entities Section)      |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure        |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfBaseView::drwChangeEntity_Direct( LPCTSTR strBlockName, PENTITYHEADER pEntityHeader, LPVOID pEntityData)
{
	PENTITIES		pEntities;
	ENTITYHEADER	tempEntityHeader;
	char			tempEntityData[4096];
	DWORD			dwEntityPos;

	if (pEntityHeader == NULL)
	{
		// pEntityHeader can not be NULL
		return FALSE;
	}

	if (pEntityData == NULL)
	{
		// pEntityData can not be NULL
		return FALSE;
	}

	// Looking for entity if found then change it
	memcpy(&tempEntityHeader, pEntityHeader, sizeof(ENTITYHEADER));
	if (drwFindEntity_Direct(strBlockName, &tempEntityHeader, tempEntityData, FIND_BYHANDLE, &dwEntityPos) == 0)
	{
		// Entity object handle is not found
		return FALSE;
	}
	if (tempEntityHeader.EntityType != pEntityHeader->EntityType)
	{
		// Entity type is not matched with memory object
		return FALSE;
	}

	pEntityHeader->PreviousEntityPos = tempEntityHeader.PreviousEntityPos;
	pEntityHeader->NextEntityPos = tempEntityHeader.NextEntityPos;

	if (strBlockName != NULL)
	{ // Search for block name
		DWORD i = FindBlockPosByName(strBlockName);
		if (i<0)
		{
			// The block cannot be found
			return FALSE;
		}
		else
			pEntities = &m_pDrawing->Blocks[i].Entities;
	}
	else
		pEntities = &m_pDrawing->Entities;

	// Change entity with new given data
	SetEntityData(pEntities, pEntityHeader, pEntityData, dwEntityPos);

	return TRUE;
}


/*-------------------------------------------------------------------*
|  drwChangeEntity                                                  |
|  Inputs:                                                          |
|      HDRAWING hDrawing = handle to the current DRAWING structure  |
|      LPCTSTR strBlockName = block name that entity must be changed|
|                             in it. (NULL = Entities Section)      |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      LPVOID pEntityData = pointer to entity data structure        |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL CJetDxfBaseView::drwChangeEntity(LPCTSTR strBlockName, PENTITYHEADER pEntityHeader, LPVOID pEntityData)
{

	// Initialize m_pDrawing ------------------
	if (m_pDrawing  == NULL)
		return FALSE;

	if (drwChangeEntity_Direct( strBlockName, pEntityHeader, pEntityData) == FALSE)
	{
		return FALSE;
	}

	// UnInitilize m_pDrawing -----------------
	return true;
}


int CJetDxfBaseView::UnitToHorizontalPixel(double unit, double x, 
										   double y, double xScale,		
										   double yScale, double Rotation)
{
	return  m_pDrawing->View.WindowLeft + (int)((-m_pDrawing->View.ViewLeft + x + \
		    (unit)*xScale) * m_pDrawing->View.PPU * m_pDrawing->View.ZoomLevel);
}

int CJetDxfBaseView::UnitToVerticalPixel(double unit, double x, double y, double xScale, double yScale, double Rotation)
{
	return m_pDrawing->View.WindowBottom - (int)((-m_pDrawing->View.ViewBottom + y +\
			(unit)*yScale) * m_pDrawing->View.PPU * m_pDrawing->View.ZoomLevel);
}

int CJetDxfBaseView::UnitToXRotationPixel(double xUnit, double yUnit, double x, double y, double xScale, double yScale, double Rotation)
{
	return m_pDrawing->View.WindowLeft + (int)((-m_pDrawing->View.ViewLeft + x + \
		   ((xUnit)*xScale*cos(Rotation*PI / 180) - (yUnit)*yScale*sin(Rotation*PI / 180)) * 1) *\
		   m_pDrawing->View.PPU * m_pDrawing->View.ZoomLevel);
}

int CJetDxfBaseView::UnitToYRotationPixel(double xUnit, double yUnit, double x , double y, double xScale, double yScale, double Rotation)
{
	return m_pDrawing->View.WindowBottom - (int)((-m_pDrawing->View.ViewBottom + y + \
		((xUnit)*xScale*sin(Rotation*PI / 180) + (yUnit)*yScale*cos(Rotation*PI / 180)) * 1) * \
		 m_pDrawing->View.PPU * m_pDrawing->View.ZoomLevel);
}