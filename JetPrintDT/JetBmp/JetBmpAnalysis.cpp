
#include "JetBmpAnalysis.h"
#include <math.h>

#define DRAW_BMP_IMAGE 10

CJetBmpAnalysis::CJetBmpAnalysis(void)
{
	m_maxDropHeight=0.0;
	m_maxDropWidth=0.0;
	m_ImageHeight=0;
	m_ImageWidth=0;
	m_dbXZoomFactor=0.0;
	m_dbYZoomFactor = 0.0;

	m_dbMax_X = 0;
	m_dbMax_Y = 0;

	m_dbBorderMax_X = 0.0;
	m_dbBorderMax_Y = 0.0;

	m_dbImageHeight = 0.0;
	m_dbImageWidth = 0.0;
	m_dbSpace = 0.0;
	m_uThreashold = 0;

	m_pFile = NULL;
	m_bIsMilInitialized = false;

	m_dbXOffSet = 0.0;
	m_dbYOffSet = 0.0;
	m_dbXLenth = 0.0;
	m_dbYLenth = 0.0;

	m_borderType = BT_ELLIPSE;
}

CJetBmpAnalysis::~CJetBmpAnalysis(void)
{

}



BOOL CJetBmpAnalysis::BmpAnalysis( 
	CString FileName, 
	double ImageWidth, 
	double ImageHeight, 
	int thresh, 
	double dropspace )
{
	string filename = CW2A(FileName);
	Mat orignImage = imread(filename);

	//根据所给参数求取放大倍数
	
	GetZoomFactor(ImageHeight,ImageWidth,dropspace);
	m_ImageHeight = ImageHeight;
	m_ImageWidth = ImageWidth;

	Mat resizeimage;
	cv::Size resizeimageSize;
	resizeimageSize.height = m_maxDropHeight;
	resizeimageSize.width = m_maxDropWidth;

	//zoom the image
	resize(orignImage, resizeimage, resizeimageSize);

	//graying image
	Mat graymat;
	cvtColor(resizeimage, graymat, CV_BGR2GRAY);

	//binary image
	Mat resultImage;
	threshold(graymat, resultImage, 128, 255, CV_THRESH_BINARY);

	//将带打印的点保存入vector<PrintedPoint>容器中
	if(FALSE==GetPrintedDrop(resultImage,dropspace))
	{
		return FALSE;
	}

	return TRUE;
}

void CJetBmpAnalysis::GetZoomFactor( double ImageHeight,double ImageWidth,double dropspace )
{
	m_maxDropWidth=(ImageHeight/dropspace);
	m_maxDropHeight=(ImageWidth/dropspace);

	//m_dbXZoomFactor = m_maxDropWidth/m_ImageWidth;
	//m_dbYZoomFactor = m_maxDropHeight/m_ImageHeight;
}


BOOL CJetBmpAnalysis::GetPrintedDrop(Mat &inarray,double dbDropSpace)
{
	m_vecPoint.clear();
	m_drawVector.clear();
	m_drawWithBorderVector.clear();
	PrintedPoint printpoint;
	PrintedPoint borderPoint;
	int iCentreWidth= m_maxDropWidth/2;
	int iCentreHeight= m_maxDropHeight/2;
	int imageWidth = inarray.cols;
	int imageHeight = inarray.rows;

	m_vecPoint.resize(static_cast<unsigned int>(m_maxDropHeight));
	if(true)
	{
		for(int i=0;i<m_vecPoint.size();++i)
		{
			m_vecPoint[i].resize(static_cast<unsigned int>(m_maxDropWidth));
			for(int j=0;j<m_vecPoint[i].size();++j)
			{
				if (0 == inarray.data[i*imageWidth + j])
				{
					printpoint.cx = (j - iCentreWidth)*dbDropSpace;
					printpoint.cy = (i - iCentreHeight)*dbDropSpace;
					borderPoint.cx = (j - iCentreWidth)*dbDropSpace + m_dbXOffSet;
					borderPoint.cy = (i - iCentreHeight)*dbDropSpace + m_dbYOffSet;
					printpoint.M1=0;
					printpoint.M2=0;
					printpoint.M3=0;
					borderPoint.M1 = 0;
					borderPoint.M2 = 0;
					borderPoint.M3 = 0;
					m_vecPoint[i][j]= (printpoint);

					m_dbBorderMax_X = (abs(borderPoint.cx)>m_dbBorderMax_X)?(abs(borderPoint.cx)):m_dbBorderMax_X;
					m_dbBorderMax_Y = (abs(borderPoint.cy)>m_dbBorderMax_Y)?(abs(borderPoint.cy)):m_dbBorderMax_Y;


					m_drawVector.push_back(printpoint);
					m_drawWithBorderVector.push_back(borderPoint);
				}
				else
				{
					printpoint.cx = POINT_ABANDED;
					printpoint.cy = POINT_ABANDED;
					printpoint.M1 = 0;
					printpoint.M2 = 0;
					printpoint.M3 = 0;
					m_vecPoint[i][j] = (printpoint);
				}
			}
		}
	}
	else
	{
		AfxMessageBox(TEXT("图形参数选择不合适，请重新选择"));
		return FALSE;
	}

	return TRUE;
}


void CJetBmpAnalysis::WriteProg( CString filePath )
{
	if(!filePath.IsEmpty())
	{
		if (PathFileExists((LPCTSTR)filePath))
		{
			DeleteFile((LPCTSTR)filePath);
		}
		
		errno_t errorResult = 0; 	
		errorResult = fopen_s(&m_pFile, CW2A(filePath), "wt");

		if(NULL == m_pFile)
		{
			AfxMessageBox(_T("打开解析的txt文件出问题!"));
			return ;
		}
	}

	CString strLine = _T("");
	strLine = _T("PROGRAM");
	WriteLine(strLine);

	strLine.Format(_T("ACC 2000 DEC 2000 STP0"));
	WriteLine(strLine);

	strLine.Format(_T("%s\n"),_T("CLR 32"));
	WriteLine(strLine);
	//移动至左上角的打印点
	double db_xPoint = 0.0;
	double db_yPoint = 0.0;
	double db_space = 0.0;
	vector<PrintedPoint> tempVec;

	int nRowCount = 1;
	for (unsigned int i=1;i<m_vecPoint.size();i++)
	{

		//逐行遍历，获取打印点
		for(UINT j=0;j<m_vecPoint[i].size();j++)
		{
			if(m_vecPoint[i][j].cx!=POINT_ABANDED && m_vecPoint[i][j].cy!= POINT_ABANDED)
			{
				tempVec.push_back(m_vecPoint[i][j]);
			}

		}
		if(0!=tempVec.size())
		{
			if(abs(tempVec[0].cx)>m_dbMax_X)
			{
				m_dbMax_X = abs(tempVec[0].cx);
			}

			if(abs(tempVec[0].cy)>m_dbMax_Y)
			{
				m_dbMax_Y = abs(tempVec[0].cy);
			}

			//首先32
			strLine.Format(_T("%s\n"),_T("CLR32"));
			WriteLine(strLine);

			//如果是奇数行，从第一个点打印
			if(nRowCount%2 == 1)
			{
				for(UINT k=1;k<tempVec.size();k++)
				{
					db_xPoint = tempVec[k].cx;
					db_yPoint = tempVec[k].cy;
					strLine.Format(_T("X%.2f Y%.2f"),-db_xPoint+m_dbXOffSet,db_yPoint+m_dbYOffSet);
					WriteLine(strLine);

					//等待基板运动至目标点
					strLine.Format(_T("%s\n"),_T("INH -516"));
					WriteLine(strLine);

					strLine.Format(_T("%s\n"),_T("Set 32"));
					WriteLine(strLine);

					//then DWL 
					strLine.Format(_T("%s\n"),_T("DWL (P1000)"));
					WriteLine(strLine);

					//Then CLR
					strLine.Format(_T("%s\n"),_T("CLR 32"));
					WriteLine(strLine);
				}

			}
			else
			{
				for(int k = tempVec.size()-1;k>=1;k--)
				{
					db_xPoint = tempVec[k].cx;
					db_yPoint = tempVec[k].cy;
					strLine.Format(_T("X%.2f Y%.2f\n"),-db_xPoint+m_dbXOffSet,db_yPoint+m_dbYOffSet);
					WriteLine(strLine);

					//等待基板运动至目标点
					strLine.Format(_T("%s\n"),_T("INH -516"));
					WriteLine(strLine);

					strLine.Format(_T("%s\n"),_T("Set 32"));
					WriteLine(strLine);

					//then DWL 
					strLine.Format(_T("%s\n"),_T("DWL (P1000)"));
					WriteLine(strLine);

					//Then CLR
					strLine.Format(_T("%s\n"),_T("CLR 32"));
					WriteLine(strLine);
				}
			}

			//清空vec的内容，让其重新填充
		}

		tempVec.clear();
		++nRowCount;
	}
	//写文件结束,同时结束程序
	strLine.Format(_T("ENDP"));
	WriteLine(strLine);

	fclose(m_pFile);

}

CPoint CJetBmpAnalysis::RetriveImageRange()
{
	CPoint point;
	point.x = static_cast<LONG>(m_dbMax_X);
	point.y = static_cast<LONG>(m_dbMax_Y);
	return point;
}

void CJetBmpAnalysis::SetImageRangeEmpty()
{
	m_dbMax_X = 0.0;
	m_dbMax_Y = 0.0;
}

void CJetBmpAnalysis::DrawBMPImageWithBorder(CWnd* pWnd,CDC* pDC)
{
	pDC->SaveDC();

	CBrush blackBrush(RGB(0,0,0));
	pDC->SelectObject(blackBrush);

	CRect rect;
	pWnd->GetWindowRect(&rect);
	pWnd->GetClientRect(&rect);

	pDC->FillRect(&rect,&blackBrush);

	DeleteObject(blackBrush);

	
	double imageRange_x = m_dbBorderMax_X;
	double imageRange_y = m_dbBorderMax_Y;

	//调节窗口和视口绘制坐标系，坐标原点在窗口中心，X轴向左为正，Y轴向上为正
	pDC->SetMapMode(MM_ANISOTROPIC );
	int ZoomScale = 1;

	pDC->SetWindowExt(((m_dbXLenth+2.0) * ZoomScale) * DRAW_BMP_IMAGE, ((m_dbYLenth+2.0) * ZoomScale) * DRAW_BMP_IMAGE);

	pDC->SetViewportExt(rect.Width()/2, (rect.Height())/2);
	pDC->SetViewportOrg(rect.Width()/2, rect.Height()/2);
	pDC->SelectObject((HBRUSH)::GetStockObject(NULL_BRUSH));

	CPen borderPen(PS_SOLID,2,RGB(90,127,208));
	pDC->SelectObject(borderPen);
	int xStart = static_cast<int>((m_dbXLenth/2.0)*DRAW_BMP_IMAGE);
	int yStart = static_cast<int>((m_dbYLenth/2.0)*DRAW_BMP_IMAGE);
	if(BT_RECT == m_borderType)
	{
		pDC->Rectangle(-xStart,yStart,xStart,-yStart);
	}
	else if(BT_ELLIPSE == m_borderType)
	{
		pDC->Ellipse(-xStart,yStart,xStart,-yStart);
	}

	DeleteObject(borderPen);


	CPen redPen(PS_SOLID,1,RGB(255,0,0));
	pDC->SelectObject(redPen);
	vector<PrintedPoint>::iterator iter1=m_drawWithBorderVector.begin();
	vector<PrintedPoint>::iterator iter2=m_drawWithBorderVector.end();

	for(;iter1!=iter2;++iter1)
	{	
		if(m_dbSpace <= 0.5 || m_vecPoint.size() >=10000 )
		{
			pDC->SetPixel(static_cast<int>(iter1->cx * DRAW_BMP_IMAGE), static_cast<int>(iter1->cy * DRAW_BMP_IMAGE), RGB(255,0,0));
		}
		else
		{
			pDC->Ellipse(static_cast<int>((iter1->cx - m_dbSpace/2) * DRAW_BMP_IMAGE),
				static_cast<int>((iter1->cy - m_dbSpace/2) * DRAW_BMP_IMAGE),
				static_cast<int>((iter1->cx + m_dbSpace/2) * DRAW_BMP_IMAGE),
				static_cast<int>((iter1->cy + m_dbSpace/2) * DRAW_BMP_IMAGE));
		}

	}

	DeleteObject(redPen);
}

void CJetBmpAnalysis::DrawBMPImage(CWnd *pWnd,CDC *pDC)
{
	//绘制坐标系
	pDC->SaveDC();

	CBrush blackBrush(RGB(0,0,0));
	pDC->SelectObject(blackBrush);

	CRect rect;
	pWnd->GetWindowRect(&rect);
	pWnd->GetClientRect(&rect);

	pDC->FillRect(&rect,&blackBrush);

	DeleteObject(blackBrush);

	CPen redPen(PS_SOLID,1,RGB(255,0,0));
	pDC->SelectObject(redPen);
	CPoint ImageRange;
	ImageRange = RetriveImageRange();

	//调节窗口和视口绘制坐标系，坐标原点在窗口中心，X轴向左为正，Y轴向上为正
	pDC->SetMapMode(MM_ANISOTROPIC );
	int ZoomScale = 3;

	pDC->SetWindowExt((ImageRange.x * ZoomScale) * DRAW_BMP_IMAGE, (ImageRange.y * ZoomScale) * DRAW_BMP_IMAGE);

	pDC->SetViewportExt(rect.Width(), (rect.Height()));
	pDC->SetViewportOrg(rect.Width()/2, rect.Height()/2);
	pDC->SelectObject((HBRUSH)::GetStockObject(NULL_BRUSH));



	vector<PrintedPoint>::iterator iter1=m_drawVector.begin();
	vector<PrintedPoint>::iterator iter2=m_drawVector.end();

	for(;iter1!=iter2;++iter1)
	{	
		if(m_dbSpace<0.50f && m_vecPoint.size() >=10000 )
		{
			pDC->SetPixel(static_cast<int>(iter1->cx * DRAW_BMP_IMAGE), static_cast<int>(iter1->cy * DRAW_BMP_IMAGE), RGB(255,0,0));
		}
		else
		{
			pDC->Ellipse(static_cast<int>((iter1->cx - m_dbSpace/2) * DRAW_BMP_IMAGE),
				static_cast<int>((iter1->cy - m_dbSpace/2) * DRAW_BMP_IMAGE),
				static_cast<int>((iter1->cx + m_dbSpace/2) * DRAW_BMP_IMAGE),
				static_cast<int>((iter1->cy + m_dbSpace/2) * DRAW_BMP_IMAGE));
		}

	}

	DeleteObject(redPen);

	//CFileLoader::m_pBmpParser->m_vecPoint.clear();
}



bool CJetBmpAnalysis::WriteLine(CString strLine)
{
	bool bResult = false;
	if(NULL == m_pFile)
	{
		bResult = false;
		return bResult;
	}

	if(fprintf_s(m_pFile,"%s\n",CW2A(strLine))<= 0)
	{
		bResult = false;
		return bResult;
	}

	bResult = true;

	return bResult;
}

bool CJetBmpAnalysis::CheckError(const double& db_Var,const double& db_Const)
{
	bool bResult = false;
	CString varText = _T("");
	CString constText = _T("");

	varText.Format(_T("%.4f"),db_Var);
	constText.Format(_T("%.4f"),db_Const);

	if(0 == wcscmp(varText,constText))
	{
		bResult = true;
	}
	else
		bResult = false;

	return bResult;
}

void CJetBmpAnalysis::SetXOffSet(double dbXOffSet)
{
	m_dbXOffSet = dbXOffSet;
}

void CJetBmpAnalysis::SetYOffSet(double dbYOffset)
{
	m_dbYOffSet = dbYOffset;
}

double CJetBmpAnalysis::GetXOffSet() const
{
	return m_dbXOffSet;
}

double CJetBmpAnalysis::GetYOffSet() const
{
	return m_dbYOffSet;
}

void CJetBmpAnalysis::SetXLenth(double dbXLenth)
{
	m_dbXLenth = dbXLenth;
}

void CJetBmpAnalysis::SetYLenth(double dbYLenth)
{
	m_dbYLenth = dbYLenth;
}

double CJetBmpAnalysis::GetXLenth() const
{
	return m_dbXLenth;
}

double CJetBmpAnalysis::GetYLenth() const
{
	return m_dbYLenth;
}

void CJetBmpAnalysis::SetBorderType(BORDERTYPE borderType)
{
	m_borderType = borderType;
}

BORDERTYPE CJetBmpAnalysis::GetBorderType() const
{
	return m_borderType;
}