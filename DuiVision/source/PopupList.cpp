#include "StdAfx.h"
#include "PopupList.h"

CPopupList::CPopupList(void)
{
	m_nHoverItem = -1;
	m_pHeadImage = NULL;
	m_pCloseImage = NULL;
	m_rcClose.SetRectEmpty();
	m_buttonState = enBSNormal;
	m_nWidth = 191;

	VERIFY(m_font.CreateFont(
		18,							// ����ĸ߶�  
		0,							// ����Ŀ��  
		0,							// ������ʾ�ĽǶ�
		0,							// ����ĽǶ�
		FW_DONTCARE,				// ����İ���
		FALSE,						// б������
		FALSE,						// ���»��ߵ�����
		0,							// ��ɾ���ߵ�����
		GB2312_CHARSET,				// ������ַ���
		OUT_DEFAULT_PRECIS,			// ����ľ���
		CLIP_DEFAULT_PRECIS,		// �ü��ľ���
		DEFAULT_QUALITY,			// �߼�����������豸��ʵ��
		DEFAULT_PITCH | FF_SWISS,	// ����������弯
		DuiSystem::GetDefaultFont()));	// ��������
}

CPopupList::~CPopupList(void)
{
	// �ͷ�ͼƬ��Դ
	if(m_pHeadImage != NULL)
	{
		delete m_pHeadImage;
		m_pHeadImage = NULL;
	}

	if(m_pCloseImage != NULL)
	{
		delete m_pCloseImage;
		m_pCloseImage = NULL;
	}

	for(int i = 0; i < m_vecItem.size(); i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		if(editListItem.pImage != NULL)
		{
			delete editListItem.pImage;
			editListItem.pImage = NULL;
		}
	}

	// �ж�������ؼ���combo�Ļ�,��combo�е�ָ������Ϊ��
	CDuiObject* pParent = GetParent();
	if(pParent->IsClass(CDuiComboBox::GetClassName()))
	{
		((CDuiComboBox*)pParent)->m_pPopupList = NULL;
	}
}

// ����XML�ڵ�
BOOL CPopupList::Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

    // ʹ��XML�ڵ��ʼ���ؼ�
	if(pXmlElem != NULL)
	{
		// ����ͼƬ����ɫ�б�
		TiXmlElement* pControlElem = NULL;
		for (pControlElem = pXmlElem->FirstChildElement("item"); pControlElem != NULL; pControlElem=pControlElem->NextSiblingElement())
		{
			if(pControlElem != NULL)
			{
				//CString strName = CEncodingUtil::AnsiToUnicode(pControlElem->Attribute("name"));
				//CString strDesc = CEncodingUtil::AnsiToUnicode(pControlElem->Attribute("desc"));
				//CString strValue = CEncodingUtil::AnsiToUnicode(pControlElem->Attribute("value"));
				//CString strImage = CEncodingUtil::AnsiToUnicode(pControlElem->Attribute("image"));
				UINT nResourceID = 0;
				CStringA strNameA = pControlElem->Attribute("name");
				DuiSystem::Instance()->ParseDuiString(strNameA);
				CString strName = CA2T(strNameA, CP_UTF8);
				CStringA strDescA = pControlElem->Attribute("desc");
				DuiSystem::Instance()->ParseDuiString(strDescA);
				CString strDesc = CA2T(strDescA, CP_UTF8);
				CStringA strValueA = pControlElem->Attribute("value");
				DuiSystem::Instance()->ParseDuiString(strValueA);
				CString strValue = CA2T(strValueA, CP_UTF8);
				CStringA strImageA = pControlElem->Attribute("image");
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

				AddItem(strName, strDesc, strValue, nResourceID, strImage);
			}
		}
	}

    return TRUE;
}

void CPopupList::SetHeadBitmap(UINT nResourceID)
{
	if(ImageFromIDResource(nResourceID, _T("PNG"), m_pHeadImage))
	{
		m_sizeHeadImage.SetSize(m_pHeadImage->GetWidth() / 2, m_pHeadImage->GetHeight());
	}
}

void CPopupList::SetHeadBitmap(CString strImage)
{
	m_pHeadImage = Image::FromFile(strImage, FALSE);

	if(m_pHeadImage->GetLastStatus() == Ok)
	{
		m_sizeHeadImage.SetSize(m_pHeadImage->GetWidth() / 2, m_pHeadImage->GetHeight());
	}
}

void CPopupList::SetDeleteBitmap(UINT nResourceID)
{
	if(ImageFromIDResource(nResourceID, _T("PNG"), m_pCloseImage))
	{
		m_sizeCloseImage.SetSize(m_pCloseImage->GetWidth() / 4, m_pCloseImage->GetHeight());
	}
}

void CPopupList::SetDeleteBitmap(CString strImage)
{
	m_pCloseImage = Image::FromFile(strImage, FALSE);

	if(m_pCloseImage->GetLastStatus() == Ok)
	{
		m_sizeCloseImage.SetSize(m_pCloseImage->GetWidth() / 4, m_pCloseImage->GetHeight());
	}
}

// ����ѡ�е���
void CPopupList::SetCurItem(UINT nItem)
{
	if (nItem >= m_vecItem.size())  return;

	m_nHoverItem = nItem;
	SetItemPoint();
	InvalidateRect(NULL);
}

// ����ѡ�е���
void CPopupList::SetCurItem(CString strValue)
{
	int nItemCount =  m_vecItem.size();
	for(int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		if(editListItem.strValue == strValue)
		{
			m_nHoverItem = i;
			SetItemPoint();
			InvalidateRect(NULL);
			return;
		}
	}
}

bool CPopupList::GetItemDesc(UINT nItem, CString &strDesc)
{
	ASSERT(nItem < m_vecItem.size());
	if (nItem >= m_vecItem.size())  return false;

	EditListItem &editListItem = m_vecItem.at(nItem);
	strDesc = editListItem.strDesc;

	return true;
}

bool CPopupList::GetItemName(UINT nItem, CString &strName)
{
	ASSERT(nItem < m_vecItem.size());
	if (nItem >= m_vecItem.size())  return false;

	EditListItem &editListItem = m_vecItem.at(nItem);
	strName = editListItem.strName;

	return true;
}

bool CPopupList::GetItemValue(UINT nItem, CString &strValue)
{
	ASSERT(nItem < m_vecItem.size());
	if (nItem >= m_vecItem.size())  return false;

	EditListItem &editListItem = m_vecItem.at(nItem);
	strValue = editListItem.strValue;

	return true;
}

bool CPopupList::GetItemImage(UINT nItem, UINT &nResourceID, CString &strImageFile)
{
	ASSERT(nItem < m_vecItem.size());
	if (nItem >= m_vecItem.size())  return false;

	EditListItem &editListItem = m_vecItem.at(nItem);

	nResourceID = editListItem.nResourceID;
	strImageFile = editListItem.strImageFile;

	return true;
}

bool CPopupList::DeleteItem(UINT nItem)
{
	ASSERT(nItem < m_vecItem.size());
	if (nItem >= m_vecItem.size())  return false;

	m_vecItem.erase(m_vecItem.begin() + m_nHoverItem);
	if(m_nHoverItem >= m_vecItem.size())
	{
		m_nHoverItem -= 1;
	}
	SetItemPoint();
	
	return true;
}

// ��������Ϣ
void CPopupList::DrawWindow(CDC &dc, CRect rcClient)
{
	int nItemCount =  m_vecItem.size();
	CFont *pOldFont = dc.SelectObject(&m_font);
	COLORREF clrOld = dc.SetTextColor(RGB(0, 20, 35));
	CRect rcItem, rcText;
	CSize sizeImage;

	for(int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		rcItem = editListItem.rcItem;
		sizeImage = editListItem.sizeImage;
		int nLeftStart = 47;
		if(editListItem.pImage == NULL)
		{
			nLeftStart = 5;
		}

		// ��ʾ��ǰ��
 		if((i == m_nHoverItem) && !editListItem.strDesc.IsEmpty())
 		{
 			dc.FillSolidRect(&rcItem, RGB(0, 147, 209));
 
			// ��ʾname��desc
 			dc.SetTextColor(RGB(0, 20, 35));
 			rcText.SetRect(rcItem.left + nLeftStart, rcItem.top + 6, rcItem.right - 2, rcItem.top + 24);
 			dc.DrawText(editListItem.strName, &rcText, DT_TOP | DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS);
 
 			dc.SetTextColor(RGB(255, 255, 255));
 			rcText.OffsetRect(0, 21);
 			dc.DrawText(editListItem.strDesc, &rcText, DT_TOP | DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS);
 		}
 		else
		{
			if(i == m_nHoverItem)
			{
				dc.FillSolidRect(&rcItem, RGB(0, 147, 209));
			}

			// ֻ��ʾname
			dc.SetTextColor(RGB(0, 20, 35));
			rcText.SetRect(rcItem.left + nLeftStart, rcItem.top, rcItem.right - 2, rcItem.bottom);
			dc.DrawText(editListItem.strName, &rcText, DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		}
	}
	dc.SelectObject(pOldFont);
	dc.SetTextColor(clrOld);
}

// ��ͼƬ��Ϣ
void CPopupList::DrawWindowEx(CDC &dc, CRect rcClient)
{
	int nItemCount =  m_vecItem.size();
	CRect rcItem;
	CSize sizeImage;
	Graphics graphics(dc);

	for(int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		rcItem = editListItem.rcItem;
		sizeImage = editListItem.sizeImage;

		// ɾ����ť
		if((i == m_nHoverItem) && (m_pCloseImage != NULL))
		{
			graphics.DrawImage(m_pCloseImage, RectF(m_rcClose.left, m_rcClose.top, m_rcClose.Width(), m_rcClose.Height()),
				m_buttonState * m_sizeCloseImage.cx, 0, m_sizeCloseImage.cx, m_sizeCloseImage.cy, UnitPixel); 
		}

		// �б�ͼƬ
		if(editListItem.pImage)
		{	
			CRect rcHead(rcItem.left + 1, rcItem.top + 2, rcItem.left + 1 + rcItem.Height() - 4, rcItem.top + 2 + rcItem.Height() - 4);
			graphics.DrawImage(editListItem.pImage, RectF(rcHead.left, rcHead.top, rcHead.Width(), rcHead.Height()),
				0, 0, editListItem.sizeImage.cx, editListItem.sizeImage.cy, UnitPixel);

			DrawImageFrame(graphics, m_pHeadImage, rcHead, i == m_nHoverItem ? m_sizeHeadImage.cx : 0, 0, m_sizeHeadImage.cx, m_sizeHeadImage.cy, 5);
		}
	}
}

void CPopupList::InitUI(CRect rcClient)
{
}

// ����б���
int CPopupList::AddItem(CString strName, CString strDesc, CString strValue, int nResourceID, CString strImageFile)
{
	EditListItem editListItem;
	editListItem.strName = strName;
	editListItem.strDesc = strDesc;
	editListItem.strValue= strValue;
	editListItem.strImageFile = strImageFile;
	editListItem.nResourceID = nResourceID;
	editListItem.pImage = NULL;
	editListItem.sizeImage.SetSize(0 ,0);
 	if(!strImageFile.IsEmpty())
 	{
 		editListItem.pImage = Image::FromFile(strImageFile, FALSE);
 
 		if(	editListItem.pImage)
 		{
 			editListItem.sizeImage.SetSize(editListItem.pImage->GetWidth(), editListItem.pImage->GetHeight());
 		}
 	}
	if(editListItem.pImage == NULL || editListItem.pImage->GetLastStatus() != Ok)
	{
		if(ImageFromIDResource(editListItem.nResourceID, _T("PNG"), editListItem.pImage))
		{
			editListItem.sizeImage.SetSize(editListItem.pImage->GetWidth(), editListItem.pImage->GetHeight());
		}
	}
	m_vecItem.push_back(editListItem);

	if(m_nHoverItem == -1)
	{
		m_nHoverItem = 0;
	}

	SetItemPoint();

	return m_vecItem.size();
}

// �����б����С
void CPopupList::SetItemPoint()
{
	int nItemCount =  m_vecItem.size();

	// �ж��Ƿ���Desc�ֶ�,�����û�еĻ�,����ʾ���б�һ������ͬ�߶ȵ�
	BOOL bHaveDesc = FALSE;
	for (int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		if(!editListItem.strDesc.IsEmpty())
		{
			bHaveDesc = TRUE;
			break;
		}
	}

	int nHeight = 4;
	nHeight += 24 * nItemCount;
	if(bHaveDesc)
	{
		if(0 == nItemCount) nHeight += 40;
		if(nItemCount >= 1) nHeight += 20;
		if(nItemCount >= 2) nHeight += 9;
		if(nItemCount >= 2) nHeight += 9;
	}

	int nStratTop = 2;
	for (int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		editListItem.rcItem.left = 2;
		editListItem.rcItem.right = m_nWidth - 2;
		if((i - 1 == m_nHoverItem) && (m_nHoverItem != -1) || (i + 1 == m_nHoverItem))
		{
			editListItem.rcItem.top = nStratTop;
			editListItem.rcItem.bottom = nStratTop + (bHaveDesc ? 33 : 24);
			nStratTop += (bHaveDesc ? 33 : 24);
		}
		else if(i == m_nHoverItem)
		{
			editListItem.rcItem.top = nStratTop;
			editListItem.rcItem.bottom = nStratTop + (bHaveDesc ? 44 : 24);
			nStratTop += (bHaveDesc ? 44 : 24);

			int nLeft = editListItem.rcItem.right - m_sizeCloseImage.cx - 7;
			int nTop = editListItem.rcItem.top + ((bHaveDesc ? 44 : 24) - m_sizeCloseImage.cy) / 2 + 1;

			m_rcClose.SetRect(nLeft, nTop, nLeft + m_sizeCloseImage.cx, nTop + m_sizeCloseImage.cy);
		}
		else
		{
			editListItem.rcItem.top = nStratTop;
			editListItem.rcItem.bottom = nStratTop + 24;
			nStratTop += 24;
		}
	}
	
	if(IsWindow(GetSafeHwnd()))
	{
		SetWindowPos(NULL, 0, 0, m_nWidth, nHeight, SWP_NOMOVE);
	}
}

BOOL CPopupList::OnMouseMove(CPoint point)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	BOOL bDraw = FALSE;
	if(rcClient.PtInRect(point))
	{	
		int nHoverItem = m_nHoverItem;
		
		if(m_nHoverItem != -1)
		{
			EditListItem &editListItem = m_vecItem.at(m_nHoverItem);
			if(editListItem.rcItem.PtInRect(point))
			{
				if((m_pCloseImage != NULL) && m_rcClose.PtInRect(point))
				{
					bDraw = m_buttonState != enBSHover;
					m_buttonState = enBSHover;
				}
				else
				{
					bDraw = m_buttonState == enBSHover;
					m_buttonState = enBSNormal;
				}
			}
			else
			{
				nHoverItem = -1;	
				bDraw = TRUE;
			}
		}

		if(m_nHoverItem != nHoverItem)
		{
			int nItemCount =  m_vecItem.size();
			for (int i = 0; i < nItemCount; i++)
			{
				EditListItem &editListItem = m_vecItem.at(i);
				if(editListItem.rcItem.PtInRect(point))
				{
					m_nHoverItem = i;
					bDraw = TRUE;
					break;
				}
			}
			if(m_nHoverItem != -1)
			{
				SetItemPoint();
			}
			if((m_pCloseImage != NULL) && m_rcClose.PtInRect(point))
			{
				m_buttonState = enBSHover;
			}
			else
			{
				m_buttonState = enBSNormal;
			}
			bDraw = TRUE;
		}
	}
	else
	{
		m_buttonState = enBSNormal;
		bDraw = TRUE;
	}
	
	return bDraw;
}

BOOL CPopupList::OnLButtonDown(CPoint point)
{
	if(m_buttonState == enBSHover)
	{
		m_buttonState = enBSDown;
		return TRUE;
	}
	
	return FALSE;
}

BOOL  CPopupList::OnLButtonUp(CPoint point)
{ 
	BOOL bDraw = FALSE;
	if(m_buttonState == enBSDown)
	{
		if((m_pCloseImage != NULL) && m_rcClose.PtInRect(point))
		{
			if(m_nHoverItem != -1)
			{
				//SendMessage(m_uMessageID, DELETE_ITEM, m_nHoverItem);
				SendMessageToParent(m_uMessageID, DELETE_ITEM, m_nHoverItem);
				bDraw = TRUE;
			}
		}	
		else
		{
			bDraw = m_buttonState != enBSNormal;
			m_buttonState = enBSNormal;
		}
	}
	else if(m_nHoverItem != -1)
	{
		EditListItem &editListItem = m_vecItem.at(m_nHoverItem);
		if(editListItem.rcItem.PtInRect(point))
		{
			//SendMessage(m_uMessageID, SELECT_ITEM, m_nHoverItem);
			SendMessageToParent(m_uMessageID, SELECT_ITEM, m_nHoverItem);
		}
	}
	
	return bDraw;
}