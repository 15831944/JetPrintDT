
#ifndef __DXF_EVALUTE_NURBS_H
#define __DXF_EVALUTE_NURBS_H

#include "JetDXFDef.h"
#include "DXFMath.h"


bool DXFRemoveBezierSingAt0( // input bezier is rational with 0/0 at start
	int,    // dimension 
	int,    // order (>=2)
	int,    // cv_stride (>=dim+1)
	double* // cv[order*cv_stride] array
	);


bool DXFEvaluateBezier(
int,            // dimension
bool,           // true if Bezier is rational
int,            // order (>=2)
int,            // cv_stride >= (is_rat)?dim+1:dim
const double*,  // cv[order*cv_stride] array
double, double, // t0,t1 = domain of bezier
int,            // number of derivatives to compute (>=0)
double,         // evaluation parameter
int,            // v_stride (>=dimension)
double*         // v[(der_count+1)*v_stride] array
);

bool DXFEvaluateNurbsBasis(
int,           // order (>=1)
const double*, // knot[] array of 2*(order-1) knots
double,        // evaluation parameter
double*        // basis_values[] array of length order*order
);

bool DXFEvaluateNurbsBasisDerivatives(
int,           // order (>=1)
const double*, // knot[] array of 2*(order-1) knots
int,           // number of derivatives
double*        // basis_values[] array of length order*order
);




/*
int dim,             // dimension
bool is_rat,         // true if NURBS is rational
int order,           // order
const double* knot,  // knot[] array of (2*order-2) doubles
int cv_stride,       // cv_stride >= (is_rat)?dim+1:dim
const double* cv,    // cv[order*cv_stride] array
int der_count,       // number of derivatives to compute
double t,            // evaluation parameter
int v_stride,        // v_stride (>=dimension)
double* v            // v[(der_count+1)*v_stride] array
)
int,           // dimension
bool,          // true if NURBS is rational
int,           // order
const double*, // knot[] array of (2*order-2) doubles
int,           // cv_stride
const double*, // cv[] array of order*cv_stride  doubles
int,           // number of derivatives to compute (>=0)
double,        // evaluation parameter
int,           // answer_stride (>=dimension)
double*        // answer[] array of length (ndir+1)*answer_stride
*/


/*
Description:
Evaluate a NURBS curve span.
Parameters:
dim - [in]
dimension (> 0).
is_rat - [in]
true or false.
order - [in]
order=degree+1 (order>=2)
knot - [in] NURBS knot vector.
NURBS knot vector with 2*(order-1) knots, knot[order-2] != knot[order-1]
cv_stride - [in]
cv - [in]
For 0 <= i < order the i-th control vertex is

cv[n],...,cv[n+(is_rat?dim:dim+1)],

where n = i*cv_stride.  If is_rat is true the cv is
in homogeneous form.
der_count - [in]
number of derivatives to evaluate (>=0)
t - [in]
evaluation parameter
v_stride - [in]
v - [out]
An array of length v_stride*(der_count+1). The evaluation
results are returned in this array.

P = v[0],...,v[m_dim-1]
Dt = v[v_stride],...
Dtt = v[2*v_stride],...
...

In general, Dt^i returned in v[n],...,v[n+m_dim-1], where

n = v_stride*i.

Returns:
True if successful.
See Also:
ON_NurbsCurve::Evaluate
ON_EvaluateNurbsSurfaceSpan
ON_EvaluateNurbsCageSpan
*/
bool DXFEvaluateNurbsSpan(
int dim,
bool is_rat,
int order,
const double* knot,
int cv_stride,
const double* cv,
int der_count,
double t,
int v_stride,
double* v
);

bool DXFEvaluateNurbsDeBoor( // for expert users only - no support available
	int,            // cv_dim ( dim+1 for rational cvs )
	int,            // order (>=2)
	int,            // cv_stride (>=cv_dim)
	double*,        // cv array - values changed to result of applying De Boor's algorithm
	const double*,  // knot array
	int,            // side,
	//    -1  return left side of B-spline span in cv array
	//    +1  return right side of B-spline span in cv array
	//    -2  return left side of B-spline span in cv array
	//        Ignore values of knots[0,...,order-3] and assume
	//        left end of span has a fully multiple knot with
	//        value "mult_k".
	//    +2  return right side of B-spline span in cv array
	//        Ignore values of knots[order,...,2*order-2] and
	//        assume right end of span has a fully multiple
	//        knot with value "mult_k".
	double,         // mult_k - used when side is +2 or -2.  See above for usage.
	double          // t
	//    If side < 0, then the cv's for the portion of the NURB span to
	//    the LEFT of t are computed.  If side > 0, then the cv's for the
	//    portion the span to the RIGHT of t are computed.  The following
	//    table summarizes the restrictions on t:
	//
	//     value of side         condition t must satisfy
	//        -2                    mult_k < t and mult_k < knots[order-1]
	//        -1                    knots[order-2] < t
	//        +1                    t < knots[order-1]
	//        +2                    t < mult_k and knots[order-2] < mult_k
	);

#endif
