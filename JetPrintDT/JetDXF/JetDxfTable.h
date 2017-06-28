#pragma once
#include "JetDxfBase.h"
#include "JetDxfRead.h"

class CJetDxfTable :
	public CJetDxfBase
{
public:
	CJetDxfTable(CJetDxfRead *pDxfRead);
	~CJetDxfTable();

	bool dxfReadData(HDXF	hDxf// handle to the openning DXF file structure
		             );
	bool dxfWriteData(HDXF pDxf);
	DWORD dxfReadTableTypeName();

	DWORD dxfGetCurrentTableType();

	DWORD dxfFindNextTableType();
	DWORD dxfReadTableType(LPVOID	pTableType);

	BOOL ReadDimStyleData(PDXFDIMSTYLE pDimStyle);
	BOOL ReadLayerData(PDXFLAYER pLayer);
	BOOL ReadLTypeData(PDXFLTYPE pLType);
	BOOL ReadStyleData(PDXFSTYLE pStyle);

	

private:
	int		GCode;
};

