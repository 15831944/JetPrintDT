#ifndef __JET_DXFNURBS_H
#define __JET_DXFNURBS_H

#include "JetDXFDef.h"
#include "DXFPoint.h"
#include "DXFNurbsKnot.h"

class CJetDxfNurbsCurve
{
	//private members
private:

	int     m_dim;            // (>=1)

	int     m_is_rat;         // 1 for rational B-splines. (Rational control vertices use homogeneous form.)
	// 0 for non-rational B-splines. (Control vertices do not have a weight coordinate.)

	int     m_order;          // order = degree+1 (>=2)

	int     m_cv_count;       // number of control vertices ( >= order )
	// knot vector memory


	int     m_knot_capacity;  // If m_knot_capacity > 0, then m_knot[] is an array of at least m_knot_capacity doubles whose memory is managed by the
	// CDxfNurbsCurve class using rhmalloc(),onrealloc(), and rhfree().If m_knot_capacity is 0 and m_knot is not NULL, then  
	// m_knot[] is assumed to be big enough for any requested operation and m_knot[] is not deleted by the destructor.

	double* m_knot;           // Knot vector. ( The knot vector has length m_order+m_cv_count-2. 
	//control vertex net memory 

	int     m_cv_stride;      // The pointer to start of "CV[i]" is m_cv + i*m_cv_stride.

	int     m_cv_capacity;    // If m_cv_capacity > 0, then m_cv[] is an array of at least m_cv_capacity doubles whose
	// memory is managed by the CDxfNurbsCurve class using rhmalloc(), onrealloc(), and rhfree().
	// If m_cv_capacity is 0 and m_cv is not NULL, then m_cv[] is assumed to be big enough for any 
	//requested operation and m_cv[] is not deleted by the destructor.


	double* m_cv;             // Control points. If m_is_rat is false, then control point is ( CV(i)[0], ..., CV(i)[m_dim-1] ).
	// If m_is_rat is true, then the control point is stored in HOMOGENEOUS form and is [ CV(i)[0], ..., CV(i)[m_dim] ].

	//public members:
public:

	//private member funcs
private:
	// Description:
	//   Zeros all fields.
	void Initialize(void);

	//public member funcs
public:
	/*
	Description:
	Use CDxfNurbsCurve::New(...) instead of new CDxfNurbsCurve(...)
	Returns:
	Pointer to an CDxfNurbsCurve.  Destroy by calling delete.
	*/
	static CJetDxfNurbsCurve* New();
	static CJetDxfNurbsCurve* New(
		const CJetDxfNurbsCurve& nurbs_curve
		);
	static CJetDxfNurbsCurve* New(
		int dimension,
		bool bIsRational,
		int order,
		int cv_count
		);

	CJetDxfNurbsCurve();
	CJetDxfNurbsCurve(const CJetDxfNurbsCurve&);

	// Description:
	//   Create a NURBS curve with knot a cv memory allocated.
	// Parameters:
	//   dimension - [in] (>= 1)
	//   bIsRational - [in] true to make a rational NURBS
	//   order - [in] (>= 2) The order=degree+1
	//   cv_count - [in] (>= order) number of control vertices
	CJetDxfNurbsCurve(
		int dimension,
		bool bIsRational,
		int order,
		int cv_count
		);

	// Description:
	//   Create a NURBS curve with knot a cv memory allocated.
	// Parameters:
	//   dimension - [in] (>= 1)
	//   bIsRational - [in] true to make a rational NURBS
	//   order - [in] (>= 2) The order=degree+1
	//   cv_count - [in] (>= order) number of control vertices
	bool Create(
		int dimension,
		bool bIsRational,
		int order,
		int cv_count
		);

	virtual ~CJetDxfNurbsCurve();

	// virtual ON_Object::SizeOf override
	unsigned int SizeOf() const;


	// Description:
	//   Create a clamped uniform NURBS curve from a list of control points
	// Parameters:
	//   dimension - [in] 1, 2 or 3
	//   order - [in] (>=2) order=degree+1
	//   point_count - [in] (>=order) number of control vertices
	//   point - [in] array of control vertex locations.
	//   knot_delta - [in] (>0.0) knot spacing
	// Returns:
	//   true if successful
	bool CreateClampedUniformNurbs(
		int dimension,
		int order,
		int point_count,
		const CDxf3dPoint* point,
		double knot_delta = 1.0
		);


	// Description:
	//   Create a periodic uniform NURBS curve from a list of control points
	// Parameters:
	//   dimension - [in] 1, 2 or 3
	//   order - [in] (>=2) order=degree+1
	//   point_count - [in] (>=max(3,order-1)) number of distinct control vertices
	//   point - [in] array of distinct control vertex locations.
	//   knot_delta - [in] (>0.0) knot spacing
	// Returns:
	//   true if successful
	bool CreatePeriodicUniformNurbs(
		int dimension,
		int order,
		int point_count,
		const CDxf3dPoint* point,
		double knot_delta = 1.0
		);



	// Description:
	//   Deallocate knot and cv memory.  Zeros all fields.
	void Destroy();


	// Description:
	// GetDimension();
	// Returns:
	//   value of m_dim
	int Dimension() const;

	// Description:
	// GetDomain()
	// Returns:
	//   domain of the NURBS curve.
	CDxfInterval Domain() const;

	// Description:
	//   Set the domain of the curve
	// Parameters:
	//   t0 - [in]
	//   t1 - [in] new domain will be [t0,t1]
	// Returns:
	//   true if successful.
	bool SetDomain(
		double t0,
		double t1
		);

	// Description:
	//   Get number of nonempty smooth (c-infinity) spans in curve
	// Returns:
	//   Number of nonempty smooth (c-infinity) spans.
	// Remarks:
	//   A nonempty span is bracked by knots m_knot[i] < m_knot[i+1]
	//   with m_order-2 <= i < m_cv_count-1.
	int SpanCount() const;

	// Description:
	//   Get number of parameters of distinct knots in NURBS curve's domain.
	// Parameters:
	//   knot_values - [out] an array of length SpanCount()+1 is 
	//       filled in with the distinct knot values in the list
	///      (m_knot[m_order-2],...,m_knot[m_cv_count-1)
	// Returns:
	//   true if successful
	bool GetSpanVector(
		double* knot_values
		) const;

	// Description:
	// Get Nurbs Degree
	// Returns:
	//   m_order-1
	int Degree() const;


	// Description:
	//   Evaluate point at a parameter.
	// Parameters:
	//   t - [in] evaluation parameter
	// Returns:
	//   Point (location of curve at the parameter t).
	// Remarks:
	//   No error handling.
	CDxf3dPoint PointAt(double t);

	// Description:
	//   Evaluate point at a parameter with error checking.
	// Parameters:
	//   t - [in] evaluation parameter
	//   point - [out] value of curve at t
	//   side - [in] optional - determines which side to evaluate from
	//               =0   default
	//               <0   to evaluate from below, 
	//               >0   to evaluate from above
	//   hint - [in/out] optional evaluation hint used to speed repeated evaluations
	// Returns:
	//   false if unable to evaluate.
	bool EvPoint(double t, CDxf3dPoint& point, int side = 0, int* hint = 0);

	// Description:
	//   Evaluate point at the start of the curve.
	// Parameters:
	//   t - [in] evaluation parameter
	// Returns:
	//   Point (location of the start of the curve.)
	// Remarks:
	//   No error handling.
	// See Also:
	//   CDxfCurve::PointAt
	CDxf3dPoint  PointAtStart();

	// Description:
	//   Evaluate point at the end of the curve.
	// Parameters:
	//   t - [in] evaluation parameter
	// Returns:
	//   Point (location of the end of the curve.)
	// Remarks:
	//   No error handling.
	CDxf3dPoint  PointAtEnd();

	// Description:
	// Evalute a Nurbs Curve
	bool Evaluate( // returns false if unable to evaluate
		double,         // evaluation parameter
		int,            // number of derivatives (>=0)
		int,            // array stride (>=Dimension())
		double*,        // array of length stride*(ndir+1)
		int = 0,        // optional - determines which side to evaluate from
		//         0 = default
		//      <  0 to evaluate from below, 
		//      >  0 to evaluate from above
		int* = 0        // optional - evaluation hint (int) used to speed
		//            repeated evaluations
		);

	// Description:
	//   Get the length of the curve.
	// Parameters:
	//   length - [out] length returned here.
	//   t - [out] parameter such that the length of the curve from its start to t is arc_length.
	//   fractional_tolerance - [in] desired fractional precision.  fabs(("exact" length from start to t) - arc_length)/arc_length <= fractional_tolerance
	//   sub_domain - [in] If not NULL, the calculation is performed on the specified sub-domain of the curve.
	// Returns:
	//   true if returned if the length calculation is successful.
	//   false is returned if the length is not calculated.
	// Remarks:
	//   The arc length will be computed so that (returned length - real length)/(real length) <= fractional_tolerance More simply, if you want N significant figures in the answer, set the
	//   fractional_tolerance to 1.0e-N.  For "nice" curves, 1.0e-8 works fine.  For very high degree NURBS and NURBS with bad parameterizations,use larger values of fractional_tolerance.
	bool GetLength(
		double* length,
		double fractional_tolerance = 1.0e-8,
		const CDxfInterval* sub_domain = NULL
		) const;

	bool ReserveCVCapacity(
		int // number of doubles to reserve
		);

	bool ReserveKnotCapacity(
		int // number of doubles to reserve
		);

	// Description:
	//   Clamp end knots.  Does not modify control points.
	// Parameters:
	//   end - [in] 0 = clamp start, 1 = clamp end, 2 = clamp start and end
	// Returns:
	//   true if successful
	bool ClampEnd(
		int end
		);

	// Description:
	//   virtual ON_Curve::HasNurbForm override.
	int HasNurbForm( // returns 0: unable to create NURBS representation
		//            with desired accuracy.
		//         1: success - returned NURBS parameterization
		//            matches the curve's to wthe desired accuracy
		//         2: success - returned NURBS point locus matches
		//            the curve's to the desired accuracy but, on
		//            the interior of the curve's domain, the 
		//            curve's parameterization and the NURBS
		//            parameterization may not match to the 
		//            desired accuracy.
		) const;

	bool GetCurveParameterFromNurbFormParameter(
		double, // nurbs_t
		double* // curve_t
		) const;


	bool GetNurbFormParameterFromCurveParameter(
		double, // curve_t
		double* // nurbs_t
		) const;

	/////////////////////////////////////////////////////////////////
	// Interface

	bool IsRational(  // true if NURBS curve is rational
		void
		) const;

	int CVSize(       // number of doubles per control vertex 
		void        // = IsRational() ? Dim()+1 : Dim()
		) const;

	int Order(        // order = degree + 1
		void
		) const;

	int CVCount(      // number of control vertices
		void
		) const;

	int KnotCount(    // total number of knots in knot vector
		void
		) const;

	/*
	Description:
	Expert user function to get a pointer to control vertex memory.  If you are not an expert user, please use
	CDxfNurbsCurve::GetCV( CDxf3dPoint& ) or CDxfNurbsCurve::GetCV( CDxf4dPoint& ).
	Parameters:
	cv_index - [in]
	Returns:
	Pointer to control vertex.
	Remarks:
	If the NURBS curve is rational, the format of the returned array is a homogeneos rational point with length m_dim+1.  If the NURBS curve is not rational,
	the format of the returned array is a nonrational euclidean point with length m_dim.
	*/
	double* CV(
		int cv_index
		) const;

	double Weight(        // get value of control vertex weight
		int             // CV index ( >= 0 and < CVCount() )
		) const;

	bool SetWeight(      // get value of control vertex weight
		int,            // CV index ( >= 0 and < CVCount() )
		double
		);

	bool SetCV(              // set a single control vertex
		int,              // CV index ( >= 0 and < CVCount() )
		CDXF::point_style, // style of input point
		const double*     // value of control vertex
		);

	bool SetCV(               // set a single control vertex
		int,               // CV index ( >= 0 and < CVCount() )
		const CDxf3dPoint& // value of control vertex
		// If NURBS is rational, weight
		// will be set to 1.
		);

	bool GetCV(              // get a single control vertex
		int,              // CV index ( >= 0 and < CVCount() )
		CDXF::point_style, // style to use for output point
		double*           // array of length >= CVSize()
		) const;

	bool GetCV(              // get a single control vertex
		int,              // CV index ( >= 0 and < CVCount() )
		CDxf3dPoint&      // gets euclidean cv when NURBS is rational
		) const;

	// Description:
	//   Set knot value.
	// Parameters:
	//   knot_index - [in] 0 <= knot_index <= KnotCount()-1
	//   knot_value - [in]
	// Remarks:
	//   m_knot[] must exist.  Use ReserveKnotCapacity to allocate m_knot[]. 
	// Returns:
	//   true if successful

	bool SetKnot(
		int knot_index,
		double knot_value
		);

	// Description:
	//   Get knot value.
	// Parameters:
	//   knot_index - [in] 0 <= knot_index <= KnotCount()-1
	// Returns:
	//   knot value = m_knot[knot_index]
	double Knot(
		int knot_index
		) const;

	// Description:
	//   Get knot multiplicity.
	// Parameters:
	//   knot_index - [in] 0 <= knot_index <= KnotCount()-1
	// Returns:
	//   knot multiplicity = m_knot[knot_index]
	int KnotMultiplicity(
		int knot_index
		) const;

	// Description:
	//   Get pointer to knot vector array.
	// Returns:
	//   pointer to knot vector array (m_knot).
	const double* Knot() const;

	// Description:
	//   Make knot vector a clamped uniform knot vector based on the current values of m_order and m_cv_count. Does not change values of control vertices.
	// Parameters:
	//   delta - [in] (>0.0) knot spacing.
	// Returns:
	//   true if successful.
	// Remarks:
	//   Allocates m_knot[] if it is not big enough.
	bool MakeClampedUniformKnotVector(
		double delta = 1.0
		);

	// Description:
	//   Make knot vector a periodic uniform knot vector based on the current values of m_order and m_cv_count.Does not change values of control vertices.
	// Parameters:
	//   delta - [in] (>0.0) knot spacing.
	// Returns:
	//   true if successful.
	// Remarks:
	//   Allocates m_knot[] if it is not big enough.
	// See Also:
	//   ON_MakePeriodicUniformKnotVector
	bool MakePeriodicUniformKnotVector(
		double delta = 1.0
		);

	bool IsClamped( // determine if knot vector is clamped
		int = 2 // end to check: 0 = start, 1 = end, 2 = start and end
		) const;

	double SuperfluousKnot(
		int // 0 = start, 1 = end
		) const;

	bool ZeroCVs(); // zeros control vertices and, if rational, sets weights to 1

	bool MakeRational();

	bool MakeNonRational();

	bool ChangeDimension(
		int desired_dimension
		);

};

#endif __JET_DXFNURBS_H