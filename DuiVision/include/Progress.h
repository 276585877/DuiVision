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

	BOOL SetBackGroundImage(UINT nResourceID = 0, CString strType= TEXT("PNG"));
	BOOL SetBackGroundImage(CString strImage = TEXT(""));
	BOOL SetForeGroundImage(UINT nResourceID = 0, CString strType= TEXT("PNG"));
	BOOL SetForeGroundImage(CString strImage = TEXT(""));
	
protected:
	virtual	BOOL OnControlTimer();
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	HRESULT OnAttributeRun(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImageBackGround(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImageForeGround(const CStringA& strValue, BOOL bLoading);
	
public:
	//��������
	int				m_nIndex;			// ��ǰ����(=m_nProgress)
	int				m_nMaxIndex;		// ������(100)
	int				m_nCount;			// ��ʱ���ٴμ�������һ��
	
	int				m_nProgress;		// ��ǰ����(0-100)

	Image*			m_pImageBackGround;	// ����ͼƬ
	CSize			m_sizeBackGround;	// ����ͼƬ��С
	Image*			m_pImageForeGround;	// ǰ��ͼƬ
	CSize			m_sizeForeGround;	// ǰ��ͼƬ��С
	int				m_nHeadLength;		// ������ͼƬͷ������

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE("value", m_nProgress, FALSE)
		DUI_CUSTOM_ATTRIBUTE("run", OnAttributeRun)
		DUI_CUSTOM_ATTRIBUTE("img-back", OnAttributeImageBackGround)
		DUI_CUSTOM_ATTRIBUTE("img-fore", OnAttributeImageForeGround)
		DUI_INT_ATTRIBUTE("head-len", m_nHeadLength, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};