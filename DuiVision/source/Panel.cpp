#include "StdAfx.h"
#include "Panel.h"

#define	SCROLL_V	1	// �������ؼ�ID

CDuiPanel::CDuiPanel(HWND hWnd, CDuiObject* pDuiObject)
			: CControlBaseFont(hWnd, pDuiObject)
{
	CRect rcScroll = CRect(0,0,0,0);
	rcScroll.top;
	rcScroll.left = rcScroll.right - 8;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CScrollV(hWnd, this, SCROLL_V, rcScroll);
 	m_vecControl.push_back(pControlBase);
	m_pControScrollV = (CControlBaseFont*)pControlBase;

	m_strXmlFile = _T("");
	m_nVirtualHeight = 0;
	m_nVirtualTop = 0;
	m_bInit = false;
}

CDuiPanel::~CDuiPanel(void)
{
}

// ���ݿؼ��������ؼ�ʵ��
CControlBase* CDuiPanel::_CreateControlByName(LPCSTR lpszName)
{
	HWND hWnd = NULL;
	CDuiObject* pParentObj = GetParent();
	while((pParentObj != NULL) && (!pParentObj->IsClass("dlg")))
	{
		pParentObj = ((CControlBase*)pParentObj)->GetParent();
	}

	if((pParentObj != NULL) && pParentObj->IsClass("dlg"))
	{
		return DuiSystem::CreateControlByName(lpszName, ((CDlgBase*)pParentObj)->GetSafeHwnd(), this);
	}

	return NULL;
}

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDuiPanel::Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

    // ʹ��XML�ڵ��ʼ��div�ؼ�
	if(pXmlElem != NULL)
	{
		InitUI(m_rc, pXmlElem);
	}

    return TRUE;
}

// ����
BOOL CDuiPanel::LoadXmlFile(CString strFileName)
{
	TiXmlDocument xmlDoc;
	TiXmlElement* pDivElem = NULL;

	m_strXmlFile = DuiSystem::GetXmlPath() + strFileName;

	xmlDoc.LoadFile(CEncodingUtil::UnicodeToAnsi(m_strXmlFile), TIXML_ENCODING_UTF8);
	if(!xmlDoc.Error())
	{
		pDivElem = xmlDoc.FirstChildElement("div");
		if(pDivElem != NULL)
		{
			// ����div�ڵ�����
			Load(pDivElem);
		}
	}

	return TRUE;
}

// ��XML����ͼƬ��Ϣ����
HRESULT CDuiPanel::OnAttributeImageScroll(const CStringA& strValue, BOOL bLoading)
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

	return bLoading?S_FALSE:S_OK;
}

// ��XML����XML����,����XML�ļ���������Ϊdiv�����ڵ�����
HRESULT CDuiPanel::OnAttributeXml(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	TiXmlDocument xmlDoc;
	TiXmlElement* pDivElem = NULL;

	m_strXmlFile = DuiSystem::GetXmlPath() + CEncodingUtil::AnsiToUnicode(strValue);

	xmlDoc.LoadFile(CEncodingUtil::UnicodeToAnsi(m_strXmlFile), TIXML_ENCODING_UTF8);
	if(!xmlDoc.Error())
	{
		pDivElem = xmlDoc.FirstChildElement("div");
		if(pDivElem != NULL)
		{
			// ����div�ڵ�����
			Load(pDivElem);
		}
	}

	return bLoading?S_FALSE:S_OK;
}

// ��ʼ�����ڿؼ�
void CDuiPanel::InitUI(CRect rcClient, TiXmlElement* pNode)
{
	CRect rcTemp;
	int nStartX = 0;
	int nStartY = 0;
	CControlBase * pControlBase = NULL;

	// �������д��ڿؼ�
	if(pNode)
	{
		m_nVirtualHeight = 0;
		TiXmlElement* pControlElem = NULL;
		for (pControlElem = pNode->FirstChildElement(); pControlElem != NULL; pControlElem=pControlElem->NextSiblingElement())
		{
			if(pControlElem != NULL)
			{
				CStringA strControlName = pControlElem->Value();
				CControlBase* pControl = _CreateControlByName(strControlName);
				if(pControl)
				{
					pControl->Load(pControlElem);
					if(pControl->IsClass(CArea::GetClassName()) || pControl->IsClass(CFrame::GetClassName()))
					{
						// Area��Frame������Ӧ���,����ӵ�Area�б���
						m_vecArea.push_back(pControl);
					}else
					{
						m_vecControl.push_back(pControl);
					}

					CRect rcCtrl = pControl->GetRect();
					if(rcCtrl.bottom > m_nVirtualHeight)
					{
						m_nVirtualHeight = rcCtrl.bottom - m_rc.top;
					}
				}
			}
		}

		// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
		m_pControScrollV->SetVisible(m_nVirtualHeight > m_rc.Height());
	}

	m_bInit = true;
}

// ��������߶�
void CDuiPanel::CalcVirtualHeight()
{
	CRect rcTemp;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			rcTemp = pControlBase->GetRect();
			if(rcTemp.bottom > m_nVirtualHeight)
			{
				// ˢ��Panel������߶�
				m_nVirtualHeight = rcTemp.bottom - m_rc.top;
			}
		}
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible(m_nVirtualHeight > m_rc.Height());
}

void CDuiPanel::SetControlRect(CRect rc)
{
	m_rc = rc;
	CRect rcTemp;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			UINT uControlID = pControlBase->GetControlID();
			if(SCROLL_V == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.left = rcTemp.right - 8;
			}else
			{
				rcTemp = pControlBase->GetRect();
				if(rcTemp.bottom > m_nVirtualHeight)
				{
					// ˢ��Panel������߶�
					m_nVirtualHeight = rcTemp.bottom - m_rc.top;
				}
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible(m_nVirtualHeight > m_rc.Height());
}

// �������ÿؼ��ɼ��Եĺ�������Ҫ�����ӿؼ��ĺ���
void CDuiPanel::SetControlVisible(BOOL bIsVisible)
{
	__super::SetControlVisible(bIsVisible);

	// ����ÿ���ӿؼ���ԭ��Windows�ؼ��Ŀɼ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			if(pControlBase->IsClass("div"))
			{
				pControlBase->SetControlVisible(bIsVisible);
			}else
			{
				// Panel�ɼ��Ա仯ʱ��,ֻ������ԭ���ؼ�,��������ʾԭ���ؼ�
				if(!bIsVisible)
				{
					pControlBase->SetControlWndVisible(bIsVisible);
				}
			}
		}
	}
}

// �������ÿؼ��Ƿ���õĺ�������Ҫ�����ӿؼ��ĺ���
void CDuiPanel::SetControlDisable(BOOL bIsDisable)
{
	__super::SetControlDisable(bIsDisable);

	// ����ÿ���ӿؼ���ԭ��Windows�ؼ��Ŀɼ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetControlDisable(bIsDisable);
		}
	}
}

void CDuiPanel::DrawControl(CDC &dc, CRect rcUpdate)
{
}

// ���ӿؼ�(֧�ֹ�����ʾ)
BOOL CDuiPanel::DrawSubControls(CDC &dc, CRect rcUpdate)
{
	// �������Ҫ������,��ֱ��ʹ�ø��ؼ��Ļ�ͼ����
	if(m_nVirtualHeight < m_rc.Height())
	{
		return __super::DrawSubControls(dc, rcUpdate);
	}

	// ������ʾλ��
	CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRange = pScrollV->GetScrollMaxRange();
	int nVirtualTop = nCurPos*(m_nVirtualHeight-m_rc.Height())/nMaxRange;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ

	// ���ݹ�����λ�û��ӿؼ�
	// ��ʼ��������ʾdc,�����Ʊ�����������ʾdc
	CBitmap	virtualBitmap;
	CDC virtualDC;
	virtualDC.CreateCompatibleDC(&dc);
	virtualBitmap.CreateCompatibleBitmap(&dc, m_rc.right-8, m_rc.top + m_nVirtualHeight);
	CBitmap* pOldVirtualBitmap = virtualDC.SelectObject(&virtualBitmap);
	virtualDC.BitBlt(m_rc.left, m_rc.top+nVirtualTop, m_rc.Width()-8, m_rc.Height(), &dc, m_rc.left, m_rc.top, SRCCOPY);	// �������Ƶ�������ʾdc

	// ������������ʾ�������
	rcUpdate.OffsetRect(0, nVirtualTop);

	// ���ӿؼ�������dc
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->GetVisible() && (pControlBase != m_pControScrollV))
		{
			// �������λ�÷����˱仯,��Ҫ�ػ������ӿؼ�,��������ٶȺ���
			if(m_nVirtualTop != nVirtualTop)
			{
				pControlBase->UpdateControl(true);
			}
			pControlBase->Draw(virtualDC, rcUpdate);		
		}
	}

	m_pControScrollV->Draw(dc, rcUpdate);

	m_nVirtualTop = nVirtualTop;

	// ������ʾdc���Ƶ�dc
	dc.BitBlt(m_rc.left, m_rc.top, m_rc.Width()-8, m_rc.Height(), &virtualDC, m_rc.left, m_rc.top + nVirtualTop, SRCCOPY);

	// �ͷ�������ʾdc
	virtualDC.SelectObject(pOldVirtualBitmap);
	virtualDC.DeleteDC();
	virtualBitmap.DeleteObject();

	return TRUE;
}

// �������任
BOOL CDuiPanel::OnMousePointChange(CPoint& point)
{
	if(m_nVirtualHeight > m_rc.Height())
	{
		// ������ʾλ��
		CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
		int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
		int nMaxRange = pScrollV->GetScrollMaxRange();
		int nVirtualTop = nCurPos*(m_nVirtualHeight-m_rc.Height())/nMaxRange;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ

		point.Offset(0, nVirtualTop);

		return TRUE;
	}

	return FALSE;
}

// �����¼�����
BOOL CDuiPanel::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(m_nVirtualHeight < m_rc.Height())
	{
		return false;
	}

	// ���¹�����,��ˢ�½���
	CScrollV* pScroll = (CScrollV*)m_pControScrollV;
	if(pScroll->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
		// ˢ�¹�����
		pScroll->UpdateControl(true);
	}

	return true;
}

// ��Ϣ��Ӧ
LRESULT CDuiPanel::OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OnMessage(uID, uMsg, wParam, lParam);
	return 0L; 
}

LRESULT CDuiPanel::OnControlUpdate(CRect rcUpdate, BOOL bUpdate, CControlBase *pUpdateControlBase) 
{
	if(pUpdateControlBase == NULL) return 0;

	if(bUpdate)
	{
		pUpdateControlBase->SetUpdate(FALSE);
	}

	int nVirtualTop = 0;
	if(m_nVirtualHeight > m_rc.Height())
	{
		// ������ʾλ��
		CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
		int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
		int nMaxRange = pScrollV->GetScrollMaxRange();
		nVirtualTop = nCurPos*(m_nVirtualHeight-m_rc.Height())/nMaxRange;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ
	}

	CRect rcAllUpDate = rcUpdate;

	if(m_bInit)
	{
		BOOL bFind = false;

		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase)
			{
				if(bFind)
				{
					if (pControlBase->GetVisible() && !(pControlBase->GetRect() & rcUpdate).IsRectEmpty())
					{
						rcAllUpDate |= pControlBase->GetRect();
						pControlBase->SetUpdate(FALSE);
					}
				}
				else if(pControlBase == pUpdateControlBase)
				{
					bFind = true;
				}
			}
		}
	}

	// ���չ���λ���޸ĸ�������
	rcAllUpDate.OffsetRect(0, -nVirtualTop);
	rcAllUpDate |= m_rc;

	InvalidateRect(&rcAllUpDate);

	return 0L; 
};

// ��Ϣ����
LRESULT CDuiPanel::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ����ÿ���ؼ������ĸ��ؼ��Ǵ�ID��������¼��Ĵ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && (pControlBase->GetControlID() == uID))
		{
			return pControlBase->OnMessage(uID, Msg, wParam, lParam);
		}
	}

	// ����¼�δ����,����ø������Ϣ����,���ջ��͸����¼����������д���
	return __super::OnMessage(uID, Msg, wParam, lParam);
}