// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI�¼�������
class CDuiHandlerMain : public CDuiHandler
{
public:
	CDuiHandlerMain(void);
	virtual ~CDuiHandlerMain(void);

	void SetDialog(CDlgBase* pDlg) { m_pDlg = pDlg; };

	virtual void OnInit();

	// Ƥ��ѡ����Ϣ����
	LRESULT OnDuiMsgSkin(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);

	// ���̼���Ϣ����
	LRESULT	OnDuiMsgInterprocess(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	LRESULT OnDuiMsgButtonGoBack(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgButtonGoForward(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgButtonRefresh(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgButtonHome(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgComboUrlKeyDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgComboUrl(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgButtonNewTab(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgButtonCloseTab(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgWebTitleChange(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);

	CDuiWebBrowserCtrl* GetTabWebControl(CControlBase* pTabCtrl);
	TabItemInfo* GetTabInfoByWebCtrlId(UINT uID);
	CDuiWebBrowserCtrl* GetCurTabWebControl();
	void InsertExplorerTab(int nIndex, CString strTitle, CString strUrl);

	CDlgBase*		m_pDlg;
	UINT			m_uTimerAni;	// ������ʱ��
	int				m_nAniIndex;	// ��������
	CStringArray	m_asUrl;		// �����tabҳǩURL

	// ��Ϣ������
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerMain)
		DUI_CONTROL_ID_MESSAGE(APP_IPC, OnDuiMsgInterprocess)
		DUI_CONTROL_NAME_MESSAGE(NAME_SKIN_WND, OnDuiMsgSkin)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"button.goback", MSG_BUTTON_UP, OnDuiMsgButtonGoBack)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"button.goforward", MSG_BUTTON_UP, OnDuiMsgButtonGoForward)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"button.reflush", MSG_BUTTON_UP, OnDuiMsgButtonRefresh)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"button.home", MSG_BUTTON_UP, OnDuiMsgButtonHome)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"combo.url", MSG_KEY_DOWN, OnDuiMsgComboUrlKeyDown)
		DUI_CONTROL_NAME_MESSAGE(L"combo.url", OnDuiMsgComboUrl)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"button.new", MSG_BUTTON_UP, OnDuiMsgButtonNewTab)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"tabctrl.main", MSG_CONTROL_BUTTON, OnDuiMsgButtonCloseTab)
		DUI_CONTROL_MSG_MESSAGE(DISPID_TITLECHANGE, OnDuiMsgWebTitleChange)
	DUI_DECLARE_MESSAGE_END()
};
