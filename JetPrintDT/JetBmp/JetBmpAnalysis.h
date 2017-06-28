#pragma once

#include <vector>
#include "../cvHead.h"
#include "../stdafx.h"
using namespace std;

typedef struct _PrintedPoint
{
public:
	double cx;
	double cy;
	int M1;
	int M2;
	int M3;
}PrintedPoint;

class CJetBmpAnalysis
{
public:
	CJetBmpAnalysis(void);
	virtual ~CJetBmpAnalysis(void);

	BOOL BmpAnalysis(
		CString FileName,
		double ImageWidth,
		double ImageHeight, 
		int thresh, 
		double dropspace);
	void WriteProg(CString filePath);
	bool WriteLine(CString strLine);
	void GetZoomFactor(double ImageHeight,double ImageWidth,double dropspace);
	BOOL GetPrintedDrop(cv::Mat &inarray,double dbDropSpace);
	CPoint RetriveImageRange();
	void SetImageRangeEmpty();
	void DrawBMPImage(CWnd *pWnd,CDC *pDC);
	void DrawBMPImageWithBorder(CWnd* pWnd,CDC* pDC);
	bool CheckError(const double& db_Var,const double& db_Const);
	void SetXOffSet(double dbXOffSet);
	void SetYOffSet(double dbYOffset);
	double GetXOffSet() const;
	double GetYOffSet() const;
	void SetXLenth(double dbXLenth);
	void SetYLenth(double dbYLenth);
	double GetXLenth() const;
	double GetYLenth() const;
	void SetBorderType(BORDERTYPE borderType);
	BORDERTYPE GetBorderType() const;
private:
	double   m_maxDropWidth;          //每行最多打印点数
	double   m_maxDropHeight;         //每列最多打印点数
	int   m_ImageWidth;			  //原图像宽度（以像素为单位）
	int   m_ImageHeight;           //原图像高度（以像素为单位）
	double       m_dbXZoomFactor;                  //放大倍数
	double       m_dbYZoomFactor;
	
	double m_dbMax_X;                     //图形在X方向最大点坐标
	double m_dbMax_Y;                     //图形在Y方向最大点坐标
	double m_dbBorderMax_X;
	double m_dbBorderMax_Y;
	double m_dbXOffSet;
	double m_dbYOffSet;
	double m_dbXLenth;
	double m_dbYLenth;

	BORDERTYPE m_borderType;
	FILE *m_pFile;
	enum {POINT_ABANDED = 100};

public:
	bool m_bIsMilInitialized;
	double m_dbImageHeight;
	double m_dbImageWidth;
	double m_dbSpace;
	UINT   m_uThreashold;
	vector<vector<PrintedPoint>> m_vecPoint;
	vector<PrintedPoint> m_drawVector;
	vector<PrintedPoint> m_drawWithBorderVector;
};
