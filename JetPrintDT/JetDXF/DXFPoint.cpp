#include "DXFPoint.h"

bool DXFIsValid(double x)
{
	return DXF_IS_VALID(x);
}

bool ON_IsValidFloat(float x)
{
	return DXF_IS_VALID_FLOAT(x);
}

const CDxfInterval CDxfInterval::EmptyInterval(DXF_UNSET_VALUE, DXF_UNSET_VALUE);

CDxfInterval::CDxfInterval()
{
	m_t[0] = m_t[1] = DXF_UNSET_VALUE;
}

CDxfInterval::CDxfInterval(double t0, double t1)
{
	Set(t0, t1);
}

CDxfInterval::~CDxfInterval()
{}

double&
CDxfInterval::operator[](int i)
{
	return m_t[(i <= 0) ? 0 : 1];
}

double
CDxfInterval::operator[](int i) const
{
	return m_t[(i <= 0) ? 0 : 1];
}

double
CDxfInterval::Min() const
{
	return m_t[IsDecreasing() ? 1 : 0];
}

void CDxfInterval::Destroy()
{
	Set(DXF_UNSET_VALUE, DXF_UNSET_VALUE);
}

void CDxfInterval::Set(double t0, double t1)
{
	m_t[0] = t0;
	m_t[1] = t1;
}

double CDxfInterval::ParameterAt(double x) const
{
	return (DXF_IS_VALID(x) ? ((1.0 - x)*m_t[0] + x*m_t[1]) : DXF_UNSET_VALUE);
}

CDxfInterval CDxfInterval::ParameterAt(CDxfInterval x) const
{
	return CDxfInterval(ParameterAt(x[0]), ParameterAt(x[1]));
}

double CDxfInterval::NormalizedParameterAt( // returns x so that min*(1.0-x) + max*x = input
	double t
	) const
{
	if (!DXF_IS_VALID(t))
		return DXF_UNSET_VALUE; // added 29 Sep 2006

	double x = m_t[0];
	if (m_t[0] != m_t[1]) {
		x = (t == m_t[1]) ? 1.0 : (t - m_t[0]) / (m_t[1] - m_t[0]);
	}
	return x;
}

CDxfInterval CDxfInterval::NormalizedParameterAt( // returns x so that min*(1.0-x) + max*x = input
	CDxfInterval t
	) const
{
	return  CDxfInterval(NormalizedParameterAt(t[0]),
		NormalizedParameterAt(t[1]));
}

double
CDxfInterval::Max() const
{
	return m_t[IsDecreasing() ? 0 : 1];
}

double
CDxfInterval::Mid() const
{
	return 0.5*(m_t[0] + m_t[1]);
}

double
CDxfInterval::Length() const
{
	return (DXF_IS_VALID(m_t[0]) && DXF_IS_VALID(m_t[1])) ? m_t[1] - m_t[0] : 0.0;
}

bool
CDxfInterval::IsIncreasing() const
{
	return (m_t[0] < m_t[1] && DXF_IS_VALID(m_t[0]) && DXF_IS_VALID(m_t[1])) ? true : false;
}

bool
CDxfInterval::IsDecreasing() const
{
	return (m_t[0] > m_t[1] && DXF_IS_VALID(m_t[0]) && DXF_IS_VALID(m_t[1])) ? true : false;
}

bool
CDxfInterval::IsInterval() const
{
	return (m_t[0] != m_t[1] && DXF_IS_VALID(m_t[0]) && DXF_IS_VALID(m_t[1])) ? true : false;
}


bool
CDxfInterval::IsSingleton() const
{
	return (m_t[0] == m_t[1] && DXF_IS_VALID(m_t[1])) ? true : false;
}

bool
CDxfInterval::IsEmptyInterval() const
{
	return (m_t[0] == DXF_UNSET_VALUE && m_t[1] == DXF_UNSET_VALUE) ? true : false;
}

bool
CDxfInterval::IsEmptySet() const
{
	return (m_t[0] == DXF_UNSET_VALUE && m_t[1] == DXF_UNSET_VALUE) ? true : false;
}

bool
CDxfInterval::IsValid() const
{
	// 05/29/2007 TimH. Changed 0 to 1.
	return (DXF_IS_VALID(m_t[0]) && DXF_IS_VALID(m_t[1]));
}

bool
CDxfInterval::MakeIncreasing()		// returns true if resulting interval IsIncreasing() 
{
	if (IsDecreasing()){
		Swap();
		return true;
	}
	return IsIncreasing();
}

bool
CDxfInterval::operator!=(const CDxfInterval& other) const
{
	return Compare(other) ? true : false;
}

bool
CDxfInterval::operator==(const CDxfInterval& other) const
{
	return Compare(other) ? false : true;
}


int
CDxfInterval::Compare(const CDxfInterval& other) const
{
	if (m_t[0] < other.m_t[0])
		return -1;
	if (m_t[0] > other.m_t[0])
		return 1;
	if (m_t[1] < other.m_t[1])
		return -1;
	if (m_t[1] > other.m_t[1])
		return 1;
	return 0;
}

bool
CDxfInterval::Includes(double t, bool bTestOpenInterval) const
{
	bool rc = false;
	if (DXF_IS_VALID(t) && DXF_IS_VALID(m_t[0]) && DXF_IS_VALID(m_t[1]))
	{
		int i = (m_t[0] <= m_t[1]) ? 0 : 1;
		if (bTestOpenInterval)
		{
			rc = (m_t[i] < t && t < m_t[1 - i]) ? true : false;
		}
		else
		{
			rc = (m_t[i] <= t && t <= m_t[1 - i]) ? true : false;
		}
	}
	return rc;
}

bool
CDxfInterval::Includes(const CDxfInterval& other, bool bProperSubSet) const
{
	bool rc = (Includes(other.m_t[0]) && Includes(other.m_t[1])) ? true : false;
	if (rc && bProperSubSet)
	{
		if (!Includes(other.m_t[0], true) && !Includes(other.m_t[1], true))
			rc = false;
	}
	return rc;
}

void
CDxfInterval::Reverse()
{
	if (!IsEmptySet()) {
		const double x = -m_t[0];
		m_t[0] = -m_t[1];
		m_t[1] = x;
	}
}

void
CDxfInterval::Swap()
{
	const double x = m_t[0];
	m_t[0] = m_t[1];
	m_t[1] = x;
}


const CDxf2dPoint CDxf2dPoint::Origin(0.0, 0.0);
const CDxf2dPoint CDxf2dPoint::UnsetPoint(DXF_UNSET_VALUE, DXF_UNSET_VALUE);

const CDxf3dPoint CDxf3dPoint::Origin(0.0, 0.0, 0.0);
const CDxf3dPoint CDxf3dPoint::UnsetPoint(DXF_UNSET_VALUE, DXF_UNSET_VALUE, DXF_UNSET_VALUE);

const CDxf2dVector CDxf2dVector::ZeroVector(0.0, 0.0);
const CDxf2dVector CDxf2dVector::XAxis(1.0, 0.0);
const CDxf2dVector CDxf2dVector::YAxis(0.0, 1.0);
const CDxf2dVector CDxf2dVector::UnsetVector(DXF_UNSET_VALUE, DXF_UNSET_VALUE);

const CDxf3dVector CDxf3dVector::ZeroVector(0.0, 0.0, 0.0);
const CDxf3dVector CDxf3dVector::XAxis(1.0, 0.0, 0.0);
const CDxf3dVector CDxf3dVector::YAxis(0.0, 1.0, 0.0);
const CDxf3dVector CDxf3dVector::ZAxis(0.0, 0.0, 1.0);
const CDxf3dVector CDxf3dVector::UnsetVector(DXF_UNSET_VALUE, DXF_UNSET_VALUE, DXF_UNSET_VALUE);


// OBSOLETE  - use CDxf3dVector::UnsetVector
const CDxf3dVector DXF_UNSET_VECTOR(DXF_UNSET_VALUE, DXF_UNSET_VALUE, DXF_UNSET_VALUE);

// OBSOLETE  - use CDxf3dPoint::Origin
const CDxf3dPoint  ON_origin(0.0, 0.0, 0.0);

// OBSOLETE  - use CDxf3dVector::XAxis
const CDxf3dVector ON_xaxis(1.0, 0.0, 0.0);

// OBSOLETE  - use CDxf3dVector::YAxis
const CDxf3dVector ON_yaxis(0.0, 1.0, 0.0);

// OBSOLETE  - use CDxf3dVector::ZAxis
const CDxf3dVector ON_zaxis(0.0, 0.0, 1.0);


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
// CDxf2dPoint
//

CDxf2dPoint::CDxf2dPoint()
{}

CDxf2dPoint::CDxf2dPoint(const double* p)
{
	if (p) {
		x = (float)p[0]; y = (float)p[1];
	}
	else {
		x = y = 0.0;
	}
}

CDxf2dPoint::CDxf2dPoint(const float* p)
{
	if (p) {
		x = p[0]; y = p[1];
	}
	else {
		x = y = 0.0;
	}
}



CDxf2dPoint::CDxf2dPoint(const CDxf2dVector& v)
{
	x = v.x; y = v.y;
}


CDxf2dPoint::CDxf2dPoint(const CDxf2dPoint& p)
{
	x = (float)p.x; y = (float)p.y;
}

CDxf2dPoint::CDxf2dPoint(const CDxf3dPoint& p)
{
	x = (float)p.x; y = (float)p.y;
}

CDxf2dPoint::CDxf2dPoint(const CDxf3dVector& v)
{
	x = (float)v.x; y = (float)v.y;
}



CDxf2dPoint& CDxf2dPoint::operator=(const double* p)
{
	if (p) {
		x = (float)p[0];
		y = (float)p[1];
	}
	else {
		x = y = 0.0;
	}
	return *this;
}

CDxf2dPoint& CDxf2dPoint::operator=(const float* p)
{
	if (p) {
		x = p[0];
		y = p[1];
	}
	else {
		x = y = 0.0;
	}
	return *this;
}

CDxf2dPoint& CDxf2dPoint::operator=(const CDxf2dVector& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

CDxf2dPoint& CDxf2dPoint::operator=(const CDxf2dPoint& p)
{
	x = (float)p.x;
	y = (float)p.y;
	return *this;
}

CDxf2dPoint& CDxf2dPoint::operator=(const CDxf3dPoint& p)
{
	x = (float)p.x;
	y = (float)p.y;
	return *this;
}

CDxf2dPoint& CDxf2dPoint::operator=(const CDxf3dVector& v)
{
	x = (float)v.x;
	y = (float)v.y;
	return *this;
}



CDxf2dPoint& CDxf2dPoint::operator+=(const CDxf2dPoint& p)
{
	x += p.x;
	y += p.y;
	return *this;
}

CDxf2dPoint& CDxf2dPoint::operator+=(const CDxf2dVector& v)
{
	x += v.x;
	y += v.y;
	return *this;
}


CDxf2dPoint& CDxf2dPoint::operator-=(const CDxf2dPoint& p)
{
	x -= p.x;
	y -= p.y;
	return *this;
}

CDxf2dPoint& CDxf2dPoint::operator-=(const CDxf2dVector& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}



CDxf2dPoint CDxf2dPoint::operator/(int i) const
{
	const float one_over_d = 1.0f / ((float)i);
	return CDxf2dPoint(x*one_over_d, y*one_over_d);
}

CDxf2dPoint CDxf2dPoint::operator/(float d) const
{
	const float one_over_d = 1.0f / d;
	return CDxf2dPoint(x*one_over_d, y*one_over_d);
}

CDxf2dPoint CDxf2dPoint::operator/(double d) const
{
	const double one_over_d = 1.0 / d;
	return CDxf2dPoint(x*one_over_d, y*one_over_d);
}

CDxf2dPoint CDxf2dPoint::operator+(const CDxf2dPoint& p) const
{
	return CDxf2dPoint(x + p.x, y + p.y);
}

CDxf2dPoint CDxf2dPoint::operator+(const CDxf2dVector& v) const
{
	return CDxf2dPoint(x + v.x, y + v.y);
}

CDxf2dVector CDxf2dPoint::operator-(const CDxf2dPoint& p) const
{
	return CDxf2dVector(x - p.x, y - p.y);
}

CDxf2dPoint CDxf2dPoint::operator-(const CDxf2dVector& v) const
{
	return CDxf2dPoint(x - v.x, y - v.y);
}

CDxf3dPoint CDxf2dPoint::operator+(const CDxf3dPoint& p) const
{
	return CDxf3dPoint(x + p.x, y + p.y, p.z);
}

CDxf3dPoint CDxf2dPoint::operator+(const CDxf3dVector& v) const
{
	return CDxf3dPoint(x + v.x, y + v.y, v.z);
}

CDxf3dVector CDxf2dPoint::operator-(const CDxf3dPoint& p) const
{
	return CDxf3dVector(x - p.x, y - p.y, -p.y);
}

CDxf3dPoint CDxf2dPoint::operator-(const CDxf3dVector& v) const
{
	return CDxf3dPoint(x - v.x, y - v.y, -v.z);
}

bool CDxf2dPoint::operator==(const CDxf2dPoint& p) const
{
	return (x == p.x&&y == p.y) ? true : false;
}

bool CDxf2dPoint::operator!=(const CDxf2dPoint& p) const
{
	return (x != p.x || y != p.y) ? true : false;
}

bool CDxf2dPoint::operator<=(const CDxf2dPoint& p) const
{
	// dictionary order
	return ((x < p.x) ? true : ((x == p.x&&y <= p.y) ? true : false));
}

bool CDxf2dPoint::operator>=(const CDxf2dPoint& p) const
{
	// dictionary order
	return ((x > p.x) ? true : ((x == p.x&&y >= p.y) ? true : false));
}

bool CDxf2dPoint::operator<(const CDxf2dPoint& p) const
{
	// dictionary order
	return ((x < p.x) ? true : ((x == p.x&&y < p.y) ? true : false));
}

bool CDxf2dPoint::operator>(const CDxf2dPoint& p) const
{
	// dictionary order
	return ((x > p.x) ? true : ((x == p.x&&y>p.y) ? true : false));
}


void CDxf2dPoint::Zero()
{
	x = y = 0.0;
}

// static
const CDxf2dVector& CDxf2dVector::UnitVector(int index)
{
	static CDxf2dVector o(0.0, 0.0);
	static CDxf2dVector x(1.0, 0.0);
	static CDxf2dVector y(0.0, 1.0);
	switch (index)
	{
	case 0:
		return x;
	case 1:
		return y;
	}
	return o;
}

CDxf2dVector::CDxf2dVector()
{}

CDxf2dVector::CDxf2dVector(const double* v)
{
	if (v) {
		x = (float)v[0]; y = (float)v[1];
	}
	else {
		x = y = 0.0;
	}
}

CDxf2dVector::CDxf2dVector(const float* v)
{
	if (v) {
		x = v[0]; y = v[1];
	}
	else {
		x = y = 0.0;
	}
}

CDxf2dVector::CDxf2dVector(const CDxf2dPoint& p)
{
	x = p.x; y = p.y;
}

CDxf2dVector::CDxf2dVector(const CDxf2dVector& v)
{
	x = (float)v.x; y = (float)v.y;
}

CDxf2dVector::CDxf2dVector(const CDxf3dVector& v)
{
	x = (float)v.x; y = (float)v.y;
}

CDxf2dVector::CDxf2dVector(const CDxf3dPoint& p)
{
	x = (float)p.x; y = (float)p.y;
}


CDxf2dVector& CDxf2dVector::operator=(const double* v)
{
	if (v) {
		x = (float)v[0];
		y = (float)v[1];
	}
	else {
		x = y = 0.0;
	}
	return *this;
}

CDxf2dVector& CDxf2dVector::operator=(const float* v)
{
	if (v) {
		x = v[0];
		y = v[1];
	}
	else {
		x = y = 0.0;
	}
	return *this;
}

CDxf2dVector& CDxf2dVector::operator=(const CDxf2dPoint& p)
{
	x = p.x;
	y = p.y;
	return *this;
}


CDxf2dVector& CDxf2dVector::operator=(const CDxf2dVector& v)
{
	x = (float)v.x;
	y = (float)v.y;
	return *this;
}

CDxf2dVector& CDxf2dVector::operator=(const CDxf3dVector& v)
{
	x = (float)v.x;
	y = (float)v.y;
	return *this;
}

CDxf2dVector& CDxf2dVector::operator=(const CDxf3dPoint& p)
{
	x = (float)p.x;
	y = (float)p.y;
	return *this;
}

CDxf2dVector CDxf2dVector::operator-() const
{
	return CDxf2dVector(-x, -y);
}


CDxf2dVector& CDxf2dVector::operator+=(const CDxf2dVector& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

CDxf2dVector& CDxf2dVector::operator-=(const CDxf2dVector& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

CDxf2dVector CDxf2dVector::operator*(int d) const
{
	return CDxf2dVector(x*d, y*d);
}

CDxf2dVector CDxf2dVector::operator*(float d) const
{
	return CDxf2dVector(x*d, y*d);
}

CDxf2dVector CDxf2dVector::operator*(double d) const
{
	return CDxf2dVector(x*d, y*d);
}

double CDxf2dVector::operator*(const CDxf2dVector& v) const
{
	return (x*v.x + y*v.y);
}

CDxf2dVector CDxf2dVector::operator/(int d) const
{
	const float one_over_d = 1.0f / ((float)d);
	return CDxf2dVector(x*one_over_d, y*one_over_d);
}

CDxf2dVector CDxf2dVector::operator/(float d) const
{
	const float one_over_d = 1.0f / d;
	return CDxf2dVector(x*one_over_d, y*one_over_d);
}

CDxf2dVector CDxf2dVector::operator/(double d) const
{
	const double one_over_d = 1.0 / d;
	return CDxf2dVector(x*one_over_d, y*one_over_d);
}

CDxf2dVector CDxf2dVector::operator+(const CDxf2dVector& v) const
{
	return CDxf2dVector(x + v.x, y + v.y);
}

CDxf2dPoint CDxf2dVector::operator+(const CDxf2dPoint& p) const
{
	return CDxf2dPoint(x + p.x, y + p.y);
}

CDxf2dVector CDxf2dVector::operator-(const CDxf2dVector& v) const
{
	return CDxf2dVector(x - v.x, y - v.y);
}

CDxf2dPoint CDxf2dVector::operator-(const CDxf2dPoint& v) const
{
	return CDxf2dPoint(x - v.x, y - v.y);
}


CDxf3dVector CDxf2dVector::operator+(const CDxf3dVector& v) const
{
	return CDxf3dVector(x + v.x, y + v.y, v.z);
}

CDxf3dPoint CDxf2dVector::operator+(const CDxf3dPoint& p) const
{
	return CDxf3dPoint(x + p.x, y + p.y, p.z);
}

CDxf3dVector CDxf2dVector::operator-(const CDxf3dVector& v) const
{
	return CDxf3dVector(x - v.x, y - v.y, -v.z);
}

CDxf3dPoint CDxf2dVector::operator-(const CDxf3dPoint& v) const
{
	return CDxf3dPoint(x - v.x, y - v.y, -v.z);
}

bool CDxf2dVector::operator==(const CDxf2dVector& v) const
{
	return (x == v.x&&y == v.y) ? true : false;
}

bool CDxf2dVector::operator!=(const CDxf2dVector& v) const
{
	return (x != v.x || y != v.y) ? true : false;
}

bool CDxf2dVector::operator<=(const CDxf2dVector& v) const
{
	// dictionary order
	return ((x < v.x) ? true : ((x == v.x&&y <= v.y) ? true : false));
}

bool CDxf2dVector::operator>=(const CDxf2dVector& v) const
{
	// dictionary order
	return ((x > v.x) ? true : ((x == v.x&&y >= v.y) ? true : false));
}

bool CDxf2dVector::operator<(const CDxf2dVector& v) const
{
	// dictionary order
	return ((x < v.x) ? true : ((x == v.x&&y < v.y) ? true : false));
}

bool CDxf2dVector::operator>(const CDxf2dVector& v) const
{
	// dictionary order
	return ((x > v.x) ? true : ((x == v.x&&y>v.y) ? true : false));
}


void CDxf2dVector::Zero()
{
	x = y = 0.0;
}

void CDxf2dVector::Reverse()
{
	x = -x;
	y = -y;
}

bool CDxf2dVector::IsZero() const
{
	return (x == 0.0f && y == 0.0f);
}

CDxf2dVector operator*(int d, const CDxf2dVector& v)
{
	return CDxf2dVector(d*v.x, d*v.y);
}

CDxf2dVector operator*(float d, const CDxf2dVector& v)
{
	return CDxf2dVector(d*v.x, d*v.y);
}

CDxf2dVector operator*(double d, const CDxf2dVector& v)
{
	return CDxf2dVector(d*v.x, d*v.y);
}

CDxf2dPoint::CDxf2dPoint(double xx, double yy)
{
	x = xx; y = yy;
}


CDxf2dPoint::operator double*()
{
	return &x;
}

CDxf2dPoint::operator const double*() const
{
	return &x;
}


CDxf2dPoint& CDxf2dPoint::operator*=(double d)
{
	x *= d;
	y *= d;
	return *this;
}

CDxf2dPoint& CDxf2dPoint::operator/=(double d)
{
	const double one_over_d = 1.0 / d;
	x *= one_over_d;
	y *= one_over_d;
	return *this;
}

CDxf2dPoint& CDxf2dPoint::operator+=(const CDxf3dVector& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

CDxf2dPoint& CDxf2dPoint::operator-=(const CDxf3dVector& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}


double CDxf2dPoint::operator*(const CDxf2dPoint& h) const
{
	return x*h.x + y*h.y;
}

double CDxf2dPoint::operator*(const CDxf2dVector& h) const
{
	return x*h.x + y*h.y;
}


double CDxf2dPoint::operator[](int i) const
{
	return (i <= 0) ? x : y;
}

double& CDxf2dPoint::operator[](int i)
{
	double* pd = (i <= 0) ? &x : &y;
	return *pd;
}

int CDxf2dPoint::MaximumCoordinateIndex() const
{
	return (fabs(y) > fabs(x)) ? 1 : 0;
}

double CDxf2dPoint::MaximumCoordinate() const
{
	double c = fabs(x); if (fabs(y) > c) c = fabs(y);
	return c;
}

int CDxf2dPoint::MinimumCoordinateIndex() const
{
	return (fabs(y) < fabs(x)) ? 1 : 0;
}

double CDxf2dPoint::MinimumCoordinate() const
{
	double c = fabs(x); if (fabs(y) < c) c = fabs(y);
	return c;
}

CDxf2dPoint operator*(int i, const CDxf2dPoint& p)
{
	double d = i;
	return CDxf2dPoint(d*p.x, d*p.y);
}

CDxf2dPoint operator*(float f, const CDxf2dPoint& p)
{
	double d = f;
	return CDxf2dPoint(d*p.x, d*p.y);
}

CDxf2dPoint operator*(double d, const CDxf2dPoint& p)
{
	return CDxf2dPoint(d*p.x, d*p.y);
}

////////////////////////////////////////////////////////////////
//
// CDxf3dPoint
//

CDxf3dPoint::CDxf3dPoint()
{}

CDxf3dPoint::CDxf3dPoint(const float* p)
{
	if (p) {
		x = (double)p[0]; y = (double)p[1]; z = (double)p[2];
	}
	else {
		x = y = z = 0.0;
	}
}

CDxf3dPoint::CDxf3dPoint(const CDxf2dPoint& p)
{
	x = (double)p.x; y = (double)p.y; z = 0.0;
}

CDxf3dPoint::CDxf3dPoint(const CDxf2dVector& p)
{
	x = (double)p.x; y = (double)p.y; z = 0.0;
}

CDxf3dPoint::CDxf3dPoint(const double* p)
{
	if (p) {
		x = p[0]; y = p[1]; z = p[2];
	}
	else {
		x = y = z = 0.0;
	}
}

CDxf3dPoint::CDxf3dPoint(double xx, double yy, double zz)
{
	x = xx; y = yy; z = zz;
}


CDxf3dPoint::CDxf3dPoint(const CDxf3dVector& v)
{
	x = v.x; y = v.y; z = v.z;
}

CDxf3dPoint::operator double*()
{
	return &x;
}

CDxf3dPoint::operator const double*() const
{
	return &x;
}

CDxf3dPoint& CDxf3dPoint::operator=(const float* p)
{
	if (p) {
		x = (double)p[0];
		y = (double)p[1];
		z = (double)p[2];
	}
	else {
		x = y = z = 0.0;
	}
	return *this;
}

CDxf3dPoint& CDxf3dPoint::operator=(const double* p)
{
	if (p) {
		x = p[0];
		y = p[1];
		z = p[2];
	}
	else {
		x = y = z = 0.0;
	}
	return *this;
}

CDxf3dPoint& CDxf3dPoint::operator=(const CDxf2dPoint& p)
{
	x = p.x;
	y = p.y;
	z = 0.0;
	return *this;
}

CDxf3dPoint& CDxf3dPoint::operator=(const CDxf2dVector& v)
{
	x = v.x;
	y = v.y;
	z = 0.0;
	return *this;
}

CDxf3dPoint& CDxf3dPoint::operator=(const CDxf3dVector& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

CDxf3dPoint& CDxf3dPoint::operator=(const CDxf3dPoint& pt)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;

	return *this;
}


CDxf3dPoint& CDxf3dPoint::operator*=(double d)
{
	x *= d;
	y *= d;
	z *= d;
	return *this;
}

CDxf3dPoint& CDxf3dPoint::operator/=(double d)
{
	const double one_over_d = 1.0 / d;
	x *= one_over_d;
	y *= one_over_d;
	z *= one_over_d;
	return *this;
}

CDxf3dPoint& CDxf3dPoint::operator+=(const CDxf3dPoint& p)
{
	x += p.x;
	y += p.y;
	z += p.z;
	return *this;
}

CDxf3dPoint& CDxf3dPoint::operator+=(const CDxf3dVector& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

CDxf3dPoint& CDxf3dPoint::operator-=(const CDxf3dPoint& p)
{
	x -= p.x;
	y -= p.y;
	z -= p.z;
	return *this;
}

CDxf3dPoint& CDxf3dPoint::operator-=(const CDxf3dVector& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

CDxf3dPoint CDxf3dPoint::operator*(int i) const
{
	double d = i;
	return CDxf3dPoint(x*d, y*d, z*d);
}

CDxf3dPoint CDxf3dPoint::operator*(float f) const
{
	double d = f;
	return CDxf3dPoint(x*d, y*d, z*d);
}

CDxf3dPoint CDxf3dPoint::operator*(double d) const
{
	return CDxf3dPoint(x*d, y*d, z*d);
}

CDxf3dPoint CDxf3dPoint::operator/(int i) const
{
	const double one_over_d = 1.0 / ((double)i);
	return CDxf3dPoint(x*one_over_d, y*one_over_d, z*one_over_d);
}

CDxf3dPoint CDxf3dPoint::operator/(float f) const
{
	const double one_over_d = 1.0 / ((double)f);
	return CDxf3dPoint(x*one_over_d, y*one_over_d, z*one_over_d);
}

CDxf3dPoint CDxf3dPoint::operator/(double d) const
{
	const double one_over_d = 1.0 / d;
	return CDxf3dPoint(x*one_over_d, y*one_over_d, z*one_over_d);
}

CDxf3dPoint CDxf3dPoint::operator+(const CDxf3dPoint& p) const
{
	return CDxf3dPoint(x + p.x, y + p.y, z + p.z);
}

CDxf3dPoint CDxf3dPoint::operator+(const CDxf3dVector& v) const
{
	return CDxf3dPoint(x + v.x, y + v.y, z + v.z);
}

CDxf3dVector CDxf3dPoint::operator-(const CDxf3dPoint& p) const
{
	return CDxf3dVector(x - p.x, y - p.y, z - p.z);
}

CDxf3dPoint CDxf3dPoint::operator-(const CDxf3dVector& v) const
{
	return CDxf3dPoint(x - v.x, y - v.y, z - v.z);
}

CDxf3dPoint CDxf3dPoint::operator+(const CDxf2dPoint& p) const
{
	return CDxf3dPoint(x + p.x, y + p.y, z);
}

CDxf3dPoint CDxf3dPoint::operator+(const CDxf2dVector& v) const
{
	return CDxf3dPoint(x + v.x, y + v.y, z);
}

CDxf3dVector CDxf3dPoint::operator-(const CDxf2dPoint& p) const
{
	return CDxf3dVector(x - p.x, y - p.y, z);
}

CDxf3dPoint CDxf3dPoint::operator-(const CDxf2dVector& v) const
{
	return CDxf3dPoint(x - v.x, y - v.y, z);
}


double CDxf3dPoint::operator*(const CDxf3dPoint& h) const
{
	return x*h.x + y*h.y + z*h.z;
}

double CDxf3dPoint::operator*(const CDxf3dVector& h) const
{
	return x*h.x + y*h.y + z*h.z;
}


bool CDxf3dPoint::operator==(const CDxf3dPoint& p) const
{
	return (x == p.x&&y == p.y&&z == p.z) ? true : false;
}

bool CDxf3dPoint::operator!=(const CDxf3dPoint& p) const
{
	return (x != p.x || y != p.y || z != p.z) ? true : false;
}

bool CDxf3dPoint::operator<=(const CDxf3dPoint& p) const
{
	// dictionary order
	return ((x<p.x) ? true : ((x == p.x) ? ((y < p.y) ? true : (y == p.y&&z <= p.z) ? true : false) : false));
}

bool CDxf3dPoint::operator>=(const CDxf3dPoint& p) const
{
	// dictionary order
	return ((x > p.x) ? true : ((x == p.x) ? ((y>p.y) ? true : (y == p.y&&z >= p.z) ? true : false) : false));
}

bool CDxf3dPoint::operator<(const CDxf3dPoint& p) const
{
	// dictionary order
	return ((x<p.x) ? true : ((x == p.x) ? ((y < p.y) ? true : (y == p.y&&z < p.z) ? true : false) : false));
}

bool CDxf3dPoint::operator>(const CDxf3dPoint& p) const
{
	// dictionary order
	return ((x > p.x) ? true : ((x == p.x) ? ((y>p.y) ? true : (y == p.y&&z>p.z) ? true : false) : false));
}

double CDxf3dPoint::operator[](int i) const
{
	return ((i <= 0) ? x : ((i >= 2) ? z : y));
}

double& CDxf3dPoint::operator[](int i)
{
	double* pd = (i <= 0) ? &x : ((i >= 2) ? &z : &y);
	return *pd;
}


void CDxf3dPoint::Zero()
{
	x = y = z = 0.0;
}


CDxf2dVector::CDxf2dVector(double xx, double yy)
{
	x = xx; y = yy;
}

CDxf2dVector::operator double*()
{
	return &x;
}

CDxf2dVector::operator const double*() const
{
	return &x;
}

CDxf2dVector& CDxf2dVector::operator*=(double d)
{
	x *= d;
	y *= d;
	return *this;
}

CDxf2dVector& CDxf2dVector::operator/=(double d)
{
	const double one_over_d = 1.0 / d;
	x *= one_over_d;
	y *= one_over_d;
	return *this;
}

double CDxf2dVector::operator*(const CDxf2dPoint& v) const
{
	return (x*v.x + y*v.y);
}

double CDxf2dVector::operator[](int i) const
{
	return ((i <= 0) ? x : y);
}

double& CDxf2dVector::operator[](int i)
{
	double* pd = (i <= 0) ? &x : &y;
	return *pd;
}

double CDxf2dVector::WedgeProduct(const CDxf2dVector& B) const{
	return x*B.y - y*B.x;
}

////////////////////////////////////////////////////////////////
//
// CDxf3dVector
//

// static
const CDxf3dVector& CDxf3dVector::UnitVector(int index)
{
	static CDxf3dVector o(0.0, 0.0, 0.0);
	static CDxf3dVector x(1.0, 0.0, 0.0);
	static CDxf3dVector y(0.0, 1.0, 0.0);
	static CDxf3dVector z(0.0, 0.0, 1.0);
	switch (index)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	}
	return o;
}

CDxf3dVector::CDxf3dVector()
{}

CDxf3dVector::CDxf3dVector(const float* v)
{
	if (v) {
		x = (double)v[0]; y = (double)v[1]; z = (double)v[2];
	}
	else {
		x = y = z = 0.0;
	}
}

CDxf3dVector::CDxf3dVector(const double* v)
{
	if (v) {
		x = v[0]; y = v[1]; z = v[2];
	}
	else {
		x = y = z = 0.0;
	}
}

CDxf3dVector::CDxf3dVector(double xx, double yy, double zz)
{
	x = xx; y = yy; z = zz;
}

CDxf3dVector::CDxf3dVector(const CDxf2dVector& v)
{
	x = v.x; y = v.y; z = 0.0;
}

CDxf3dVector::CDxf3dVector(const CDxf2dPoint& p)
{
	x = p.x; y = p.y; z = 0.0;
}

CDxf3dVector::CDxf3dVector(const CDxf3dPoint& p)
{
	x = p.x; y = p.y; z = p.z;
}

CDxf3dVector::operator double*()
{
	return &x;
}

CDxf3dVector::operator const double*() const
{
	return &x;
}

CDxf3dVector& CDxf3dVector::operator=(const float* v)
{
	if (v) {
		x = (double)v[0];
		y = (double)v[1];
		z = (double)v[2];
	}
	else {
		x = y = z = 0.0;
	}
	return *this;
}

CDxf3dVector& CDxf3dVector::operator=(const double* v)
{
	if (v) {
		x = v[0];
		y = v[1];
		z = v[2];
	}
	else {
		x = y = z = 0.0;
	}
	return *this;
}

CDxf3dVector& CDxf3dVector::operator=(const CDxf2dVector& v)
{
	x = v.x;
	y = v.y;
	z = 0.0;
	return *this;
}

CDxf3dVector& CDxf3dVector::operator=(const CDxf2dPoint& p)
{
	x = p.x;
	y = p.y;
	z = 0.0;
	return *this;
}

CDxf3dVector& CDxf3dVector::operator=(const CDxf3dPoint& p)
{
	x = p.x;
	y = p.y;
	z = p.z;
	return *this;
}

CDxf3dVector CDxf3dVector::operator-() const
{
	return CDxf3dVector(-x, -y, -z);
}

CDxf3dVector& CDxf3dVector::operator*=(double d)
{
	x *= d;
	y *= d;
	z *= d;
	return *this;
}

CDxf3dVector& CDxf3dVector::operator/=(double d)
{
	const double one_over_d = 1.0 / d;
	x *= one_over_d;
	y *= one_over_d;
	z *= one_over_d;
	return *this;
}

CDxf3dVector& CDxf3dVector::operator+=(const CDxf3dVector& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

CDxf3dVector& CDxf3dVector::operator-=(const CDxf3dVector& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

CDxf3dVector CDxf3dVector::operator*(int i) const
{
	double d = i;
	return CDxf3dVector(x*d, y*d, z*d);
}

CDxf3dVector CDxf3dVector::operator*(float f) const
{
	double d = f;
	return CDxf3dVector(x*d, y*d, z*d);
}

CDxf3dVector CDxf3dVector::operator*(double d) const
{
	return CDxf3dVector(x*d, y*d, z*d);
}


CDxf3dVector CDxf3dVector::operator/(int i) const
{
	const double one_over_d = 1.0 / ((double)i);
	return CDxf3dVector(x*one_over_d, y*one_over_d, z*one_over_d);
}

CDxf3dVector CDxf3dVector::operator/(float f) const
{
	const double one_over_d = 1.0 / ((double)f);
	return CDxf3dVector(x*one_over_d, y*one_over_d, z*one_over_d);
}

CDxf3dVector CDxf3dVector::operator/(double d) const
{
	const double one_over_d = 1.0 / d;
	return CDxf3dVector(x*one_over_d, y*one_over_d, z*one_over_d);
}

CDxf3dVector CDxf3dVector::operator+(const CDxf3dVector& v) const
{
	return CDxf3dVector(x + v.x, y + v.y, z + v.z);
}

CDxf3dPoint CDxf3dVector::operator+(const CDxf3dPoint& p) const
{
	return CDxf3dPoint(x + p.x, y + p.y, z + p.z);
}

CDxf3dVector CDxf3dVector::operator-(const CDxf3dVector& v) const
{
	return CDxf3dVector(x - v.x, y - v.y, z - v.z);
}

CDxf3dPoint CDxf3dVector::operator-(const CDxf3dPoint& v) const
{
	return CDxf3dPoint(x - v.x, y - v.y, z - v.z);
}

CDxf3dVector CDxf3dVector::operator+(const CDxf2dVector& v) const
{
	return CDxf3dVector(x + v.x, y + v.y, z);
}

CDxf3dPoint CDxf3dVector::operator+(const CDxf2dPoint& p) const
{
	return CDxf3dPoint(x + p.x, y + p.y, z);
}

CDxf3dVector CDxf3dVector::operator-(const CDxf2dVector& v) const
{
	return CDxf3dVector(x - v.x, y - v.y, z);
}

CDxf3dPoint CDxf3dVector::operator-(const CDxf2dPoint& v) const
{
	return CDxf3dPoint(x - v.x, y - v.y, z);
}

bool CDxf3dVector::operator==(const CDxf3dVector& v) const
{
	return (x == v.x&&y == v.y&&z == v.z) ? true : false;
}

bool CDxf3dVector::operator!=(const CDxf3dVector& v) const
{
	return (x != v.x || y != v.y || z != v.z) ? true : false;
}

bool CDxf3dVector::operator<=(const CDxf3dVector& v) const
{
	// dictionary order
	return ((x<v.x) ? true : ((x == v.x) ? ((y < v.y) ? true : (y == v.y&&z <= v.z) ? true : false) : false));
}

bool CDxf3dVector::operator>=(const CDxf3dVector& v) const
{
	// dictionary order
	return ((x > v.x) ? true : ((x == v.x) ? ((y>v.y) ? true : (y == v.y&&z >= v.z) ? true : false) : false));
}

bool CDxf3dVector::operator<(const CDxf3dVector& v) const
{
	// dictionary order
	return ((x<v.x) ? true : ((x == v.x) ? ((y < v.y) ? true : (y == v.y&&z < v.z) ? true : false) : false));
}

bool CDxf3dVector::operator>(const CDxf3dVector& v) const
{
	// dictionary order
	return ((x > v.x) ? true : ((x == v.x) ? ((y>v.y) ? true : (y == v.y&&z>v.z) ? true : false) : false));
}

double CDxf3dVector::operator[](int i) const
{
	return ((i <= 0) ? x : ((i >= 2) ? z : y));
}

double& CDxf3dVector::operator[](int i)
{
	double* pd = (i <= 0) ? &x : ((i >= 2) ? &z : &y);
	return *pd;
}

void CDxf3dVector::Zero()
{
	x = y = z = 0.0;
}

bool CDxf3dVector::IsTiny(double tiny_tol) const
{
	return (fabs(x) <= tiny_tol && fabs(y) <= tiny_tol && fabs(z) <= tiny_tol);
}

bool CDxf3dVector::IsZero() const
{
	return (x == 0.0 && y == 0.0 && z == 0.0);
}


bool CDxf2dVector::IsValid() const
{
	return (DXF_IS_VALID(x) && DXF_IS_VALID(y)) ? true : false;
}

bool CDxf2dVector::IsUnsetVector() const
{
	return (DXF_UNSET_VALUE == x && DXF_UNSET_VALUE == y);
}

bool CDxf3dVector::IsValid() const
{
	return (DXF_IS_VALID(x) && DXF_IS_VALID(y) && DXF_IS_VALID(z)) ? true : false;
}

bool CDxf3dVector::IsUnsetVector() const
{
	return (DXF_UNSET_VALUE == x && DXF_UNSET_VALUE == y && DXF_UNSET_VALUE == z);
}

bool CDxf2dPoint::IsValid() const
{
	return (DXF_IS_VALID(x) && DXF_IS_VALID(y)) ? true : false;
}

bool CDxf2dPoint::IsUnsetPoint() const
{
	return (DXF_UNSET_VALUE == x && DXF_UNSET_VALUE == y);
}

bool CDxf3dPoint::IsValid() const
{
	return (DXF_IS_VALID(x) && DXF_IS_VALID(y) && DXF_IS_VALID(z)) ? true : false;
}

void CDxf2dPoint::Set(double xx, double yy)
{
	x = xx; y = yy;
}

void CDxf3dPoint::Set(double xx, double yy, double zz)
{
	x = xx; y = yy; z = zz;
}

void CDxf2dVector::Set(double xx, double yy)
{
	x = xx; y = yy;
}

void CDxf3dVector::Set(double xx, double yy, double zz)
{
	x = xx; y = yy; z = zz;
}


