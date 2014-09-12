#pragma once
#include "DuiEdit.h"

// �����б�����Ϣ
struct ComboListItem
{
	UINT		nResourceID;		// ͼƬ��ԴID
	CString		strImageFile;		// ͼƬ�ļ�
	CString		strName;			// �б�����(��ʾ���б����һ��)
	CString		strDesc;			// ����(��ʾ���б���ڶ���)
	CString		strValue;			// �б���ֵ
};

class CDuiComboBox : public CDuiEdit
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiComboBox, _T("combobox"))
public:
	CDuiComboBox(HWND hWnd, CDuiObject* pDuiObject);
	virtual ~CDuiComboBox(void);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);

	void SetComboValue(CString strComboValue);
	CString GetComboValue();
	int GetItemCount();
	int AddItem(CString strName, CString strDesc, CString strValue, int nResourceID, CString strImageFile = _T(""));
	void ClearItems();

	HRESULT OnAttributeHeadImage(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeDeleteImage(const CString& strValue, BOOL bLoading);

	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
public:
	UINT					m_nResourceIDHeadBitmap;
	UINT					m_nResourceIDDeleteBitmap;
	CString					m_strImageHeadBitmap;
	CString					m_strImageDeleteBitmap;
	CString					m_strXmlFile;	// ���������б����XML�ļ�(���Բ���Ҫ)
	CPopupList*				m_pPopupList;	// ���������б�����ָ��
	vector<ComboListItem>	m_vecItem;		// �����б����б�
	CString					m_strComboValue;// �����б�ѡ�����Ŀ��ֵ

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("head-image"), OnAttributeHeadImage)
		DUI_CUSTOM_ATTRIBUTE(_T("del-image"), OnAttributeDeleteImage)
		DUI_TSTRING_ATTRIBUTE(_T("value"), m_strComboValue, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("xml"), m_strXmlFile, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};