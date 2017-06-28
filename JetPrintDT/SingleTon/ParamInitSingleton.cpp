#include "ParamInitSingleton.h"


void CParamInitSingleton::CharToWchar(const char *constCharString, LPTSTR result)
{
	int   nLen = strlen(constCharString) + 1;
	int   nwLen = MultiByteToWideChar(CP_ACP, 0, constCharString, nLen, NULL, 0);

	TCHAR   *wString = nullptr;
	wString = new TCHAR[nwLen];


	MultiByteToWideChar(CP_ACP, 0, constCharString, nLen, wString, nwLen);
	_tcscpy_s(result, nwLen, wString);//   wcscpy(outWchar,wString);


	delete[] wString;
}


void CParamInitSingleton::WCharToChar(TCHAR *InWchar, char *OutStr)
{
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, InWchar, -1, NULL, 0, NULL, FALSE) + 1;
	WideCharToMultiByte(CP_OEMCP, NULL, InWchar, wcslen(InWchar), OutStr, dwNum, NULL, FALSE);
}

void CParamInitSingleton::ParaseNode(XMLElement* pElem){
	if (nullptr == pElem)
		return;
	else{
		//FLOW
		if (0 == strcmp(pElem->Value(), "FLOW")){
			m_dbFlow = pElem->DoubleText();
			m_pFlowNode = pElem;
			//return;
		}


		//VISA
		else if (0 == strcmp(pElem->Value(), "DC_OFFSET")){
			m_dbDCOffset = pElem->DoubleText();
			m_pDCOffsetNode = pElem;
			//return;
		}

		else if (0 == strcmp(pElem->Value(), "DC_PEAK")){
			m_dbDCPeak = pElem->DoubleText();
			m_pDCPeakNode = pElem;
			//return;
		}
		else if (0 == strcmp(pElem->Value(), "DC_FREQ")){
			m_dbDCFreq = pElem->DoubleText();
			m_pDCFreqNode = pElem;
			//return;
		}

		else if (0 == strcmp(pElem->Value(), "DC_RATIO")){
			m_dbDCRatio = pElem->DoubleText();
			m_pDCRatioNode = pElem;
			//return;
		}
			
		else if (0 == strcmp(pElem->Value(), "AC_PEAK")){
			m_dbACPeak = pElem->DoubleText();
			m_pACPeakNode = pElem;
			//return;
		}
			
		else if (0 == strcmp(pElem->Value(), "AC_OFFSET")){
			m_dbACOffset = pElem->DoubleText();
			m_pACOffsetNode = pElem;
			//return;
		}
		
		else if (0 == strcmp(pElem->Value(), "AC_FREQ")){
			m_dbACFreq = pElem->DoubleText();
			m_pACFreqNode = pElem;
			//return;
		}
			
		else if (0 == strcmp(pElem->Value(), "AC_RATIO")){
			m_dbACRatio = pElem->DoubleText();
			m_pACRatioNode = pElem; 
			//return;
		}
		

		//MOTION
		else if (0 == strcmp(pElem->Value(), "X_TARGET_POS")){
			m_dbXTargetPos = pElem->DoubleText();
			m_pXTargetPosNode = pElem;
			//return;
		}
		
		else if (0 == strcmp(pElem->Value(), "Y_TARGET_POS")){
			m_dbYTargetPos = pElem->DoubleText();
			m_pYTargetPosNode = pElem;
			//return;
		}
		else if (0 == strcmp(pElem->Value(), "X_STEP_LENTH")){
			m_dbXStepLenth = pElem->DoubleText();
			m_pXStepLenthNode = pElem;
			//return;
		}
		else if (0 == strcmp(pElem->Value(), "Y_STEP_LENTH")){
			m_dbYStepLenth = pElem->DoubleText();
			m_pYStepLenthNode = pElem;
			//return;
		}
		else if (0 == strcmp(pElem->Value(), "X_PRINTORIGN_POS")){
			m_dbXPrintOrignPos = pElem->DoubleText();
			m_pXPrintOrignPosNode = pElem;
		}
		else if (0 == strcmp(pElem->Value(), "Y_PRINTORIGN_POS")){
			m_dbYPrintOrignPos = pElem->DoubleText();
			m_pYPrintOrignPosNode = pElem;
		}

		//IMAGE
		else if (0 == strcmp(pElem->Value(), "IMAGE_WIDTH")){
			m_dbBMPWidth = pElem->DoubleText();
			m_pBMPWidthNode = pElem;
			//return;
		}
		else if (0 == strcmp(pElem->Value(), "IMAGE_HEIGHT")){
			m_dbBMPHeight = pElem->DoubleText();
			m_pBMPHeightNode = pElem;
			//return;
		}
		
		else if (0 == strcmp(pElem->Value(), "DROP_DIST")){
			m_dbBMPDropDist = pElem->DoubleText();
			m_pBMPDropDistNode = pElem;
			//return;
		}
		else if (0 == strcmp(pElem->Value(), "THRESHOLD")){
			m_dbThreshold = pElem->DoubleText();
			m_pThresholdNode = pElem;
		}
		
		else if (0 == strcmp(pElem->Value(), "IMAGE_ORIGN_WIDTH")){
			m_dbDXFOrignWidth = pElem->DoubleText();
			m_pDXFOrignWidthNode = pElem;
			//return;
		}
		else if (0 == strcmp(pElem->Value(), "IMAGE_ORIGN_HEIGHT")){
			m_dbDXFOrignHeight = pElem->DoubleText();
			m_pDXFOrignWidthNode = pElem;
			//return;
		}
		else if (0 == strcmp(pElem->Value(), "DEST_WIDTH")){
			m_dbDXFDestWidth = pElem->DoubleText();
			m_pDXFDestWidthNode = pElem;
			//return;
		}
		else if (0 == strcmp(pElem->Value(), "DEST_HEIGHT")){
			m_dbDXFDestHeight = pElem->DoubleText();
			m_pDXFDestHeightNode = pElem;
		}

		else if (0 == strcmp(pElem->Value(), "DOWNLOAD_SPEED")){
			m_dbDownloadSpeed = pElem->DoubleText();
			m_pDownloadSpeedNode = pElem;
		}

		else if (0 == strcmp(pElem->Value(), "LOCATE_CAMERA_XPOS")){
			m_dbXLocateCameraPos = pElem->DoubleText();
			m_pXLocateCameraPosNode = pElem;
		}

		else if (0 == strcmp(pElem->Value(), "LOCATE_CAMERA_YPOS")){
			m_dbYLocateCameraPos = pElem->DoubleText();
			m_pYLocateCameraPosNode = pElem;
		}

		else if (0 == strcmp(pElem->Value(), "JET_XPOS")){
			m_dbXJetPos = pElem->DoubleText();
			m_pXJetPosNode = pElem;
		}

		else if (0 == strcmp(pElem->Value(), "JET_YPOS")){
			m_dbYJetPos = pElem->DoubleText();
			m_pYJetPosNode = pElem;
		}
			
		else if (0 == strcmp(pElem->Value(), "FILE_PATH")){
			m_FilePath.Format(_T("%s"), pElem->GetText());
			m_pFilePathNode = pElem;
			//return;
		}
		else if (0 == strcmp(pElem->Value(), "FILE_NAME")){
			m_FileName.Format(_T("%s"), pElem->GetText());
			m_pFileNameNode = pElem;
		}

		else if (0 == strcmp(pElem->Value(), "WATCH_CAMERA_ID")){
			m_iwatchCameraID = pElem->IntText();
			m_pWatchCameraIDNode = pElem;
		}

		else if (0 == strcmp(pElem->Value(), "LOCATE_CAMERA_ID")){
			m_ilocateCameraID = pElem->IntText();
			m_pLocateCameraIDNode = pElem;
		}

		if (false == pElem->NoChildren()){
			XMLElement *pTmepNode = pElem->FirstChildElement();
			//pNode = pNode->FirstChild();
			ParaseNode(pTmepNode);
		}

		if (pElem&&pElem->NextSibling()){
			XMLElement *pTmepNode = pElem->NextSiblingElement();
			//pNode = pNode->NextSibling();
			ParaseNode(pTmepNode);
		}
	}
}


void CParamInitSingleton::MbcsToUtf8(char *szFilenameIn, char*szFilenameOut)
{
	wchar_t   *pwchar = 0;
	int len = 0;
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	len = MultiByteToWideChar(codepage, 0, szFilenameIn, -1, NULL, 0);
	pwchar = new wchar_t[len];
	memset(pwchar, 0, sizeof(wchar_t)*(len));
	if (pwchar != 0)
	{
		len = MultiByteToWideChar(codepage, 0, szFilenameIn, -1, pwchar, len);
		if (len != 0)
		{
			len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, 0, 0, 0, 0);
			if (NULL == szFilenameOut)
			{
				szFilenameOut = new char[len];
				memset(szFilenameOut, 0, sizeof(char)*len);
			}

			if (szFilenameOut != 0)
			{
				len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, szFilenameOut, len, 0, 0);
			}
			delete[]pwchar;
		}
	}
}

void CParamInitSingleton::DecodeXml(){
	CString appPath = CHostInstance::Instance()->GetAppPath();
	CString connector = _T("\\");
	CString fullPath = appPath.GetBuffer(0) + connector + m_XmlFile.GetBuffer(0);

	XMLError errorCode = XML_SUCCESS;
	errorCode = m_ParamDoc.LoadFile(CW2A(fullPath));

	if (errorCode == XML_SUCCESS){
		XMLElement *rootElem = m_ParamDoc.RootElement();

		ParaseNode(rootElem);
	}
}


void CParamInitSingleton::WriteXMLFile(){

	CString appPath = CHostInstance::Instance()->GetAppPath();
	CString connector = _T("\\");
	CString fullPath = appPath.GetBuffer(0) + connector + m_XmlFile.GetBuffer(0);
	m_ParamDoc.SaveFile(CW2A(fullPath));
}