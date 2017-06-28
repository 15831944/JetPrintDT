#include "DXFNurbsKnot.h"
#include  "DXFMath.h"
#include "DXFEvaluteNurbs.h"

double DXFDomainTolerance(double a, double b)
{
	if (a == b)
		return 0.0;
	double tol = (fabs(a) + fabs(b) + fabs(a - b))* DXF_SQRT_EPSILON;
	if (tol <  DXF_EPSILON)
		tol = DXF_EPSILON;
	return tol;
}

/////////////////////////////////////////////////////////////////
//
// Computes tolerance associated with knot[i]
//

double DXFKnotTolerance(int order, int cv_count, const double* knot,
	int knot_index)
{
	const int knot_count = DXFKnotCount(order, cv_count);
	int i0, i1, j;
	double a, b, tol;
	i0 = knot_index - order + 1;
	if (i0 < 0)
		i0 = 0;
	i1 = knot_index + order - 1;
	if (i1 >= knot_count)
		i1 = knot_count - 1;
	for (j = knot_index; j > i0; j--) {
		if (knot[j] != knot[knot_index])
			break;
	}
	a = fabs(knot[knot_index] - knot[j]);
	for (j = knot_index; j < i1; j++) {
		if (knot[j] != knot[knot_index])
			break;
	}
	b = fabs(knot[knot_index] - knot[j]);
	tol = (a == 0.0 && b == 0.0) ? 0.0 : (a + b + fabs(knot[knot_index]))* DXF_SQRT_EPSILON;
	return tol;
}

/////////////////////////////////////////////////////////////////
//
// Computes tolerance associated with span of a knot vector
//

double DXFSpanTolerance(int order, int cv_count, const double* knot, int span_index)
{
	const int i0 = span_index + order - 2;
	return DXFDomainTolerance(knot[i0], knot[i0 + 1]);
}

/////////////////////////////////////////////////////////////////
//
// Computes number of knots in knot vector
//

int DXFKnotCount(int order, int cv_count)
{
	return (order + cv_count - 2);
}

/////////////////////////////////////////////////////////////////
//
// Computes number of knots in knot vector
//

int DXFKnotMultiplicity(
	int order,          // order (>=2)
	int cv_count,       // cv_count (>=order)
	const double* knot, // knot[]
	int knot_index      // knot_index
	)
{
	int knot_count = order + cv_count - 2;
	int km = 0;
	if (knot && knot_index >= 0 && knot_index < knot_count) {
		while (knot_index > 0 && knot[knot_index] == knot[knot_index - 1])
			knot_index--;
		knot += knot_index;
		knot_count -= knot_index;
		km = 1;
		while (km < knot_count && knot[0] == knot[km])
			km++;
	}
	return km;
}

/////////////////////////////////////////////////////////////////
//
// Computes number of non-empty spans
//

int DXFKnotVectorSpanCount(
	int order,         // order (>=2)
	int cv_count,      // cv count
	const double* knot // knot[] array
	)
{
	if (0 == knot)
	{
		if (0 != order || 0 != cv_count)
		{
			DXF_ERROR("NULL knot[] passed to DXFKnotVectorSpanCount.");
		}
		return 0;
	}
	int i, span_count = 0;
	for (i = order - 1; i < cv_count; i++) {
		if (knot[i] > knot[i - 1])
			span_count++;
	}
	return span_count;
}

/////////////////////////////////////////////////////////////////
//
// Gets span vector from knot vector
//

bool DXFGetKnotVectorSpanVector(
	int order,          // order (>=2)
	int cv_count,       // cv count
	const double* knot, // knot[] array
	double* s           // s[] array
	)
{
	if (0 == knot || 0 == s)
	{
		if (0 != order || 0 != cv_count)
		{
			DXF_ERROR("NULL knot[] or s[] passed to DXFKnotVectorSpanCount.");
			return false;
		}
		return true;
	}

	int i, span_count = 0;
	s[span_count++] = knot[order - 2];
	for (i = order - 1; i < cv_count; i++) {
		if (knot[i] > knot[i - 1])
			s[span_count++] = knot[i];
	}
	return (span_count>1) ? true : false;
}


/////////////////////////////////////////////////////////////////
//
// Computes span index for evaluation of parameter
//

int DXFNurbsSpanIndex(
	int order,          // (>=2)
	int cv_count,
	const double* knot, // knot[] array or length DXFKnotCount(order,cv_count)
	double t,           // evaluation parameter
	int side,           // side 0 = default, -1 = from below, +1 = from above
	int hint            // hint (or 0 if no hint available)
	)
{
	int j, len;

	// shift knot so that domain is knot[0] to knot[len]
	knot += (order - 2);
	len = cv_count - order + 2;

	// see if hint helps 
	if (hint > 0 && hint < len - 1) {
		while (hint > 0 && knot[hint - 1] == knot[hint]) hint--;
		if (hint > 0) {
			// have knot[hint-1] < knot[hint]
			if (t < knot[hint]) {
				len = hint + 1;
				hint = 0;
			}
			else {
				if (side < 0 && t == knot[hint])
					hint--;
				knot += hint;
				len -= hint;
			}
		}
	}
	else
		hint = 0;

	j = DXFSearchMonotoneArray(knot, len, t);
	if (j < 0)
		j = 0;
	else if (j >= len - 1)
		j = len - 2;
	else if (side < 0) {
		// if user wants limit from below and t = an internal knot,
		// back up to prevous span
		while (j > 0 && t == knot[j])
			j--;
	}
	return (j + hint);
}


int DXFNextNurbsSpanIndex(int order, int cv_count, const double* knot, int span_index)

/*
Get index of next non-degenerate NURBS span

INPUT:
order, cv_count, knot
knot vector
span_index
current span index ( >= 0 and <= cv_count-order )
OUTPUT:
i = DXFNextNurbsSpanIndex()
i>=0: successful - the index of the next span or
cv_count-order if the input value
was cv_count-or
knot[i+order-2] < knot[i+order-1]
<0: failure

COMMENTS:
The first span in a NURBS has span_index = 0.  The last span in a NURBS
has span_index = cv_count-order.
A span of a degree d NURBS is defined by d+1 CVs and 2*d knots.  For a
given span_index, the associated knots and CVs are
{knot[span_index], ..., knot[span_index+2*d-1]}
and
{CV[span_index], ..., CV[span_index + d]}
The domain of the span is
[ knot[span_index+order-2], knot[span_index+order-1] ].

EXAMPLE:
// print the values of all distinct knots in a NURBS's domain
int span_index = 0;
int next_span_index = 0;
for (;;) {
printf( "knot[%2d] = %g\n", span_index+order-2, knot[span_index+order-2] );
next_span_index =DXFNextNurbsSpanIndex( order, cv_count, knot, span_index );
if ( next_span_index < 0 )
break; // illegal input
if ( next_span_index == span_index ) {
// end of the domain
printf( "knot[%2d] = %g\n", cv_count-1, knot[cv_count-1] );
break;
}
next_span_index = span_index;
}

*/

{

	if (span_index < 0 || span_index > cv_count - order || !knot)
		return -1;

	if (span_index < cv_count - order) {
		do {
			span_index++;
		} while (span_index < cv_count - order &&
			knot[span_index + order - 2] == knot[span_index + order - 1]);
	}
	return span_index;
}


int DXFGetSpanIndices(int order,
	int cv_count,
	const double* knot,
	int* span_indices)

	/* span_indices should have size greater than the number of
	spans (cv_count is big enough).

	returns span count.
	fills in span_indices with index of last in each bunch of multiple knots at
	start of span, and first in buch at end of nurb.


	*/
{
	int span_index, next_span_index, j;

	span_index = -1;
	next_span_index = 0;
	j = 0;
	while (span_index != next_span_index) {
		span_index = next_span_index;
		span_indices[j] = span_index + order - 2;
		next_span_index = DXFNextNurbsSpanIndex(order, cv_count, knot, span_index);
		if (next_span_index < 0)
			return next_span_index;
		j++;
	}

	span_indices[j] = span_index + order - 1;

	return j;
}


/////////////////////////////////////////////////////////////////
//
// Computes value for superfluous knot used in systems like OpenGL and 3dsMax
//

double DXFSuperfluousKnot(
	int order, int cv_count, const double* knot,
	int end)
{
	double k;
	const int knot_count = order + cv_count - 2;
	// gets superfluous knot for translation to other formats
	k = knot[(end) ? knot_count - 1 : 0];
	if (order > 2 && cv_count >= 2 * order - 2 && cv_count >= 6) {
		// check for non-clamped knots
		if (end) {
			if (knot[cv_count - 1] < knot[knot_count - 1])
				k += (knot[order + 1] - knot[order]);
		}
		else {
			if (knot[0] < knot[order - 2])
				k -= (knot[cv_count - order + 1] - knot[cv_count - order]);
		}
	}
	return k;
}

/////////////////////////////////////////////////////////////////
//
// Used to determine when a knot vector is periodic
//

bool DXFIsKnotVectorPeriodic(
	int order,
	int cv_count,
	const double* knot
	)

{
	double tol;
	const double* k1;
	int i;

	if (order < 2 || cv_count < order || !knot) {
		DXF_ERROR("DXFIsKnotVectorPeriodic(): illegal input");
		return false;
	}

	if (order == 2)
		return false; // convention is that degree 1 curves cannot be periodic.

	if (order <= 4) {
		if (cv_count < order + 2)
			return false;
	}
	else if (cv_count < 2 * order - 2) {
		return false;
	}

	tol = fabs(knot[order - 1] - knot[order - 3])* DXF_SQRT_EPSILON;
	if (tol < fabs(knot[cv_count - 1] - knot[order - 2])* DXF_SQRT_EPSILON)
		tol = fabs(knot[cv_count - 1] - knot[order - 2])* DXF_SQRT_EPSILON;
	k1 = knot + cv_count - order + 1;
	i = 2 * (order - 2);
	while (i--) {
		if (fabs(knot[1] - knot[0] + k1[0] - k1[1]) > tol)
			return false;
		knot++; k1++;
	}
	return true;
}

/////////////////////////////////////////////////////////////////
//
// Used to determine when a knot vector is clamped
//

bool DXFIsKnotVectorClamped(
	int order,
	int cv_count,
	const double* knot,
	int end // (default = 2) 0 = left end, 1 = right end, 2 = both
	)

{
	if (order <= 1 || cv_count < order || !knot || end < 0 || end > 2)
		return false;
	bool rc = true;
	if ((end == 0 || end == 2) && knot[0] != knot[order - 2])
		rc = false;
	if ((end == 1 || end == 2) && knot[cv_count - 1] != knot[order + cv_count - 3])
		rc = false;
	return rc;
}

bool DXFIsKnotVectorUniform(
	int order,
	int cv_count,
	const double* knot
	)
{
	bool rc = (order >= 2 && cv_count >= order && 0 != knot);
	if (rc)
	{
		const double delta = knot[order - 1] - knot[order - 2];
		const double delta_tol = DXF_SQRT_EPSILON*delta;
		int i0, i1;
		double d;
		if (DXFIsKnotVectorClamped(order, cv_count, knot))
		{
			i0 = order;
			i1 = cv_count;
		}
		else
		{
			i0 = 1;
			i1 = DXFKnotCount(order, cv_count);
		}
		for (/*empty*/; i0 < i1 && rc; i0++)
		{
			d = knot[i0] - knot[i0 - 1];
			if (fabs(d - delta) > delta_tol)
				rc = false;
		}
	}
	return rc;
}

/////////////////////////////////////////////////////////////////
//
// Used to determine properties of knot vector
//

bool DXFKnotVectorHasBezierSpans(
	int order,          // order (>=2)
	int cv_count,       // cv count
	const double* knot  // knot[] array
	)
{
	int knot_count = DXFKnotCount(order, cv_count);
	if (knot_count < 2)
		return false;
	int span_count = DXFKnotVectorSpanCount(order, cv_count, knot);
	if (span_count < 1)
		return false;
	if (order >= 2 &&
		cv_count >= order &&
		knot_count == (span_count + 1)*(order - 1) &&
		knot[0] == knot[order - 2] && knot[cv_count - 1] == knot[knot_count - 1])
		return true;
	return false;
}

/////////////////////////////////////////////////////////////////
//
// Used to determine properties of knot vector
//

CDXF::knot_style DXFKnotVectorStyle(
	int order,
	int cv_count,
	const double* knot
	)
{
	CDXF::knot_style s = CDXF::unknown_knot_style;
	if (order >= 2 && cv_count >= order && knot && knot[order - 2] < knot[cv_count - 1]) {
		const int knot_count = order + cv_count - 2;
		const double delta = 0.5*((knot[order - 1] - knot[order - 2]) + (knot[cv_count - 1] - knot[cv_count - 2]));
		const double ktol = delta*1.0e-6;
		int i;
		if (DXFIsKnotVectorClamped(order, cv_count, knot)) {
			if (order == cv_count) {
				s = CDXF::piecewise_bezier_knots;
			}
			else {
				s = CDXF::clamped_end_knots;
				for (i = order - 1; i <= cv_count - 1; i++) {
					if (fabs(knot[i] - knot[i - 1] - delta) > ktol) {
						break;
					}
				}
				if (i >= cv_count) {
					s = CDXF::quasi_uniform_knots;
				}
				else {
					const int degree = order - 1;
					for (i = order - 1; i < cv_count - 1; i += degree) {
						if (knot[i] != knot[i + degree - 1])
							break;
					}
					if (i >= cv_count - 1)
						s = CDXF::piecewise_bezier_knots;
				}
			}
		}
		else {
			// check for uniform knots
			s = CDXF::non_uniform_knots;
			for (i = 1; i < knot_count; i++) {
				if (fabs(knot[i] - knot[i - 1] - delta) > ktol) {
					break;
				}
			}
			if (i >= knot_count)
				s = CDXF::uniform_knots;
		}
	}
	return s;
}


/////////////////////////////////////////////////////////////////
//
// Used to set the domain of a knot vector
//

bool DXFSetKnotVectorDomain(int order, int cv_count, double* knot, double t0, double t1)
{
	bool rc = false;
	if (order < 2 || cv_count < order || 0 == knot || t0 >= t1 || !DXFIsValid(t0) || !DXFIsValid(t1))
	{
		DXF_ERROR("DXFSetKnotVectorDomain - invalid input");
	}
	else if (knot[order - 2] >= knot[cv_count - 1]
		|| !DXFIsValid(knot[order - 2])
		|| !DXFIsValid(knot[cv_count - 2]))
	{
		DXF_ERROR("DXFSetKnotVectorDomain - invalid input knot vector");
	}
	else
	{
		const CDxfInterval oldd(knot[order - 2], knot[cv_count - 1]);
		const CDxfInterval newd(t0, t1);
		if (oldd != newd)
		{
			int i, knot_count = DXFKnotCount(order, cv_count);
			for (i = 0; i < knot_count; i++)
			{
				knot[i] = newd.ParameterAt(oldd.NormalizedParameterAt(knot[i]));
			}
		}
		rc = true;
	}
	return rc;
}


/////////////////////////////////////////////////////////////////
//
// Used to get the domain of a knot vector
//

bool DXFGetKnotVectorDomain(
	int order,
	int cv_count,
	const double* knot,
	double* k0, double* k1
	)
{
	if (order < 2 || cv_count < order || knot == NULL)
		return false;
	if (k0)
		*k0 = knot[order - 2];
	if (k1)
		*k1 = knot[cv_count - 1];
	return true;
}

/////////////////////////////////////////////////////////////////
//
// Used to reverse knot vectors
//

bool DXFReverseKnotVector(
	int order,
	int cv_count,
	double* knot
	)
{
	if (order < 2 || cv_count < order || knot == NULL)
		return false;
	const int knot_count = (order + cv_count - 2);
	double t;
	int i, j;
	for (i = 0, j = knot_count - 1; i <= j; i++, j--) {
		t = knot[i];
		knot[i] = -knot[j];
		knot[j] = -t;
	}
	return true;
}

bool DXFClampKnotVector(
	int order,          // order (>=2)
	int cv_count,       // cv count
	double* knot,       // knot[] array
	int end             // 0 = clamp start, 1 = clamp end, 2 = clamp both ends
	)
{
	// sets initial/final order-2 knot values to match knot[order-2]/knot[cv_count-1]
	bool rc = false;
	int i, i0;
	if (knot && order >= 2 && cv_count >= order) {
		if (end == 0 || end == 2) {
			i0 = order - 2;
			for (i = 0; i < i0; i++) {
				knot[i] = knot[i0];
			}
			rc = true;
		}
		if (end == 1 || end == 2) {
			const int knot_count = DXFKnotCount(order, cv_count);
			i0 = cv_count - 1;
			for (i = i0 + 1; i < knot_count; i++) {
				knot[i] = knot[i0];
			}
			rc = true;
		}
	}
	return rc;
}


bool DXFClampKnotVector(
	int cv_dim,   // dimension of cv's = ( = dim+1 for rational cvs )
	int order,
	int cv_count,
	int cv_stride,
	double* cv,   // NULL or cv array with room for at least knot_multiplicity new cvs
	double* knot, // knot array with room for at least knot_multiplicity new knots
	int end       // 0 = clamp start, 1 = clamp end, 2 = clamp both ends
	)
{
	// sets initial/final order-2 knot values to match knot[order-2]/knot[cv_count-1]
	bool rc = false;
	int i, i0;

	if (knot && order >= 2 && cv_count >= order) {
		if (end == 0 || end == 2) {
			if (cv) {
				DXFEvaluateNurbsDeBoor(cv_dim, order, cv_stride, cv, knot, 1, 0.0, knot[order - 2]);
			}
			i0 = order - 2;
			for (i = 0; i < i0; i++)
				knot[i] = knot[i0];
			rc = true;
		}
		if (end == 1 || end == 2) {
			i0 = cv_count - order;
			knot += i0;
			if (cv) {
				cv += i0*cv_stride;
				DXFEvaluateNurbsDeBoor(cv_dim, order, cv_stride, cv, knot, -1, 0.0, knot[order - 1]);
			}
			i0 = order - 1;
			for (i = 2 * order - 3; i > i0; i--)
				knot[i] = knot[i0];
			rc = true;
		}
	}
	return rc;
}


bool DXFMakeClampedUniformKnotVector(
int order,
int cv_count,
double* knot,
double delta
)
{
	bool rc = false;
	if (order >= 2 && cv_count >= order && knot != NULL && delta > 0.0)
	{
		double k;
		int i;
		for (i = order - 2, k = 0.0; i < cv_count; i++, k += delta)
			knot[i] = k;
		DXFClampKnotVector(order, cv_count, knot, 2);
		rc = true;
	}
	return rc;
}

// Description:
//   Fill in knot values for a clamped uniform knot
//   vector.
// Parameters:
//   order - [in] (>=2) order (degree+1) of the NURBS
//   cv_count - [in] (>=order) total number of control points
//       in the NURBS.
//   knot - [in/out] Input is an array with room for 
//       DXFKnotCount(order,cv_count) doubles.  Output is
//       a periodic uniform knot vector with domain
//       (0, (1+cv_count-order)*delta).
//   delta - [in] (>0, default=1.0) spacing between knots.
// Returns:
//   true if successful
bool DXFMakePeriodicUniformKnotVector(
int order,
int cv_count,
double* knot,
double delta
)
{
	bool rc = false;
	if (order >= 2 && cv_count >= order && knot != NULL && delta > 0.0)
	{
		double k = 0.0;
		int i, knot_count = DXFKnotCount(order, cv_count);
		for (i = order - 2, k = 0.0; i < knot_count; i++, k += delta)
			knot[i] = k;
		for (i = order - 3, k = -delta; i >= 0; i--, k -= delta)
			knot[i] = k;
		rc = true;
	}
	return rc;
}

static bool DXFInsertSingleKnot(int cv_dim, int order,
	int cv_stride,
	double *cv,   // NULL or array of length at least order*cv_stride+cv_dim
	double *knot, // array of length at least 2*order-1 and existing knot values in
	// knot[0], ..., knot[2*order-3]
	double knot_value // knot[order-2] <= knot_value < knot[order-1]
	// and knot[0] < knot_vale
	)
{
	double alpha0, alpha1;
	double *k0, *k1, *prev_cv;
	int    i, d, cv_inc, degree;

	if (order < 2 || !knot || knot_value < knot[order - 2] || knot[order - 1] <= knot_value) {
		DXF_ERROR("DXFInsertSingleKnot() - illegal knot input");
		return false;
	}

	if (cv) {
		if (cv_dim < 1 || cv_stride < cv_dim) {
			DXF_ERROR("DXFInsertSingleKnot() - illegal cv input");
			return false;
		}
	}

	degree = order - 1;

	// move last degree many knots over one spot
	k1 = knot + 2 * degree;
	k0 = k1 - 1;
	i = degree;
	while (i--)
		*k1-- = *k0--;

	// insert new knot value 
	*k1 = knot_value;

	if (cv) {
		// move last cv over one spot
		memcpy(cv + cv_dim*order, cv + cv_dim*degree, cv_dim*sizeof(*cv));

		// compute new cv values
		k0 = knot + degree - 1;
		k1 = k0 + order;
		cv += order*cv_stride;
		prev_cv = cv - cv_stride;
		cv_inc = cv_stride - cv_dim;
		i = degree;
		if (knot_value - *k0 <= *k1 - knot_value) {
			while (i--) {
				alpha1 = (knot_value - *k0) / (*k1 - *k0);
				alpha0 = 1.0 - alpha1;
				k0--; k1--;
				cv -= cv_inc;
				prev_cv -= cv_inc;
				d = cv_dim;
				while (d--) {
					--cv;
					--prev_cv;
					*cv = *cv * alpha1 + *prev_cv * alpha0;
				}
			}
		}
		else {
			while (i--) {
				alpha0 = (*k1 - knot_value) / (*k1 - *k0);
				alpha1 = 1.0 - alpha0;
				k0--; k1--;
				cv -= cv_inc;
				prev_cv -= cv_inc;
				d = cv_dim;
				while (d--) {
					--cv;
					--prev_cv;
					*cv = *cv * alpha1 + *prev_cv * alpha0;
				}
			}
		}
	}

	return true;
}

int DXFInsertKnot(
	double knot_value,
	int knot_multiplicity,
	int cv_dim,   // dimension of cv's = ( = dim+1 for rational cvs )
	int order,
	int cv_count,
	int cv_stride,
	double* cv,   // NULL or cv array with room for at least knot_multiplicity new cvs
	double* knot, // knot array with room for at least knot_multiplicity new knots
	int* hint     // optional hint about where to search for span to add knots to
	// pass NULL if no hint is available
	)
{
	int rc = 0; // return code = number of knots added

	if (order < 2 || cv_count < order || !knot)
	{
		DXF_ERROR("DXFInsertKnot(): illegal input");
		return 0;
	}

	if (cv)
	{
		if (cv_dim < 1 || cv_stride < cv_dim)
		{
			DXF_ERROR("DXFInsertKnot(): illegal input");
			return 0;
		}
	}

	if (knot_multiplicity >= order)
	{
		DXF_ERROR("DXFInsertKnot(): requested knot_multiplicity > degree");
		return 0;
	}

	// shift knot vector and cv array so that knot_value lies in first span
	int span_index = DXFNurbsSpanIndex(order, cv_count, knot, knot_value, 1, hint ? *hint : 0);
	knot += span_index;
	if (cv)
		cv += (span_index*cv_stride);
	cv_count -= span_index;

	const double knot_tolerance = DXFSpanTolerance(order, cv_count, knot, 0);

	// check that knot_value is interior to NURBS domain
	if (span_index == 0)
	{
		if (knot_value < knot[order - 1])
		{
			if (knot_value <= knot[order - 2] + knot_tolerance)
			{
				DXF_ERROR("DXFInsertKnot(): requested knot_value at start of NURBS domain");
				return 0;
			}
		}
	}
	if (span_index == cv_count - order)
	{
		if (knot_value > knot[order - 2] && knot_value >= knot[order - 1] - knot_tolerance)
		{
			DXF_ERROR("DXFInsertKnot(): requested knot_value at end of NURBS domain");
			return 0;
		}
	}

	// if knot_value is nearly equal to an existing knot, make it exactly equal
	if (knot_value <= 0.5*(knot[order - 2] + knot[order - 1]) && fabs(knot_value - knot[order - 2]) <= knot_tolerance) {
		knot_value = knot[order - 2];
	}
	else if (fabs(knot_value - knot[order - 1]) <= knot_tolerance) {
		knot_value = knot[order - 1];
	}

	const int degree = order - 1;

	// set m = number of knots to add
	int m = 0;
	int j;
	if (knot_value == knot[order - 2]) {
		for (j = order - 2; m < knot_multiplicity && knot[j - m] == knot_value; m++)
			; // empty for
	}
	else if (knot_value == knot[order - 1]) {
		for (j = order - 1; m < knot_multiplicity && knot[j + m] == knot_value; m++)
			; // empty for
	}
	m = knot_multiplicity - m;
	if (hint)
		*hint = span_index + m;

	if (m <= 0)
		return 0; // no knots need to be added

	double* new_knot = (double*)malloc(((2 * degree + m) + (order + m)*cv_dim)*sizeof(*new_knot));
	if (!new_knot) {
		DXF_ERROR("DXFInsertKnot(): out of memory");
		return 0;
	}
	double* new_cv = 0;
	memcpy(new_knot, knot, 2 * degree*sizeof(*new_knot));
	if (cv) {
		new_cv = new_knot + (2 * degree + m);
		for (j = 0; j < order; j++) {
			memcpy(new_cv + j*cv_dim, cv + j*cv_stride, cv_dim*sizeof(*new_cv));
		}
	}

	// add m more knots at knot_value
	rc = 0;
	while (m>0) {
		if (!DXFInsertSingleKnot(cv_dim, order, cv_dim, new_cv, new_knot, knot_value))
			break;
		m--;
		if (new_cv)
			new_cv += cv_stride;
		new_knot++;
		rc++;
	}
	new_knot -= rc;
	new_cv -= rc*cv_stride;

	if (rc > 0) {
		// make room for rc many new knots
		int i0 = DXFKnotCount(order, cv_count) - 1; // knot[i0] = last input knot
		int i1 = i0 + rc;
		int j = (cv_count - order);
		while (j--)
			knot[i1--] = knot[i0--];

		// update knot vector
		memcpy(knot + degree, new_knot + degree, (degree + rc)*sizeof(*new_knot));

		if (cv) {
			// make room for rc many new CVs
			i0 = (cv_count - 1)*cv_stride;             // cv[i0] = last coord of last input cv */
			i1 = i0 + rc*cv_stride;
			j = cv_count - order;
			while (j--) {
				memcpy(cv + i1, cv + i0, cv_dim*sizeof(*cv));
				i1 -= cv_stride;
				i0 -= cv_stride;
			}

			// update cv values
			for (j = 0; j < order + rc; j++) {
				memcpy(cv, new_cv, cv_dim*sizeof(*new_cv));
				cv += cv_stride;
				new_cv += cv_dim;
			}
		}

	}

	free(new_knot);

	return rc;
}

