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

	BOOL ProcessSkinMsg(UINT Msg, WPARAM wParam, LPARAM lParam);

	// ���̼���Ϣ
	void	ProcessInterprocessMsg(UINT Msg, WPARAM wParam, DUI_INTERPROCESS_MSG* pInterMsg);

	virtual void OnInit();
	virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	void	OnOption();
	void	OnOptionOK(CDlgBase* pDlg);

	CDlgBase* m_pDlg;
	UINT m_uTimerAni;	// ������ʱ��
	int m_nAniIndex;	// ��������
};
