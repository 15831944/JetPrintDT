#ifndef __JET_DFX_BLOCK_H
#define __JET_DFX_BLOCK_H
#include "JetDxfBase.h"

class CJetDxfBlock :
	public CJetDxfBase
{
public:
	CJetDxfBlock(CJetDxfRead *pDxfRead);
	~CJetDxfBlock();

public:
	bool dxfReadBlockHeader(PDXFBLOCKHEADER pBlockHeader);

	bool ReadBlockHeader(PDXFBLOCKHEADER pBlockHeader);

	bool dxfReadData(HDXF pDxf);
	bool dxfWriteData(HDXF hDxf);
	bool dxfGetBlockOpen();
	bool dxfFindBlockHeader( PDXFBLOCKHEADER	pBlockHeader/*pointer to blockheader structure*/);

public:
	PDXFBLOCKHEADER m_pDxfBlockHeader;
};

#endif //__JET_DFX_BLOCK_H



