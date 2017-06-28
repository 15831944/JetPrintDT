
#ifndef __JET_DFX_BASE_H
#define __JET_DFX_BASE_H

#include "afx.h"
#include "JetDXFDef.h"
class CJetDxfRead;


class CJetDxfBase :
	public CObject
{
public:
	CJetDxfBase(CJetDxfRead* pDxfRead);
	virtual ~CJetDxfBase();

public:
	virtual bool dxfReadData(HDXF pDxf) = 0;
	virtual bool dxfWriteData(HDXF pDxf) = 0;

	// *** DXF Writer ***
	BOOL dxfWriteParamString(
		int		GroupCode,	// group code
		LPCTSTR	value		// pointer to null terminated string to put in DXF
		);

	BOOL dxfWriteParamInteger(
		int		GroupCode,	// group code
		int		value		// an integer value to put in dxf file
		);

	BOOL dxfWriteParamDouble(
		int		GroupCode,	// group code
		double	value		// a real value to put in DXF file
		);

public:
	CJetDxfRead *m_pDxfRead;

};


#endif

