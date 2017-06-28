

#include "OpenSpline.h"


COpenSpline::COpenSpline() :m_Degree(3), m_bIsClosed(false)
{

}

COpenSpline::COpenSpline(const COpenList<CDxf3dPoint> &controlPoints, int degree)
{
	m_Degree = degree;
	m_ControlPoints = controlPoints;
	m_bIsClosed = false;

	UpdateSpline();
}

COpenSpline::~COpenSpline()
{

}

COpenSpline::COpenSpline(const COpenSpline &spline)
{
	this->m_ControlPoints = spline.GetControlPoints();
	this->m_Degree = spline.GetDegree();
	this->m_FitPoints = spline.GetFitPoints();
	this->m_KnotVector = spline.GetKnotVector();
	this->m_SplineCurve = spline.m_SplineCurve;
	this->m_Weight = spline.GetWeights();
	this->m_bIsClosed = spline.IsClosed();
}
COpenSpline& COpenSpline::operator=(const COpenSpline& spline)
{
	if (this == &spline)
		return *this;

	this->m_ControlPoints = spline.GetControlPoints();
	this->m_Degree = spline.GetDegree();
	this->m_FitPoints = spline.GetFitPoints();
	this->m_KnotVector = spline.GetKnotVector();
	this->m_SplineCurve = spline.m_SplineCurve;
	this->m_Weight = spline.GetWeights();
	this->m_bIsClosed = spline.IsClosed();
	return *this;
}

void COpenSpline::AppendControlPoint(const CDxf3dPoint& controlPoint)
{
	m_ControlPoints.AddTail(controlPoint);
}

void COpenSpline::AppendControlPoints(COpenList<CDxf3dPoint> &controlPoints)
{
	m_ControlPoints.AddTail(&controlPoints);
}

void COpenSpline::RemoveLastControlPoint()
{
	m_ControlPoints.RemoveTail();
}

void COpenSpline::SetControlPoints(const COpenList<CDxf3dPoint> &controlPoints)
{
	m_ControlPoints = controlPoints;
}

COpenList<CDxf3dPoint> COpenSpline::GetControlPoints() const
{
	return m_ControlPoints;
}

int COpenSpline::CountControlPoints() const
{
	return m_ControlPoints.GetSize();
}

CDxf3dPoint COpenSpline::GetControlPointAt(int i) const
{
	return m_ControlPoints.GetAt(i);
}

void COpenSpline::AppendFitPoint(const CDxf3dPoint& point)
{
	m_FitPoints.AddTail(point);
}

void COpenSpline::PrependFitPoint(const CDxf3dPoint& point)
{
	m_FitPoints.AddHead(point);
}

void COpenSpline::InsertFitPointAt(const CDxf3dPoint& point)
{

}

void COpenSpline::RemoveFitPointAt(const CDxf3dPoint& point)
{

}

void COpenSpline::RemoveLastFitPoint()
{
	m_FitPoints.RemoveTail();
}

void COpenSpline::SetFitPoints(const COpenList<CDxf3dPoint>& points)
{
	m_FitPoints = points;
}

COpenList<CDxf3dPoint> COpenSpline::GetFitPoints() const
{
	return m_FitPoints;
}

int COpenSpline::CountFitPoints() const
{
	return m_FitPoints.GetSize();
}
bool COpenSpline::HasFitPoints() const
{
	return m_FitPoints.IsEmpty();
}

COpenList<double> COpenSpline::GetKnotVector() const
{
	return m_KnotVector;
}

COpenList<double> COpenSpline::GetActualKnotVector() const
{
	COpenList<double> knot;
	for (int i = 0; i < m_SplineCurve.KnotCount(); i++)
	{
		knot.AddTail(m_SplineCurve.Knot(i));
	}

	return knot;
}
void COpenSpline::SetKnotVector(const COpenList<double>& knots)
{
	m_KnotVector = knots;
}

void COpenSpline::AppendKnot(double k)
{
	m_KnotVector.AddTail(k);
}

COpenList<double> COpenSpline::GetWeights() const
{
	return m_Weight;
}

void COpenSpline::SetDegree(int degree)
{
	m_Degree = degree;
}

int COpenSpline::GetDegree() const
{
	return m_Degree;
}

int COpenSpline::GetOrder() const
{
	return m_Degree + 1;
}

CDxf3dPoint COpenSpline::GetPointAt(double dbStep)
{
	UpdateSpline();

	CDxf3dPoint point = m_SplineCurve.PointAt(dbStep);
	CDxf3dPoint pt;
	pt.x = point.x;
	pt.y = point.y;

	return pt;
}

bool COpenSpline::IsClosed() const
{
	return m_bIsClosed;
}

void COpenSpline::SetCloseState(bool bClosed)
{
	m_bIsClosed = bClosed;
}

void COpenSpline::UpdateFromControlPoints()
{
	if (m_ControlPoints.GetSize() < m_Degree + 1)
	{
		//invalidate();
		return;
	}

	// periodic:
	if (m_bIsClosed && !HasFitPoints())
	{
		CDxf3dPoint* points = new CDxf3dPoint[m_ControlPoints.GetSize()];
		for (int i = 0; i < m_ControlPoints.GetSize(); ++i)
		{
			CDxf3dPoint dbPoint = m_ControlPoints.GetAt(i);
			points[i] = CDxf3dPoint(dbPoint.x, dbPoint.y, 0.0);
		}
		m_SplineCurve.CreatePeriodicUniformNurbs(3, GetOrder(), m_ControlPoints.GetSize(), points);
		delete[] points;
	}

	// open or from fit points:
	else {
		m_SplineCurve.Create(3, false, GetOrder(), m_ControlPoints.GetSize());

		// setting control points:
		for (int i = 0; i < m_ControlPoints.GetSize(); ++i)
		{
			CDxf3dPoint cp = m_ControlPoints.GetAt(i);
			CDxf3dPoint onp(cp.x, cp.y, 0.0);
			m_SplineCurve.SetCV(i, onp);
			//qDebug() << "RSpline: controlPoints[" << i << "]: " << cp;
		}

		bool knotCondition = (m_KnotVector.GetSize() == GetOrder() + m_ControlPoints.GetSize() - 2);
		//knotCondition = true;

		// genetate knot vector automatically:
		if (m_KnotVector.IsEmpty() || !knotCondition)
		{
			int si = DXFKnotCount(GetOrder(), m_ControlPoints.GetSize());
			double* knot = new double[si];
			//ON_MakePeriodicUniformKnotVector(getOrder(), controlPoints.size(), knot);
			DXFMakeClampedUniformKnotVector(GetOrder(), m_ControlPoints.GetSize(), knot);
			for (int i = 0; i < si; ++i)
			{
				//                qDebug() << "RSpline: knot[" << i << "]: " << knot[i];
				m_SplineCurve.SetKnot(i, knot[i]);
			}
			delete[] knot;
		}
		else
		{
			int k = 0;
			for (int i = 0; i < m_KnotVector.GetSize(); ++i)
			{
				//qDebug() << "RSpline: knot[" << i << "]: " << knotVector.at(i);
				bool ok = m_SplineCurve.SetKnot(k++, m_KnotVector.GetAt(i));
				if (!ok)
				{

				}
			}
		}
	}
}

void COpenSpline::UpdateFromFitPoints(bool useTangents)
{
	if (m_FitPoints.GetSize() < m_Degree)
	{
		//invalidate();
		return;
	}

	//// call into plugin
	//if (splineProxy != NULL) 
	//{
	//	RSpline spline = splineProxy->updateFromFitPoints(*this, useTangents);
	//	this->degree = spline.degree;
	//	this->periodic = spline.periodic;
	//	this->controlPoints = spline.controlPoints;
	//	this->knotVector = spline.knotVector;
	//	this->weights = spline.weights;
	//	this->tangentStart = spline.tangentStart;
	//	this->tangentEnd = spline.tangentEnd;
	//	this->curve = spline.curve;
	//	this->dirty = false;
	//}
	//else
	//{
	//	invalidate();
	//	return;
	//}
}

void COpenSpline::UpdateSpline()
{
	if (m_Degree < 1)
	{
		return;
	}

	if (m_FitPoints.GetSize() == 0)
	{
		UpdateFromControlPoints();
	}
	else
	{
		UpdateFromFitPoints();
	}
}

void COpenSpline::DestroySpline()
{
	m_SplineCurve.Destroy();
}