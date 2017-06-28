#ifndef _OPEN_LIST_H
#define _OPEN_LIST_H
#include <iostream>
#include <assert.h>
using namespace std;
template <typename T>
class COpenList
{
protected:
	class Node
	{
	public:
		//ºó¼Ì
		Node *pNext;
		//Ç°Çý
		Node *pPrev;
		T m_data;

		Node()
		{
			memset(&m_data, 0, sizeof(m_data));
			pNext = NULL;
			pPrev = NULL;
		}
	};

private:
	void Destroy();
	
public:
	COpenList();
	COpenList(const COpenList &m_List);
	~COpenList();
	const COpenList& operator=(const COpenList &m_List);

	const T GetHead();
	const T GetTail();

	void AddHead(T data);
	void AddTail(T data);
	void AddTail(const COpenList *pList);
	void RemoveAll();
	void RemoveHead();
	void RemoveTail();

	T GetAt(int nIndex) const;
	void RemoveAt(int nIndex);
	void SetAt(int nIndex, T num);

	void InsertAfter(int nIndex, T num);
	void InsertBefore(int nIndex, T num);

	int GetSize() const;
	bool IsEmpty() const;
    
	void PrintList();
	Node* GetHeadPtr();
	Node* GetTailPtr();
protected:

	//Link_List pHead;
	Node *pHead;
	Node *pTail;
	int m_Lenth;
};


template<typename T>
COpenList<T>::COpenList()
{
	pHead = NULL;
	pTail = NULL;
	m_Lenth = 0;
}

template<typename T>
COpenList<T>::~COpenList()
{
	RemoveAll();
}

template<typename T>
COpenList<T>::COpenList(const COpenList<T> &m_List)
{
	if (!IsEmpty())
	{
		RemoveAll();
	}

	Node *pTempNode = m_List.pHead;
	if (NULL == pTempNode)
	{
		cout << "List doesn't exists" << endl;
		return;
	}

	this->pHead = new Node;
	this->pTail = new Node;
	this->m_Lenth = 0;
	assert(pHead);

	Node *pNewHead = pHead;

	pNewHead->m_data = pTempNode->m_data;
	pTail->m_data = pTempNode->m_data;
	pTail->pNext = NULL;
	pTail->pPrev = pNewHead;
	pNewHead->pNext = pTail;
	pNewHead->pPrev = NULL;

	++m_Lenth;

	while (NULL != pTempNode->pNext)
	{

		if (m_Lenth == m_List.GetSize() - 1)
		{
			pTail->m_data = pTempNode->pNext->m_data;
			pTail->pNext = NULL;
			pTail->pPrev = pNewHead;
			pNewHead->pNext = pTail;
		}
		else
		{
			Node *pTemp = new Node;
			if (NULL != pTemp)
			{
				pTemp->m_data = pTempNode->pNext->m_data;
				pTemp->pNext = NULL;
			}

			pNewHead->pNext = pTemp;
			pTemp->pPrev = pNewHead;
			pNewHead = pNewHead->pNext;
		}

		pTempNode = pTempNode->pNext;
		++m_Lenth;
	}

}

template<typename T>
const COpenList<T>&COpenList<T>::operator=(const COpenList<T> &m_List)
{
	if (this == &m_List)
		return *this;

	if (!IsEmpty())
	{
		RemoveAll();
	}

	Node *pTempNode = m_List.pHead;
	if (NULL == pTempNode)
	{
		cout << "List doesn't exists" << endl;
		return *this;
	}

	this->pHead = new Node;
	this->pTail = new Node;
	this->m_Lenth = 0;
	assert(pHead);

	Node *pNewHead = pHead;

	pNewHead->m_data = pTempNode->m_data;
	pTail->m_data = pTempNode->m_data;
	pTail->pNext = NULL;
	pTail->pPrev = pNewHead;
	pNewHead->pNext = pTail;
	pNewHead->pPrev = NULL;

	++m_Lenth;

	while (NULL != pTempNode->pNext)
	{

		if (m_Lenth == m_List.GetSize() - 1)
		{
			pTail->m_data = pTempNode->pNext->m_data;
			pTail->pNext = NULL;
			pTail->pPrev = pNewHead;
			pNewHead->pNext = pTail;
		}
		else
		{
			Node *pTemp = new Node;
			if (NULL != pTemp)
			{
				pTemp->m_data = pTempNode->pNext->m_data;
				pTemp->pNext = NULL;
			}

			pNewHead->pNext = pTemp;
			pTemp->pPrev = pNewHead;
			pNewHead = pNewHead->pNext;
		}

		pTempNode = pTempNode->pNext;
		++m_Lenth;
	}

	return *this;
}

template<typename T>
const T COpenList<T>::GetHead()
{
	T result;
	if (!IsEmpty())
		result = pHead->m_data;
	else
	{
		cerr << "EmptyList" << endl;
	}

	return result;
}

template<typename T>
const T COpenList<T>::GetTail()
{
	T result;
	if (!IsEmpty())
		result = pTail->m_data;
	else
	{
		cerr << "EmptyList" << endl;
	}

	return result;
}

template<typename T>
bool COpenList<T>::IsEmpty() const
{
	return m_Lenth == 0;
}

template<typename T>
int COpenList<T>::GetSize() const
{
	return m_Lenth;
}

template<typename T>
void COpenList<T>::AddHead(T data)
{
	if (IsEmpty())
	{
		pHead = new Node;
		pHead->m_data = data;
		pTail = new Node;
		pTail->m_data = data;

		pHead->pNext = pTail;
		pHead->pPrev = NULL;
		pTail->pPrev = pHead;
		pTail->pNext = NULL;
	}
	else if (1 == GetSize())
	{
		pHead->m_data = data;
		pHead->pNext = pTail;
	}
	else
	{
		Node *pNode = new Node;
		pNode->m_data = data;
		pNode->pNext = pHead;
		pHead->pPrev = pNode;


		pHead = pNode;
	}


	++m_Lenth;
}

template<typename T>
void COpenList<T>::AddTail(T data)
{
	if (IsEmpty())
	{
		pHead = new Node;
		pHead->m_data = data;
		pTail = new Node;
		pTail->m_data = data;

		pHead->pNext = pTail;
		pHead->pPrev = NULL;
		pTail->pNext = NULL;
		pTail->pPrev = pHead;
	}
	else if (1 == GetSize())
	{
		pTail->m_data = data;
	}
	else
	{
		Node *pNode = new Node;
		pNode->m_data = data;
		pNode->pNext = NULL;

		pTail->pNext = pNode;
		pNode->pPrev = pTail;

		pTail = pNode;
	}


	++m_Lenth;
}

template<typename T>
void COpenList<T>::AddTail(const COpenList *pList)
{
	if (pList->IsEmpty())
		return;

	for (int i = 0; i < pList->GetSize(); i++)
	{
		AddTail(pList->GetAt(i));
	}
}

template<typename T>
void COpenList<T>::RemoveAll()
{
	if (IsEmpty())
		return;


	Node *pTemp = pHead;
	Node *pTempIter = pTemp;
	while (NULL != pTemp)
	{
		pTempIter = pTemp->pNext;
		delete pTemp;
		pTemp = pTempIter;
	}

	m_Lenth = 0;
}

template<typename T>
void COpenList<T>::RemoveHead()
{
	if (IsEmpty())
		return;
	else if (1 == GetSize())
		RemoveAll();
	else
	{
		Node *pTemp = pHead;
		pHead = pHead->pNext;

		delete pTemp;

		pHead->pPrev = NULL;
	}

	--m_Lenth;
}

template<typename T>
void COpenList<T>::RemoveTail()
{
	if (IsEmpty())
		return;
	else if (1 == GetSize())
	{
		RemoveAll();
	}
	else
	{
		Node *pTemp = pTail->pPrev;
		delete pTail;

		pTail = pTemp;
		pTail->pNext = NULL;
	}

	--m_Lenth;
}

template<typename T>
T COpenList<T>::GetAt(int nIndex) const
{
	T result;
	if (nIndex >= GetSize())
		cerr << "Wrong Index" << endl;
	else
	{
		int nCount = 0;
		Node *pTemp = pHead;
		while (pTemp)
		{
			if (nCount == nIndex)
			{
				result = pTemp->m_data;
			}

			pTemp = pTemp->pNext;
			++nCount;
		}
	}

	return result;
}

template<typename T>
void COpenList<T>::RemoveAt(int nIndex)
{
	if (nIndex >= GetSize())
	{
		cerr << "Wrong Index" << endl;
		return;
	}
	else
	{
		if (nIndex == 0)
		{
			RemoveHead();
		}
		else if (nIndex == GetSize() - 1)
		{
			RemoveTail();
		}
		else
		{
			int nCount = 0;
			Node *pTemp = pHead;
			while (pTemp)
			{
				if (nCount == nIndex)
				{
					Node *pTempPrev = pTemp->pPrev;
					Node *pTempNext = pTemp->pNext;

					if (NULL != pTempPrev)
						pTempPrev->pNext = pTempNext;
					if (NULL != pTempNext)
						pTempNext->pPrev = pTempPrev;

					delete pTemp;

					break;
				}

				pTemp = pTemp->pNext;
				++nCount;
			}
		}

		--m_Lenth;

	}
}

template<typename T>
void COpenList<T>::SetAt(int nIndex, T num)
{
	if (nIndex >= GetSize())
		return;
	else
	{
		int nCount = 0;
		Node *pTemp = pHead;
		while (pTemp)
		{
			if (nCount == nIndex)
			{
				pTemp->m_data = num;
			}
			pTemp = pTemp->pNext;
			++nCount;
		}
	}
}

template<typename T>
void COpenList<T>::InsertAfter(int nIndex, T num)
{
	if (nIndex >= GetSize())
		return;
	else
	{
		int nCount = 0;
		Node *pTemp = pHead;
		while (pTemp)
		{
			if (nCount == nIndex)
			{
				Node *pNewNode = new Node;
				pNewNode->m_data = num;
				pNewNode->pNext = pTemp->pNext;
				pTemp->pNext = pNewNode;
				pNewNode->pPrev = pTemp;


				++m_Lenth;
				break;
			}

			++nCount;
			pTemp = pTemp->pNext;
		}
	}
}

template<typename T>
void COpenList<T>::InsertBefore(int nIndex, T num)
{
	if (nIndex >= GetSize())
		return;
	else
	{
		int nCount = 0;
		Node *pTemp = pHead;
		while (pTemp)
		{
			if (nCount == nIndex)
			{
				Node *pNewNode = new Node;
				pNewNode->m_data = num;
				pNewNode->pPrev = pTemp->pPrev;
				pTemp->pPrev = pNewNode;
				pNewNode->pNext = pTemp;

				++m_Lenth;
				break;
			}

			++nCount;
			pTemp = pTemp->pNext;
		}
	}
}

template<typename T>
void COpenList<T>::PrintList()
{
	Node *pTemp = this->pHead;

	if (NULL == pHead)
	{
		cout << "List doesn't exists" << endl;
		return;
	}

	while (NULL != pTemp)
	{
		cout << pTemp->m_data << " ";
		pTemp = pTemp->pNext;
	}

	cout << endl;
}

template<typename T>
typename COpenList<T>::Node* COpenList<T>::GetHeadPtr()
{
	return pHead;
}

template<typename T>
typename COpenList<T>::Node* COpenList<T>::GetTailPtr()
{
	return pTail;
}

#endif
