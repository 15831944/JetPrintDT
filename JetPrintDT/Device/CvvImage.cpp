#include "CvvImage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CV_INLINE RECT NormalizeRect( RECT r );
CV_INLINE RECT NormalizeRect( RECT r )
{
	int t;

	if( r.left > r.right )
	{
		t = r.left;
		r.left = r.right;
		r.right = t;
	}

	if( r.top > r.bottom )
	{
		t = r.top;
		r.top = r.bottom;
		r.bottom = t;
	}

	return r;
}

CV_INLINE CvRect RectToCvRect( RECT sr );
CV_INLINE CvRect RectToCvRect( RECT sr )
{
	sr = NormalizeRect( sr );
	return cvRect( sr.left, sr.top, sr.right - sr.left, sr.bottom - sr.top );
}

CV_INLINE RECT CvRectToRect( CvRect sr );
CV_INLINE RECT CvRectToRect( CvRect sr )
{
	RECT dr;
	dr.left = sr.x;
	dr.top = sr.y;
	dr.right = sr.x + sr.width;
	dr.bottom = sr.y + sr.height;

	return dr;
}

CV_INLINE IplROI RectToROI( RECT r );
CV_INLINE IplROI RectToROI( RECT r )
{
	IplROI roi;
	r = NormalizeRect( r );
	roi.xOffset = r.left;
	roi.yOffset = r.top;
	roi.width = r.right - r.left;
	roi.height = r.bottom - r.top;
	roi.coi = 0;

	return roi;
}

void  FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin )
{
	assert( bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));

	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

	memset( bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = width;
	bmih->biHeight = origin ? abs(height) : -abs(height);
	bmih->biPlanes = 1;
	bmih->biBitCount = (unsigned short)bpp;
	bmih->biCompression = BI_RGB;

	if( bpp == 8 )
	{
		RGBQUAD* palette = bmi->bmiColors;
		int i;
		for( i = 0; i < 256; i++ )
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}
}



CvvImage::CvvImage()
{
	m_img = 0;
}

void CvvImage::Destroy()
{
	cvReleaseImage( &m_img );
}

CvvImage::~CvvImage()
{
	Destroy();
}

bool  CvvImage::Create( int w, int h, int bpp, int origin )
{
	const unsigned max_img_size = 10000;

	if( (bpp != 8 && bpp != 24 && bpp != 32) ||
		(unsigned)w >=  max_img_size || (unsigned)h >= max_img_size ||
		(origin != IPL_ORIGIN_TL && origin != IPL_ORIGIN_BL))
	{
		assert(0); // most probably, it is a programming error
		return false;
	}

	if( !m_img || Bpp() != bpp || m_img->width != w || m_img->height != h )
	{
		if( m_img && m_img->nSize == sizeof(IplImage))
			Destroy();

		/* prepare IPL header */
		m_img = cvCreateImage( cvSize( w, h ), IPL_DEPTH_8U, bpp/8 );
	}

	if( m_img )
		m_img->origin = origin == 0 ? IPL_ORIGIN_TL : IPL_ORIGIN_BL;

	return m_img != 0;
}

void  CvvImage::CopyOf( CvvImage& image, int desired_color )
{
	IplImage* img = image.GetImage();
	if( img )
	{
		CopyOf( img, desired_color );
	}
}


#define HG_IS_IMAGE(img)                                                  \
	((img) != 0 && ((const IplImage*)(img))->nSize == sizeof(IplImage) && \
	((IplImage*)img)->imageData != 0)


void  CvvImage::CopyOf( IplImage* img, int desired_color )
{
	if( HG_IS_IMAGE(img) )
	{
		int color = desired_color;
		CvSize size = cvGetSize( img ); 
		
		if( color < 0 )
			color = img->nChannels > 1;

		if( Create( size.width, size.height,
			(!color ? 1 : img->nChannels > 1 ? img->nChannels : 3)*8,
			img->origin ))
		{
			m_img->align = img->align;
			memset(m_img->imageData, 0, m_img->imageSize);
			cvCopy(img, m_img,NULL);
		}
	}
}


bool  CvvImage::Load( const char* filename, int desired_color )
{
	IplImage* img = cvLoadImage( filename, desired_color );
	if( !img )
		return false;

	CopyOf( img, desired_color );
	cvReleaseImage( &img );

	return true;
}


bool  CvvImage::LoadRect( const char* filename,
	int desired_color, CvRect r )
{
	if( r.width < 0 || r.height < 0 ) return false;

	IplImage* img = cvLoadImage( filename, desired_color );
	if( !img )
		return false;

	if( r.width == 0 || r.height == 0 )
	{
		r.width = img->width;
		r.height = img->height;
		r.x = r.y = 0;
	}

	if( r.x > img->width || r.y > img->height ||
		r.x + r.width < 0 || r.y + r.height < 0 )
	{
		cvReleaseImage( &img );
		return false;
	}

	/* truncate r to m_img image */
	if( r.x < 0 )
	{
		r.width += r.x;
		r.x = 0;
	}
	if( r.y < 0 )
	{
		r.height += r.y;
		r.y = 0;
	}

	if( r.x + r.width > img->width )
		r.width = img->width - r.x;

	if( r.y + r.height > img->height )
		r.height = img->height - r.y;

	cvSetImageROI( img, r );
	CopyOf( img, desired_color );

	cvReleaseImage( &img );
	return true;
}


bool  CvvImage::Save( const char* filename )
{
	if( !m_img )
		return false;
	cvSaveImage( filename, m_img );
	return true;
}


void  CvvImage::Show( const char* window )
{
	if( m_img )
		cvShowImage( window, m_img );
}


void  CvvImage::Show( HDC dc, int x, int y, int w, int h, int from_x, int from_y )
{
	if( m_img && m_img->depth == IPL_DEPTH_8U )
	{
		uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
		BITMAPINFO* bmi = (BITMAPINFO*)buffer;
		int bmp_w = m_img->width, bmp_h = m_img->height;

		FillBitmapInfo( bmi, bmp_w, bmp_h, Bpp(), m_img->origin );

		from_x = MIN( MAX( from_x, 0 ), bmp_w - 1 );
		from_y = MIN( MAX( from_y, 0 ), bmp_h - 1 );

		int sw = MAX( MIN( bmp_w - from_x, w ), 0 );
		int sh = MAX( MIN( bmp_h - from_y, h ), 0 );

		SetDIBitsToDevice(
			dc, x, y, sw, sh, from_x, from_y, from_y, sh,
			m_img->imageData + from_y*m_img->widthStep,
			bmi, DIB_RGB_COLORS );
	}
}


void  CvvImage::DrawToHDC( HDC hDCDst, RECT* pDstRect ) 
{
	if( pDstRect && m_img && m_img->depth == IPL_DEPTH_8U && m_img->imageData )
	{
		uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
		BITMAPINFO* bmi = (BITMAPINFO*)buffer;
		int bmp_w = m_img->width, bmp_h = m_img->height;

		CvRect roi = cvGetImageROI( m_img );
		CvRect dst = RectToCvRect( *pDstRect );

		if( roi.width == dst.width && roi.height == dst.height )
		{
			Show( hDCDst, dst.x, dst.y, dst.width, dst.height, roi.x, roi.y );
			return;
		}

		if( roi.width > dst.width )
		{
			SetStretchBltMode(
				hDCDst,           // handle to device context
				HALFTONE );
		}
		else
		{
			SetStretchBltMode(
				hDCDst,           // handle to device context
				COLORONCOLOR );
		}

		FillBitmapInfo( bmi, bmp_w, bmp_h, Bpp(), m_img->origin );

		::StretchDIBits(
			hDCDst,
			dst.x, dst.y, dst.width, dst.height,
			roi.x, roi.y, roi.width, roi.height,
			m_img->imageData, bmi, DIB_RGB_COLORS, SRCCOPY );
	}
}


void  CvvImage::Fill( int color )
{
	cvSet( m_img, cvScalar(color&255,(color>>8)&255,(color>>16)&255,(color>>24)&255) );
}

void CvvImage::InitializeLocateOverlay(IplImage &image){

	CString rulerLabel = _T("");

	long imageWidth = image.width;
	long imageHeight = image.height;

	// Attach the device context.
	int center_x = imageWidth / 2;
	int center_y = imageHeight / 2;

	// Create a blue pen. 
	CvFont font;
	double hscale = 0.2;
	double vscale = 0.2;
	int linewidth = 0;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, hscale, vscale, linewidth, 0, CV_AA);
	CvScalar color = cvScalar(0, 0, 255);
	CvPoint posStart = cvPoint(0, 0);
	CvPoint posEnd = cvPoint(0, 0);
	CvPoint textPos = cvPoint(0, 0);

	// Draw a cross in the overlay buffer. 
	posStart = cvPoint(0, imageHeight / 2);
	posEnd = cvPoint(imageWidth, imageHeight / 2);
	cvLine(&image, posStart, posEnd, color);

	posStart = cvPoint(imageWidth / 2, 0);
	posEnd = cvPoint(imageWidth / 2, imageHeight);
	cvLine(&image, posStart, posEnd, color);

}

void CvvImage::InitializeLocateOverlay(){

	CString rulerLabel = _T("");

	long imageWidth = m_img->width;
	long imageHeight = m_img->height;

	// Attach the device context.
	int center_x = imageWidth / 2;
	int center_y = imageHeight / 2;

	// Create a blue pen. 
	CvFont font;
	double hscale = 0.2;
	double vscale = 0.2;
	int linewidth = 0;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, hscale, vscale, linewidth,0,CV_AA);
	CvScalar color = cvScalar(0, 0, 255);
	CvPoint posStart = cvPoint(0,0);
	CvPoint posEnd = cvPoint(0, 0);
	CvPoint textPos = cvPoint(0, 0);

	// Draw a cross in the overlay buffer. 
	posStart = cvPoint(0, imageHeight / 2);
	posEnd = cvPoint(imageWidth, imageHeight / 2);
	cvLine(m_img,posStart,posEnd,color);

	posStart = cvPoint(imageWidth / 2, 0);
	posEnd = cvPoint(imageWidth / 2, imageHeight);
	cvLine(m_img, posStart, posEnd, color);

}

void CvvImage::CreateAlphaMat(cv::Mat &mat)
{
	for (int i = 0; i < mat.rows; ++i) {
		for (int j = 0; j < mat.cols; ++j) {
			cv::Vec4b& rgba = mat.at<cv::Vec4b>(i, j);
			rgba[3] = cv::saturate_cast<uchar>(0);
		}
	}
}

IplImage* CvvImage::InitializeWatchOverlayByAddImage(IplImage *pImage){
	if (m_BlackMat.empty()){
		m_BlackMat = cv::Mat(cv::Size(pImage->width/2, pImage->height/2), CV_8UC4, cv::Scalar::all(0));
		m_BlackImage = m_BlackMat;

		InitializeWatchOverlay(m_BlackImage);

		cv::Mat tempMat = cv::Mat(&m_BlackImage);
		CreateAlphaMat(tempMat);
		m_BlackImage = tempMat;
	}

	pImage->nChannels = m_BlackImage.nChannels;
	cvSetImageROI(pImage, cv::Rect(0, 0, m_BlackImage.width, m_BlackImage.height));
	cvSetImageROI(&m_BlackImage, cv::Rect(0, 0, m_BlackImage.width, m_BlackImage.height));

	cvAddWeighted(pImage, 0.8, &m_BlackImage, 0.2, 0.9, pImage);
	
	cvResetImageROI(pImage);
	return pImage;
}

void CvvImage::InitializeWatchOverlay(IplImage& image){
	char szText[64] = { 0 };

	long imageWidth = image.width;
	long imageHeight = image.height;

	// Attach the device context.
	int center_x = imageWidth / 2;
	int center_y = imageHeight / 2;

	// Create a blue pen. 
	CvFont font;
	double hscale = 0.3;
	double vscale = 0.3;
	double linewidth = 0.4;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hscale, vscale, linewidth, 1, CV_AA);
	CvScalar color = cvScalar(0, 0, 255);
	CvPoint posStart = cvPoint(0, 0);
	CvPoint posEnd = cvPoint(0, 0);
	CvPoint textPos = cvPoint(0, 0);

	// Draw a cross in the overlay buffer. 
	posStart = cvPoint(0, imageHeight / 2);
	posEnd = cvPoint(imageWidth, imageHeight / 2);
	cvLine(&image, posStart, posEnd, color);

	posStart = cvPoint(imageWidth / 2, 0);
	posEnd = cvPoint(imageWidth / 2, imageHeight);
	cvLine(&image, posStart, posEnd, color);

	for (int i = 1; i <= 30; i++)
	{
		//×ó²à
		if (0 == (i) % 5)
		{
			posStart = cvPoint(center_x - 10 * i, center_y);
			posEnd = cvPoint(center_x - 10 * i, center_y + 25);
			cvLine(&image, posStart, posEnd, color);

			posStart = cvPoint(center_x - 10 * i, center_y);
			posEnd = cvPoint(center_x - 10 * i, center_y - 25);
			cvLine(&image, posStart, posEnd, color);

			if (0 == (i % 2))
			{
				sprintf(szText, "%.1f", -150.0*(i / 5));
				textPos = cvPoint(center_x - 10 * i, center_y - 25);
				cvPutText(&image, szText, textPos, &font, color);
			}
			else
			{
				sprintf(szText, "%.1f", -150.0*(i / 5));
				textPos = cvPoint(center_x - 10 * i, center_y + 25);
				cvPutText(&image, szText, textPos, &font, color);
			}

		}
		else
		{
			posStart = cvPoint(center_x - 10 * i, center_y);
			posEnd = cvPoint(center_x - 10 * i, center_y + 15);
			cvLine(&image, posStart, posEnd, color);

			posStart = cvPoint(center_x - 10 * i, center_y);
			posEnd = cvPoint(center_x - 10 * i, center_y - 15);
			cvLine(&image, posStart, posEnd, color);
		}

		//ÓÒ²à
		if (0 == (i) % 5)
		{
			posStart = cvPoint(center_x + 10 * i, center_y);
			posEnd = cvPoint(center_x + 10 * i, center_y + 25);
			cvLine(&image, posStart, posEnd, color);

			posStart = cvPoint(center_x + 10 * i, center_y);
			posEnd = cvPoint(center_x + 10 * i, center_y - 25);
			cvLine(&image, posStart, posEnd, color);

			if (0 == (i % 2))
			{
				sprintf(szText, "%.1f", 150.0*(i / 5));
				textPos = cvPoint(center_x + 10 * i, center_y - 25);
				cvPutText(&image, szText, textPos, &font, color);
			}
			else
			{
				sprintf(szText, "%.1f", 150.0*(i / 5));
				textPos = cvPoint(center_x + 10 * i, center_y + 25);
				cvPutText(&image, szText, textPos, &font, color);
			}

		}
		else
		{
			posStart = cvPoint(center_x + 10 * i, center_y);
			posEnd = cvPoint(center_x + 10 * i, center_y + 15);
			cvLine(&image, posStart, posEnd, color);

			posStart = cvPoint(center_x + 10 * i, center_y);
			posEnd = cvPoint(center_x + 10 * i, center_y - 15);
			cvLine(&image, posStart, posEnd, color);
		}
	}

	for (int i = 1; i <= 80; i++)
	{
		//ÉÏ²à
		if (0 == (i) % 5)
		{
			posStart = cvPoint(center_x, center_y - 4 * i);
			posEnd = cvPoint(center_x + 25, center_y - 4 * i);
			cvLine(&image, posStart, posEnd, color);

			posStart = cvPoint(center_x, center_y - 4 * i);
			posEnd = cvPoint(center_x - 25, center_y - 4 * i);
			cvLine(&image, posStart, posEnd, color);

			//TextOut
			if (0 == i % 2)
			{
				sprintf(szText, "%.1f", -60.0*(i / 5));
				textPos = cvPoint(center_x - 55, center_y - 4 * i);
				cvPutText(&image, szText, textPos, &font, color);
			}
			else
			{
				sprintf(szText, "%.1f", -60.0*(i / 5));
				textPos = cvPoint(center_x + 25, center_y - 4 * i);
				cvPutText(&image, szText, textPos, &font, color);
			}


		}
		else
		{
			posStart = cvPoint(center_x, center_y - 4 * i);
			posEnd = cvPoint(center_x + 15, center_y - 4 * i);
			cvLine(&image, posStart, posEnd, color);

			posStart = cvPoint(center_x, center_y - 4 * i);
			posEnd = cvPoint(center_x - 15, center_y - 4 * i);
			cvLine(&image, posStart, posEnd, color);

		}

		//ÏÂ²à
		if (0 == (i) % 5)
		{
			posStart = cvPoint(center_x, center_y + 4 * i);
			posEnd = cvPoint(center_x + 25, center_y + 4 * i);
			cvLine(&image, posStart, posEnd, color);

			posStart = cvPoint(center_x, center_y + 4 * i);
			posEnd = cvPoint(center_x - 25, center_y + 4 * i);
			cvLine(&image, posStart, posEnd, color);

			//TextOut
			if (0 == i % 2)
			{
				sprintf(szText, "%.1f", 60.0*(i / 5));
				textPos = cvPoint(center_x - 55, center_y + 4 * i);
				cvPutText(&image, szText, textPos, &font, color);
			}
			else
			{
				sprintf(szText, "%.1f", 60.0*(i / 5));
				textPos = cvPoint(center_x + 25, center_y + 4 * i);
				cvPutText(&image, szText, textPos, &font, color);
			}


		}
		else
		{
			posStart = cvPoint(center_x, center_y + 4 * i);
			posEnd = cvPoint(center_x + 15, center_y + 4 * i);
			cvLine(&image, posStart, posEnd, color);

			posStart = cvPoint(center_x, center_y + 4 * i);
			posEnd = cvPoint(center_x - 15, center_y + 4 * i);
			cvLine(&image, posStart, posEnd, color);
		}
	}
}


void CvvImage::InitializeWatchOverlay(){
	char szText[64] = { 0 };

	long imageWidth = m_img->width;
	long imageHeight = m_img->height;

	// Attach the device context.
	int center_x = imageWidth / 2;
	int center_y = imageHeight / 2;

	// Create a blue pen. 
	CvFont font;
	double hscale = 0.3;
	double vscale = 0.3;
	double linewidth = 0.4;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hscale, vscale, linewidth, 1,CV_AA);
	CvScalar color = cvScalar(0, 0, 255);
	CvPoint posStart = cvPoint(0, 0);
	CvPoint posEnd = cvPoint(0, 0);
	CvPoint textPos = cvPoint(0, 0);

	// Draw a cross in the overlay buffer. 
	posStart = cvPoint(0, imageHeight / 2);
	posEnd = cvPoint(imageWidth, imageHeight / 2);
	cvLine(m_img, posStart, posEnd, color);

	posStart = cvPoint(imageWidth / 2, 0);
	posEnd = cvPoint(imageWidth / 2, imageHeight);
	cvLine(m_img, posStart, posEnd, color);

	for (int i = 1; i <= 30; i++)
	{
		//×ó²à
		if (0 == (i) % 5)
		{
			posStart = cvPoint(center_x - 10 * i, center_y);
			posEnd = cvPoint(center_x - 10 * i, center_y + 25);
			cvLine(m_img, posStart, posEnd, color);

			posStart = cvPoint(center_x - 10 * i, center_y);
			posEnd = cvPoint(center_x - 10 * i, center_y - 25);
			cvLine(m_img, posStart, posEnd, color);

			if (0 == (i % 2))
			{
				sprintf(szText,"%.2f um", -150.0*(i / 5));
				textPos = cvPoint(center_x - 10 * i, center_y - 25);
				cvPutText(m_img, szText, textPos, &font, color);
			}
			else
			{
				sprintf(szText, "%.2f um", -150.0*(i / 5));
				textPos = cvPoint(center_x - 10 * i, center_y + 25);
				cvPutText(m_img,szText, textPos, &font, color);
			}

		}
		else
		{
			posStart = cvPoint(center_x - 10 * i, center_y);
			posEnd = cvPoint(center_x - 10 * i, center_y + 15);
			cvLine(m_img, posStart, posEnd, color);

			posStart = cvPoint(center_x - 10 * i, center_y);
			posEnd = cvPoint(center_x - 10 * i, center_y - 15);
			cvLine(m_img, posStart, posEnd, color);
		}

		//ÓÒ²à
		if (0 == (i) % 5)
		{
			posStart = cvPoint(center_x + 10 * i, center_y);
			posEnd = cvPoint(center_x + 10 * i, center_y + 25);
			cvLine(m_img, posStart, posEnd, color);

			posStart = cvPoint(center_x + 10 * i, center_y);
			posEnd = cvPoint(center_x + 10 * i, center_y - 25);
			cvLine(m_img, posStart, posEnd, color);

			if (0 == (i % 2))
			{
				sprintf(szText, "%.2f um", 150.0*(i / 5));
				textPos = cvPoint(center_x + 10 * i, center_y - 25);
				cvPutText(m_img, szText, textPos, &font, color);
			}
			else
			{
				sprintf(szText, "%.2f um", 150.0*(i / 5));
				textPos = cvPoint(center_x + 10 * i, center_y + 25);
				cvPutText(m_img,szText, textPos, &font, color);
			}

		}
		else
		{
			posStart = cvPoint(center_x + 10 * i, center_y);
			posEnd = cvPoint(center_x + 10 * i, center_y + 15);
			cvLine(m_img, posStart, posEnd, color);

			posStart = cvPoint(center_x + 10 * i, center_y);
			posEnd = cvPoint(center_x + 10 * i, center_y - 15);
			cvLine(m_img, posStart, posEnd, color);
		}
	}

	for (int i = 1; i <= 80; i++)
	{
		//ÉÏ²à
		if (0 == (i) % 5)
		{
			posStart = cvPoint(center_x, center_y - 4 * i);
			posEnd = cvPoint(center_x + 25, center_y - 4 * i);
			cvLine(m_img, posStart, posEnd, color);

			posStart = cvPoint(center_x, center_y - 4 * i);
			posEnd = cvPoint(center_x - 25, center_y - 4 * i);
			cvLine(m_img, posStart, posEnd, color);

			//TextOut
			if (0 == i % 2)
			{
				sprintf(szText, "%.2f um", -60.0*(i / 5));
				textPos = cvPoint(center_x - 55, center_y - 4 * i);
				cvPutText(m_img, szText, textPos, &font, color);
			}
			else
			{
				sprintf(szText, "%.2f um", -60.0*(i / 5));
				textPos = cvPoint(center_x + 25, center_y - 4 * i);
				cvPutText(m_img, szText, textPos, &font, color);
			}


		}
		else
		{
			posStart = cvPoint(center_x, center_y - 4 * i);
			posEnd = cvPoint(center_x + 15, center_y - 4 * i);
			cvLine(m_img, posStart, posEnd, color);

			posStart = cvPoint(center_x, center_y - 4 * i);
			posEnd = cvPoint(center_x - 15, center_y - 4 * i);
			cvLine(m_img, posStart, posEnd, color);

		}

		//ÏÂ²à
		if (0 == (i) % 5)
		{
			posStart = cvPoint(center_x, center_y + 4 * i);
			posEnd = cvPoint(center_x + 25, center_y + 4 * i);
			cvLine(m_img, posStart, posEnd, color);

			posStart = cvPoint(center_x, center_y + 4 * i);
			posEnd = cvPoint(center_x - 25, center_y + 4 * i);
			cvLine(m_img, posStart, posEnd, color);

			//TextOut
			if (0 == i % 2)
			{
				sprintf(szText, "%.2f um", 60.0*(i / 5));
				textPos = cvPoint(center_x - 55, center_y + 4 * i);
				cvPutText(m_img,szText, textPos, &font, color);
			}
			else
			{
				sprintf(szText, "%.2f um", 60.0*(i / 5));
				textPos = cvPoint(center_x + 25, center_y + 4 * i);
				cvPutText(m_img,szText, textPos, &font, color);
			}


		}
		else
		{
			posStart = cvPoint(center_x, center_y + 4 * i);
			posEnd = cvPoint(center_x + 15, center_y + 4 * i);
			cvLine(m_img, posStart, posEnd, color);

			posStart = cvPoint(center_x, center_y + 4 * i);
			posEnd = cvPoint(center_x - 15, center_y + 4 * i);
			cvLine(m_img, posStart, posEnd, color);
		}
	}
}

void CvvImage::RotateImage(int angle, bool bClockWise){
	double rotateAngle = angle*CV_PI / 180;
	double sinAngle = sin(rotateAngle);
	double cosAngle = cos(rotateAngle);

	int orignWidth = m_img->width;
	int orignHeight = m_img->height;
	int rotateWidth = int(orignHeight*fabs(sinAngle) + orignWidth*fabs(cosAngle));
	int rotateHeight = int(orignHeight*fabs(cosAngle) + orignWidth*fabs(sinAngle));

	IplImage *pRotate = cvCreateImage(cvSize(rotateWidth, rotateHeight), m_img->depth, m_img->nChannels);
	cvZero(pRotate);


	int tempLenth = sqrt(orignWidth*orignWidth + orignHeight*orignHeight) + 10;
	int tempX = (tempLenth + 1) / 2 - orignWidth / 2;
	int tempY = (tempLenth + 1) / 2 - orignHeight / 2;

	int flag = -1;
	
	IplImage *pTemp = cvCreateImage(cvSize(tempLenth, tempLenth), m_img->depth, m_img->nChannels);
	cvZero(pTemp);

	cvSetImageROI(pTemp, cvRect(tempX, tempY, m_img->width, m_img->height));
	cvCopy(m_img, pTemp, nullptr);

	cvResetImageROI(pTemp);

	if (bClockWise)
		flag = 1;

	float m[6];
	int w = pTemp->width;
	int h = pTemp->height;
	m[0] = (float)(cos(flag*angle*CV_PI / 180));
	m[1] = (float)(sin(flag*angle*CV_PI / 180));
	m[3] = -m[1];
	m[4] = m[0];

	m[2] = w*0.5f;
	m[5] = h*0.5f;

	CvMat tempMat = cvMat(2, 3, CV_32F, m);
	cvGetQuadrangleSubPix(pTemp, pRotate, &tempMat);

	cvReleaseImage(&pTemp);

	CopyOf(pRotate);

	cvReleaseImage(&pRotate);
}