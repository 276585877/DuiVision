#include "StdAfx.h"
#include "DuiMenu.h"

CDuiMenu::CDuiMenu(CString strFont/* = TEXT("����")*/, int nFontWidth/* = 12*/, FontStyle fontStyle/* = FontStyleRegular*/)
	: CDlgPopup()
{
	m_strFont = strFont;
	m_nFontWidth = nFontWidth;
	m_fontStyle = fontStyle;
	m_uAlignment = DT_LEFT;
	m_uVAlignment = DT_TOP;

	m_nLeft = 30;
	m_nHeight = 30;
	m_nWidth = 113;
	m_nFrameWidth = 0;
	m_nSeparatorHeight = 4;
}

CDuiMenu::~CDuiMenu(void)
{
}

BOOL CDuiMenu::Create(CWnd *pParent, CPoint point, UINT uMessageID, UINT nResourceID, int nFrameSize/* = 4*/, int nMinWidth/* = 112*/, enumBackMode enBackMode/* = enBMFrame*/)
{
	CRect rc(point.x - nMinWidth / 2, point.y, point.x + nMinWidth / 2, point.y + nMinWidth);
	return CDlgPopup::Create(pParent, rc, uMessageID, nResourceID, enBackMode, nFrameSize);
}

BOOL CDuiMenu::Create(CWnd *pParent, CPoint point, UINT uMessageID, CString strImage, int nFrameSize/* = 4*/, int nMinWidth/* = 112*/, enumBackMode enBackMode/* = enBMFrame*/)
{
	CRect rc(point.x - nMinWidth / 2, point.y, point.x + nMinWidth / 2, point.y + nMinWidth);
	return CDlgPopup::Create(pParent, rc, uMessageID, strImage, enBackMode, nFrameSize);
}

BOOL CDuiMenu::Create(CWnd *pParent, CPoint point, UINT uMessageID)
{
	int nMinWidth = m_nWidth;
	CRect rc(point.x - nMinWidth / 2, point.y, point.x + nMinWidth / 2, point.y + nMinWidth);
	return CDlgPopup::Create(pParent, rc, uMessageID);
}

void CDuiMenu::DrawWindowEx(CDC &dc, CRect rcClient)
{
}

// ���ؼ���XML�ڵ㺯���������²��Menu item��Ϣ
BOOL CDuiMenu::Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl)
{
	SetRect(CRect(0, 0, m_nWidth, m_nHeight));

	__super::Load(pXmlElem, bLoadSubControl);

	if(pXmlElem == NULL)
	{
		return FALSE;
	}

	if(!bLoadSubControl)
	{
		// �������ӿؼ�
		return TRUE;
	}

	// �˵����ڿ������Ϊpopup���ڵĿ��
	m_nWidth = m_size.cx;

	// ��������
	Create(m_pParent, m_point, m_uMessageID);
	
	// �����²��item�ڵ���Ϣ(��������¶�ʹ��DlgPopup��Load�ؼ���ʽ���ز˵���,����Ľ����Ƚ����õ�)
	int nIdIndex = 100;
	TiXmlElement* pItemElem = NULL;
	for (pItemElem = pXmlElem->FirstChildElement("item"); pItemElem != NULL; pItemElem=pItemElem->NextSiblingElement())
	{
		CStringA strId = pItemElem->Attribute("id");
		int nId = nIdIndex;
		if(strId != "")
		{
			nId = atoi(strId);
		}

		CStringA strType = pItemElem->Attribute("type");
		CStringA strName = pItemElem->Attribute("name");
		CStringA strImage = pItemElem->Attribute("image");
		CStringA strTitle = pItemElem->Attribute("title");
		
		if(strType == "separator")
		{
			// �ָ���Ҳ������ͼƬ�ķ�ʽ
			AddSeparator();
			continue;
		}
		CString strTitleU = CA2T(strTitle, CP_UTF8);
		if(strImage.Find(".") != -1)	// ����ͼƬ�ļ�
		{
			CString strImgFile = DuiSystem::GetSkinPath() + CEncodingUtil::AnsiToUnicode(strImage);
			AddMenu(strTitleU, nIdIndex, strImgFile);
		}else
		if(!strImage.IsEmpty())
		{
			UINT nResourceID = atoi(strImage);
			AddMenu(strTitleU, nIdIndex, nResourceID);
		}else
		{
			AddMenu(strTitleU, nIdIndex);
		}

		nIdIndex++;
	}

	// ˢ�¸��˵��ؼ���λ��
	SetMenuPoint();

	m_bInit = TRUE;

    return TRUE;
}

// ����ָ�����ֵĲ˵��ڵ�
BOOL CDuiMenu::LoadSubMenu(TiXmlElement* pXmlElem, CString strSubItemName)
{
	if(pXmlElem == NULL)
	{
		return FALSE;
	}

	// �ݹ�����²�ڵ�,���Ƿ���ָ�����ֵĽڵ�
	TiXmlElement* pItemElem = NULL;
	for (pItemElem = pXmlElem->FirstChildElement(); pItemElem != NULL; pItemElem=pItemElem->NextSiblingElement())
	{
		CStringA strNameA = pItemElem->Attribute("name");
		if(strSubItemName == CEncodingUtil::AnsiToUnicode(strNameA))
		{
			// �����Ӳ˵�
			return Load(pItemElem);
		}
		if(LoadSubMenu(pItemElem, strSubItemName))
		{
			// ����ݹ���سɹ��򷵻�,����������±�������
			return TRUE;
		}
	}
	return FALSE;
}

// ����XML�ڵ��ж���Ĳ˵��������ؼ�
BOOL CDuiMenu::LoadXmlNode(TiXmlElement* pXmlElem, CString strXmlFile)
{
	if(pXmlElem == NULL)
	{
		return FALSE;
	}

	TiXmlElement* pControlElem = NULL;
	for (pControlElem = pXmlElem->FirstChildElement(); pControlElem != NULL; pControlElem=pControlElem->NextSiblingElement())
	{
		if(pControlElem != NULL)
		{
			CStringA strControlName = pControlElem->Value();
			CControlBase* pControl = _CreateControlByName(strControlName);
			if(pControl)
			{
				if(pControl->Load(pControlElem))
				{
					// ���Load�ɹ�,����ӿؼ�
					if(pControl->IsClass(CArea::GetClassName()) || pControl->IsClass(CFrame::GetClassName()))
					{
						// Area��Frame������Ӧ���,����ӵ�Area�б���
						m_vecArea.push_back(pControl);
					}else
					{
						m_vecControl.push_back(pControl);
					}
					// ����ǲ˵���ؼ�,�����ò˵���Ĳ˵�XML����
					if(pControl->IsClass(CMenuItem::GetClassName()))
					{
						((CMenuItem*)pControl)->SetMenuXml(strXmlFile);
					}
				}else
				{
					// ����ֱ��ɾ���ؼ�����ָ��
					delete pControl;
				}
			}
		}
	}

	return TRUE;
}

// ����XML�ļ��ж���Ĳ˵�
BOOL CDuiMenu::LoadXmlFile(CString strFileName, CString strSubItemName)
{
	TiXmlDocument xmlDoc;
	TiXmlElement* pDivElem = NULL;

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
		pDivElem = xmlDoc.FirstChildElement(GetClassName());
		if(pDivElem != NULL)
		{
			if(!strSubItemName.IsEmpty())
			{
				// ����һ��XML��root�ڵ������(�������ӿؼ�)
				Load(pDivElem, FALSE);
				// ָ�����Ӳ˵���,�����Ӳ˵������в��Ҳ�����
				LoadSubMenu(pDivElem, strSubItemName);
			}else
			{
				// ����menu�ڵ�(�������ýڵ�˵����XML�ļ�����)
				LoadXmlNode(pDivElem, strFileName);
			}
		}
	}

	return TRUE;
}

// ����XML�ļ�
BOOL CDuiMenu::LoadXmlFile(CString strFileName, CWnd *pParent, CPoint point, UINT uMessageID, CString strSubItemName)
{
	m_pParent = pParent;
	m_point = point;
	m_uMessageID = uMessageID;

	TiXmlDocument xmlDoc;
	TiXmlElement* pDivElem = NULL;

	if(strFileName.Find(_T(":")) == -1)
	{
		if(strFileName.Find(_T(".xml")) == -1)
		{
			m_strXmlFile = DuiSystem::Instance()->GetXmlFile(CEncodingUtil::UnicodeToAnsi(strFileName));
		}else
		{
			m_strXmlFile = DuiSystem::GetXmlPath() + strFileName;
		}
	}else
	{
		m_strXmlFile = strFileName;
	}

	BOOL bRet = TRUE;
	xmlDoc.LoadFile(CEncodingUtil::UnicodeToAnsi(m_strXmlFile), TIXML_ENCODING_UTF8);
	if(!xmlDoc.Error())
	{
		pDivElem = xmlDoc.FirstChildElement(GetClassName());
		if(pDivElem != NULL)
		{
			if(!strSubItemName.IsEmpty())
			{
				// ����һ��XML��root�ڵ������(�������ӿؼ�)
				Load(pDivElem, FALSE);
				// ָ�����Ӳ˵���,�����Ӳ˵������в��Ҳ�����
				bRet = LoadSubMenu(pDivElem, strSubItemName);
			}else
			{
				// ����menu�ڵ�����
				bRet = Load(pDivElem);
			}
		}
	}

	return bRet;
}

// UI��ʼ��,�˺����ڴ��ڵ�OnCreate�����е���
void CDuiMenu::InitUI(CRect rcClient)
{
	// ����в˵����Ԥ����ֵ,��������Ӧ��ֵ���ؼ�
	if(m_vecMenuItemValue.size() > 0)
	{
		for (size_t i = 0; i < m_vecMenuItemValue.size(); i++)
		{
			MenuItemValue& itemValue = m_vecMenuItemValue.at(i);
			CControlBase* pControlBase = GetControl(itemValue.strName);
			if(pControlBase)
			{
				if(!itemValue.strTitle.IsEmpty())
				{
					((CControlBaseFont*)pControlBase)->SetTitle(itemValue.strTitle);
				}
				if(!itemValue.bVisible)
				{
					pControlBase->SetVisible(FALSE);
				}
				if(itemValue.bDisable)
				{
					pControlBase->SetDisable(TRUE);
				}
				if(itemValue.nCheck != -1)
				{
					((CMenuItem*)pControlBase)->SetCheck(itemValue.nCheck);
				}
			}
		}

		// ˢ�²˵���λ����Ϣ
		SetMenuPoint();
	}
}

// ��Ӳ˵���Ԥ������Ϣ(���ò˵������)
void CDuiMenu::SetItemTitle(CString strName, CString strTitle)
{
	MenuItemValue itemValue;
	itemValue.strName = strName;
	itemValue.strTitle = strTitle;
	itemValue.bVisible = TRUE;
	itemValue.bDisable = FALSE;
	itemValue.nCheck = -1;
	m_vecMenuItemValue.push_back(itemValue);
}

// ��Ӳ˵���Ԥ������Ϣ(���ò˵���ɼ���)
void CDuiMenu::SetItemVisible(CString strName, BOOL bVisible)
{
	MenuItemValue itemValue;
	itemValue.strName = strName;
	itemValue.strTitle = _T("");
	itemValue.bVisible = bVisible;
	itemValue.bDisable = FALSE;
	itemValue.nCheck = -1;
	m_vecMenuItemValue.push_back(itemValue);
}

// ��Ӳ˵���Ԥ������Ϣ(���ò˵����Ƿ����)
void CDuiMenu::SetItemDisable(CString strName, BOOL bDisable)
{
	MenuItemValue itemValue;
	itemValue.strName = strName;
	itemValue.strTitle = _T("");
	itemValue.bVisible = TRUE;
	itemValue.bDisable = bDisable;
	itemValue.nCheck = -1;
	m_vecMenuItemValue.push_back(itemValue);
}

// ��Ӳ˵���Ԥ������Ϣ(���ò˵����Ƿ�ѡ��)
void CDuiMenu::SetItemCheck(CString strName, int nCheck)
{
	MenuItemValue itemValue;
	itemValue.strName = strName;
	itemValue.strTitle = _T("");
	itemValue.bVisible = TRUE;
	itemValue.bDisable = FALSE;
	itemValue.nCheck = nCheck;
	m_vecMenuItemValue.push_back(itemValue);
}

int CDuiMenu::AddMenu(CString strText, UINT uMenuID, int nResourceID, BOOL bSelect, int nIndex)
{
	CControlBase * pControlBase = NULL;

	FontFamily fontFamily(m_strFont.AllocSysString());
	Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);;

	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentNear);
	strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
	Size size = GetTextBounds(font, strFormat, strText);

	if(size.Width > m_nWidth - m_nLeft - 4)
	{
		m_nWidth = size.Width + m_nLeft + 4;
	}

	pControlBase = new CMenuItem(GetSafeHwnd(),this, uMenuID, CRect(0, 0, 0, 0), strText, m_nLeft, bSelect);
	((CControlBaseFont *)pControlBase)->SetFont(m_strFont, m_nFontWidth, m_fontStyle);
	if(nResourceID != -1)
	{
		((CMenuItem *)pControlBase)->SetBitmap(nResourceID);
	}

	if(nIndex >= 0 && nIndex < m_vecControl.size())
	{
		m_vecControl.insert(m_vecControl.begin() + nIndex, pControlBase);
	}
	else
	{
		m_vecControl.push_back(pControlBase);
	}

	SetMenuPoint();
	return m_vecControl.size();
}

int CDuiMenu::AddMenu(CString strText, UINT uMenuID, CString strImage, BOOL bSelect, int nIndex)
{
	CControlBase * pControlBase = NULL;

	FontFamily fontFamily(m_strFont.AllocSysString());
	Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);;

	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentNear);
	strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
	Size size = GetTextBounds(font, strFormat, strText);

	if(size.Width > m_nWidth - m_nLeft - 4)
	{
		m_nWidth = size.Width + m_nLeft + 4;
	}

	pControlBase = new CMenuItem(GetSafeHwnd(),this, uMenuID, CRect(0, 0, 0, 0), strText, m_nLeft, bSelect);
	((CControlBaseFont *)pControlBase)->SetFont(m_strFont, m_nFontWidth, m_fontStyle);
	if(!strImage.IsEmpty())
	{
		((CMenuItem *)pControlBase)->SetBitmap(strImage);
	}

	if(nIndex >= 0 && nIndex < m_vecControl.size())
	{
		m_vecControl.insert(m_vecControl.begin() + nIndex, pControlBase);
	}
	else
	{
		m_vecControl.push_back(pControlBase);
	}

	SetMenuPoint();
	return m_vecControl.size();
}

// ��Ӳ˵��ָ�
int CDuiMenu::AddSeparator(int nIndex)
{
	// ����ʹ�þ��οؼ���Ҳ����ʹ��ͼƬ�ؼ�
	CControlBase * pControlBase = new CRectangle(GetSafeHwnd(),this, -1, CRect(0, 0, 0, 0), Color(254, 227, 229, 230));

	if(nIndex >= 0 && nIndex < m_vecControl.size())
	{
		m_vecControl.insert(m_vecControl.begin() + nIndex, pControlBase);
	}
	else
	{
		m_vecControl.push_back(pControlBase);
	}

	SetMenuPoint();
	return m_vecControl.size();
}

// ���ò˵���λ��
void CDuiMenu::SetMenuPoint()
{
	int nXPos = 2;
	int nYPos = 2;
	CRect rc;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl[i];
		if(pControlBase == NULL)
		{
			continue;
		}
		if(pControlBase->IsClass(CMenuItem::GetClassName()))	// �����MenuItem���Ϳؼ�
		{
			CMenuItem* pMenuItem = (CMenuItem*)pControlBase;
			pMenuItem->SetFrameWidth(m_nFrameWidth);
			if(!pMenuItem->GetVisible())
			{
				// �˵���ɼ�
				rc.SetRect(0,0,0,0);
			}else
			if(pMenuItem->IsSeparator())
			{
				// �ָ���
				rc.SetRect(m_nLeft + 1, nYPos + 1, m_nWidth - 1, nYPos + 2);
				nYPos += 4;
			}else
			{
				// ��ͨ�˵���
				rc.SetRect(nXPos, nYPos, m_nWidth - 2, nYPos + m_nHeight);
				nYPos += m_nHeight;
			}
		}else
		if(-1 == pControlBase->GetControlID())
		{
			rc.SetRect(m_nLeft + 4, nYPos + 1, m_nWidth - 9, nYPos + 2);
			nYPos += 4;
		}/*
		else
		{
			rc.SetRect(nXPos, nYPos, m_nWidth - 2, nYPos + m_nHeight);
			nYPos += m_nHeight;
		}
		*/
		SetControlRect(pControlBase, rc);
	}
	nYPos += 2;
	SetWindowPos(NULL, 0, 0, m_nWidth, nYPos, SWP_NOMOVE);
	InvalidateRect(NULL);
}

// ��ȡ���˵�����
CDuiMenu* CDuiMenu::GetParentMenu()
{
	CDuiObject* pParentObj = GetParent();
	while((pParentObj != NULL) && (!pParentObj->IsClass("menu")))
	{
		if(pParentObj->IsClass("popup"))
		{
			pParentObj = ((CDlgPopup*)pParentObj)->GetParent();
		}else
		if(pParentObj->IsClass("dlg"))
		{
			pParentObj = ((CDlgBase*)pParentObj)->GetParent();
		}else
		{
			pParentObj = ((CControlBase*)pParentObj)->GetParent();
		}
	}
	if((pParentObj != NULL) && pParentObj->IsClass("menu"))
	{
		return (CDuiMenu*)pParentObj;
	}

	return NULL;
}

// ��ȡ��ǰ���ڻ״̬���Ӳ˵���
CMenuItem* CDuiMenu::GetHoverMenuItem()
{
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl[i];
		if(pControlBase->IsClass(CMenuItem::GetClassName()))	// �����MenuItem���Ϳؼ�
		{
			CMenuItem* pMenuItem = (CMenuItem*)pControlBase;
			if(pMenuItem->IsHover())
			{
				return pMenuItem;
			}
		}
	}

	return NULL;
}

// ��Ϣ��Ӧ
LRESULT CDuiMenu::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((Msg != BUTTOM_UP) && (Msg != BUTTOM_CHECK))
	{
		return 0;
	}

	CControlBase* pControl = GetControl(uID);
	if(pControl && !pControl->GetAction().IsEmpty())
	{
		// ����˵���������action������ӵ�������������У�ͨ��������ִ��
		CString strControlName = pControl->GetName();
		CString strAction = pControl->GetAction();
		//CDuiObject* pParent = pControl->GetParent();
		CDlgBase* pParentDlg = pControl->GetParentDialog();

		DuiSystem::Instance()->AddDuiActionTask(uID, Msg, wParam, lParam, strControlName, strAction, pParentDlg);
	}else
	{
		// ����͵���Popup�ĺ���
		__super::OnMessage(uID, Msg, wParam, lParam);

		/*tagMenuInfo* pMenuInfo = new tagMenuInfo;
		pMenuInfo->uMenuID = uID;
		pMenuInfo->bSelect = (bool)lParam;
		pMenuInfo->bDown = (bool)wParam;
	
		PostMessage(m_uMessageID, Msg, (LPARAM)pMenuInfo);*/
	}

	if(Msg == BUTTOM_UP)
	{
		// ����и��˵�,�����˵��ر�,������ֱ�ӹرյķ���,���������Զ��رձ�ʶ,��ͨ������¼������Զ��ر�
		CDuiMenu* pParentMenu = GetParentMenu();
		if(pParentMenu && !pParentMenu->IsAutoClose())
		{
			pParentMenu->SetAutoClose(TRUE);
			pParentMenu->SetForegroundWindow();
			pParentMenu->PostMessage(WM_LBUTTONDOWN, 0, 0);
		}
		// �ر�����
		CloseWindow();
	}

	return 0;
}

// ���ش���ȥ����ʱ��Ĺرմ��ڲ���
BOOL CDuiMenu::OnNcCloseWindow()
{
	// ����и��˵�,�����˵����ڹر�
	CDuiMenu* pParentMenu = GetParentMenu();
	if(pParentMenu && !pParentMenu->IsAutoClose())
	{
		// �������ڸ��˵�������,�򲻹رո�����
		CMenuItem* pHoverItem = pParentMenu->GetHoverMenuItem();
		if(pHoverItem == NULL)
		{
			pParentMenu->SetAutoClose(TRUE);
			pParentMenu->SetForegroundWindow();
			pParentMenu->PostMessage(WM_LBUTTONDOWN, 0, 0);
		}
	}

	// ����������ǲ˵���,�򽫲˵����еĵ����˵�ָ������Ϊ��
	CDuiObject* pParent = GetParent();
	if((pParent != NULL) && (pParent->IsClass(CMenuItem::GetClassName())))
	{
		((CMenuItem*)pParent)->SetPopupMenu(NULL);
	}

	// �ر�����
	m_pParentDuiObject = NULL;
	CloseWindow();	

	return TRUE;
}