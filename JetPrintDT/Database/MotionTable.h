#ifndef __MOTION_TABLE_H
#define __MOTION_TABLE_H

#include "SQLiteTbl.h"
#include "../SingleTon/SingletonTemplate.h"
#define MOTION_TABLE  "MotionTable"

class CMotionTable :public SQLiteTbl,public CSingletonTemplate<CMotionTable>
{
	friend class CSingletonTemplate<CMotionTable>;
	//public members:
public:
	SQLiteString m_CurrentDate;
	double m_XOrignPos;
	double m_YOrignPos;
	double m_XStepLenth;
	double m_YStepLenth;
	double m_MoveSpeed;
	double m_XOffSet;
	double m_YOffSet;

	int m_nColumnCount;
	char * szTableName;

	//private member funcs:
private:
	CMotionTable();
	CMotionTable(const CMotionTable& table);
	CMotionTable& operator=(const CMotionTable& table);

	//public member funcs;
public:
	virtual const char* TableName();
	virtual bool open();
	virtual bool Open(const char* szFileName);
	virtual int ItemCount();
	void MbcsToUtf8(char *szFilenameIn, char*szFilenameOut);
	
	virtual ~CMotionTable();
};


#endif //__MOTION_TABLE_H