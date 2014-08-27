// Tabҳ�ؼ�
#pragma once
#include "ControlBase.h"

// Tabҳ��Ϣ�ṹ����
struct TabItemInfo
{
	BOOL			bVisible;	// �Ƿ�ɼ�
	Image*			pImage;		// ͼƬ
	CSize			sizeImage;	// ͼƬ��С
	int				nImageCount;// ͼƬ����(������1��3,�����1,��hoverͼƬʹ��tabctrl���õ�)
	int				nImageIndex;// ͼƬ����,���pImageΪ��,��ʹ�ô�������tabctrl�в���ͼƬ
	CRect			rc;			// ҳ�������λ��
	CString			strText;	// ����
	CString			strAction;	// ��������
	UINT			nItemID;	// ID
	CString			strName;	// ����
	BOOL			bOutLink;	// �Ƿ��ⲿ����ҳ��
	CControlBase*	pControl;	// Tabҳ��Ӧ����ҳ�������ؼ�
};


class CDuiTabCtrl : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiTabCtrl, _T("tabctrl"))
public:
	CDuiTabCtrl(HWND hWnd, CDuiObject* pDuiObject);
	CDuiTabCtrl(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle= TEXT(""), BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	virtual~CDuiTabCtrl(void);

	BOOL InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, UINT nResourceID, CControlBase* pControl, int nImageCount = -1, BOOL bOutLink = FALSE, int nItemWidth = 0, CString strType= TEXT("PNG"));
	BOOL InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, CString strImage, CControlBase* pControl, int nImageCount = -1, BOOL bOutLink = FALSE, int nItemWidth = 0);
	BOOL InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, int nImageIndex, CControlBase* pControl, BOOL bOutLink = FALSE, int nItemWidth = 0);

	int  GetItemIndex(CString strTabName);
	TabItemInfo* GetItemInfo(int nItem);
	void RefreshItems();
	void DeleteItem(int nItem);
	void DeleteItem(CString strTabName);
	int  SetSelectItem(int nItem);
	void SetItemVisible(int nItem, BOOL bVisible);
	void SetItemVisible(CString strTabName, BOOL bVisible);
	BOOL GetItemVisible(int nItem);
	BOOL GetItemVisible(CString strTabName);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadTabXml(CString strFileName);
	
protected:
	// ���ݿؼ��������ؼ�ʵ��
	CControlBase* _CreateControlByName(LPCTSTR lpszName);

	virtual BOOL OnCheckMouseResponse(UINT nFlags, CPoint point);
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	virtual	BOOL OnControlTimer();
	virtual BOOL DrawSubControls(CDC &dc, CRect rcUpdate);

	BOOL InsertItem(int nItem, TabItemInfo &itemInfo);
	
public:
	vector<TabItemInfo>		m_vecItemInfo;			// Tabҳ��Ϣ�б�
	vector<CRect>			m_vecRcSeperator;		// tabλ���б�

	Color					m_clrText;				// ������ɫ

	int						m_nTabItemWidth;		// ÿ��Tabҳ�Ŀ��
	int						m_nTabCtrlHeight;		// TabCtrl���ֵĸ߶�

	BOOL					m_bInit;				// �Ƿ��ʼ�����

	int						m_nHoverItem;			// ��ǰ��ʾ��ҳ������
	int						m_nDownItem;			// ��ǰ�����ҳ������
	BOOL					m_bAnimateChangeTab;	// �Ƿ�֧���л�tabҳ�Ķ���
	int						m_nOldItem;				// �л�ǰ��ҳ������
	int						m_nAnimateCount;		// �л�������֡��
	int						m_nCurXPos;				// �л������е�ǰ�ĺ�����λ��

	DUI_IMAGE_ATTRIBUTE_DEFINE(Seperator);			// ����ָ�ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(Hover);				// �����ȵ�ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("img-sep"), OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE(_T("img-hover"), OnAttributeImageHover)
		DUI_INT_ATTRIBUTE(_T("item-width"), m_nTabItemWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("tab-height"), m_nTabCtrlHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("animate"), m_bAnimateChangeTab, FALSE)
		DUI_INT_ATTRIBUTE(_T("animate-count"), m_nAnimateCount, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};