#pragma once

//////////////////////////////////////////////////////////////////////////////////
// DuiVision Class Name Declaration
#define DUIOBJ_DECLARE_CLASS_NAME(theclass, classname)   \
public:                                                 \
    static CControlBase* CheckAndNew(LPCSTR lpszName, HWND hWnd, CDuiObject* pDuiObject)       \
    {                                                   \
        if (strcmp(GetClassName(), lpszName)  == 0)     \
		{\
			CControlBase* pControl = new theclass(hWnd, pDuiObject);							\
			return pControl;                        \
		}\
        else                                            \
            return NULL;                                \
    }                                                   \
                                                        \
    static LPCSTR GetClassName()                        \
    {                                                   \
        return classname;                               \
    }                                                   \
                                                        \
    virtual LPCSTR GetObjectClass()                     \
    {                                                   \
        return classname;                               \
    }                                                   \
                                                        \
    virtual BOOL IsClass(LPCSTR lpszName)               \
    {                                                   \
        if(strcmp(GetClassName(), lpszName)  == 0) return TRUE;  \
		return __super::IsClass(lpszName);				\
    }                                                   \
														\
	virtual LPCSTR BaseObjectClassName()				\
	{													\
		return __super::GetObjectClass();				\
	}													\


//////////////////////////////////////////////////////////////////////////
// Xml Attributes Declaration Map

// Attribute Declaration
#define DUI_DECLARE_ATTRIBUTES_BEGIN()                            \
public:                                                             \
    virtual HRESULT SetAttribute(                                   \
        CStringA strAttribName,                                     \
        CStringA strValue,                                          \
        BOOL     bLoading)                                          \
    {                                                               \
        HRESULT hRet = __super::SetAttribute(                       \
            strAttribName,                                          \
            strValue,                                               \
            bLoading                                                \
            );                                                      \
        if (SUCCEEDED(hRet))                                        \
            return hRet;                                            \

#define DUI_DECLARE_ATTRIBUTES_END()                              \
            return E_FAIL;                                          \
                                                                    \
        return hRet;                                                \
    }                                                               \

#define DUI_CHAIN_ATTRIBUTE(varname, allredraw)                   \
        if (SUCCEEDED(hRet = varname.SetAttribute(strAttribName, strValue, bLoading)))   \
        {                                                           \
            return hRet;                                            \
        }                                                           \
        else                                                        \

// Custom Attribute
// HRESULT OnAttributeChange(CStringA& strValue, BOOL bLoading)
#define DUI_CUSTOM_ATTRIBUTE(attribname, func)                    \
        if (attribname == strAttribName)                            \
        {                                                           \
            hRet = func(strValue, bLoading);                        \
        }                                                           \
        else                                                        \

// Int = %d StringA
#define DUI_INT_ATTRIBUTE(attribname, varname, allredraw)         \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = ::StrToIntA(strValue);                        \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// UInt = %u StringA
#define DUI_UINT_ATTRIBUTE(attribname, varname, allredraw)        \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = (UINT)::StrToIntA(strValue);                  \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// DWORD = %u StringA
#define DUI_DWORD_ATTRIBUTE(attribname, varname, allredraw)       \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = (DWORD)::StrToIntA(strValue);                 \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// StringA = StringA
#define DUI_STRING_ATTRIBUTE(attribname, varname, allredraw)      \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = strValue;                                     \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// StringT = StringA
#define DUI_TSTRING_ATTRIBUTE(attribname, varname, allredraw)     \
        if (attribname == strAttribName)                            \
        {                                                           \
			ParseDuiString(strValue);	\
            varname = CA2T(strValue, CP_UTF8);                      \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// DWORD = %X StringA
#define DUI_HEX_ATTRIBUTE(attribname, varname, allredraw)         \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = CDuiObject::HexStringToULong(strValue);        \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// Color = %08X StringA
#define DUI_COLOR_ATTRIBUTE(attribname, varname, allredraw)       \
        if (attribname == strAttribName)                            \
        {                                                           \
			if(strValue.Find(",") == -1)	\
			{	\
				varname = CDuiObject::HexStringToColor(strValue);        \
			}else	\
			{	\
				varname = CDuiObject::StringToColor(strValue);        \
			}	\
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// COLORREF = %08X StringA
#define DUI_RGBCOLOR_ATTRIBUTE(attribname, varname, allredraw)       \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = CDuiObject::HexStringToRGBColor(strValue);        \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// HFONT = %04X StringA
#define DUI_FONT_ATTRIBUTE(attribname, varname, allredraw)        \
        if (attribname == strAttribName)                            \
        {                                                           \
            DWORD dwValue = CDuiObject::HexStringToULong(strValue);  \
            varname = DuiSystem::GetFont(LOWORD(dwValue));         \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// Value In {String1 : Value1, String2 : Value2 ...}
#define DUI_ENUM_ATTRIBUTE(attribname, vartype, allredraw)        \
        if (attribname == strAttribName)                            \
        {                                                           \
            vartype varTemp;                                        \
                                                                    \
            hRet = allredraw ? S_OK : S_FALSE;                      \

#define DUI_ENUM_VALUE(enumstring, enumvalue)                     \
            if (strValue == enumstring)                             \
                varTemp = enumvalue;                                \
            else                                                    \

#define DUI_ENUM_END(varname)                                     \
                return E_FAIL;                                      \
                                                                    \
            varname = varTemp;                                      \
        }                                                           \
        else                                                        \

// BkStyle From StringA Key
#define DUI_STYLE_ATTRIBUTE(attribname, varname, allredraw)       \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = DuiSystem::GetStyle(strValue);                  \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// BkSkin From StringA Key
#define DUI_SKIN_ATTRIBUTE(attribname, varname, allredraw)        \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = DuiSystem::GetSkin(strValue);                    \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else   



class CControlBase;
class CDuiHandler;

// DUI����
class CDuiObject
{
public:
	CDuiObject(void);
	virtual ~CDuiObject(void);

	virtual BOOL IsClass(LPCSTR lpszName) { return FALSE; }	// �Ƿ����
	virtual LPCSTR GetObjectClass() { return NULL; }		// ��ȡ����
	virtual LPCSTR BaseObjectClassName() { return NULL; }	// ��ȡ������

	UINT GetID() {return m_uID;};							// ��ȡDUI����ID
	void SetName(CString strName) { m_strName = strName; }	// ����DUI��������
	CString GetName() {return m_strName;};					// ��ȡDUI��������
	BOOL IsThisObject(UINT uID, CString strName);			// �ж��Ƿ�˶���

	BOOL RegisterHandler(CDuiHandler* pDuiHandler);			// ע���¼��������
	void SetDuiHandler(CDuiHandler* pDuiHandler) { m_pDuiHandler = pDuiHandler;	}	// �������¼��������
	CDuiHandler* GetDuiHandler() { return m_pDuiHandler; }	// ��ȡ�¼��������

	virtual LRESULT OnBaseMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0L; };
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL) { return 0L; };
	
	virtual HRESULT SetAttribute(CStringA strAttribName, CStringA strValue, BOOL bLoading);
	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);

	virtual void SetRect(CRect rc) { m_rc = rc;};
	virtual CRect GetRect() { return m_rc;};

	static void ParseDuiString(CStringA& strString);
	static ULONG HexStringToULong(LPCSTR lpszValue, int nSize = -1);
    static Color HexStringToColor(LPCSTR lpszValue);
	static Color StringToColor(LPCSTR lpszValue);
	static COLORREF HexStringToRGBColor(LPCSTR lpszValue);
	static void ParseKeyCode(LPCSTR lpszValue, UINT& nChar, UINT& nFlag);

protected:
	UINT	m_uID;					// DUI����ID
	CString	m_strName;				// DUI��������
	CRect	m_rc;					// ����
	CDuiHandler* m_pDuiHandler;		// �¼��������
};
