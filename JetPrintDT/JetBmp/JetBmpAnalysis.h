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
	double   m_maxDropWidth;          //ÿ������ӡ����
	double   m_maxDropHeight;         //ÿ������ӡ����
	int   m_ImageWidth;			  //ԭͼ���ȣ�������Ϊ��λ��
	int   m_ImageHeight;           //ԭͼ��߶ȣ�������Ϊ��λ��
	double       m_dbXZoomFactor;                  //�Ŵ���
	double       m_dbYZoomFactor;
	
	double m_dbMax_X;                     //ͼ����X������������
	double m_dbMax_Y;                     //ͼ����Y������������
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
