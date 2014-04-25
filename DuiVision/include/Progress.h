#pragma once
#include "ControlBase.h"


class CDuiProgress : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiProgress, "progress")
public:
	CDuiProgress(HWND hWnd, CDuiObject* pDuiObject);
	CDuiProgress(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress = 0,
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CDuiProgress(void);

	int SetProgress(int nProgress);
	int GetProgress() { return m_nProgress; };
	BOOL SetRun(BOOL bRun, int nIndex = -1);

	HRESULT OnAttributeRun(const CStringA& strValue, BOOL bLoading);
	
protected:
	virtual	BOOL OnControlTimer();
	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	
public:
	//��������
	int m_nIndex;		// ��ǰ����(=m_nProgress)
	int m_nMaxIndex;// ������(100)
	int m_nCount;		// ��ʱ���ٴμ�������һ��
	
	int m_nProgress;	// ��ǰ����(0-100)

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE("value", m_nProgress, FALSE)
		DUI_CUSTOM_ATTRIBUTE("run", OnAttributeRun)
	DUI_DECLARE_ATTRIBUTES_END()
};