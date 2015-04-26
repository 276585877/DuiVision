#pragma once

#include "ControlBase.h"

class CScrollV : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CScrollV, _T("scroll"))
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
	BOOL SetScrollSize(int nPageRange, int nRowRange = 0);
	BOOL SetScrollInfo(int nMaxRange, int nCurrentPos, int nPageRange = 0, int nRowRange = 0);
	BOOL ScrollRow(int nRow);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual void SetControlRect(CRect rc);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	HRESULT OnAttributeUpImage(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeDownImage(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeMaxRange(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeCurrentPos(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributePageRange(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeRowRange(const CString& strValue, BOOL bLoading);

protected:	
	void DrawRange(CDC &dc, CRect rcUpdate, int nXPos);
	int MoveRange(int nMove);
	int SetRange();

	// ��Ϣ��Ӧ
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);

public:	
	CBitmap			m_bitmap;
	enumButtonState m_enButtonState;		// ���״̬
	BOOL			m_bHover;				// �Ƿ�����ȵ�״̬
	BOOL			m_bShowScroll;			// �Ƿ���ʾ������

	CControlBase*	m_pControlUpImage;		// �ϼ�ͷ�ؼ�
	CControlBase*	m_pControlDownImage;	// �¼�ͷ�ؼ�

	int				m_nArrowLen;			// ��ͷ�ĳ���

	int				m_nMaxRange;			// ���ֵ
	int				m_nCurrentPos;			// ��ǰλ��
	int				m_nPageRange;			// һҳ������
	int				m_nRowRange;			// һ�з�����
	CRect			m_rcBlock;				// �������С
	BOOL			m_bAutoCalcRange;		// �Ƿ��Զ�����ķ�ҳ��Χ
	int				m_nDownTop;				// �����ʱ�����������λ��

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("maxrange"), OnAttributeMaxRange)
		DUI_CUSTOM_ATTRIBUTE(_T("curpos"), OnAttributeCurrentPos)
		DUI_CUSTOM_ATTRIBUTE(_T("pagerange"), OnAttributePageRange)
		DUI_CUSTOM_ATTRIBUTE(_T("rowrange"), OnAttributeRowRange)
		DUI_CUSTOM_ATTRIBUTE(_T("upimg"), OnAttributeUpImage)
		DUI_CUSTOM_ATTRIBUTE(_T("downimg"), OnAttributeDownImage)
	DUI_DECLARE_ATTRIBUTES_END()
};
