#include "JetDxfNurbs.h"
#include "DXFNurbsKnot.h"
#include "DXFEvaluteNurbs.h"

CJetDxfNurbsCurve* CJetDxfNurbsCurve::New()
{
	return new CJetDxfNurbsCurve();
}

CJetDxfNurbsCurve* CJetDxfNurbsCurve::New(
	const CJetDxfNurbsCurve& nurbs_curve
	)
{
	return new CJetDxfNurbsCurve(nurbs_curve);
}

CJetDxfNurbsCurve* CJetDxfNurbsCurve::New(
	int dimension,
	bool bIsRational,
	int order,
	int cv_count
	)
{
	return new CJetDxfNurbsCurve(dimension, bIsRational, order, cv_count);
}


CJetDxfNurbsCurve::CJetDxfNurbsCurve()
{
	Initialize();
}

CJetDxfNurbsCurve::CJetDxfNurbsCurve(const CJetDxfNurbsCurve& src)
{
	Initialize();
	*this = src;
}

CJetDxfNurbsCurve::CJetDxfNurbsCurve(int dim, bool bIsRational, int order, int cv_count)
{
	Initialize();
	Create(dim, bIsRational, order, cv_count);
}

CJetDxfNurbsCurve::~CJetDxfNurbsCurve()
{
	Destroy();
}

unsigned int CJetDxfNurbsCurve::SizeOf() const
{
	unsigned int sz = (sizeof(*this));
	sz += m_knot_capacity*sizeof(*m_knot);
	sz += m_cv_capacity*sizeof(*m_cv);
	return sz;
}

int CJetDxfNurbsCurve::Dimension() const
{
	return m_dim;
}

bool CJetDxfNurbsCurve::IsRational() const
{
	return m_is_rat ? true : false;
}

int CJetDxfNurbsCurve::CVSize() const
{
	return (m_dim > 0) ? (m_is_rat ? (m_dim + 1) : m_dim) : 0;
}

int CJetDxfNurbsCurve::Order(void) const
{
	return m_order;
}

int CJetDxfNurbsCurve::CVCount(void) const
{
	return m_cv_count;
}

int CJetDxfNurbsCurve::KnotCount(void) const
{
	return DXFKnotCount(m_order, m_cv_count);
}

double* CJetDxfNurbsCurve::CV(int i) const
{
	return (m_cv) ? (m_cv + i*m_cv_stride) : NULL;
}

double CJetDxfNurbsCurve::Weight(int i) const
{
	return (m_cv && m_is_rat) ? m_cv[i*m_cv_stride + m_dim] : 1.0;
}


double CJetDxfNurbsCurve::Knot(int i) const
{
	return (m_knot) ? m_knot[i] : 0.0;
}

int CJetDxfNurbsCurve::KnotMultiplicity(int i) const
{
	return DXFKnotMultiplicity(m_order, m_cv_count, m_knot, i);
}

const double* CJetDxfNurbsCurve::Knot() const
{
	return m_knot;
}

double CJetDxfNurbsCurve::SuperfluousKnot(int end) const
{
	return(m_knot) ? DXFSuperfluousKnot(m_order, m_cv_count, m_knot, end) : 0.0;
}


bool CJetDxfNurbsCurve::MakePeriodicUniformKnotVector(double delta)
{
	ReserveKnotCapacity(DXFKnotCount(m_order, m_cv_count));
	return DXFMakePeriodicUniformKnotVector(m_order, m_cv_count, m_knot, delta);
}


bool CJetDxfNurbsCurve::MakeClampedUniformKnotVector(double delta)
{
	ReserveKnotCapacity(DXFKnotCount(m_order, m_cv_count));
	return DXFMakeClampedUniformKnotVector(m_order, m_cv_count, m_knot, delta);
}

bool CJetDxfNurbsCurve::CreateClampedUniformNurbs(
	int dimension,
	int order,
	int point_count,
	const CDxf3dPoint* point,
	double knot_delta
	)
{
	bool rc = (dimension >= 1 && dimension <= 3 && point != NULL);
	if (rc)
		rc = Create(dimension, false, order, point_count);
	if (rc)
	{
		int i;
		for (i = 0; i < point_count; i++)
			SetCV(i, CDXF::intrinsic_point_style, point[i]);
	}
	if (rc)
		rc = MakeClampedUniformKnotVector(knot_delta);
	return rc;
}

bool CJetDxfNurbsCurve::CreatePeriodicUniformNurbs(
	int dimension,
	int order,
	int point_count,
	const CDxf3dPoint* point,
	double knot_delta
	)
{
	bool rc = (dimension >= 1 && dimension <= 3 && point != NULL);
	if (rc)
		rc = Create(dimension, false, order, point_count + (order - 1));
	if (rc)
	{
		int i;
		for (i = 0; i < point_count; i++)
			SetCV(i, CDXF::intrinsic_point_style, point[i]);
		for (i = 0; i <= order - 2; i++)
			SetCV(m_cv_count - m_order + 1 + i, CDXF::intrinsic_point_style, CV(i));
	}
	if (rc)
		rc = MakePeriodicUniformKnotVector(knot_delta);
	return rc;
}

bool CJetDxfNurbsCurve::Create(
	int dim,      // dimension (>= 1)
	bool is_rat,  // true to make a rational NURBS
	int order,    // order (>= 2)
	int cv_count  // cv count (>= order)
	)
{
	if (dim < 1)
		return false;
	if (order < 2)
		return false;
	if (cv_count < order)
		return false;
	m_dim = dim;
	m_is_rat = (is_rat) ? true : false;
	m_order = order;
	m_cv_count = cv_count;
	m_cv_stride = (m_is_rat) ? m_dim + 1 : m_dim;
	bool rc = ReserveKnotCapacity(KnotCount());
	if (!ReserveCVCapacity(CVCount()*m_cv_stride))
		rc = false;
	return rc;
}

void CJetDxfNurbsCurve::Destroy()
{
	double* cv = (m_cv && m_cv_capacity) ? m_cv : NULL;
	double* knot = (m_knot && m_knot_capacity) ? m_knot : NULL;
	Initialize();
	if (cv)
		free(cv);
	if (knot)
		free(knot);
}

void CJetDxfNurbsCurve::Initialize()
{
	m_dim = 0;
	m_is_rat = 0;
	m_order = 0;
	m_cv_count = 0;
	m_knot_capacity = 0;
	m_knot = 0;
	m_cv_stride = 0;
	m_cv_capacity = 0;
	m_cv = 0;
}


CDxfInterval CJetDxfNurbsCurve::Domain() const
{
	CDxfInterval d;
	if (!DXFGetKnotVectorDomain(m_order, m_cv_count, m_knot, &d.m_t[0], &d.m_t[1]))
		d.Destroy();
	return d;
}

bool CJetDxfNurbsCurve::SetDomain(double t0, double t1)
{
	bool rc = false;
	if (m_order >= 2 && m_cv_count >= m_order && m_knot && t0 < t1) {
		//DestroyCurveTree();
		const double k0 = m_knot[m_order - 2];
		const double k1 = m_knot[m_cv_count - 1];
		if (k0 == t0 && k1 == t1)
			rc = true;
		else if (k0 < k1)
		{

			const double d = (t1 - t0) / (k1 - k0);
			const double km = 0.5*(k0 + k1);
			const int knot_count = KnotCount();
			int i;
			for (i = 0; i < knot_count; i++)
			{
				if (m_knot[i] <= km) {
					m_knot[i] = (m_knot[i] - k0)*d + t0;
				}
				else {
					m_knot[i] = (m_knot[i] - k1)*d + t1;
				}
			}
			rc = true;
		}
	}
	return rc;
}



int CJetDxfNurbsCurve::SpanCount() const
{
	// number of smooth spans in curve
	return DXFKnotVectorSpanCount(m_order, m_cv_count, m_knot);
}

bool CJetDxfNurbsCurve::GetSpanVector( // span "knots" 
	double* s // array of length SpanCount() + 1 
	) const
{
	return DXFGetKnotVectorSpanVector(m_order, m_cv_count, m_knot, s);
}

int CJetDxfNurbsCurve::Degree() const
{
	return m_order >= 2 ? m_order - 1 : 0;
}


bool CJetDxfNurbsCurve::Evaluate( double t, int der_count,  int v_stride,  double* v,  int side,  int* hint  ) 
{
	bool rc = false;

	if (m_order < 2)      // GBA added 01-12-06 to fix crash bug
		return false;

	const int span_index = DXFNurbsSpanIndex(m_order, m_cv_count, m_knot, t, side, (nullptr != hint) ? *hint : 0);
	rc = DXFEvaluateNurbsSpan(
		m_dim, m_is_rat?true:false, m_order,
		m_knot + span_index,
		m_cv_stride, m_cv + (m_cv_stride*span_index),
		der_count,
		t,
		v_stride, v
		);
	if (nullptr != hint)
		*hint = span_index;
	return rc;
}


bool
CJetDxfNurbsCurve::SetWeight(int i, double w)
{
	bool rc = false;
	if (m_is_rat) {
		double* cv = CV(i);
		if (cv) {
			cv[m_dim] = w;
			rc = true;
		}
	}
	else if (w == 1.0) {
		rc = true;
	}
	return rc;
}

bool
CJetDxfNurbsCurve::SetCV(int i, CDXF::point_style style, const double* Point)
{
	bool rc = true;
	int k;
	double w;

	// feeble but fast check for properly initialized class
	if (!m_cv || i < 0 || i >= m_cv_count)
		return false;

	double* cv = m_cv + i*m_cv_stride;

	switch (style) {

	case CDXF::not_rational:  // input Point is not rational
		memcpy(cv, Point, m_dim*sizeof(*cv));
		if (IsRational()) {
			// NURBS curve is rational - set weight to one
			cv[m_dim] = 1.0;
		}
		break;

	case CDXF::homogeneous_rational:  // input Point is homogeneous rational
		if (IsRational()) {
			// NURBS curve is rational
			memcpy(cv, Point, (m_dim + 1)*sizeof(*cv));
		}
		else {
			// NURBS curve is not rational
			w = (Point[m_dim] != 0.0) ? 1.0 / Point[m_dim] : 1.0;
			for (k = 0; k < m_dim; k++) {
				cv[k] = w*Point[k];
			}
		}
		break;

	case CDXF::euclidean_rational:  // input Point is euclidean rational
		if (IsRational()) {
			// NURBS curve is rational - convert euclean point to homogeneous form
			w = Point[m_dim];
			for (k = 0; k < m_dim; k++)
				cv[k] = w*Point[k]; // 22 April 2003 - bug fix [i] to [k]
			cv[m_dim] = w;
		}
		else {
			// NURBS curve is not rational
			memcpy(cv, Point, m_dim*sizeof(*cv));
		}
		break;

	case CDXF::intrinsic_point_style:
		memcpy(cv, Point, CVSize()*sizeof(*cv));
		break;

	default:
		rc = false;
		break;
	}
	return rc;
}

bool
CJetDxfNurbsCurve::SetCV(int i, const CDxf3dPoint& point)
{
	bool rc = false;
	double* cv = CV(i);
	if (cv) {
		cv[0] = point.x;
		if (m_dim > 1) {
			cv[1] = point.y;
			if (m_dim > 2)
				cv[2] = point.z;
			if (m_dim > 3) {
				memset(&cv[3], 0, (m_dim - 3)*sizeof(*cv));
			}
		}
		if (m_is_rat) {
			cv[m_dim] = 1.0;
		}
		rc = true;
	}
	return rc;
}

bool
CJetDxfNurbsCurve::GetCV(int i, CDXF::point_style style, double* Point) const
{
	const double* cv = CV(i);
	if (!cv)
		return false;
	int dim = Dimension();
	double w = (IsRational()) ? cv[dim] : 1.0;
	switch (style) {
	case CDXF::euclidean_rational:
		Point[dim] = w;
		// no break here
	case CDXF::not_rational:
		if (w == 0.0)
			return false;
		w = 1.0 / w;
		while (dim--) *Point++ = *cv++ * w;
		break;
	case CDXF::homogeneous_rational:
		Point[dim] = w;
		memcpy(Point, cv, dim*sizeof(*Point));
		break;
	case CDXF::intrinsic_point_style:
		memcpy(Point, cv, CVSize()*sizeof(*Point));
		break;
	default:
		return false;
	}
	return true;
}

bool
CJetDxfNurbsCurve::GetCV(int i, CDxf3dPoint& point) const
{
	bool rc = false;
	const double* cv = CV(i);
	if (cv) {
		if (m_is_rat) {
			if (cv[m_dim] != 0.0) {
				const double w = 1.0 / cv[m_dim];
				point.x = cv[0] * w;
				point.y = (m_dim > 1) ? cv[1] * w : 0.0;
				point.z = (m_dim > 2) ? cv[2] * w : 0.0;
				rc = true;
			}
		}
		else {
			point.x = cv[0];
			point.y = (m_dim > 1) ? cv[1] : 0.0;
			point.z = (m_dim > 2) ? cv[2] : 0.0;
			rc = true;
		}
	}
	return rc;
}

bool CJetDxfNurbsCurve::SetKnot(int knot_index, double k)
{
	if (knot_index < 0 || knot_index >= KnotCount())
		return false;
	m_knot[knot_index] = k;
	return true;
}


CDxf3dPoint CJetDxfNurbsCurve::PointAt(double t)
{
	CDxf3dPoint p(0.0, 0.0, 0.0);
	if (!EvPoint(t, p))
		p = DXF_UNSET_POINT;
	return p;
}

bool CJetDxfNurbsCurve::EvPoint(double t, CDxf3dPoint& point, int side, int* hint)
{
	bool rc = false;
	double ws[128];
	double* v;
	if (Dimension() <= 3) {
		v = &point.x;
		point.x = 0.0;
		point.y = 0.0;
		point.z = 0.0;
	}
	else if (Dimension() <= 128) {
		v = ws;
	}
	else {
		v = (double*)malloc(Dimension()*sizeof(*v));
	}
	rc = Evaluate(t, 0, Dimension(), v, side, hint);
	if (Dimension() > 3) {
		point.x = v[0];
		point.y = v[1];
		point.z = v[2];
		if (Dimension() > 128)
			free(v);
	}
	return rc;
}

CDxf3dPoint CJetDxfNurbsCurve::PointAtStart()
{
	return PointAt(Domain().Min());
}

CDxf3dPoint CJetDxfNurbsCurve::PointAtEnd()
{
	return PointAt(Domain().Max());
}

bool CJetDxfNurbsCurve::ZeroCVs()
{
	bool rc = false;
	int i;
	if (m_cv) {
		if (m_cv_capacity > 0) {
			memset(m_cv, 0, m_cv_capacity*sizeof(*m_cv));
			if (m_is_rat) {
				for (i = 0; i < m_cv_count; i++) {
					SetWeight(i, 1.0);
				}
			}
			rc = true;
		}
		else {
			double* cv;
			int s = CVSize()*sizeof(*cv);
			for (i = 0; i < m_cv_count; i++) {
				cv = CV(i);
				memset(cv, 0, s);
				if (m_is_rat)
					cv[m_dim] = 1.0;
			}
			rc = (i > 0) ? true : false;
		}
	}
	return rc;
}

bool CJetDxfNurbsCurve::IsClamped( // determine if knot vector is clamped
	int end // (default= 2) end to check: 0 = start, 1 = end, 2 = start and end
	) const
{
	return DXFIsKnotVectorClamped(m_order, m_cv_count, m_knot, end);
}


bool CJetDxfNurbsCurve::MakeRational()
{
	if (!IsRational()) {
		const int dim = Dimension();
		const int cv_count = CVCount();
		if (cv_count > 0 && m_cv_stride >= dim && dim > 0) {
			const int new_stride = (m_cv_stride == dim) ? dim + 1 : m_cv_stride;
			ReserveCVCapacity(cv_count*new_stride);
			const double* old_cv;
			double* new_cv;
			int cvi, j;
			for (cvi = cv_count - 1; cvi >= 0; cvi--) {
				old_cv = CV(cvi);
				new_cv = m_cv + (cvi*new_stride);
				for (j = dim - 1; j >= 0; j--) {
					new_cv[j] = old_cv[j];
				}
				new_cv[dim] = 1.0;
			}
			m_cv_stride = new_stride;
			m_is_rat = 1;
		}
	}
	return IsRational();
}

bool CJetDxfNurbsCurve::MakeNonRational()
{
	if (IsRational()) {
		const int dim = Dimension();
		const int cv_count = CVCount();
		if (cv_count > 0 && m_cv_stride >= dim + 1 && dim > 0) {
			double w;
			const double* old_cv;
			double* new_cv = m_cv;
			int cvi, j;
			for (cvi = 0; cvi < cv_count; cvi++) {
				old_cv = CV(cvi);
				w = old_cv[dim];
				w = (w != 0.0) ? 1.0 / w : 1.0;
				for (j = 0; j < dim; j++) {
					*new_cv++ = w*(*old_cv++);
				}
			}
			m_is_rat = 0;
			m_cv_stride = dim;
		}
	}

	return (!IsRational()) ? true : false;
}

bool CJetDxfNurbsCurve::ChangeDimension(int desired_dimension)
{
	bool rc = false;
	int i, j;
	if (desired_dimension < 1)
		return false;
	if (desired_dimension == m_dim)
		return true;



	if (desired_dimension < m_dim)
	{
		if (m_is_rat) {
			double* cv;
			for (i = 0; i < m_cv_count; i++) {
				cv = CV(i);
				cv[desired_dimension] = cv[m_dim];
			}
		}
		m_dim = desired_dimension;
		rc = true;
	}
	else
	{
		const double* cv0;
		double* cv1;
		//const int cv_size0 = CVSize();
		const int cv_size1 = m_is_rat ? desired_dimension + 1 : desired_dimension;
		const int cv_stride1 = (m_cv_stride < cv_size1) ? cv_size1 : m_cv_stride;
		if (m_cv_stride < cv_stride1 && m_cv_capacity > 0) {
			m_cv_capacity = cv_stride1*m_cv_count;
			m_cv = (double*)realloc(m_cv, m_cv_capacity*sizeof(*m_cv));
		}
		for (i = CVCount() - 1; i >= 0; i--) {
			cv0 = CV(i);
			cv1 = m_cv + (i*cv_stride1);
			if (m_is_rat)
				cv1[desired_dimension] = cv0[m_dim];
			for (j = desired_dimension - 1; j >= m_dim; j--) {
				cv1[j] = 0.0;
			}
			for (j = m_dim - 1; j >= 0; j--) {
				cv1[j] = cv0[j];
			}
		}
		m_dim = desired_dimension;
		m_cv_stride = cv_stride1;
		rc = true;
	}
	return rc;
}

bool CJetDxfNurbsCurve::GetLength(
	double* length,
	double fractional_tolerance,
	const CDxfInterval* sub_domain
	) const
{
	// 3rd party developers who want to enhance openNURBS
	// may provide a working GetLength here.
	if (length)
		*length = 0;
	return false;
}

bool CJetDxfNurbsCurve::ReserveCVCapacity(int desired_capacity)
{
	// If m_cv_capacity == 0 and m_cv != NULL, then the user
	// has hand built the CJetDxfNurbsCurve.m_cv array and is responsible
	// for making sure it's always big enough.
	bool rc = true;
	if (desired_capacity > m_cv_capacity) {
		if (!m_cv) {
			// no cv array - allocate one
			m_cv = (double*)malloc(desired_capacity*sizeof(*m_cv));
			if (!m_cv) {
				rc = false;
			}
			else {
				m_cv_capacity = desired_capacity;
			}
		}
		else if (m_cv_capacity > 0) {
			// existing m_cv[] is too small and the fact that
			// m_cv_capacity > 0 indicates that CJetDxfNurbsCurve is
			// managing the m_cv[] memory, so we need to grow
			// the m_cv[] array.
			m_cv = (double*)realloc(m_cv, desired_capacity*sizeof(*m_cv));
			if (!m_cv) {
				rc = false;
				m_cv_capacity = 0;
			}
			else {
				m_cv_capacity = desired_capacity;
			}
		}
	}
	return rc;
}

bool CJetDxfNurbsCurve::ClampEnd(
	int end // 0 = clamp start, 1 = clamp end, 2 = clamp start and end 
	)
{
	// Curve tree doesn't change when you clamp // DestroyCurveTree();
	return DXFClampKnotVector(CVSize(), m_order,
		m_cv_count, m_cv_stride, m_cv,
		m_knot, end);
}

bool CJetDxfNurbsCurve::ReserveKnotCapacity(int desired_capacity)
{
	// If m_knot_capacity == 0 and m_knot != NULL, then the user
	// has hand built the CJetDxfNurbsCurve.m_knot array and is responsible
	// for making sure it's always big enough.
	bool rc = true;
	if (desired_capacity > m_knot_capacity) {
		if (!m_knot) {
			// no knot array - allocate one
			m_knot = (double*)malloc(desired_capacity*sizeof(*m_knot));
			if (!m_knot) {
				m_knot_capacity = 0;
				rc = false;
			}
			else {
				m_knot_capacity = desired_capacity;
			}
		}
		else if (m_knot_capacity > 0) {
			// existing m_knot[] is too small and the fact that
			// m_knot_capacity > 0 indicates that CJetDxfNurbsCurve is
			// managing the m_knot[] memory, so we need to grow
			// the m_knot[] array.
			m_knot = (double*)realloc(m_knot, desired_capacity*sizeof(*m_knot));
			if (!m_knot) {
				rc = false;
				m_knot_capacity = 0;
			}
			else {
				m_knot_capacity = desired_capacity;
			}
		}
	}
	return rc;
}

int CJetDxfNurbsCurve::HasNurbForm() const
{
	return 1;
}


bool CJetDxfNurbsCurve::GetCurveParameterFromNurbFormParameter(double nurbs_t,double* curve_t) const
{
	*curve_t = nurbs_t;
	return true;
}

bool CJetDxfNurbsCurve::GetNurbFormParameterFromCurveParameter(double curve_t,double* nurbs_t) const
{
	*nurbs_t = curve_t;
	return true;
}
