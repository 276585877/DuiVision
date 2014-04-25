#pragma once
#include "DlgPopup.h"

#include <vector>
using namespace std;

// �˵���Ԥ������Ϣ
struct MenuItemValue
{
	//UINT		uID;			// �˵���ID
	CString		strName;		// �˵�������
	CString		strTitle;		// �˵������
	BOOL		bVisible;		// �Ƿ�ɼ�
	BOOL		bDisable;		// �Ƿ����
	int			nCheck;			// �Ƿ�ѡ��
};

class CMenuItem;

class CMenuEx : public CDlgPopup
{
public:
	static LPCSTR GetClassName() { return "menu";}
	virtual BOOL IsClass(LPCSTR lpszName)
	{
		if(strcmp(GetClassName(), lpszName)  == 0) return TRUE;
		return __super::IsClass(lpszName);
	}

public:
	CMenuEx(CString strFont = DuiSystem::GetDefaultFont(), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular);
	virtual ~CMenuEx(void);

	// �����˵�����
	BOOL Create(CWnd *pParent, CPoint point, UINT uMessageID, UINT nResourceID, int nFrameSize = 4, int nMinWidth = 113, enumBackMode enBackMode = enBMFrame);
	BOOL Create(CWnd *pParent, CPoint point, UINT uMessageID, CString strImage, int nFrameSize = 4, int nMinWidth = 113, enumBackMode enBackMode = enBMFrame);
	BOOL Create(CWnd *pParent, CPoint point, UINT uMessageID);

	// ��Ӳ˵���ͷָ���
	int AddMenu(CString strText, UINT uMenuID, int nResourceID = -1, BOOL bSelect = false, int nIndex = -1);
	int AddMenu(CString strText, UINT uMenuID, CString strImage, BOOL bSelect = false, int nIndex = -1);
	int AddSeparator(int nIndex = -1);

	// Ԥ��ֵ�˵��������
	void SetItemTitle(CString strName, CString strTitle);
	void SetItemVisible(CString strName, BOOL bVisible);
	void SetItemDisable(CString strName, BOOL bDisable);
	void SetItemCheck(CString strName, int nCheck);

	// ���ò˵���λ��
	void SetMenuPoint();

	// ��ȡ���˵�����
	CMenuEx* GetParentMenu();
	// ��ȡ��ǰ����˵������
	CMenuItem* GetHoverMenuItem();

	// ���ز˵�
	BOOL LoadSubMenu(TiXmlElement* pXmlElem, CString strSubItemName);
	BOOL LoadXmlNode(TiXmlElement* pXmlElem, CString strXmlFile = _T(""));
	BOOL LoadXmlFile(CString strFileName, CString strSubItemName = _T(""));
	BOOL LoadXmlFile(CString strFileName, CWnd *pParent, CPoint point, UINT uMessageID, CString strSubItemName = _T(""));

	virtual void DrawWindowEx(CDC &dc, CRect rcClient);
	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);
	virtual void InitUI(CRect rcClient);

	// ��Ϣ��Ӧ
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);

	virtual BOOL OnNcCloseWindow();

public:
	CString		m_strTitle;				// ����
	CString		m_strFont;				// �˵�����
	int			m_nFontWidth;			// ������
	FontStyle	m_fontStyle;			// ������
	UINT		m_uAlignment;			// ˮƽ���뷽ʽ
	UINT		m_uVAlignment;			// ��ֱ���뷽ʽ

	int			m_nLeft;				// ��߼��
	int			m_nHeight;				// �˵���Ĭ�ϸ߶�
	int			m_nWidth;				// �˵��������
	int			m_nSeparatorHeight;		// �ָ��߸߶�
	CPoint		m_point;				// �˵�λ��
	CWnd*		m_pParent;				// ������

	vector<MenuItemValue> m_vecMenuItemValue;	// �˵���Ԥ������Ϣ

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_TSTRING_ATTRIBUTE("title", m_strTitle, FALSE)
		DUI_TSTRING_ATTRIBUTE("font", m_strFont, FALSE)
		DUI_INT_ATTRIBUTE("fontwidth", m_nFontWidth, FALSE)
		DUI_INT_ATTRIBUTE("item-height", m_nHeight, FALSE)
		DUI_INT_ATTRIBUTE("left", m_nLeft, FALSE)
		DUI_INT_ATTRIBUTE("sep-height", m_nSeparatorHeight, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};
