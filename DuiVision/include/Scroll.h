#pragma once

#include "ControlBase.h"

#define MSG_SCROLL_CHANGE	1	// ������λ�ñ���¼�

class CScrollV : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CScrollV, "scroll")
public:
	CScrollV(HWND hWnd, CDuiObject* pDuiObject);
	CScrollV(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible = TRUE);
	virtual ~CScrollV(void);

	virtual BOOL SetBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	virtual BOOL SetBitmap(CString strImage);

	BOOL SetScrollCurrentPos(int nCurrentPos);
	int GetScrollCurrentPos() { return m_nCurrentPos; }
	BOOL SetScrollMaxRange(int nMaxRange);
	int GetScrollMaxRange() { return m_nMaxRange; }
	BOOL SetScrollSize(int nPageRange, int nRowRange);
	BOOL SetScrollInfo(int nMaxRange, int nCurrentPos, int nPageRange, int nRowRange);
	BOOL ScrollRow(int nRow);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual void SetControlRect(CRect rc);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	HRESULT OnAttributeUpImage(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeDownImage(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeMaxRange(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeCurrentPos(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributePageRange(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeRowRange(const CStringA& strValue, BOOL bLoading);

protected:	
	void DrawRange(CDC &dc, CRect rcUpdate, int nXPos);
	int MoveRange(int nMove);
	int SetRange();

	// ��Ϣ��Ӧ
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);

public:	
	CBitmap			m_bitmap;	
	enumButtonState m_enButtonState;
	BOOL			m_bHover;
	BOOL			m_bShowScroll;

	CControlBase*	m_pControlUpImage;		// �ϼ�ͷ�ؼ�
	CControlBase*	m_pControlDownImage;	// �¼�ͷ�ؼ�

	int				m_nArrowLen;			// ��ͷ�ĳ���

	int				m_nMaxRange;			// ���ֵ
	int				m_nCurrentPos;			// ��ǰλ��
	int				m_nPageRange;			// һҳ������
	int				m_nRowRange;			// һ�з�����
	CRect			m_rcBlock;				// �������С
	int				m_nDownTop;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("maxrange", OnAttributeMaxRange)
		DUI_CUSTOM_ATTRIBUTE("curpos", OnAttributeCurrentPos)
		DUI_CUSTOM_ATTRIBUTE("pagerange", OnAttributePageRange)
		DUI_CUSTOM_ATTRIBUTE("rowrange", OnAttributeRowRange)
		DUI_CUSTOM_ATTRIBUTE("upimg", OnAttributeUpImage)
		DUI_CUSTOM_ATTRIBUTE("downimg", OnAttributeDownImage)
	DUI_DECLARE_ATTRIBUTES_END()
};
