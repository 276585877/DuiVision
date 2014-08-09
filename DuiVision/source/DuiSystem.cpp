#include "StdAfx.h"
#include "DuiSystem.h"

template<> DuiSystem* Singleton<DuiSystem>::ms_Singleton = 0;

static DuiSystem* g_pIns = NULL;
static UINT g_nIDTemplate = 0;

static ULONG_PTR gdiplusToken;
static GdiplusStartupInput gdiplusStartupInput;

DuiSystem::DuiSystem(HINSTANCE hInst, DWORD dwLangID, CString strResourceFile, UINT uAppID, UINT nIDTemplate, CStringA strStyle)
    :m_hInst(hInst), m_uAppID(uAppID)
	//,m_funCreateTextServices(NULL)
{
	g_pIns = this;
	m_dwLangID = dwLangID;
	if((g_nIDTemplate == 0) && (nIDTemplate != 0))
	{
		g_nIDTemplate = nIDTemplate;
	}
	m_bLogEnable = FALSE;
	m_pNotifyMsgBox = NULL;
	m_strCurStyle = strStyle;
	if(strResourceFile.IsEmpty())
	{
		m_strResourceFile = _T("xml\\resource.xml");
	}else
	{
		m_strResourceFile = strResourceFile;
	}

	// TinyXml����Ϊ��ѹ���ո�ģʽ��Ĭ����ѹ���ո񣬻ᵼ�³���һ���Ŀո����ʱ��ת��Ϊһ���ո�
	TiXmlBase::SetCondenseWhiteSpace(false);

	ZeroMemory(&m_NotifyIconData, sizeof m_NotifyIconData);

    createSingletons();
	//m_rich20=LoadLibrary(_T("riched20.dll"));
	//if(m_rich20) m_funCreateTextServices= (PCreateTextServices)GetProcAddress(m_rich20,"CreateTextServices");
}

DuiSystem::~DuiSystem(void)
{
	// �����¼��������
	for (size_t i = 0; i < m_vecDuiHandler.size(); i++)
	{
		CDuiHandler* pDuiHandler = m_vecDuiHandler.at(i);
		if (pDuiHandler)
		{
			delete pDuiHandler;
		}		
	}

	// ����DUI�Ի���
	for (size_t i = 0; i < m_vecDuiDialog.size(); i++)
	{
		CDlgBase* pDlgBase = m_vecDuiDialog.at(i);
		if (pDlgBase)
		{
			delete pDlgBase;
		}		
	}

	// �ͷŸ�����Դ��
	m_mapCfgPool.RemoveAll();
	m_mapStylePool.RemoveAll();
	m_mapXmlPool.RemoveAll();
	m_mapSkinPool.RemoveAll();
	m_mapStringPool.RemoveAll();
	m_mapFontPool.RemoveAll();

	// ����Trayͼ��
	DelTray();

	//if(m_rich20) FreeLibrary(m_rich20);
	//m_funCreateTextServices=NULL;

	// ���������ô˺����ر�GDI+������ᵼ�¹ر�GDI+֮�󻹵���GDI+�ĺ���������쳣
	destroySingletons();
}

// ��������
DuiSystem* DuiSystem::Instance()
{
	if(g_pIns == NULL)
	{
		new DuiSystem(NULL);
	}

	return g_pIns;
}

// �ͷ�
void DuiSystem::Release()
{
	if(g_pIns != NULL)
	{
		delete g_pIns;
		g_pIns = NULL;
	}
}

void DuiSystem::createSingletons()
{
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// ������Դ�ļ�
	LoadResource();

	// ��ʼ����־
	InitLog();

	// ��������������߳�
	m_TaskMsg.Startup();
}

void DuiSystem::destroySingletons()
{
	// ֹͣ����������߳�
	m_TaskMsg.Shutdown();

	// ֹͣ��־
	DoneLog();

	//�ر�gdiplus�Ļ���
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

// ��ȡ����ϵͳ�汾�����汾��
int DuiSystem::GetOSMajorVersion()
{
	// ��ȡ����ϵͳ�汾��Ϣ
	OSVERSIONINFOEX osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!( GetVersionEx ((OSVERSIONINFO *) &osvi)))
	{
		// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx ((OSVERSIONINFO *) &osvi))
		{
			return 0;
		}
	}

	return osvi.dwMajorVersion;
}

// ��ȡ��ǰ����(�ַ�������)
CStringA DuiSystem::GetLanguage()
{
	DWORD dwLangID = DuiSystem::Instance()->GetCurrentLanguage();
	if(dwLangID == 0)
	{
		// �������IDΪ0��ʾ��ȡ��ǰϵͳ������
		dwLangID = ::GetSystemDefaultLangID();
	}
	switch(dwLangID)
	{
	case LANGUAGE_PAGE_ENGLISH:
		return "en-us";break;	// Ӣ��
	case LANGUAGE_PAGE_CHINESE:
		return "zh-cn";break;	// ��������
	case LANGUAGE_PAGE_CHINESE_TW:
	case LANGUAGE_PAGE_CHINESE_HK:
	case LANGUAGE_PAGE_CHINESE_SGP:
		return "zh-tw";break;	// ��������
	}

	return "en-us";	// Ĭ��ΪӢ��
}

// ��ȡ��ǰ����
DWORD DuiSystem::GetCurrentLanguage()
{
	return m_dwLangID;
}

// ���õ�ǰ����
void DuiSystem::SetCurrentLanguage(DWORD dwLangID)
{
	m_dwLangID = dwLangID;
}

// ȡ����Ŀ¼
CString DuiSystem::GetExePath()
{
	TCHAR szFullPath[MAX_PATH];
	TCHAR szdrive[_MAX_DRIVE];
	TCHAR szdir[_MAX_DIR];
	::GetModuleFileName(NULL, szFullPath, MAX_PATH);
	_wsplitpath(szFullPath, szdrive, szdir, NULL, NULL);

	CString szPath;
	szPath.Format(_T("%s%s"), szdrive, szdir);
	//szPath = szPath.Left(szPath.GetLength() - 1);

	return szPath;
}

// ��ȡSkinĿ¼
CString DuiSystem::GetSkinPath()
{
	return GetExePath();
}

// ��ȡXMLĿ¼
CString DuiSystem::GetXmlPath()
{
	return GetExePath() + _T("xml\\");
}

// ·����׼��
BOOL DuiSystem::PathCanonicalize(CString& strPath)
{
	TCHAR strOut[MAX_PATH];
	memset(strOut, 0, MAX_PATH*sizeof(TCHAR));
	if(::PathCanonicalize(strOut, strPath))
	{
		strPath = strOut;
		return TRUE;
	}
	return FALSE;
}

// ������Դ
BOOL DuiSystem::LoadResource()
{
	// ���ͷŸ�����Դ��
	m_mapCfgPool.RemoveAll();
	m_mapStylePool.RemoveAll();
	m_mapXmlPool.RemoveAll();
	m_mapSkinPool.RemoveAll();
	m_mapStringPool.RemoveAll();
	m_mapFontPool.RemoveAll();

	CString strResXmlFile = GetExePath() + m_strResourceFile;
	return LoadResourceXml(strResXmlFile, m_strCurStyle);
}

// ����XML��Դ�ļ�
BOOL DuiSystem::LoadResourceXml(CString strResFile, CStringA strStyleA)
{
	m_strCurStyle = strStyleA;

	// ������Դ�ļ�
	TiXmlDocument xmlDoc;
	xmlDoc.LoadFile(CEncodingUtil::UnicodeToAnsi(strResFile), TIXML_ENCODING_UTF8);
	if(!xmlDoc.Error())
	{
		TiXmlElement* pRootElem = xmlDoc.RootElement();
		if(pRootElem != NULL)
		{
			TiXmlElement* pResElem = NULL;
			for (pResElem = pRootElem->FirstChildElement("res"); pResElem != NULL; pResElem=pResElem->NextSiblingElement())
			{
				CStringA strType = pResElem->Attribute("type");
				if(strType == "res")	// ��Դ�ļ�
				{
					CStringA strLang = pResElem->Attribute("lang");
					if(strLang.IsEmpty() || (strLang == DuiSystem::GetLanguage()))
					{
						// ������Դ�ļ�
						CStringA strFile = pResElem->Attribute("file");
						CString strFileU = GetExePath() + CA2T(strFile, CP_UTF8);
						LoadResourceXml(strFileU, strStyleA);
					}
				}else
				if(strType == "cfg")	// ȫ������
				{
					CStringA strName = pResElem->Attribute("name");
					CStringA strValue = pResElem->Attribute("value");
					m_mapCfgPool.SetAt(strName, strValue);
					// ���DuiSystemδ���õ�ǰ������,�����ͨ��defaultStyle������������ǰ���
					if(m_strCurStyle.IsEmpty() && (strName == "defaultStyle"))
					{
						strStyleA = strValue;
						m_strCurStyle = strValue;
					}
				}else
				if(strType == "style")	// �����
				{
					CStringA strName = pResElem->Attribute("name");
					CStringA strValue = pResElem->Attribute("value");
					m_mapStylePool.SetAt(strName, strValue);
				}else
				if(strType == "xml")	// XML�ļ�����
				{
					CStringA strStyle = pResElem->Attribute("style");
					CStringA strName = pResElem->Attribute("name");
					CStringA strFile = pResElem->Attribute("file");
					if(strStyle.IsEmpty() || (strStyle == m_strCurStyle))
					{
						m_mapXmlPool.SetAt(strName, strFile);
					}
				}else
				if(strType == "img")	// ͼ��
				{
					CStringA strStyle = pResElem->Attribute("style");
					CStringA strName = pResElem->Attribute("name");
					CStringA strFile = pResElem->Attribute("file");
					if(strStyle.IsEmpty() || (strStyle == m_strCurStyle))
					{
						m_mapSkinPool.SetAt(strName, strFile);
					}
				}else
				if(strType == "str")	// �ַ���
				{
					CStringA strLang = pResElem->Attribute("lang");
					if(strLang.IsEmpty() || (strLang == DuiSystem::GetLanguage()))
					{
						// ���δָ�����Ի�ָ�������Բ��Һ͵�ǰ������ͬ��������ַ�����Դ
						CStringA strStyle = pResElem->Attribute("style");
						CStringA strName = pResElem->Attribute("name");
						CStringA strValue = pResElem->Attribute("value");
						if(strStyle.IsEmpty() || (strStyle == m_strCurStyle))
						{
							m_mapStringPool.SetAt(strName, strValue);
						}
					}
				}else
				if(strType == "font")	// ����
				{
					CStringA strLangA = pResElem->Attribute("lang");
					CStringA strNameA = pResElem->Attribute("name");
					CStringA strFontA = pResElem->Attribute("font");
					int nFontWidth = atoi(pResElem->Attribute("size"));
					CStringA strOSA = pResElem->Attribute("os");
					CStringA strBoldA = pResElem->Attribute("bold");
					BOOL bBold = FALSE;
					if(pResElem->Attribute("bold"))
					{
						bBold = (strcmp(pResElem->Attribute("bold"), "true") == 0);
					}
					BOOL bItalic = FALSE;
					if(pResElem->Attribute("italic"))
					{
						bItalic = (strcmp(pResElem->Attribute("italic"), "true") == 0);
					}
					BOOL bUnderline = FALSE;
					if(pResElem->Attribute("underline"))
					{
						bUnderline = (strcmp(pResElem->Attribute("underline"), "true") == 0);
					}
					BOOL bStrikeout = FALSE;
					if(pResElem->Attribute("strikeout"))
					{
						bStrikeout = (strcmp(pResElem->Attribute("strikeout"), "true") == 0);
					}
					FontStyle fontStyle = FontStyleRegular;
					if(bBold)
					{
						fontStyle = FontStyle((int)fontStyle + (int)FontStyleBold);
					}
					if(bItalic)
					{
						fontStyle = FontStyle((int)fontStyle + (int)FontStyleItalic);
					}
					if(bUnderline)
					{
						fontStyle = FontStyle((int)fontStyle + (int)FontStyleUnderline);
					}
					if(bStrikeout)
					{
						fontStyle = FontStyle((int)fontStyle + (int)FontStyleStrikeout);
					}
					DuiFontInfo fontInfo;
					fontInfo.strName = CA2T(strNameA, CP_UTF8);
					// ��������һ�¹���,���ڲ�֧�ֵ�������ȱʡ�����滻
					CString strFont = CA2T(strFontA, CP_UTF8);
					fontInfo.strFont = DuiSystem::GetDefaultFont(strFont);
					fontInfo.nFontWidth = nFontWidth;
					fontInfo.fontStyle = fontStyle;
					fontInfo.strOS = CA2T(strOSA, CP_UTF8);
					if(!fontInfo.strOS.IsEmpty())
					{
						// ���OS���Էǿ�,���жϵ�ǰ����ϵͳ�Ƿ����OS����
						if(CheckOSName(fontInfo.strOS))
						{
							m_mapFontPool.SetAt(strNameA, fontInfo);
						}
					}else
					{
						m_mapFontPool.SetAt(strNameA, fontInfo);
					}
				}
			}
		}
	}

	return TRUE;
}

// ��ȡϵͳ������Ϣ
CString DuiSystem::GetConfig(CStringA strName)
{
	CStringA strCfg;
	if(m_mapCfgPool.Lookup(strName, strCfg))
	{
		return CA2T(strCfg, CP_UTF8);
	}
	return _T("");
}

// ��ȡXML�ļ�
CString DuiSystem::GetXmlFile(CStringA strName)
{
	CStringA strXmlFile;
	if(m_mapXmlPool.Lookup(strName, strXmlFile))
	{
		return GetExePath() + CEncodingUtil::AnsiToUnicode(strXmlFile);
	}
	return _T("");
}

// ��ȡSkin
CStringA DuiSystem::GetSkin(CStringA strName)
{
	if(strName.Find("skin:") == 0)
	{
		strName.Delete(0, 5);
	}

	CStringA strSkin;
	m_mapSkinPool.Lookup(strName, strSkin);
	return strSkin;
}

// ��ȡ�ַ���ֵ
CString DuiSystem::GetString(CStringA strName)
{
	CStringA strString;
	m_mapStringPool.Lookup(strName, strString);
	// ���ַ����е��滻�������滻
	ParseDuiString(strString);
	return CA2T(strString, CP_UTF8);
}

// �����ַ���ֵ
void DuiSystem::SetString(CStringA strName, CString strValue)
{
	m_mapStringPool.SetAt(strName, CT2A(strValue, CP_UTF8));
}

// �����ַ������滻���е��滻����
void DuiSystem::ParseDuiString(CStringA& strString)
{
	CStringA strTmp = strString;
	int nPos1 = strTmp.Find("[");
	while(nPos1 != -1)
	{
		int nPos2 = strTmp.Find("]");
		if(nPos2 > (nPos1 + 1))
		{
			CStringA strName = strTmp.Mid(nPos1+1, nPos2-nPos1-1);
			CStringA strValue;
			if(m_mapStringPool.Lookup(strName, strValue))
			{
				strTmp.Replace(strTmp.Mid(nPos1, nPos2-nPos1+1), strValue);
			}
		}
		nPos1 = strTmp.Find("[", nPos1+1);
	}
	strString = strTmp;
}

// ��ȡ����ϵͳ����
CString DuiSystem::GetOSName()
{
	int nOSType       =  OS_UNKNOWN;
	int nServicePack  =  PACK_UNKNOWN;
	
	// ��ȡ����ϵͳ�汾��Ϣ
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	
	if( !( GetVersionEx((OSVERSIONINFO *) &osvi ) ) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx( (OSVERSIONINFO *) &osvi) )
		{
			return L"";
		}
	}
	
	//����ϵͳ�����汾
	nServicePack = osvi.wServicePackMajor;
	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		
		if ( osvi.dwMajorVersion <= 4 )
		{
			nOSType = WINDOWS_NT;
		}
		
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			nOSType = WINDOWS_2K;
		}
		
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			nOSType = WINDOWS_XP;
		}
		
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			nOSType = WINDOWS_2003;
		}
		
		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
		{
			nOSType = WINDOWS_VISTA;
		}

		if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
		{
			nOSType = WINDOWS_7;
		}

		if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
		{
			nOSType = WINDOWS_8;
		}

		//����ϵͳ�汾��Win8����
		if (osvi.dwMajorVersion > 6)
		{
			nOSType = WINDOWS_HIGH;
		}
		break;
		
	case VER_PLATFORM_WIN32_WINDOWS:
		
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			nOSType = WINDOWS_95;
		} 
		
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			nOSType = WINDOWS_98; 
		} 
		
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			nOSType = WINDOWS_ME;
		} 
		break;
		
	case VER_PLATFORM_WIN32s:  
		
		nOSType = WINDOWS_WIN32;
		break;
	default:
		nOSType = OS_UNKNOWN;
		break;
	}

	CString strOSType = _T("");
	switch( nOSType )
	{
	case WINDOWS_8:
		strOSType = L"win8";
		break;
	case WINDOWS_7:
		strOSType = L"win7";
		break;
	case WINDOWS_VISTA:
		strOSType = L"vista";
		break;
	case WINDOWS_2003:
		strOSType = L"win2003";
		break;
	case WINDOWS_XP:
		strOSType = L"winxp";
		break;
	case WINDOWS_2K:
		strOSType = L"win2000";
		break;
	case WINDOWS_NT:
		strOSType = L"winnt";
		break;
	case WINDOWS_ME:
		strOSType = L"winme";
		break;
	case WINDOWS_98:
		strOSType = L"win98";
		break;
	case WINDOWS_95:
		strOSType = L"win95";
		break;
	}
	return strOSType;
}

// ��鵱ǰ����ϵͳ�Ƿ���ָ����OS�ַ�����Χ��
BOOL DuiSystem::CheckOSName(CString strOS)
{
	CString strCurOSName = GetOSName();
	int nPos = -1;
	while((nPos = strOS.Find(L",")) != -1)
	{
		CString strTemp = strOS.Left(nPos);
		strOS.Delete(0, nPos+1);
		if(strCurOSName.CompareNoCase(strTemp) == 0)
		{
			return TRUE;
		}
	}
	if(!strOS.IsEmpty())
	{
		if(strCurOSName.CompareNoCase(strOS) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// ��ȡ������Ϣ
BOOL DuiSystem::GetFont(CStringA strName, DuiFontInfo& fontInfo)
{
	return m_mapFontPool.Lookup(strName, fontInfo);
}

// ��ȡȱʡ������Ϣ(��������������,���ж���������Ƿ񲻺���,�����ʾͻ��ɿ����õ�)
// Ŀǰ����Լ������ĺ�Ӣ��ϵͳ��Ĭ��������д���
CString DuiSystem::GetDefaultFont(CString strFont)
{
	int nOSVer = DuiSystem::GetOSMajorVersion();
	if(strFont.IsEmpty())
	{
		// ����Ϊ�����ղ���ϵͳ�汾�͵�ǰ���Ծ���Ĭ������
		if(nOSVer >= 6)
		{
			return (GetLanguage() == "zh-cn") ? TEXT("΢���ź�") : TEXT("Segoe UI");
		}else
		{
			return TEXT("Tahoma");
		}
	}else
	if((nOSVer < 6) && ((strFont == _T("΢���ź�")) || (strFont == _T("Segoe UI"))))
	{
		// ����ǵͰ汾����ϵͳ,������΢���ź�
		return TEXT("Tahoma");
	}else
	if(!strFont.IsEmpty())
	{
		// ����������ֱ�ӷ������������
		return strFont;
	}else
	if(nOSVer >= 6)
	{
		// Vista����Ĭ������
		return (GetLanguage() == "zh-cn") ? TEXT("΢���ź�") : TEXT("Segoe UI");
	}

	// �Ͱ汾����ϵͳĬ������
	return TEXT("Tahoma");	
}

// ��ȡ���ڱ�����Ϣ
BOOL DuiSystem::GetWindowBkInfo(int& nType, int& nIDResource, COLORREF& clr, CString& strImgFile)
{
	// �����¼���������Ӧ�û�ȡ����ı�����Ϣ
	CDuiHandler* pDuiHandler = GetDuiHandler(0);
	if(pDuiHandler == NULL)
	{
		return FALSE;
	}

	// ���Ȼ�ȡSkin���ͣ�Ȼ��������ͻ�ȡSkinֵ
	if(pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_GET_SKIN_TYPE, (WPARAM)&nType, 0) != 0)
	{
		if(nType == BKTYPE_IMAGE_RESOURCE)
		{
			return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_GET_SKIN_VALUE, nType, (LPARAM)&nIDResource);
		}else
		if(nType == BKTYPE_COLOR)
		{
			return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_GET_SKIN_VALUE, nType, (LPARAM)&clr);
		}else
		if(nType == BKTYPE_IMAGE_FILE)
		{
			return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_GET_SKIN_VALUE, nType, (LPARAM)&strImgFile);
		}
	}

	return FALSE;
}

// ���ô��ڱ�����Ϣ
BOOL DuiSystem::SetWindowBkInfo(int nType, int nIDResource, COLORREF clr, CString strImgFile)
{
	// �����¼�������󽫱�����Ϣ���浽Ӧ��
	CDuiHandler* pDuiHandler = GetDuiHandler(0);
	if(pDuiHandler == NULL)
	{
		return FALSE;
	}

	if(nType == BKTYPE_IMAGE_RESOURCE)
	{
		return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_SET_SKIN_VALUE, nType, nIDResource);
	}else
	if(nType == BKTYPE_COLOR)
	{
		return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_SET_SKIN_VALUE, nType, (LPARAM)clr);
	}else
	if(nType == BKTYPE_IMAGE_FILE)
	{
		return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_SET_SKIN_VALUE, nType, (LPARAM)(&strImgFile));
	}

	return TRUE;
}

// ���ݿؼ����������ؼ�ʵ��
CControlBase* DuiSystem::CreateControlByName(LPCSTR lpszName, HWND hWnd, CDuiObject* pParentObject)
{
	CControlBase *pControl = NULL;

	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiPanel);

	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiButton);
	CREATE_DUICONTROL_BY_CLASS_NAME(CImageButton);
	CREATE_DUICONTROL_BY_CLASS_NAME(CCheckButton);
	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiRadioButton);
	CREATE_DUICONTROL_BY_CLASS_NAME(CHideButton);
	CREATE_DUICONTROL_BY_CLASS_NAME(CLinkButton);
	CREATE_DUICONTROL_BY_CLASS_NAME(CTextButton);

	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiListCtrl);
	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiGridCtrl);
	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiTreeCtrl);

	CREATE_DUICONTROL_BY_CLASS_NAME(CArea);
	CREATE_DUICONTROL_BY_CLASS_NAME(CFrame);
	CREATE_DUICONTROL_BY_CLASS_NAME(CImageString);
	CREATE_DUICONTROL_BY_CLASS_NAME(CRectangle);
	CREATE_DUICONTROL_BY_CLASS_NAME(CRuning);
	CREATE_DUICONTROL_BY_CLASS_NAME(CScrollV);
	CREATE_DUICONTROL_BY_CLASS_NAME(CSelectBox);

	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiPicture);

	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiProgress);

	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiTabCtrl);

	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiText);

	CREATE_DUICONTROL_BY_CLASS_NAME(CMenuItem);

	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiEdit);
	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiComboBox);

	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiActiveX);
	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiFlashCtrl);
	CREATE_DUICONTROL_BY_CLASS_NAME(CDuiMediaPlayer);

	return NULL;
}

// ���ݿؼ�ID��ȡ�ӿؼ����󣨴�����ע���DUI�Ի����в��ң�
CControlBase* DuiSystem::GetControlFromDuiDialog(UINT uControlID)
{
	for (size_t i = 0; i < m_vecDuiDialog.size(); i++)
	{
		CDlgBase* pDlgBase = m_vecDuiDialog.at(i);
		if (pDlgBase)
		{
			CControlBase* pControlBase = pDlgBase->GetControl(uControlID);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// ���ݿؼ�ID��ȡ�ӿؼ�����
CControlBase* DuiSystem::GetControl(CDuiObject* pDuiObject, UINT uControlID)
{
	if(pDuiObject == NULL)
	{
		return NULL;
	}

	// �����ж��Ƿ������DUI����
	if(pDuiObject->GetID() == uControlID)
	{
		return (CControlBase*)pDuiObject;
	}

	// �ж��Ƿ��²�DUI�ؼ�����
	if(pDuiObject->IsClass("dlg"))
	{
		CDlgBase* pDlgBase = (CDlgBase*)pDuiObject;
		CControlBase* pControlBase = pDlgBase->GetControl(uControlID);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}else
		{
			// ����base control
			pControlBase = pDlgBase->GetBaseControl(uControlID);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}else
	if(pDuiObject->IsClass("popup"))
	{
		CDlgPopup* pDlgPopup = (CDlgPopup*)pDuiObject;
		CControlBase* pControlBase = pDlgPopup->GetControl(uControlID);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}
	}else
	{
		CControlBase* pControl = (CControlBase*)pDuiObject;
		CControlBase* pControlBase = pControl->GetControl(uControlID);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}
	}

	// ���û���ҵ����ٵ�����ע��ĶԻ����в��ң������ID���ҷ�ʽ��
	return DuiSystem::Instance()->GetControlFromDuiDialog(uControlID);
}

// ���ݿؼ�����ȡ�ӿؼ�����
CControlBase* DuiSystem::GetControl(CDuiObject* pDuiObject, CString strControlName)
{
	if(pDuiObject == NULL)
	{
		return NULL;
	}

	// �����ж��Ƿ������DUI����
	if(pDuiObject->GetName() == strControlName)
	{
		return (CControlBase*)pDuiObject;
	}

	// �ж��Ƿ��²�DUI�ؼ�����
	if(pDuiObject->IsClass("dlg"))
	{
		CDlgBase* pDlgBase = (CDlgBase*)pDuiObject;
		CControlBase* pControlBase = pDlgBase->GetControl(strControlName);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}else
		{
			// ����base control
			pControlBase = pDlgBase->GetBaseControl(strControlName);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}else
	if(pDuiObject->IsClass("popup"))
	{
		CDlgPopup* pDlgPopup = (CDlgPopup*)pDuiObject;
		CControlBase* pControlBase = pDlgPopup->GetControl(strControlName);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}
	}else
	{
		CControlBase* pControl = (CControlBase*)pDuiObject;
		CControlBase* pControlBase = pControl->GetControl(strControlName);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}
	}

	return NULL;
}

// ���ݶԻ��������ӿؼ�����ȡ�Ի����е��ӿؼ�����
CControlBase* DuiSystem::GetDialogControl(CString strDialogName, CString strControlName)
{
	CDlgBase* pDuiObject = DuiSystem::Instance()->GetDuiDialog(strDialogName);
	if(pDuiObject == NULL)
	{
		return NULL;
	}

	// �ж��Ƿ��²�DUI�ؼ�����
	CDlgBase* pDlgBase = (CDlgBase*)pDuiObject;
	CControlBase* pControlBase = pDlgBase->GetControl(strControlName);
	if(pControlBase != NULL)
	{
		return pControlBase;
	}else
	{
		// ����base control
		pControlBase = pDlgBase->GetBaseControl(strControlName);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}
	}

	return NULL;
}

// ���DUI�¼��������
void DuiSystem::AddDuiHandler(CDuiHandler* pDuiHandler)
{
	// �ж�����Ѿ���ӹ����Ͳ����ظ����
	for (size_t i = 0; i < m_vecDuiHandler.size(); i++)
	{
		CDuiHandler* _pDuiHandler = m_vecDuiHandler.at(i);
		if (_pDuiHandler == pDuiHandler)
		{
			return;
		}		
	}

	m_vecDuiHandler.push_back(pDuiHandler);
}

// ɾ��DUI�¼��������
BOOL DuiSystem::RemoveDuiHandler(CDuiHandler* pDuiHandler)
{
	vector<CDuiHandler*>::iterator it;
	for(it=m_vecDuiHandler.begin();it!=m_vecDuiHandler.end();++it)
	{
		if(*it == pDuiHandler)
		{
			m_vecDuiHandler.erase(it);
			delete pDuiHandler;
			return TRUE;
		}
	}

	return FALSE;
}

// ��ȡDUI�¼��������(��������)
CDuiHandler* DuiSystem::GetDuiHandler(int nIndex)
{
	if(m_vecDuiHandler.size() == 0)
	{
		return NULL;
	}

	if(nIndex < 0 || nIndex > (m_vecDuiHandler.size() - 1))
	{
		return NULL;
	}
	return m_vecDuiHandler.at(nIndex);
}

// ��ָ����DUI����ע���¼��������
BOOL DuiSystem::RegisterHandler(CDuiObject* pDuiObject, CDuiHandler* pDuiHandler, CString strControlName, BOOL bInit)
{
	if(pDuiObject == NULL || pDuiHandler == NULL)
	{
		return FALSE;
	}

	DuiSystem::Instance()->AddDuiHandler(pDuiHandler);

	if(strControlName.IsEmpty())
	{
		pDuiObject->RegisterHandler(pDuiHandler);
	}else
	{
		CControlBase* pSubControl = DuiSystem::GetControl(pDuiObject, strControlName);
		if(pSubControl == NULL)
		{
			return FALSE;
		}
		pSubControl->RegisterHandler(pDuiHandler);
	}

	// �Ƿ�ע��֮��������ʼ��
	if(bInit)
	{
		pDuiHandler->OnInit();
	}

	return TRUE;
}

// ��������ע����¼����������д���
LRESULT DuiSystem::CallDuiHandler(UINT uID, CString strName, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (size_t i = 0; i < m_vecDuiHandler.size(); i++)
	{
		CDuiHandler* pDuiHandler = m_vecDuiHandler.at(i);
		if (pDuiHandler)
		{
			pDuiHandler->OnDuiMessage(uID, strName, uMsg, wParam, lParam);
		}		
	}

	return 0;
}

// ���DUI�Ի���
void DuiSystem::AddDuiDialog(CDlgBase* pDuiDialog)
{
	m_vecDuiDialog.push_back(pDuiDialog);
}

// ɾ��DUI�Ի���
void DuiSystem::RemoveDuiDialog(CDlgBase* pDuiDialog)
{
	vector<CDlgBase*>::iterator it;
	for(it=m_vecDuiDialog.begin();it!=m_vecDuiDialog.end();++it)
	{
		if(*it == pDuiDialog)
		{
			m_vecDuiDialog.erase(it);
			break;
		}
	}
	if(pDuiDialog != NULL)
	{
		delete pDuiDialog;
	}
}

// ��ȡDUI�Ի���ָ��(��������)
CDlgBase* DuiSystem::GetDuiDialog(int nIndex)
{
	if(nIndex < 0 || nIndex > (m_vecDuiDialog.size() - 1))
	{
		return NULL;
	}
	return m_vecDuiDialog.at(nIndex);
}

// ��ȡDUI�Ի���ָ��(����name)
CDlgBase* DuiSystem::GetDuiDialog(CString strName)
{
	for (size_t i = 0; i < m_vecDuiDialog.size(); i++)
	{
		CDlgBase* pDlgBase = m_vecDuiDialog.at(i);
		if(pDlgBase->GetName() == strName)
		{
			return pDlgBase;
		}
	}
	return NULL;
}

// �������жԻ���ı���Ƥ��
void DuiSystem::ResetAllWindowsBkSkin()
{
	for (size_t i = 0; i < m_vecDuiDialog.size(); i++)
	{
		CDlgBase* pDlgBase = m_vecDuiDialog.at(i);
		pDlgBase->InitWindowBkSkin();
	}
}

// �����Ի���
CDlgBase* DuiSystem::CreateDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule, UINT nIDTemplate, BOOL bAdd)
{
	// ����XMLģ���ļ�������
	CString strXmlContent = lpszXmlTemplate;
	CString strXmlFile = _T("");
	if(strXmlContent.Find(_T("<?xml")) != 0)
	{
		strXmlContent = _T("");
		strXmlFile = lpszXmlTemplate;
		if(strXmlFile.Find(_T(".xml")) == -1)
		{
			strXmlFile = DuiSystem::Instance()->GetXmlFile(CEncodingUtil::UnicodeToAnsi(strXmlFile));
		}else
		if(strXmlFile.Find(_T(":")) == -1)
		{
			strXmlFile = _T("xml:") + strXmlFile;
		}
		if(strXmlFile.IsEmpty())
		{
			return NULL;
		}
	}

	CDlgBase* pParentDlg = NULL;
	if(pParentObject && pParentObject->IsClass("dlg"))
	{
		pParentDlg = ((CDlgBase*)pParentObject);
		if(nIDTemplate == 0)
		{
			nIDTemplate = pParentDlg->GetIDTemplate();
		}
	}
	if(nIDTemplate == 0 && g_nIDTemplate != 0)
	{
		nIDTemplate = g_nIDTemplate;
	}
	if(g_nIDTemplate == 0)
	{
		g_nIDTemplate = nIDTemplate;
	}
	CDlgBase* pDlg = new CDlgBase(nIDTemplate, pParentDlg);
	pDlg->SetParent(pParentObject);
	pDlg->SetXmlFile(strXmlFile);
	pDlg->SetXmlContent(strXmlContent);

	if(!strName.IsEmpty())
	{
		// ����name
		pDlg->SetName(strName);
	}

	if(bAdd)
	{
		// ��ӵ��Ի����б���
		DuiSystem::Instance()->AddDuiDialog(pDlg);
	}

	if(!bModule)
	{
		// ����Ƿ�ģʽ�Ի���,��ֱ�Ӵ�������
		pDlg->Create(pDlg->GetIDTemplate(), NULL);
	}

	return pDlg;
}

// ��ʾ�Ի���
int DuiSystem::ShowDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(lpszXmlTemplate, pParentObject, strName, bModule, 0, TRUE);
	if(pDlg == NULL)
	{
		return 0;
	}

	int nResponse = pDlg->DoModal();
	DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	return nResponse;
}

// ͨ����Ϣ�Ի���
int DuiSystem::DuiMessageBox(CDuiObject* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption, UINT uType, int nWidth, int nHeight)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(_T("dlg_msgbox"), pParent, _T(""), TRUE, 0, TRUE);
	if(pDlg == NULL)
	{
		return 0;
	}

	// �������ݺͱ���
	pDlg->SetControlValue(_T("msgbox.text"), _T("title"), lpszText);
	if(lpszCaption != _T(""))
	{
		pDlg->SetControlValue(_T("msgbox.title"), _T("title"), lpszCaption);
	}

	// ��ť
	UINT uButtonType = uType & 0x0F;
	for(UINT i=0; i< 10; i++)
	{
		CString strButtonDivName;
		strButtonDivName.Format(_T("msgbox.type.%u"), i);
		pDlg->SetControlValue(strButtonDivName, _T("visible"), (uButtonType == i) ? _T("1") : _T("0"));
	}

	// ͼ��
	if((uType & 0xF0) == MB_ICONINFORMATION)
	{
		pDlg->SetControlValue(_T("msgbox.icon"), _T("image"), _T("skin:IDB_ICON_INFO"));
	}else
	if((uType & 0xF0) == MB_ICONQUESTION)
	{
		pDlg->SetControlValue(_T("msgbox.icon"), _T("image"), _T("skin:IDB_ICON_QUESTION"));
	}else
	if((uType & 0xF0) == MB_ICONWARNING)
	{
		pDlg->SetControlValue(_T("msgbox.icon"), _T("image"), _T("skin:IDB_ICON_WARN"));
	}else
	if((uType & 0xF0) == MB_ICONERROR)
	{
		pDlg->SetControlValue(_T("msgbox.icon"), _T("image"), _T("skin:IDB_ICON_ERROR"));
	}else
	{
		pDlg->SetControlValue(_T("msgbox.icon"), _T("image"), _T("skin:IDB_ICON_CHECK"));
	}

	if(nWidth != 0)
	{
		CString strTemp;
		strTemp.Format(_T("%d"), nWidth);
		pDlg->SetControlValue(_T(""), _T("width"), strTemp);
	}
	if(nHeight != 0)
	{
		CString strTemp;
		strTemp.Format(_T("%d"), nHeight);
		pDlg->SetControlValue(_T(""), _T("height"), strTemp);
	}

	int nResponse = pDlg->DoModal();
	DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	return nResponse;
}

// ��̬��ʾ��Ϣ������ӿ�:������̬��Ϣ��ʾ��
void DuiSystem::CreateNotifyMsgBox(LPCTSTR lpszXmlTemplate, CString strName)
{
	if(m_pNotifyMsgBox != NULL)
	{
		RemoveNotifyMsgBox();
	}

	m_pNotifyMsgBox = CreateDuiDialog(lpszXmlTemplate, NULL, strName, FALSE);
	m_pNotifyMsgBox->ShowWindow(SW_HIDE);
}

// ɾ����̬��Ϣ��ʾ��
void DuiSystem::RemoveNotifyMsgBox()
{
	if(m_pNotifyMsgBox != NULL)
	{
		RemoveDuiDialog(m_pNotifyMsgBox);
		m_pNotifyMsgBox = NULL;
	}
}

// ���ö�̬��Ϣ��ʾ��ؼ�����
void DuiSystem::SetNotifyMsgBoxControlTitle(CString strControlName, CString strControlTitle)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		CControlBaseFont* pControl = (CControlBaseFont*)(pNotifyMsgBox->GetControl(strControlName));
		if(pControl == NULL)
		{
			// �������ͨ�ؼ���δ�ҵ�,����һ�»����ؼ�
			pControl = (CControlBaseFont*)(pNotifyMsgBox->GetBaseControl(strControlName));
		}
		if(pControl)
		{
			pControl->SetTitle(strControlTitle);
		}
	}
}

// ���ö�̬��Ϣ��ʾ��ؼ�ͼƬ
void DuiSystem::SetNotifyMsgBoxControlImage(CString strControlName, CString strControlImage)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		CControlBaseFont* pControl = (CControlBaseFont*)(pNotifyMsgBox->GetControl(strControlName));
		if(pControl)
		{
			pControl->SetImage(strControlImage);
		}
	}
}

// ���ö�̬��Ϣ��ʾ��ؼ��ɼ���
void DuiSystem::SetNotifyMsgBoxControlVisible(CString strControlName, BOOL bVisible)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		CControlBase* pControl = pNotifyMsgBox->GetControl(strControlName);
		if(pControl == NULL)
		{
			// �������ͨ�ؼ���δ�ҵ�,����һ�»����ؼ�
			pControl = (CControlBaseFont*)(pNotifyMsgBox->GetBaseControl(strControlName));
		}
		if(pControl)
		{
			pControl->SetVisible(bVisible);
		}
	}
}

// ���ö�̬��Ϣ��ʾ���С
void DuiSystem::SetNotifyMsgBoxSize(int nWidth, int nHeight)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		pNotifyMsgBox->SetMinSize(nWidth, nHeight);
		pNotifyMsgBox->SetRect(CRect(0, 0, nWidth, nHeight));
		::SetWindowPos(pNotifyMsgBox->m_hWnd, NULL, 0, 0, nWidth, nHeight, SWP_SHOWWINDOW);
	}
}

// ��ʾ��̬��Ϣ��ʾ��
void DuiSystem::ShowNotifyMsgBox(UINT uDelayTime)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		int nScreenWidth= GetSystemMetrics(SM_CXFULLSCREEN);
		int nScreenHeight= GetSystemMetrics(SM_CYFULLSCREEN);
		CRect rect;
		pNotifyMsgBox->GetWindowRect(rect);
		/*if(rect.left == 0 && rect.top == 0)
		{
			//������ʾ
			rect.MoveToXY((nScreenWidth-rect.Width()) / 2, (nScreenHeight-rect.Height()) / 2);
			pNotifyMsgBox->MoveWindow(rect, FALSE);
		}*/
		// ��ʾ��Windows���½�
		rect.MoveToXY(nScreenWidth-rect.Width(), nScreenHeight-rect.Height());
		pNotifyMsgBox->MoveWindow(rect, FALSE);
		// �Ѵ��ڷ�����ǰ����ʾ
		::SetForegroundWindow(pNotifyMsgBox->m_hWnd);
		pNotifyMsgBox->ShowWindow(SW_SHOW);
		::BringWindowToTop(pNotifyMsgBox->m_hWnd);
		// �����ö�
		::SetWindowPos(pNotifyMsgBox->m_hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

		// �����Զ��رն�ʱ��(���ش���ģʽ,��Notify��������)
		pNotifyMsgBox->SetAutoCloseTimer(uDelayTime, TRUE);
	}
}

// ���ض�̬��Ϣ��ʾ��
void DuiSystem::HideNotifyMsgBox()
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		pNotifyMsgBox->ShowWindow(SW_HIDE);
	}
}

// ִ�г���
BOOL DuiSystem::ExecuteProcess(CString strProcessName, CString strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess)
{
    BOOL bRet = FALSE;
    PROCESS_INFORMATION processInfo;
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    HANDLE hProcess = NULL;

    if (bForceAdmin)	// ����Ա��ʽ����
    {
        SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };

        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.hwnd = NULL;
        sei.lpVerb = _T("runas");
        sei.lpFile = strProcessName;
        sei.lpParameters = (LPWSTR)(LPCWSTR)strCmdLine;
        sei.nShow = SW_SHOWNORMAL;

        bRet = ::ShellExecuteEx(&sei);

        hProcess = sei.hProcess;
    }else
    {
        bRet = ::CreateProcess(
            strProcessName, 
            (LPWSTR)(LPCWSTR)strCmdLine, 
            NULL, NULL, FALSE, 0, NULL, NULL, &si, &processInfo
            );

        if (bRet)
        {
            ::CloseHandle(processInfo.hThread);
            hProcess = processInfo.hProcess;
        }
    }

    if (bRet)
    {
        if (bWaitProcess)
        {
            ::WaitForSingleObject(hProcess, INFINITE);
        }
        ::CloseHandle(hProcess);
    }

    return bRet;
}

// ִ��UI�߳�����(��һ������ͨ���������ڷ���Ϣ,����������Ϣ����ִ�еķ�ʽʵ�������̵߳��������̵߳�������ת)
int DuiSystem::RunUITask(DuiVision::IBaseTask* pTask, const DuiVision::CTaskMgr* pTaskMgr)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	return pDlg->SendMessage(WM_UI_TASK, (WPARAM)pTask, (LPARAM)pTaskMgr);
}

//
// DUI����������
//
class CDuiActionTask : public DuiVision::IBaseTask
{
public:
	CDuiActionTask(LONG type, UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam, CString strControlName, CString strAction, CDuiObject* pParent)
		: DuiVision::IBaseTask(type), m_uID(uID), m_uMsg(uMsg), m_wParam(wParam), m_lParam(lParam),
		m_pParent(pParent), m_strControlName(strControlName), m_strAction(strAction)
	{
		SetUITask(TRUE);	// ����Ϊ��ҪתUI�̴߳��������
	}

	// ������
	virtual BOOL TaskProcess(DuiVision::CTaskMgr *pMgr)
	{
		DoAction();
		return TRUE;
	}

	void DoAction()
	{
		if(!m_strAction.IsEmpty())
		{
			// ���������action,�����ִ��
			if(m_strAction.Find(_T("dlg:")) == 0)	// ����:��һ���Ի���,���ڴ�й©����Ϊͨ��DuiSystem�����͹���
			{
				if(m_uMsg == BUTTOM_UP)	// ���ſ��¼��Ŵ���
				{
					CString strXmlFile = m_strAction;
					strXmlFile.Delete(0, 4);
					if(!strXmlFile.IsEmpty())
					{
						DuiSystem::ShowDuiDialog(strXmlFile, m_pParent);
					}
				}
			}else
			if(m_strAction.Find(_T("link:")) == 0)	// ����:��һ��ҳ������
			{
				if(m_uMsg == BUTTOM_UP)	// ���ſ��¼��Ŵ���
				{
					CString strLink = m_strAction;
					strLink.Delete(0, 5);
					if(!strLink.IsEmpty())
					{
						ShellExecute(NULL, TEXT("open"), strLink, NULL,NULL,SW_NORMAL);
					}
				}
			}else
			if(m_strAction.Find(_T("run:")) == 0)	// ����:ִ��һ������
			{
				if(m_uMsg == BUTTOM_UP)	// ���ſ��¼��Ŵ���
				{
					CString strProcess = m_strAction;
					strProcess.Delete(0, 4);
					strProcess.MakeLower();
					if(!strProcess.IsEmpty())
					{
						strProcess.MakeLower();
						BOOL bForceAdmin = FALSE;
						if(strProcess.Find(_T("admin@")) == 0)
						{
							bForceAdmin = TRUE;
							strProcess.Delete(0, 6);
						}
						BOOL bWaitProcess = FALSE;
						if(strProcess.Find(_T("&")) == (strProcess.GetLength()-1))
						{
							bWaitProcess = TRUE;
							strProcess.Delete(strProcess.GetLength()-1, 1);
						}
						if(strProcess.Find(_T(".exe")) == -1)
						{
							strProcess = DuiSystem::Instance()->GetString(CEncodingUtil::UnicodeToAnsi(strProcess));
						}
						if(strProcess.Find(_T("{platpath}")) == 0)
						{
							strProcess.Delete(0, 10);
							strProcess = DuiSystem::GetExePath() + strProcess;
						}
						CString strCmdLine = _T("");
						int nPos = strProcess.Find(_T("|"));
						if(nPos != -1)
						{
							strCmdLine = strProcess;
							strCmdLine.Delete(0, nPos+1);
							strProcess = strProcess.Left(nPos);
						}
						DuiSystem::PathCanonicalize(strProcess);	// ·����׼��
						DuiSystem::ExecuteProcess(strProcess, strCmdLine, bForceAdmin, bWaitProcess);
					}
				}
			}else
			if(m_strAction.Find(ACTION_CLOSE_WINDOW) == 0)	// ����:�ر�ָ���Ĵ���
			{
				if(m_uMsg == BUTTOM_UP)	// ���ſ��¼��Ŵ���
				{
					CString strWndName = m_strAction;
					strWndName.Delete(0, 13);
					if(!strWndName.IsEmpty())
					{
						CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(strWndName);
						if(pDlg != NULL)
						{
							//pDlg->PostMessage(WM_QUIT, 0, 0);
							pDlg->DoClose();
						}
					}
				}
			}else
			if(m_strAction.Find(ACTION_SHOW_WINDOW) == 0)	// ����:��ʾָ���Ĵ���
			{
				if(m_uMsg == BUTTOM_UP)	// ���ſ��¼��Ŵ���
				{
					CString strWndName = m_strAction;
					strWndName.Delete(0, 12);
					if(!strWndName.IsEmpty())
					{
						CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(strWndName);
						if(pDlg != NULL)
						{
							pDlg->SetForegroundWindow();
							pDlg->ShowWindow(SW_NORMAL);
							pDlg->ShowWindow(SW_SHOW);
							pDlg->BringWindowToTop();
						}
					}
				}
			}
		}else
		{
			// �ҵ��ؼ�,���ÿؼ�����Ϣ����
			CControlBase* pControl = DuiSystem::GetControl(m_pParent, m_uID);
			if(pControl)
			{
				pControl->CallDuiHandler(m_uMsg, m_wParam, m_lParam);
			}else
			{
				// ���δ�ҵ��ؼ�,��ͨ��DuiSystem��������ע����¼����������д���
				DuiSystem::Instance()->CallDuiHandler(m_uID, m_strControlName, m_uMsg, m_wParam, m_lParam);
			}
		}
	}

protected:
	CDuiObject*	m_pParent;			// ������
	UINT		m_uID;				// �ؼ�ID
	CString		m_strControlName;	// �ؼ���
	UINT		m_uMsg;				// ��Ϣ
	WPARAM		m_wParam;			// ����1
	LPARAM		m_lParam;			// ����2
	CString		m_strAction;		// ����
};

// ���DUI��������
void DuiSystem::AddDuiActionTask(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam, CString strControlName, CString strAction, CDuiObject* pParent)
{
	DuiVision::CTaskMgr* pTaskMgr = DuiSystem::Instance()->GetTaskMgr();
	if(pTaskMgr)
	{
		pTaskMgr->AddTask(new CDuiActionTask(1, uID, uMsg, wParam, lParam, strControlName, strAction, pParent));
		pTaskMgr->StartTask();
	}
}

//
// ��ʾ��Ϣ������
//
class CDuiNotifyMsgTask : public DuiVision::IBaseTask
{
public:
	CDuiNotifyMsgTask(LONG type, CString strMsg, UINT uNotifyType = MB_ICONINFORMATION | 2, CString strCaption = _T(""), int nDelayTime = 0, int nWidth = 0, int nHeight = 0)
		: DuiVision::IBaseTask(type), m_strMsg(strMsg), m_uNotifyType(uNotifyType), m_strCaption(strCaption),
		m_nDelayTime(nDelayTime), m_nWidth(nWidth), m_nHeight(nHeight)
	{
		SetUITask(TRUE);	// ����Ϊ��ҪתUI�̴߳��������
	}

	// ������
	virtual BOOL TaskProcess(DuiVision::CTaskMgr *pMgr)
	{
		DoAction();
		return TRUE;
	}

	void DoAction()
	{
		// ÿ�ζ���ɾ���ɵĴ���,���´���һ��,���򴰿��������رչ��Ļ�,��Ӧ��Ϣ��������
		DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));

		// �������ݺͱ���
		DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.text"), m_strMsg);
		if(m_strCaption != _T(""))
		{
			DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.title"), m_strCaption);
		}

		// ��ť
		UINT uButtonType = m_uNotifyType & 0x0F;
		for(UINT i=0; i< 10; i++)
		{
			CString strButtonDivName;
			strButtonDivName.Format(_T("notify.type.%u"), i);
			DuiSystem::SetNotifyMsgBoxControlVisible(strButtonDivName, (uButtonType == i) ? TRUE : FALSE);
		}

		// ͼ��
		if((m_uNotifyType & 0xF0) == MB_ICONINFORMATION)
		{
			DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_INFO"));
		}else
		if((m_uNotifyType & 0xF0) == MB_ICONQUESTION)
		{
			DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_QUESTION"));
		}else
		if((m_uNotifyType & 0xF0) == MB_ICONWARNING)
		{
			DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_WARN"));
		}else
		if((m_uNotifyType & 0xF0) == MB_ICONERROR)
		{
			DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_ERROR"));
		}else
		{
			DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_CHECK"));
		}

		if((m_nWidth != 0) && (m_nHeight != 0))
		{
			DuiSystem::SetNotifyMsgBoxSize(m_nWidth, m_nHeight);
		}

		DuiSystem::ShowNotifyMsgBox(m_nDelayTime);
	}

protected:
	CString		m_strXmlTemplate;	// XMLģ��
	UINT		m_uNotifyType;		// ��ʾ��ͼ�������
	CString		m_strMsg;			// ��ʾ��Ϣ
	CString		m_strCaption;		// ����
	int			m_nCheck;			// �����ֵ
	int			m_nWidth;			// ���ڿ��
	int			m_nHeight;			// ���ڸ߶�
	int			m_nDelayTime;		// �ӳ�ʱ��(��)
};

// �����ʾ��Ϣ����
void DuiSystem::AddDuiNotifyMsgTask(CString strMsg, UINT uNotifyType, CString strCaption, int nDelayTime, int nWidth, int nHeight)
{
	DuiVision::CTaskMgr* pTaskMgr = DuiSystem::Instance()->GetTaskMgr();
	if(pTaskMgr)
	{
		pTaskMgr->AddTask(new CDuiNotifyMsgTask(2, strMsg, uNotifyType, strCaption, nDelayTime, nWidth, nHeight));
		pTaskMgr->StartTask();
	}
}

// ����������
CDlgPopup* DuiSystem::CreateDuiPopupWnd(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, UINT nIDTemplate, BOOL bAdd)
{
	CString strXmlFile = lpszXmlTemplate;
	if(strXmlFile.Find(_T(".xml")) == -1)
	{
		strXmlFile = DuiSystem::Instance()->GetXmlFile(CEncodingUtil::UnicodeToAnsi(strXmlFile));
	}else
	if(strXmlFile.Find(_T(":")) == -1)
	{
		strXmlFile = _T("xml:") + strXmlFile;
	}
	if(strXmlFile.IsEmpty())
	{
		return NULL;
	}

	CDlgBase* pParentDlg = NULL;
	if(pParentObject && pParentObject->IsClass("dlg"))
	{
		pParentDlg = ((CDlgBase*)pParentObject);
		if(nIDTemplate == 0)
		{
			nIDTemplate = pParentDlg->GetIDTemplate();
		}
	}
	if(nIDTemplate == 0 && g_nIDTemplate != 0)
	{
		nIDTemplate = g_nIDTemplate;
	}
	if(g_nIDTemplate == 0)
	{
		g_nIDTemplate = nIDTemplate;
	}
	/*
	CDlgPopup* pPopup = new CPopupList(nIDTemplate, pParentDlg);
	pPopup->SetParent(pParentObject);
	pPopup->SetXmlFile(strXmlFile);
	if(bAdd)
	{
		//DuiSystem::Instance()->AddDuiDialog(pDlg);
	}*/

	return NULL;
}

// ��Ӷ�ʱ��
UINT DuiSystem::AddDuiTimer(UINT uTimerResolution, CString strTimerName, BOOL bAppTimer)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return 0;
	}

	return ((CTimer*)pDlg)->SetTimer(uTimerResolution, strTimerName, bAppTimer);
}

// ɾ����ʱ��(����IDɾ��)
void DuiSystem::RemoveDuiTimer(UINT uTimerID)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return;
	}

	((CTimer*)pDlg)->KillTimer(uTimerID);
}

// ɾ����ʱ��(��������ɾ��)
void DuiSystem::RemoveDuiTimer(CString strTimerName)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return;
	}

	((CTimer*)pDlg)->KillTimer(strTimerName);
}

// �����¼��������Ķ�ʱ��������
void DuiSystem::CallDuiHandlerTimer(UINT uTimerID, CString strTimerName)
{
	for (size_t i = 0; i < m_vecDuiHandler.size(); i++)
	{
		CDuiHandler* pDuiHandler = m_vecDuiHandler.at(i);
		if (pDuiHandler)
		{
			pDuiHandler->OnTimer(uTimerID, strTimerName);
		}		
	}
}

// ��ʼ������ͼ��
BOOL DuiSystem::InitTray(CDuiHandler* pDuiHandler, CString strIcon, CString strTip, CString strMenuXml)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	pDlg->SetTrayHandler(pDuiHandler);
	pDlg->SetTratMenuXml(strMenuXml);

	m_NotifyIconData.cbSize = NOTIFYICONDATAA_V1_SIZE;
	m_NotifyIconData.hWnd = pDlg->m_hWnd;
	m_NotifyIconData.uID = 1;
	m_NotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_NotifyIconData.uCallbackMessage = WM_SYSTEM_TRAYICON;

	CStringA strTrayIcon = DuiSystem::Instance()->GetSkin("IDB_TRAY_ICON");
	if(!strIcon.IsEmpty())
	{
		strTrayIcon = CEncodingUtil::UnicodeToAnsi(strIcon);
	}
	if (strTrayIcon.IsEmpty()) return FALSE;

	// �ͷ�ͼ����Դ
	if(m_NotifyIconData.hIcon != NULL)
	{
		DestroyIcon(m_NotifyIconData.hIcon);
		m_NotifyIconData.hIcon = NULL;
	}

	if(strTrayIcon.Find(".") != -1)	// ����ͼ���ļ�
	{
		CString strIconFile = DuiSystem::GetSkinPath() + CEncodingUtil::AnsiToUnicode(strTrayIcon);
		WORD wIndex = 0;
		m_NotifyIconData.hIcon = ::ExtractAssociatedIcon(m_hInst, strIconFile.GetBuffer(0), &wIndex);
	}else	// ����ͼ����Դ
	{
		UINT nResourceID = atoi(strTrayIcon);
		m_NotifyIconData.hIcon = AfxGetApp()->LoadIcon(nResourceID);
	}

	CString sWindowText = DuiSystem::Instance()->GetString("APP_NAME");
	if(!strTip.IsEmpty())
	{
		sWindowText = strTip;
	}
	_tcscpy_s(m_NotifyIconData.szTip, sWindowText);

	BOOL bRet = FALSE;
	int nRetry = 3;
	while(((bRet = Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData)) != TRUE) && (nRetry > 0))
	{
		// ʧ�����������3��
		nRetry--;
		Sleep(100);
	}

	return bRet;
}

// ɾ������ͼ��
void DuiSystem::DelTray()
{
	if(m_NotifyIconData.cbSize)
	{
		// �ͷ�ͼ����Դ
		if(m_NotifyIconData.hIcon != NULL)
		{
			DestroyIcon(m_NotifyIconData.hIcon);
		}
		Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);
		ZeroMemory(&m_NotifyIconData, sizeof m_NotifyIconData);
	}
}

// ��������ͼ��
BOOL DuiSystem::SetTrayIcon(CString strIcon)
{
	if(m_NotifyIconData.cbSize)
	{
		m_NotifyIconData.uFlags = NIF_ICON;

		// �ͷ�ͼ����Դ
		if(m_NotifyIconData.hIcon != NULL)
		{
			DestroyIcon(m_NotifyIconData.hIcon);
			m_NotifyIconData.hIcon = NULL;
		}

		if(strIcon.Find(_T(":")) == -1)
		{
			if(strIcon.Find(_T(".")) != -1)	// ����ͼ���ļ�(skinsĿ¼�µ�)
			{
				CString strIconFile = DuiSystem::GetSkinPath() + strIcon;
				WORD wIndex = 0;
				m_NotifyIconData.hIcon = ::ExtractAssociatedIcon(m_hInst, strIconFile.GetBuffer(0), &wIndex);
			}else	// ����ͼ����Դ
			{
				UINT nResourceID = _wtoi(strIcon);
				m_NotifyIconData.hIcon = AfxGetApp()->LoadIcon(nResourceID);
			}
		}else
		{
			// ����ͼ���ļ�(����·��)
			CString strIconFile = strIcon;
			WORD wIndex = 0;
			m_NotifyIconData.hIcon = ::ExtractAssociatedIcon(m_hInst, strIconFile.GetBuffer(0), &wIndex);
		}

		BOOL bRet = FALSE;
		int nRetry = 10;
		while(((bRet = Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData)) != TRUE) && (nRetry > 0))
		{
			// ʧ�����������10��
			nRetry--;
		}

		return bRet;
	}

	return FALSE;
}

// �������̵�Tip��Ϣ
BOOL DuiSystem::SetTrayTip(CString szToolTip)
{
	if(m_NotifyIconData.cbSize)
	{
		m_NotifyIconData.uFlags = NIF_MESSAGE | NIF_TIP;
		CString sWindowText = DuiSystem::Instance()->GetString("APP_NAME");
		if(szToolTip != _T(""))
		{
			sWindowText += _T("\n");
			sWindowText += szToolTip;
		}
		_tcscpy_s(m_NotifyIconData.szTip, sWindowText);
		return Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData);
	}
	return FALSE;
}

// ���ͽ��̼�ͨ����Ϣ
BOOL DuiSystem::SendInterprocessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, CString strInfo)
{
	// number of characters in memory-mapped file
	const DWORD dwMemoryFileSize = sizeof(DUI_INTERPROCESS_MSG);

	// memory-mapped file name
	const LPCTSTR sMemoryFileName = _T("DF034858-1608-4147-0604-4A0CD86F6C9F");

	HANDLE hFileMapping = NULL;
	LPVOID pViewOfFile = NULL;

	// Create file mapping which can contain dwMemoryFileSize characters
	hFileMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,           // system paging file
		NULL,                           // security attributes
		PAGE_READWRITE,                 // protection
		0,                              // high-order DWORD of size
		dwMemoryFileSize*sizeof(TCHAR), // low-order DWORD of size
		sMemoryFileName);               // name

	DWORD dwError = GetLastError();     // if ERROR_ALREADY_EXISTS 
	// this instance is not first (other instance created file mapping)

	if(! hFileMapping)
	{
		//TRACE(_T("Creating of file mapping failed.\n"));
		return FALSE;
	}

	pViewOfFile = MapViewOfFile(
		hFileMapping,               // handle to file-mapping object
		FILE_MAP_ALL_ACCESS,        // desired access
		0,
		0,
		0);                         // map all file

	if(!pViewOfFile)
	{
		//TRACE(_T("MapViewOfFile failed.\n"));
		CloseHandle(hFileMapping);
		return FALSE;
	}

	// ��ʼ�����̼�֪ͨ��Ϣ
	DUI_INTERPROCESS_MSG interMsg;
	memset(&interMsg, 0, sizeof(DUI_INTERPROCESS_MSG));
	interMsg.uControlID = APP_IPC;	// �ؼ�IDʹ��Ԥ�������̼���Ϣ�ؼ�ID
	interMsg.uMsg = uMsg;
	interMsg.wParam = wParam;
	interMsg.lParam = lParam;
	wcscpy_s(interMsg.wInfo, strInfo.GetBuffer(0));
	strInfo.ReleaseBuffer();
	memcpy(pViewOfFile, &interMsg, sizeof(DUI_INTERPROCESS_MSG));

	DWORD result;
	LRESULT ok = SendMessageTimeout(HWND_BROADCAST,	// ���͹㲥��Ϣ
		WM_CHECK_ITS_ME,
		wParam,			// wParam����,��ʾӦ������
		0,				// lParam����,Ĭ��Ϊ0
		SMTO_BLOCK |
		SMTO_ABORTIFHUNG,
		200,			// ��ʱʱ��
		&result);

	UnmapViewOfFile(pViewOfFile);
	CloseHandle(hFileMapping);

	return TRUE;
}

// ��־��ʼ��
void DuiSystem::InitLog()
{
	// ��ʼ����־�ļ�·������
	CString strLogFile = GetConfig("logfile");
	if(strLogFile.IsEmpty())
	{
		return;
	}
	m_strLogFile = GetExePath() + strLogFile;
	m_nLogLevel = _wtoi(GetConfig("loglevel"));
	m_bLogEnable = TRUE;
	InitializeCriticalSection(&m_WriteLogMutex);

	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, L"------------------DuiVision Start-------------------");
}

// ������־
void DuiSystem::DoneLog()
{
	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, L"------------------DuiVision End-------------------");
}

// ��¼��־
void DuiSystem::LogEvent(int nLevel, LPCWSTR lpFormat, ...)
{
	if(!DuiSystem::Instance()->IsLogEnable())
	{
		return;
	}

	if(nLevel < DuiSystem::Instance()->GetLogLevel())
	{
		return;
	}

	EnterCriticalSection(DuiSystem::Instance()->GetLogMutex());

	const int nBufLen = MAX_PATH * 2;
	WCHAR szBuf[nBufLen];
	va_list ap;

	va_start(ap, lpFormat);
	_vsnwprintf(szBuf, nBufLen, lpFormat, ap);
	szBuf[nBufLen - 1] = 0;

	FILE* lpFile = _tfopen(DuiSystem::Instance()->GetLogFile(), _T("a+"));
	if ( lpFile != NULL )
	{
		// ��ȡ�ļ���С
		struct _stat FileBuff;
		int nResult = -1;

		nResult = _wstat(DuiSystem::Instance()->GetLogFile(), &FileBuff);
		if (0 != nResult)
		{
			LeaveCriticalSection(DuiSystem::Instance()->GetLogMutex());
			return;
		}

		long lSize = FileBuff.st_size;

		// �ļ������趨��С��Ҫ����ת����Ĭ�����1MB
		if (lSize > MAXLOGFILESIZE)
		{
			fclose(lpFile);

			// ɾ�������ļ�
			_unlink(CEncodingUtil::UnicodeToAnsi(DuiSystem::Instance()->GetLogFile() + _T(".bak")));

			// �������ļ���Ϊ�����ļ���
			rename(CEncodingUtil::UnicodeToAnsi(DuiSystem::Instance()->GetLogFile()), CEncodingUtil::UnicodeToAnsi(DuiSystem::Instance()->GetLogFile() + _T(".bak")));

			// �����ļ�
			lpFile = _tfopen(DuiSystem::Instance()->GetLogFile(), _T("w+"));//,ccs=UTF-8"));
			if (lpFile == NULL)
			{
				LeaveCriticalSection(DuiSystem::Instance()->GetLogMutex());
				return;
			}
		}

		SYSTEMTIME st;
		GetLocalTime(&st);

		CString strLevel;
		if (nLevel == LOG_LEVEL_DEBUG)
		{
			strLevel = __DEBUG;
		}
		else if (nLevel == LOG_LEVEL_INFO)
		{
			strLevel = __INFO;
		}
		else if (nLevel == LOG_LEVEL_ERROR)
		{
			strLevel = __ERROR;
		}
		else if (nLevel == LOG_LEVEL_CRITICAL)
		{
			strLevel = __CRITICAL;
		}
		else
		{
			strLevel = __DEBUG;
		}

		LPCTSTR lpStr = _tcschr(lpFormat, _T('\n'));
		if ( lpStr != NULL )
		{
			lpStr = _T("%s %02d-%02d-%02d %02d:%02d:%02d[%u] : %s");
		}
		else
		{
			lpStr = _T("%s %02d-%02d-%02d %02d:%02d:%02d[%u] : %s\n");
		}

		DWORD dwCurThreadID = GetCurrentThreadId();

		_ftprintf(lpFile, lpStr, strLevel, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, dwCurThreadID, szBuf);
		fclose(lpFile);
	}

	va_end(ap);

	LeaveCriticalSection(DuiSystem::Instance()->GetLogMutex());
}


/*
HRESULT DuiSystem::CreateTextServices( IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk )
{
	if(!m_funCreateTextServices) return E_NOTIMPL;
	return m_funCreateTextServices(punkOuter,pITextHost,ppUnk);
}
*/