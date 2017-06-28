#ifndef __SINGLETON_BASE_H
#define __SINGLETON_BASE_H
#include "../stdafx.h"
#include "afx.h"
#include <stack>
class CLockableObject
{
public:
	inline CLockableObject() { InitializeCriticalSection(&m_CriticalSection); }

	//Make below virtual if planning to inherit from this class!
	virtual inline ~CLockableObject() { DeleteCriticalSection(&m_CriticalSection); }

	inline void Lock() { EnterCriticalSection(&m_CriticalSection); }
	inline void Unlock() { LeaveCriticalSection(&m_CriticalSection); }

private:
	CRITICAL_SECTION m_CriticalSection;
};


class CSingletonBase
{

public:
	// Destructor should be virtual and public so it can be more easily destroyed
	virtual ~CSingletonBase();

protected:
	CSingletonBase(void);

	//Adds a singleton to the Singleton Instance Manager
	static bool AddToObserver(CSingletonBase* pCSingletonBase);

	//Adds THIS singleton to the Singleton Instance Manager
	bool AddToObserver();

	//Returns Critical Section (key) from CSingletonObserver
	static const CLockableObject*	GetManagerKey();

	//Returns Critical Section key from Singleton
	virtual const CLockableObject*	GetKey() = 0; //{ return nullptr; }//=0;


public:
	//Generic instance grabbing techique
	virtual void*	GetInstance(void** pOut){ return nullptr; };//=0;
};

class CSingletonObserver : public CSingletonBase
{
public:
	virtual ~CSingletonObserver();

	//Adds a singleton to be observed
	bool Add(CSingletonBase* pCSingletonBase);

	//Returns TRUE if removal successful, or if it didn't exist anyway
	bool RemoveAll();


	//Removes everything from stack, then kills itself
	static bool RemoveAllAndDie();


protected:
	CSingletonObserver();

	const CLockableObject*	GetKey();

	static CSingletonObserver* ms_pOnlyInstance;

	static	CLockableObject ms_key;

	static	bool m_bCanCreateInstance;

	std::stack< CSingletonBase* > m_stack;


public:
	//Implement Generic instance grabbing techique from base
	void*	GetInstance(void** pOut);
	static	CSingletonObserver* Instance();

	//If the singleton has been destroyed, it will prevent further versions of itself created unless told otherwise.
	//This is to prevent methods erronously calling Instance after destruction causing a Singleton to be created that may never be cleaned up.
	static	inline void EnableInstanceCreation(bool bEnabled = true) { m_bCanCreateInstance = bEnabled; }

	//Returns TRUE if further instance creation is permitted
	static	inline bool CanCreateInstance() { return m_bCanCreateInstance; }
};



#endif //__SINGLETON_BASE_H