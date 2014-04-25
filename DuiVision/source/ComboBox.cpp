#include "StdAfx.h"
#include "ComboBox.h"

CDuiComboBox::CDuiComboBox(HWND hWnd, CDuiObject* pDuiObject)
	: CEditEx(hWnd, pDuiObject)
{
	m_bReadOnly = true;
	m_nResourceIDHeadBitmap = 0;
	m_nResourceIDDeleteBitmap = 0;
	m_strImageHeadBitmap = _T("");
	m_strImageDeleteBitmap = _T("");
	m_strXmlFile = _T("");
	m_pPopupList = NULL;
	m_strComboValue = _T("");
}

CDuiComboBox::~CDuiComboBox(void)
{
}

// ���ؼ���XML�ڵ㺯���������²��item����
BOOL CDuiComboBox::Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl)
{
	__super::Load(pXmlElem);

	if(pXmlElem == NULL)
	{
		return FALSE;
	}
	
	// �����²��item�ڵ���Ϣ
	TiXmlElement* pItemElem = NULL;
	for (pItemElem = pXmlElem->FirstChildElement("item"); pItemElem != NULL; pItemElem=pItemElem->NextSiblingElement())
	{
		CStringA strNameA = pItemElem->Attribute("name");
		DuiSystem::Instance()->ParseDuiString(strNameA);
		CString strName = CA2T(strNameA, CP_UTF8);
		CStringA strDescA = pItemElem->Attribute("desc");
		DuiSystem::Instance()->ParseDuiString(strDescA);
		CString strDesc = CA2T(strDescA, CP_UTF8);
		CStringA strValueA = pItemElem->Attribute("value");
		DuiSystem::Instance()->ParseDuiString(strValueA);
		CString strValue = CA2T(strValueA, CP_UTF8);
		UINT nResourceID = 0;
		CStringA strImageA = pItemElem->Attribute("image");
		CString strImage = CA2T(strImageA, CP_UTF8);
		if(!strImage.IsEmpty())
		{
			if(strImage.Find(_T("skin:")) == 0)
			{
				strImage = CEncodingUtil::AnsiToUnicode(DuiSystem::Instance()->GetSkin(CEncodingUtil::UnicodeToAnsi(strImage)));
			}

			if(strImage.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
			{
				strImage = DuiSystem::GetExePath() + strImage;
			}else	// ����ͼƬ��Դ
			{
				nResourceID = _wtoi(strImage);
				strImage = _T("");
			}
		}

		ComboListItem comboItem;
		comboItem.nResourceID = nResourceID;
		comboItem.strImageFile = strImage;
		comboItem.strName = strName;
		comboItem.strDesc = strDesc;
		comboItem.strValue = strValue;
		m_vecItem.push_back(comboItem);

		// ����ǵ�ǰֵ,����±༭�����ʾ����
		if(!strValue.IsEmpty() && (strValue == m_strComboValue))
		{
			SetTitle(strName);
		}
	}

    return TRUE;
}

// ����Comboѡ�����ֵ
void CDuiComboBox::SetComboValue(CString strComboValue)
{
	m_strComboValue = strComboValue;

	// ���������б��ѡ����
	if(m_pPopupList)
	{
		m_pPopupList->SetCurItem(strComboValue);
	}

	// ������һ�±༭������Ϊ��,���������б�Ϊ��ʱ��δ����
	SetTitle(_T(""));

	// ���ñ༭������
	for (size_t i = 0; i < m_vecItem.size(); i++)
	{
		ComboListItem* pItem = &(m_vecItem.at(i));
		if(pItem->strValue == strComboValue)
		{
			SetTitle(pItem->strName);
			break;
		}
	}

	UpdateControl();
}

// ��ȡComboѡ�����ֵ
CString CDuiComboBox::GetComboValue()
{
	return m_strComboValue;
}

// ��ȡCombo���������
int CDuiComboBox::GetItemCount()
{
	return m_vecItem.size();
}

// ���Combo��
int CDuiComboBox::AddItem(CString strName, CString strDesc, CString strValue, int nResourceID, CString strImageFile)
{
	CString strImage = strImageFile;
	if(!strImage.IsEmpty())
	{
		if(strImage.Find(_T("skin:")) == 0)
		{
			strImage = CEncodingUtil::AnsiToUnicode(DuiSystem::Instance()->GetSkin(CEncodingUtil::UnicodeToAnsi(strImage)));
		}

		if(strImage.Find(_T(".")) != -1)	// ͼƬ�ļ�
		{
			if(strImage.Find(_T(":")) == -1)
			{
				strImage = DuiSystem::GetExePath() + strImage;
			}
		}else	// ����ͼƬ��Դ
		{
			nResourceID = _wtoi(strImage);
			strImage = _T("");
		}
	}

	ComboListItem comboItem;
	comboItem.nResourceID = nResourceID;
	comboItem.strImageFile = strImage;
	comboItem.strName = strName;
	comboItem.strDesc = strDesc;
	comboItem.strValue = strValue;
	m_vecItem.push_back(comboItem);

	// ����ǵ�ǰֵ,����±༭�����ʾ����
	if(!strValue.IsEmpty() && (strValue == m_strComboValue))
	{
		SetTitle(strName);
	}

	if(m_pPopupList)
	{
		return m_pPopupList->AddItem(strName, strDesc, strValue, nResourceID, strImageFile);
	}
	return 0;
}

// ���Combo������
void CDuiComboBox::ClearItems()
{
	m_vecItem.clear();

	// �ر�popuplist����
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		m_buttonState = enBSNormal;
		m_EditState = enBSNormal;
		InvalidateRect(GetRect());
		pDlg->CloseDlgPopup();
		m_pPopupList = NULL;
	}
}

// ��XML����ͼƬ��Ϣ����
HRESULT CDuiComboBox::OnAttributeHeadImage(const CStringA& strValue, BOOL bLoading)
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
		m_strImageHeadBitmap = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		if(strSkin.Find(":") != -1)
		{
			m_strImageHeadBitmap = CA2T(strSkin, CP_UTF8);
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = atoi(strSkin);
		m_nResourceIDHeadBitmap = nResourceID;
	}

	return bLoading?S_FALSE:S_OK;
}

// ��XML����ͼƬ��Ϣ����
HRESULT CDuiComboBox::OnAttributeDeleteImage(const CStringA& strValue, BOOL bLoading)
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
		m_strImageDeleteBitmap = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		if(strSkin.Find(":") != -1)
		{
			m_strImageDeleteBitmap = CA2T(strSkin, CP_UTF8);
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = atoi(strSkin);
		m_nResourceIDDeleteBitmap = nResourceID;
	}

	return bLoading?S_FALSE:S_OK;
}

// ��Ϣ����
LRESULT CDuiComboBox::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if((BUTTOM == wParam) && (BUTTOM_DOWN == lParam) && (m_pPopupList == NULL))	// ������˱༭���������ť
	{
		CPopupList *pPopupList = new CPopupList;
		m_pPopupList = pPopupList;
		pPopupList->SetParent(this);	// ��PopupList�ĸ��ؼ�ָ��combobox
		pPopupList->SetWidth(m_nWidth);
		if(m_nResourceIDHeadBitmap != 0)
		{
			pPopupList->SetHeadBitmap(m_nResourceIDHeadBitmap);
		}else
		if(!m_strImageHeadBitmap.IsEmpty())
		{
			pPopupList->SetHeadBitmap(m_strImageHeadBitmap);
		}

		if(m_nResourceIDDeleteBitmap != 0)
		{
			pPopupList->SetDeleteBitmap(m_nResourceIDDeleteBitmap);
		}else
		if(!m_strImageDeleteBitmap.IsEmpty())
		{
			pPopupList->SetDeleteBitmap(m_strImageDeleteBitmap);
		}
		
		CRect rcClient = GetRect();
		rcClient.top = rcClient.bottom;
		CDlgBase* pDlg = GetParentDialog();
		if(pDlg)
		{
			pDlg->OpenDlgPopup(pPopupList, rcClient, GetID());
		}

		// ���봰�ڴ���֮����ܼ�������
		for (size_t i = 0; i < m_vecItem.size(); i++)
		{
			ComboListItem* pItem = &(m_vecItem.at(i));
			pPopupList->AddItem(pItem->strName, pItem->strDesc, pItem->strValue,
				pItem->nResourceID, pItem->strImageFile);
		}

		if(!m_strXmlFile.IsEmpty())
		{
			pPopupList->LoadXmlFile(m_strXmlFile);
		}

		// ����ѡ�����
		SetComboValue(m_strComboValue);
	}else
	if((SELECT_ITEM == wParam) && m_pPopupList)	// ������ѡ��
	{
		CString strName;
		m_pPopupList->GetItemName(lParam, strName);
		m_pPopupList->GetItemValue(lParam, m_strComboValue);
		SetTitle(strName);
		CDlgBase* pDlg = GetParentDialog();
		if(pDlg)
		{
			m_buttonState = enBSNormal;
			m_EditState = enBSNormal;
			InvalidateRect(GetRect());
			pDlg->CloseDlgPopup();
			m_pPopupList = NULL;
		}
	}else
	if((DELETE_ITEM == wParam) && m_pPopupList)	// ɾ���������б���
	{
		// ���������ɾ����ťͼƬ���ſ��Խ���ɾ��
		if(!m_strImageDeleteBitmap.IsEmpty() || (m_nResourceIDDeleteBitmap != 0))
		{
			m_pPopupList->DeleteItem(lParam);
		}
	}

	return __super::OnMessage(uID, uMsg, wParam, lParam);
}

// �����¼�����
BOOL CDuiComboBox::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// �����ǰ���ڽ���״̬,���¼�ͷ���������б�
	if(m_bDown && (nChar == VK_DOWN) && (nFlags == 0) && IsFocusControl())
	{
		// ģ�������
		SendMessage(m_uID, BUTTOM, BUTTOM_DOWN);
		return true;
	}

	return false;
}