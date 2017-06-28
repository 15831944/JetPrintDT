#ifndef  __OPEN_SPLINE_H
#define __OPEN_SPLINE_H
#include "OpenList.h"
//#include "OpenList.cpp"
#include "JetDxfNurbs.h"

class COpenSpline
{
	//private members:
private:
	CJetDxfNurbsCurve m_SplineCurve;


	//public members:
public:
	COpenList<CDxf3dPoint> m_ControlPoints;
	COpenList<double> m_KnotVector;
	COpenList<double> m_Weight;
	COpenList<CDxf3dPoint> m_FitPoints;
	int m_Degree;
	bool m_bIsClosed;

	//private memberfunc
private:

	//public memberfunc
public:
	COpenSpline();
	COpenSpline(const COpenList<CDxf3dPoint> &controlPoints, int degree);
	virtual ~COpenSpline();
	COpenSpline(const COpenSpline &spline);
	COpenSpline& operator=(const COpenSpline& spline);
	void AppendControlPoint(const CDxf3dPoint& controlPoint);
	void AppendControlPoints(COpenList<CDxf3dPoint> &controlPoints);
	void RemoveLastControlPoint();
	void SetControlPoints(const COpenList<CDxf3dPoint> &);
	void GetControlPoints();
	COpenList<CDxf3dPoint> GetControlPoints() const;
	int CountControlPoints() const;
	CDxf3dPoint GetControlPointAt(int i) const;

	void AppendFitPoint(const CDxf3dPoint& point);
	void PrependFitPoint(const CDxf3dPoint& point);
	void InsertFitPointAt(const CDxf3dPoint& point);
	void RemoveFitPointAt(const CDxf3dPoint& point);
	void RemoveLastFitPoint();
	void SetFitPoints(const COpenList<CDxf3dPoint>& points);
	COpenList<CDxf3dPoint> GetFitPoints() const;
	int CountFitPoints() const;
	bool HasFitPoints() const;

	COpenList<double> GetKnotVector() const;
	COpenList<double> GetActualKnotVector() const;
	void SetKnotVector(const COpenList<double>& knots);
	void AppendKnot(double k);
	COpenList<double> GetWeights() const;

	void SetDegree(int degree);
	int GetDegree() const;
	int GetOrder() const;

	void SetCloseState(bool bClosed);
	bool IsClosed() const;

	CDxf3dPoint GetPointAt(double dbStep);
	void UpdateFromControlPoints() ;
	void UpdateFromFitPoints(bool useTangents = false) ;
	void UpdateSpline();
	void DestroySpline();

};

#endif //__OPEN_SPLINE_H