#ifndef __CUSTOMEDRAW_SINGLETON_H
#define __CUSTOMEDRAW_SINGLETON_H

#include "SingletonTemplate.h"
class CCustomeDrawSingleton :public CSingletonTemplate<CCustomeDrawSingleton>
{
public:
	~CCustomeDrawSingleton(){}
private:
	CCustomeDrawSingleton(){
		m_iRowCount = 0;;
		m_iColumnCount = 0;
		m_iRowDist = 0;
		m_iColumnWidth = 0;
		m_iPrintTimes = 0;
		m_bInfinitePrint = false;
		m_eCurrentType = POINT_ARRAY;
	}
	CCustomeDrawSingleton(const CCustomeDrawSingleton&);
	CCustomeDrawSingleton& operator=(const CCustomeDrawSingleton&);

	typedef enum DRAW_TYPE{POINT_ARRAY,LINE_ARRAY,GRID_ARRAY};
public:
	int m_iRowCount;
	int m_iColumnCount;
	int m_iRowDist;
	int m_iColumnWidth;
	int m_iPrintTimes;
	bool m_bInfinitePrint;
	DRAW_TYPE m_eCurrentType;
};
#endif //__CUSTOMEDRAW_SINGLETON_H