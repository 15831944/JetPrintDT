#include "ParamTable.h"
#include "../SingleTon/HostInstance.h"

CParamTable::CParamTable()
{
	//these must match table above
	SetItem(0, "CurrentDate", MYDB_TYPE_TEXT, &m_CurrentDate);
	SetItem(1, "Material", MYDB_TYPE_TEXT, &m_Material);
	SetItem(2, "Flux", MYDB_TYPE_DOUBLE, &m_FlowRate);
	SetItem(3, "DC_OFFSET", MYDB_TYPE_DOUBLE, &m_DCOffSet);
	SetItem(4, "AC_OFFSET", MYDB_TYPE_DOUBLE, &m_ACOffset);
	//5
	SetItem(5, "AC_PEAK", MYDB_TYPE_DOUBLE, &m_ACPeak);
	SetItem(6, "AC_RATIO", MYDB_TYPE_DOUBLE, &m_ACRatio);
	SetItem(7, "AC_FREQ", MYDB_TYPE_DOUBLE, &m_ACFreq);

	szTableName = new char[strlen(PARAM_TABLE) + 1];
	memset(szTableName, 0, sizeof(char)*(strlen(PARAM_TABLE) + 1));

	memcpy_s(szTableName,strlen(PARAM_TABLE)+1,PARAM_TABLE,strlen(PARAM_TABLE)+1);
}

CParamTable::~CParamTable()
{
	if (NULL != szTableName)
	{
		delete[]szTableName;
		szTableName = NULL;
	}
}

const char* CParamTable::TableName()
{
	return szTableName;
}

bool CParamTable::Open(const char* pFile)
{

	char szName[128] = { 0 };
	MbcsToUtf8(const_cast<char*>(pFile), szName);
	if (SQLiteTbl::Open(szName))
	{
		if (Query())
			return true;
	}
	//anything goes wrong, close and return false
	Close();
	return false;
}

int CParamTable::ItemCount()
{
	return 8;
}

void CParamTable::MbcsToUtf8(char *szFilenameIn, char*szFilenameOut)
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

bool CParamTable::open(){
	bool bResult = false;
	CHostInstance* pInst = CHostInstance::Instance();
	CString appPath = pInst->GetAppPath();
	CString database = _T("\\JetTable.sqlite");
	CString path = appPath + database;

	int nLenth = 0;
	nLenth = WideCharToMultiByte(CP_ACP, 0, path, -1, NULL, 0, NULL, NULL);
	char * dataBaseName = NULL;
	dataBaseName = new char[nLenth + 1];
	memset(dataBaseName, 0, sizeof(char)*nLenth);
	WideCharToMultiByte(CP_ACP, 0, path, -1, dataBaseName, nLenth, NULL, NULL);

	bResult = Open(dataBaseName);

	if (NULL != dataBaseName)
	{
		delete[]dataBaseName;
		dataBaseName = NULL;
	}

	return bResult;
}