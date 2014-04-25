// ͼƬ�ؼ�
#pragma once
#include "ControlBase.h"

// ͼƬ��ʾģʽ
enum enumShowMode
{
	enSMNormal = 0,			// ����
	enSMTile,				// ƽ��
	enSMExtrude,			// ����
	enSMFrame,				// �߿�
	enSMMID					// �Ź���߿�
};

class CDuiPicture : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiPicture, "img")
public:
	CDuiPicture(HWND hWnd, CDuiObject* pDuiObject);
	CDuiPicture(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, 
		BOOL bIsVisible = TRUE);
	virtual ~CDuiPicture(void);

	BOOL SetShowMode(enumShowMode enShowMode, int nFrameSize = 2);
	BOOL SetShowModeMID(int nWLT, int nHLT, int nWRB, int nHRB);

protected:
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	enumShowMode	m_enShowMode;	// ��ʾģʽ
	int				m_nFrameSize;	// �߿��С(enSMFrameģʽ)
	int				m_nWLT;			// ���Ͻǿ��(�Ź���ģʽ)
	int				m_nHLT;			// ���ϽǸ߶�(�Ź���ģʽ)
	int				m_nWRB;			// ���½ǿ��(�Ź���ģʽ)
	int				m_nHRB;			// ���½Ǹ߶�(�Ź���ģʽ)

	DUI_DECLARE_ATTRIBUTES_BEGIN()
 		DUI_INT_ATTRIBUTE("framesize", m_nFrameSize, FALSE)
		DUI_INT_ATTRIBUTE("width-lt", m_nWLT, FALSE)
		DUI_INT_ATTRIBUTE("height-lt", m_nHLT, FALSE)
		DUI_INT_ATTRIBUTE("width-rb", m_nWRB, FALSE)
		DUI_INT_ATTRIBUTE("height-rb", m_nHRB, FALSE)
		DUI_ENUM_ATTRIBUTE("mode", enumShowMode, TRUE)
            DUI_ENUM_VALUE("normal", enSMNormal)
            DUI_ENUM_VALUE("tile", enSMTile)
            DUI_ENUM_VALUE("extrude", enSMExtrude)
			DUI_ENUM_VALUE("frame", enSMFrame)
			DUI_ENUM_VALUE("mid", enSMMID)
        DUI_ENUM_END(m_enShowMode)
    DUI_DECLARE_ATTRIBUTES_END()
};
