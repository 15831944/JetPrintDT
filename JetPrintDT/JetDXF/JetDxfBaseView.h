#ifndef __JET_DXF_DRAW_H
#define __JET_DXF_DRAW_H

#include "JetDXFDef.h"

#define		MAX_DIMSTYLES	64
#define		MAX_LAYERS		128
#define		MAX_LTYPES		128
#define		MAX_STYLES		128


// Tables ***************************
typedef struct tag_TABLES
{
	LAYER		Layer[MAX_LAYERS];
	LTYPE		LType[MAX_LTYPES];
	STYLE		Style[MAX_STYLES];
	DIMSTYLE	DimStyle[MAX_DIMSTYLES];

	UINT		NumLayers;
	UINT		NumLTypes;
	UINT		NumStyles;
	UINT		NumDimStyles;

	UINT		CurrentLayer;
	UINT		CurrentLType;
	UINT		CurrentStyle;
	UINT		CurrentDimStyle;

	//	OBJHANDLE	CurrentLayerHandle;		// Current Layer for drawing
	//	OBJHANDLE	CurrentTextStyleHandle;	// Current TextStyle for drawing
	//	OBJHANDLE	CurrentDimStyleHandle;	// Current DimStyle for drawing
	//	OBJHANDLE	CurrentLineTypeHandle;	// Current LineType for drawing
	//	short		CurrentColor;			// Current Color for drawing
	//	short		CurrentLineWeight;		// Current LineWeight for drawing
} TABLES, *PTABLES;

// Drawing Structure ****************
typedef struct tag_DRAWING
{
	TABLES		Tables;
	ENTITIES	Entities;

	BLOCKHEADER	Blocks[256];
	DWORD		BlocksNumber;
	DWORD		CurrentBlock;

	OBJHANDLE	LastObjhandle;
	DWORD		LastDimBlkNum;

	VIEW		View;
} DRAWING, *PDrawing;

class CJetDxfBaseView
{
public:
	CJetDxfBaseView();
	~CJetDxfBaseView();

	
	BOOL drwCreate();

	BOOL drwDestroy(); //Free m_pDrawing And m_hDrawing

	// TABLES ******
	OBJHANDLE drwAddTableType(
		DWORD		dwTableType,		// table type to be added
		LPVOID		pTableType			// pointer to table type structure to add
		);

	BOOL drwDeleteTableType(
		DWORD		dwTableType,		// table type to be removed
		OBJHANDLE	TableTypeObjhandle	// handle of table type to remove
		);

	OBJHANDLE drwFindTableType_Direct(
		DWORD		dwTableType,		// table type to get data ( TAB_NOTSET = search in all table types )
		DWORD		dwFindType,			// find type
		LPVOID		pTableType			// pointer to table type structure
		);

	OBJHANDLE drwFindTableType(
		DWORD		dwTableType,		// table type to get data ( TAB_NOTSET = search in all table types )
		DWORD		dwFindType,			// find type
		LPVOID		pTableType			// pointer to table type structure
		);

	// BLOCKS ******
	OBJHANDLE drwAddBlock_Direct(
		PBLOCKHEADER	pBlockHeader	// pointer to blockheader structure
		);

	OBJHANDLE drwAddBlock(
		PBLOCKHEADER	pBlockHeader	// pointer to blockheader structure
		);

	BOOL drwDeleteBlock(
		OBJHANDLE		BlockObjhandle	// handle of block to Delete
		);

	OBJHANDLE drwFindBlock_Direct(
		DWORD			dwFindType,		// find type
		PBLOCKHEADER	pBlockHeader	// pointer to block header data
		);

	OBJHANDLE drwFindBlock(
		DWORD			dwFindType,		// find type
		PBLOCKHEADER	pBlockHeader	// pointer to block header data
		);

	int drwGetBlocksNum();
	int FindBlockPosByName(
		LPCTSTR strBlockName			//BlockName As InPut
		);

	int FindBlockPosByHandle(
		OBJHANDLE BlockObjhandle		// handle of block that entity must be added in it. (NULL = Entity Section)
		);

	// ENTITIES ******
	BOOL AddToEntitiesList(
		PENTITIES pEntities,            //pointer to entity object
		PENTITYHEADER pEntityHeader,    // pointer to entity data header
		LPVOID pEntityData,				// pointer to entity data structure to add
		WORD EntitySize);               //entity object size

	OBJHANDLE drwAddEntity_Direct(
		OBJHANDLE		BlockObjhandle,	// handle of block that entity must be added in it. (NULL = Entity Section)
		PENTITYHEADER	pEntityHeader,	// pointer to entity data header
		LPVOID			pEntityData		// pointer to entity data structure to add
		);

	BOOL AddEntity(
		PENTITIES pEntities,			//pointer to entity object
		PENTITYHEADER pEntityHeader,	//pointer to entity header
		LPVOID pEntityData);			//pointer to entity data

	OBJHANDLE drwAddEntity(
		OBJHANDLE		BlockObjhandle,	// handle of block that entity must be added in it. (NULL = Entity Section)
		PENTITYHEADER	pEntityHeader,	// pointer to entity data header
		LPVOID			pEntityData		// pointer to entity data structure to add
		);

	BOOL drwDeleteEntity(
		LPCTSTR			strBlockName,	// block name that entity must be deleted in it. (NULL=Entity Section)
		OBJHANDLE		EntityHandle	// handle of entity to be deleted
		);

	DWORD GetCurrentEntityData(
		PENTITIES pEntities,			//pointer to entity object
		PENTITYHEADER pEntityHeader,	//pointer to entity header
		LPVOID pEntityData				//pointer to entity data
		);

	DWORD SetEntityData(
		PENTITIES pEntities,			//pointer to entity
		PENTITYHEADER pEntityHeader,	//pointer to entity header
		LPVOID pEntityData,				//pointer to entity data	
		DWORD dwEntityPos               //entity pos in entity list
		);

	OBJHANDLE drwFindEntity_Direct(
		LPCTSTR			strBlockName,	// block name that entity must be found in it. (NULL=Entity Section)
		PENTITYHEADER	pEntityHeader,	// pointer to entity data header
		LPVOID			pEntityData,	// pointer to entity data structure
		DWORD			dwFindType,		// find type
		DWORD*			pdwEntityPos = NULL// position of found entity
		);

	OBJHANDLE drwFindEntity(
		LPCTSTR			strBlockName,	// block name that entity must be found in it. (NULL=Entity Section)
		PENTITYHEADER	pEntityHeader,	// pointer to entity data header
		LPVOID			pEntityData,	// pointer to entity data structure
		DWORD			dwFindType		// find type
		);

	BOOL drwChangeEntity_Direct(
		LPCTSTR			strBlockName,	// block name that entity must be changed in it. (NULL=Entity Section)
		PENTITYHEADER	pEntityHeader,	// pointer to entity data header
		LPVOID			pEntityData		// pointer to entity data structure
		);

	BOOL drwChangeEntity(
		LPCTSTR			strBlockName,	// block name that entity must be changed in it. (NULL=Entity Section)
		PENTITYHEADER	pEntityHeader,	// pointer to entity data header
		LPVOID			pEntityData		// pointer to entity data structure
		);

	int UnitToHorizontalPixel(double unit, double x = 0, double y = 0, double xScale = 1, double yScale = 1, double Rotation = 0);
	int UnitToVerticalPixel(double unit, double x = 0, double y = 0, double xScale = 1, double yScale = 1, double Rotation = 0);
	int UnitToXRotationPixel(double xUnit, double yUnit, double x = 0, double y = 0, double xScale = 1, double yScale = 1, double Rotation = 0);
	int UnitToYRotationPixel(double xUnit, double yUnit, double x = 0, double y = 0, double xScale = 1, double yScale = 1, double Rotation = 0);

public:
	PDrawing m_pDrawing;
};


#endif //__JET_DXF_DRAW_H

