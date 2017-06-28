#include "../stdafx.h"
#include "JetDxfBase.h"
#include "JetDxfRead.h"

CJetDxfBase::CJetDxfBase(CJetDxfRead* pDxfRead) :m_pDxfRead(pDxfRead)
{
}


CJetDxfBase::~CJetDxfBase()
{
}


BOOL CJetDxfBase::dxfWriteParamString(
	int		GroupCode,	// group code
	LPCTSTR	value		// pointer to null terminated string to put in DXF
	)
{
	if (NULL == m_pDxfRead->pDxf)
		return false;

	if (fprintf(m_pDxfRead->pDxf->pStream, "%d\n%s\n", GroupCode, value) <= 0)
	{
		// file write error
		return FALSE;
	}

	return TRUE;
}

BOOL CJetDxfBase::dxfWriteParamInteger(
	int		GroupCode,	// group code
	int		value		// an integer value to put in dxf file
	)
{
	if (NULL == m_pDxfRead->pDxf)
		return FALSE;

	if (fprintf(m_pDxfRead->pDxf->pStream, "%d\n%d\n", GroupCode, value) <= 0)
	{
		// file write error
		return FALSE;
	}


	return TRUE;
}

BOOL CJetDxfBase::dxfWriteParamDouble(
	int		GroupCode,	// group code
	double	value		// a real value to put in DXF file
	)
{
	if (NULL == m_pDxfRead->pDxf)
		return FALSE;

	if (fprintf(m_pDxfRead->pDxf->pStream, "%d\n%f\n", GroupCode, value) <= 0)
	{
		// file write error
		return FALSE;
	}

	return TRUE;
}

