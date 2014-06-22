#include "StdAfx.h"
#include "DuiHandlerMain.h"
#include "registry.h"


#define XML_ABOUT_DLG L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<dlg name=\"dlg_about\" title=\"MsgBox\" width=\"450\" height=\"230\" appwin=\"0\" >\
	<base>\
		\<imgbtn name=\"button.close\" pos=\"-45,0,-0,29\" skin=\"IDB_BT_CLOSE\"/>\
		<text name=\"title\" crtext=\"FFFFFF\" crmark=\"800000\" font=\"big\"\
				pos=\"10,5,200,25\" title=\"����[APP_NAME]\" mask=\"[APP_NAME]\" response=\"0\"\ />\
	</base>\
	<body>\
		<area name=\"area-1\" pos=\"0,0,-0,40\" begin-transparent=\"100\" end-transparent=\"30\" />\
		<area name=\"area-2\" pos=\"0,40,-0,-0\" begin-transparent=\"30\" end-transparent=\"30\" />\
		<area name=\"area-3\" pos=\"0,-37,-0,-36\" begin-transparent=\"70\" end-transparent=\"70\" />\
		<area name=\"area-4\" pos=\"0,-36,-0,-0\" begin-transparent=\"88\" end-transparent=\"88\" />\
		\
		<img name=\"icon\" pos=\"25,45\" width=\"128\" height=\"128\" image=\"skins\\scriptnet.jpg\" mode=\"normal\" framesize=\"0\" response=\"0\" />\
		\
		<text crtext=\"000000\" pos=\"170,45,-25,65\" title=\"[APP_NAME] [APP_VER]\" />\
		<text crtext=\"000000\" pos=\"170,65,-25,85\" title=\"��Ȩ����2013-2014\" />\
		<linkbtn name=\"linkbtn1\" crtext=\"800000\"\
			pos=\"170,100,-25,130\" title=\"���������\" href=\"http://www.blueantstudio.net\" />\
		<text crtext=\"000080\" pos=\"170,140,-25,160\" title=\"�˶Ի����ɶ����XML���ݴ���\" />\
		\
		<button name=\"button.ok\" skin=\"IDB_BT_DEFAULT\" title=\"[OK]\" pos=\"-100,-30,-20,-6\" />\
	</body>\
</dlg>";


CDuiHandlerMain::CDuiHandlerMain(void) : CDuiHandler()
{
	m_pDlg = NULL;
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerMain::~CDuiHandlerMain(void)
{
}

// ��ʼ��
void CDuiHandlerMain::OnInit()
{
	// ��ʼ������ͼ��
	DuiSystem::Instance()->InitTray();
	// ��tabҳ3ע���¼��������
	//DuiSystem::RegisterHandler(m_pDlg, new CDuiHandlerTab3(), _T("tab3"));
	// ������ҳע���¼��������
	//CDuiHandlerTools* pDuiHandlerTools = new CDuiHandlerTools();
	//pDuiHandlerTools->SetDialog(m_pDlg);
	//DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerTools, _T("tab.tools"));
	//pDuiHandlerTools->OnInit();
	// ����������ʱ��
	m_uTimerAni = DuiSystem::AddDuiTimer(500);
}

// Ƥ����Ϣ����(ʵ��Ƥ���ı���ͻ�ȡ)
BOOL CDuiHandlerMain::ProcessSkinMsg(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(Msg == MSG_GET_SKIN_TYPE)	// ��ȡSkin����
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		int nBkType = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE);
		*(int*)wParam = nBkType;
		return TRUE;
	}else
	if(Msg == MSG_GET_SKIN_VALUE)	// ��ȡSkinֵ
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			*(int*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES);
			return TRUE;
		}else
		if(wParam == BKTYPE_COLOR)
		{
			*(COLORREF*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR);
			return TRUE;
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			*(CString*)lParam = reg.GetStringValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE);
			return TRUE;
		}
	}else
	if(Msg == MSG_SET_SKIN_VALUE)	// ����Skinֵ
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE, wParam);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES, lParam);
		}else
		if(wParam == BKTYPE_COLOR)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR, lParam);
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			CString* pstrImgFile = (CString*)lParam;
			reg.SetStringValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE, *pstrImgFile);
		}
		return TRUE;
	}
	return FALSE;
}

// DUI�¼�����
LRESULT CDuiHandlerMain::OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("CDuiHandlerTest::OnDuiMessage:uID=%d, name=%s, msg=%d, wParam=%d, lParam=%d"),
						uID, strName, Msg, wParam, lParam);

	if(strName == NAME_SKIN_WND)	// Ƥ����Ϣ����
	{
		return ProcessSkinMsg(Msg, wParam, lParam);
	}else
	if(uID == APP_IPC)	// ���̼���Ϣ
	{
		ProcessInterprocessMsg(Msg, wParam, (DUI_INTERPROCESS_MSG*)lParam);
	}else
	if((strName == NAME_TRAY_ICON) && (Msg == MSG_TRAY_DBCLICK))	// ����˫����Ϣ
	{
		DuiSystem::ShowDuiDialog(_T("dlg_login"), NULL);
	}else
	if((strName == _T("notify_button_1")) && (Msg == BUTTOM_UP))
	{
		DuiSystem::AddDuiNotifyMsgTask(_T("��ʾ��Ϣ1��\n�����ʾ���񣬴��ڴ�С����Ϊ400*250���˴���5���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T("�Զ������"), 5000, 400, 250);
	}else
	if((strName == _T("notify_button_2")) && (Msg == BUTTOM_UP))
	{
		DuiSystem::AddDuiNotifyMsgTask(_T("��ʾ��Ϣ2��\n�����ʾ���񣬴��ڴ�С����Ϊ300*200���˴���3���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T(""), 3000, 300, 200);
	}else
	if((strName == _T("notify_button_3")) && (Msg == BUTTOM_UP))	// �Զ����Notify��ʾ����
	{
		DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));
		// ���ñ��������
		DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.title"), _T("�Զ�����ʾ����"));
		DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.text"), _T("������ʽ��ʾ���ڣ����Զ��رգ���СΪ250*300����ʾȷ����ť��................."));
		// ����ͼ��
		DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_INFO"));
		// ��������,5������,��Ҫ�Ѳ��õ��������ص�
		DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.0"), TRUE);
		DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.1"), FALSE);
		DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.2"), FALSE);
		DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.3"), FALSE);
		DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.4"), FALSE);
		// ���ô�С
		DuiSystem::SetNotifyMsgBoxSize(250, 300);
		// ��ʾ����
		DuiSystem::ShowNotifyMsgBox();
	}else
	if((strName == _T("msgbox_button_1")) && (Msg == BUTTOM_UP))
	{
		DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���1��Ĭ�ϵĶԻ���\n1.����ʹ�ó���Ĭ�ϱ���\n2.��ťʹ��Ĭ�ϵ�ȷ��(MB_OK)��ť\n3.ͼ��ʹ��Ĭ�ϵ���Ϣͼ��"));
	}else
	if((strName == _T("msgbox_button_2")) && (Msg == BUTTOM_UP))
	{
		DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���2��\n1.����ʹ���Զ������\n2.��ťʹ��ȷ��ȡ��(MB_OKCANCEL)��ť\n3.ͼ��ʹ�ô���ͼ��"), _T("�Զ������"), MB_OKCANCEL|MB_ICONERROR);
	}else
	if((strName == _T("msgbox_button_3")) && (Msg == BUTTOM_UP))
	{
		DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���2��\n1.����ʹ�ó���Ĭ�ϱ���\n2.��ťʹ���Ƿ�ȡ��(MB_YESNOCANCEL)��ť\n3.ͼ��ʹ�ü��ͼ��\n4.�Զ���Ի���Ĵ�С"), _T(""), MB_YESNOCANCEL, 370, 200);
	}else
	if((strName == _T("timer_button_1")) && (Msg == BUTTOM_UP))	// ������ʾ��ʱ��
	{
		DuiSystem::AddDuiTimer(20000, _T("timer_notify"));
		DuiSystem::DuiMessageBox(NULL, _T("��ʾ��ʱ������������ʱ����20�룡"));
	}else
	if((strName == _T("timer_button_2")) && (Msg == BUTTOM_UP))	// ֹͣ��ʾ��ʱ��
	{
		DuiSystem::RemoveDuiTimer(_T("timer_notify"));
		DuiSystem::DuiMessageBox(NULL, _T("��ʾ��ʱ����ֹͣ��"));
	}else
	if((strName == _T("xml_content_dlg_button")) && (Msg == BUTTOM_UP))	// XML���ݶԻ���
	{
		// ��ʾͨ�������XML���ݼ��ش���
		CString strDlgXmlContent = XML_ABOUT_DLG;
		CDlgBase* pDlg = DuiSystem::CreateDuiDialog(strDlgXmlContent, GetControlDialog(uID), _T(""), TRUE, 0, TRUE);
		if(pDlg != NULL)
		{
			int nResponse = pDlg->DoModal();
			DuiSystem::Instance()->RemoveDuiDialog(pDlg);
		}
	}else
	if((strName == _T("button_hide_tab")) && (Msg == BUTTOM_UP))	// ������tabҳ�е�ĳ��ҳ��
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
		if(pTabCtrl)
		{
			pTabCtrl->SetItemVisible(7, FALSE);
			//pTabCtrl->DeleteItem(6);
			//pTabCtrl->DeleteItem(L"tab_6");
		}
	}else
	if((strName == _T("button_show_tab")) && (Msg == BUTTOM_UP))	// ��ʾ��tabҳ�е�ĳ��ҳ��
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
		if(pTabCtrl)
		{
			pTabCtrl->SetItemVisible(7, TRUE);
		}
	}else
	if((strName == _T("pic_button_2")) && (Msg == BUTTOM_UP))	// ˢ��Tabҳ
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
		if(pTabCtrl)
		{
			pTabCtrl->LoadTabXml(L"tab_ext.xml");
		}
	}else
	if((strName == _T("button_load_tab")) && (Msg == BUTTOM_UP))	// ����TabҳXML
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
		if(pTabCtrl)
		{
			pTabCtrl->DeleteItem(L"tab_5");
			pTabCtrl->DeleteItem(L"tab_6");
			pTabCtrl->DeleteItem(L"tab_7");
			pTabCtrl->LoadTabXml(L"tab_ext.xml");
		}
	}else
	if((strName == _T("button_hide_tab_check2")) && (Msg == BUTTOM_UP))	// ����tabҳ
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.check")));
		if(pTabCtrl)
		{
			pTabCtrl->SetItemVisible(1, FALSE);
		}
	}else
	if((strName == _T("button_show_tab_check2")) && (Msg == BUTTOM_UP))	// ��ʾtabҳ
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.check")));
		if(pTabCtrl)
		{
			pTabCtrl->SetItemVisible(1, TRUE);
		}
	}else
	if((strName == _T("menu_1")) && (Msg == BUTTOM_UP))	// �˵���ʾ��ť
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_1"));
		CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
		pDuiMenu->SetParent(pButton);
		CPoint point;
		CRect rc = pButton->GetRect();
		point.SetPoint(rc.left + 75, rc.bottom);
		if(pDlg != NULL)
		{
			pDlg->ClientToScreen(&point);
		}
		// ��ʾ����ڲ˵�����ʱ����Ĳ˵������ʾ���⡢�ɼ��ԡ��Ƿ���á��Ƿ�ѡ�������
		// �����ڵ���LoadXmlFile֮ǰͨ���˵���������������Ӧ�˵��������
		pDuiMenu->SetItemTitle(L"item_login", L"��֤--�޸�Title");
		pDuiMenu->SetItemCheck(L"item_setup", 0);
		pDuiMenu->SetItemVisible(L"item_help", FALSE);
		pDuiMenu->SetItemDisable(L"item_about", TRUE);
		pDuiMenu->LoadXmlFile(_T("menu_tray"), pDlg, point, WM_DUI_MENU);
		pDuiMenu->ShowWindow(SW_SHOW);
	}else
	if((strName == _T("menu_2")) && (Msg == BUTTOM_UP))	// �˵���ʾ��ť
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_2"));
		CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
		pDuiMenu->SetParent(pButton);
		CPoint point;
		CRect rc = pButton->GetRect();
		point.SetPoint(rc.left + 75, rc.bottom);
		if(pDlg != NULL)
		{
			pDlg->ClientToScreen(&point);
		}
		pDuiMenu->LoadXmlFile(_T("duivision\\menu_2.xml"), pDlg, point, WM_DUI_MENU);
		pDuiMenu->ShowWindow(SW_SHOW);
	}else
	if((strName == _T("menu_3")) && (Msg == BUTTOM_UP))	// �˵���ʾ��ť
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_3"));
		CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
		pDuiMenu->SetParent(pButton);
		CPoint point;
		CRect rc = pButton->GetRect();
		point.SetPoint(rc.left + 75, rc.bottom);
		if(pDlg != NULL)
		{
			pDlg->ClientToScreen(&point);
		}
		pDuiMenu->LoadXmlFile(_T("duivision\\menu_3.xml"), pDlg, point, WM_DUI_MENU);
		pDuiMenu->ShowWindow(SW_SHOW);
	}else
	if((strName == _T("listctrl_1")) && (Msg == BUTTOM_DOWN))	// �б�ؼ����
	{
		// ������б�ؼ�ĳһ��,��ʾ���е���ϸ��Ϣ,���������wParam��ʾ�ؼ��к�
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_1"));
		ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
		if(pDlg && pRowInfo)
		{
			CDlgPopup* pDlgPopup = new CDlgPopup;
			CRect rc = pListCtrl->GetRect();
			rc.OffsetRect(50, 30);
			pDlgPopup->LoadXmlFile(_T("xml:dlg_notice"));
			CControlBaseFont* pControlTitle = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.title")));
			if(pControlTitle)
			{
				pControlTitle->SetTitle(pRowInfo->strTitle);
			}
			CControlBaseFont* pControlTime = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.time")));
			if(pControlTime)
			{
				pControlTime->SetTitle(pRowInfo->strTime);
			}
			CControlBaseFont* pControlContent = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.content")));
			if(pControlContent)
			{
				//CString strTmp = L"www\nwww\nj";
				pControlContent->SetTitle(pRowInfo->strContent);
			}
			pDlg->OpenDlgPopup(pDlgPopup, rc, 0);
		}
	}else
	if((strName == _T("listctrl_2")) && (Msg == BUTTOM_DOWN))
	{
		// ������б�ؼ�ĳһ��,��ʾ���е���ϸ��Ϣ
		// ���������wParam��ʾ�ؼ��к�,lParam��ʾ��������ӵ�����(����1��2�ֱ�Ϊ0��1)
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
		ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
		if(pDlg && pRowInfo)
		{
			if(lParam != -1)
			{
				// ������е�ĳ������
				CString strMsg;
				strMsg.Format(_T("������б��е�����\n�����ݣ�%s\n���������ţ�%d"), pRowInfo->strTitle, lParam);
				DuiSystem::DuiMessageBox(NULL, strMsg);
			}else
			{
				CDlgPopup* pDlgPopup = new CDlgPopup;
				CRect rc = pListCtrl->GetRect();
				rc.OffsetRect(50, 30);
				pDlgPopup->LoadXmlFile(_T("xml:dlg_notice"));
				CControlBaseFont* pControlTitle = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.title")));
				if(pControlTitle)
				{
					pControlTitle->SetTitle(pRowInfo->strTitle);
				}
				CControlBaseFont* pControlTime = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.time")));
				if(pControlTime)
				{
					pControlTime->SetTitle(pRowInfo->strTime);
				}
				CControlBaseFont* pControlContent = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.content")));
				if(pControlContent)
				{
					pControlContent->SetTitle(pRowInfo->strContent);
				}
				pDlg->OpenDlgPopup(pDlgPopup, rc, 0);
			}
		}
	}else
	if((strName == _T("treectrl_1")) && (Msg == BUTTOM_DOWN))
	{
		CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
		HTREEITEM hNode = (HTREEITEM)wParam;
		int nItem = lParam;
		if(nItem == 0)
		{
			pTreeCtrl->ToggleNode(hNode);
		}
	}else
	if((strName == _T("item_setup")) && (Msg == BUTTOM_UP))	// ϵͳ���ò˵�
	{
		OnOption();
	}else
	if((strName == _T("config.button.ok")) && (Msg == BUTTOM_UP))	// ϵͳ���öԻ����ȷ�ϰ�ť
	{
		// ���ȷ�ϰ�ť֮���������Ӧ�Ĵ���,��������Ƿ�رմ���,��ȡ�����пؼ���ֵ��
		CDlgBase* pDlg = GetControlDialog(uID);
		if(pDlg)
		{
			OnOptionOK(pDlg);
		}
	}

    return 0;
}

// DUI��ʱ���¼�����
void CDuiHandlerMain::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
		/*CRuning* pRunning = (CRuning*)GetControl(_T("animateimg_scan"));
		if(pRunning)
		{
			pRunning->SetRun(FALSE, m_nAniIndex);
		}*/

		CDuiProgress* pProgress1 = (CDuiProgress*)GetControl(_T("progress_1"));
		if(pProgress1)
		{
			pProgress1->SetProgress(m_nAniIndex*10);
		}

		CDuiProgress* pProgress2 = (CDuiProgress*)GetControl(_T("progress_2"));
		if(pProgress2)
		{
			pProgress2->SetProgress(m_nAniIndex*10);
		}

		m_nAniIndex++;
		if(m_nAniIndex > 10)
		{
			m_nAniIndex = 0;
		}
	}else
	if(_T("timer_notify") == strTimerName)
	{
		// ��ʱ��ʾ��ʾ��
		DuiSystem::AddDuiNotifyMsgTask(_T("�����ɶ�ʱ����������ʾ��\n�������ڣ�20��\n�˴���5���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T(""), 5000);
		//DuiSystem::RemoveDuiTimer(_T("timer_notify"));
	}
}

// ���̼���Ϣ����
void CDuiHandlerMain::ProcessInterprocessMsg(UINT Msg, WPARAM wParam, DUI_INTERPROCESS_MSG* pInterMsg)
{
	// �����в���,���Զ������в������д���,Ҳ����ֱ����ʾ������
	CString strCmd = pInterMsg->wInfo;
	if(!strCmd.IsEmpty())
	{
		DuiSystem::DuiMessageBox(NULL, L"ִ���������в���:" + strCmd);
	}else
	{
		CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(L"dlg_main");
		if(pDlg)
		{
			pDlg->SetForegroundWindow();
			pDlg->ShowWindow(SW_NORMAL);
			pDlg->ShowWindow(SW_SHOW);
			pDlg->BringWindowToTop();
		}
	}
}

// ��ʾϵͳ���öԻ���
void CDuiHandlerMain::OnOption()
{
	USES_CONVERSION;

	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(_T("dlg_option"), m_pDlg, _T(""), TRUE, 0, TRUE);
	if(pDlg == NULL)
	{
		return;
	}

	// IP��ַ
	pDlg->SetControlValue(L"config.server.ip", L"title", L"192.168.1.1");

	int nResponse = pDlg->DoModal();
	DuiSystem::Instance()->RemoveDuiDialog(pDlg);
}

//ϵͳ���öԻ���ȷ��
void CDuiHandlerMain::OnOptionOK(CDlgBase* pDlg)
{
	USES_CONVERSION;

	// IP��ַ
	CString strServerAddr = L"";
	CDuiEdit* pControlIp = static_cast<CDuiEdit*>(pDlg->GetControl(L"config.server.ip"));
	if(pControlIp)
	{
		strServerAddr = pControlIp->GetEditText();
		if(strServerAddr == L"")
		{
			// ��ַ����Ϊ����ʾ
			return;
		}
	}

	pDlg->DoOK();
}