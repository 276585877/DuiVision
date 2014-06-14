// Panel�ؼ����˿ؼ���һ���ؼ�����
#pragma once

#include "ControlBase.h"

using namespace  std;

class CDuiPanel : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiPanel, "div")

public:
	CDuiPanel(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiPanel(void);

	void	SetXmlFile(CString strXmlFile) {m_strXmlFile = strXmlFile;}

	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadXmlFile(CString strFileName);
	HRESULT OnAttributeXml(const CStringA& strValue, BOOL bLoading);

	void SetVirtualHeight(int nHeight) { m_nVirtualHeight = nHeight; }
	void CalcVirtualHeight();

	void SetEnableScroll(BOOL bEnableScroll) { m_bEnableScroll = bEnableScroll; }
	BOOL GetEnableScroll() { return m_bEnableScroll; }

	virtual void SetControlVisible(BOOL bIsVisible);
	virtual	void SetControlDisable(BOOL bIsDisable);

protected:
	// ��Ϣ��Ӧ
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL);
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// ���ݿؼ��������ؼ�ʵ��
	CControlBase* _CreateControlByName(LPCSTR lpszName);

	HRESULT OnAttributeImageScroll(const CStringA& strValue, BOOL bLoading);

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	virtual BOOL DrawSubControls(CDC &dc, CRect rcUpdate);

	virtual BOOL OnMousePointChange(CPoint& point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);

	virtual void InitUI(CRect rcClient, TiXmlElement* pNode);

public:
	BOOL				m_bInit;					// �Ƿ��ʼ�����
	CString				m_strXmlFile;				// XML�ļ���
	int					m_nVirtualHeight;			// Panel����ĸ߶�
	CControlBaseFont*	m_pControScrollV;			// ��ֱ������
	int					m_nVirtualTop;				// ��ǰ������ʾ�Ķ���λ��
	BOOL				m_bEnableScroll;			// �Ƿ��������

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("img-scroll", OnAttributeImageScroll)
		DUI_CUSTOM_ATTRIBUTE("xml", OnAttributeXml)
		DUI_INT_ATTRIBUTE("scroll", m_bEnableScroll, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
