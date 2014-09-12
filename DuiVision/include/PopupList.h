#pragma once
#include "DlgPopup.h"

#include <vector>
using namespace std;

// ��Ϣ
#define				SELECT_ITEM					11
#define				DELETE_ITEM					12

// �б����
struct EditListItem
{
	UINT		nResourceID;	// ID
	CString		strImageFile;	// ͼƬ�ļ�
	Image*		pImage;			// ͼƬָ��
	CSize		sizeImage;		// ͼƬ��С
	CString		strName;		// ��ʾ��
	CString		strDesc;		// ����
	CString		strValue;		// ֵ
	CRect		rcItem;			// λ��
};

class CPopupList :  public CDlgPopup
{
	static LPCTSTR GetClassName() { return _T("popuplist");}

public:
	CPopupList(void);
	virtual ~CPopupList(void);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);

	void SetWidth(int nWidth) { m_nWidth = nWidth; }

	bool GetItemDesc(UINT nItem, CString &strDesc);
	bool GetItemName(UINT nItem, CString &strName);
	bool GetItemValue(UINT nItem, CString &strValue);
	bool GetItemImage(UINT nItem, UINT &nResourceID, CString &strImageFile);
	int AddItem(CString strName, CString strDesc, CString strValue, int nResourceID, CString strImageFile = _T(""));
	bool DeleteItem(UINT nItem);

	void SetCurItem(UINT nItem);
	void SetCurItem(CString strValue);

	virtual void DrawWindow(CDC &dc, CRect rcClient);
	virtual void DrawWindowEx(CDC &dc, CRect rcClient);

	virtual void InitUI(CRect rcClient);

	void SetItemPoint();

	virtual BOOL OnMouseMove(CPoint point);
	virtual BOOL OnLButtonDown(CPoint point);
	virtual BOOL OnLButtonUp(CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
	CRect					m_rcClose;			// �б���Ĺر�����
	enumButtonState			m_buttonState;
	vector<EditListItem>	m_vecItem;
	int						m_nHoverItem;
	CFont					m_font;
	int						m_nWidth;			// �б���

	DUI_IMAGE_ATTRIBUTE_DEFINE(Head);		// �����б������ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(Close);		// �����б���ɾ��ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("width"), m_nWidth, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
