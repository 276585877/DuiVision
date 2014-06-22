#include "StdAfx.h"
#include "DuiTab.h"

CDuiTabCtrl::CDuiTabCtrl(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBaseFont(hWnd, pDuiObject)
{
	m_pImageSeperator = NULL;
	m_pImageHover = NULL;
	m_nHoverItem = 0;
	m_nDownItem = 0;
	m_nOldItem = -1;
	m_nTabItemWidth = 0;
	m_nTabCtrlHeight = 0;
	m_clrText = Color(225, 255, 255, 255);
	m_bAnimateChangeTab = FALSE;
	m_nAnimateCount = 10;
	m_nCurXPos = 0;
	m_bInit = FALSE;
}

CDuiTabCtrl::CDuiTabCtrl(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle/*= TEXT("")*/, BOOL bIsVisible/* = TRUE*/, 
						   BOOL bIsDisable/* = FALSE*/ ,BOOL bIsPressDown/* = FALSE*/)
	: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, strTitle, bIsVisible, bIsDisable)
{
	m_pImageSeperator = NULL;
	m_pImageHover = NULL;
	m_nHoverItem = 0;
	m_nDownItem = 0;
	m_nOldItem = -1;
	m_nTabItemWidth = 0;
	m_nTabCtrlHeight = 0;
	m_clrText = Color(225, 255, 255, 255);
	m_bAnimateChangeTab = FALSE;
	m_nAnimateCount = 10;
	m_nCurXPos = 0;
	m_bInit = FALSE;
}

CDuiTabCtrl::~CDuiTabCtrl(void)
{
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}

	if(m_pImageHover != NULL)
	{
		delete m_pImageHover;
		m_pImageHover = NULL;
	}

	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfoTemp = m_vecItemInfo.at(i);
		if(itemInfoTemp.pImage != NULL)
		{
			delete itemInfoTemp.pImage;
			itemInfoTemp.pImage = NULL;
		}
	}
}

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTabCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTabCtrl, Hover, 2)

// ���ݿؼ��������ؼ�ʵ��
CControlBase* CDuiTabCtrl::_CreateControlByName(LPCSTR lpszName)
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

// ���ؼ���XML�ڵ㺯���������²��tabҳ������
BOOL CDuiTabCtrl::Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl)
{
	__super::Load(pXmlElem);

	if(pXmlElem == NULL)
	{
		return FALSE;
	}

	// ���û������tabctrl�߶�,����hoverͼƬ�ĸ߶�
	if((m_pImageHover != NULL) && (m_nTabCtrlHeight == 0))
	{
		m_nTabCtrlHeight = m_sizeHover.cy;
	}

	// ���û������tab��Ŀ��,����hoverͼƬ�Ŀ��
	if((m_pImageHover != NULL) && (m_nTabItemWidth == 0))
	{
		m_nTabItemWidth = m_sizeHover.cx;
	}

	BOOL bAllVisible = TRUE;
	
	// �����²��tabҳ�ڵ���Ϣ
	int nIdIndex = m_vecItemInfo.size();
	TiXmlElement* pTabElem = NULL;
	for (pTabElem = pXmlElem->FirstChildElement("tab"); pTabElem != NULL; pTabElem=pTabElem->NextSiblingElement())
	{
		CStringA strId = pTabElem->Attribute("id");
		int nId = nIdIndex;
		if(strId != "")
		{
			nId = atoi(strId);
		}

		CStringA strNameA = pTabElem->Attribute("name");
		CString strName = CEncodingUtil::AnsiToUnicode(strNameA);
		if(GetItemIndex(strName) != -1)
		{
			// ����Ѿ�������ͬ���ֵ�tabҳ,������
			continue;
		}

		CStringA strTabXmlA = pTabElem->Attribute("tabxml");
		CString strTabXml = CEncodingUtil::AnsiToUnicode(strTabXmlA);
		if(!strTabXmlA.IsEmpty())
		{
			// ��xml�ļ�����Ƕ�׵�tabҳ
			LoadTabXml(strTabXml);
			nIdIndex = m_vecItemInfo.size();
			continue;
		}

		CStringA strActionA = pTabElem->Attribute("action");
		CString strAction = CEncodingUtil::AnsiToUnicode(strActionA);
		CStringA strOutLinkA = pTabElem->Attribute("outlink");
		BOOL bOutLink = ((strOutLinkA == "1") || (strOutLinkA == "true"));
		CStringA strImageA = pTabElem->Attribute("image");
		CStringA strImageIndex = pTabElem->Attribute("img-index");
		int nImageIndex = -1;
		if(!strImageIndex.IsEmpty())
		{
			nImageIndex = atoi(strImageIndex);
		}
		CStringA strImageCount = pTabElem->Attribute("img-count");
		int nImageCount = -1;
		if(!strImageCount.IsEmpty())
		{
			nImageCount = atoi(strImageCount);
		}
		// visible���Կ�����visible��show
		CStringA strVisibleA = pTabElem->Attribute("visible");
		if(strVisibleA.IsEmpty())
		{
			strVisibleA = pTabElem->Attribute("show");
		}
		BOOL bVisible = ((strVisibleA == "1") || (strVisibleA == "true") || (strVisibleA == ""));
		CStringA strActive = pTabElem->Attribute("active");
		CStringA strDivXml = pTabElem->Attribute("div");

		CStringA strScrollA = pTabElem->Attribute("scroll");
		BOOL bEnableScroll = (strScrollA == "1");

		// ����Panel�ؼ���ÿ��Tabҳ�����Զ�����һ��Panel�ؼ�����ʹû�м�����XML�ڵ�
		CDuiPanel* pControlPanel = (CDuiPanel*)_CreateControlByName("div");
		pControlPanel->SetName(CEncodingUtil::AnsiToUnicode(strNameA));	// div�ؼ�����������Ϊtab������
		pControlPanel->SetEnableScroll(bEnableScroll);
		m_vecControl.push_back(pControlPanel);
		if(!strDivXml.IsEmpty())
		{
 			pControlPanel->LoadXmlFile(CEncodingUtil::AnsiToUnicode(strDivXml));			
		}

		// ����XML��Tab�ڵ�ĸ����²�ؼ��ڵ�
		pControlPanel->Load(pTabElem);

		CString strTitle = pControlPanel->GetTitle();
		
		// ͨ��Skin��ȡ
		CStringA strSkinA = "";
		if(strImageA.Find("skin:") == 0)
		{
			strSkinA = DuiSystem::Instance()->GetSkin(strImageA);
		}else
		{
			strSkinA = strImageA;
		}

		if(strSkinA.Find(".") != -1)	// ����ͼƬ�ļ�
		{
			CString strImgFile = DuiSystem::GetSkinPath() + CA2T(strSkinA, CP_UTF8);
			if(strSkinA.Find(":") != -1)
			{
				strImgFile = CA2T(strSkinA, CP_UTF8);
			}
			InsertItem(-1, nId, strName, strTitle, strAction, strImgFile, pControlPanel, nImageCount, bOutLink);
		}else
		if(nImageIndex != -1)	// ����ͼƬ
		{
			InsertItem(-1, nId, strName, strTitle, strAction, nImageIndex, pControlPanel, bOutLink);
		}else
		if(!strSkinA.IsEmpty())	// ͼƬ��Դ
		{
			UINT uResourceID = atoi(strSkinA);
			InsertItem(-1, nId, strName, strTitle, strAction, uResourceID, pControlPanel, nImageCount, bOutLink);
		}else
		if(strSkinA.IsEmpty())	// ͼƬΪ��
		{
			InsertItem(-1, nId, strName, strTitle, strAction, _T(""), pControlPanel, nImageCount, bOutLink);
		}

		TabItemInfo &itemInfo = m_vecItemInfo.at(nIdIndex);
		itemInfo.bVisible = bVisible;
		if(!bVisible)
		{
			bAllVisible = FALSE;
		}

		if(strActive == "true")	// ����Ϊ��ǰ���ҳ��
		{
			m_nHoverItem = nIdIndex;
			m_nDownItem = nIdIndex;
		}
		if(!bVisible && (m_nDownItem == nIdIndex))
		{
			m_nDownItem++;
			m_nHoverItem = m_nDownItem;
		}

		nIdIndex++;
	}

	// ��������ڼ���Ƕ��tabҳ����,����Ҫ���������
	if(!bLoadSubControl)
	{
		return TRUE;
	}

	// ֻ��ʾ��ǰ���tabҳ��Ӧ��Panel��������ҳ���Panel��������
	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(i);
		if(itemInfo.pControl != NULL)
		{
			if(i == m_nDownItem)
			{
				itemInfo.pControl->SetVisible(TRUE);
			}else
			{
				itemInfo.pControl->SetVisible(FALSE);
			}
		}
	}

	// �����ҳ�������ص�,����Ҫˢ������ҳ��
	if(!bAllVisible)
	{
		RefreshItems();
	}

	m_bInit = TRUE;

    return TRUE;
}

// ��Ƕ�׵�xml�ļ��м���tabҳ
BOOL CDuiTabCtrl::LoadTabXml(CString strFileName)
{
	// ���Ƚ�����tabҳ������
	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(i);
		if(itemInfo.pControl != NULL)
		{
			itemInfo.pControl->SetVisible(FALSE);
		}
	}

	TiXmlDocument xmlDoc;
	TiXmlElement* pTabElem = NULL;

	CString strXmlFile;
	if(strFileName.Find(_T(":")) == -1)
	{
		if(strFileName.Find(_T(".xml")) == -1)
		{
			strXmlFile = DuiSystem::Instance()->GetXmlFile(CEncodingUtil::UnicodeToAnsi(strFileName));
		}else
		{
			strXmlFile = DuiSystem::GetXmlPath() + strFileName;
		}
	}else
	{
		strXmlFile = strFileName;
	}

	xmlDoc.LoadFile(CEncodingUtil::UnicodeToAnsi(strXmlFile), TIXML_ENCODING_UTF8);
	if(!xmlDoc.Error())
	{
		pTabElem = xmlDoc.FirstChildElement(GetClassName());
		if(pTabElem != NULL)
		{
			// �����²�tabҳ
			Load(pTabElem, FALSE);
		}
	}

	// ֻ��ʾ��ǰ���tabҳ��Ӧ��Panel��������ҳ���Panel��������
	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(i);
		if(itemInfo.pControl != NULL)
		{
			if(i == m_nDownItem)
			{
				itemInfo.pControl->SetVisible(TRUE);
			}else
			{
				itemInfo.pControl->SetVisible(FALSE);
			}
		}
	}

	// ���¼���ÿ��tabҳ��λ��,��ˢ�½���
	RefreshItems();

	return TRUE;
}

// ����tabҳ(ʹ����ԴͼƬ)
BOOL CDuiTabCtrl::InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, UINT nResourceID, CControlBase* pControl, int nImageCount, BOOL bOutLink, int nItemWidth/* = 0*/, CString strType/*= TEXT("PNG")*/)
{
	TabItemInfo itemInfo;
	itemInfo.bVisible = TRUE;
	itemInfo.nItemID = nItemID;
	itemInfo.strName = strName;
	itemInfo.strText = strItemText;
	itemInfo.strAction = strAction;
	itemInfo.bOutLink = bOutLink;
	itemInfo.sizeImage.SetSize(0, 0);
	itemInfo.nImageCount = 3;
	if(nImageCount != -1)
	{
		itemInfo.nImageCount = nImageCount;
	}
	itemInfo.nImageIndex = -1;

	if(ImageFromIDResource(nResourceID, strType, itemInfo.pImage))
	{
		itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / itemInfo.nImageCount, itemInfo.pImage->GetHeight());
	}

	itemInfo.rc.SetRect(m_rc.left, m_rc.top, m_rc.left + (nItemWidth == 0 ? itemInfo.sizeImage.cx : nItemWidth), m_rc.bottom);

	itemInfo.pControl = pControl;

	return InsertItem(nItem, itemInfo);
}

// ����tabҳ(ʹ��ͼƬ�ļ�)
BOOL CDuiTabCtrl::InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, CString strImage, CControlBase* pControl, int nImageCount, BOOL bOutLink, int nItemWidth/* = 0*/)
{
	TabItemInfo itemInfo;
	itemInfo.bVisible = TRUE;
	itemInfo.nItemID = nItemID;
	itemInfo.strName = strName;
	itemInfo.strText = strItemText;
	itemInfo.strAction = strAction;
	itemInfo.bOutLink = bOutLink;
	itemInfo.sizeImage.SetSize(0, 0);
	itemInfo.nImageCount = 3;
	if(nImageCount != -1)
	{
		itemInfo.nImageCount = nImageCount;
	}
	itemInfo.nImageIndex = -1;

	if(strImage.IsEmpty())
	{
		itemInfo.pImage = NULL;
	}else
	{
		itemInfo.pImage = Image::FromFile(strImage, m_bImageUseECM);
	}

	if(itemInfo.pImage && (itemInfo.pImage->GetLastStatus() == Ok))
	{
		itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / itemInfo.nImageCount, itemInfo.pImage->GetHeight());
	}

	int nWidth = (nItemWidth == 0 ? itemInfo.sizeImage.cx : nItemWidth);
	if(nWidth == 0)
	{
		nWidth = m_sizeHover.cx;
	}
	itemInfo.rc.SetRect(m_rc.left, m_rc.top, m_rc.left + nWidth, m_rc.bottom);

	itemInfo.pControl = pControl;

	return InsertItem(nItem, itemInfo);
}

// ����tabҳ(����ͼƬ��ʽ)
BOOL CDuiTabCtrl::InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, int nImageIndex, CControlBase* pControl, BOOL bOutLink, int nItemWidth/* = 0*/)
{
	TabItemInfo itemInfo;
	itemInfo.bVisible = TRUE;
	itemInfo.nItemID = nItemID;
	itemInfo.strName = strName;
	itemInfo.strText = strItemText;
	itemInfo.strAction = strAction;
	itemInfo.bOutLink = bOutLink;
	itemInfo.pImage = NULL;
	itemInfo.sizeImage.SetSize(0, 0);
	itemInfo.nImageCount = 1;
	itemInfo.nImageIndex = nImageIndex;

	if((m_pImage != NULL) && (m_nImagePicCount > 0))
	{
		itemInfo.sizeImage.SetSize(m_pImage->GetWidth() / m_nImagePicCount, m_pImage->GetHeight());
	}

	itemInfo.rc.SetRect(m_rc.left, m_rc.top, m_rc.left + (nItemWidth == 0 ? itemInfo.sizeImage.cx : nItemWidth), m_rc.bottom);

	itemInfo.pControl = pControl;

	return InsertItem(nItem, itemInfo);
}

BOOL CDuiTabCtrl::InsertItem(int nItem, TabItemInfo &itemInfo)
{
	if(m_vecItemInfo.size() > 0)
	{
		CRect rc;
		m_vecRcSeperator.push_back(rc);
	}
	if(nItem <= -1 || nItem >= m_vecItemInfo.size())
	{
		m_vecItemInfo.push_back(itemInfo);
	}
	else
	{
		m_vecItemInfo.insert(m_vecItemInfo.begin() + nItem, itemInfo);
	}

	int nXPos = m_rc.left;
	int nYPos = m_rc.top;

	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfoTemp = m_vecItemInfo.at(i);
		int nItemWidth = itemInfoTemp.rc.Width();
		int nItemHeight = itemInfoTemp.sizeImage.cy;

		if(m_nTabItemWidth == 0)
		{
			// ���tabctrlû������ÿ��tabҳ�Ŀ��,���Ե�һ��tabҳ��ͼƬ���Ϊ׼
			m_nTabItemWidth = nItemWidth;
		}
		if(m_nTabCtrlHeight == 0)
		{
			// ���tabctrlû�����ø߶�,���Ե�һ��tabҳ��ͼƬ�߶�Ϊ׼
			m_nTabCtrlHeight = nItemHeight;
		}

		itemInfoTemp.rc.SetRect(nXPos, nYPos, nXPos + m_nTabItemWidth, nYPos + m_nTabCtrlHeight);

		nXPos += m_nTabItemWidth;

		if(i < m_vecItemInfo.size() - 1 && m_pImageSeperator != NULL)
		{
			CRect &rc = m_vecRcSeperator.at(i);
			rc.SetRect(nXPos, nYPos, nXPos + m_sizeSeperator.cx, nYPos + m_sizeSeperator.cy);
			nXPos += m_sizeSeperator.cx;
		}
		/*
		if(itemInfoTemp.pControl != NULL)
		{
			CRect rc = itemInfoTemp.pControl->GetRect();
			rc.top += itemInfoTemp.sizeImage.cy;
			itemInfoTemp.pControl->SetRect(rc);
			//itemInfoTemp.pControl->UpdateControl();
		}*/
	}

	UpdateControl(true);
	return true;
}

// ����tab���ֻ�ȡ����
int CDuiTabCtrl::GetItemIndex(CString strTabName)
{
	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfoTemp = m_vecItemInfo.at(i);
		if(itemInfoTemp.strName == strTabName)
		{
			return i;
		}
	}
	return -1;
}

// ��ȡָ��tabҳ��Ϣ
TabItemInfo* CDuiTabCtrl::GetItemInfo(int nItem)
{
	if((nItem < 0) || (nItem >= m_vecItemInfo.size()))
	{
		return NULL;
	}

	TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);
	return &itemInfo;
}

// ����ѡ���tabҳ
int CDuiTabCtrl::SetSelectItem(int nItem)
{
	int nOldDownItem = m_nDownItem;
	if(m_nDownItem != nItem && nItem >= 0 && nItem < m_vecItemInfo.size())
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);
		if(itemInfo.bOutLink)	// �ⲿ����
		{
			m_nHoverItem = -1;
			SendMessage(BUTTOM_DOWN, nItem, 0);
		}else
		{
			m_nOldItem = m_nDownItem;	// �����л�ǰ��ҳ������,�����л�����
			m_nDownItem = nItem;					
			m_nHoverItem = -1;

			SendMessage(BUTTOM_DOWN, m_nDownItem, 0);

			// ֻ��ʾ��ǰ���tabҳ��Ӧ��Panel��������ҳ���Panel��������
			for(size_t i = 0; i < m_vecItemInfo.size(); i++)
			{
				TabItemInfo &itemInfo = m_vecItemInfo.at(i);
				if(itemInfo.pControl != NULL)
				{
					if(i == m_nDownItem)
					{
						itemInfo.pControl->SetVisible(TRUE);
						SetWindowFocus();
					}else
					{
						itemInfo.pControl->SetVisible(FALSE);
					}
				}
				// ��������˶���,�������л�������ʱ��
				if(m_bAnimateChangeTab)
				{
					m_nCurXPos = 0;
					m_bRunTime = true;
				}
			}
		}

		UpdateControl();
	}

	return nOldDownItem;
}

// ˢ������Tabҳ
void CDuiTabCtrl::RefreshItems()
{
	// ���¼���ÿ��tabҳ��λ��,��ˢ�½���
	int nXPos = m_rc.left;
	int nYPos = m_rc.top;

	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfoTemp = m_vecItemInfo.at(i);
		if(!itemInfoTemp.bVisible)
		{
			// ���ص�tabҳ��������Ϊ0
			itemInfoTemp.rc.SetRect(0,0,0,0);
			if(i < m_vecRcSeperator.size())
			{
				CRect &rc = m_vecRcSeperator.at(i);
				rc.SetRect(0,0,0,0);
			}			
			continue;
		}

		int nItemWidth = itemInfoTemp.rc.Width();
		int nItemHeight = itemInfoTemp.sizeImage.cy;

		if(m_nTabItemWidth == 0)
		{
			// ���tabctrlû������ÿ��tabҳ�Ŀ��,���Ե�һ��tabҳ��ͼƬ���Ϊ׼
			m_nTabItemWidth = nItemWidth;
		}
		if(m_nTabCtrlHeight == 0)
		{
			// ���tabctrlû�����ø߶�,���Ե�һ��tabҳ��ͼƬ�߶�Ϊ׼
			m_nTabCtrlHeight = nItemHeight;
		}

		itemInfoTemp.rc.SetRect(nXPos, nYPos, nXPos + m_nTabItemWidth, nYPos + m_nTabCtrlHeight);

		nXPos += m_nTabItemWidth;

		if(i < m_vecItemInfo.size() - 1 && m_pImageSeperator != NULL)
		{
			CRect &rc = m_vecRcSeperator.at(i);
			rc.SetRect(nXPos, nYPos, nXPos + m_sizeSeperator.cx, nYPos + m_sizeSeperator.cy);
			nXPos += m_sizeSeperator.cx;
		}
	}

	UpdateControl(true);
}

// ɾ��ָ��tabҳ
void CDuiTabCtrl::DeleteItem(int nItem)
{
	if((nItem < 0) || (nItem >= m_vecItemInfo.size()))
	{
		return;
	}

	int nIndex = 0;
	vector<TabItemInfo>::iterator it;
	for(it=m_vecItemInfo.begin();it!=m_vecItemInfo.end();++it)
	{
		if(nIndex == nItem)
		{
			m_vecItemInfo.erase(it);
			break;
		}
		nIndex++;
	}

	// ���¼���ÿ��tabҳ��λ��,��ˢ�½���
	RefreshItems();
}

// ɾ��ָ��tabҳ(��������)
void CDuiTabCtrl::DeleteItem(CString strTabName)
{
	vector<TabItemInfo>::iterator it;
	for(it=m_vecItemInfo.begin();it!=m_vecItemInfo.end();++it)
	{
		TabItemInfo &itemInfo = *it;
		if(itemInfo.strName == strTabName)
		{
			m_vecItemInfo.erase(it);
			break;
		}
	}

	// ���¼���ÿ��tabҳ��λ��,��ˢ�½���
	RefreshItems();
}

// ����tabҳ�Ŀɼ���
void CDuiTabCtrl::SetItemVisible(int nItem, BOOL bVisible)
{
	if((nItem < 0) || (nItem >= m_vecItemInfo.size()))
	{
		return;
	}

	TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);
	itemInfo.bVisible = bVisible;

	// ���¼���ÿ��tabҳ��λ��,��ˢ�½���
	RefreshItems();
}

// ����tabҳ�Ŀɼ���(����tab����)
void CDuiTabCtrl::SetItemVisible(CString strTabName, BOOL bVisible)
{
	int nItem = GetItemIndex(strTabName);
	if(nItem != -1)
	{
		SetItemVisible(nItem, bVisible);
	}
}

// ��ȡtabҳ�Ŀɼ���
BOOL CDuiTabCtrl::GetItemVisible(int nItem)
{
	if((nItem < 0) || (nItem >= m_vecItemInfo.size()))
	{
		return FALSE;
	}

	TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);
	return itemInfo.bVisible;
}

// ��ȡtabҳ�Ŀɼ���
BOOL CDuiTabCtrl::GetItemVisible(CString strTabName)
{
	int nItem = GetItemIndex(strTabName);
	if(nItem != -1)
	{
		return GetItemVisible(nItem);
	}

	return FALSE;
}

// �ж�����Ƿ��ڿؼ�����Ӧ������
BOOL CDuiTabCtrl::OnCheckMouseResponse(UINT nFlags, CPoint point)
{
	CRect rc = m_rc;
	rc.bottom = rc.top + m_nTabCtrlHeight;
	if(!rc.PtInRect(point))
	{
		return true;
	}

	// �ж������겻������tabҳ��Χ��,������Ӧ
	BOOL bPtInTabs = false;
	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(i);
		if(itemInfo.bVisible && itemInfo.rc.PtInRect(point))
		{
			bPtInTabs = true;
			break;
		}
	}
	
	return bPtInTabs;
}

BOOL CDuiTabCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
{
	int nOldHoverItem = m_nHoverItem;

	if(m_rc.PtInRect(point))
	{
		if(m_nHoverItem != -1)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(m_nHoverItem);
			if(itemInfo.rc.PtInRect(point))
			{
				return false;
			}
			m_nHoverItem = -1;		
		}		

		BOOL bMousenDown = false;
		if(m_nDownItem != -1)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);
			if(itemInfo.rc.PtInRect(point))
			{
				bMousenDown = true;
				m_nHoverItem = -1;
			}		
		}

		if(!bMousenDown)
		{
			for(size_t i = 0; i < m_vecItemInfo.size(); i++)
			{
				TabItemInfo &itemInfo = m_vecItemInfo.at(i);
				if(itemInfo.rc.PtInRect(point))
				{
					m_nHoverItem = i;
					break;
				}
			}
		}
	}
	else
	{
		m_nHoverItem = -1;
	}

	if(nOldHoverItem != m_nHoverItem)
	{
		UpdateControl();
		return true;
	}
	
	return false;
}

BOOL CDuiTabCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_nHoverItem != -1)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nHoverItem);
		if(itemInfo.rc.PtInRect(point))
		{
			if(m_nDownItem != m_nHoverItem)
			{
				int nDownItem = m_nHoverItem;
				if(!itemInfo.strAction.IsEmpty())
				{
					// ���action�ǿ�,��ִ�ж���
					DuiSystem::AddDuiActionTask(GetID(), BUTTOM_UP, nDownItem, 0, GetName(), itemInfo.strAction, GetParent());
				}

				if(itemInfo.bOutLink)	// �ⲿ����
				{
					m_nHoverItem = -1;
					SendMessage(BUTTOM_DOWN, nDownItem, 0);
				}else
				{
					m_nOldItem = m_nDownItem;	// �����л�ǰ��ҳ������,�����л�����
					m_nDownItem = m_nHoverItem;					
					m_nHoverItem = -1;

					// ɾ���ɵ�Tooltip
					CDlgBase* pDlg = GetParentDialog();
					if(pDlg)
					{
						pDlg->ClearTooltip();
					}

					// ����¼���Ϣ
					SendMessage(BUTTOM_DOWN, m_nDownItem, 0);

					// ֻ��ʾ��ǰ���tabҳ��Ӧ��Panel��������ҳ���Panel��������
					for(size_t i = 0; i < m_vecItemInfo.size(); i++)
					{
						TabItemInfo &itemInfo = m_vecItemInfo.at(i);
						if(itemInfo.pControl != NULL)
						{
							if(i == m_nDownItem)
							{
								itemInfo.pControl->SetVisible(TRUE);
								SetWindowFocus();
							}else
							{
								itemInfo.pControl->SetVisible(FALSE);
							}
						}
						// ��������˶���,�������л�������ʱ��
						if(m_bAnimateChangeTab)
						{
							m_nCurXPos = 0;
							m_bRunTime = true;
						}
					}
				}

				UpdateControl();

				return true;
			}
		}		
	}	
	
	return false;
}

BOOL CDuiTabCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	return false;
}

// �����¼�����
BOOL CDuiTabCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(m_nDownItem != -1)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);
		if(itemInfo.rc.PtInRect(point))
		{
			if(itemInfo.pControl != NULL)
			{
				return itemInfo.pControl->OnScroll(bVertical, nFlags, point);
			}
		}
	}

	return false;
}

// �����¼�����
BOOL CDuiTabCtrl::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_nDownItem != -1)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);
		if(itemInfo.pControl != NULL)
		{
			return itemInfo.pControl->OnKeyDown(nChar, nRepCnt, nFlags);
		}
	}

	return false;
}

void CDuiTabCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeight * 3);

		Graphics graphics(m_memDC);

		FontFamily fontFamily(m_strFont.AllocSysString());
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);

		SolidBrush solidBrush(m_clrText);			// �������ֻ�ˢ

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// ���ֵĶ��뷽ʽ
		StringFormat strFormat;
		strFormat.SetAlignment(StringAlignmentCenter);		// ˮƽ�����м����
		strFormat.SetLineAlignment(StringAlignmentCenter);	// ��ֱ�����м����
		//strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		
		for(int i = 0; i < 3; i++)
		{
			m_memDC.BitBlt(0, i * nHeight, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

			int nXPos = 0;
			int nYPos = i * nHeight;
			for(size_t j = 0; j < m_vecItemInfo.size(); j++)
			{
				TabItemInfo &itemInfo = m_vecItemInfo.at(j);
				
				if(!itemInfo.bVisible)
				{
					continue;
				}

				if(itemInfo.pImage != NULL)	// ʹ��tabҳָ����ͼƬ
				{
					int nImageIndex = i;
					if(itemInfo.nImageCount == 1)
					{
						nImageIndex = 0;
					}
					int nX = (itemInfo.rc.Width() - itemInfo.sizeImage.cx) / 2;
					graphics.DrawImage(itemInfo.pImage, Rect(nXPos + nX, nYPos,  itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
						itemInfo.sizeImage.cx * nImageIndex, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
				}else
				if((m_pImage != NULL) && (itemInfo.nImageIndex != -1))	// ʹ��tabctrl������ͼƬ
				{
					// ����ͼ
					int nX = (itemInfo.rc.Width() - itemInfo.sizeImage.cx) / 2;
					graphics.DrawImage(m_pImage, Rect(nXPos + nX, nYPos,  itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
						itemInfo.sizeImage.cx * itemInfo.nImageIndex, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
				}

				// ���ȵ�ͼ(�������tabctrl���õ��ȵ�ͼ�Ļ�)
				if((m_pImageHover != NULL) && (i > 0))
				{
					int nX = (itemInfo.rc.Width() - m_sizeHover.cx) / 2;
					if(nX < 0)
					{
						nX = 0;
					}
					graphics.DrawImage(m_pImageHover, Rect(nXPos + nX, nYPos,  m_sizeHover.cx, m_sizeHover.cy),
						m_sizeHover.cx * (i-1), 0, m_sizeHover.cx, m_sizeHover.cy, UnitPixel);
				}

				// ����
				if(!itemInfo.strText.IsEmpty())
				{
					RectF rectText(nXPos, nYPos + itemInfo.sizeImage.cy + 1, itemInfo.rc.Width(), m_nTabCtrlHeight - itemInfo.sizeImage.cy - 1);
					if(m_nTabCtrlHeight <= itemInfo.sizeImage.cy)
					{
						// ���tabctrl�߶�С��ͼƬ�߶�,������ֱ�Ӿ�����ʾ
						rectText.Y = nYPos;
						rectText.Height = m_nTabCtrlHeight;
					}
					graphics.DrawString(itemInfo.strText.AllocSysString(), (INT)wcslen(itemInfo.strText.AllocSysString()), &font, 
							rectText, &strFormat, &solidBrush);
				}

				nXPos += itemInfo.rc.Width();

				// ���ָ�ͼƬ(�������췽ʽ)
				if(j < m_vecItemInfo.size() - 1 && m_pImageSeperator != NULL)
				{
					CRect &rc = m_vecRcSeperator.at(j);
					int nSepHeight = itemInfo.rc.Height();	// m_sizeSeperator.cy
					graphics.DrawImage(m_pImageSeperator, Rect(nXPos, nYPos, m_sizeSeperator.cx, nSepHeight),
						0, 0, m_sizeSeperator.cx, m_sizeSeperator.cy, UnitPixel);

					nXPos += m_sizeSeperator.cx;
				}
			}
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);

	if(m_nHoverItem != -1)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nHoverItem);

		dc.BitBlt(itemInfo.rc.left,itemInfo.rc.top, itemInfo.rc.Width(), itemInfo.rc.Height(), &m_memDC, itemInfo.rc.left - m_rc.left,itemInfo.rc.top - m_rc.top + m_rc.Height(), SRCCOPY);
	}

	if(m_nDownItem != -1)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);

		dc.BitBlt(itemInfo.rc.left,itemInfo.rc.top, itemInfo.rc.Width(), itemInfo.rc.Height(), &m_memDC, itemInfo.rc.left - m_rc.left,itemInfo.rc.top - m_rc.top + m_rc.Height() * 2, SRCCOPY);
	}
}

// �ؼ��Ķ�����ʱ��
BOOL CDuiTabCtrl::OnControlTimer()
{
	if(!m_bRunTime || (m_nOldItem == -1))
	{
		return FALSE;
	}

	int nAnimatePos = m_rc.Width() / m_nAnimateCount;

	BOOL bFinish = FALSE;
	if(m_nDownItem > m_nOldItem)	// �����л�
	{
		m_nCurXPos += nAnimatePos;
		if(m_nCurXPos >= m_rc.Width())
		{
			bFinish = TRUE;
		}
	}else	// �����л�
	{
		m_nCurXPos -= nAnimatePos;
		if((m_rc.Width() + m_nCurXPos) <= 0)
		{
			bFinish = TRUE;
		}
	}

	if(bFinish)
	{
		// �����л�������ʱ��
		m_bRunTime = false;
	}

	UpdateControl();

	return true;
}

// ���ӿؼ�(֧���л�tabҳ�Ķ�����ʾ)
BOOL CDuiTabCtrl::DrawSubControls(CDC &dc, CRect rcUpdate)
{
	if(!m_bRunTime)
	{
		// ���û�д��ڶ�������,��ֱ�ӻ������ӿؼ�
		return __super::DrawSubControls(dc, rcUpdate);
	}

	TabItemInfo* pOldTabInfo = GetItemInfo(m_nOldItem);
	TabItemInfo* pNewTabInfo = GetItemInfo(m_nDownItem);
	if((pOldTabInfo == NULL) || (pNewTabInfo == NULL))
	{
		return FALSE;
	}

	int nLeft = m_rc.left;
	int nTop = m_rc.top;
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	// ��ʼ����tabҳ����dc,�����Ʊ���������dc
	CBitmap	aniBitmapOld;
	CDC animateDCOld;
	animateDCOld.CreateCompatibleDC(&dc);
	aniBitmapOld.CreateCompatibleBitmap(&dc, m_rc.right, m_rc.bottom);
	CBitmap* pOldAniBitmapOld = animateDCOld.SelectObject(&aniBitmapOld);
	animateDCOld.BitBlt(nLeft, nTop, nWidth, nHeight, &dc, nLeft, nTop, SRCCOPY);	// �������Ƶ�����dc

	// ��ʼ����tabҳ����dc,�����Ʊ���������dc
	CBitmap	aniBitmapNew;
	CDC animateDCNew;
	animateDCNew.CreateCompatibleDC(&dc);
	aniBitmapNew.CreateCompatibleBitmap(&dc, m_rc.right, m_rc.bottom);
	CBitmap* pOldAniBitmapNew = animateDCNew.SelectObject(&aniBitmapNew);
	animateDCNew.BitBlt(nLeft, nTop, nWidth, nHeight, &dc, nLeft, nTop, SRCCOPY);	// �������Ƶ�����dc

	// ����tabҳ
	// ���þ�tabҳΪ�ɼ�
	pOldTabInfo->pControl->SetVisible(TRUE);
	pNewTabInfo->pControl->SetVisible(FALSE);
	// ����tabҳ������dc
	pOldTabInfo->pControl->Draw(animateDCOld, rcUpdate);
	// ����dc���Ƶ�dc
	int nTabTop = pOldTabInfo->pControl->GetRect().top;
	if(m_nCurXPos > 0)
	{
		dc.BitBlt(nLeft, nTabTop, nWidth-m_nCurXPos, m_rc.bottom-nTabTop, &animateDCOld, nLeft+m_nCurXPos, nTabTop, SRCCOPY);
	}else
	{
		dc.BitBlt(nLeft-m_nCurXPos, nTabTop, nWidth+m_nCurXPos, m_rc.bottom-nTabTop, &animateDCOld, nLeft, nTabTop, SRCCOPY);
	}

	// �ͷž�tabҳ����dc
	animateDCOld.SelectObject(pOldAniBitmapOld);
	animateDCOld.DeleteDC();
	aniBitmapOld.DeleteObject();

	// ����tabҳ
	//UpdateAnimateDC(dc, m_rc.right, m_rc.bottom);	// ��ʼ������dc
	//m_animateDC.BitBlt(nLeft, nTop, nWidth, nHeight, &dc, nLeft, nTop, SRCCOPY);	// �������Ƶ�����dc
	// ������tabҳΪ�ɼ�
	pOldTabInfo->pControl->SetVisible(FALSE);
	pNewTabInfo->pControl->SetVisible(TRUE);
	// ����tabҳ������dc
	pNewTabInfo->pControl->Draw(animateDCNew, rcUpdate);
	// ����dc���Ƶ�dc
	nTabTop = pNewTabInfo->pControl->GetRect().top;
	if(m_nCurXPos > 0)
	{
		dc.BitBlt(nLeft+nWidth-m_nCurXPos, nTabTop, m_nCurXPos, m_rc.bottom-nTabTop, &animateDCNew, nLeft, nTabTop, SRCCOPY);
	}else
	{
		dc.BitBlt(nLeft+0, nTabTop, -m_nCurXPos, m_rc.bottom-nTabTop, &animateDCNew, nLeft+nWidth+m_nCurXPos, nTabTop, SRCCOPY);
	}

	// �ͷ���tabҳ����dc
	animateDCNew.SelectObject(pOldAniBitmapNew);
	animateDCNew.DeleteDC();
	aniBitmapNew.DeleteObject();

	return TRUE;
}