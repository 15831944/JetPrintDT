#ifndef __DXF_NURBSKNOT_H
#define __DXF_NURBSKNOT_H

#include "JetDxfDef.h"
#include "DXFPoint.h"

double DXFDomainTolerance(
	double, // start of domain
	double  // end of domain
	);

double DXFKnotTolerance(
int,           // order (>=2)
int,           // cv count
const double*, // knot[] array
int            // knot index
);

double DXFSpanTolerance(
int,           // order (>=2)
int,           // cv count
const double*, // knot[] array
int            // span index
);

int DXFKnotCount( // returns (order + cv_count - 2)
int, // order (>=2)
int  // cv_count (>=order)
);

int DXFKnotMultiplicity(
	int,   // order (>=2)
	int,   // cv_count (>=order)
	const double*, // knot[]
	int            // knot_index
	);

int DXFKnotVectorSpanCount(
	int,           // order (>=2)
	int,           // cv count
	const double*  // knot[] array
	);

bool DXFGetKnotVectorSpanVector(
	int,           // order (>=2)
	int,           // cv count
	const double*, // knot[] array
	double*        // s[] array
	);

/*
Description:
Given an evaluation parameter t in the domain of a NURBS curve, DXFNurbsSpanIndex(order,cv_count,knot,t,0,0) returns the integer
i such that (knot[i],...,knot[i+2*degree-1]),  and (cv[i],...,cv[i+degree]) are the knots and control points that
define the span of the NURBS that are used for evaluation at t.
Parameters:
order - [in] order >= 2
cv_count - [in] cv_count >= order
knot - [in] valid knot vector
t - [in] evaluation parameter
side - [in] determines which span is used when t is at a knot
value;  side = 0 for the default (from above),
side = -1 means from below, and
side = +1 means from above.
hint - [in] Search hint, or 0 if not hint is available.
Returns:
Returns the index described above.
*/
int DXFNurbsSpanIndex(
	int order,
	int cv_count,
	const double* knot,
	double t,
	int side,
	int hint
	);

int DXFNextNurbsSpanIndex(
	// returns  0: input span_index < 0
	//           cv_count-order: input span_index = cv_count-order
	//         -1: input span_index > cv_count-order;
	//     otherwise next span index
	int order,
	int cv_count,
	const double* knot,
	int            // current span_index 
	);

int DXFGetSpanIndices( // returns span count, which is one less than length of span_indices[]
	int order,
	int cv_count,
	const double* knot,
	int*           // span_indices[cv_count-order+2]. 
	//Indices of knots at end of group of mult knots 
	//at start of span, and knot at start of group of mult knots
	//at end of spline.
	);

double DXFSuperfluousKnot(
	int order,
	int cv_count,
	const double* knot,
	int            // 0 = first superfluous knot 
	// 1 = last superfluous knot
	);

bool DXFIsKnotVectorPeriodic(
	int order,
	int cv_count,
	const double* knot
	);

bool DXFIsKnotVectorClamped(
	int order,
	int cv_count,
	const double* knot,
	int = 2        // 0 = check left end, 1 = check right end, 2 = check both
	);

bool DXFIsKnotVectorUniform(
	int order,
	int cv_count,
	const double* knot
	);

//////////
// returns true if all knots have multiplicity = degree
bool DXFKnotVectorHasBezierSpans(
	int order,
	int cv_count,
	const double* knot
	);


CDXF::knot_style ON_KnotVectorStyle(
	int order,
	int cv_count,
	const double* knot
	);

/*
Description:
Set the domain of a knot vector.
Parameters:
order - [in] order >= 2
cv_count - [in] cv_count >= order
knot - [in/out] input existing knots and returns knots with new domain.
t0 - [in]
t1 - [in] New domain will be the interval (t0,t1).
Returns:
True if input is valid and the returned knot vector has the requested domain.  False if the input is
invalid, in which case the input knot vector is not changed.
*/
bool DXFSetKnotVectorDomain(
	int order,
	int cv_count,
	double* knot,
	double t0,
	double t1
	);

bool DXFGetKnotVectorDomain(
	int,           // order (>=2)
	int,           // cv count
	const double*, // knot[] array
	double*, double*
	);

bool DXFReverseKnotVector(
	int,           // order (>=2)
	int,           // cv count
	double*        // knot[] array
	);

bool DXFClampKnotVector(
	// Sets inital/final order-2 knots to values in
	// knot[order-2]/knot[cv_count-1].
	int,           // order (>=2)
	int,           // cv count
	double*,       // knot[] array
	int            // 0 = clamp left end, 1 = right end, 2 = clamp both ends
	);

bool DXFClampKnotVector(
	int,       // cv_dim ( = dim+1 for rational cvs )
	int,       // order (>=2)
	int,       // cv_count,
	int,       // cv_stride, 
	double*,   // cv[] NULL or array of order many cvs
	double*,   // knot[] array with room for at least knot_multiplicity new knots
	int        // end  0 = clamp start, 1 = clamp end, 2 = clamp both ends
	);


/*
Description:
Fill in knot values for a clamped uniform knot
vector.
Parameters:
order - [in] (>=2) order (degree+1) of the NURBS
cv_count - [in] (>=order) total number of control points
in the NURBS.
knot - [in/out] Input is an array with room for DXFKnotCount(order,cv_count) doubles.  
Output is a clamped uniform knot vector with domain (0, (1+cv_count-order)*delta).
delta - [in] (>0, default=1.0) spacing between knots.
Returns:
true if successful
*/
bool DXFMakeClampedUniformKnotVector(
	int order,
	int cv_count,
	double* knot,
	double delta = 1.0
	);

/*
Description:
Fill in knot values for a clamped uniform knot
vector.
Parameters:
order - [in] (>=2) order (degree+1) of the NURBS
cv_count - [in] (>=order) total number of control points
in the NURBS.
knot - [in/out] Input is an array with room for
ON_KnotCount(order,cv_count) doubles.  Output is
a periodic uniform knot vector with domain
(0, (1+cv_count-order)*delta).
delta - [in] (>0, default=1.0) spacing between knots.
Returns:
true if successful
See Also:
ON_NurbsCurve::MakePeriodicUniformKnotVector
*/
bool DXFMakePeriodicUniformKnotVector(
	int order,
	int cv_count,
	double* knot,
	double delta = 1.0
	);

double DXFGrevilleAbcissa( // get Greville abcissae from knots
	int,           // order (>=2)
	const double*  // knot[] array (length = order-1)
	);

/*
Returns:
Number of knots added.
*/
int DXFInsertKnot(
	double,    // knot_value,
	int,       // knot_multiplicity, (1 to order-1 including multiplicity of any existing knots)
	int,       // cv_dim ( = dim+1 for rational cvs )
	int,       // order (>=2)
	int,       // cv_count,
	int,       // cv_stride (>=cv_dim)
	double*,   // cv[]  NULL or cv array with room for at least knot_multiplicity new cvs
	double*,   // knot[] knot array with room for at least knot_multiplicity new knots
	int*       // hint, optional hint about where to search for span to add knots to
	// pass NULL if no hint is available
	);

#endif //__DXF_KNOT_H