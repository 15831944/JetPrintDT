#ifndef __JET_DXF_READ_H
#define __JET_DXF_READ_H

#include "JetDXFDef.h"
#include "SimpleList.h"

class CJetDxfBlock;
class CJetDxfTable;
class CJetDxfEntities;
class CJetDxfView;


#define	READBUFFERSIZE	32768

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------
typedef struct tag_DXFREAD
{
	long	FileSize;
	long	StoredStreamPos;
	DWORD	CurrentSection;
	DWORD	CurrentTableType;
	bool	isBlockOpen;

	char	ReadBuffer[READBUFFERSIZE];
	long	BufferDataSize;
	long	BufferPos;
	long	CurrentPos;
} DXFREAD, *PDXFREAD;

typedef struct tag_DXFWRITE
{
	char	CurrentLayer[32];
	char	CurrentLine[32];
	char	CurrentTextStyle[32];
	char	CurrentDimStyle[32];
	int		CurrentColor;
	DWORD	DoneSections;
	DWORD	CurrentSection;
	DWORD	DoneTableTypes;
	DWORD	CurrentTableType;
	UINT	DimStylesNumber;
} DXFWRITE, *PDXFWRITE;

typedef struct tag_DXF
{
	DWORD		Size;
	FILE*		pStream;
	DXFWRITE	Write;
	DXFREAD		Read;
	BOOL		isWriteMode;
} DXF, *PDXF;

class CJetDxfRead
{
public:
	CJetDxfRead();
	~CJetDxfRead();

	HDXF dxfOpenFile(
		LPCTSTR	FileName	// pointer to DXF file name
		);

	// *** Sections ***
	DWORD dxfFindNextSection();

	DWORD dxfGetCurrentSection();

	PDXF InitilizePDXF(HDXF hDxf);
	BOOL UnInitilizePDXF(HDXF hDxf);


	// *** DXF Read ***
	void ReadLine(LPTSTR valuestr);

	BOOL ReadParamFromDxfFile(
		int&	GroupCode,	// group code
		LPTSTR valuestr		// pointer to null terminated string to put data in it
		);

	BOOL dxfReadParam(
		int&	GroupCode,	// group code
		LPTSTR	valuestr	// pointer to null terminated string to put data in it
		);

	BOOL FindParamFromDxfFile(
		int		GroupCode,	// group code
		LPCTSTR	valuestr	// pointer to null terminated string to find
		);

	BOOL dxfFindParam(
		int		GroupCode,	// group code
		LPCTSTR	valuestr	// pointer to null terminated string to find
		);

	BOOL dxfStorePos();

	BOOL dxfRestorePos();

	BOOL LoadDataFromFile(OBJHANDLE BlockObjhandle,LPCTSTR strFileName);
	void ReadEntitiesDataFromFile(OBJHANDLE BlockObjHandle);

	bool InitView(CJetDxfView *pView);
	bool dxfCloseFile();
public:
	PDXF	pDxf;
	HDXF    hDxf;
	FILE*	pStream;
	CJetDxfTable *m_pDxfTable;
	CJetDxfBlock *m_pDxfBlock;
	CJetDxfEntities *m_pDxfEntity;

	CJetDxfView * m_pDxfView;
};
#endif  //__JET_DXF_READ_H




