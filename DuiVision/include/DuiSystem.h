#pragma once

#include "duisingleton.h"
#include  <afxtempl.h>

#define CREATE_DUICONTROL_BY_CLASS_NAME(classname)   \
	pControl = classname::CheckAndNew(lpszName, hWnd, pParentObject);	\
    if (pControl)	\
        return pControl;	\


// ������ID����
#define LANGUAGE_PAGE_ENGLISH		0x0409	// Ӣ��
#define LANGUAGE_PAGE_CHINESE		0x0804	// ���ļ���
#define LANGUAGE_PAGE_CHINESE_TW	0x0404	// ̨�己��
#define LANGUAGE_PAGE_CHINESE_HK	0x0C04	// ��ۡ���������
#define LANGUAGE_PAGE_CHINESE_SGP	0x1004	// �¼��·���

//��־��Ϣ�ȼ�
#define LOG_LEVEL_DEBUG 0x0001      //������Ϣ
#define LOG_LEVEL_INFO	0x0002      //һ����Ϣ
#define LOG_LEVEL_ERROR 0x0004      //������Ϣ
#define LOG_LEVEL_CRITICAL 0x0008	//������Ϣ

// ����ϵͳ���Ͷ���
enum OSType
{
	OS_UNKNOWN		= -1,
    WINDOWS_WIN32	= 100,
    WINDOWS_95,
    WINDOWS_98,
    WINDOWS_ME,
    WINDOWS_NT,
    WINDOWS_2K,
    WINDOWS_XP,
    WINDOWS_2003,
    WINDOWS_VISTA,
	WINDOWS_7,
	WINDOWS_8,
    WINDOWS_HIGH
};

// ����ϵͳ�������Ͷ���
enum OSPack
{
	PACK_UNKNOWN	= -1,
    SP1				= 1,
    SP2, 
    SP3,
    SP4,
    SP5,
    SP6,
    SP7,
    SP8
 };

class CDlgPopup;
class CTaskMgr;

class DuiSystem : public Singleton<DuiSystem>
{

public:
    DuiSystem(HINSTANCE hInst, DWORD dwLangID = 0, CString strResourceFile = _T(""), UINT uAppID = 0, UINT nIDTemplate = 0, CStringA strStyle = "");
    ~DuiSystem(void);

	static DuiSystem* Instance();
	static void Release();

	void SetInstance(HINSTANCE hInst) { m_hInst = hInst; }
    HINSTANCE GetInstance() { return m_hInst; }

	// ��ȡ����ϵͳ�汾�����汾��
	static int GetOSMajorVersion();
	// ��ȡ��ǰ����(�ַ���)
	static CStringA GetLanguage();
	// ��ȡ��ǰ����(ID)
	DWORD GetCurrentLanguage();
	// ���õ�ǰ����(ID)
	void SetCurrentLanguage(DWORD dwLangID);
	// ��ȡAppID
	UINT GetAppID() { return m_uAppID; }
	// ��ȡ����·��
	static CString GetExePath();
	// ��ȡSkin·��
	static CString GetSkinPath();
	// ��ȡXML·��
	static CString GetXmlPath();
	// ·����׼��
	static BOOL PathCanonicalize(CString& strPath);
	// ������Դ
	BOOL LoadResource();
	// ����XML��Դ�ļ�(����ָ�����صķ��)
	BOOL LoadResourceXml(CString strResFile, CStringA strStyleA = "");
	// ��ȡϵͳ������Ϣ
	CString GetConfig(CStringA strName);
	// ��ȡXML�ļ�
	CString GetXmlFile(CStringA strName);
	// ��ȡSkin
	CStringA GetSkin(CStringA strName);
	// ��ȡ�ַ���ֵ
	CString GetString(CStringA strName);
	// �����ַ���ֵ
	void SetString(CStringA strName, CString strValue);
	// �����ַ������滻���е��滻����
	void ParseDuiString(CStringA& strString);
	// ��ȡ����ϵͳ����
	CString GetOSName();
	// ��鵱ǰ����ϵͳ�Ƿ���ָ����OS�ַ�����Χ��
	BOOL CheckOSName(CString strOS);
	// ��ȡ������Ϣ
	BOOL GetFont(CStringA strName, DuiFontInfo& fontInfo);
	// ��ȡȱʡ������Ϣ(��������������,���ж���������Ƿ񲻺���,�����ʾͻ��ɿ����õ�)
	static CString GetDefaultFont(CString strFont = _T(""));
	// ��ȡ���ڱ�����Ϣ
	BOOL GetWindowBkInfo(int& nType, int& nIDResource, COLORREF& clr, CString& strImgFile);
	// ���ô��ڱ�����Ϣ
	BOOL SetWindowBkInfo(int nType, int nIDResource, COLORREF clr, CString strImgFile);

	// ���ݿؼ����������ؼ�ʵ��
	static CControlBase* CreateControlByName(LPCSTR lpszName, HWND hWnd, CDuiObject* pParentObject);

	// ��ȡ�ӿؼ�����
	CControlBase* GetControlFromDuiDialog(UINT uControlID);
	static CControlBase* GetControl(CDuiObject* pDuiObject, UINT uControlID);
	static CControlBase* GetControl(CDuiObject* pDuiObject, CString strControlName);
	static CControlBase* GetDialogControl(CString strDialogName, CString strControlName);

	// ��ָ����DUI����ע���¼��������
	static BOOL RegisterHandler(CDuiObject* pDuiObject, CDuiHandler* pDuiHandler, CString strControlName = _T(""), BOOL bInit = FALSE);
	void AddDuiHandler(CDuiHandler* pDuiHandler);
	BOOL RemoveDuiHandler(CDuiHandler* pDuiHandler);
	CDuiHandler* GetDuiHandler(int nIndex);
	LRESULT CallDuiHandler(UINT uID, CString strName, UINT uMsg, WPARAM wParam, LPARAM lParam);
	vector<CDuiHandler*>* GetDuiHandlerVect() { return &m_vecDuiHandler; }

	// �Ի����������
	static CDlgBase* CreateDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName = _T(""), BOOL bModule = TRUE, UINT nIDTemplate = 0, BOOL bAdd = TRUE);
	static int ShowDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName = _T(""), BOOL bModule = TRUE);
	static int DuiMessageBox(CDuiObject* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption = _T(""), UINT uType = MB_OK|MB_ICONINFORMATION, int nWidth = 0, int nHeight = 0);
	void AddDuiDialog(CDlgBase* pDuiDialog);
	void RemoveDuiDialog(CDlgBase* pDuiDialog);
	CDlgBase* GetDuiDialog(int nIndex);
	CDlgBase* GetDuiDialog(CString strName);
	void ResetAllWindowsBkSkin();

	// ��̬��ʾ��Ϣ��
	CDlgBase* GetNotifyMsgBox() { return m_pNotifyMsgBox; }
	void CreateNotifyMsgBox(LPCTSTR lpszXmlTemplate, CString strName = _T(""));
	void RemoveNotifyMsgBox();
	static void SetNotifyMsgBoxControlTitle(CString strControlName, CString strControlTitle);
	static void SetNotifyMsgBoxControlImage(CString strControlName, CString strControlImage);
	static void SetNotifyMsgBoxControlVisible(CString strControlName, BOOL bVisible);
	static void SetNotifyMsgBoxSize(int nWidth, int nHeight);
	static void ShowNotifyMsgBox(UINT uDelayTime = 0);
	static void HideNotifyMsgBox();
	static void AddDuiNotifyMsgTask(CString strMsg, UINT uNotifyType = MB_ICONINFORMATION | 2, CString strCaption = _T(""), int nDelayTime = 0, int nWidth = 0, int nHeight = 0);

	// ����������
	static CDlgPopup* CreateDuiPopupWnd(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, UINT nIDTemplate = 0, BOOL bAdd = FALSE);

	// ����¼����������
	static BOOL ExecuteProcess(CString strProcessName, CString strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess);
	static int RunUITask(DuiVision::IBaseTask* pTask, const DuiVision::CTaskMgr* pTaskMgr);
	DuiVision::CTaskMgr* GetTaskMgr() { return &m_TaskMsg; }
	static void AddDuiActionTask(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam, CString strControlName, CString strAction, CDuiObject* pParent);

	// ��ʱ��������
	static UINT AddDuiTimer(UINT uTimerResolution, CString strTimerName = _T(""), BOOL bAppTimer = TRUE);
	static void RemoveDuiTimer(UINT uTimerID);
	static void RemoveDuiTimer(CString strTimerName);
	void CallDuiHandlerTimer(UINT uTimerID, CString strTimerName);

	// ���̲�������
	BOOL InitTray(CDuiHandler* pDuiHandler = NULL, CString strIcon = _T(""), CString strTip = _T(""), CString strMenuXml = _T(""));
	void DelTray();
	BOOL SetTrayIcon(CString strIcon);
	BOOL SetTrayTip(CString szToolTip);

	// ���̼���Ϣ����
	BOOL SendInterprocessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, CString strInfo);

	// ��־����
	CString GetLogFile() { return m_strLogFile; }
	int GetLogLevel() { return m_nLogLevel; }
	BOOL IsLogEnable() { return m_bLogEnable; }
	CRITICAL_SECTION* GetLogMutex() { return &m_WriteLogMutex; }
	void InitLog();
	void DoneLog();
	static void	LogEvent(int nLevel, LPCWSTR lpFormat, ...);

	//HRESULT CreateTextServices(IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk);

protected:
    void createSingletons();
    void destroySingletons();

	CString			m_strResourceFile;								// ��Դ�����ļ���
	CMap<CStringA,LPCSTR,CStringA,LPCSTR> m_mapCfgPool;				// ϵͳ������Ϣ
	CMap<CStringA,LPCSTR,CStringA,LPCSTR> m_mapStylePool;			// �����Ϣ
	CMap<CStringA,LPCSTR,CStringA,LPCSTR> m_mapXmlPool;				// XML�ļ���
	CMap<CStringA,LPCSTR,CStringA,LPCSTR> m_mapSkinPool;			// Skin��
	CMap<CStringA,LPCSTR,CStringA,LPCSTR> m_mapStringPool;			// �ַ�����
	CMap<CStringA,LPCSTR,DuiFontInfo,DuiFontInfo&> m_mapFontPool;	// �����
	
	HINSTANCE				m_hInst;
	UINT					m_uAppID;								// Ӧ��ID

	DWORD					m_dwLangID;								// ��ǰ����ID
	CStringA				m_strCurStyle;							// ��ǰ�����

	CString					m_strLogFile;							// ��־�ļ���
	int						m_nLogLevel;							// ��־����
	BOOL					m_bLogEnable;							// �Ƿ�������־����
	CRITICAL_SECTION		m_WriteLogMutex;						// ��־ͬ����

	DuiVision::CTaskMgr		m_TaskMsg;								// ������Ϣ�������

	vector<CDlgBase*>		m_vecDuiDialog;							// DUI�Ի����б�
	vector<CDuiHandler*>	m_vecDuiHandler;						// �¼���������б�

	CDlgBase*				m_pNotifyMsgBox;						// ��̬��ʾ��Ϣ��

	NOTIFYICONDATA			m_NotifyIconData;						// ��������

	//HINSTANCE	m_rich20;	//richedit module
	//PCreateTextServices	m_funCreateTextServices;
};
