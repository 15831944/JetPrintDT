#define _CRT_SECURE_NO_WARNINGS

#ifndef __PARAMINIT_SINGLETON_H
#define __PARAMINIT_SINGLETON_H

#include "SingletonTemplate.h"
#include "HostInstance.h"
#include "../DataStruct/tinyxml2.h"
using namespace tinyxml2;


class CParamInitSingleton :public CSingletonTemplate<CParamInitSingleton>
{
	friend class CSingletonTemplate<CParamInitSingleton>;
public:
	virtual ~CParamInitSingleton(){}
private:
	CParamInitSingleton(){
		m_dbFlow = 0.0;
		m_pFlowNode = nullptr;
		m_dbACOffset = 0.0;
		m_pACOffsetNode = nullptr;
		m_dbACPeak = 0.0;
		m_pACPeakNode = nullptr;
		m_dbACFreq = 0.0;
		m_pACFreqNode = nullptr;
		m_dbACRatio = 0.0;
		m_pACRatioNode = nullptr;
		m_dbDCOffset = 0.0;
		m_pDCOffsetNode = nullptr;
		m_dbDCPeak = 0.0;
		m_pDCPeakNode = nullptr;
		m_dbDCFreq = 0.0;
		m_pDCFreqNode = nullptr;
		m_dbDCRatio = 0.0;
		m_pDCRatioNode = nullptr;
		m_dbXTargetPos = 0.0;
		m_pXTargetPosNode = nullptr;
		m_dbYTargetPos = 0.0;
		m_pYTargetPosNode = nullptr;
		m_dbXStepLenth = 0.0;
		m_pXStepLenthNode = nullptr;
		m_dbYStepLenth = 0.0;
		m_pYStepLenthNode = nullptr;
		m_dbXPrintOrignPos = 0.0;
		m_pXPrintOrignPosNode = nullptr;
		m_dbYPrintOrignPos = 0.0;
		m_pYPrintOrignPosNode = nullptr;
		m_dbDXFOrignWidth = 0.0;
		m_pDXFOrignWidthNode = nullptr;
		m_dbDXFOrignHeight = 0.0;
		m_pDXFOrignWidthNode = nullptr;
		m_dbDXFDestWidth = 0.0;
		m_pDXFDestWidthNode = nullptr;
		m_dbDXFDestHeight = 0.0;
		m_pDXFDestHeightNode = nullptr;
		m_dbBMPWidth = 0.0;
		m_pBMPWidthNode = nullptr;
		m_dbBMPHeight = 0.0;
		m_pBMPHeightNode = nullptr;
		 m_dbBMPDropDist = 0.0;
		 m_pBMPDropDistNode = nullptr;
		 m_dbThreshold = 0.0;
		 m_pThresholdNode = nullptr;
		 m_iwatchCameraID = 0;
		 m_pWatchCameraIDNode = nullptr;
		 m_ilocateCameraID = 0;
		 m_pLocateCameraIDNode = nullptr;

		 m_FilePath = _T("");
		 m_pFilePathNode = nullptr;
		 m_FileName = _T("");
		 m_pFileNameNode = nullptr;

		 m_dbSpeed = 0.0;
		 m_pSpeedNode = nullptr;
		 m_iPlacodeChioce = 0;
		 m_pPlacodeChioceNode = nullptr;

		 m_dbXLocateCameraPos = 0.0;
		 m_pXLocateCameraPosNode = nullptr;
		 m_dbYLocateCameraPos = 0.0;
		 m_pYLocateCameraPosNode = nullptr;
		 m_dbXJetPos = 0.0;
		 m_pXJetPosNode = nullptr;
		 m_dbYJetPos = 0.0;
		 m_pYJetPosNode = nullptr;

		 m_dbDownloadSpeed = 0.0f;
		 m_XmlFile = _T("Param.xml");
		 m_StartTime = _T("");
	}
	CParamInitSingleton(const CParamInitSingleton&);
	CParamInitSingleton& operator=(const CParamInitSingleton&);

private:
	//print view param
	double m_dbFlow;		//for  read
	XMLNode *m_pFlowNode;	//for  write

	double m_dbACOffset;
	XMLNode *m_pACOffsetNode;

	double m_dbACPeak;
	XMLNode *m_pACPeakNode;

	double m_dbACFreq;
	XMLNode *m_pACFreqNode;

	double m_dbACRatio;
	XMLNode *m_pACRatioNode;

	double m_dbDCOffset;
	XMLNode *m_pDCOffsetNode;

	double m_dbDCPeak;
	XMLNode *m_pDCPeakNode;

	double m_dbDCFreq;
	XMLNode *m_pDCFreqNode;

	double m_dbDCRatio;
	XMLNode *m_pDCRatioNode;

	double m_dbXTargetPos;
	XMLNode *m_pXTargetPosNode;

	double m_dbYTargetPos;
	XMLNode *m_pYTargetPosNode;

	double m_dbXStepLenth;
	XMLNode *m_pXStepLenthNode;

	double m_dbYStepLenth;
	XMLNode *m_pYStepLenthNode;

	double m_dbXPrintOrignPos;
	XMLNode *m_pXPrintOrignPosNode;

	double m_dbYPrintOrignPos;
	XMLNode *m_pYPrintOrignPosNode;

	double m_dbDXFOrignWidth;
	XMLNode *m_pDXFOrignWidthNode;

	double m_dbDXFOrignHeight;
	XMLNode *m_pDXFOrignHeightNode;

	double m_dbDXFDestWidth;
	XMLNode *m_pDXFDestWidthNode;

	double m_dbDXFDestHeight;
	XMLNode *m_pDXFDestHeightNode;

	double m_dbBMPWidth;
	XMLNode *m_pBMPWidthNode;

	double m_dbBMPHeight;
	XMLNode *m_pBMPHeightNode;

	double m_dbBMPDropDist;
	XMLNode *m_pBMPDropDistNode;

	double m_dbThreshold;
	XMLNode *m_pThresholdNode;

	int m_iwatchCameraID;
	XMLNode *m_pWatchCameraIDNode;

	int m_ilocateCameraID;
	XMLNode *m_pLocateCameraIDNode;

	CString m_FilePath;
	XMLNode *m_pFilePathNode;

	CString m_FileName;
	XMLNode *m_pFileNameNode;

	double m_dbDownloadSpeed;
	XMLNode *m_pDownloadSpeedNode;

	double m_dbSpeed;
	XMLNode *m_pSpeedNode;

	int m_iPlacodeChioce;
	XMLNode *m_pPlacodeChioceNode;

	double m_dbXLocateCameraPos;
	XMLNode *m_pXLocateCameraPosNode;

	double m_dbYLocateCameraPos;
	XMLNode *m_pYLocateCameraPosNode;

	double m_dbXJetPos;
	XMLNode *m_pXJetPosNode;

	double m_dbYJetPos;
	XMLNode *m_pYJetPosNode;

	CString m_XmlFile;
	CString m_StartTime;

	tinyxml2::XMLDocument m_ParamDoc;

public:
	double GetFlow() const{
		return m_dbFlow;
	}
	void SetFlow(double dbFlow){
		if (nullptr != m_pFlowNode){
			m_pFlowNode->ToElement()->SetText(dbFlow);
		}
	}


	double GetACOffset() const{
		return m_dbACOffset;
	}
	void SetACOffset(double dbOffset){
		if (nullptr != m_pACOffsetNode){
			m_pACOffsetNode->ToElement()->SetText(dbOffset);
		}
	}

	double GetACPeak() const{
		return m_dbACPeak;
	}
	void SetACPeak(double peak){
		if (nullptr != m_pACPeakNode){
			m_pACPeakNode->ToElement()->SetText(peak);
		}
	}

	double GetACFreq() const{
		return m_dbACFreq;
	}
	void SetACFreq(double freq){
		if (nullptr != m_pACFreqNode){
			m_pACFreqNode->ToElement()->SetText(freq);
		}
	}

	double GetACRatio() const{
		return m_dbACRatio;
	}
	void SetACRatio(double ratio){
		if (nullptr != m_pACRatioNode){
			m_pACRatioNode->ToElement()->SetText(ratio);
		}
	}


	double GetDCOffset() const{
		return m_dbDCOffset;
	}
	void SetDCOffset(double offset){
		if (nullptr != m_pDCOffsetNode){
			m_pDCOffsetNode->ToElement()->SetText(offset);
		}
	}

	double GetDCPeak() const{
		return m_dbDCPeak;
	}
	void SetDCPeak(double peak){
		if (nullptr != m_pDCPeakNode){
			m_pDCPeakNode->ToElement()->SetText(peak);
		}
	}

	double GetDCFreq() const{
		return m_dbDCFreq;
	}
	void SetDCFreq(double freq){
		if (nullptr != m_pDCFreqNode){
			m_pDCFreqNode->ToElement()->SetText(freq);
		}
	}

	double GetDCRatio() const{
		return m_dbDCRatio;
	}
	void SetDCRatio(double ratio){
		if (nullptr != m_pDCRatioNode){
			m_pDCRatioNode->ToElement()->SetText(ratio);
		}
	}

	double GetXTargetPos() const{
		return m_dbXTargetPos;
	}
	void SetXTargetPos(double pos){
		if (nullptr != m_pXTargetPosNode){
			m_pXTargetPosNode->ToElement()->SetText(pos);
		}
	}

	double GetYTargetPos() const{
		return m_dbYTargetPos;
	}
	void SetYTargetPos(double pos){
		if (nullptr != m_pYTargetPosNode){
			m_pYTargetPosNode->ToElement()->SetText(pos);
		}
	}

	double GetXStepLenth() const{
		return m_dbXStepLenth;
	}
	void SetXStepLenth(double step){
		if (nullptr != m_pXStepLenthNode){
			m_pXStepLenthNode->ToElement()->SetText(step);
		}
	}

	double GetYStepLenth() const{
		return m_dbYStepLenth;
	}
	void SetYStepLenth(double dbLenth){
		if (nullptr != m_pYStepLenthNode){
			m_pYStepLenthNode->ToElement()->SetText(dbLenth);
		}
	}

	double GetXPrintOrignPos() const{
		return m_dbXPrintOrignPos;
	}

	void SetXPrintOrignPos(double dbX){
		if (nullptr != m_pXPrintOrignPosNode){
			m_pXPrintOrignPosNode->ToElement()->SetText(dbX);
		}
	}

	double GetYPrintOrignPos() const{
		return m_dbYPrintOrignPos;
	}

	void SetYPrintOrignPos(double dbY){
		if (nullptr != m_pYPrintOrignPosNode){
			m_pYPrintOrignPosNode->ToElement()->SetText(dbY);
		}
	}

	double GetDXFOrignWidth() const{
		return m_dbDXFOrignWidth;
	}


	double GetDXFOrignHeight() const{
		return m_dbDXFOrignHeight;
	}

	double GetDXFDestWidth() const {
		return m_dbDXFDestWidth;
	}
	void SetDXFDestWidth(double width){
		if (nullptr != m_pDXFDestWidthNode){
			m_pDXFDestWidthNode->ToElement()->SetText(width);
		}
	}

	double GetDXFDestHeight() const{
		return m_dbDXFDestHeight;
	}
	void SetDXFDestHeight(double height){
		if (nullptr != m_pDXFDestHeightNode){
			m_pDXFDestHeightNode->ToElement()->SetText(height);
		}
	}

	double GetBMPWidth() const {
		return m_dbBMPWidth;
	}
	void SetBMPWidth(double width){
		if (nullptr != m_pBMPWidthNode){
			m_pBMPWidthNode->ToElement()->SetText(width);
		}
	}

	double GetBMPHeight() const{
		return m_dbBMPHeight;
	}
	void SetBMPHeight(double height){
		if (nullptr != m_pBMPHeightNode){
			m_pBMPHeightNode->ToElement()->SetText(height);
		}
	}

	double GetBMPDropDist() const{
		return m_dbBMPDropDist;
	}
	void SetBMPDropDist(double dist){
		if (nullptr != m_pBMPDropDistNode){
			m_pBMPDropDistNode->ToElement()->SetText(dist);
		}
	}

	double GetBMPThreshold() const {
		return m_dbThreshold;
	}
	void SetBMPThreshold(double threshold){
		if (nullptr != m_pThresholdNode){
			m_pThresholdNode->ToElement()->SetText(threshold);
		}
	}

	double GetDownloadSpeed() const{
		return m_dbDownloadSpeed;
	}

	void SetDownloadSpeed(double speed){
		if (nullptr != m_pDownloadSpeedNode){
			m_pDownloadSpeedNode->ToElement()->SetText(speed);
		}
	}

	CString GetFilePath() const {
		return m_FilePath;
	}
	void SetFilePaht(CString path){
		if (nullptr != m_pFilePathNode){
			m_pFilePathNode->ToElement()->SetText(CW2A(path));
		}
	}

	CString GetFileName() const{
		return m_FileName;
	}
	void SetFileName(CString filename){
		if (nullptr != m_pFileNameNode){
			m_pFileNameNode->ToElement()->SetText(CW2A(filename));
		}
	}

	double GetPrintSpeed() const{
		return m_dbSpeed;
	}
	void SetPrintSpeed(double speed){
		if (nullptr != m_pSpeedNode){
			m_pSpeedNode->ToElement()->SetText(speed);
		}
	}


	int GetPlacodeChioce() const{
		return m_iPlacodeChioce;
	}
	void SetPlacodeChioce(int ichioce){
		if (nullptr != m_pPlacodeChioceNode){
			m_pPlacodeChioceNode->ToElement()->SetText(ichioce);
		}
	}

	double GetXLocateCameraPos() const{
		return m_dbXLocateCameraPos;
	}

	void SetXLocateCameraPos(double dbPos){
		if (nullptr != m_pXLocateCameraPosNode){
			m_pXLocateCameraPosNode->ToElement()->SetText(dbPos);
		}
	}

	double GetYLocateCameraPos() const{
		return m_dbYLocateCameraPos;
	}

	void SetYLocateCameraPos(double dbPos){
		if (nullptr != m_pYLocateCameraPosNode){
			m_pYLocateCameraPosNode->ToElement()->SetText(dbPos);
		}
	}

	double GetXJetPos() const{
		return m_dbXJetPos;
	}

	void SetXJetPos(double dbPos){
		if (nullptr != m_pXJetPosNode){
			m_pXJetPosNode->ToElement()->SetText(dbPos);
		}
	}

	double GetYJetPos() const{
		return m_dbYJetPos;
	}

	void SetYJetPos(double dbPos){
		if (nullptr != m_pYJetPosNode){
			m_pYJetPosNode->ToElement()->SetText(dbPos);
		}
	}

	int GetWatchCameraID() const{
		return m_iwatchCameraID;
	}

	void SetWatchCameraID(int iCameraID){
		if (nullptr != m_pWatchCameraIDNode){
			m_pWatchCameraIDNode->ToElement()->SetText(iCameraID);
		}
	}

	int GetLocateCameraID() const{
		return m_ilocateCameraID;
	}

	void SetLocateCameraID(int iCameraID){
		if (nullptr != m_pLocateCameraIDNode){
			m_pLocateCameraIDNode->ToElement()->SetText(iCameraID);
		}
	}

	CString GetStartTime() const{
		return m_StartTime;
	}

	void InitializeStartTime(){
		CTime startTime = CTime::GetCurrentTime();
		m_StartTime = startTime.Format(_T("%H:%M:%S"));
	}
	
	void init(){
		DecodeXml();
		InitializeStartTime();
	}
	void WriteXMLFile();
private:
	void DecodeXml();
	void ParaseNode(XMLElement* pElem);
	void CharToWchar(const char *constCharString, LPTSTR result);
	void WCharToChar(TCHAR *InWcharm, char* result);
	void MbcsToUtf8(char *szFilenameIn, char*szFilenameOut);
	
};

#endif //__PARAMINIT_SINGLETON_H