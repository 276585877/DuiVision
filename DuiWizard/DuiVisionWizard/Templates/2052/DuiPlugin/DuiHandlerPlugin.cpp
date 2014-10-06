#include "StdAfx.h"
#include "DuiHandlerPlugin.h"
[!if TAB_CHECK_1]
#include "DuiHandler[!output TAB_CLASS_1].h"
[!else]
#include "DuiHandlerHome.h"
[!endif]
[!if TAB_CHECK_2]
#include "DuiHandler[!output TAB_CLASS_2].h"
[!endif]
[!if TAB_CHECK_3]
#include "DuiHandler[!output TAB_CLASS_3].h"
[!endif]
[!if TAB_CHECK_4]
#include "DuiHandler[!output TAB_CLASS_4].h"
[!endif]
[!if TAB_CHECK_5]
#include "DuiHandler[!output TAB_CLASS_5].h"
[!endif]
[!if TAB_CHECK_6]
#include "DuiHandler[!output TAB_CLASS_6].h"
[!endif]

//////////////////////////////////////////////////////////////
// CDuiHandlerPlugin

CDuiHandlerPlugin::CDuiHandlerPlugin(void) : CDuiHandler()
{
	m_pPanel = NULL;
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerPlugin::~CDuiHandlerPlugin(void)
{
}

// ��ʼ��
void CDuiHandlerPlugin::OnInit()
{
[!if TAB_CHECK_1]
	// ��tabҳ'[!output TAB_TITLE_1]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_1]* pDuiHandler[!output TAB_CLASS_1] = new CDuiHandler[!output TAB_CLASS_1]();
	DuiSystem::RegisterHandler(m_pPanel, pDuiHandler[!output TAB_CLASS_1], _T("tab.[!output TAB_CLASS_1]"));
	pDuiHandler[!output TAB_CLASS_1]->OnInit();
[!else]
	CDuiHandlerHome* pDuiHandlerHome = new CDuiHandlerHome();
	DuiSystem::RegisterHandler(m_pPanel, pDuiHandlerHome, _T("tab.Home"));
	pDuiHandlerHome->OnInit();
[!endif]
[!if TAB_CHECK_2]
	// ��tabҳ'[!output TAB_TITLE_2]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_2]* pDuiHandler[!output TAB_CLASS_2] = new CDuiHandler[!output TAB_CLASS_2]();
	DuiSystem::RegisterHandler(m_pPanel, pDuiHandler[!output TAB_CLASS_2], _T("tab.[!output TAB_CLASS_2]"));
	pDuiHandler[!output TAB_CLASS_2]->OnInit();
[!endif]
[!if TAB_CHECK_3]
	// ��tabҳ'[!output TAB_TITLE_3]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_3]* pDuiHandler[!output TAB_CLASS_3] = new CDuiHandler[!output TAB_CLASS_3]();
	DuiSystem::RegisterHandler(m_pPanel, pDuiHandler[!output TAB_CLASS_3], _T("tab.[!output TAB_CLASS_3]"));
	pDuiHandler[!output TAB_CLASS_3]->OnInit();
[!endif]
[!if TAB_CHECK_4]
	// ��tabҳ'[!output TAB_TITLE_4]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_4]* pDuiHandler[!output TAB_CLASS_4] = new CDuiHandler[!output TAB_CLASS_4]();
	DuiSystem::RegisterHandler(m_pPanel, pDuiHandler[!output TAB_CLASS_4], _T("tab.[!output TAB_CLASS_4]"));
	pDuiHandler[!output TAB_CLASS_4]->OnInit();
[!endif]
[!if TAB_CHECK_5]
	// ��tabҳ'[!output TAB_TITLE_5]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_5]* pDuiHandler[!output TAB_CLASS_5] = new CDuiHandler[!output TAB_CLASS_5]();
	DuiSystem::RegisterHandler(m_pPanel, pDuiHandler[!output TAB_CLASS_5], _T("tab.[!output TAB_CLASS_5]"));
	pDuiHandler[!output TAB_CLASS_5]->OnInit();
[!endif]
[!if TAB_CHECK_6]
	// ��tabҳ'[!output TAB_TITLE_6]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_6]* pDuiHandler[!output TAB_CLASS_6] = new CDuiHandler[!output TAB_CLASS_6]();
	DuiSystem::RegisterHandler(m_pPanel, pDuiHandler[!output TAB_CLASS_6], _T("tab.[!output TAB_CLASS_6]"));
	pDuiHandler[!output TAB_CLASS_6]->OnInit();
[!endif]
}

// DUI��ʱ���¼�����
void CDuiHandlerPlugin::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
