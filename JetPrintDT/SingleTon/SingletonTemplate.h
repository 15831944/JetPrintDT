#ifndef __SINGLETON_TEMPLATE_H
#define __SINGLETON_TEMPLATE_H

#include "../stdafx.h"

#include "SingletonBase.h"

template<typename T= CSingletonBase>
class CSingletonTemplate :public CSingletonBase
{
protected:
	//Constructor has to be private as part of the singleton way
	CSingletonTemplate< T >() : CSingletonBase()
	{
	};
public:
	//Destructor must be virtual and public so everything can be cleaned up properly.
	virtual ~CSingletonTemplate()
	{
		ms_key.Lock();

		ms_bSingletonCreatedFlag = ms_bCanCreateInstance = false;

		//We are no longer pointing at anything...
		ms_pOnlyInstance = nullptr;
		ms_key.Unlock();
	};



	//Returns Critical Section key from Singleton
	const CLockableObject*	GetKey()
	{
		return &ms_key;
	};

	//Implement Generic instance grabbing techique from base
	virtual void*	GetInstance(void** pOut)
	{
		return *pOut = Instance();
	};




	//-----------------------------------------------------------------------------
	// Name:	Instance()
	// Desc:	Returns only instance of contained object if available, otherwise 
	//  returns NULL.
	// Returns:	Can return NULL.  Do NOT clean this up, unless not using 
	//			CSingletonObserver
	//-----------------------------------------------------------------------------		
	static	T* Instance()
	{
		if (ms_bCanCreateInstance && !ms_bSingletonCreatedFlag)
		{
			//The Booleans say we should check to see if an instance needs to be created.
			//Now we use the more reliable, but time consuming CRITICAL_SECTION lock

			//Lock this section with this singleton's critical section object				
			ms_key.Lock();


			//"Double-Checked Locking" - we are now in the Mutex section, which means only one thread at a time should be allowed through here.
			// This means that testing the boolean now will 100% be correct, as this thread of execution can't be preempted out before another completes
			if (ms_bCanCreateInstance && !ms_bSingletonCreatedFlag)
			{
				//Create the only Instance of the Singleton
				ms_pOnlyInstance = new T;

				//Set the instance creation flag
				ms_bSingletonCreatedFlag = (ms_pOnlyInstance == nullptr ? false : true);

				//Add this singleton to be Observed, so it's lifetime can be properly controlled
				AddToObserver(ms_pOnlyInstance);
			}
			ms_key.Unlock();
		}

		//Could be NULL if we aren't allowed to create a Singleton
		return ms_pOnlyInstance;
	};



	//If the singleton has been destroyed, it will prevent further versions of itself created unless told otherwise.
	//This is to prevent methods erronously calling Instance after destruction causing a Singleton to be created that may never be cleaned up.
	static void EnableInstanceCreation(bool bEnabled = true) { ms_bCanCreateInstance = bEnabled; }



	//Returns TRUE if further instance creation is permitted
	static bool CanCreateInstance() { return ms_bCanCreateInstance; }



protected:

	//m_iTest is only an attribute for the benefit of test stub
	int		m_iTest;

	//ms_pOnlyInstance holds the only Instance of this Singleton
	//static TSingleton<T>* ms_pOnlyInstance;
	static T* ms_pOnlyInstance;

	//Only needs to be static because we are locking something classwide rather then instance wide
	static	CLockableObject ms_key;

	//Flags if we are allowed to create an instance if necessary.  FALSE when Singleton destroyed, but can be reset via mutators to allow another instance.
	//	This flag means there can only ever be 0..1 Instance in memory at one time, and prevents recreation after destruction by mistake.
	//The flag is made volatile to prevent the result being erronously cached in the registers by the compiler while another thread may have changed its value.
	static	volatile bool ms_bCanCreateInstance;

	//volatile - Member data is loaded from memory each time it is accessed; disables certain optimizations.
	// - this ensures that the compiler does not optimise out our cheap lock hint, before going on to do the more expensive CRITICAL_SECTION mutex lock
	static	volatile bool ms_bSingletonCreatedFlag;
};

template < class T > T*					CSingletonTemplate< T >::ms_pOnlyInstance = nullptr;
template < class T > CLockableObject	CSingletonTemplate< T >::ms_key;
template < class T > volatile bool		CSingletonTemplate< T >::ms_bCanCreateInstance = true;
template < class T > volatile bool		CSingletonTemplate< T >::ms_bSingletonCreatedFlag = false;
#endif //__SINGLETON_TEMPLATE_H