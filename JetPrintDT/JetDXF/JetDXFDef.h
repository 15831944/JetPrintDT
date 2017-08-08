/*-------------------------------------------------------------------*\
|  CadLib Version 2.1                                                 |
|  Written by Omid Shahabi <omid_shahabi@hotmail.com>                 |
|  Copyright ?2002-2004                                              |
|  Pars Technology Development Co.                                    |
|                                                                     |
|  This software is distributed on an "AS IS" basis, WITHOUT WARRANTY |
|  OF ANY KIND, either express or implied.                            |
|                                                                     |
|                                                                     |
|  DXFDef.h                                                           |
\*-------------------------------------------------------------------*/

#ifndef DXFDEF_H
#define DXFDEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../stdafx.h"
#include <vector>
#include <list>
#include <map>
using namespace std;

// Handle to DXF
DECLARE_HANDLE(HDXF);

//
#define COORDINATE_X    0x0001
#define COORDINATE_Y    0x0002

// Section Definitions
#define	SEC_NOTSET		0x0000
#define	SEC_HEADER		0x0001
#define	SEC_CLASSES		0x0002
#define	SEC_TABLES		0x0004
#define	SEC_BLOCKS		0x0008
#define	SEC_ENTITIES	0x0010
#define	SEC_OBJECTS		0x0020
#define	SEC_UNKNOWN		0x0040

// Table Type Definitions
#define	TAB_NOTSET		0x0000
#define	TAB_APPID		0x0001
#define	TAB_BLOCKRECORD	0x0002
#define	TAB_DIMSTYLE	0x0004
#define	TAB_LAYER		0x0008
#define	TAB_LTYPE		0x0010
#define	TAB_STYLE		0x0020
#define	TAB_UCS			0x0040
#define	TAB_VIEW		0x0080
#define	TAB_VPORT		0x0100
#define	TAB_UNKNOWN		0x0200

// Text Justification Types
#define	TJ_LEFT			0x0000
#define	TJ_CENTER		0x0001
#define	TJ_RIGHT		0x0002
#define	TJ_ALIGNED		0x0003
#define	TJ_MIDDLE		0x0004
#define	TJ_FIT			0x0005
#define	TJ_BOTTOMLEFT	0x0100
#define	TJ_BOTTOMCENTER	0x0101
#define	TJ_BOTTOMRIGHT	0x0102
#define	TJ_MIDDLELEFT	0x0200
#define	TJ_MIDDLECENTER	0x0201
#define	TJ_MIDDLERIGHT	0x0202
#define	TJ_TOPLEFT		0x0300
#define	TJ_TOPCENTER	0x0301
#define	TJ_TOPRIGHT		0x0302

// Structures
//  Base ----------
typedef struct tag_REALPOINT
{
	double	x;
	double	y;
	double	z;
} REALPOINT, *PREALPOINT;

typedef struct tag_REALRECT
{
	double	top;
	double	left;
	double	bottom;
	double	right;
} REALRECT, *PREALRECT;

//  TABLES --------
typedef struct tag_DXFLAYER
{
	char	Name[512];					// Layer Name
	int		StandardFlags;				// Standard flags
	int		Color;						// Layer Color
	char	LineType[512];				// Line Type
} DXFLAYER, *PDXFLAYER;

typedef struct tag_DXFLTYPE
{
	char	Name[512];					// Line Type Name
	int		StandardFlags;				// Standard flags
	char	DescriptiveText[512];		// Descriptive Text
	int		ElementsNumber;				// Line Elements Number
	double	TotalPatternLength;			// Total Pattern Length
	double	Elements[30];				// Line Elements
} DXFLTYPE, *PDXFLTYPE;

typedef struct tag_DXFSTYLE
{
	char	Name[512];					// Style Name
	char	PrimaryFontFilename[512];	// Primary Font Filename
	int		StandardFlags;				// Standard flag values -> 4=Vertical text
	int		TextGenerationFlags;		// Text generation flags -> 2=Text is backward  4=Text is upside down
	double	FixedTextHeight;			// Fixed text height
	double	WidthFactor;				// Width Factor
	double	Height;						// Height
	double	ObliqueAngle;				// Oblique angle
} DXFSTYLE, *PDXFSTYLE;

typedef struct tag_DXFDIMSTYLE
{
	char	Name[512];					// DimStyle Name
	int		StandardFlags;				// Standard flag values
	int		DIMCLRD;					// Dimension line & Arrow heads color
	double	DIMDLE;						// Dimension line size after Extensionline
	int		DIMCLRE;					// Extension line color
	double	DIMEXE;						// Extension line size after Dimline
	double	DIMEXO;						// Offset from origin
	char	DIMBLK1[512];				// 1st Arrow head
	char	DIMBLK2[512];				// 2nd Arrow head
	double	DIMASZ;						// Arrow size
	char	DIMTXSTY[512];				// Text style
	DWORD	DIMTXSTYObjhandle;			// Text style Object Handle
	int		DIMCLRT;					// Text color
	double	DIMTXT;						// Text height
	int		DIMTAD;						// Vertical Text Placement
	double	DIMGAP;						// Offset from dimension line
} DXFDIMSTYLE, *PDXFDIMSTYLE;

//  BLOCKS --------
typedef struct tag_DXFBLOCKHEADER
{
	char		Name[512];				// Block name
	char		Flags;					// Block-type flags
	REALPOINT	BasePoint;				// Base point
	char		LayerName[512];			// Layer Name
} DXFBLOCKHEADER, *PDXFBLOCKHEADER;

//  ENTITIES ------
typedef struct tag_DXFENTITYHEADER
{
	unsigned short	EntityType;			// Entity type
	char		LayerName[512];			// Used Layer's Name
	char		LTypeName[512];			// Used LineType's Name
	short		Color;					// Color (0=BYBLOCK, 256=BYLAYER, negative=layer is turned off)
	double		Thickness;				// Thickness (default=0)
	double		LineTypeScale;			// Linetype scale (default=1.0)
	double		ExtrusionDirection[3];	// Extrusion direction. (default = 0, 0, 1)
	short		LineWeight;				// Lineweight enum value (*2000*)
} DXFENTITYHEADER, *PDXFENTITYHEADER;

typedef struct tag_DXFENTLINE {
	REALPOINT	Point0;
	REALPOINT	Point1;
} DXFENTLINE, *PDXFENTLINE;

typedef struct tag_DXFENTARC {
	REALPOINT	Point0;
	double		Radius;
	double		StartAngle;
	double		EndAngle;
} DXFENTARC, *PDXFENTARC;

typedef struct tag_DXFENTCIRCLE {
	REALPOINT	Point0;
	double		Radius;
} DXFENTCIRCLE, *PDXFENTCIRCLE;

typedef struct tag_DXFENTDIMENSION {
	REALPOINT	DefPoint3;
	REALPOINT	DefPoint4;
	double		DimRotationAngle;
	REALPOINT	DimLineDefPoint;
	char		BlockName[16];
	char		DimStyleName[512];
	char		DimText[1024];
} DXFENTDIMENSION, *PDXFENTDIMENSION;

typedef struct tag_DXFENTINSERT {
	REALPOINT	Point0;
	double		XScale;
	double		YScale;
	double		ZScale;
	double		RotationAngle;
	char		BlockHeaderName[512];
} DXFENTINSERT, *PDXFENTINSERT;

typedef struct tag_DXFENTPOINT {
	REALPOINT	Point0;
} DXFENTPOINT, *PDXFENTPOINT;

typedef struct tag_DXFENTSOLID {
	REALPOINT	Point0;
	REALPOINT	Point1;
	REALPOINT	Point2;
	REALPOINT	Point3;
} DXFENTSOLID, *PDXFENTSOLID;

typedef struct tag_DXFTEXTDATA {
	double		Height;
	double		RotationAngle;
	double		WidthFactor;
	double		Oblique;
	char		GenerationFlag;
	short		Justification;
	REALPOINT	SecondAlignmentPoint;
	char		TextStyleName[512];
} _DXFTEXTDATA,*_PDXFTEXTDATA;

typedef struct tag_DXFENTTEXT {
	REALPOINT		Point0;
	_DXFTEXTDATA	TextData;
	char			strText[2048];
} DXFENTTEXT, *PDXFENTTEXT;

typedef struct tag_DXFENTVERTEX {
  REALPOINT			Point;
  unsigned short	Flag;
  double			StartWidth;
  double			EndWidth;
  double			Bulge;
  double			TangentDir;
} DXFENTVERTEX, *PDXFENTVERTEX;

// this code is implemented by tran duy dung 20.10.2003
typedef struct tag_DXFENTPOLYLINE {
	PDXFENTVERTEX	pVertex;
	int				nVertex;
	unsigned short	Flag;
} DXFENTPOLYLINE, *PDXFENTPOLYLINE;

typedef struct tag_DXFENTELLIPSE {
  REALPOINT		CenterPoint;
  REALPOINT		MajorAxisEndPoint;
  double		MinorToMajorRatio;
  double		StartParam;
  double		EndParam;
} DXFENTELLIPSE, *PDXFENTELLIPSE;

typedef struct tag_DXFENTLWPOLYLINE{
	UINT        CountOfVertex;
	bool        b_PolylineClose;
	double  *   pCoordinateX;
	double  *   pCoordinateY;
}DXFENTLWPOLYLINE, *PDXFENTLWPOLYLINE;

typedef struct tag_ENTLWPOLYLINE{
	UINT        CountOfVertex;
	bool        b_PolylineClose;
	REALPOINT *pVertexPoint;
}ENTLWPOLYLINE,*PENTLWPOLYLINE;

typedef struct tag_ENTSPLINE{
	UINT        SplineRank;
	UINT        SplineKnots;
	UINT        SplineControlPoints;
	UINT        SplineMatchPoints;
	double    * pKnotPoint;
	REALPOINT * pControlPoint;
}ENTSPLINE, *PENTSPLINE;

typedef struct tag_DXFENTSPLINE{
	UINT        SplineRank;
	UINT        SplineKnots;
	UINT        SplineControlPoints;
	UINT        SplineMatchPoint;
	UINT        uFlag;
	double    * pControlPoint_X;
	double    * pControlPoint_Y;
	double    * pKontPoint;
}DXFENTSPLINE,*PDXFENTSPLINE;

// Arrow Heads --------
struct ARROWHEAD
{
	LPCTSTR	ArrowHeadName;
	int		BLKNum;
	LPCTSTR	Command;
	double	Param1;
	double	Param2;
	double	Param3;
	double	Param4;
	double	Param5;
	double	Param6;
	double	Param7;
	double	Param8;
};

// Arrow Heads Data
static ARROWHEAD ArrowHeads[] =
{
	{reinterpret_cast<LPCTSTR>("ClosedFilled"), 1, reinterpret_cast<LPCTSTR>("SOLID"), 0, 0, -1, -0.1667, -1, 0.1667, -1, 0.1667},
	{reinterpret_cast<LPCTSTR>("ClosedFilled"), 2, reinterpret_cast<LPCTSTR>("SOLID"), 0, 0, -1, -0.1667, -1, 0.1667, -1, 0.1667},
	{reinterpret_cast<LPCTSTR>("Oblique"), 1, reinterpret_cast<LPCTSTR>("LINE"), -0.5, -0.5, 0.5, 0.5, 0, 0, 0, 0},
	{reinterpret_cast<LPCTSTR>("Oblique"), 2, reinterpret_cast<LPCTSTR>("LINE"), -0.5, -0.5, 0.5, 0.5, 0, 0, 0, 0},
};

static int ArrowHeadsNum = sizeof(ArrowHeads) / sizeof(ARROWHEAD);



DECLARE_HANDLE(HDRAWING);

// Definitions
typedef DWORD OBJHANDLE;
#define	MAX_STRLEN 512
#define PI	3.1415926535

// Entities and Objects Definitions
#define	ENT_LINE		1
#define	ENT_POINT		2
#define	ENT_CIRCLE		3
#define	ENT_SHAPE		4
#define	ENT_ELLIPSE		5
#define ENT_SPLINE		6
#define ENT_TEXT		7
#define ENT_ARC			8
#define ENT_TRACE		9
#define ENT_REGION		10
#define ENT_SOLID		11
#define ENT_BLOCK		12
#define ENT_ENDBLK		13
#define ENT_INSERT		14
#define ENT_ATTDEF		15
#define ENT_ATTRIB		16
#define ENT_SEQEND		17
#define ENT_POLYLINE	19
#define ENT_VERTEX		20
#define ENT_LINE3D		21
#define ENT_FACE3D		22
#define ENT_DIMENSION	23
#define ENT_VIEWPORT	24
#define ENT_SOLID3D		25
#define ENT_RAY			26
#define ENT_XLINE		27
#define ENT_MTEXT		28
#define ENT_LEADER		29
#define ENT_TOLERANCE	30
#define ENT_MLINE		31
#define OBJ_DICTIONARY	32
#define OBJ_MLINESTYLE	33
#define OBJ_CLASS		34
#define ENT_BODY		35
#define ENT_LWPOLYLINE  37
#define OBJ_GROUP		36
#define OBJ_PROXY		100
#define OBJ_XRECORD		101
#define OBJ_IDBUFFER	102

// Flag Definitions
#define	POLYLINE_CLOSED			1
#define	POLYLINE_CURVEFIT		2
#define	POLYLINE_SPLINEFIT		4
#define	POLYLINE_3DPLINE		8
#define	POLYLINE_3DMESH			16
#define	POLYLINE_CLOSEDN		32
#define	POLYLINE_POLYFACEMESH	64
#define	POLYLINE_CONTINUELT		128

// FindType Definitions
#define FIND_FIRST				0x0001
#define FIND_LAST				0x0002
#define FIND_NEXT				0x0004
#define FIND_PREVIOUS			0x0008
#define FIND_BYTYPE				0x0010
#define FIND_BYLAYEROBJHANDLE	0x0020
#define FIND_BYLTYPEOBJHANDLE	0x0040
#define FIND_BYCOLOR			0x0080
#define FIND_BYTHICKNESS		0x0100
#define FIND_BYNAME				0x0200
#define FIND_BYHANDLE			0x0400

// Color Definitions
#define	COLOR_BYLAYER	256
#define	COLOR_BYBLOCK	0
#define	COLOR_RED		1
#define	COLOR_YELLOW	2
#define	COLOR_GREEN		3
#define	COLOR_CYAN		4
#define	COLOR_BLUE		5
#define	COLOR_MAGENTA	6
#define	COLOR_WHITE		7


// Tables Structures *******************************************

typedef struct tag_DIMSTYLE {
	OBJHANDLE Objhandle;			// Handle
	char	Name[MAX_STRLEN];		// name of dimension style
	char	StandardFlags;			// Standard flag values
	double	dimasz;					// Arrow size
	//	OBJHANDLE dimblk1Objhandle;		// 1st Arrow head
	//	OBJHANDLE dimblk2Objhandle;		// 2nd Arrow head
	char	dimblk1[16];			// 1st Arrow head
	char	dimblk2[16];			// 2nd Arrow head
	short	dimclrd;				// Dimension line & Arrow heads color
	short	dimclre;				// Extension line color
	short	dimclrt;				// Text color
	double	dimdle;					// Dimension line size after Extensionline
	double	dimexe;					// Extension line size after Dimline
	double	dimexo;					// Offset from origin
	double	dimgap;					// Offset from dimension line
	double	dimtxt;					// Text height
	char	dimtad;					// Vertical Text Placement
	OBJHANDLE dimtxstyObjhandle;	// Text style handle
} DIMSTYLE, *PDIMSTYLE;

typedef struct tag_LAYER {
	OBJHANDLE Objhandle;			// Handle
	char	Name[MAX_STRLEN];		// Layer Name
	char	StandardFlags;			// Standard flags
	short	Color; 					// Layer Color (if negative, layer is Off)
	OBJHANDLE LineTypeObjhandle;	// Handle of linetype for this layer
	short	LineWeight;				// Layer's Line Weight                      (*2000*)
	BOOL	PlotFlag;				// TRUE=Plot this layer                     (*2000*)
	OBJHANDLE PlotStyleObjhandle;	// handle of PlotStyleName object           (*2000*)
} LAYER, *PLAYER;

typedef struct tag_LTYPE {
	OBJHANDLE Objhandle;			// Handle
	char	Name[MAX_STRLEN];		// Line Type Name
	char	StandardFlags;			// Standard flags
	char	DescriptiveText[512];	// Descriptive Text
	short	ElementsNumber;			// Line Elements Number
	double	Elements[30];			// Line Elements (Max=30)
	double	PatternLength;          // Length of linetype pattern
} LTYPE, *PLTYPE;

typedef struct tag_STYLE {
	OBJHANDLE Objhandle;			// Handle
	char	Name[MAX_STRLEN];		// Style Name
	char	StandardFlags;			// Standard flag values -> 4=Vertical text
	char	TextGenerationFlags;	// Text generation flags -> 2=Text is backward  4=Text is upside down
	char	PrimaryFontFilename[256];// Primary Font Filename
	char	BigFontFilename[256];	// Big font filename
	double	ObliqueAngle;			// Oblique angle
	double	FixedTextHeight;		// Fixed text height
	double	WidthFactor;			// Width Factor
	double	LastHeightUsed;			// Height
} STYLE, *PSTYLE;


// Entity Structures *******************************************
typedef struct tag_ENTLINE {
	REALPOINT	Point0;
	REALPOINT	Point1;
} ENTLINE, *PENTLINE;

typedef struct tag_ENTARC {
	REALPOINT	Point0;
	double		Radius;
	double		StartAngle;
	double		EndAngle;
} ENTARC, *PENTARC;

typedef struct tag_ENTCIRCLE {
	REALPOINT	Point0;
	double		Radius;
} ENTCIRCLE, *PENTCIRCLE;

typedef struct tag_ENTDIMENSION {
	REALPOINT	DefPoint3;
	REALPOINT	DefPoint4;
	double		DimRotationAngle;
	REALPOINT	DimLineDefPoint;
	OBJHANDLE	BlockheaderObjhandle;
	OBJHANDLE	DimStyleObjhandle;
	char		DimText[1024];
} ENTDIMENSION, *PENTDIMENSION;

typedef struct tag_ENTINSERT {
	REALPOINT	Point0;
	double		XScale;
	double		YScale;
	double		ZScale;
	double		RotationAngle;
	OBJHANDLE	BlockHeaderObjhandle;
} ENTINSERT, *PENTINSERT;

typedef struct tag_ENTPOINT {
	REALPOINT	Point0;
} ENTPOINT, *PENTPOINT;

typedef struct tag_ENTSOLID {
	REALPOINT	Point0;
	REALPOINT	Point1;
	REALPOINT	Point2;
	REALPOINT	Point3;
} ENTSOLID, *PENTSOLID;

typedef struct tag_TEXTDATA {
	double		Height;
	double		RotationAngle;
	double		WidthFactor;
	double		Oblique;
	char		GenerationFlag;
	short		Justification;
	REALPOINT	SecondAlignmentPoint;
	OBJHANDLE	TextStyleObjhandle;
} _TEXTDATA, *_PTEXTDATA;

typedef struct tag_ENTTEXT {
	REALPOINT	Point0;
	_TEXTDATA	TextData;
	char		strText[2048];
} ENTTEXT, *PENTTEXT;

typedef struct tag_ENTVERTEX {
	REALPOINT			Point;
	unsigned short	Flag;
	double			StartWidth;
	double			EndWidth;
	double			Bulge;
	double			TangentDir;
} ENTVERTEX, *PENTVERTEX;

//--------------this code is implemented by Tran duy Dung 2003/10/20
typedef struct tag_ENTPOLYLINE {
	PENTVERTEX		pVertex;
	UINT			nVertex;
	unsigned short	Flag;
} ENTPOLYLINE, *PENTPOLYLINE;

typedef struct tag_ENTELLIPSE {
	REALPOINT		CenterPoint;
	REALPOINT		MajorAxisEndPoint;
	double		MinorToMajorRatio;
	double		StartParam;
	double		EndParam;
} ENTELLIPSE, *PENTELLIPSE;



//**************************************************************
typedef struct tag_ENTITYHEADER
{
	unsigned short	EntityType;			// Entity type
	OBJHANDLE	Objhandle;				// Handle 
	OBJHANDLE	LayerObjhandle;			// Used Layer's Handle
	OBJHANDLE	LTypeObjhandle;			// Used LineType's Handle
	BOOL		Deleted;				// 0 = Not Deleted
	short		Color;					// Color (0=BYBLOCK, 256=BYLAYER, negative=layer is turned off)
	double		Thickness;				// Thickness (default=0)
	double		LineTypeScale;			// Linetype scale (default=1.0)
	double		ExtrusionDirection[3];	// Extrusion direction. (default = 0, 0, 1)
	short		LineWeight;				// Lineweight enum value (*2000*)
	DWORD		PreviousEntityPos;
	DWORD		NextEntityPos;
} ENTITYHEADER, *PENTITYHEADER;

typedef struct tag_ENTITIES
{
	HGLOBAL		hEntities;
	PBYTE       pEntityHeap;
	DWORD		TotalSize;
	DWORD		FreePos;
	DWORD		LastEntityPos;
	DWORD		CurrentEntityPos;
	DWORD		EntitiesNumber;
} ENTITIES, *PENTITIES;

// Blocks Structures *******************************************
typedef struct tag_BLOCKHEADER
{
	OBJHANDLE	Objhandle;				// Handle
	char		Name[MAX_STRLEN];		// Block name
	char		Flags;					// Block-type flags
	REALPOINT	BasePoint;				// Base point
	OBJHANDLE	LayerObjhandle;			// Layer's Handle
	ENTITIES	Entities;
} BLOCKHEADER, *PBLOCKHEADER;

// View Structure **********************************************
typedef struct tag_VIEW
{
	BOOL	Viewable;		// TRUE = Drawing can be viewed
	int		WindowLeft;		// Drawing Window Boundary Properties (In Pixels)
	int		WindowTop;		// Drawing Window Boundary Properties (In Pixels)
	int		WindowRight;	// Drawing Window Boundary Properties (In Pixels)
	int		WindowBottom;	// Drawing Window Boundary Properties (In Pixels)
	double	ViewLeft;		// Drawing View Properties (In Units)
	double	ViewBottom;		// Drawing View Properties (In Units)
	double	ViewRight;		// Drawing View Properties (In Units)
	double	ViewTop;		// Drawing View Properties (In Units)
	double	PPU;			// Pixels Per Unit
	double	ZoomLevel;		// Zoom Level
} VIEW, *PVIEW;



#if !defined(ON_OS_WINDOWS)
#define ON_OS_WINDOWS
#endif

#if defined(ON_OS_WINDOWS) && defined(ON_COMPILER_MSC)

#if defined(_M_X64) || defined(_M_IX86)
#if !defined(ON_LITTLE_ENDIAN)
#define ON_LITTLE_ENDIAN
#endif
#endif

#endif


#if defined(PI)
#define DXF_PI           PI
#else
#define DXF_PI           3.141592653589793238462643
#endif

#define DXF_DEGREES_TO_RADIANS DXF_PI/180.0
#define DXF_RADIANS_TO_DEGREES 180.0/DXF_PI

#define DXF_SQRT2          1.414213562373095048801689
#define DXF_SQRT3          1.732050807568877293527446
#define DXF_SQRT3_OVER_2   0.8660254037844386467637230
#define DXF_1_OVER_SQRT2   0.7071067811865475244008445
#define DXF_SIN_PI_OVER_12 0.2588190451025207623488990
#define DXF_COS_PI_OVER_12 0.9659258262890682867497433

#define DXF_LOG2         0.6931471805599453094172321
#define DXF_LOG10        2.302585092994045684017991

#define DXF_ArrayCount(a) (sizeof(a)/sizeof((a)[0]))

#if defined(DBL_MAX)
#define DXF_DBL_MAX DBL_MAX
#else
#define DXF_DBL_MAX 1.7976931348623158e+308
#endif

#if defined(DBL_MIN)
#define DXF_DBL_MIN DBL_MIN
#else
#define DXF_DBL_MIN 2.22507385850720200e-308
#endif

#if defined(DBL_EPSILON)
#define DXF_EPSILON DBL_EPSILON
#else
#define DXF_EPSILON 2.2204460492503131e-16
#endif
#define DXF_SQRT_EPSILON 1.490116119385000000e-8

/*
// In cases where lazy evaluation of a double value is
// performed, b-rep tolerances being a notable example,
// this value is used to indicate the value has not been
// computed.  This value must be < -1.0e308. and > -ON_DBL_MAX
//
// The reasons ON_UNSET_VALUE is a valid finite number are:
//
//   1) It needs to round trip through fprintf/sscanf.
//   2) It needs to persist unchanged through assigment
/       and not generate exceptions when assigned.
//   3) ON_UNSET_VALUE == ON_UNSET_VALUE needs to be true.
//   4) ON_UNSET_VALUE != ON_UNSET_VALUE needs to be false.
//
// Ideally, it would also have these SNaN attributes
//   * When used in a calculation, a floating point exception
//     occures.
//   * No possibility of a valid calculation would generate
//     ON_UNSET_VALUE.
//   * float f = (float)ON_UNSET_VALUE would create an invalid
//     float and generate an exception.
*/
#define DXF_UNSET_VALUE -1.23432101234321e+308

/*
// ON_UNSET_FLOAT is used to indicate a texture coordinate
// value cannot be calculated or is not well defined.
// In hindsight, this value should have been ON_FLT_QNAN
// because many calculation convert float texture coordinates
// to doubles and the "unset"ness attribute is lost.
*/
#define DXF_UNSET_FLOAT -1.234321e+38f

/*
// In rare cases when an absolute "zero" tolerance is
// required, ON_ZERO_TOLERANCE is used to compare
// numbers.  This number should be no smaller than
// ON_EPSILON and should be several orders of
// magnitude smaller than ON_SQRT_EPSILON
//
*/
#define DXF_ZERO_TOLERANCE 1.0e-12

/* default value for angle tolerances = 1 degree */
#define DXF_DEFAULT_ANGLE_TOLERANCE (DXF_PI/180.0)
#define DXF_MINIMUM_ANGLE_TOLERANCE (DXF_DEFAULT_ANGLE_TOLERANCE/10.0)


// pair of integer indices.  This
// is intentionally a struct/typedef
// rather than a class so that it
// can be used in other structs.
struct tagDXF_2dex
{
	int i;
	int j;
};

typedef struct tagDXF_2dex DXF_2dex;

// triplet of integer indices.  This
// is intentionally a struct/typedef
// rather than a class so that it
// can be used in other structs.
struct tagDXF_3dex
{
	int i;
	int j;
	int k;
};

typedef struct tagDXF_3dex DXF_3dex;

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <float.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>

//#include <WinBase.h>
// DXF enums
class  CDXF
{
public:
	//// unit_system ///////////////////////////////////////////////////////////////
	enum unit_system
	{
		// The constant enum values are saved in 3dm files 
		// and must never be changed.  The values > 11 were
		// added 5 April 2006.
		no_unit_system = 0,

		// atomic distances
		angstroms = 12,  // 1.0e-10 meters

		// SI units
		nanometers = 13,  // 1.0e-9 meters
		microns = 1,  // 1.0e-6 meters
		millimeters = 2,  // 1.0e-3 meters
		centimeters = 3,  // 1.0e-2 meters
		decimeters = 14,  // 1.0e-1 meters
		meters = 4,
		dekameters = 15,  // 1.0e+1 meters
		hectometers = 16,  // 1.0e+2 meters
		kilometers = 5,  // 1.0e+3 meters
		megameters = 17,  // 1.0e+6 meters
		gigameters = 18,  // 1.0e+9 meters

		// english distances
		microinches = 6,  //    2.54e-8 meters (1.0e-6 inches)
		mils = 7,  //    2.54e-5 meters (0.001 inches)
		inches = 8,  //    0.0254  meters
		feet = 9,  //    0.3408  meters (12 inches)
		yards = 19,  //    0.9144  meters (36 inches)
		miles = 10,  // 1609.344   meters (5280 feet)

		// printer distances
		printer_point = 20,  // 1/72 inches (computer points)
		printer_pica = 21,  // 1/6 inches  (computer picas)

		// terrestrial distances
		nautical_mile = 22, // 1852 meters 
		//    Approximately 1 minute of arc on a terrestrial great circle.
		//    See http://en.wikipedia.org/wiki/Nautical_mile.

		// astronomical distances
		astronomical = 23, // 1.4959787e+11 // http://en.wikipedia.org/wiki/Astronomical_unit
		// 1.495979e+11  // http://units.nist.gov/Pubs/SP811/appenB9.htm  
		//    An astronomical unit (au) is the mean distance from the 
		//    center of the earth to the center of the sun.
		lightyears = 24, // 9.4607304725808e+15 // http://en.wikipedia.org/wiki/Light_year
		// 9.46073e+15 meters  // http://units.nist.gov/Pubs/SP811/appenB9.htm
		//    A light year is the distance light travels in one Julian year.
		//    The speed of light is exactly 299792458 meters/second.
		//    A Julian year is exactly 365.25 * 86400 seconds and is 
		//    approximately the time it takes for one earth orbit.
		parsecs = 25, // 3.08567758e+16  // http://en.wikipedia.org/wiki/Parsec
		// 3.085678e+16    // http://units.nist.gov/Pubs/SP811/appenB9.htm  

		// Custom unit systems
		custom_unit_system = 11 // x meters with x defined in ON_3dmUnitsAndTolerances.m_custom_unit_scale
	};

	static unit_system UnitSystem(int); // convert integer to unit_system enum

	//// point_style ///////////////////////////////////////////////////////////////
	enum point_style
	{
		unknown_point_style = 0,
		not_rational = 1,
		homogeneous_rational = 2,
		euclidean_rational = 3,
		intrinsic_point_style = 4, // point format used in definition
		point_style_count = 5
	};

	static point_style PointStyle(int); // convert integer to point_style enum

	//// knot_style ///////////////////////////////////////////////////////////////
	enum knot_style // if a knot vector meets the conditions of two styles,
	{               // then the style with the lowest value is used
		unknown_knot_style = 0, // unknown knot style
		uniform_knots = 1, // uniform knots (ends not clamped)
		quasi_uniform_knots = 2, // uniform knots (clamped ends, degree >= 2)
		piecewise_bezier_knots = 3, // all internal knots have full multiplicity
		clamped_end_knots = 4, // clamped end knots (with at least 1 interior non-uniform knot)
		non_uniform_knots = 5, // known to be none of the above
		knot_style_count = 6
	};

	static knot_style KnotStyle(int); // convert integer to knot_style enum


	//// curve_style ///////////////////////////////////////////////////////////////
	enum curve_style
	{
		unknown_curve_style = 0,
		line = 1,
		circle = 2,
		ellipse = 3, // with distinct foci (not a circle)
		parabola = 4,
		hyperbola = 5,
		planar_polyline = 6, // not a line segment
		polyline = 7, // non-planar polyline
		planar_freeform_curve = 8, // planar but none of the above
		freeform_curve = 9, // known to be none of the above
		curve_style_count = 10
	};

	static curve_style CurveStyle(int); // convert integer to curve_style enum

	//// sort_algorithm ///////////////////////////////////////////////////////////////
	enum sort_algorithm
	{
		heap_sort = 0,
		quick_sort = 1
	};

	static sort_algorithm SortAlgorithm(int); // convert integer to sort_method enum

	//// endian-ness ///////////////////////////////////////////////////////////////
	enum endian {
		little_endian = 0, // least significant byte first or reverse byte order - Intel x86, ...
		big_endian = 1  // most significant byte first - Motorola, Sparc, MIPS, ...
	};

	static endian Endian(int); // convert integer to endian enum
	static endian Endian();    // returns endian-ness of current CPU

	//// view coordinates ///////////////////////////////////////////////////////////

	enum coordinate_system
	{
		world_cs = 0,
		camera_cs = 1,
		clip_cs = 2,
		screen_cs = 3
	};

	static coordinate_system CoordinateSystem(int); // convert integer to coordinate_system enum

	//// exception types ///////////////////////////////////////////////////////////
	enum exception_type
	{
		unknown_exception = 0,
		out_of_memory,
		corrupt_object,               // invalid object encountered - continuing would crash or
		// result in corrupt object being saved in archive.
		unable_to_write_archive,      // write operation failed - out of file space/read only mode/...?
		unable_to_read_archive,       // read operation failed - truncated archive/locked file/... ?
		unable_to_seek_archive,       // seek operation failed - locked file/size out of bounds/... ?
		unexpected_end_of_archive,    // truncated archive
		unexpected_value_in_archive   // corrupt archive?
	};
	static exception_type ExceptionType(int); // convert integer to exception_type enum


	//// curvature style /////////////////////////////////////////////////////////
	enum curvature_style
	{
		unknown_curvature_style = 0,
		gaussian_curvature = 1,
		mean_curvature = 2, // unsigned mean curvature
		min_curvature = 3, // minimum unsigned radius of curvature
		max_curvature = 4, // maximum unsigned radius of curvature
		curvature_style_count = 5
	};
	static curvature_style CurvatureStyle(int); // convert integer to curvature_style enum

	//// view display mode /////////////////////////////////////////////////////////////
	enum display_mode
	{
		default_display = 0, // default display
		wireframe_display = 1, // wireframe display
		shaded_display = 2, // shaded display
		renderpreview_display = 3  // render preview display
	};
	static display_mode DisplayMode(int); // convert integer to display_mode enum


	//// bitmap_type ///////////////////////////////////////////////////
	enum bitmap_type
	{
		unknown_bitmap_type = 0,
		windows_bitmap = 1,     // BITMAPINFO style
		opengl_bitmap = 2,      // unpacked OpenGL RGB or RGBA
		png_bitmap = 3
	};
	static bitmap_type BitmapType(int); // convert integer to bitmap_type enum

	//// Types of Curves ///////////////////////////////////////////////////////////
	enum eCurveType
	{
		ctCurve, // nothing
		ctArc,
		ctCircle,
		ctLine,
		ctNurbs,
		ctOnsurface,
		ctProxy,
		ctPolycurve,
		ctPolyline,
	};
};


#define DXF_ERROR(msg) DXF_Error(__FILE__,__LINE__,msg)

#if defined (cplusplus) || defined(_cplusplus) || defined(__cplusplus) 
extern "C"
{

	void __cdecl DXF_ErrorMessage(
		int message_type, // 0=warning - serious problem that code is designed to handle
		// 1=error - serious problem code will attempt to handle The thing causing the error is a bug that must be fixed.
		// 2=assert failed - crash is nearly certain
		const char* sErrorMessage
		);



	void __cdecl DXF_Error(const char* sFileName, int line_number,
		const char* sFormat, ...);

}

#endif

#endif // DXFDEF_H
