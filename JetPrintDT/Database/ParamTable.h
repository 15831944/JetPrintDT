#ifndef __PARAM_TABLE_H
#define __PRAME_TABLE_H

#include "SQLiteTbl.h"
#include "../SingleTon/SingletonTemplate.h"
#define PARAM_TABLE  "ParamTable"

class CParamTable :public SQLiteTbl,public CSingletonTemplate<CParamTable>
{
	friend class CSingletonTemplate<CParamTable>;
	//public members:
public:
	SQLiteString m_CurrentDate;
	SQLiteString m_Material;
	double m_FlowRate;
	double m_DCOffSet;
	double m_ACOffset;
	double m_ACPeak;
	double m_ACRatio;
	double m_ACFreq;

	int m_nColumnCount;
	char * szTableName;


	//private member funcs:
private:
	CParamTable();
	CParamTable(const CParamTable& table);
	CParamTable& operator=(const CParamTable& table);

	//public member funcs;
public:
	virtual const char* TableName();
	virtual bool open();
	virtual bool Open(const char* szFileName);
	virtual int ItemCount() ;
	void MbcsToUtf8(char *szFilenameIn, char*szFilenameOut);
	virtual ~CParamTable();
};


#endif //__PARAM_TABLE_H