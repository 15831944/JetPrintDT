#include "MotionTable.h"
#include "../SingleTon/HostInstance.h"

CMotionTable::CMotionTable()
{
	//these must match table above
	SetItem(0, "CurrentDate", MYDB_TYPE_TEXT, &m_CurrentDate);
	SetItem(1, "XOrignPos", MYDB_TYPE_DOUBLE, &m_XOrignPos);
	SetItem(2, "YOrignPos", MYDB_TYPE_DOUBLE, &m_YOrignPos);
	SetItem(3, "XStepLenth", MYDB_TYPE_DOUBLE, &m_XStepLenth);
	//5
	SetItem(4, "YStepLenth", MYDB_TYPE_DOUBLE, &m_YStepLenth);
	SetItem(5, "MoveSpeed", MYDB_TYPE_DOUBLE, &m_MoveSpeed);
	SetItem(6, "XOffSet", MYDB_TYPE_DOUBLE, &m_XOffSet);
	SetItem(7, "YOffSet", MYDB_TYPE_DOUBLE, &m_YOffSet);

	szTableName = new char[strlen(MOTION_TABLE) + 1];
	memset(szTableName, 0, sizeof(char)*(strlen(MOTION_TABLE) + 1));
}

CMotionTable::~CMotionTable()
{
	if (NULL != szTableName)
	{
		delete[] szTableName;
		szTableName = NULL;
	}
}

const char* CMotionTable::TableName()
{
	memcpy_s(szTableName, strlen(MOTION_TABLE) + 1, MOTION_TABLE, strlen(MOTION_TABLE) + 1);
	return szTableName;
}

bool CMotionTable::Open(const char* pFile)
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

int CMotionTable::ItemCount()
{
	return 8;
}

void CMotionTable::MbcsToUtf8(char *szFilenameIn, char*szFilenameOut)
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
			delete []pwchar;
		}
	}
}

bool CMotionTable::open(){
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