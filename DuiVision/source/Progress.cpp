#include "StdAfx.h"
#include "Progress.h"

CDuiProgress::CDuiProgress(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBaseFont(hWnd, pDuiObject)
{
	m_bRunTime = false;
	m_nIndex = 0;
	m_nMaxIndex = 100;
	m_nCount = 0;

	SetBitmapCount(2);

	m_pImageBackGround = NULL;
	m_sizeBackGround = CSize(0, 0);
	m_pImageForeGround = NULL;
	m_sizeForeGround = CSize(0, 0);
	m_nHeadLength = 0;

	m_nProgress = 0;
	SetProgress(0);
}

CDuiProgress::CDuiProgress(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress/* = 0*/, 
					 BOOL bIsVisible/* = TRUE*/, BOOL bIsDisable/* = FALSE*/)
	: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, TEXT(""), bIsVisible, bIsDisable)
{
	m_bRunTime = false;
	m_nIndex = 0;
	m_nMaxIndex = 100;
	m_nCount = 0;

	SetBitmapCount(2);

	m_pImageBackGround = NULL;
	m_sizeBackGround = CSize(0, 0);
	m_pImageForeGround = NULL;
	m_sizeForeGround = CSize(0, 0);
	m_nHeadLength = 0;

	m_nProgress = 0;
	SetProgress(nProgress);
}

CDuiProgress::~CDuiProgress(void)
{
}

int CDuiProgress::SetProgress(int nProgress)
{
	int nOldProgress = m_nProgress;
	if(nProgress >= 0 && nProgress <= 100 && m_nProgress != nProgress)
	{
		m_nProgress = nProgress;
		UpdateControl(true);
	}
	return nOldProgress;
}

// �����Ƿ��Զ�����
BOOL CDuiProgress::SetRun(BOOL bRun, int nIndex/* = -1*/)
{
	m_bRunTime = bRun;
	if(nIndex != -1)
	{
		m_nIndex = nIndex;
		m_nProgress = nIndex;
	}
	UpdateControl();
	return TRUE;
}

// ��XML�����Զ���������
HRESULT CDuiProgress::OnAttributeRun(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	BOOL bRun = (strValue == "true");
	SetRun(bRun);

	return bLoading?S_FALSE:S_OK;
}

// ���ñ���ͼƬ��Դ
BOOL CDuiProgress::SetBackGroundImage(UINT nResourceID/* = 0*/, CString strType/*= TEXT("PNG")*/)
{
	if(m_pImageBackGround != NULL)
	{
		delete m_pImageBackGround;
		m_pImageBackGround = NULL;
	}

	if(ImageFromIDResource(nResourceID, strType, m_pImageBackGround))
	{
		m_sizeBackGround.SetSize(m_pImageBackGround->GetWidth(), m_pImageBackGround->GetHeight());
		return true;
	}
	return false;
}

// ���ñ���ͼƬ�ļ�
BOOL CDuiProgress::SetBackGroundImage(CString strImage/* = TEXT("")*/)
{
	if(m_pImageBackGround != NULL)
	{
		delete m_pImageBackGround;
		m_pImageBackGround = NULL;
	}

	m_pImageBackGround = Image::FromFile(strImage, TRUE);

	if(m_pImageBackGround->GetLastStatus() == Ok)
	{
		m_sizeBackGround.SetSize(m_pImageBackGround->GetWidth(), m_pImageBackGround->GetHeight());
		return true;
	}
	return false;
}

// ��XML���ñ���ͼƬ��Ϣ����
HRESULT CDuiProgress::OnAttributeImageBackGround(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CStringA strSkin = "";
	if(strValue.Find("skin:") == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(".") != -1)	// ����ͼƬ�ļ�
	{
		CString strImgFile = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		if(strSkin.Find(":") != -1)
		{
			strImgFile = CA2T(strSkin, CP_UTF8);
		}
		if(!SetBackGroundImage(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = atoi(strSkin);
		if(!SetBackGroundImage(nResourceID, TEXT("PNG")))
		{
			if(!SetBackGroundImage(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

// ����ǰ��ͼƬ��Դ
BOOL CDuiProgress::SetForeGroundImage(UINT nResourceID/* = 0*/, CString strType/*= TEXT("PNG")*/)
{
	if(m_pImageForeGround != NULL)
	{
		delete m_pImageForeGround;
		m_pImageForeGround = NULL;
	}

	if(ImageFromIDResource(nResourceID, strType, m_pImageForeGround))
	{
		m_sizeForeGround.SetSize(m_pImageForeGround->GetWidth(), m_pImageForeGround->GetHeight());
		return true;
	}
	return false;
}

// ����ǰ��ͼƬ�ļ�
BOOL CDuiProgress::SetForeGroundImage(CString strImage/* = TEXT("")*/)
{
	if(m_pImageForeGround != NULL)
	{
		delete m_pImageForeGround;
		m_pImageForeGround = NULL;
	}

	m_pImageForeGround = Image::FromFile(strImage, TRUE);

	if(m_pImageForeGround->GetLastStatus() == Ok)
	{
		m_sizeForeGround.SetSize(m_pImageForeGround->GetWidth(), m_pImageForeGround->GetHeight());
		return true;
	}
	return false;
}

// ��XML����ǰ��ͼƬ��Ϣ����
HRESULT CDuiProgress::OnAttributeImageForeGround(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CStringA strSkin = "";
	if(strValue.Find("skin:") == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(".") != -1)	// ����ͼƬ�ļ�
	{
		CString strImgFile = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		if(strSkin.Find(":") != -1)
		{
			strImgFile = CA2T(strSkin, CP_UTF8);
		}
		if(!SetForeGroundImage(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = atoi(strSkin);
		if(!SetForeGroundImage(nResourceID, TEXT("PNG")))
		{
			if(!SetForeGroundImage(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

BOOL CDuiProgress::OnControlTimer()
{
	if(!m_bRunTime || !m_bIsVisible)
	{
		return FALSE;
	}

	if(++m_nCount == 3)
	{
		m_nCount = 0;
		if(++m_nIndex >= m_nMaxIndex)
		{
			m_nIndex = 0;
		}
		m_nProgress = m_nIndex;
		UpdateControl();
		return true;
	}

	return false;
}

void CDuiProgress::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeight);

		m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

		Graphics graphics(m_memDC);

		if(m_pImageForeGround != NULL)	// ʹ�ñ�����ǰ��ͼƬ��������
		{
			if(m_pImageBackGround != NULL)	// ������
			{
				DrawImageFrameMID(graphics, m_pImageBackGround, CRect(0, 0, nWidth, nHeight),
					0, 0, m_sizeBackGround.cx, m_sizeBackGround.cy,
					m_nHeadLength, 0, m_nHeadLength, 0);
			}

			if(m_nProgress != 0)	// ��ǰ��
			{
				DrawImageFrameMID(graphics, m_pImageForeGround, CRect(0, 0, nWidth * m_nProgress / 100, nHeight),
					0, 0, m_sizeForeGround.cx, m_sizeForeGround.cy,
					m_nHeadLength, 0, m_nHeadLength, 0);
			}
		}else
		if(m_pImage != NULL)	// ʹ�õ���ͼƬ��������
		{
			DrawImageFrame(graphics, m_pImage, CRect(0, 0, nWidth, nHeight), 
				0, 0, m_sizeImage.cx, m_sizeImage.cy, 2);

			if(m_nProgress != 0)
			{
				DrawImageFrame(graphics, m_pImage, CRect(0, 0, nWidth * m_nProgress / 100, nHeight), 
					m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, 2);
			}
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);
}