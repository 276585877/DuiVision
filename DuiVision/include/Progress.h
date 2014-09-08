#pragma once
#include "ControlBase.h"


class CDuiProgress : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiProgress, _T("progress"))
public:
	CDuiProgress(HWND hWnd, CDuiObject* pDuiObject);
	CDuiProgress(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress = 0,
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CDuiProgress(void);

	int SetProgress(int nProgress);
	int GetProgress() { return m_nProgress; }
	int SetMaxProgress(int nMaxProgress) { m_nMaxProgress = nMaxProgress; }
	int GetMaxProgress() { return m_nMaxProgress; }
	BOOL SetRun(BOOL bRun, int nIndex = -1);
	
protected:
	virtual	BOOL OnControlTimer();
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	HRESULT OnAttributeRun(const CString& strValue, BOOL bLoading);
	
public:
	//��������
	int				m_nProgress;		// ��ǰ����(0-100)
	int				m_nMaxProgress;		// ������ֵ(Ĭ��100)
	int				m_nCount;			// ��ʱ����������
	int				m_nTimerCount;		// ��ʱ���ٴν�������һ��

	int				m_nHeadLength;		// ������ͼƬͷ������

	DUI_IMAGE_ATTRIBUTE_DEFINE(BackGround);	// ���屳��ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(ForeGround);	// ����ǰ��ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("value"), m_nProgress, FALSE)
		DUI_INT_ATTRIBUTE(_T("max-value"), m_nMaxProgress, FALSE)
		DUI_INT_ATTRIBUTE(_T("timer-count"), m_nTimerCount, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("run"), OnAttributeRun)
		DUI_CUSTOM_ATTRIBUTE(_T("img-back"), OnAttributeImageBackGround)
		DUI_CUSTOM_ATTRIBUTE(_T("img-fore"), OnAttributeImageForeGround)
		DUI_INT_ATTRIBUTE(_T("head-len"), m_nHeadLength, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};