#include "StdAfx.h"
#include "DuiListCtrl.h"

#define	SCROLL_V	1	// �������ؼ�ID
#define	LISTBK_AREA	2	// ����Area�ؼ�ID

CDuiGridCtrl::CDuiGridCtrl(HWND hWnd, CDuiObject* pDuiObject)
			: CDuiPanel(hWnd, pDuiObject)
{
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
	m_nHeaderHeight = 0;
	m_nLeftPos = 0;

	m_pImageSeperator = NULL;
	m_sizeSeperator = CSize(0, 0);
	m_pImageCheckBox = NULL;
	m_sizeCheckBox = CSize(0, 0);

	m_nBkTransparent = 30;

	m_nHoverRow = 0;
	m_nDownRow = -1;
	m_bEnableDownRow = FALSE;
	m_bSingleLine = TRUE;
	m_bTextWrap = FALSE;

	m_bGridTooltip = TRUE;
	m_nTipRow = -1;
	m_nTipItem = -1;
	m_nTipVirtualTop = 0;

	m_nFirstViewRow = 0;
	m_nLastViewRow = 0;
	m_nVirtualTop = 0;
}

CDuiGridCtrl::~CDuiGridCtrl(void)
{
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}
	if(m_pImageCheckBox != NULL)
	{
		delete m_pImageCheckBox;
		m_pImageCheckBox = NULL;
	}
}

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, CheckBox, 6)

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDuiGridCtrl::Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl)
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
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > (m_rc.Height() - m_nHeaderHeight));
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange(m_vecRowInfo.size() * m_nRowHeight);

	// �����²��cloumn�ڵ���Ϣ
	TiXmlElement* pColumnElem = NULL;
	for (pColumnElem = pXmlElem->FirstChildElement("column"); pColumnElem != NULL; pColumnElem=pColumnElem->NextSiblingElement())
	{
		CStringA strTitleA = pColumnElem->Attribute("title");
		CStringA strClrTextA = pColumnElem->Attribute("crtext");
		CStringA strWidthA = pColumnElem->Attribute("width");
		DuiSystem::Instance()->ParseDuiString(strTitleA);
		CString strTitle = CA2T(strTitleA, CP_UTF8);
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
		int nWidth = -1;
		if(!strWidthA.IsEmpty())
		{
			nWidth = atoi(strWidthA);
		}
		InsertColumn(-1, strTitle, nWidth, clrText);
	}

	// �����²��row�ڵ���Ϣ
	TiXmlElement* pRowElem = NULL;
	for (pRowElem = pXmlElem->FirstChildElement("row"); pRowElem != NULL; pRowElem=pRowElem->NextSiblingElement())
	{
		CStringA strIdA = pRowElem->Attribute("id");
		CStringA strCheckA = pRowElem->Attribute("check");
		CStringA strImageA = pRowElem->Attribute("image");
		CStringA strRightImageA = pRowElem->Attribute("right-img");
		CStringA strClrTextA = pRowElem->Attribute("crtext");

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
		InsertRow(-1, strId, nImageIndex, clrText, strImage, nRightImageIndex, strRightImage, nCheck);

		int nRowIndex = m_vecRowInfo.size()-1;
		int nItemIndex = 0;
		// �����²��item�ڵ���Ϣ
		TiXmlElement* pItemElem = NULL;
		for (pItemElem = pRowElem->FirstChildElement("item"); pItemElem != NULL; pItemElem=pItemElem->NextSiblingElement())
		{
			CStringA strTitleA = pItemElem->Attribute("title");
			CStringA strContentA = pItemElem->Attribute("content");
			CStringA strClrTextA = pItemElem->Attribute("crtext");
			CStringA strImageA = pItemElem->Attribute("image");
			CStringA strLinkA = pItemElem->Attribute("link");
			CStringA strLinkActionA = pItemElem->Attribute("linkaction");
			CStringA strFontTitleA = pItemElem->Attribute("font-title");
			DuiSystem::Instance()->ParseDuiString(strTitleA);
			DuiSystem::Instance()->ParseDuiString(strContentA);
			DuiSystem::Instance()->ParseDuiString(strLinkA);
			DuiSystem::Instance()->ParseDuiString(strLinkActionA);
			CString strTitle = CA2T(strTitleA, CP_UTF8);
			CString strContent = CA2T(strContentA, CP_UTF8);
			CString strLink = CA2T(strLinkA, CP_UTF8);
			CString strLinkAction = CA2T(strLinkActionA, CP_UTF8);
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

			// ͼƬ,ͨ��Skin��ȡ
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

			BOOL bUseTitleFont = (strFontTitleA == "1");

			if(!strLink.IsEmpty())
			{
				SetSubItemLink(nRowIndex, nItemIndex, strLink, strLinkAction, nImageIndex, clrText, strImage);
			}else
			{
				SetSubItem(nRowIndex, nItemIndex, strTitle, strContent, bUseTitleFont, nImageIndex, clrText, strImage);
			}
			nItemIndex++;
		}
	}

    return TRUE;
}

// �����
BOOL CDuiGridCtrl::InsertColumn(int nColumn, CString strTitle, int nWidth, Color clrText)
{
	GridColumnInfo columnInfo;
	columnInfo.strTitle = strTitle;
	columnInfo.clrText = clrText;
	columnInfo.nWidth = nWidth;
	if(nColumn <= -1 || nColumn >= m_vecColumnInfo.size())
	{
		m_vecColumnInfo.push_back(columnInfo);
	}
	else
	{
		m_vecColumnInfo.insert(m_vecColumnInfo.begin() + nColumn, columnInfo);
	}

	int nXPos = 0;
	int nYPos = 0;

	for(size_t i = 0; i < m_vecColumnInfo.size(); i++)
	{
		GridColumnInfo &columnInfoTemp = m_vecColumnInfo.at(i);
		int nWidth = columnInfoTemp.nWidth;
		if(nWidth == -1)
		{
			nWidth = m_rc.Width() - nXPos;
		}
		columnInfoTemp.rcHeader.SetRect(nXPos, nYPos, nXPos + nWidth, nYPos + m_nRowHeight);
		nXPos += columnInfoTemp.nWidth;
	}

	UpdateControl(true);
	return true;
}

// �����
BOOL CDuiGridCtrl::InsertRow(int nItem, CString strId, int nImageIndex, Color clrText, CString strImage,
							 int nRightImageIndex, CString strRightImage, int nCheck)
{
	GridRowInfo rowInfo;
	rowInfo.strId = strId;
	rowInfo.nCheck = nCheck;
	rowInfo.nImageIndex = nImageIndex;
	rowInfo.sizeImage.SetSize(0, 0);
	rowInfo.nRightImageIndex = nRightImageIndex;
	rowInfo.sizeRightImage.SetSize(0, 0);
	rowInfo.bRowColor = FALSE;
	rowInfo.clrText = clrText;
	rowInfo.nHoverItem = -1;
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
		rowInfo.pImage = Image::FromFile(strImage, m_bImageUseECM);
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
		rowInfo.pRightImage = Image::FromFile(strRightImage, m_bImageUseECM);
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

	return InsertRow(nItem, rowInfo);
}

// �����
BOOL CDuiGridCtrl::InsertRow(int nItem, GridRowInfo &rowInfo)
{
	if(nItem <= -1 || nItem >= m_vecRowInfo.size())
	{
		m_vecRowInfo.push_back(rowInfo);
	}
	else
	{
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nItem, rowInfo);
	}

	// �������б���е�λ��
	CalcRowsPos();	

	UpdateControl(true);
	return true;
}

// ���ñ��������(���ֱ����)
BOOL CDuiGridCtrl::SetSubItem(int nRow, int nItem, CString strTitle, CString strContent, BOOL bUseTitleFont, int nImageIndex, Color clrText, CString strImage)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		GridItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = bUseTitleFont;
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strTitle = strTitle;
	itemInfo.strContent = strContent;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	GridColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// ͼƬ
	if(!strImage.IsEmpty() && (strImage.Find(_T(":")) == -1))
	{
		strImage = DuiSystem::GetSkinPath() + strImage;
	}
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		itemInfo.pImage = Image::FromFile(strImage, m_bImageUseECM);
		if(itemInfo.pImage->GetLastStatus() == Ok)
		{
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / 1, itemInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		itemInfo.pImage = NULL;
		if((itemInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			itemInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return TRUE;
}

// ���ñ��������(���ӱ����)
BOOL CDuiGridCtrl::SetSubItemLink(int nRow, int nItem, CString strLink, CString strLinkAction, int nImageIndex, Color clrText, CString strImage)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		GridItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = FALSE;
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strLink = strLink;
	itemInfo.strLinkAction = strLinkAction;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	GridColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// ͼƬ
	if(!strImage.IsEmpty() && (strImage.Find(_T(":")) == -1))
	{
		strImage = DuiSystem::GetSkinPath() + strImage;
	}
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		itemInfo.pImage = Image::FromFile(strImage, m_bImageUseECM);
		if(itemInfo.pImage->GetLastStatus() == Ok)
		{
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / 1, itemInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		itemInfo.pImage = NULL;
		if((itemInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			itemInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return TRUE;
}

// ɾ����
BOOL CDuiGridCtrl::DeleteRow(int nItem)
{
	if((nItem < 0) || (nItem >= m_vecRowInfo.size()))
	{
		return FALSE;
	}

	int nIndex = 0;
	vector<GridRowInfo>::iterator it;
	for(it=m_vecRowInfo.begin();it!=m_vecRowInfo.end();++it)
	{
		if(nIndex == nItem)
		{
			m_vecRowInfo.erase(it);
			break;
		}
		nIndex++;
	}

	// �������б���е�λ��
	CalcRowsPos();

	UpdateControl(true);
	return true;
}

// ��������λ��
void CDuiGridCtrl::CalcRowsPos()
{
	int nXPos = 0;//m_rc.left;
	int nYPos = 0;//m_rc.top;

	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		rowInfoTemp.rcCheck.SetRect(0,0,0,0);

		nYPos += m_nRowHeight;
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > (m_rc.Height() - m_nHeaderHeight));
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange(m_vecRowInfo.size() * m_nRowHeight);
}

// ��ȡĳһ������Ϣ
GridRowInfo* CDuiGridCtrl::GetRowInfo(int nRow)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return &rowInfo;
}

// ��ȡĳһ����Ԫ����Ϣ
GridItemInfo* CDuiGridCtrl::GetItemInfo(int nRow, int nItem)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return NULL;
	}

	if((nItem < 0) || (nItem >= m_vecColumnInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	if(nItem >= rowInfo.vecItemInfo.size())
	{
		return NULL;
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);

	return &itemInfo;
}

// ����ĳһ���е���ɫ
void CDuiGridCtrl::SetRowColor(int nRow, Color clrText)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowColor = TRUE;
	rowInfo.clrText = clrText;
}

// ����ĳһ���еļ���״̬
void CDuiGridCtrl::SetRowCheck(int nRow, int nCheck)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.nCheck = nCheck;
}

// ��ȡĳһ�еļ���״̬
int CDuiGridCtrl::GetRowCheck(int nRow)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return -1;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return rowInfo.nCheck;
}

// ����б�
void CDuiGridCtrl::ClearItems()
{
	m_vecRowInfo.clear();
	m_pControScrollV->SetVisible(FALSE);
	UpdateControl(true);
}

// ��XML����Font-title����
HRESULT CDuiGridCtrl::OnAttributeFontTitle(const CStringA& strValue, BOOL bLoading)
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

void CDuiGridCtrl::SetControlRect(CRect rc)
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
				rcTemp.top += m_nHeaderHeight;
				rcTemp.left = rcTemp.right - m_nScrollWidth;
			}else
			if(LISTBK_AREA == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.top += m_nHeaderHeight;
				rcTemp.right -= m_nScrollWidth;
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
		GridRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		nYPos += m_nRowHeight;
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > (m_rc.Height() - m_nHeaderHeight));
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange(m_vecRowInfo.size() * m_nRowHeight);
}

// �ж�ָ��������λ���Ƿ���ĳһ����
BOOL CDuiGridCtrl::PtInRow(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcRow;
	// rcRow�����ǲ����нڵ�ʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�еļ�����
BOOL CDuiGridCtrl::PtInRowCheck(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcCheck;
	// rcCheck�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�е�ĳ������,����������
int CDuiGridCtrl::PtInRowItem(CPoint point, GridRowInfo& rowInfo)
{
	for(size_t i = 0; i < rowInfo.vecItemInfo.size(); i++)
	{
		GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(i);
		CRect rc = itemInfo.rcItem;
		// rcItem�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
		rc.OffsetRect(m_rc.left + m_nLeftPos, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
		if(rc.PtInRect(point))
		{
			return i;
		}
	}

	return -1;
}

// ���õ�Ԫ���Tooltip
void CDuiGridCtrl::SetGridTooltip(int nRow, int nItem, CString strTooltip)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return;
	}

	CDlgBase* pDlg = GetParentDialog();
	if(pDlg && ((m_nTipRow != nRow) || (m_nTipItem != nItem) || (m_nTipVirtualTop != m_nVirtualTop)))
	{
		GridItemInfo* pGridInfo = GetItemInfo(nRow, nItem);
		if(pGridInfo && (pGridInfo->bNeedTitleTip || pGridInfo->bNeedContentTip))
		{
			CRect rc = pGridInfo->rcItem;
			rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop+m_nHeaderHeight);
			pDlg->SetTooltip(this, strTooltip, rc, TRUE);
		}else
		{
			pDlg->ClearTooltip();
		}
		m_nTipRow = nRow;
		m_nTipItem = nItem;
		m_nTipVirtualTop = m_nVirtualTop;
	}
}

// ���Tooltip
void CDuiGridCtrl::ClearGridTooltip()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		m_nTipRow = -1;
		m_nTipItem = -1;
		m_nTipVirtualTop = 0;
	}
}

BOOL CDuiGridCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	int nOldHoverRow = m_nHoverRow;
	BOOL bHoverItemChange = FALSE;
	int nOldHoverItem = -1;

	if(m_rc.PtInRect(point))
	{
		if((m_nHoverRow >= 0) && (m_nHoverRow < m_vecRowInfo.size()))
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
			nOldHoverItem = rowInfo.nHoverItem;
			if(PtInRow(point, rowInfo))
			{
				rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
				if(nOldHoverItem != rowInfo.nHoverItem)
				{
					bHoverItemChange = TRUE;
					UpdateControl(TRUE);
				}

				if(m_bGridTooltip)	// ���õ�Ԫ��Tooltip
				{
					GridItemInfo* pGridInfo = GetItemInfo(m_nHoverRow, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(m_nHoverRow, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(m_nHoverRow, rowInfo.nHoverItem, pGridInfo->strContent);
					}else
					{
						ClearGridTooltip();
					}
				}

				return false;
			}
			m_nHoverRow = -1;		
		}

		BOOL bMousenDown = false;
		if((m_nDownRow >= 0) && (m_nDownRow < m_vecRowInfo.size()))
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
			nOldHoverItem = rowInfo.nHoverItem;
			if(PtInRow(point, rowInfo))
			{
				rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
				bMousenDown = true;
				m_nHoverRow = -1;
				if(nOldHoverItem != rowInfo.nHoverItem)
				{
					bHoverItemChange = TRUE;
				}

				if(m_bGridTooltip)	// ���õ�Ԫ��Tooltip
				{
					GridItemInfo* pGridInfo = GetItemInfo(m_nDownRow, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(m_nDownRow, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(m_nDownRow, rowInfo.nHoverItem, pGridInfo->strContent);
					}else
					{
						ClearGridTooltip();
					}
				}
			}		
		}

		if(!bMousenDown)
		{
			for(size_t i = 0; i < m_vecRowInfo.size(); i++)
			{
				GridRowInfo &rowInfo = m_vecRowInfo.at(i);
				nOldHoverItem = rowInfo.nHoverItem;
				if(PtInRow(point, rowInfo))
				{
					rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
					m_nHoverRow = i;
					if(nOldHoverItem != rowInfo.nHoverItem)
					{
						bHoverItemChange = TRUE;
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

	if((nOldHoverRow != m_nHoverRow) || bHoverItemChange)
	{
		UpdateControl(TRUE);
		return true;
	}
	
	return false;
}

BOOL CDuiGridCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// ���ô��ڽ���,��������޷����й����¼��Ĵ���
	SetWindowFocus();

	if((m_nHoverRow >= 0) && (m_nHoverRow < m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				if(m_bEnableDownRow)
				{
					m_nDownRow = m_nHoverRow;
					m_nHoverRow = -1;
				}

				SendMessage(BUTTOM_DOWN, m_bEnableDownRow ? m_nDownRow : m_nHoverRow, rowInfo.nHoverItem);

				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(BUTTOM_DOWN, m_nDownRow, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

BOOL CDuiGridCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(BUTTOM_UP, m_nHoverRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
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
BOOL CDuiGridCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
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
LRESULT CDuiGridCtrl::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == SCROLL_V) && (Msg == MSG_SCROLL_CHANGE))
	{
		// ����ǹ�������λ�ñ���¼�,��ˢ����ʾ
		UpdateControl(true);
	}else
	if((uID == GetID()) && (Msg == BUTTOM_DOWN) && (lParam != -1))
	{
		// ������е�ĳ������
		GridRowInfo* pRowInfo = GetRowInfo(wParam);
		if(pRowInfo && (lParam >= 0) && (lParam < pRowInfo->vecItemInfo.size()))
		{
			GridItemInfo &itemInfo = pRowInfo->vecItemInfo.at(lParam);
			// ת��ΪBUTTOM_UP��Ϣ,��ΪDuiSystem������ʱ��ֻ����BUTTOM_UP��Ϣ
			if(!itemInfo.strLinkAction.IsEmpty())
			{
				DuiSystem::AddDuiActionTask(uID, BUTTOM_UP, wParam, lParam, GetName(), itemInfo.strLinkAction, GetParent());
			}
		}
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

void CDuiGridCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	// �б�ͼ����:
	// 1.�б������߶�Ϊÿһ�и߶�*����
	// 2.�б���ʾ��top������scroll�ؼ���¼
	// 3.�ػ�ʱ��,����top����λ�ü������ʾ�ĵ�һ�е����,������ʾ�߶ȼ������ʾ�����һ�е����
	// 4.���ݼ��������ʾ����,����Ӧ�����ݵ��ڴ�dc��
	// 5.�������ʾ��top��������ڴ�dc�Ŀ���
	int nWidth = m_rc.Width() - m_nScrollWidth;	// ��ȥ�������Ŀ��
	int nHeightAll = m_vecRowInfo.size()*m_nRowHeight; // �ܵ�����߶� //m_rc.Height();
	CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRange = pScrollV->GetScrollMaxRange();

	m_nVirtualTop = (nMaxRange > 0) ? nCurPos*(nHeightAll-m_rc.Height())/nMaxRange : 0;	// ��ǰ������λ�ö�Ӧ�������topλ��
	m_nFirstViewRow = m_nVirtualTop / m_nRowHeight;					// ��ʾ�ĵ�һ�����
	m_nLastViewRow = (m_nVirtualTop + m_rc.Height() - m_nHeaderHeight) / m_nRowHeight;	// ��ʾ�����һ�����
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
		
		m_memDC.BitBlt(0, 0, nWidth, nHeightView, &dc, m_rc.left, m_rc.top, WHITENESS);	// ����ɫ����
		DrawVerticalTransition(m_memDC, dc, CRect(0, nYViewPos, nWidth, m_rc.Height()+nYViewPos-m_nHeaderHeight),	// ����͸����
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

		// �����ֶβ����м����
		StringFormat strFormatHeader;
		strFormatHeader.SetAlignment(StringAlignmentCenter);	// �м����
		strFormatHeader.SetLineAlignment(StringAlignmentCenter);	// �м����
		if(!m_bTextWrap)
		{
			strFormatHeader.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
		}

		// ��������
		if((m_nHeaderHeight > 0) && (m_vecColumnInfo.size() > 0))
		{
			// ����Ԫ������
			int nPosItemX = 0;
			for(size_t j = 0; j < m_vecColumnInfo.size(); j++)
			{
				GridColumnInfo &columnInfo = m_vecColumnInfo.at(j);
				int nWidth = columnInfo.nWidth;
				if(j == 0)
				{
					nWidth += m_nLeftPos;
				}
				RectF rect(nPosItemX, 0, nWidth, m_nHeaderHeight-1);

				// ���б���
				CString strTitle = columnInfo.strTitle;
				graphics.DrawString(strTitle.AllocSysString(), (INT)wcslen(strTitle.AllocSysString()),
					&font, rect, &strFormatHeader, &solidBrushT);

				nPosItemX += nWidth;
			}
		}
		
		if(m_vecRowInfo.size() > 0)
		{
			for(size_t i = m_nFirstViewRow; i <= m_nLastViewRow && i < m_vecRowInfo.size(); i++)
			{
				GridRowInfo &rowInfo = m_vecRowInfo.at(i);
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
					graphics.DrawImage(m_pImageCheckBox, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nCheckImgY, m_sizeCheckBox.cx, m_sizeCheckBox.cy),
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
					graphics.DrawImage(rowInfo.pImage, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy),
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
					graphics.DrawImage(m_pImage, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
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
					graphics.DrawImage(rowInfo.pRightImage, Rect(nWidth-rowInfo.sizeRightImage.cx-1, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy),
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
					graphics.DrawImage(m_pImage, Rect(nWidth-m_sizeImage.cx-1, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nRightImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nRightImageWidth = m_sizeImage.cx + 1;
				}

				// ����Ԫ������
				int nPosItemX = (m_nLeftPos != 0) ? m_nLeftPos : nXPos;
				for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
				{
					GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
					BOOL bSingleLine = (itemInfo.strContent.IsEmpty() || !itemInfo.strLink.IsEmpty());
					RectF rect(nPosItemX, m_nHeaderHeight + nVI*m_nRowHeight + 1, itemInfo.rcItem.Width(), bSingleLine ? (m_nRowHeight - 2) : (m_nRowHeight / 2 - 2));

					// ����Ԫ��ͼƬ
					int nItemImageX = 0;
					int nImgY = 3;
					if(itemInfo.pImage != NULL)
					{
						if((itemInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
						}
						// ʹ�õ�Ԫ��ָ����ͼƬ
						graphics.DrawImage(itemInfo.pImage, Rect(nPosItemX+nItemImageX, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
							0, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
						nItemImageX += (itemInfo.sizeImage.cx + 3);
					}else
					if((itemInfo.nImageIndex != -1) && (m_pImage != NULL))
					{
						if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
						}
						// ʹ������ͼƬ
						graphics.DrawImage(m_pImage, Rect(nPosItemX+nItemImageX, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
							itemInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
						nItemImageX += (m_sizeImage.cx + 3);
					}
					rect.Offset(nItemImageX, 0);
					rect.Width -= nItemImageX;

					// ����Ԫ��������������
					SolidBrush solidBrushItem(m_clrText);
					if(m_nHoverRow == i)
					{
						solidBrushItem.SetColor(m_clrTextHover);
					}else
					if(m_nDownRow == i)
					{
						solidBrushItem.SetColor(m_clrTextDown);
					}else
					if(itemInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
					{
						solidBrushItem.SetColor(itemInfo.clrText);
					}
					CString strItemTitle = itemInfo.strTitle;
					// �����Ƿ���Ҫ��ʾtip
					itemInfo.bNeedTitleTip = rect.Width < GetTextBounds(font, strItemTitle).Width;
					itemInfo.bNeedContentTip = rect.Width < GetTextBounds(font, itemInfo.strContent).Width;
					if(!itemInfo.strLink.IsEmpty())
					{
						strItemTitle = itemInfo.strLink;
						if((m_nHoverRow == i) && (rowInfo.nHoverItem == j))
						{
							solidBrushItem.SetColor(m_clrTextHover);
						}else
						{
							solidBrushItem.SetColor((itemInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue()) ? itemInfo.clrText : m_clrText);
						}
					}
					// ����bUseTitleFont�����ñ������廹����ͨ����
					graphics.DrawString(strItemTitle.AllocSysString(), (INT)wcslen(strItemTitle.AllocSysString()),
						itemInfo.bUseTitleFont ? &fontTitle : &font, rect, &strFormat, itemInfo.bUseTitleFont ? &solidBrushT : &solidBrushItem);

					// ����Ԫ������
					if(!bSingleLine)
					{
						rect.Offset(0, m_nRowHeight / 2 + 2);
						rect.Height = m_nRowHeight / 2 - 4;
						graphics.DrawString(itemInfo.strContent.AllocSysString(), (INT)wcslen(itemInfo.strContent.AllocSysString()),
							&font, rect, &strFormat, &solidBrushItem);
					}

					nPosItemX += itemInfo.rcItem.Width();
				}

				// ���ָ���(��������ģʽ)
				if(m_pImageSeperator != NULL)
				{
					// ʹ������ģʽ���Ի�ͼ
					graphics.DrawImage(m_pImageSeperator, RectF(0, m_nHeaderHeight + (nVI+1)*m_nRowHeight, nWidth-2, m_sizeSeperator.cy),
							0, 0, m_sizeSeperator.cx, m_sizeSeperator.cy, UnitPixel);
				}else
				{
					// δָ��ͼƬ,�򻭾���
					graphics.FillRectangle(&solidBrushS, 0, m_nHeaderHeight + (nVI+1)*m_nRowHeight, nWidth-2, 1);
				}
			}
		}
	}

	// ���������DC,ʹ����ķ�ʽ�ϲ�����
	if(m_nHeaderHeight > 0)
	{
		dc.BitBlt(m_rc.left,m_rc.top, nWidth, m_nHeaderHeight, &m_memDC, 0, 0, SRCCOPY);//SRCAND);
	}
	dc.BitBlt(m_rc.left,m_rc.top + m_nHeaderHeight, nWidth, m_rc.Height() - m_nHeaderHeight, &m_memDC, 0, nYViewPos + m_nHeaderHeight, SRCCOPY);//SRCAND);
}