#include "SingletonBase.h"

CSingletonObserver*		CSingletonObserver::ms_pOnlyInstance = NULL;
CLockableObject			CSingletonObserver::ms_key;
bool					CSingletonObserver::m_bCanCreateInstance = true;

CSingletonBase::CSingletonBase(void)
{

}

CSingletonBase::~CSingletonBase()
{

}

const CLockableObject* CSingletonBase::GetManagerKey()
{
	//TODO: Use manager!
	CSingletonObserver *pCSingletonObserver = CSingletonObserver::Instance();

	if (pCSingletonObserver)
	{
		return reinterpret_cast<CSingletonBase*>(pCSingletonObserver)->GetKey();
	}
	else
	{
		return nullptr;
	}
}

bool CSingletonBase::AddToObserver(CSingletonBase* pCSingletonBase)
{
	if (pCSingletonBase == nullptr)
		return false;
	CSingletonObserver *pCSingletonObserver = CSingletonObserver::Instance();
	if (pCSingletonObserver && pCSingletonBase)
	{
		pCSingletonObserver->Add(pCSingletonBase);
	}
	else
	{
		return false;
	}
	return true;
}

bool CSingletonBase::AddToObserver()
{
	return AddToObserver(this);
}

//Observer

CSingletonObserver::CSingletonObserver() :
CSingletonBase()
{

}

CSingletonObserver::~CSingletonObserver()
{
	ms_key.Lock();
	m_bCanCreateInstance = FALSE;

	ms_pOnlyInstance = NULL;
	ms_key.Unlock();
}

inline void* CSingletonObserver::GetInstance(void** pOut)
{
	return *pOut = Instance();
}

inline CSingletonObserver* CSingletonObserver::Instance()
{
	if (m_bCanCreateInstance && !ms_pOnlyInstance)
	{

		ms_key.Lock();

		if (m_bCanCreateInstance && !ms_pOnlyInstance) // Double-checked locking
		{
			ms_pOnlyInstance = new CSingletonObserver;
		}
		ms_key.Unlock();
	}


	return ms_pOnlyInstance;
}


bool CSingletonObserver::Add(CSingletonBase* pCSingletonBase)
{
	//TODO: Cannot check if singleton is already there!
	m_stack.push(pCSingletonBase);
	return true;
}


bool CSingletonObserver::RemoveAll()
{
	CSingletonBase* p = NULL;

	ms_key.Lock();

	//Loop while the stack isn't empty
	while (!m_stack.empty())
	{
		p = m_stack.top();
		m_stack.pop();
		if (p)	delete p;
	}
	p = nullptr;
	ms_key.Unlock();
	return true;
}

bool CSingletonObserver::RemoveAllAndDie()
{

	CSingletonObserver* pCSingletonObserver = CSingletonObserver::Instance();

	//If we can get an instance of the manager...
	if (pCSingletonObserver)
	{
		pCSingletonObserver->RemoveAll();

		delete pCSingletonObserver;
		pCSingletonObserver = NULL;

		return true;
	}
	else
	{
		return true;
	}
}

const CLockableObject*	CSingletonObserver::GetKey()
{
	return &ms_key;
}