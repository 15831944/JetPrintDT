#pragma once



// CMainDXFView ��ͼ

class CMainDXFView : public CScrollView
{
	DECLARE_DYNCREATE(CMainDXFView)

protected:
	CMainDXFView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CMainDXFView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
	virtual void OnInitialUpdate();     // �����ĵ�һ��

	DECLARE_MESSAGE_MAP()
};


