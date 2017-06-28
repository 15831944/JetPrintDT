#include "../stdafx.h"
#include "JetDxfBlock.h"
#include "JetDxfRead.h"

CJetDxfBlock::CJetDxfBlock(CJetDxfRead *pDxfRead) :CJetDxfBase(pDxfRead)
{
	m_pDxfBlockHeader = NULL;
}


CJetDxfBlock::~CJetDxfBlock()
{
}


bool CJetDxfBlock::dxfGetBlockOpen()
{
	bool	result = false;

	if (NULL == m_pDxfRead->pDxf)
		return false;

	result = m_pDxfRead->pDxf->Read.isBlockOpen;

	
	return result;
}


bool CJetDxfBlock::dxfReadBlockHeader(PDXFBLOCKHEADER pBlockHeader)
{
	
	if (NULL == m_pDxfRead->pDxf)
		return false;

	// Check if current section is BLOCKS
	if (m_pDxfRead->pDxf->Read.CurrentSection != SEC_BLOCKS)
	{
		// Current section is not BLOCKS
		return false;
	}

	m_pDxfRead->dxfStorePos();
	if (m_pDxfRead->FindParamFromDxfFile(0, _T("BLOCK")))
	{
		ReadBlockHeader(pBlockHeader);
	}
		
	else
	{
		// Can not read block header
		m_pDxfRead->dxfRestorePos();
		return FALSE;
	}

	
	return true;
}

bool CJetDxfBlock::ReadBlockHeader(PDXFBLOCKHEADER pBlockHeader)
{
	int		GCode;
	LPTSTR pParam = new TCHAR[1024];
	if (NULL == pParam)
	{
		return FALSE;
	}

	m_pDxfRead->dxfStorePos();
	m_pDxfRead->ReadParamFromDxfFile(GCode, pParam);
	while (GCode != 0)
	{
		switch (GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pBlockHeader->LayerName, CW2A(pParam));
			break;
		case 2:		// Block Name
			strcpy_s(pBlockHeader->Name, CW2A(pParam));
			break;
		case 70:	// Block-type flags
			pBlockHeader->Flags = char(_wtoi(pParam));
		case 10:	// Base point X
			pBlockHeader->BasePoint.x = _wtoi(pParam);
			break;
		case 20:	// Base point Y
			pBlockHeader->BasePoint.y = _wtoi(pParam);
			break;
		case 30:	// Base point Z
			pBlockHeader->BasePoint.z = _wtoi(pParam);
			break;
		case 3:		// Block Name
			strcpy_s(pBlockHeader->Name, CW2A(pParam));
			break;
		}
		m_pDxfRead->dxfStorePos();
		m_pDxfRead->ReadParamFromDxfFile(GCode, pParam);
	}

	if ((GCode == 0) && (strcmp(CW2A(pParam), "ENDBLK") == 0))
	{
		m_pDxfRead->pDxf->Read.isBlockOpen = FALSE;

		do{
			m_pDxfRead->dxfStorePos();
			m_pDxfRead->ReadParamFromDxfFile(GCode, pParam);
		} while (GCode != 0);

		if (strcmp(CW2A(pParam), "ENDSEC") == 0)
			m_pDxfRead->pDxf->Read.CurrentSection = SEC_NOTSET; // Blocks section has been finished
		else // Reached to next block
			m_pDxfRead->dxfRestorePos();
	}
	else
	{
		m_pDxfRead->dxfRestorePos();
		m_pDxfRead->pDxf->Read.isBlockOpen = TRUE;
	}


	if(NULL != pParam)
	{
		delete [] pParam;
		pParam = NULL;
	}
	return true;
}

bool CJetDxfBlock::dxfReadData(HDXF pDxf)
{
	return true;
}

bool CJetDxfBlock::dxfWriteData(HDXF hDxf)
{
	return true;
}

bool CJetDxfBlock::dxfFindBlockHeader(PDXFBLOCKHEADER	pBlockHeader)
{
	if (NULL == m_pDxfRead->pDxf)
		return false;
	

	if (m_pDxfRead->pDxf->Read.CurrentSection != SEC_BLOCKS)
	{
		return false;
	}

	m_pDxfRead->dxfStorePos();
	if (m_pDxfRead->FindParamFromDxfFile(0, _T("BLOCK")))
	{
		//m_pDxfBlockHeader = pBlockHeader;
		//dxfReadDataDirect();
		ReadBlockHeader(pBlockHeader);
	}
	else
	{
		// Can not read block header
		m_pDxfRead->dxfRestorePos();
		return FALSE;
	}

	
	return TRUE;
}