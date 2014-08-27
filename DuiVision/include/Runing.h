// ����ͼƬ����ͼƬ�࣬����ͨ�����ͼƬ���ɶ�������������ָ����ͼƬ
#pragma once
#include "ControlBase.h"


class CRuning : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CRuning, _T("animateimg"))
public:
	CRuning(HWND hWnd, CDuiObject* pDuiObject);
	CRuning(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, 
		UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CRuning(void);
	
	HRESULT OnAttributeMaxIndex(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeRun(const CString& strValue, BOOL bLoading);
	BOOL SetRun(BOOL bRun, int nIndex = -1);
	void SetTimerCount(int nTimerCount) { m_nTimerCount = nTimerCount; }
	
protected:
	virtual	BOOL OnControlTimer();
	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	
public:
	//��������
	int m_nIndex;
	int m_nMaxIndex;
	int m_nCount;
	int m_nTimerCount;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("index"), m_nIndex, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("maxindex"), OnAttributeMaxIndex)
		DUI_INT_ATTRIBUTE(_T("timer-count"), m_nTimerCount, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("run"), OnAttributeRun)
	DUI_DECLARE_ATTRIBUTES_END()
};