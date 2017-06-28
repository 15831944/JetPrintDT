#ifndef __DXF_POINT_H
#define __DXF_POINT_H

#include "JetDxfDef.h"
#include "DXFMath.h"

class CDxf2dPoint;
class CDxf3dPoint;

class CDxf2dVector;
class CDxf3dVector;

class CDxfInterval;

////////////////////////////////////////////////////////////////
//
//   CDxfInterval
//
class  CDxfInterval
{
public:

	static const CDxfInterval EmptyInterval; // (DXF_UNSET_VALUE,DXF_UNSET_VALUE)

	////////
	// The default constructor creates an empty interval (DXF_UNSET_VALUE,DXF_UNSET_VALUE)
	CDxfInterval();

	CDxfInterval(double t0, double t1);

	~CDxfInterval();

	bool operator!=(const CDxfInterval&) const;
	bool operator==(const CDxfInterval&) const;

	// Interval = [m_t[0], m_t[1]]
	double m_t[2];

	/*
	Description:
	Sets interval to (ON_UNSET_VALUE,ON_UNSET_VALUE)
	See Also:
	CDxfInterval::Set
	*/
	void Destroy();

	/*
	Description:
	Sets interval to [t0,t1]
	Parameters:
	t0 - [in]
	t1 - [in]
	See Also:
	CDxfInterval::CDxfInterval( double, double )
	*/
	void Set(
		double t0,
		double t1
		);

	/*
	Description:
	Convert normalized parameter to interval value, or pair of values.
	Parameters:
	normalized_parameter - [in]
	Returns:
	Interval parameter
	min*(1.0-normalized_parameter) + max*normalized_parameter
	See Also:
	CDxfInterval::NormalizedParameterAt
	*/
	double ParameterAt(
		double normalized_parameter
		) const;
	CDxfInterval ParameterAt(
		CDxfInterval normalized_interval
		) const;

	/*
	Description:
	Convert interval value, or pair of values, to normalized parameter.
	Parameters:
	interval_parameter - [in] value in interval
	Returns:
	Normalized parameter x so that
	min*(1.0-x) + max*x = interval_parameter.
	See Also:
	CDxfInterval::ParameterAt
	*/
	double NormalizedParameterAt(
		double interval_parameter
		) const;
	CDxfInterval NormalizedParameterAt(
		CDxfInterval interval_parameter
		) const;

	double& operator[](int); // returns (index<=0) ? m_t[0] : m_t[1]
	double operator[](int) const; // returns (index<=0) ? m_t[0] : m_t[1]

	double Min() const; // returns smaller of m_t[0] and m_t[1]
	double Max() const; // returns larger of m_t[0] and m_t[1]
	double Mid() const; // returns 0.5*(m_t[0] + m_t[1])
	double Length() const;

	bool IsIncreasing() const; // returns true if m_t[0] < m_t[1]
	bool IsDecreasing() const; // returns true if m_t[0] > m_t[0];
	bool IsInterval() const;   // returns truc if m_t[0] != m_t[1]
	bool IsSingleton() const;  // returns true if m_t[0] == m_t[1] != ON_UNSET_VALUE
	bool IsEmptyInterval() const;   // returns true if m_t[0] == m_t[1] == ON_UNSET_VALUE
	bool IsValid() const;      // returns ON_IsValid(m_t[0]) && ON_IsValid(m_t[1])

	// OBSOLETE - Use IsEmptyInterval()
	bool IsEmptySet() const;   // returns true if m_t[0] == m_t[1] == ON_UNSET_VALUE

	bool MakeIncreasing();		// returns true if resulting interval IsIncreasing() 

	/*
	Returns:
	@untitled table
	0      this is idential to other
	-1      this[0] < other[0]
	+1      this[0] > other[0]
	-1      this[0] == other[0] and this[1] < other[1]
	+1      this[0] == other[0] and this[1] > other[1]
	*/
	int Compare(const CDxfInterval& other) const;

	/*
	Description:
	Test a value t to see if it is inside the interval.
	Parameters:
	t - [in] value to test
	bTestOpenInterval - [in]
	If false, t is tested to see if it satisfies min <= t <= max.
	If true, t is tested to see if it satisfies min < t < max.
	Returns:
	true if t is in the interval and false if t is not
	in the interval.
	*/
	bool Includes(
		double t,
		bool bTestOpenInterval = false
		) const;

	/*
	Description:
	Test an interval to see if it is contained in this interval.
	Parameters:
	other - [in] interval to test
	bProperSubSet - [in] if true, then the test is for a proper subinterval.
	Returns:
	If bProperSubSet is false, then the result is true when
	this->Min() <= other.Min() and other.Max() <= this->Max().
	If bProperSubSet is true, then the result is true when
	this->Min() <= other.Min() and other.Max() <= this->Max()
	and at least one of the inequalites is strict.
	*/
	bool Includes(
		const CDxfInterval& other,
		bool bProperSubSet = false
		) const;

	/*
	Description:
	Changes interval to [-m_t[1],-m_t[0]].
	*/
	void Reverse();

	/*
	Description:
	Swaps m_t[0] and m_t[1].
	*/
	void Swap();

};

////////////////////////////////////////////////////////////////
//
//   CDxf2dPoint
//
class  CDxf2dPoint
{
public:
	double x, y;

	static const CDxf2dPoint Origin;     // (0.0,0.0)
	static const CDxf2dPoint UnsetPoint; // (DXF_UNSET_VALUE,DXF_UNSET_VALUE)

	// use implicit destructor, copy constructor
	CDxf2dPoint();                         // x,y not initialized
	CDxf2dPoint(double x, double y);
	CDxf2dPoint(const CDxf2dPoint&);       //from 2d point
	CDxf2dPoint(const CDxf3dPoint&);       // from 3d point
	CDxf2dPoint(const CDxf2dVector&);      // from 2d vector
	CDxf2dPoint(const CDxf3dVector&);      // from 3d vector
	CDxf2dPoint(const double*);            // from double[2] array

	CDxf2dPoint(const float*);             // from float[2] array

	// (double*) conversion operators
	operator double*();
	operator const double*() const;

	// use implicit operator=(const CDxf2dPoint&)
	CDxf2dPoint& operator=(const CDxf2dPoint&);
	CDxf2dPoint& operator=(const CDxf3dPoint&);
	CDxf2dPoint& operator=(const CDxf2dVector&);
	CDxf2dPoint& operator=(const CDxf3dVector&);
	CDxf2dPoint& operator=(const double*); // point = double[2] support
	CDxf2dPoint& operator=(const float*);  // point = float[2] support

	CDxf2dPoint& operator*=(double);
	CDxf2dPoint& operator/=(double);
	CDxf2dPoint& operator+=(const CDxf2dPoint&);  // Adding this was a mistake - cannot remove without breaking SDK
	CDxf2dPoint& operator+=(const CDxf2dVector&);
	CDxf2dPoint& operator+=(const CDxf3dVector&); // Adding this was a mistake - cannot remove without breaking SDK
	CDxf2dPoint& operator-=(const CDxf2dPoint&);  // Adding this was a mistake - cannot remove without breaking SDK
	CDxf2dPoint& operator-=(const CDxf2dVector&);
	CDxf2dPoint& operator-=(const CDxf3dVector&); // Adding this was a mistake - cannot remove without breaking SDK

	CDxf2dPoint  operator*(int) const;
	CDxf2dPoint  operator/(int) const;
	CDxf2dPoint  operator*(float) const;
	CDxf2dPoint  operator/(float) const;
	CDxf2dPoint  operator*(double) const;
	CDxf2dPoint  operator/(double) const;

	CDxf2dPoint  operator+(const CDxf2dPoint&) const;
	CDxf2dPoint  operator+(const CDxf2dVector&) const;
	CDxf2dVector operator-(const CDxf2dPoint&) const;
	CDxf2dPoint  operator-(const CDxf2dVector&) const;
	CDxf3dPoint  operator+(const CDxf3dPoint&) const;
	CDxf3dPoint  operator+(const CDxf3dVector&) const;
	CDxf3dVector operator-(const CDxf3dPoint&) const;
	CDxf3dPoint  operator-(const CDxf3dVector&) const;


	double operator*(const CDxf2dPoint&) const; // dot product for points acting as vectors
	double operator*(const CDxf2dVector&) const; // dot product for points acting as vectors

	bool operator==(const CDxf2dPoint&) const;
	bool operator!=(const CDxf2dPoint&) const;

	// dictionary order comparisons
	bool operator<=(const CDxf2dPoint&) const;
	bool operator>=(const CDxf2dPoint&) const;
	bool operator<(const CDxf2dPoint&) const;
	bool operator>(const CDxf2dPoint&) const;

	// index operators mimic double[2] behavior
	double& operator[](int);
	double operator[](int) const;

	/*
	Returns:
	False if any coordinate is infinte, a nan, or DXF_UNSET_VALUE.
	*/
	bool IsValid() const;

	/*
	Returns:
	True if every coordinate is DXF_UNSET_VALUE.
	*/
	bool IsUnsetPoint() const;

	// set 2d point value
	void Set(double x, double y);

	double DistanceTo(const CDxf2dPoint&) const;

	int MaximumCoordinateIndex() const;
	double MaximumCoordinate() const; // absolute value of maximum coordinate

	int MinimumCoordinateIndex() const;
	double MinimumCoordinate() const; // absolute value of minimum coordinate

	void Zero(); // set all coordinates to zero;
};


////////////////////////////////////////////////////////////////
//
//   CDxf3dPoint
//
class  CDxf3dPoint
{
public:
	double x, y, z;

	static const CDxf3dPoint Origin;     // (0.0,0.0,0.0)
	static const CDxf3dPoint UnsetPoint; // (DXF_UNSET_VALUE,DXF_UNSET_VALUE,DXF_UNSET_VALUE)

	// use implicit destructor, copy constructor
	CDxf3dPoint();                         // x,y,z not initialized
	CDxf3dPoint(double x, double y, double z);
	CDxf3dPoint(const CDxf2dPoint&);       // from 2d point
	CDxf3dPoint(const CDxf2dVector&);      // from 2d vector
	CDxf3dPoint(const CDxf3dVector&);      // from 3d vector
	CDxf3dPoint(const double*);            // from double[3] array

	CDxf3dPoint(const float*);             // from float[3] array

	// (double*) conversion operators
	operator double*();
	operator const double*() const;

	// use implicit operator=(const CDxf3dPoint&)
	CDxf3dPoint& operator=(const CDxf2dPoint&);
	CDxf3dPoint& operator=(const CDxf2dVector&);
	CDxf3dPoint& operator=(const CDxf3dVector&);
	CDxf3dPoint& operator=(const CDxf3dPoint&);
	CDxf3dPoint& operator=(const double*); // point = double[3] support
	CDxf3dPoint& operator=(const float*);  // point = float[3] support

	CDxf3dPoint& operator*=(double);
	CDxf3dPoint& operator/=(double);
	CDxf3dPoint& operator+=(const CDxf3dPoint&);  // Adding this was a mistake - cannot remove without breaking SDK
	CDxf3dPoint& operator+=(const CDxf3dVector&);
	CDxf3dPoint& operator-=(const CDxf3dPoint&);  // Adding this was a mistake - cannot remove without breaking SDK
	CDxf3dPoint& operator-=(const CDxf3dVector&);

	CDxf3dPoint  operator*(int) const;
	CDxf3dPoint  operator/(int) const;
	CDxf3dPoint  operator*(float) const;
	CDxf3dPoint  operator/(float) const;
	CDxf3dPoint  operator*(double) const;
	CDxf3dPoint  operator/(double) const;

	CDxf3dPoint  operator+(const CDxf3dPoint&) const;
	CDxf3dPoint  operator+(const CDxf3dVector&) const;
	CDxf3dVector operator-(const CDxf3dPoint&) const;
	CDxf3dPoint  operator-(const CDxf3dVector&) const;
	CDxf3dPoint  operator+(const CDxf2dPoint&) const;
	CDxf3dPoint  operator+(const CDxf2dVector&) const;
	CDxf3dVector operator-(const CDxf2dPoint&) const;
	CDxf3dPoint  operator-(const CDxf2dVector&) const;

	double operator*(const CDxf3dPoint&) const; // dot product for points acting as vectors
	double operator*(const CDxf3dVector&) const; // dot product for points acting as vectors

	bool operator==(const CDxf3dPoint&) const;
	bool operator!=(const CDxf3dPoint&) const;

	// dictionary order comparisons
	bool operator<=(const CDxf3dPoint&) const;
	bool operator>=(const CDxf3dPoint&) const;
	bool operator<(const CDxf3dPoint&) const;
	bool operator>(const CDxf3dPoint&) const;

	// index operators mimic double[3] behavior
	double& operator[](int);
	double operator[](int) const;

	/*
	Returns:
	False if any coordinate is infinte, a nan, or ON_UNSET_VALUE.
	*/
	bool IsValid() const;


	// set 3d point value
	void Set(double x, double y, double z);


	void Zero(); // set all coordinates to zero;

};

////////////////////////////////////////////////////////////////
//
//   CDxf2dVector
//
class CDxf2dVector
{
public:
	double x, y;

	static const CDxf2dVector ZeroVector;  // (0.0,0.0)
	static const CDxf2dVector XAxis;       // (1.0,0.0)
	static const CDxf2dVector YAxis;       // (0.0,1.0)
	static const CDxf2dVector UnsetVector; // (ON_UNSET_VALUE,ON_UNSET_VALUE)

	// Description:
	//   A index driven function to get unit axis vectors.
	// Parameters:
	//   index - [in] 0 returns (1,0), 1 returns (0,1)
	// Returns:
	//   Unit 2d vector with vector[i] = (i==index)?1:0;
	static const CDxf2dVector& UnitVector(
		int // index
		);

	// use implicit destructor, copy constructor
	CDxf2dVector();                     // x,y not initialized
	CDxf2dVector(double x, double y);

	CDxf2dVector(const CDxf2dVector&);
	CDxf2dVector(const CDxf3dVector&); // from 3d vector
	CDxf2dVector(const CDxf2dPoint&);  // from 2d point
	CDxf2dVector(const CDxf3dPoint&);  // from 3d point
	CDxf2dVector(const double*);       // from double[2] array
	CDxf2dVector(const float*);        // from double[2] array

	// (double*) conversion operators
	operator double*();
	operator const double*() const;

	// use implicit operator=(const CDxf2dVector&)
	CDxf2dVector& operator=(const CDxf2dVector&);
	CDxf2dVector& operator=(const CDxf3dVector&);
	CDxf2dVector& operator=(const CDxf2dPoint&);
	CDxf2dVector& operator=(const CDxf3dPoint&);
	CDxf2dVector& operator=(const double*); // vector = double[2] support
	CDxf2dVector& operator=(const float*);  // vector = float[2] support

	CDxf2dVector  operator-() const;

	CDxf2dVector& operator*=(double);
	CDxf2dVector& operator/=(double);
	CDxf2dVector& operator+=(const CDxf2dVector&);
	CDxf2dVector& operator-=(const CDxf2dVector&);
	// DO NOT ADD ANY MORE overrides of += or -=

	double operator*(const CDxf2dVector&) const; // inner (dot) product
	double operator*(const CDxf2dPoint&) const; // inner (dot) product (point acting as vector)
	
	CDxf2dVector  operator*(int) const;
	CDxf2dVector  operator/(int) const;
	CDxf2dVector  operator*(float) const;
	CDxf2dVector  operator/(float) const;
	CDxf2dVector  operator*(double) const;
	CDxf2dVector  operator/(double) const;

	CDxf2dVector  operator+(const CDxf2dVector&) const;
	CDxf2dPoint   operator+(const CDxf2dPoint&) const;
	CDxf2dVector  operator-(const CDxf2dVector&) const;
	CDxf2dPoint   operator-(const CDxf2dPoint&) const;
	CDxf3dVector  operator+(const CDxf3dVector&) const;
	CDxf3dPoint   operator+(const CDxf3dPoint&) const;
	CDxf3dVector  operator-(const CDxf3dVector&) const;
	CDxf3dPoint   operator-(const CDxf3dPoint&) const;

	bool operator==(const CDxf2dVector&) const;
	bool operator!=(const CDxf2dVector&) const;

	// dictionary order comparisons
	bool operator<=(const CDxf2dVector&) const;
	bool operator>=(const CDxf2dVector&) const;
	bool operator<(const CDxf2dVector&) const;
	bool operator>(const CDxf2dVector&) const;

	// index operators mimic double[2] behavior
	double& operator[](int);
	double operator[](int) const;

	/*
	Returns:
	False if any coordinate is infinte, a nan, or ON_UNSET_VALUE.
	*/
	bool IsValid() const;

	/*
	Returns:
	True if every coordinate is ON_UNSET_VALUE.
	*/
	bool IsUnsetVector() const;

	// set 2d vector value
	void Set(double x, double y);


	// Signed area of the parallelagram.  The volume element.
	// returns x*B.y - y*B.x
	double WedgeProduct(const CDxf2dVector& B) const;


	int IsParallelTo(
		// returns  1: this and other vectors are parallel
		//         -1: this and other vectors are anti-parallel
		//          0: this and other vectors are not parallel
		//             or at least one of the vectors is zero
		const CDxf2dVector& other,                           // other vector     
		double angle_tolerance = DXF_DEFAULT_ANGLE_TOLERANCE // optional angle tolerance (radians)
		) const;

	bool IsPerpendicularTo(
		// returns true:  this and other vectors are perpendicular
		//         false: this and other vectors are not perpendicular
		//                or at least one of the vectors is zero
		const CDxf2dVector& other,                           // other vector     
		double angle_tolerance = DXF_DEFAULT_ANGLE_TOLERANCE // optional angle tolerance (radians)
		) const;

	void Zero(); // set all coordinates to zero;
	void Reverse(); // negate all coordinates

	// Returns:
	//   true if vector is the zero vector.
	bool IsZero() const;

};


////////////////////////////////////////////////////////////////
//
//   CDxf3dVector
//
class  CDxf3dVector
{
public:
	double x, y, z;

	static const CDxf3dVector ZeroVector;  // (0.0,0.0,0.0)
	static const CDxf3dVector XAxis;       // (1.0,0.0,0.0)
	static const CDxf3dVector YAxis;       // (0.0,1.0,0.0)
	static const CDxf3dVector ZAxis;       // (0.0,0.0,1.0)
	static const CDxf3dVector UnsetVector; // (ON_UNSET_VALUE,ON_UNSET_VALUE,ON_UNSET_VALUE)

	// Description:
	//   A index driven function to get unit axis vectors.
	// Parameters:
	//   index - [in] 0 returns (1,0,0), 1 returns (0,1,0), 
	//                2 returns (0,0,1)
	// Returns:
	//   Unit 3d vector with vector[i] = (i==index)?1:0;
	static const CDxf3dVector& UnitVector(
		int // index
		);

	// use implicit destructor, copy constructor
	CDxf3dVector();                     // x,y,z not initialized
	CDxf3dVector(double x, double y, double z);
	CDxf3dVector(const CDxf2dVector&);  // from 2d vector
	CDxf3dVector(const CDxf2dPoint&);   // from 2d point
	CDxf3dVector(const CDxf3dPoint&);   // from 3d point
	CDxf3dVector(const double*);        // from double[3] array
	CDxf3dVector(const float*);         // from float[3] array

	// (double*) conversion operators
	operator double*();
	operator const double*() const;

	// use implicit operator=(const CDxf3dVector&)
	CDxf3dVector& operator=(const CDxf2dVector&);
	CDxf3dVector& operator=(const CDxf2dPoint&);
	CDxf3dVector& operator=(const CDxf3dPoint&);
	CDxf3dVector& operator=(const double*); // vector = double[3] support
	CDxf3dVector& operator=(const float*);  // vector = float[3] support

	CDxf3dVector  operator-() const;

	CDxf3dVector& operator*=(double);
	CDxf3dVector& operator/=(double);
	CDxf3dVector& operator+=(const CDxf3dVector&);
	CDxf3dVector& operator-=(const CDxf3dVector&);
	// DO NOT ADD ANY MORE overrides of += or -=

	CDxf3dVector  operator*(int) const;
	CDxf3dVector  operator/(int) const;
	CDxf3dVector  operator*(float) const;
	CDxf3dVector  operator/(float) const;
	CDxf3dVector  operator*(double) const;
	CDxf3dVector  operator/(double) const;

	CDxf3dVector  operator+(const CDxf3dVector&) const;
	CDxf3dPoint   operator+(const CDxf3dPoint&) const;
	CDxf3dVector  operator-(const CDxf3dVector&) const;
	CDxf3dPoint   operator-(const CDxf3dPoint&) const;
	CDxf3dVector  operator+(const CDxf2dVector&) const;
	CDxf3dPoint   operator+(const CDxf2dPoint&) const;
	CDxf3dVector  operator-(const CDxf2dVector&) const;
	CDxf3dPoint   operator-(const CDxf2dPoint&) const;

	bool operator==(const CDxf3dVector&) const;
	bool operator!=(const CDxf3dVector&) const;

	// dictionary order comparisons
	bool operator<=(const CDxf3dVector&) const;
	bool operator>=(const CDxf3dVector&) const;
	bool operator<(const CDxf3dVector&) const;
	bool operator>(const CDxf3dVector&) const;

	// index operators mimic double[3] behavior
	double& operator[](int);
	double operator[](int) const;

	/*
	Returns:
	False if any coordinate is infinte, a nan, or ON_UNSET_VALUE.
	*/
	bool IsValid() const;

	/*
	Returns:
	True if every coordinate is ON_UNSET_VALUE.
	*/
	bool IsUnsetVector() const;

	// set 3d vector value
	void Set(double x, double y, double z);


	void Zero(); // set all coordinates to zero;

	// Description:
	//   Test a vector to see if it is very short
	//
	// Parameters:
	//   tiny_tol - [in] (default = ON_ZERO_TOLERANCE) a nonzero
	//              value used as the coordinate zero tolerance.
	//
	// Returns:
	//   ( fabs(x) <= tiny_tol && fabs(y) <= tiny_tol && fabs(z) <= tiny_tol )
	//
	bool IsTiny(
		double tiny_tol = DXF_ZERO_TOLERANCE // tiny_tol
		) const;

	// Returns:
	//   true if vector is the zero vector.
	bool IsZero() const;
};

const CDxf3dPoint  DXF_UNSET_POINT(DXF_UNSET_VALUE, DXF_UNSET_VALUE, DXF_UNSET_VALUE);

#endif

