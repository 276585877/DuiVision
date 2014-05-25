#include "StdAfx.h"
#include "StaticText.h"

#define	SCROLL_V	1	// �������ؼ�ID
#define	LISTBK_AREA	2	// ����Area�ؼ�ID

CStaticText::CStaticText(HWND hWnd, CDuiObject* pDuiObject)
			 : CControlBaseFont(hWnd, pDuiObject)
{
	CRect rcScroll = CRect(0,0,0,0);
	rcScroll.top;
	rcScroll.left = rcScroll.right - 8;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CScrollV(hWnd, this, SCROLL_V, rcScroll);
 	m_vecControl.push_back(pControlBase);
	m_pControScrollV = (CControlBaseFont*)pControlBase;
	m_bScrollV = FALSE;

	CRect rcBk = CRect(0,0,0,0);
	pControlBase = new CArea(hWnd, this, LISTBK_AREA, rcBk, 100, 100);
 	m_vecControl.push_back(pControlBase);
	m_pControBkArea = (CControlBase*)pControlBase;

	m_nBkTransparent = 0;

	m_enButtonState = enBSNormal;
	m_bEnableHover = FALSE;
	m_bEnableShadow = FALSE;
	m_bBack = false;
	m_clrBack = Color(0, 128, 0);
	m_clrText = Color(254, 0, 0, 0);
	m_clrTextHover = Color(128, 80, 80);
	m_clrTextShadow = Color(80, 80, 80);
	m_nStart = 0;
	m_clrMark = Color(237, 128, 27);

	m_nVirtualHeight = 0;

	SetBitmapCount(1);
}

CStaticText::CStaticText(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString m_strTitle, Color clr/* = Color(0, 0, 0)*/,
						 CString strFont, int nFontWidth/* = 12*/, FontStyle fontStyle/* = FontStyleRegular*/, BOOL bIsVisible/* = TRUE*/)
			 : CControlBaseFont(hWnd, pDuiObject, uControlID, rc, m_strTitle, bIsVisible, FALSE, FALSE, strFont, nFontWidth, fontStyle)
{
	CRect rcScroll = CRect(0,0,0,0);
	rcScroll.top;
	rcScroll.left = rcScroll.right - 8;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CScrollV(hWnd, this, SCROLL_V, rcScroll);
 	m_vecControl.push_back(pControlBase);
	m_pControScrollV = (CControlBaseFont*)pControlBase;
	m_bScrollV = FALSE;

	CRect rcBk = CRect(0,0,0,0);
	pControlBase = new CArea(hWnd, this, LISTBK_AREA, rcBk, 100, 100);
 	m_vecControl.push_back(pControlBase);
	m_pControBkArea = (CControlBase*)pControlBase;

	m_nBkTransparent = 0;

	m_enButtonState = enBSNormal;
	m_bEnableHover = FALSE;
	m_bEnableShadow = FALSE;
	m_bBack = false;
	m_clrBack = Color(0, 128, 0);
	m_clrText = clr;
	m_clrTextHover = Color(128, 80, 80);
	m_clrTextShadow = Color(80, 80, 80);
	m_nStart = 0;
	m_clrMark = Color(237, 128, 27);

	SetBitmapCount(1);
}

CStaticText::~CStaticText(void)
{

}

void CStaticText::SetMarkText(CString strMark)
{
	if(m_strMark != strMark)
	{
		m_strMark = strMark;
		UpdateControl(true);
	}
}

void CStaticText::SetTitleMarkText(CString strTitle, CString strMark, int nStart)
{
	if(m_strTitle != strTitle || m_strMark != strMark)
	{
		m_strTitle = strTitle;
		m_strMark = strMark;
		UpdateControl(true);
	}
}

void CStaticText::SetMarkText(CString strMark, Color clrMark, int nStart)
{
	if(m_strMark != strMark || *((DWORD *)&m_clrMark) != *((DWORD *)&clrMark) || m_nStart != nStart)
	{
		m_strMark = strMark;
		m_clrMark = clrMark ;
		m_nStart = nStart;
		UpdateControl(true);
	}
}

void CStaticText::SetMarkText(CString strTitle, CString strMark, Color clrMark, int nStart)
{
	if(m_strTitle != strTitle || m_strMark != strMark || *((DWORD *)&m_clrMark) != *((DWORD *)&clrMark) || m_nStart != nStart)
	{
		m_strTitle = strTitle;
		m_strMark = strMark;
		m_clrMark = clrMark ;
		m_nStart = nStart;
		UpdateControl(true);
	}
}

// ��XML���ñ�����ɫ����
HRESULT CStaticText::OnAttributeBackColor(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_clrBack = CDuiObject::HexStringToColor(strValue);
	m_bBack = true;

	return bLoading?S_FALSE:S_OK;
}

// ��XML�����ȵ���ɫ����
HRESULT CStaticText::OnAttributeTextHoverColor(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_clrTextHover = CDuiObject::HexStringToColor(strValue);
	m_bEnableHover = TRUE;

	return bLoading?S_FALSE:S_OK;
}

// ��XML������Ӱ��ɫ����
HRESULT CStaticText::OnAttributeTextShadowColor(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_clrTextShadow = CDuiObject::HexStringToColor(strValue);
	m_bEnableShadow = TRUE;

	return bLoading?S_FALSE:S_OK;
}

// ��XML����ͼƬ��Ϣ����
HRESULT CStaticText::OnAttributeImageScroll(const CStringA& strValue, BOOL bLoading)
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
		if(!m_pControScrollV->SetBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = atoi(strSkin);
		if(!m_pControScrollV->SetBitmap(nResourceID, TEXT("PNG")))
		{
			if(!m_pControScrollV->SetBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	m_bScrollV = TRUE;

	return bLoading?S_FALSE:S_OK;
}

void CStaticText::SetControlRect(CRect rc)
{
	m_rc = rc;

	CRect rcTemp;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			UINT uControlID = pControlBase->GetControlID();
			if((SCROLL_V == uControlID) && m_bScrollV)
			{
				rcTemp = m_rc;
				rcTemp.left = rcTemp.right - 8;
			}else
			if(LISTBK_AREA == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.right -= 8;
			}else
			{
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}
}

BOOL CStaticText::OnControlMouseMove(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable && m_enButtonState != enBSDown)
	{
		if(m_rc.PtInRect(point))
		{
			m_enButtonState = enBSHover;
		}
		else
		{
			m_enButtonState = enBSNormal;
		}
	}

	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

BOOL CStaticText::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			m_enButtonState = enBSDown;
		}
	}
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

BOOL CStaticText::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			if(m_enButtonState == enBSDown)
			{
				//ShellExecute(NULL, TEXT("open"), m_strLink, NULL,NULL,SW_NORMAL); 
			}
			m_enButtonState = enBSHover;			
		}
		else
		{
			m_enButtonState = enBSNormal;
		}
	}
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

// �����¼�����
BOOL CStaticText::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(!m_pControScrollV->GetVisible())
	{
		return false;
	}

	// ���¹�����,��ˢ�½���
	CScrollV* pScroll = (CScrollV*)m_pControScrollV;
	if(pScroll->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
	}

	return true;
}

// ��Ϣ��Ӧ
LRESULT CStaticText::OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OnMessage(uID, uMsg, wParam, lParam);
	return 0L; 
}

// ��Ϣ��Ӧ
LRESULT CStaticText::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == SCROLL_V) && (Msg == MSG_SCROLL_CHANGE))
	{
		// ����ǹ�������λ�ñ���¼�,��ˢ����ʾ
		UpdateControl(true);
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

// ������ʾ���ַ����ܸ߶�Ӧ���Ƕ��
int CStaticText::GetVirtualHeight()
{
	FontFamily fontFamily(m_strFont.AllocSysString());
	Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);

	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentNear);
	strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);

	int nWidth = m_rc.Width();
	if(m_bScrollV)
	{
		nWidth -= 8;
	}
	
	Size size = GetTextBounds(font, strFormat, nWidth, m_strTitle);

	// ������ֻ������Ҫ���ܸ߶ȴ����ı���ĸ߶�ʱ��Ż���ʾ
	m_pControScrollV->SetVisible(size.Height > m_rc.Height());

	return size.Height;
}

void CStaticText::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();
	
	// ������ʾλ��
	CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRange = pScrollV->GetScrollMaxRange();
	int nVirtualTop = 0;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ
	m_nVirtualHeight = GetVirtualHeight();
	if(m_nVirtualHeight > m_rc.Height())
	{
		nVirtualTop = nCurPos*(m_nVirtualHeight-m_rc.Height())/nMaxRange;
	}else
	{
		nVirtualTop = 0;
	}

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, m_nVirtualHeight);

		Graphics graphics(m_memDC);
		
		if(m_bBack)
		{
			SolidBrush brush(m_clrBack);
			graphics.FillRectangle(&brush, 0, 0, nWidth, nHeight);
		}
		else
		{
			//m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);
			m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, WHITENESS);	// ����ɫ����
			DrawVerticalTransition(m_memDC, dc, CRect(0, 0+nVirtualTop, nWidth, nHeight+nVirtualTop),	// ����͸����
					m_rc, m_nBkTransparent, m_nBkTransparent);
		}
		
		FontFamily fontFamily(m_strFont.AllocSysString());
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);

		SolidBrush solidBrush(m_clrText);
		
		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		StringFormat strFormat;
		// ˮƽ����
		if(m_uAlignment == Align_Left)
		{
			strFormat.SetAlignment(StringAlignmentNear);	// �����
		}else
		if(m_uAlignment == Align_Center)
		{
			strFormat.SetAlignment(StringAlignmentCenter);	// �м����
		}else
		if(m_uAlignment == Align_Right)
		{
			strFormat.SetAlignment(StringAlignmentFar);	// �Ҷ���
		}

		// ��ֱ����
		if(m_uVAlignment == VAlign_Top)
		{
			strFormat.SetLineAlignment(StringAlignmentNear);	// �϶���
		}else
		if(m_uVAlignment == VAlign_Middle)
		{
			strFormat.SetLineAlignment(StringAlignmentCenter);	// �м����
		}else
		if(m_uVAlignment == VAlign_Bottom)
		{
			strFormat.SetLineAlignment(StringAlignmentFar);	// �¶���
		}

		//strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
		strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		//strFormat.SetTrimming(StringTrimmingEllipsisWord);	// �Ե���Ϊ��λȥβ,��ȥ����ʹ��ʡ�Ժ�

		int nXPos = 0;
		if(m_pImage != NULL)
		{
			graphics.DrawImage(m_pImage, Rect(0, (nHeight - m_sizeImage.cy) / 2, m_sizeImage.cx, m_sizeImage.cy),
				0, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
			nXPos += m_sizeImage.cx + 5;
		}
		
		Size size = GetTextBounds(font, strFormat, nWidth, m_strTitle);
		CPoint point = GetOriginPoint(nWidth - nXPos, nHeight, size.Width, size.Height, m_uAlignment, m_uVAlignment);
		
		int nStart = m_strTitle.Find(m_strMark, m_nStart);
		if(m_strMark.IsEmpty() || (nStart == -1))
		{
			int nTextWidth = nWidth - nXPos - point.x;
			if(m_bScrollV)
			{
				nTextWidth -= 8;
			}

			// �Ȼ���Ӱ
			if(m_bEnableShadow)
			{
				RectF rectShadow(nXPos + point.x + 1, point.y + 1, nTextWidth, max(size.Height, nHeight));
				SolidBrush solidBrushS(m_clrTextShadow);
				graphics.DrawString(m_strTitle.AllocSysString(), (INT)wcslen(m_strTitle.AllocSysString()), &font, 
					rectShadow, &strFormat, &solidBrushS);
			}

			// �ٻ�����������
			RectF rect(nXPos + point.x, point.y, nTextWidth, max(size.Height, nHeight));
			if((m_enButtonState == enBSHover) && m_bEnableHover)
			{
				SolidBrush solidBrushH(m_clrTextHover);
				graphics.DrawString(m_strTitle.AllocSysString(), (INT)wcslen(m_strTitle.AllocSysString()), &font, 
					rect, &strFormat, &solidBrushH);	
			}else
			{
				graphics.DrawString(m_strTitle.AllocSysString(), (INT)wcslen(m_strTitle.AllocSysString()), &font, 
					rect, &strFormat, &solidBrush);	
			}
		}
		else
		{
			SolidBrush solidBrushM(m_clrMark);
			SolidBrush solidBrushS(m_clrTextShadow);

			CString srtL = m_strTitle.Left(nStart);
			CString srtR  = m_strTitle.Right(m_strTitle.GetLength() - m_strMark.GetLength() - nStart);
			Size sizeL = GetTextBounds(font, strFormat, srtL);
			Size sizeM = GetTextBounds(font, strFormat, m_strMark);
			Size sizeR = GetTextBounds(font, strFormat, srtR);

			if(m_bEnableShadow)
			{
				graphics.DrawString(srtL.AllocSysString(), (INT)wcslen(srtL.AllocSysString()), &font, 
					PointF(nXPos + point.x + 1, point.y + 1), &strFormat, &solidBrushS);
				graphics.DrawString(m_strMark.AllocSysString(), (INT)wcslen(m_strMark.AllocSysString()), &font, 
					PointF(nXPos + point.x + sizeL.Width + 2 + 1, point.y + 1), &strFormat, &solidBrushS);
			}
			graphics.DrawString(srtL.AllocSysString(), (INT)wcslen(srtL.AllocSysString()), &font, 
				PointF(nXPos + point.x, point.y), &strFormat, &solidBrush);
			graphics.DrawString(m_strMark.AllocSysString(), (INT)wcslen(m_strMark.AllocSysString()), &font, 
				PointF(nXPos + point.x + sizeL.Width + 2, point.y), &strFormat, &solidBrushM);

			if(m_bEnableShadow)
			{
				RectF rect(nXPos + point.x + sizeL.Width + sizeM.Width + 4 + 1, point.y + 1, nWidth - (nXPos + sizeL.Width + sizeM.Width + 4 + point.x), nHeight);
				graphics.DrawString(srtR.AllocSysString(), (INT)wcslen(srtR.AllocSysString()), &font, 
					PointF(nXPos + point.x + sizeL.Width + sizeM.Width + 4, point.y), &strFormat, &solidBrushS);
			}
			//RectF rect(nXPos + point.x + sizeL.Width + sizeM.Width + 4, point.y, nWidth - (nXPos + sizeL.Width + sizeM.Width + 4 + point.x), size.Height);
			RectF rect(nXPos + point.x + sizeL.Width + sizeM.Width + 4, point.y, nWidth - (nXPos + sizeL.Width + sizeM.Width + 4 + point.x), nHeight);
			graphics.DrawString(srtR.AllocSysString(), (INT)wcslen(srtR.AllocSysString()), &font, 
				PointF(nXPos + point.x + sizeL.Width + sizeM.Width + 4, point.y), &strFormat, &solidBrush);
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, nVirtualTop, SRCCOPY);
}