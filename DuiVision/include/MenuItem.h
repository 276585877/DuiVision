#pragma once
#include "ControlBase.h"

class CMenuEx;

class CMenuItem : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CMenuItem, "menuitem")
public:
	CMenuItem(HWND hWnd, CDuiObject* pDuiObject);
	CMenuItem(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle= TEXT(""), int nLeft = 30, BOOL bSelect = false, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	virtual ~CMenuItem(void);

	// ����״̬
	BOOL GetCheck();
	BOOL SetCheck(BOOL bCheck);

	// �Ƿ�ָ���
	BOOL IsSeparator() { return m_bIsSeparator; }
	// �Ƿ񵯳��˵�
	BOOL IsPopup() { return m_bIsPopup; }
	// �Ƿ��ڻ״̬(����ڴ˲˵���)
	BOOL IsHover() { return (m_enButtonState == enBSHover); }

	// ��ȡ���˵�����
	CMenuEx* GetParentMenu();
	// ���ò˵����Ƕ��XML�ļ���
	void SetMenuXml(CString strMenuXml) { m_strMenuXml = strMenuXml; }
	// ��ȡ�˵����Ƕ��XML�ļ���
	CString GetMenuXml() { return m_strMenuXml; }
	// ��ʾ�����˵�
	void ShowPopupMenu();
	// ��ȡ�����˵�
	CMenuEx* GetPopupMenu() { return m_pPopupMenu; }
	// ���õ����˵�
	void SetPopupMenu(CMenuEx* pPopupMenu) { m_pPopupMenu = pPopupMenu; }
	// ���ò˵���ͱ߿�֮��ľ���
	void SetFrameWidth(int nFrameWidth) { m_nFrameWidth = nFrameWidth; }

	HRESULT OnAttributeCheck(const CStringA& strValue, BOOL bLoading);

	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);

protected:
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual	void SetControlDisable(BOOL bIsDisable);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	enumButtonState		m_enButtonState;
	BOOL				m_bDown;			// ����Ǽ��򣬱�ʾ�Ƿ�ѡ��
	BOOL				m_bMouseDown;
	BOOL				m_bSelect;			// �Ƿ�ѡ��,���������ѡ���
	BOOL				m_bIsSeparator;		// �Ƿ�ָ���
	BOOL				m_bIsPopup;			// �Ƿ񵯳��˵�
	CMenuEx*			m_pPopupMenu;		// �����˵��Ķ���ָ��
	CString				m_strMenuXml;		// Ƕ�ײ˵���XML�ļ�
	int					m_nLeft;			// �˵������ֵ���߾�
	int					m_nFrameWidth;		// �˵������߿�Ŀ��

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE("select", m_bSelect, FALSE)
		DUI_INT_ATTRIBUTE("separator", m_bIsSeparator, FALSE)
		DUI_TSTRING_ATTRIBUTE("menu", m_strMenuXml, FALSE)
		DUI_CUSTOM_ATTRIBUTE("check", OnAttributeCheck)
	DUI_DECLARE_ATTRIBUTES_END()
};