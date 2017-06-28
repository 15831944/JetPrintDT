#include "StdAfx.h"
#include "JetBmpAnalysis.h"

#include <math.h>

#define DRAW_BMP_IMAGE 10

CJetBmpAnalysis::CJetBmpAnalysis(void)
{
	m_maxDropHeight=0.0;
	m_maxDropWidth=0.0;
	m_ImageHeight=0.0;
	m_ImageWidth=0.0;
	m_zoom=0;

	m_dbMax_X = 0;
	m_dbMax_Y = 0;


	m_dbImageHeight = 0.0;
	m_dbImageWidth = 0.0;
	m_dbSpace = 0.0;
	m_uThreashold = 0;

	m_pFile = NULL;
	m_bIsMilInitialized = false;
}

CJetBmpAnalysis::~CJetBmpAnalysis(void)
{
	if(m_milSystem)       
		MsysFree (m_milSystem);   

	if(m_milApplication)  
	{
		// Enable the typical MIL error message display[CALL TO MIL]
		MappControl(M_ERROR,M_PRINT_ENABLE);

		// Unhook MIL error on function DisplayError() [CALL TO MIL]

		// Free the application [CALL TO MIL]
		MappFree(m_milApplication);
	}

	MbufFree(m_bufSrcImage);
	MbufFree(m_bufZoomImage);
	MbufFree(m_bufBinaryImage);
}



BOOL CJetBmpAnalysis::BmpAnalysis( 
	MIL_CONST_TEXT_PTR FileName, 
	double ImageWidth, 
	double ImageHeight, 
	MIL_INT thresh, 
	double dropspace )
{
	//��ȡ������BMPͼ��
	MbufRestore(FileName,m_milSystem,&m_bufSrcImage);

	//��ȡԴͼ�����ظ߶ȺͿ��
	m_ImageWidth=MbufDiskInquire(FileName,M_SIZE_X,M_NULL);
	m_ImageHeight=MbufDiskInquire(FileName,M_SIZE_Y,M_NULL);
	//��������������ȡ�Ŵ���
	IsImageZoom(ImageHeight,ImageWidth,dropspace);
	//���䱣��Ŵ��ͼ��
	MbufAlloc2d(m_milSystem,(MIL_INT)m_ImageWidth,(MIL_INT)m_ImageHeight,8+M_UNSIGNED, 
		M_IMAGE+M_PROC+M_DISP,  &m_bufZoomImage);

	MbufAlloc2d(m_milSystem,(MIL_INT)m_ImageWidth,(MIL_INT)m_ImageHeight,8+M_UNSIGNED, 
		M_IMAGE+M_PROC+M_DISP,  &m_bufBinaryImage);

	MimResize(m_bufSrcImage,m_bufZoomImage,(MIL_DOUBLE)m_zoom,(MIL_DOUBLE)m_zoom,M_BILINEAR + M_OVERSCAN_ENABLE +M_FAST );
	//���䱣���ֵ�����ͼ��

	MimBinarize(m_bufZoomImage,m_bufBinaryImage,M_GREATER_OR_EQUAL,thresh,M_NULL);

	//��ȡȫ������ӡ��
	unsigned char*UserArrayPtr = new unsigned char[int(m_ImageWidth*m_ImageHeight)];
	MbufGet2d(m_bufBinaryImage,0,0,(MIL_INT)m_ImageWidth,(MIL_INT)m_ImageHeight,UserArrayPtr);

	//������ӡ�ĵ㱣����vector<PrintedPoint>������
	if(FALSE==GetPrintedDrop(UserArrayPtr,dropspace))
	{
		if(NULL != UserArrayPtr)
		{
			delete [] UserArrayPtr;
			UserArrayPtr = NULL;
		}
		return FALSE;
	}

	if(NULL != UserArrayPtr)
	{
		delete [] UserArrayPtr;
		UserArrayPtr = NULL;
	}

	return TRUE;
}

void CJetBmpAnalysis::IsImageZoom( double ImageHeight,double ImageWidth,double dropspace )
{
	m_maxDropWidth=(ImageHeight/dropspace);
	m_maxDropHeight=(ImageWidth/dropspace);
	if(m_maxDropWidth>m_ImageWidth || m_maxDropHeight>m_ImageHeight)
	{
		if((m_maxDropWidth/m_ImageWidth)>=(m_maxDropHeight/m_ImageHeight))
		{
			m_zoom=m_maxDropWidth/m_ImageWidth;
			m_ImageWidth*=m_zoom;
			m_ImageHeight*=m_zoom;
		}
		else
		{
			m_zoom=m_maxDropHeight/m_ImageHeight;
			m_ImageWidth=MIL_INT(m_zoom*m_ImageWidth);
			m_ImageHeight=MIL_INT(m_zoom*m_ImageHeight);
		}
	}
	else
	{
		m_zoom=1.0;
	}
}

BOOL CJetBmpAnalysis::GetPrintedDrop( unsigned char*UserArrayPtr,double dropspace )
{
	m_vecPoint.clear();
	MIL_INT iGetWidthSpace;
	MIL_INT iGetHeightSpace;

	if(m_ImageWidth/m_maxDropWidth<=1.0)
	{
		iGetWidthSpace=1;
	}
	else
	{
		iGetWidthSpace=MIL_INT(m_ImageWidth/m_maxDropWidth+0.5);
	}

	if(m_ImageHeight/m_maxDropHeight<=1.0)
	{
		iGetHeightSpace=1;
	}
	else
	{
		iGetHeightSpace=MIL_INT(m_ImageHeight/m_maxDropHeight+0.5);
	}

	PrintedPoint printpoint;
	MIL_INT iCentreWidth=(MIL_INT)m_ImageWidth/2;
	MIL_INT iCentreHeight=(MIL_INT)m_ImageHeight/2;

	m_vecPoint.resize(unsigned int(m_ImageHeight));
	if(iGetHeightSpace==1 && iGetWidthSpace==1)
	{
		for(UINT i=0;i<m_vecPoint.size();++i)
		{
			m_vecPoint[i].resize(unsigned int(m_ImageWidth));
			for(UINT j=0;j<m_vecPoint[i].size();++j)
			{
				if(0==UserArrayPtr[i*(MIL_INT)m_ImageWidth+j])
				{
					printpoint.cx=(j-iCentreWidth)*dropspace;
					printpoint.cy=(i-iCentreHeight)*dropspace;
					printpoint.M1=0;
					printpoint.M2=0;
					printpoint.M3=0;
					m_vecPoint[i][j]= (printpoint);

					m_drawVector.push_back(printpoint);
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
		AfxMessageBox(TEXT("ͼ�β���ѡ�񲻺��ʣ�������ѡ��"));
		return FALSE;
	}

	return TRUE;
}

void CJetBmpAnalysis::WriteProg( CString filePath )
{
	if(!filePath.IsEmpty())
	{
		fopen_s(&m_pFile,CW2A(filePath),"wt");
	}


	CString strLine = _T("");
	strLine = _T("PROGRAM");
	WriteLine(strLine);

	//�ƶ������ϽǵĴ�ӡ��
	double db_xPoint = 0.0;
	double db_yPoint = 0.0;

	vector<PrintedPoint> tempVec;
	for (unsigned int i=1;i<m_vecPoint.size();i++)
	{

		//���б�������ȡ��ӡ��
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
			
			//����CLR128
			strLine.Format(_T("CLR128"));
			WriteLine(strLine);

			//Ȼ���ƶ�����һ����
			db_xPoint = tempVec[0].cx;
			db_yPoint = tempVec[0].cy;
			strLine.Format(_T("X%.2f Y%.2f"),db_xPoint,db_yPoint);
			WriteLine(strLine);

			//����ԭλ�˶�һ�Σ�Ȼ��Set128
			WriteLine(strLine);
			strLine.Format(_T("SET128"));
			WriteLine(strLine);

			for(UINT k=1;k<tempVec.size();)
			{
				if(abs(tempVec[k].cx)> m_dbMax_X)
				{
					m_dbMax_X = abs(tempVec[k].cx);
				}

				if(abs(tempVec[k].cy > m_dbMax_Y))
				{
					m_dbMax_Y = abs(tempVec[k].cy);
				}

				if((tempVec[k].cx - tempVec[k-1].cx) == m_dbSpace)
				{
					//����������ĵ㣬���ù�

					//��������һ����
					if(k == tempVec.size()-1)
					{
						strLine.Format(_T("SET128"));
						WriteLine(strLine);

						db_xPoint = tempVec[k].cx;
						db_yPoint = tempVec[k].cy;
						strLine.Format(_T("X%.2f Y%.2f"),db_xPoint,db_yPoint);
						WriteLine(strLine);

						db_xPoint = tempVec[k].cx;
						db_yPoint = tempVec[k].cy;
						strLine.Format(_T("X%.2f Y%.2f"),db_xPoint,db_yPoint);
						WriteLine(strLine);

						strLine.Format(_T("CLR128"));
						WriteLine(strLine);
					}
				}
				else
				{
					//����˵��Ǽ�ϵ��յ�
					//�õ���ƶ���K-1�㴦��Ȼ�����128���ƶ���K��
					strLine.Format(_T("SET128"));
					WriteLine(strLine);

					db_xPoint = tempVec[k-1].cx;
					db_yPoint = tempVec[k-1].cy;
					strLine.Format(_T("X%.2f Y%.2f"),db_xPoint,db_yPoint);
					WriteLine(strLine);

					db_xPoint = tempVec[k-1].cx;
					db_yPoint = tempVec[k-1].cy;
					strLine.Format(_T("X%.2f Y%.2f"),db_xPoint,db_yPoint);
					WriteLine(strLine);

					strLine.Format(_T("CLR128"));
					WriteLine(strLine);

					//Ȼ���˶���K�㣬����Set128
					db_xPoint = tempVec[k].cx;
					db_yPoint = tempVec[k].cy;
					strLine.Format(_T("X%.2f Y%.2f"),db_xPoint,db_yPoint);
					WriteLine(strLine);

					db_xPoint = tempVec[k].cx;
					db_yPoint = tempVec[k].cy;
					strLine.Format(_T("X%.2f Y%.2f"),db_xPoint,db_yPoint);
					WriteLine(strLine);

					strLine.Format(_T("SET128"));
					WriteLine(strLine);
				}

				k++;
			}

		}

		//���vec�����ݣ������������
		tempVec.clear();
	}

	//д�ļ�����,ͬʱ��������
	strLine.Format(_T("ENDP"));
	WriteLine(strLine);

	fclose(m_pFile);
}

CPoint CJetBmpAnalysis::RetriveImageRange()
{
	CPoint point;
	point.x = long(m_dbMax_X);
	point.y = long(m_dbMax_Y);
	return point;
}

void CJetBmpAnalysis::DrawBMPImage(CWnd *pWnd,CDC *pDC)
{
	//��������ϵ
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

	//���ڴ��ں��ӿڻ�������ϵ������ԭ���ڴ������ģ�X������Ϊ����Y������Ϊ��
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
		if(m_dbSpace <= 0.5 || m_vecPoint.size() >=10000 )
		{
			pDC->SetPixel((int(iter1->cx ))* DRAW_BMP_IMAGE, (int(iter1->cy)) * DRAW_BMP_IMAGE, RGB(255,0,0));
		}
		else
		{
			pDC->Ellipse((int(iter1->cx - m_dbSpace/2)) * DRAW_BMP_IMAGE,
				(int(iter1->cy - m_dbSpace/2)) * DRAW_BMP_IMAGE,
				(int(iter1->cx + m_dbSpace/2)) * DRAW_BMP_IMAGE,
				(int(iter1->cy + m_dbSpace/2)) * DRAW_BMP_IMAGE);
		}

	}

	DeleteObject(redPen);

	//CFileLoader::m_pBmpParser->m_vecPoint.clear();
}


bool CJetBmpAnalysis::InitMilSystem()
{
	MappAllocDefault(M_SETUP, &m_milApplication, &m_milSystem, M_NULL, M_NULL, M_NULL);
	if (M_NULL_ERROR != MappGetError(M_GLOBAL, M_NULL))
	{
		m_bIsMilInitialized = false;
		return false; 
	}

	m_bIsMilInitialized = true;
	return true;
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