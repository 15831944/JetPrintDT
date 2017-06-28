#ifndef __CV_SHOWVIDEO_H
#define __CV_SHOWVIDEO_H

#include "cvHead.h"
#include "CvvImage.h"
using namespace cv;

class CCVShowVideo
{
private:
	int m_iImageHeight;
	int m_iImageWidth;
	int m_iXOffset;
	int m_iYOffset;
	LPBITMAPINFO m_lpBmi;
	int iDibFlag;

	CvvImage m_CurrentImage;
	IplImage *m_pOverlayImage;
	Mat      m_OverlayMat;
public:
	bool m_bShowScale;
	float fRatio;
public:
	CCVShowVideo();
	virtual ~CCVShowVideo();

	void SetImageHeight(int iHeight);
	void SetImageWidth(int iWidth);
	void InitializeImage(char * pBuffer);
	void ScrollImage(int icx, int icy);
	LPBITMAPINFO CtreateMapInfo(IplImage* workImg, int flag);

	int  imageType(IplImage* p);
	int  imageClone(IplImage* pi, IplImage** ppo);  //  ¸´ÖÆ IplImage Î»Í¼
	int  imageReplace(IplImage* pi, IplImage** ppo);  //  Î»Í¼Ìæ»»
	
	void SaveImage(char* szfilename){
		m_CurrentImage.Save(szfilename);
	}

	IplImage *GetImage(){
		return m_CurrentImage.GetImage();
	}

	IplImage* RotateImage(IplImage &image, int degree);

	void InitializeWatchOverlay();
};

#endif //__CV_SHOWVIDEO_H