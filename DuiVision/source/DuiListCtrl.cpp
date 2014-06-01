#include "StdAfx.h"
#include "DuiListCtrl.h"

#define	SCROLL_V	1	// �������ؼ�ID
#define	LISTBK_AREA	2	// ����Area�ؼ�ID

CDuiListCtrl::CDuiListCtrl(HWND hWnd, CDuiObject* pDuiObject)
			: CDuiPanel(hWnd, pDuiObject)
{
	/*CRect rcScroll = CRect(0,0,0,0);
	rcScroll.top;
	rcScroll.left = rcScroll.right - 8;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CScrollV(hWnd, this, SCROLL_V, rcScroll);
 	m_vecControl.push_back(pControlBase);
	m_pControScrollV = (CControlBaseFont*)pControlBase;*/

	CRect rcBk = CRect(0,0,0,0);
	CControlBase* pControlBase = new CArea(hWnd, this, LISTBK_AREA, rcBk, 100, 100);
 	m_vecControl.push_back(pControlBase);
	m_pControBkArea = (CControlBase*)pControlBase;

	m_strFontTitle = DuiSystem::GetDefaultFont();
	m_nFontTitleWidth = 12;
	m_fontTitleStyle = FontStyleRegular;

	m_clrText = Color(225, 64, 64, 64);
	m_clrTextHover = Color(128, 0, 0);
	m_clrTextDown = Color(0, 112, 235);
	m_clrTitle = Color(255, 32, 32, 32);
	m_clrSeperator = Color(200, 160, 160, 160);
	m_nRowHeight = 50;

	m_pImageSeperator = NULL;
	m_sizeSeperator = CSize(0, 0);
	m_pImageCheckBox = NULL;
	m_sizeCheckBox = CSize(0, 0);

	m_nBkTransparent = 30;

	m_nHoverRow = 0;
	m_nDownRow = 0;
	m_bSingleLine = TRUE;
	m_bTextWrap = FALSE;

	m_bRowTooltip = TRUE;
	m_nTipRow = -1;
	m_nTipVirtualTop = 0;

	m_nFirstViewRow = 0;
	m_nLastViewRow = 0;
	m_nVirtualTop = 0;
}

CDuiListCtrl::~CDuiListCtrl(void)
{
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}
}

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDuiListCtrl::Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl)
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

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > m_rc.Height());

	// �����²��row�ڵ���Ϣ
	TiXmlElement* pRowElem = NULL;
	for (pRowElem = pXmlElem->FirstChildElement("row"); pRowElem != NULL; pRowElem=pRowElem->NextSiblingElement())
	{
		CStringA strIdA = pRowElem->Attribute("id");
		CStringA strTitleA = pRowElem->Attribute("title");
		CStringA strContentA = pRowElem->Attribute("content");
		CStringA strTimeA = pRowElem->Attribute("time");
		CStringA strCheckA = pRowElem->Attribute("check");
		CStringA strImageA = pRowElem->Attribute("image");
		CStringA strRightImageA = pRowElem->Attribute("right-img");
		CStringA strClrTextA = pRowElem->Attribute("crtext");
		CStringA strLink1A = pRowElem->Attribute("link1");
		CStringA strLinkAction1A = pRowElem->Attribute("linkaction1");
		CStringA strLink2A = pRowElem->Attribute("link2");
		CStringA strLinkAction2A = pRowElem->Attribute("linkaction2");

		DuiSystem::Instance()->ParseDuiString(strTitleA);
		DuiSystem::Instance()->ParseDuiString(strContentA);

		int nCheck = -1;
		if(!strCheckA.IsEmpty())
		{
			nCheck = atoi(strCheckA);
		}

		// ���ͼƬ,ͨ��Skin��ȡ
		CStringA strSkin = "";
		if(strImageA.Find("skin:") == 0)
		{
			strSkin = DuiSystem::Instance()->GetSkin(strImageA);
		}else
		{
			strSkin = strImageA;
		}

		int nImageIndex = -1;
		CString strImage = _T("");
		if(strSkin.Find(".") != -1)
		{
			// ͼƬ�ļ�
			strImage = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		}else
		if(!strSkin.IsEmpty())
		{
			// ͼƬ����
			nImageIndex = atoi(strSkin);
		}

		// �ұ�ͼƬ,ͨ��Skin��ȡ
		CStringA strRightSkin = "";
		if(strRightImageA.Find("skin:") == 0)
		{
			strRightSkin = DuiSystem::Instance()->GetSkin(strRightImageA);
		}else
		{
			strRightSkin = strRightImageA;
		}

		int nRightImageIndex = -1;
		CString strRightImage = _T("");
		if(strRightSkin.Find(".") != -1)
		{
			// ͼƬ�ļ�
			strRightImage = DuiSystem::GetSkinPath() + CA2T(strRightSkin, CP_UTF8);
		}else
		if(!strRightSkin.IsEmpty())
		{
			// ͼƬ����
			nRightImageIndex = atoi(strRightSkin);
		}

		CString strId = CA2T(strIdA, CP_UTF8);
		CString strTitle = CA2T(strTitleA, CP_UTF8);
		CString strContent = CA2T(strContentA, CP_UTF8);
		CString strTime = CA2T(strTimeA, CP_UTF8);
		Color clrText = Color(0,0,0,0);
		if(!strClrTextA.IsEmpty())
		{
			if(strClrTextA.Find(",") == -1)
			{
				clrText = CDuiObject::HexStringToColor(strClrTextA);
			}else
			{
				clrText = CDuiObject::StringToColor(strClrTextA);
			}
		}
		CString strLink1 = CA2T(strLink1A, CP_UTF8);
		CString strLinkAction1 = CA2T(strLinkAction1A, CP_UTF8);
		CString strLink2 = CA2T(strLink2A, CP_UTF8);
		CString strLinkAction2 = CA2T(strLinkAction2A, CP_UTF8);
		InsertItem(-1, strId, strTitle, strContent, strTime, nImageIndex, clrText, strImage, nRightImageIndex, strRightImage,
			strLink1, strLinkAction1, strLink2, strLinkAction2, nCheck);
	}

    return TRUE;
}

// ����б���
BOOL CDuiListCtrl::InsertItem(int nItem, CString strId, CString strTitle, CString strContent, CString strTime,
							  int nImageIndex, Color clrText, CString strImage, int nRightImageIndex, CString strRightImage,
							  CString strLink1, CString strLinkAction1, CString strLink2, CString strLinkAction2,
							  int nCheck)
{
	if(!strContent.IsEmpty())
	{
		m_bSingleLine = FALSE;
	}

	ListRowInfo rowInfo;
	rowInfo.strId = strId;
	rowInfo.strTitle = strTitle;
	rowInfo.strContent = strContent;
	rowInfo.strTime = strTime;
	rowInfo.nCheck = nCheck;
	rowInfo.nImageIndex = nImageIndex;
	rowInfo.sizeImage.SetSize(0, 0);
	rowInfo.nRightImageIndex = nRightImageIndex;
	rowInfo.sizeRightImage.SetSize(0, 0);
	rowInfo.bRowColor = FALSE;
	rowInfo.clrText = clrText;
	rowInfo.strLink1 = strLink1;
	rowInfo.strLinkAction1 = strLinkAction1;
	rowInfo.strLink2 = strLink2;
	rowInfo.strLinkAction2 = strLinkAction2;
	rowInfo.nHoverLink = -1;
	rowInfo.bNeedTitleTip = FALSE;
	rowInfo.bNeedContentTip = FALSE;
	if(clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
	{
		rowInfo.bRowColor = TRUE;
	}

	// ���ͼƬ
	if(!strImage.IsEmpty() && (strImage.Find(_T(":")) == -1))
	{
		strImage = DuiSystem::GetSkinPath() + strImage;
	}
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		rowInfo.pImage = Image::FromFile(strImage, TRUE);
		if(rowInfo.pImage->GetLastStatus() == Ok)
		{
			rowInfo.sizeImage.SetSize(rowInfo.pImage->GetWidth() / 1, rowInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		rowInfo.pImage = NULL;
		if((rowInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	// �ұ�ͼƬ
	if(!strRightImage.IsEmpty() && (strRightImage.Find(_T(":")) == -1))
	{
		strRightImage = DuiSystem::GetSkinPath() + strRightImage;
	}
	if(!strRightImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		rowInfo.pRightImage = Image::FromFile(strRightImage, TRUE);
		if(rowInfo.pRightImage->GetLastStatus() == Ok)
		{
			rowInfo.sizeRightImage.SetSize(rowInfo.pRightImage->GetWidth() / 1, rowInfo.pRightImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		rowInfo.pRightImage = NULL;
		if((rowInfo.nRightImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeRightImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	//rowInfo.rcRow.SetRect(m_rc.left, m_rc.top, m_rc.left, m_rc.bottom);

	return InsertItem(nItem, rowInfo);
}

// ����б���
BOOL CDuiListCtrl::InsertItem(int nItem, CString strTitle, int nCheck, Color clrText, int nImageIndex, CString strLink1, CString strLinkAction1, CString strLink2, CString strLinkAction2)
{
	return InsertItem(nItem, _T(""), strTitle, _T(""), _T(""), nImageIndex, clrText, _T(""), -1, _T(""), strLink1, strLinkAction1, strLink2, strLinkAction2, nCheck);
}

BOOL CDuiListCtrl::InsertItem(int nItem, ListRowInfo &rowInfo)
{
	if(m_vecRowInfo.size() > 0)
	{
		CRect rc;
	}
	if(nItem <= -1 || nItem >= m_vecRowInfo.size())
	{
		m_vecRowInfo.push_back(rowInfo);
	}
	else
	{
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nItem, rowInfo);
	}

	int nXPos = 0;//m_rc.left;
	int nYPos = 0;//m_rc.top;

	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		ListRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - 8;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		rowInfoTemp.rcCheck.SetRect(0,0,0,0);
		rowInfoTemp.rcLink1.SetRect(0,0,0,0);
		rowInfoTemp.rcLink2.SetRect(0,0,0,0);

		nYPos += m_nRowHeight;
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > m_rc.Height());

	UpdateControl(true);
	return true;
}

// ɾ���б���
BOOL CDuiListCtrl::DeleteItem(int nItem)
{
	if((nItem < 0) || (nItem >= m_vecRowInfo.size()))
	{
		return FALSE;
	}

	int nIndex = 0;
	vector<ListRowInfo>::iterator it;
	for(it=m_vecRowInfo.begin();it!=m_vecRowInfo.end();++it)
	{
		if(nIndex == nItem)
		{
			m_vecRowInfo.erase(it);
			break;
		}
		nIndex++;
	}

	UpdateControl(true);
	return true;
}

// ��ȡĳһ���б���
ListRowInfo* CDuiListCtrl::GetItemInfo(int nRow)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return NULL;
	}

	ListRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return &rowInfo;
}

// ����ĳһ���е���ɫ
void CDuiListCtrl::SetRowColor(int nRow, Color clrText)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return;
	}

	ListRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowColor = TRUE;
	rowInfo.clrText = clrText;
}

// ����б�
void CDuiListCtrl::ClearItems()
{
	m_vecRowInfo.clear();
	m_pControScrollV->SetVisible(FALSE);
	UpdateControl(true);
}

// ��XML����Font-title����
HRESULT CDuiListCtrl::OnAttributeFontTitle(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	DuiFontInfo fontInfo;
	BOOL bFindFont = DuiSystem::Instance()->GetFont(strValue, fontInfo);
	if (!bFindFont) return E_FAIL;

	m_strFontTitle = fontInfo.strFont;
	m_nFontTitleWidth = fontInfo.nFontWidth;	
	m_fontTitleStyle = fontInfo.fontStyle;

	return bLoading?S_FALSE:S_OK;
}

// ���÷ָ���ͼƬ��Դ
BOOL CDuiListCtrl::SetSeperator(UINT nResourceID/* = 0*/, CString strType/*= TEXT("PNG")*/)
{
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}

	if(ImageFromIDResource(nResourceID, strType, m_pImageSeperator))
	{
		m_sizeSeperator.SetSize(m_pImageSeperator->GetWidth(), m_pImageSeperator->GetHeight());
		return true;
	}
	return false;
}

// ���÷ָ���ͼƬ�ļ�
BOOL CDuiListCtrl::SetSeperator(CString strImage/* = TEXT("")*/)
{
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}

	m_pImageSeperator = Image::FromFile(strImage, TRUE);

	if(m_pImageSeperator->GetLastStatus() == Ok)
	{
		m_sizeSeperator.SetSize(m_pImageSeperator->GetWidth(), m_pImageSeperator->GetHeight());
		return true;
	}
	return false;
}

// ��XML���÷ָ�ͼƬ��Ϣ����
HRESULT CDuiListCtrl::OnAttributeImageSeperator(const CStringA& strValue, BOOL bLoading)
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
		if(!SetSeperator(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = atoi(strSkin);
		if(!SetSeperator(nResourceID, TEXT("PNG")))
		{
			if(!SetSeperator(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

// ���ü���ͼƬ��Դ
BOOL CDuiListCtrl::SetCheckBoxImage(UINT nResourceID/* = 0*/, CString strType/*= TEXT("PNG")*/)
{
	if(m_pImageCheckBox != NULL)
	{
		delete m_pImageCheckBox;
		m_pImageCheckBox = NULL;
	}

	if(ImageFromIDResource(nResourceID, strType, m_pImageCheckBox))
	{
		m_sizeCheckBox.SetSize(m_pImageCheckBox->GetWidth() / 6, m_pImageCheckBox->GetHeight());
		return true;
	}
	return false;
}

// ���ü���ͼƬ�ļ�
BOOL CDuiListCtrl::SetCheckBoxImage(CString strImage/* = TEXT("")*/)
{
	if(m_pImageCheckBox != NULL)
	{
		delete m_pImageCheckBox;
		m_pImageCheckBox = NULL;
	}

	m_pImageCheckBox = Image::FromFile(strImage, TRUE);

	if(m_pImageCheckBox->GetLastStatus() == Ok)
	{
		m_sizeCheckBox.SetSize(m_pImageCheckBox->GetWidth() / 6, m_pImageCheckBox->GetHeight());
		return true;
	}
	return false;
}

// ��XML���ü���ͼƬ��Ϣ����
HRESULT CDuiListCtrl::OnAttributeImageCheckBox(const CStringA& strValue, BOOL bLoading)
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
		if(!SetCheckBoxImage(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = atoi(strSkin);
		if(!SetCheckBoxImage(nResourceID, TEXT("PNG")))
		{
			if(!SetCheckBoxImage(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

void CDuiListCtrl::SetControlRect(CRect rc)
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

	// ���¼��������е�λ��
	int nXPos = 0;
	int nYPos = 0;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		ListRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - 8;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		nYPos += m_nRowHeight;
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > m_rc.Height());
}

// �ж�ָ��������λ���Ƿ���ĳһ����
BOOL CDuiListCtrl::PtInRow(CPoint point, ListRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcRow;
	// rcRow�����ǲ����нڵ�ʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�еļ�����
BOOL CDuiListCtrl::PtInRowCheck(CPoint point, ListRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcCheck;
	// rcCheck�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�е�ĳ��������,������������
int CDuiListCtrl::PtInRowLink(CPoint point, ListRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcLink1;
	// rcLink�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	if(rc.PtInRect(point))
	{
		return 0;
	}

	rc = rowInfo.rcLink2;
	// rcLink�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	if(rc.PtInRect(point))
	{
		return 1;
	}

	return -1;
}

// �����е�Tooltip
void CDuiListCtrl::SetRowTooltip(int nRow, CString strTooltip)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return;
	}

	CDlgBase* pDlg = GetParentDialog();
	if(pDlg && ((m_nTipRow != nRow) || (m_nTipVirtualTop != m_nVirtualTop)))
	{
		ListRowInfo &rowInfo = m_vecRowInfo.at(nRow);
		if(rowInfo.bNeedTitleTip || rowInfo.bNeedContentTip)
		{
			CRect rc = rowInfo.rcRow;
			rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
			pDlg->SetTooltip(this, strTooltip, rc);
		}
		m_nTipRow = nRow;
		m_nTipVirtualTop = m_nVirtualTop;
	}
}

// ���Tooltip
void CDuiListCtrl::ClearRowTooltip()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		m_nTipRow = -1;
		m_nTipVirtualTop = 0;
	}
}

BOOL CDuiListCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	int nOldHoverRow = m_nHoverRow;
	BOOL bHoverLinkChange = FALSE;
	int nOldHoverLink = -1;

	if(m_rc.PtInRect(point))
	{
		// ���������ȵ���,�ж�����Ƿ���ĳ������λ��
		if((m_nHoverRow != -1) && (m_nHoverRow < m_vecRowInfo.size()))
		{
			ListRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
			nOldHoverLink = rowInfo.nHoverLink;
			if(PtInRow(point, rowInfo))
			{
				if(m_bRowTooltip)
				{
					if(rowInfo.bNeedTitleTip)
					{
						SetRowTooltip(m_nHoverRow, rowInfo.strTitle);
					}else
					if(rowInfo.bNeedContentTip)
					{
						SetRowTooltip(m_nHoverRow, rowInfo.strContent);
					}else
					{
						ClearRowTooltip();
					}
				}

				rowInfo.nHoverLink = PtInRowLink(point, rowInfo);
				if(nOldHoverLink != rowInfo.nHoverLink)
				{
					bHoverLinkChange = TRUE;
					UpdateControl(TRUE);
				}
				return false;
			}
			m_nHoverRow = -1;		
		}

		// �������ڵ�ǰ��,�ж�����Ƿ���ĳ������λ��
		BOOL bMousenDown = false;
		if((m_nDownRow != -1) && (m_nDownRow < m_vecRowInfo.size()))
		{
			ListRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
			nOldHoverLink = rowInfo.nHoverLink;
			if(PtInRow(point, rowInfo))
			{
				if(m_bRowTooltip)
				{
					if(rowInfo.bNeedTitleTip)
					{
						SetRowTooltip(m_nDownRow, rowInfo.strTitle);
					}else
					if(rowInfo.bNeedContentTip)
					{
						SetRowTooltip(m_nDownRow, rowInfo.strContent);
					}else
					{
						ClearRowTooltip();
					}
				}

				rowInfo.nHoverLink = PtInRowLink(point, rowInfo);
				bMousenDown = true;
				m_nHoverRow = -1;
				if(nOldHoverLink != rowInfo.nHoverLink)
				{
					bHoverLinkChange = TRUE;
				}
			}		
		}

		// �����ȵ��к��ȵ�����
		if(!bMousenDown)
		{
			for(size_t i = 0; i < m_vecRowInfo.size(); i++)
			{
				ListRowInfo &rowInfo = m_vecRowInfo.at(i);
				nOldHoverLink = rowInfo.nHoverLink;
				if(PtInRow(point, rowInfo))
				{
					rowInfo.nHoverLink = PtInRowLink(point, rowInfo);
					m_nHoverRow = i;
					if(nOldHoverLink != rowInfo.nHoverLink)
					{
						bHoverLinkChange = TRUE;
					}
					break;
				}
			}
		}
	}
	else
	{
		m_nHoverRow = -1;
	}

	if((nOldHoverRow != m_nHoverRow) || bHoverLinkChange)
	{
		UpdateControl(TRUE);
		return true;
	}
	
	return false;
}

BOOL CDuiListCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// ���ô��ڽ���,��������޷����й����¼��Ĵ���
	SetWindowFocus();

	if(m_nHoverRow != -1)
	{
		ListRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverLink = PtInRowLink(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				m_nDownRow = m_nHoverRow;
				m_nHoverRow = -1;

				SendMessage(BUTTOM_DOWN, m_nDownRow, rowInfo.nHoverLink);

				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if(m_nDownRow != -1)
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		ListRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverLink = PtInRowLink(point, rowInfo);
			SendMessage(BUTTOM_DOWN, m_nDownRow, rowInfo.nHoverLink);
			return true;
		}
	}
	
	return false;
}

BOOL CDuiListCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if(m_nHoverRow != -1)
	{
		ListRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(BUTTOM_UP, m_nDownRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if(m_nDownRow != -1)
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		ListRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(BUTTOM_UP, m_nDownRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}
	}

	return false;
}

// �����¼�����
BOOL CDuiListCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if((m_vecRowInfo.size() * m_nRowHeight) <= m_rc.Height())
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
LRESULT CDuiListCtrl::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == SCROLL_V) && (Msg == MSG_SCROLL_CHANGE))
	{
		// ����ǹ�������λ�ñ���¼�,��ˢ����ʾ
		UpdateControl(true);
	}else
	if((uID == GetID()) && (Msg == BUTTOM_DOWN) && (lParam != -1))
	{
		// ������е�ĳ������
		ListRowInfo* pRowInfo = GetItemInfo(wParam);
		if(pRowInfo)
		{
			// ת��ΪBUTTOM_UP��Ϣ,��ΪDuiSystem������ʱ��ֻ����BUTTOM_UP��Ϣ
			if((lParam == 0) && (!pRowInfo->strLinkAction1.IsEmpty()))
			{
				DuiSystem::AddDuiActionTask(uID, BUTTOM_UP, wParam, lParam, GetName(), pRowInfo->strLinkAction1, GetParent());
			}else
			if((lParam == 1) && (!pRowInfo->strLinkAction2.IsEmpty()))
			{
				DuiSystem::AddDuiActionTask(uID, BUTTOM_UP, wParam, lParam, GetName(), pRowInfo->strLinkAction2, GetParent());
			}
		}
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

void CDuiListCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	// �б�ͼ����:
	// 1.�б������߶�Ϊÿһ�и߶�*����
	// 2.�б���ʾ��top������scroll�ؼ���¼
	// 3.�ػ�ʱ��,����top����λ�ü������ʾ�ĵ�һ�е����,������ʾ�߶ȼ������ʾ�����һ�е����
	// 4.���ݼ��������ʾ����,����Ӧ�����ݵ��ڴ�dc��
	// 5.�������ʾ��top��������ڴ�dc�Ŀ���
	int nWidth = m_rc.Width() - 8;	// ��ȥ�������Ŀ��
	int nHeightAll = m_vecRowInfo.size()*m_nRowHeight; // �ܵ�����߶� //m_rc.Height();
	CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRange = pScrollV->GetScrollMaxRange();

	m_nVirtualTop = nCurPos*(nHeightAll-m_rc.Height())/nMaxRange;	// ��ǰ������λ�ö�Ӧ�������topλ��
	m_nFirstViewRow = m_nVirtualTop / m_nRowHeight;					// ��ʾ�ĵ�һ�����
	m_nLastViewRow = (m_nVirtualTop + m_rc.Height()) / m_nRowHeight;	// ��ʾ�����һ�����
	if(m_nLastViewRow >= m_vecRowInfo.size())
	{
		m_nLastViewRow = m_vecRowInfo.size() - 1;
	}
	if(m_nLastViewRow < 0)
	{
		m_nLastViewRow = 0;
	}
	int nHeightView = (m_nLastViewRow - m_nFirstViewRow +1) * m_nRowHeight;	// ��ʾ�漰��������߶�
	int nYViewPos = m_nVirtualTop - (m_nFirstViewRow * m_nRowHeight);		// �ڴ�dc��ʾ����Ļʱ���topλ��
	if(nYViewPos < 0)
	{
		nYViewPos = 0;
	}

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeightView);

		Graphics graphics(m_memDC);
		
		m_memDC.BitBlt(0, 0, nWidth, nHeightView, &dc, m_rc.left ,m_rc.top, WHITENESS);	// ����ɫ����
		DrawVerticalTransition(m_memDC, dc, CRect(0, nYViewPos, nWidth, m_rc.Height()+nYViewPos),	// ����͸����
				m_rc, m_nBkTransparent, m_nBkTransparent);
		
		FontFamily fontFamilyTitle(m_strFontTitle.AllocSysString());
		Font fontTitle(&fontFamilyTitle, (REAL)m_nFontTitleWidth, m_fontTitleStyle, UnitPixel);

		FontFamily fontFamily(m_strFont.AllocSysString());
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);

		SolidBrush solidBrush(m_clrText);			// �������ֻ�ˢ
		SolidBrush solidBrushH(m_clrTextHover);		// �ȵ����ֻ�ˢ
		SolidBrush solidBrushD(m_clrTextDown);		// ��ǰ�л�ˢ
		SolidBrush solidBrushT(m_clrTitle);			// �������ֻ�ˢ
		SolidBrush solidBrushS(m_clrSeperator);		// �ָ��߻�ˢ

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// ��ͨ���ֵĶ��뷽ʽ
		StringFormat strFormat;
		strFormat.SetAlignment(StringAlignmentNear);	// �����
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
		strFormat.SetTrimming(StringTrimmingEllipsisWord);	// �Ե���Ϊ��λȥβ,��ȥ����ʹ��ʡ�Ժ�
		//strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		if(!m_bTextWrap)
		{
			strFormat.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
		}

		// ʱ���ֶβ����Ҷ���
		StringFormat strFormatRight;
		strFormatRight.SetAlignment(StringAlignmentFar);	// �Ҷ���
		if(m_uVAlignment == VAlign_Top)
		{
			strFormatRight.SetLineAlignment(StringAlignmentNear);	// �϶���
		}else
		if(m_uVAlignment == VAlign_Middle)
		{
			strFormatRight.SetLineAlignment(StringAlignmentCenter);	// �м����
		}else
		if(m_uVAlignment == VAlign_Bottom)
		{
			strFormatRight.SetLineAlignment(StringAlignmentFar);	// �¶���
		}
		//strFormatRight.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		if(!m_bTextWrap)
		{
			strFormatRight.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
		}
		
		if(m_vecRowInfo.size() > 0)
		{
			for(size_t i = m_nFirstViewRow; i <= m_nLastViewRow && i <= m_vecRowInfo.size(); i++)
			{
				ListRowInfo &rowInfo = m_vecRowInfo.at(i);
				SolidBrush solidBrushRow(rowInfo.clrText);	// �ж������ɫ

				int nXPos = 0;
				int nVI = i - m_nFirstViewRow;

				// ������
				int nCheckImgY = 3;
				if((m_sizeCheckBox.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
				{
					nCheckImgY = (m_nRowHeight - m_sizeCheckBox.cy) / 2 + 1;
				}
				if((rowInfo.nCheck != -1) && (m_pImageCheckBox != NULL))
				{
					int nCheckImageIndex = ((m_nHoverRow == i) ? ((rowInfo.nCheck==1) ? 4 : 1) : ((rowInfo.nCheck==1) ? 2 : 0));
					graphics.DrawImage(m_pImageCheckBox, Rect(nXPos, nVI*m_nRowHeight + nCheckImgY, m_sizeCheckBox.cx, m_sizeCheckBox.cy),
						nCheckImageIndex * m_sizeCheckBox.cx, 0, m_sizeCheckBox.cx, m_sizeCheckBox.cy, UnitPixel);
					rowInfo.rcCheck.SetRect(nXPos, i*m_nRowHeight + nCheckImgY, nXPos + m_sizeCheckBox.cx, i*m_nRowHeight + nCheckImgY + m_sizeCheckBox.cy);
					nXPos += (m_sizeCheckBox.cx + 3);
				}

				// �������ͼƬ
				int nImgY = 3;
				if(rowInfo.pImage != NULL)
				{
					if((rowInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
					}
					// ʹ��������ָ����ͼƬ
					graphics.DrawImage(rowInfo.pImage, Rect(nXPos, nVI*m_nRowHeight + nImgY, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy),
						0, 0, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy, UnitPixel);
					nXPos += (rowInfo.sizeImage.cx + 3);
				}else
				if((rowInfo.nImageIndex != -1) && (m_pImage != NULL))
				{
					if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
					}
					// ʹ������ͼƬ
					graphics.DrawImage(m_pImage, Rect(nXPos, nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nXPos += (m_sizeImage.cx + 3);
				}

				// �����ұ�ͼƬ
				int nRightImageWidth = 0;
				nImgY = 3;
				if(rowInfo.pRightImage != NULL)
				{
					if((rowInfo.sizeRightImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - rowInfo.sizeRightImage.cy) / 2 + 1;
					}
					// ʹ��������ָ����ͼƬ
					graphics.DrawImage(rowInfo.pRightImage, Rect(nWidth-rowInfo.sizeRightImage.cx-1, nVI*m_nRowHeight + nImgY, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy),
						0, 0, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy, UnitPixel);
					nRightImageWidth = rowInfo.sizeRightImage.cx + 1;
				}else
				if((rowInfo.nRightImageIndex != -1) && (m_pImage != NULL))
				{
					if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
					}
					// ʹ������ͼƬ
					graphics.DrawImage(m_pImage, Rect(nWidth-m_sizeImage.cx-1, nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nRightImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nRightImageWidth = m_sizeImage.cx + 1;
				}

				// ������
				RectF rect(nXPos, nVI*m_nRowHeight + 1, nWidth-20, m_bSingleLine ? (m_nRowHeight - 2) : (m_nRowHeight / 2 - 2) );
				if(!rowInfo.strTime.IsEmpty())
				{
					Size size = GetTextBounds(font, rowInfo.strTime);
					rect.Width -= (size.Width + 5);
				}

				// ��������
				int nLinkWidth = 0;
				if(!rowInfo.strLink2.IsEmpty())
				{
					Size sizeLink = GetTextBounds(font, strFormatRight, rowInfo.strLink2);
					nLinkWidth += (sizeLink.Width + 10);
					RectF rectLink(nWidth-nLinkWidth-nRightImageWidth, nVI*m_nRowHeight + (m_nRowHeight - sizeLink.Height)/2, sizeLink.Width+8, sizeLink.Height);
					rowInfo.rcLink2.SetRect(rectLink.X,rectLink.Y,rectLink.X+sizeLink.Width,rectLink.Y+rectLink.Height);
					rowInfo.rcLink2.OffsetRect(0, m_nFirstViewRow*m_nRowHeight);
					if(((m_nHoverRow == i) || (m_nDownRow == i)) && (rowInfo.nHoverLink == 1))
					{
						graphics.DrawString(rowInfo.strLink2.AllocSysString(), (INT)wcslen(rowInfo.strLink2.AllocSysString()), &font, 
							rectLink, &strFormatRight, &solidBrushH);
					}else
					{
						graphics.DrawString(rowInfo.strLink2.AllocSysString(), (INT)wcslen(rowInfo.strLink2.AllocSysString()), &font, 
							rectLink, &strFormatRight, &solidBrush);
					}
				}
				if(!rowInfo.strLink1.IsEmpty())
				{
					Size sizeLink = GetTextBounds(font, strFormatRight, rowInfo.strLink1);
					nLinkWidth += (sizeLink.Width + 10);
					RectF rectLink(nWidth-nLinkWidth-nRightImageWidth, nVI*m_nRowHeight + (m_nRowHeight - sizeLink.Height)/2, sizeLink.Width+8, sizeLink.Height);
					rowInfo.rcLink1.SetRect(rectLink.X,rectLink.Y,rectLink.X+sizeLink.Width,rectLink.Y+rectLink.Height);
					rowInfo.rcLink1.OffsetRect(0, m_nFirstViewRow*m_nRowHeight);
					if(((m_nHoverRow == i) || (m_nDownRow == i)) && (rowInfo.nHoverLink == 0))
					{
						graphics.DrawString(rowInfo.strLink1.AllocSysString(), (INT)wcslen(rowInfo.strLink1.AllocSysString()), &font, 
							rectLink, &strFormatRight, &solidBrushH);
					}else
					{
						graphics.DrawString(rowInfo.strLink1.AllocSysString(), (INT)wcslen(rowInfo.strLink1.AllocSysString()), &font, 
							rectLink, &strFormatRight, &solidBrush);
					}
				}
				
				rect.Width -= nLinkWidth;
				// �����Ƿ���Ҫ��ʾtip
				rowInfo.bNeedTitleTip = rect.Width < GetTextBounds(font, rowInfo.strTitle).Width;
				rowInfo.bNeedContentTip = rect.Width < GetTextBounds(font, rowInfo.strContent).Width;
				RectF rectTime(nWidth-nRightImageWidth-2-100, nVI*m_nRowHeight + 1, 100, m_bSingleLine ? m_nRowHeight : (m_nRowHeight / 2) );
				if(m_nHoverRow == i)
				{
					// ��������
					graphics.DrawString(rowInfo.strTitle.AllocSysString(), (INT)wcslen(rowInfo.strTitle.AllocSysString()), &fontTitle, 
						rect, &strFormat, &solidBrushH);
					// ��ʱ��
					if(!rowInfo.strTime.IsEmpty())
					{
						graphics.DrawString(rowInfo.strTime.AllocSysString(), (INT)wcslen(rowInfo.strTime.AllocSysString()), &font, 
						rectTime, &strFormatRight, &solidBrushH);
					}
					// ��������
					if(!m_bSingleLine)
					{
						rect.Offset(0, m_nRowHeight / 2 + 2);
						rect.Width = nWidth-20;
						rect.Height = m_nRowHeight / 2 - 4;
						graphics.DrawString(rowInfo.strContent.AllocSysString(), (INT)wcslen(rowInfo.strContent.AllocSysString()), &font, 
								rect, &strFormat, &solidBrushH);
					}
				}else
				if(m_nDownRow == i)
				{
					// ��������
					graphics.DrawString(rowInfo.strTitle.AllocSysString(), (INT)wcslen(rowInfo.strTitle.AllocSysString()), &fontTitle, 
						rect, &strFormat, &solidBrushD);
					// ��ʱ��
					if(!rowInfo.strTime.IsEmpty())
					{
						graphics.DrawString(rowInfo.strTime.AllocSysString(), (INT)wcslen(rowInfo.strTime.AllocSysString()), &font, 
						rectTime, &strFormatRight, &solidBrushD);
					}
					// ��������
					if(!m_bSingleLine)
					{
						rect.Offset(0, m_nRowHeight / 2 + 2);
						rect.Width = nWidth-20;
						rect.Height = m_nRowHeight / 2 - 4;
						graphics.DrawString(rowInfo.strContent.AllocSysString(), (INT)wcslen(rowInfo.strContent.AllocSysString()), &font, 
								rect, &strFormat, &solidBrushD);
					}
				}else
				{
					// ��������
					graphics.DrawString(rowInfo.strTitle.AllocSysString(), (INT)wcslen(rowInfo.strTitle.AllocSysString()), &fontTitle, 
						rect, &strFormat, rowInfo.bRowColor ? &solidBrushRow : &solidBrushT);
					// ��ʱ��
					if(!rowInfo.strTime.IsEmpty())
					{
						graphics.DrawString(rowInfo.strTime.AllocSysString(), (INT)wcslen(rowInfo.strTime.AllocSysString()), &font, 
						rectTime, &strFormatRight, rowInfo.bRowColor ? &solidBrushRow : &solidBrush);
					}
					// ��������
					if(!m_bSingleLine)
					{
						rect.Offset(0, m_nRowHeight / 2 + 2);
						rect.Width = nWidth-20;
						rect.Height = m_nRowHeight / 2 - 4;
						graphics.DrawString(rowInfo.strContent.AllocSysString(), (INT)wcslen(rowInfo.strContent.AllocSysString()), &font, 
								rect, &strFormat, rowInfo.bRowColor ? &solidBrushRow : &solidBrush);
					}
				}

				// ���ָ���(��������ģʽ)
				if(m_pImageSeperator != NULL)
				{
					// ʹ������ģʽ���Ի�ͼ
					graphics.DrawImage(m_pImageSeperator, RectF(0, (nVI+1)*m_nRowHeight, nWidth-2, m_sizeSeperator.cy),
							0, 0, m_sizeSeperator.cx, m_sizeSeperator.cy, UnitPixel);

					// ʹ��ƽ�̷�ʽ������(�ݲ�ʹ�����ַ�ʽ)
					//TextureBrush tileBrush(m_pImageSeperator, WrapModeTile);
					//graphics.FillRectangle(&tileBrush, RectF(0, (nVI+1)*m_nRowHeight, nWidth-2, m_sizeSeperator.cy));
				}else
				{
					// δָ��ͼƬ,�򻭾���
					graphics.FillRectangle(&solidBrushS, 0, (nVI+1)*m_nRowHeight, nWidth-2, 1);
				}
			}
		}
	}

	// ���������DC,ʹ����ķ�ʽ�ϲ�����
	dc.BitBlt(m_rc.left,m_rc.top, nWidth, m_rc.Height(), &m_memDC, 0, nYViewPos, SRCCOPY);//SRCAND);
}