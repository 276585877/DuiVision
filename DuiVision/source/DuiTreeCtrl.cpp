#include "StdAfx.h"
#include "DuiTreeCtrl.h"

#define	SCROLL_V	1	// �������ؼ�ID
#define	LISTBK_AREA	2	// ����Area�ؼ�ID

CDuiTreeCtrl::CDuiTreeCtrl(HWND hWnd, CDuiObject* pDuiObject)
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
	m_nLeftPos = 0;

	m_pImageSeperator = NULL;
	m_sizeSeperator = CSize(0, 0);
	m_pImageCheckBox = NULL;
	m_sizeCheckBox = CSize(0, 0);
	m_pImageCollapse = NULL;
	m_sizeCollapse = CSize(0, 0);
	m_pImageToggle = NULL;
	m_sizeToggle = CSize(0, 0);

	m_nBkTransparent = 30;

	m_nHoverRow = 0;
	m_nDownRow = -1;
	m_bEnableDownRow = FALSE;
	m_bSingleLine = TRUE;
	m_bTextWrap = FALSE;

	m_bGridTooltip = TRUE;
	m_nTipNode = NULL;
	m_nTipItem = -1;
	m_nTipVirtualTop = 0;

	m_nFirstViewRow = 0;
	m_nVirtualTop = 0;
	m_nVisibleRowCount = 0;

	m_nNodeIndex = 1;
}

CDuiTreeCtrl::~CDuiTreeCtrl(void)
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
	if(m_pImageCollapse != NULL)
	{
		delete m_pImageCollapse;
		m_pImageCollapse = NULL;
	}
	if(m_pImageToggle != NULL)
	{
		delete m_pImageToggle;
		m_pImageToggle = NULL;
	}
}

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, CheckBox, 6)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, Collapse, 8)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, Toggle, 6)

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDuiTreeCtrl::Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl)
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

	// �����²��node�ڵ���Ϣ
	LoadNode(NULL, pXmlElem);

	// ˢ��������
	//RefreshNodeRows();

    return TRUE;
}

// ����XML���ڵ�
BOOL CDuiTreeCtrl::LoadNode(HTREEITEM hParentNode, TiXmlElement* pXmlElem)
{
	// �����²��Node�ڵ���Ϣ
	TiXmlElement* pNodeElem = NULL;
	for (pNodeElem = pXmlElem->FirstChildElement("node"); pNodeElem != NULL; pNodeElem=pNodeElem->NextSiblingElement("node"))
	{
		CStringA strIdA = pNodeElem->Attribute("id");
		CStringA strCheckA = pNodeElem->Attribute("check");
		CStringA strImageA = pNodeElem->Attribute("image");
		CStringA strRightImageA = pNodeElem->Attribute("right-img");
		CStringA strClrTextA = pNodeElem->Attribute("crtext");
		CStringA strCollapseA = pNodeElem->Attribute("collapse");

		int nCheck = -1;
		if(!strCheckA.IsEmpty())
		{
			nCheck = atoi(strCheckA);
		}

		BOOL bCollapse = (strCollapseA == "1");

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
			strImage = CA2T(strSkin, CP_UTF8);
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
			strRightImage = CA2T(strRightSkin, CP_UTF8);
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

		TreeNodeInfo nodeInfo;
		nodeInfo.hParentNode = hParentNode;
		nodeInfo.strId = strId;
		nodeInfo.nCheck = nCheck;
		nodeInfo.nImageIndex = nImageIndex;
		nodeInfo.pImage = NULL;
		nodeInfo.sizeImage.SetSize(0, 0);
		nodeInfo.nRightImageIndex = nRightImageIndex;
		nodeInfo.pRightImage = NULL;
		nodeInfo.sizeRightImage.SetSize(0, 0);
		nodeInfo.bRowColor = FALSE;
		nodeInfo.clrText = clrText;
		nodeInfo.nHoverItem = -1;
		nodeInfo.bCollapse = bCollapse;
		nodeInfo.bHide = FALSE;
		HTREEITEM hNode = InsertNode(hParentNode, nodeInfo);
		if(hNode == NULL)
		{
			continue;
		}

		int nItemIndex = 0;
		// �����²��item�ڵ���Ϣ
		TiXmlElement* pItemElem = NULL;
		for (pItemElem = pNodeElem->FirstChildElement("item"); pItemElem != NULL; pItemElem=pItemElem->NextSiblingElement("item"))
		{
			CStringA strTitleA = pItemElem->Attribute("title");
			CStringA strContentA = pItemElem->Attribute("content");
			CStringA strClrTextA = pItemElem->Attribute("crtext");
			CStringA strImageA = pItemElem->Attribute("image");
			CStringA strImageCountA = pItemElem->Attribute("img-count");
			CStringA strLinkA = pItemElem->Attribute("link");
			CStringA strLinkActionA = pItemElem->Attribute("linkaction");
			CStringA strFontTitleA = pItemElem->Attribute("font-title");
			CStringA strShowCollapseA = pItemElem->Attribute("collapse");
			DuiSystem::Instance()->ParseDuiString(strTitleA);
			DuiSystem::Instance()->ParseDuiString(strContentA);
			DuiSystem::Instance()->ParseDuiString(strLinkA);
			DuiSystem::Instance()->ParseDuiString(strLinkActionA);
			CString strTitle = CA2T(strTitleA, CP_UTF8);
			CString strContent = CA2T(strContentA, CP_UTF8);
			CString strLink = CA2T(strLinkA, CP_UTF8);
			CString strLinkAction = CA2T(strLinkActionA, CP_UTF8);
			BOOL bShowCollapse = (strShowCollapseA == "1");
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
				strImage = CA2T(strSkin, CP_UTF8);
			}else
			if(!strSkin.IsEmpty())
			{
				// ͼƬ����
				nImageIndex = atoi(strSkin);
			}
			int nImageCount = atoi(strImageCountA);

			BOOL bUseTitleFont = (strFontTitleA == "1");

			if(bShowCollapse)
			{
				// ��������ͼƬ��Ԫ��
				SetSubItemCollapse(hNode, nItemIndex, strImage, nImageCount);
			}else
			if(!strLink.IsEmpty())
			{
				// �������ӵ�Ԫ��
				SetSubItemLink(hNode, nItemIndex, strLink, strLinkAction, nImageIndex, clrText, strImage);
			}else
			{
				// ��ͨ��Ԫ��
				SetSubItem(hNode, nItemIndex, strTitle, strContent, bUseTitleFont, nImageIndex, clrText, strImage);
			}
			nItemIndex++;
		}

		// �����ӽڵ�
		LoadNode(hNode, pNodeElem);
	}

	return TRUE;
}

// �����
BOOL CDuiTreeCtrl::InsertColumn(int nColumn, CString strTitle, int nWidth, Color clrText)
{
	TreeColumnInfo columnInfo;
	columnInfo.strTitle = strTitle;
	columnInfo.clrText = clrText;
	columnInfo.nWidth = nWidth;
	if(nColumn <= -1 || nColumn >= (int)m_vecColumnInfo.size())
	{
		m_vecColumnInfo.push_back(columnInfo);
	}
	else
	{
		m_vecColumnInfo.insert(m_vecColumnInfo.begin() + nColumn, columnInfo);
	}

	int nXPos = 0;
	int nYPos = 0;

	for(size_t i = 0; i < (int)m_vecColumnInfo.size(); i++)
	{
		TreeColumnInfo &columnInfoTemp = m_vecColumnInfo.at(i);
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

// ������ڵ�
HTREEITEM CDuiTreeCtrl::InsertNode(HTREEITEM hParentNode, CString strId, CString strTitle, BOOL bCollapse,
							int nImageIndex, Color clrText, CString strImage,
							int nRightImageIndex, CString strRightImage, int nCheck)
{
	TreeNodeInfo nodeInfo;
	nodeInfo.hParentNode = hParentNode;
	nodeInfo.strId = strId;
	nodeInfo.nCheck = nCheck;
	nodeInfo.nImageIndex = nImageIndex;
	nodeInfo.sizeImage.SetSize(0, 0);
	nodeInfo.nRightImageIndex = nRightImageIndex;
	nodeInfo.sizeRightImage.SetSize(0, 0);
	nodeInfo.bRowColor = FALSE;
	nodeInfo.clrText = clrText;
	nodeInfo.nHoverItem = -1;
	nodeInfo.bCollapse = bCollapse;
	nodeInfo.bHide = FALSE;
	if(clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
	{
		nodeInfo.bRowColor = TRUE;
	}

	// ���ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, nodeInfo.pImage))
		{
			nodeInfo.sizeImage.SetSize(nodeInfo.pImage->GetWidth() / 1, nodeInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		nodeInfo.pImage = NULL;
		if((nodeInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			nodeInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	// �ұ�ͼƬ
	if(!strRightImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strRightImage, m_bImageUseECM, nodeInfo.pRightImage))
		{
			nodeInfo.sizeRightImage.SetSize(nodeInfo.pRightImage->GetWidth() / 1, nodeInfo.pRightImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		nodeInfo.pRightImage = NULL;
		if((nodeInfo.nRightImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			nodeInfo.sizeRightImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	HTREEITEM hNode = InsertNode(hParentNode, nodeInfo);

	if(!strTitle.IsEmpty() && (hNode != NULL))
	{
		SetSubItem(hNode, 0, strTitle);
	}

	return hNode;
}

// ������ڵ�
HTREEITEM CDuiTreeCtrl::InsertNode(HTREEITEM hParentNode, TreeNodeInfo &nodeInfo)
{
	nodeInfo.hNode = m_nNodeIndex++;

	int nParentRow = GetNodeRow(hParentNode);
	int nLastChildRow = GetNodeLastChildRow(hParentNode);

	TreeNodeInfo* pNodeInfo = NULL;
	if(nLastChildRow != -1)
	{
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nLastChildRow + 1, nodeInfo);
		pNodeInfo = &(m_vecRowInfo.at(nLastChildRow + 1));
	}else
	if(nParentRow != -1)
	{
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nParentRow + 1, nodeInfo);
		pNodeInfo = &(m_vecRowInfo.at(nParentRow + 1));
	}else
	{
		m_vecRowInfo.push_back(nodeInfo);
		pNodeInfo = &(m_vecRowInfo.at(m_vecRowInfo.size() - 1));
	}

	if(pNodeInfo == NULL)
	{
		return NULL;
	}

	// ˢ�������е�λ��
	RefreshNodeRows();

	return pNodeInfo->hNode;
}

// ���ñ��������(���ֱ����)
BOOL CDuiTreeCtrl::SetSubItem(HTREEITEM hNode, int nItem, CString strTitle, CString strContent, BOOL bUseTitleFont, int nImageIndex, Color clrText, CString strImage)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		TreeItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.nImageCount = 0;
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = bUseTitleFont;
		itemInfo.bShowCollapse = FALSE;
		TreeColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strTitle = strTitle;
	itemInfo.strContent = strContent;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	TreeColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
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
BOOL CDuiTreeCtrl::SetSubItemLink(HTREEITEM hNode, int nItem, CString strLink, CString strLinkAction, int nImageIndex, Color clrText, CString strImage)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		TreeItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.nImageCount = 0;
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = FALSE;
		itemInfo.bShowCollapse = FALSE;
		TreeColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strLink = strLink;
	itemInfo.strLinkAction = strLinkAction;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	TreeColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
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

// ���ñ����Ϊ����ͼƬ��ʾ
BOOL CDuiTreeCtrl::SetSubItemCollapse(HTREEITEM hNode, int nItem, CString strImage, int nImageCount)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		TreeItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.nImageCount = 0;
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = FALSE;
		itemInfo.bShowCollapse = FALSE;
		TreeColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.bShowCollapse = TRUE;
	itemInfo.nImageCount = nImageCount;

	TreeColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// ����ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
		{
			if(itemInfo.nImageCount == 0)
			{
				itemInfo.nImageCount = 6;
			}
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / itemInfo.nImageCount, itemInfo.pImage->GetHeight());
		}
	}

	return TRUE;
}

// ɾ���ڵ�
BOOL CDuiTreeCtrl::DeleteNode(HTREEITEM hNode)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}

	int nIndex = 0;
	vector<TreeNodeInfo>::iterator it;
	for(it=m_vecRowInfo.begin();it!=m_vecRowInfo.end();++it)
	{
		if(nIndex == nRow)
		{
			m_vecRowInfo.erase(it);
			break;
		}
		nIndex++;
	}

	UpdateControl(true);
	return true;
}

// ��ȡ�ڵ���к�
int CDuiTreeCtrl::GetNodeRow(HTREEITEM hNode)
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hNode == hNode)
		{
			return i;
		}
	}

	return -1;
}

// ��ȡ�ڵ�����һ���ӽڵ��к�(��������ײ���ӽڵ�)
int CDuiTreeCtrl::GetNodeLastChildRow(HTREEITEM hNode)
{
	int nRow = -1;
	HTREEITEM hLastChildNode = NULL;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hParentNode == hNode)
		{
			hLastChildNode = rowInfoTemp.hNode;
			nRow = i;
		}
	}

	if(hLastChildNode != NULL)
	{
		int nLastChildRow = GetNodeLastChildRow(hLastChildNode);
		if(nLastChildRow != -1)
		{
			return nLastChildRow;
		}
	}

	return nRow;
}

// �ж�һ���ڵ��Ƿ����ӽڵ�
BOOL CDuiTreeCtrl::HaveChildNode(HTREEITEM hNode)
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hParentNode == hNode)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// ��ȡ��һ���ӽڵ���
HTREEITEM CDuiTreeCtrl::GetChildNode(HTREEITEM hNode)
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hParentNode == hNode)
		{
			return rowInfoTemp.hNode;
		}
	}

	return NULL;
}

// ��ȡ��һ���ӽڵ���
HTREEITEM CDuiTreeCtrl::GetNextSiblingNode(HTREEITEM hNode)
{
	BOOL bFind = FALSE;
	HTREEITEM hParentNode = NULL;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hNode == hNode)
		{
			hParentNode = rowInfoTemp.hParentNode;
			bFind = TRUE;
		}
		if(bFind && (rowInfoTemp.hParentNode == hParentNode))
		{
			return rowInfoTemp.hNode;
		}
	}

	return NULL;
}

// ��ȡǰһ���ӽڵ���
HTREEITEM CDuiTreeCtrl::GetPrevSiblingNode(HTREEITEM hNode)
{
	BOOL bFind = FALSE;
	HTREEITEM hParentNode = NULL;
	for(size_t i = m_vecRowInfo.size()-1; i >= 0; i--)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hNode == hNode)
		{
			hParentNode = rowInfoTemp.hParentNode;
			bFind = TRUE;
		}
		if(bFind && (rowInfoTemp.hParentNode == hParentNode))
		{
			return rowInfoTemp.hNode;
		}
	}

	return NULL;
}

// ��ȡһ���ڵ�Ĳ㼶
int CDuiTreeCtrl::GetNodeLevel(HTREEITEM hNode)
{
	int nLevel = 0;
	TreeNodeInfo* pNodeInfo = GetNodeInfo(hNode);
	while(pNodeInfo != NULL)
	{
		if(pNodeInfo->hParentNode == NULL)
		{
			break;
		}
		nLevel++;
		pNodeInfo = GetNodeInfo(pNodeInfo->hParentNode);
	}

	return nLevel;
}

// ���ݽڵ�ID��ȡ�ڵ�ľ��
HTREEITEM CDuiTreeCtrl::GetNodeWithId(CString strId)
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.strId == strId)
		{
			return rowInfoTemp.hNode;
		}
	}

	return NULL;
}

// ��ȡĳһ������Ϣ
TreeNodeInfo* CDuiTreeCtrl::GetNodeInfo(HTREEITEM hNode)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return NULL;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	return &rowInfo;
}

// ��ȡĳһ����Ԫ����Ϣ
TreeItemInfo* CDuiTreeCtrl::GetItemInfo(HTREEITEM hNode, int nItem)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return NULL;
	}

	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return NULL;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	if(nItem >= (int)rowInfo.vecItemInfo.size())
	{
		return NULL;
	}

	TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);

	return &itemInfo;
}

// ����ĳһ����Ԫ����Ϣ
void CDuiTreeCtrl::SetItemInfo(HTREEITEM hNode, int nItem, TreeItemInfo* pItemInfo)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return;
	}

	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		TreeItemInfo itemInfo;
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
		itemInfo.bShowCollapse = FALSE;
		TreeColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strTitle = pItemInfo->strTitle;
	itemInfo.strContent = pItemInfo->strContent;
	itemInfo.nImageIndex = pItemInfo->nImageIndex;
	itemInfo.pImage = pItemInfo->pImage;
	itemInfo.sizeImage = pItemInfo->sizeImage;
	itemInfo.clrText = pItemInfo->clrText;
	itemInfo.strLink = pItemInfo->strLink;
	itemInfo.strLinkAction = pItemInfo->strLinkAction;
	itemInfo.bNeedTitleTip = pItemInfo->bNeedTitleTip;
	itemInfo.bNeedContentTip = pItemInfo->bNeedContentTip;
	itemInfo.bUseTitleFont = pItemInfo->bUseTitleFont;
	itemInfo.bShowCollapse = pItemInfo->bShowCollapse;

	TreeColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
}

// ����ĳһ���е���ɫ
void CDuiTreeCtrl::SetNodeColor(HTREEITEM hNode, Color clrText)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowColor = TRUE;
	rowInfo.clrText = clrText;
}

// �л��ڵ������״̬
void CDuiTreeCtrl::ToggleNode(HTREEITEM hNode)
{
	if(HaveChildNode(hNode))
	{
		TreeNodeInfo* pNodeInfo = GetNodeInfo(hNode);
		if(pNodeInfo != NULL)
		{
			pNodeInfo->bCollapse = !pNodeInfo->bCollapse;
			RefreshNodeRows();
		}
	}
}

// ����ĳһ���ڵ�ļ���״̬
void CDuiTreeCtrl::SetNodeCheck(HTREEITEM hNode, int nCheck)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.nCheck = nCheck;
}

// ��ȡĳһ���ڵ�ļ���״̬
int CDuiTreeCtrl::GetNodeCheck(HTREEITEM hNode)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return -1;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	return rowInfo.nCheck;
}

// ������ڵ�
void CDuiTreeCtrl::ClearNodes()
{
	m_vecRowInfo.clear();
	m_pControScrollV->SetVisible(FALSE);
	UpdateControl(true);
}

// �����ӽڵ�
void CDuiTreeCtrl::HideChildNodes(HTREEITEM hItem)
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hParentNode == hItem)
		{
			rowInfoTemp.bHide = TRUE;
			HideChildNodes(rowInfoTemp.hNode);
		}
	}
}

// ˢ�����нڵ���
void CDuiTreeCtrl::RefreshNodeRows()
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		rowInfoTemp.bHide = FALSE;
	}

	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.bCollapse)
		{
			HideChildNodes(rowInfoTemp.hNode);
		}
	}

	// ���¼��������е�λ��
	int nXPos = 0;
	int nYPos = 0;
	int nVisibleRows = 0;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.bHide)
		{
			continue;
		}

		// ������λ��
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		int nXPos = 0;
		if(m_pImageToggle != NULL)
		{
			int nNodeLevel = GetNodeLevel(rowInfoTemp.hNode);
			nXPos += (nNodeLevel * m_sizeToggle.cx);
			if(HaveChildNode(rowInfoTemp.hNode))
			{
				nXPos += m_sizeToggle.cx;
			}
		}

		// �������λ��
		if((rowInfoTemp.nCheck != -1) && (m_pImageCheckBox != NULL))
		{
			int nCheckImgY = 3;
			if((m_sizeCheckBox.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
			{
				nCheckImgY = (m_nRowHeight - m_sizeCheckBox.cy) / 2 + 1;
			}
			rowInfoTemp.rcCheck.left = nXPos;
			rowInfoTemp.rcCheck.right = rowInfoTemp.rcCheck.left + m_sizeCheckBox.cx;
			rowInfoTemp.rcCheck.top = rowInfoTemp.rcRow.top + nCheckImgY;
			rowInfoTemp.rcCheck.bottom = rowInfoTemp.rcCheck.top + m_sizeCheckBox.cy;
		}

		// ����ÿ����Ԫ��λ��
		for(int j = 0; j < (int)rowInfoTemp.vecItemInfo.size(); j++)
		{
			TreeItemInfo &itemInfo = rowInfoTemp.vecItemInfo.at(j);
			itemInfo.rcItem.top = rowInfoTemp.rcRow.top;
			itemInfo.rcItem.bottom = rowInfoTemp.rcRow.bottom;
			// ��һ����Ԫ�����λ��Ҫ�ų�����ͼƬ�Ŀ��
			if((j == 0) && (m_pImageToggle != NULL) && HaveChildNode(rowInfoTemp.hNode))
			{
				int nNodeLevel = GetNodeLevel(rowInfoTemp.hNode);
				itemInfo.rcItem.left = (nNodeLevel+1) * m_sizeToggle.cx + rowInfoTemp.rcRow.left;
			}
		}

		nYPos += m_nRowHeight;
		nVisibleRows++;
	}
	m_nVisibleRowCount = nVisibleRows;

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible((nVisibleRows * m_nRowHeight) > m_rc.Height());
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange(nVisibleRows * m_nRowHeight);

	UpdateControl(true);
}

// ��XML����Font-title����
HRESULT CDuiTreeCtrl::OnAttributeFontTitle(const CStringA& strValue, BOOL bLoading)
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

void CDuiTreeCtrl::SetControlRect(CRect rc)
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
				rcTemp.left = rcTemp.right - m_nScrollWidth;
			}else
			if(LISTBK_AREA == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.right -= m_nScrollWidth;
			}else
			{
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}

	// ���¼��������е�λ��
	RefreshNodeRows();
}

// �ж�ָ��������λ���Ƿ���ĳһ����
BOOL CDuiTreeCtrl::PtInRow(CPoint point, TreeNodeInfo& rowInfo)
{
	if(rowInfo.bHide)
	{
		return FALSE;
	}

	CRect rc = rowInfo.rcRow;
	// rcRow�����ǲ����нڵ�ʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�еļ�����
BOOL CDuiTreeCtrl::PtInRowCheck(CPoint point, TreeNodeInfo& rowInfo)
{
	if(rowInfo.bHide)
	{
		return FALSE;
	}

	CRect rc = rowInfo.rcCheck;
	// rcCheck�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�е�����ͼƬλ��
BOOL CDuiTreeCtrl::PtInRowCollapse(CPoint point, TreeNodeInfo& rowInfo)
{
	if(rowInfo.bHide)
	{
		return FALSE;
	}

	// �жϽڵ������ͼƬλ��
	if((m_pImageToggle != NULL) && HaveChildNode(rowInfo.hNode))
	{
		int nNodeLevel = GetNodeLevel(rowInfo.hNode);
		CRect rc = rowInfo.rcRow;
		rc.left = nNodeLevel * m_sizeToggle.cx;
		rc.right = rc.left + m_sizeToggle.cx;
		rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
		if(rc.PtInRect(point))
		{
			return TRUE;
		}
	}

	// �ж�����Ƿ�����ʾ����ͼƬ�ĵ�Ԫ����
	for(size_t i = 0; i < rowInfo.vecItemInfo.size(); i++)
	{
		TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(i);
		if(!itemInfo.bShowCollapse)
		{
			continue;
		}

		CRect rc = itemInfo.rcItem;
		// rcItem�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
		rc.OffsetRect(m_rc.left + m_nLeftPos, m_rc.top-m_nVirtualTop);
		if(rc.PtInRect(point))
		{
			return TRUE;
		}
	}

	return FALSE;
}

// �ж�ָ��������λ���Ƿ���ĳһ�е�ĳ������,����������
int CDuiTreeCtrl::PtInRowItem(CPoint point, TreeNodeInfo& rowInfo)
{
	if(rowInfo.bHide)
	{
		return -1;
	}

	for(size_t i = 0; i < rowInfo.vecItemInfo.size(); i++)
	{
		TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(i);
		CRect rc = itemInfo.rcItem;
		// rcItem�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
		rc.OffsetRect(m_rc.left + m_nLeftPos, m_rc.top-m_nVirtualTop);
		if(rc.PtInRect(point))
		{
			return i;
		}
	}

	return -1;
}

// ���õ�Ԫ���Tooltip
void CDuiTreeCtrl::SetGridTooltip(HTREEITEM hNode, int nItem, CString strTooltip)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return;
	}

	CDlgBase* pDlg = GetParentDialog();
	if(pDlg && ((m_nTipNode != hNode) || (m_nTipItem != nItem) || (m_nTipVirtualTop != m_nVirtualTop)))
	{
		TreeItemInfo* pGridInfo = GetItemInfo(hNode, nItem);
		if(pGridInfo && (pGridInfo->bNeedTitleTip || pGridInfo->bNeedContentTip))
		{
			CRect rc = pGridInfo->rcItem;
			rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
			pDlg->SetTooltip(this, strTooltip, rc, TRUE);
		}else
		{
			pDlg->ClearTooltip();
		}
		m_nTipNode = hNode;
		m_nTipItem = nItem;
		m_nTipVirtualTop = m_nVirtualTop;
	}
}

// ���Tooltip
void CDuiTreeCtrl::ClearGridTooltip()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		m_nTipNode = NULL;
		m_nTipItem = -1;
		m_nTipVirtualTop = 0;
	}
}

BOOL CDuiTreeCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
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
		if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
		{
			TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
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
					TreeItemInfo* pGridInfo = GetItemInfo(rowInfo.hNode, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(rowInfo.hNode, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(rowInfo.hNode, rowInfo.nHoverItem, pGridInfo->strContent);
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
		if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
		{
			TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
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
					TreeItemInfo* pGridInfo = GetItemInfo(rowInfo.hNode, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(rowInfo.hNode, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(rowInfo.hNode, rowInfo.nHoverItem, pGridInfo->strContent);
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
				TreeNodeInfo &rowInfo = m_vecRowInfo.at(i);
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

BOOL CDuiTreeCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// ���ô��ڽ���,��������޷����й����¼��Ĵ���
	SetWindowFocus();

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo) && !PtInRowCollapse(point, rowInfo))	// ����������¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				if(m_bEnableDownRow)
				{
					m_nDownRow = m_nHoverRow;
					m_nHoverRow = -1;
				}

				SendMessage(BUTTOM_DOWN, rowInfo.hNode, rowInfo.nHoverItem);

				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(BUTTOM_DOWN, rowInfo.hNode, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

BOOL CDuiTreeCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(BUTTOM_UP, rowInfo.hNode, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}else
			if(PtInRowCollapse(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.bCollapse = !rowInfo.bCollapse;
				RefreshNodeRows();

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(BUTTOM_UP, rowInfo.hNode, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}else
			if(PtInRowCollapse(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.bCollapse = !rowInfo.bCollapse;
				RefreshNodeRows();

				return true;
			}
		}
	}

	return false;
}

// �����¼�����
BOOL CDuiTreeCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if((m_nVisibleRowCount * m_nRowHeight) <= m_rc.Height())
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
LRESULT CDuiTreeCtrl::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == SCROLL_V) && (Msg == MSG_SCROLL_CHANGE))
	{
		// ����ǹ�������λ�ñ���¼�,��ˢ����ʾ
		UpdateControl(true);
	}else
	if((uID == GetID()) && (Msg == BUTTOM_DOWN) && (lParam != -1))
	{
		// ������е�ĳ������
		TreeNodeInfo* pRowInfo = GetNodeInfo(wParam);
		if(pRowInfo && (lParam >= 0) && (lParam < (int)pRowInfo->vecItemInfo.size()))
		{
			TreeItemInfo &itemInfo = pRowInfo->vecItemInfo.at(lParam);
			// ת��ΪBUTTOM_UP��Ϣ,��ΪDuiSystem������ʱ��ֻ����BUTTOM_UP��Ϣ
			if(!itemInfo.strLinkAction.IsEmpty())
			{
				DuiSystem::AddDuiActionTask(uID, BUTTOM_UP, wParam, lParam, GetName(), itemInfo.strLinkAction, GetParent());
			}
		}
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

void CDuiTreeCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	// �б�ͼ����:
	// 1.�б������߶�Ϊÿһ�и߶�*����
	// 2.�б���ʾ��top������scroll�ؼ���¼
	// 3.�ػ�ʱ��,����top����λ�ü������ʾ�ĵ�һ�е����,������ʾ�߶ȼ������ʾ�����һ�е����
	// 4.���ݼ��������ʾ����,����Ӧ�����ݵ��ڴ�dc��
	// 5.�������ʾ��top��������ڴ�dc�Ŀ���
	int nWidth = m_rc.Width() - m_nScrollWidth;	// ��ȥ�������Ŀ��
	int nHeightAll = m_nVisibleRowCount*m_nRowHeight; // �ܵ�����߶� //m_rc.Height();
	CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRange = pScrollV->GetScrollMaxRange();

	m_nVirtualTop = (nMaxRange > 0) ? nCurPos*(nHeightAll-m_rc.Height())/nMaxRange : 0;	// ��ǰ������λ�ö�Ӧ�������topλ��
	if(m_nVirtualTop < 0)
	{
		m_nVirtualTop = 0;
		pScrollV->SetScrollCurrentPos(0);
	}
	m_nFirstViewRow = m_nVirtualTop / m_nRowHeight;			// ��ʾ�ĵ�һ�����
	int nViewRowCount = m_rc.Height() / m_nRowHeight + 1;	// ��ʾ������
	int nHeightView = nViewRowCount * m_nRowHeight;	// ��ʾ�漰��������߶�
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
		DrawVerticalTransition(m_memDC, dc, CRect(0, nYViewPos, nWidth, m_rc.Height()+nYViewPos),	// ����͸����
				m_rc, m_nBkTransparent, m_nBkTransparent);
		
		BSTR bsFontTitle = m_strFontTitle.AllocSysString();
		FontFamily fontFamilyTitle(bsFontTitle);
		Font fontTitle(&fontFamilyTitle, (REAL)m_nFontTitleWidth, m_fontTitleStyle, UnitPixel);
		::SysFreeString(bsFontTitle);

		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

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
		
		if(m_vecRowInfo.size() > 0)
		{
			int nRowIndex = 0;
			int nFirstRowCount = 0;
			for(size_t i = 0; (nRowIndex < nViewRowCount) && (i < m_vecRowInfo.size()); i++)
			{
				TreeNodeInfo &rowInfo = m_vecRowInfo.at(i);
				if(rowInfo.bHide)
				{
					continue;
				}
				if(nFirstRowCount < m_nFirstViewRow)
				{
					nFirstRowCount++;
					continue;
				}

				SolidBrush solidBrushRow(rowInfo.clrText);	// �ж������ɫ

				int nXPos = 0;
				int nVI = nRowIndex;//i - m_nFirstViewRow;
				nRowIndex++;

				// �����ڵ�����ͼƬ(toggle)
				if(m_pImageToggle != NULL)
				{
					int nNodeLevel = GetNodeLevel(rowInfo.hNode);
					nXPos += (nNodeLevel * m_sizeToggle.cx);
					if(HaveChildNode(rowInfo.hNode))
					{
						int nToggleImgY = (m_nRowHeight - m_sizeToggle.cy) / 2;
						int nToggleIndex = (m_nHoverRow == i) ? 1 : 0;
						graphics.DrawImage(m_pImageToggle, Rect(nXPos, nVI*m_nRowHeight + nToggleImgY, m_sizeToggle.cx, m_sizeToggle.cy),
							rowInfo.bCollapse ? nToggleIndex*m_sizeToggle.cx : (3+nToggleIndex)*m_sizeToggle.cx, 0, m_sizeToggle.cx, m_sizeToggle.cy, UnitPixel);
						nXPos += m_sizeToggle.cx;
					}
				}

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

				// ����Ԫ������
				int nPosItemX = (m_nLeftPos != 0) ? m_nLeftPos : nXPos;
				for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
				{
					TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
					BOOL bSingleLine = (itemInfo.strContent.IsEmpty() || !itemInfo.strLink.IsEmpty());
					RectF rect((Gdiplus::REAL)nPosItemX, (Gdiplus::REAL)(nVI*m_nRowHeight + 1), (Gdiplus::REAL)(itemInfo.rcItem.Width()), (Gdiplus::REAL)(bSingleLine ? (m_nRowHeight - 2) : (m_nRowHeight / 2 - 2)));
					if(j == 0)
					{
						rect.Width -= nPosItemX;
					}

					// ����Ԫ��ͼƬ
					int nItemImageX = 0;
					int nImgY = 3;
					if((itemInfo.pImage != NULL) && (itemInfo.nImageCount <= 1))
					{
						if((itemInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
						}
						// ʹ�õ�Ԫ��ָ����ͼƬ
						graphics.DrawImage(itemInfo.pImage, Rect(nPosItemX+nItemImageX, nVI*m_nRowHeight + nImgY, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
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
						graphics.DrawImage(m_pImage, Rect(nPosItemX+nItemImageX, nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
							itemInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
						nItemImageX += (m_sizeImage.cx + 3);
					}
					rect.Offset((Gdiplus::REAL)nItemImageX, 0);
					rect.Width -= (Gdiplus::REAL)nItemImageX;

					// ������ͼƬ
					if(itemInfo.bShowCollapse)
					{
						int nCollapseIndex = (m_nHoverRow == i) ? 1 : 0;
						if(itemInfo.pImage != NULL)
						{
							graphics.DrawImage(itemInfo.pImage, Rect(nPosItemX+nItemImageX, nVI*m_nRowHeight + (m_nRowHeight-itemInfo.sizeImage.cy)/2, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
								rowInfo.bCollapse ? nCollapseIndex*itemInfo.sizeImage.cx : ((itemInfo.nImageCount / 2)+nCollapseIndex)*itemInfo.sizeImage.cx, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
						}else
						if(m_pImageCollapse != NULL)
						{
							graphics.DrawImage(m_pImageCollapse, Rect(nPosItemX+nItemImageX, nVI*m_nRowHeight + (m_nRowHeight-m_sizeCollapse.cy)/2, m_sizeCollapse.cx, m_sizeCollapse.cy),
								rowInfo.bCollapse ? nCollapseIndex*m_sizeCollapse.cx : (4+nCollapseIndex)*m_sizeCollapse.cx, 0, m_sizeCollapse.cx, m_sizeCollapse.cy, UnitPixel);
						}
					}

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
					BSTR bsItemTitle = strItemTitle.AllocSysString();
					graphics.DrawString(bsItemTitle, (INT)wcslen(bsItemTitle),
						itemInfo.bUseTitleFont ? &fontTitle : &font, rect, &strFormat, itemInfo.bUseTitleFont ? &solidBrushT : &solidBrushItem);
					::SysFreeString(bsItemTitle);

					// ����Ԫ������
					if(!bSingleLine)
					{
						rect.Offset(0, (Gdiplus::REAL)(m_nRowHeight / 2 + 2));
						rect.Height = (Gdiplus::REAL)(m_nRowHeight / 2 - 4);
						BSTR bsItemContent = itemInfo.strContent.AllocSysString();
						graphics.DrawString(bsItemContent, (INT)wcslen(bsItemContent), &font, rect, &strFormat, &solidBrushItem);
						::SysFreeString(bsItemContent);
					}

					if(j == 0)
					{
						// Ϊ��ʹ�ڶ��п�ʼ�Ƕ����,���Եڶ��п�ʼλ�ð��յ�һ�еĿ�ȼ���
						nPosItemX = itemInfo.rcItem.right;
					}else
					{
						nPosItemX += itemInfo.rcItem.Width();
					}
				}

				// ���ָ���(��������ģʽ)
				if(m_pImageSeperator != NULL)
				{
					// ʹ������ģʽ���Ի�ͼ
					graphics.DrawImage(m_pImageSeperator, RectF(0, (Gdiplus::REAL)((nVI+1)*m_nRowHeight), (Gdiplus::REAL)(nWidth-2), (Gdiplus::REAL)m_sizeSeperator.cy),
							0, 0, (Gdiplus::REAL)m_sizeSeperator.cx, (Gdiplus::REAL)m_sizeSeperator.cy, UnitPixel);
				}/*else
				{
					// δָ��ͼƬ,�򻭾���
					graphics.FillRectangle(&solidBrushS, 0, (nVI+1)*m_nRowHeight, nWidth-2, 1);
				}*/
			}
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, nWidth, m_rc.Height(), &m_memDC, 0, nYViewPos, SRCCOPY);//SRCAND);
}