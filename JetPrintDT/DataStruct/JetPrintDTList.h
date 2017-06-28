#ifndef _DXF_LIST_H
#define _DXF_LIST_H
#include <iostream>
#include <assert.h>
using namespace std;

#define UNUSABLE CProProcess::unusable
#define PARAM_USABLE(T) CProProcess::param_usable<T>::value


//pre process
class CProProcess{
public:
	class unusable{
	};

	template<typename T>
	class param_usable
	{
	public:
		enum{value = true};
	};

	template<>
	class param_usable<unusable>
	{
	public:
		enum{value = false};
	};
};


template <typename T,class INSTANCE = UNUSABLE>
class CJetPrintDTList
{
public:
	//callback func pointer to create data
	typedef void (INSTANCE::*CreateData)(T&);
	//callback func pointer to destroy data
	typedef void (INSTANCE::*DestroyData)(T data);
	//callback func pointer to copy data
	typedef void (INSTANCE::*CopyData)(T destData,T sourceData);


	static CreateData m_CreateData;
	static DestroyData m_DestroyData;
	static CopyData m_CopyData;
	static INSTANCE *m_pInstance;

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
			m_data = nullptr;
			if (m_CreateData){
				(m_pInstance->*m_CreateData)(m_data);
			}
			//memset(m_data, 0, sizeof(m_data));
			pNext = NULL;
			pPrev = NULL;
		}

		virtual ~Node(){
			if (m_DestroyData)
				(m_pInstance->*m_DestroyData)(m_data);
		}


		Node(const Node& node){
			pNext = node.pNext;
			pPrev = node.pPrev;

			if (m_CreateData)
				m_data = (m_pInstance->*m_CreateData)();

			if (m_CopyData)
				(m_pInstance->*m_CopyData)(m_data, node.m_data);
		}

		Node & operator=(const Node& node){
			if (this == &node)
				return *this;

			pNext = node.pNext;
			pPrev = node.pPrev;

			if (m_CreateData)
				m_data = (m_pInstance->*m_CreateData)();

			if (m_CopyData)
				(m_pInstance->*m_CopyData)(m_data, node.m_data);
		}
	};

private:
	void Destroy();
	
public:
	CJetPrintDTList();
	CJetPrintDTList(const CJetPrintDTList &m_List);
	~CJetPrintDTList();
	const CJetPrintDTList& operator=(const CJetPrintDTList &m_List);

	//brief get head data
	//param void
	//return head data
	const T GetHead();

	//brief get tail data
	//param void
	//return tail data
	const T GetTail();

	//brief add an element to the list from head
	//param data the data that is to be added
	//return void
	void AddHead(T data);

	//brief add an element to the list from tail
	//param data the data that is to be added
	//return void
	void AddTail(T data);

	//brief add a list to the list from tail
	//param pList the list that is to be added
	//return void
	void AddTail(const CJetPrintDTList *pList);

	//brief remove all the elements in a list
	//param void
	//return void
	void RemoveAll();

	//brief remove remove the head node from a list
	//param void
	//return void
	void RemoveHead();

	//brief remove remove the tail node from a list
	//param void
	//return void
	void RemoveTail();

	//brief get the data in a given index
	//param nIndex 
	//return the data corresponding to the index
	T GetAt(int nIndex) const;


	//brief remove the node from a list in a given index
	//param nIndex 
	//return void
	void RemoveAt(int nIndex);

	//brief modify the node data from a list in a given index
	//param nIndex
	//      num the final data
	//return void
	void SetAt(int nIndex, T num);

	//brief insert a node into a list after a given index
	//param nIndex
	//      num the final data
	//return void
	void InsertAfter(int nIndex, T num);

	//brief insert a node into a list before a given index
	//param nIndex
	//      num the final data
	//return void
	void InsertBefore(int nIndex, T num);

	//brief get the list size
	//param 
	//return the list size
	int GetSize() const;

	//brief check if a list is empty
	//param 
	//return true if the list is empty
	//	     false if the list is not empty
	bool IsEmpty() const;
	
	//brief output the list content
	//param void
	//return void
	void PrintList();

	//brief get the head node pointer of the list
	//param void
	//return head node pointer
	Node* GetHeadPtr();

	//brief get the tail node pointer of the list
	//param void
	//return tail node pointer
	Node* GetTailPtr();


	const T& operator[](int i) const
	{
		return GetAt(i);
	}
	T& operator[](int i)
	{
		return GetAt(i);
	}
	class iterator
	{
	public:
		Node* m_pCurrentNode;
		inline iterator() :m_pCurrentNode(NULL){}
		inline iterator(Node* pNode) : m_pCurrentNode(pNode){};
		inline iterator(const iterator& it) :m_pCurrentNode(it.m_pCurrentNode){}

		inline T& operator*() const { return m_pCurrentNode->m_data; }
		inline T& operator->() const { return m_pCurrentNode->m_data; }

		inline bool operator==(const iterator &it) const { return m_pCurrentNode == it.m_pCurrentNode; }
		inline bool operator!=(const iterator &it) const { return  m_pCurrentNode != it.m_pCurrentNode; }

		inline iterator &operator++()
		{
			m_pCurrentNode = m_pCurrentNode->pNext;
			return *this;
		}
		inline iterator operator++(int)
		{
			iterator temp = *this;
			m_pCurrentNode = m_pCurrentNode->pNext;
			return temp;
		}
		inline iterator &operator--()
		{
			m_pCurrentNode = m_pCurrentNode->pPrev;
			return *this;
		}
		inline iterator operator--(int)
		{
			iterator temp = *this;
			m_pCurrentNode = m_pCurrentNode->pPrev;
			return temp;
		}
	};

	iterator begin()
	{
		return iterator(pHead);
	}

	iterator end()
	{
		return iterator(pTail->pNext);
	}

	//callback funcs
	CreateData GetCreateDataFunc();
	DestroyData GetDestroyDataFunc();
	CopyData GetCopyDataFunc();
	INSTANCE *GetInstance();

	void RegisterCreateDataFunc(CreateData);
	void RegisterDestroyDataFunc(DestroyData);
	void RegisterCopyDataFunc(CopyData);
	void RegisterInstance(INSTANCE*);

protected:

	//Link_List pHead;
	Node *pHead;
	Node *pTail;
	int m_Lenth;
};

template<typename T, typename INSTANCE> 
typename CJetPrintDTList<T,INSTANCE>::CreateData CJetPrintDTList<T, INSTANCE>::m_CreateData = nullptr;
template<typename T, typename INSTANCE> 
typename  CJetPrintDTList<T, INSTANCE>::CopyData CJetPrintDTList<T, INSTANCE>::m_CopyData = nullptr;
template<typename T, typename INSTANCE> 
typename  CJetPrintDTList<T, INSTANCE>::DestroyData CJetPrintDTList<T, INSTANCE>::m_DestroyData = nullptr;
template<typename T, typename INSTANCE>  
INSTANCE* CJetPrintDTList<T, INSTANCE>::m_pInstance = nullptr;

template<typename T,typename INSTANCE>
CJetPrintDTList<T,INSTANCE>::CJetPrintDTList()
{
	pHead = NULL;
	pTail = NULL;
	m_Lenth = 0;
}

template<typename T, typename INSTANCE>
CJetPrintDTList<T,INSTANCE>::~CJetPrintDTList()
{
	RemoveAll();
}

template<typename T, typename INSTANCE>
CJetPrintDTList<T,INSTANCE>::CJetPrintDTList(const CJetPrintDTList<T,INSTANCE> &m_List)
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

	if (m_CopyData){
		(m_pInstance->*m_CopyData)(pNewHead->m_data, pTempNode->m_data);
		(m_pInstance->*m_CopyData)(pTail->m_data ,pTempNode->m_data);
	}
	else
	{
		pNewHead->m_data = pTempNode->m_data;
		pTail->m_data, pTempNode->m_data;
	}
		
	pTail->pNext = NULL;
	pTail->pPrev = pNewHead;
	pNewHead->pNext = pTail;
	pNewHead->pPrev = NULL;

	++m_Lenth;

	while (NULL != pTempNode->pNext)
	{

		if (m_Lenth == m_List.GetSize() - 1)
		{
			if (m_CopyData)
				(m_pInstance->*m_CopyData)(pTail->m_data, pTempNode->pNext.m_data);
			else
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
				if (m_CopyData)
					(m_pInstance->*m_CopyData)(pTemp->m_data, pTempNode->pNext->m_data);
				else
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

template<typename T, typename INSTANCE>
const CJetPrintDTList<T,INSTANCE>&CJetPrintDTList<T,INSTANCE>::operator=(const CJetPrintDTList<T,INSTANCE> &m_List)
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

	if (m_CopyData){
		(m_pInstance->*m_CopyData)(pNewHead->m_data, pTempNode->m_data);
		(m_pInstance->*m_CopyData)(pTail->m_data, pTempNode->m_data);
	}
		
	else{
		pNewHead->m_data = pTempNode->m_data;
		pTail->m_data = pTempNode->m_data;
	}
		
	pTail->pNext = NULL;
	pTail->pPrev = pNewHead;
	pNewHead->pNext = pTail;
	pNewHead->pPrev = NULL;

	++m_Lenth;

	while (NULL != pTempNode->pNext)
	{

		if (m_Lenth == m_List.GetSize() - 1)
		{
			if (m_CopyData)
				(m_pInstance->*m_CopyData)(pTail->m_data, pTempNode->pNext->m_data);
			else
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
				if (m_CopyData)
					(m_pInstance->*m_CopyData)(pTemp->m_data, pTempNode->pNext->m_data);
				else
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

template<typename T, typename INSTANCE>
const T CJetPrintDTList<T,INSTANCE>::GetHead()
{
	T result;
	if (m_CreateData)
		(m_pInstance->*m_CreateData)(result);
	if (!IsEmpty()){
		if (m_CopyData)
			(m_pInstance->*m_CopyData)(result, pHead->m_data);
		else
			result = pHead->m_data;
	}
		
	else
	{
		cerr << "EmptyList" << endl;
	}

	return result;
}

template<typename T, typename INSTANCE>
const T CJetPrintDTList<T,INSTANCE>::GetTail()
{
	T result;
	if (m_CreateData)
		result = (m_pInstance->*m_CreateData)();

	if (!IsEmpty()){
		if (m_CopyData)
			(m_pInstance->*m_CopyData)(result, pTail->m_data);
		else
			result = pTail->m_data;
	}
	else
	{
		cerr << "EmptyList" << endl;
	}

	return result;
}

template<typename T, typename INSTANCE>
bool CJetPrintDTList<T,INSTANCE>::IsEmpty() const
{
	return m_Lenth == 0;
}

template<typename T, typename INSTANCE>
int CJetPrintDTList<T,INSTANCE>::GetSize() const
{
	return m_Lenth;
}

template<typename T, typename INSTANCE>
void CJetPrintDTList<T,INSTANCE>::AddHead(T data)
{
	if (IsEmpty())
	{
		pHead = new Node;
		if (m_CopyData)
			(m_pInstance->*m_CopyData)(pHead->m_data, data);
		else
			pHead->m_data = data;
		pTail = new Node;
		if (m_CopyData)
			(m_pInstance->*m_CopyData)(pTail->m_data, data);
		else
			pTail->m_data = data;

		pHead->pNext = pTail;
		pHead->pPrev = NULL;
		pTail->pPrev = pHead;
		pTail->pNext = NULL;
	}
	else if (1 == GetSize())
	{
		if (m_CopyData)
			(m_pInstance->*m_CopyData)(pHead->m_data, data);
		else
			pHead->m_data = data;
		pHead->pNext = pTail;
	}
	else
	{
		Node *pNode = new Node;
		if (m_CopyData)
			(m_pInstance->*m_CopyData)(pNode->m_data, data);
		else
			pNode->m_data = data;
		pNode->pNext = pHead;
		pHead->pPrev = pNode;


		pHead = pNode;
	}


	++m_Lenth;
}

template<typename T, typename INSTANCE>
void CJetPrintDTList<T,INSTANCE>::AddTail(T data)
{
	if (IsEmpty())
	{
		pHead = new Node;
		if (m_CopyData)
			(m_pInstance->*m_CopyData)(pHead->m_data, data);
		else
			pHead->m_data = data;
		pTail = new Node;
		if (m_CopyData)
			(m_pInstance->*m_CopyData)(pTail->m_data, data);
		else
			pTail->m_data = data;

		pHead->pNext = pTail;
		pHead->pPrev = NULL;
		pTail->pNext = NULL;
		pTail->pPrev = pHead;
	}
	else if (1 == GetSize())
	{
		if (m_CopyData)
			(m_pInstance->*m_CopyData)(pTail->m_data, data);
		else
			pTail->m_data = data;
	}
	else
	{
		Node *pNode = new Node;
		if (m_CopyData)
			(m_pInstance->*m_CopyData)(pNode->m_data, data);
		else
			pNode->m_data = data;
		pNode->pNext = NULL;

		pTail->pNext = pNode;
		pNode->pPrev = pTail;

		pTail = pNode;
	}


	++m_Lenth;
}

template<typename T, typename INSTANCE>
void CJetPrintDTList<T,INSTANCE>::AddTail(const CJetPrintDTList *pList)
{
	if (pList->IsEmpty())
		return;

	for (int i = 0; i < pList->GetSize(); i++)
	{
		AddTail(pList->GetAt(i));
	}
}

template<typename T,typename INSTANCE>
void CJetPrintDTList<T,INSTANCE>::RemoveAll()
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

template<typename T, typename INSTANCE>
void CJetPrintDTList<T,INSTANCE>::RemoveHead()
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

template<typename T,typename INSTANCE>
void CJetPrintDTList<T,INSTANCE>::RemoveTail()
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

template<typename T,typename INSTANCE>
typename T CJetPrintDTList<T,INSTANCE>::GetAt(int nIndex) const
{
	T result = nullptr;
	if (m_CreateData)
		(m_pInstance->*m_CreateData)(result);

	memset(result, 0, sizeof(T));

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
				if (m_CopyData)
					(m_pInstance->*m_CopyData)(result, pTemp->m_data);
				else
					result = pTemp->m_data;
			}

			pTemp = pTemp->pNext;
			++nCount;
		}
	}

	return result;
}

template<typename T,typename INSTANCE>
void CJetPrintDTList<T,INSTANCE>::RemoveAt(int nIndex)
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

template<typename T,typename INSTANCE>
void CJetPrintDTList<T,INSTANCE>::SetAt(int nIndex, T num)
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
				if (m_CopyData)
					(m_pInstance->*m_CopyData)(pTemp->m_data, num);
				else
					pTemp->m_data = num;
			}
			pTemp = pTemp->pNext;
			++nCount;
		}
	}
}

template<typename T,typename INSTANCE>
void CJetPrintDTList<T,INSTANCE>::InsertAfter(int nIndex, T num)
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
				if (m_CopyData)
					(m_pInstance->*m_CopyData)(pNewNode->m_data, num);
				else
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

template<typename T,typename INSTANCE>
void CJetPrintDTList<T,INSTANCE>::InsertBefore(int nIndex, T num)
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
				if (m_CopyData)
					(m_pInstance->*m_CopyData)(pNewNode->m_data, num);
				else
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

template<typename T,typename INSTANCE>
void CJetPrintDTList<T,INSTANCE>::PrintList()
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

template<typename T,typename INSTANCE>
typename CJetPrintDTList<T,INSTANCE>::Node* CJetPrintDTList<T,INSTANCE>::GetHeadPtr()
{
	return pHead;
}

template<typename T,typename INSTANCE>
typename CJetPrintDTList<T,INSTANCE>::Node* CJetPrintDTList<T,INSTANCE>::GetTailPtr()
{
	return pTail;
}

template<typename T,typename INSTANCE>
typename CJetPrintDTList<T,INSTANCE>::CreateData CJetPrintDTList<T, INSTANCE>::GetCreateDataFunc(){
	return m_CreateData;
}

template<typename T,typename INSTANCE>
typename CJetPrintDTList<T,INSTANCE>::DestroyData CJetPrintDTList<T, INSTANCE>::GetDestroyDataFunc(){
	return m_DestroyData;
}

template<typename T,typename INSTANCE>
typename CJetPrintDTList<T,INSTANCE>::CopyData CJetPrintDTList<T, INSTANCE>::GetCopyDataFunc(){
	return m_CopyData;
}

template<typename T,typename INSTANCE>
typename INSTANCE* CJetPrintDTList<T, INSTANCE>::GetInstance(){
	return m_pInstance;
}

template<typename T,typename INSTANCE>
void CJetPrintDTList<T, INSTANCE>::RegisterCreateDataFunc(typename CJetPrintDTList<T,INSTANCE>::CreateData createFunc){
	m_CreateData = createFunc;
}

template<typename T,typename INSTANCE>
void CJetPrintDTList<T, INSTANCE>::RegisterDestroyDataFunc(typename CJetPrintDTList<T,INSTANCE>::DestroyData destroyFunc){
	m_DestroyData = destroyFunc;
}

template<typename T,typename INSTANCE>
void CJetPrintDTList<T, INSTANCE>::RegisterCopyDataFunc(typename CJetPrintDTList<T,INSTANCE>::CopyData copyFunc){
	m_CopyData = copyFunc;
}

template<typename T,typename INSTANCE>
void CJetPrintDTList<T, INSTANCE>::RegisterInstance(INSTANCE* pInst){
	m_pInstance = pInst;
}

#endif
