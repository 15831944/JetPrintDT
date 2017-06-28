#ifndef _CAMERA_SHARED_BUFFER_H
#define _CAMERA_SHARED_BUFFER_H
#include "SingletonTemplate.h"
#include <string>
#include <vector>
using namespace std;

#define CAMERA_WIDTH 1600
#define CAMERA_HEIGHT 900
#define IMAGE_SIZE CAMERA_WIDTH*CAMERA_HEIGHT*3

enum { CAMERA_OPEN, CAMERA_CLOSE, CAMERA_START_GRAB, CAMERA_STOP_GRAB, CAMERA_SWITCH_DEVICE,CAMERA_EXIT };

class CCameraParam:public CSingletonTemplate<CCameraParam>{
	friend CSingletonTemplate<CCameraParam>;
private:
	CCameraParam(){
		m_bOpenWatchCamera = false;
	    m_bOpenLocateCamera = false;
		m_bWatchCameraGrab = false;
		m_bLocateCameraGrab = false;
		m_lWatchCameraSerial = 0;
		m_lLocateCameraSerial = 0;
		m_iCurrentProcessID = 0;
		m_iCurrentCameraHeight = 0;
		m_iCurrentCameraWidth = 0;
		m_iCurrentImageSize = 0;
		m_bCameraOpenFlag = false;
		m_bMessageArrived = false;
		m_bEnableOpenCamera = false;
		m_bEnableCloseCamera = false;
		memset(m_szImageBuffer,0,IMAGE_SIZE);
	}

	CCameraParam(const CCameraParam& param);
	CCameraParam& operator=(const CCameraParam& param);

public:
	bool m_bOpenWatchCamera;
	bool m_bOpenLocateCamera;
	bool m_bWatchCameraGrab;
	bool m_bLocateCameraGrab;
	long m_lWatchCameraSerial;
	long m_lLocateCameraSerial;
	int m_iCurrentProcessID;
	int m_iCurrentCameraHeight;
	int m_iCurrentCameraWidth;
	int m_iCurrentImageSize;
	bool m_bCameraOpenFlag;
	bool m_bMessageArrived;
	bool m_bEnableOpenCamera;
	bool m_bEnableCloseCamera;
	char m_szImageBuffer[IMAGE_SIZE];
};



#define BUFFER_SIZE sizeof(CCameraParam)
template <typename T = char>
class CCameraSharedBuffer :public CSingletonTemplate<CCameraSharedBuffer<T>>
{
	friend CSingletonTemplate<CCameraSharedBuffer>;
private:
	struct USMHEADER
	{

	};

	struct USMTHREAD
	{
		DWORD id;
		int evidx;
	};

	// Strings of handle ids
	wstring cwmn;
	wstring fmn;
	wstring evrn;
	wstring evwn;

	wstring stringid;

	bool WasFirst = false;
	HANDLE hEventWrote = 0;
	HANDLE hMutexWriting = 0;
	HANDLE hEventMeReading = 0;
	HANDLE hFM = 0;
	unsigned long long ClientSZ = 0;
	DWORD MaxThreads = 0;
	PVOID  Buff = 0;

	HANDLE CreateEvR(int idx)
	{
		TCHAR n[1000] = { 0 };
		swprintf_s(n, L"%s%i", evrn.c_str(), idx);
		HANDLE hX = CreateEvent(0, TRUE, TRUE, n);
		return hX;
	}

	HANDLE CreateEvW()
	{
		TCHAR n[1000] = { 0 };
		swprintf_s(n, L"%s", evwn.c_str());
		HANDLE hX = CreateEvent(0, 0, 0, n);
		return hX;
	}

	HANDLE CreateCWM()
	{
		HANDLE hX = OpenMutex(MUTEX_MODIFY_STATE | SYNCHRONIZE, false, cwmn.c_str());
		if (hX != 0)
			return hX;
		hX = CreateMutex(0, 0, cwmn.c_str());
		return hX;
	}

	HANDLE CreateFM()
	{
		// Try to open the map , or else create it
		WasFirst = true;
		HANDLE hX = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, false, fmn.c_str());
		if (hX != 0)
		{
			WasFirst = false;
			return hX;
		}

		unsigned long long  FinalSize = ClientSZ*sizeof(T) + MaxThreads*sizeof(USMTHREAD) + sizeof(USMHEADER);
		ULARGE_INTEGER ulx = { 0 };
		ulx.QuadPart = FinalSize;
		hX = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, ulx.HighPart, ulx.LowPart, fmn.c_str());
		if (hX != 0)
		{
			LPVOID Buff = MapViewOfFile(hFM, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
			if (Buff)
			{
				memset(Buff, 0, (size_t)FinalSize);
				UnmapViewOfFile(Buff);
			}
		}
		return hX;
	}

	CCameraSharedBuffer(const wchar_t* string_id = 0, bool Init = false, unsigned long long csz = BUFFER_SIZE, DWORD MaxTh = 100)
	{
		if (!string_id)
			return;
		CreateInit(string_id, Init, csz, MaxTh);
	}

	void operator =(const CCameraSharedBuffer &x)
	{
		// Terminate current
		End();

		// Recreate
		CreateInit(x.stringid.c_str(), true, x.ClientSZ, x.MaxThreads);
	}

	CCameraSharedBuffer(const CCameraSharedBuffer& x)
	{
		operator=(x);
	}


public:
	//-----------------------------------------------------------------------------
	// Name:	Instance()
	// Desc:	Returns only instance of contained object if available, otherwise 
	//  returns NULL.
	// Returns:	Can return NULL.  Do NOT clean this up, unless not using 
	//			CSingletonObserver
	//-----------------------------------------------------------------------------		
	static	CCameraSharedBuffer* Instance()
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
				ms_pOnlyInstance = new CCameraSharedBuffer(L"{4BF41A4E-ACF9-4E1D-A479-14DE9FF83BC2}", true);

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


	void End()
	{
		// Remove the ID from the thread
		if (Buff)
		{
			USMTHREAD* th = (USMTHREAD*)((char*)((char*)Buff + sizeof(USMHEADER)));
			WaitForSingleObject(hMutexWriting, INFINITE);
			// Find 
			for (unsigned int y = 0; y < MaxThreads; y++)
			{
				USMTHREAD& tt = th[y];
				DWORD myid = GetCurrentThreadId();
				if (tt.id == myid)
				{
					tt.id = 0;
					tt.evidx = 0;
					break;
				}
			}
			ReleaseMutex(hMutexWriting);
		}


		if (hEventWrote)
			CloseHandle(hEventWrote);
		hEventWrote = 0;
		if (hFM)
			CloseHandle(hFM);
		hFM = 0;
		if (hEventMeReading)
			CloseHandle(hEventMeReading);
		hEventMeReading = 0;
		if (hMutexWriting)
			CloseHandle(hMutexWriting);
		hMutexWriting = 0;
	}


	bool IsFirst() { return WasFirst; }


	void CreateInit(const wchar_t* string_id, bool Init = false, unsigned long long csz = BUFFER_SIZE, DWORD MaxTh = 100)
	{
		if (!string_id)
			return;
		if (wcslen(string_id) == 0)
			return;

		TCHAR xup[1000] = { 0 };
		stringid = string_id;

		swprintf_s(xup, 1000, L"%s_cwmn", stringid.c_str());
		cwmn = xup;
		swprintf_s(xup, 1000, L"%s_evrn", stringid.c_str());
		evrn = xup;
		swprintf_s(xup, 1000, L"%s_evwn", stringid.c_str());
		evwn = xup;
		swprintf_s(xup, 1000, L"%s_fmn", stringid.c_str());
		fmn = xup;

		if (!csz)
			csz = BUFFER_SIZE;
		ClientSZ = csz;
		if (!MaxTh)
			MaxTh = 100;
		MaxThreads = MaxTh;
		if (Init)
		{
			int iv = Initialize();
			if (iv <= 0)
			{
				End();
				throw iv;
			}
		}
	}

	~CCameraSharedBuffer(){
		End();
	}

	int Initialize()
	{
		hEventWrote = 0;
		hMutexWriting = 0;
		hFM = 0;
		Buff = 0;
		hEventMeReading = 0;

		if (hMutexWriting == 0)
			hMutexWriting = CreateCWM();
		if (hMutexWriting == 0)
			return -1;
		if (hFM == 0)
			hFM = CreateFM();
		if (hFM == 0)
			return -1;
		if (hEventWrote == 0)
			hEventWrote = CreateEvW();
		if (hEventWrote == 0)
			return -1;
		if (Buff == 0)
			Buff = MapViewOfFile(hFM, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		if (!Buff)
			return -1;

		// Acquire lock for Count variable
		// USMHEADER* h = (USMHEADER*)Buff;
		USMTHREAD* th = (USMTHREAD*)((char*)((char*)Buff + sizeof(USMHEADER)));
		WaitForSingleObject(hMutexWriting, INFINITE);
		// Find 
		for (unsigned int y = 0; y < MaxThreads; y++)
		{
			USMTHREAD& tt = th[y];
			if (tt.id == 0)
			{
				tt.id = GetCurrentThreadId();
				tt.evidx = (y + 1);
				hEventMeReading = CreateEvR(y + 1);
				break;
			}
		}
		ReleaseMutex(hMutexWriting);

		if (!hEventMeReading)
			return -1;

		return 1;

	}

	const T* BeginRead(bool FailOnNotReady = false)
	{
		if (!Buff)
			return 0;

		// Is someone writing 
		if (FailOnNotReady)
		{
			DWORD x = WaitForSingleObject(hMutexWriting, 0);
			if (x != WAIT_OBJECT_0)
				return 0;
		}
		else
			WaitForSingleObject(hMutexWriting, INFINITE);

		// Reset our reading event
		ResetEvent(hEventMeReading);

		// Release the mutex, but now any writing thread that locks it must wait for is
		ReleaseMutex(hMutexWriting);

		// Return the pointer
		const char* a1 = (const char*)Buff;
		a1 += sizeof(USMHEADER);
		a1 += sizeof(USMTHREAD)*MaxThreads;

		return (T*)a1;
	}

	void EndRead()
	{
		SetEvent(hEventMeReading);
	}

	unsigned long long ReadData(T* b, size_t sz, size_t offset = 0, bool FailIfNotReady = false)
	{
		const T* ptr = BeginRead(FailIfNotReady);
		if (!ptr)
			return (unsigned long long) - 1;
		memcpy(b, ptr + offset, sz);
		EndRead();
		return sz;
	}


	DWORD NotifyOnRead(bool Wait)
	{
		// See if any thread is reading
		USMTHREAD* th = (USMTHREAD*)((char*)((char*)Buff + sizeof(USMHEADER)));
		vector<HANDLE> evs;

		// Find 
		bool S = true;
		for (unsigned int y = 0; y < MaxThreads; y++)
		{
			USMTHREAD& tt = th[y];
			if (tt.evidx > 0)
			{
				// Open the event
				TCHAR n[1000] = { 0 };
				swprintf_s(n, L"%s%i", evrn.c_str(), tt.evidx);
				HANDLE hEv = OpenEvent(SYNCHRONIZE, 0, n);
				if (hEv == 0) // duh
				{
					S = false;
					break;
				}
				evs.push_back(hEv);
			}
		}
		DWORD fi = 0;
		if (!S)
			return (DWORD)-1;
		if (evs.empty())
			return (DWORD)-2;

		// Wait for any thread to terminate reading
		fi = WaitForMultipleObjects((DWORD)evs.size(), &evs[0], FALSE, Wait ? INFINITE : 0);

		// Cleanup
		for (unsigned int i = 0; i < evs.size(); i++)
			CloseHandle(evs[i]);
		evs.clear();

		return fi;
	}

	T* BeginWrite(bool FailOnNotReady = false)
	{
		// Lock the writing mutex
		if (FailOnNotReady)
		{
			DWORD x = WaitForSingleObject(hMutexWriting, 0);
			if (x != WAIT_OBJECT_0)
				return 0;
		}
		else
			WaitForSingleObject(hMutexWriting, INFINITE);

		// Having locked the writing mutex, no reading thread can start now
		// After that, no new threads can read
		vector<HANDLE> evs;
		evs.reserve(MaxThreads);

		// Wait for threads that are already in read state
		USMTHREAD* th = (USMTHREAD*)((char*)((char*)Buff + sizeof(USMHEADER)));

		// Find 
		bool S = true;
		for (unsigned int y = 0; y < MaxThreads; y++)
		{
			USMTHREAD& tt = th[y];
			if (tt.evidx > 0)
			{
				// Open the event
				TCHAR n[1000] = { 0 };
				swprintf_s(n, L"%s%i", evrn.c_str(), tt.evidx);
				HANDLE hEv = OpenEvent(SYNCHRONIZE, 0, n);
				if (hEv == 0) // duh
				{
					S = false;
					break;
				}
				evs.push_back(hEv);
			}
		}
		DWORD fi = 0;
		if (S)
		{
			// Wait for all these threads to terminate reading
			fi = WaitForMultipleObjects((DWORD)evs.size(), &evs[0], TRUE, FailOnNotReady ? 0 : INFINITE);
			if (fi == -1 || fi == WAIT_TIMEOUT)
				S = false;
		}
		else
		{
			fi = (DWORD)-1;
		}

		// Cleanup
		for (unsigned int i = 0; i < evs.size(); i++)
			CloseHandle(evs[i]);
		evs.clear();
		if (!S)
		{
			ReleaseMutex(hMutexWriting);
			return 0;
		}

		// Return the pointer
		char* a1 = (char*)Buff;
		a1 += sizeof(USMHEADER);
		a1 += sizeof(USMTHREAD)*MaxThreads;

		ResetEvent(hEventWrote);
		return (T*)a1;
	}

	void EndWrite()
	{
		ReleaseMutex(hMutexWriting);
		SetEvent(hEventWrote);
	}

	DWORD NotifyWrite(bool Wait)
	{
		// Wait for all these threads to terminate reading
		return WaitForSingleObject(hEventWrote, Wait ? INFINITE : 0);
	}

	unsigned long long WriteData(const T* b, size_t sz, size_t offset = 0, bool FailIfNotReady = false)
	{
		T* ptr = BeginWrite(FailIfNotReady);
		if (!ptr)
			return (unsigned long long) - 1;
		memcpy(ptr + offset, b, sz);
		EndWrite();
		return sz;
	}
};


#endif // _CAMERA_SHARED_BUFFER_H