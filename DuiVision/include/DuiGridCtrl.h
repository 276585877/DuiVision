// Panel�ؼ����˿ؼ���һ���ؼ�����
#pragma once

#include "Panel.h"

using namespace  std;

// ����Ϣ
struct GridColumnInfo
{
	CRect	rcHeader;		// ��ͷλ����Ϣ
	CString strTitle;		// ����
	Color	clrText;		// ������ɫ
	int		nWidth;			// �п��
};

// ��Ԫ����Ϣ
struct GridItemInfo
{
	CRect	rcItem;			// λ����Ϣ
	CString strTitle;		// ����
	CString strContent;		// ����
	int		nImageIndex;	// ͼƬ����
	Image * pImage;			// ͼƬ����
	CSize	sizeImage;		// ͼƬ��С
	Color	clrText;		// ������ɫ
	CString strLink;		// ���ӵ�����
	CString strLinkAction;	// ���ӵĶ���
	BOOL	bNeedTitleTip;	// �Ƿ���Ҫ��ʾtitle tip(titleʵ�ʿ�ȴ�����ʾ���)
	BOOL	bNeedContentTip;// �Ƿ���Ҫ��ʾcontent tip(contentʵ�ʿ�ȴ�����ʾ���)
	BOOL	bUseTitleFont;	// �Ƿ�ʹ�ñ���������ʾ����
};

// ����Ϣ
struct GridRowInfo
{
	CRect	rcRow;			// ��λ����Ϣ
	CString	strId;			// ID
	int		nCheck;			// ����״̬(-1��ʾ����ʾ)
	CRect	rcCheck;		// ����λ����Ϣ
	int		nImageIndex;	// ͼƬ����
	Image * pImage;			// ͼƬ����
	CSize	sizeImage;		// ͼƬ��С
	int		nRightImageIndex;// �ұ�ͼƬ����
	Image * pRightImage;	// �ұ�ͼƬ����
	CSize	sizeRightImage;	// �ұ�ͼƬ��С
	BOOL	bRowColor;		// ʹ���ж������ɫ
	Color	clrText;		// ��������ɫ
	int		nHoverItem;		// ��ǰ�ȵ���
	vector<GridItemInfo> vecItemInfo;
};

class CDuiGridCtrl : public CDuiPanel
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiGridCtrl, "gridctrl")

public:
	CDuiGridCtrl(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiGridCtrl(void);

	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);

	BOOL InsertColumn(int nColumn, CString strTitle, int nWidth = -1, Color clrText = Color(0, 0, 0, 0));
	BOOL InsertRow(int nItem, CString strId,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""),
		int nRightImageIndex = -1, CString strRightImage = _T(""),
		int nCheck = -1);
	BOOL InsertRow(int nItem, GridRowInfo &rowInfo);
	BOOL SetSubItem(int nRow, int nItem, CString strTitle, CString strContent = _T(""), BOOL bUseTitleFont = FALSE,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL SetSubItemLink(int nRow, int nItem, CString strLink, CString strLinkAction = _T(""),
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL DeleteRow(int nItem);
	int  GetRowCount() { return m_vecRowInfo.size(); }
	GridRowInfo* GetRowInfo(int nRow);
	GridItemInfo* GetItemInfo(int nRow, int nItem);
	void SetRowColor(int nRow, Color clrText);
	void SetRowCheck(int nRow, int nCheck);
	int  GetRowCheck(int nRow);
	void ClearItems();

	BOOL PtInRow(CPoint point, GridRowInfo& rowInfo);
	BOOL PtInRowCheck(CPoint point, GridRowInfo& rowInfo);
	int  PtInRowItem(CPoint point, GridRowInfo& rowInfo);

	void SetGridTooltip(int nRow, int nItem, CString strTooltip);
	void ClearGridTooltip();

protected:
	vector<GridColumnInfo> m_vecColumnInfo;
	vector<GridRowInfo> m_vecRowInfo;

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);

	HRESULT OnAttributeFontTitle(const CStringA& strValue, BOOL bLoading);

	// ��Ϣ��Ӧ
	virtual LRESULT OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam);

public:
	CControlBase*		m_pControBkArea;	// ����Area
	CString				m_strFontTitle;		// ��������
	int					m_nFontTitleWidth;	// ����������
	FontStyle			m_fontTitleStyle;	// ��������Style
	Color				m_clrText;			// ������ɫ
	Color				m_clrTextHover;		// ������ɫ
	Color				m_clrTextDown;		// ������ɫ
	Color				m_clrTitle;			// ������ɫ
	Color				m_clrSeperator;		// �ָ�����ɫ
	int					m_nLeftPos;			// �����ʼλ��
	int					m_nRowHeight;		// �и߶�
	int					m_nHeaderHeight;	// �����и߶�
	int					m_nBkTransparent;	// ����͸����
	BOOL				m_bSingleLine;		// ��ʾ��������
	BOOL				m_bTextWrap;		// �����Ƿ���

	int					m_nHoverRow;		// ��ǰ����ƶ���������
	int					m_nDownRow;			// ��ǰ�����������
	BOOL				m_bEnableDownRow;	// ������ʾ��ǰ�����

	int					m_nFirstViewRow;	// ��ǰ��ʾ���ĵ�һ�е����
	int					m_nLastViewRow;		// ��ǰ��ʾ�������һ�е����
	int					m_nVirtualTop;		// ��ǰ������λ�ö�Ӧ�������topλ��

	BOOL				m_bGridTooltip;		// �Ƿ���ʾ��Ԫ���Tooltip
	int					m_nTipRow;			// ��ǰtip��
	int					m_nTipItem;			// ��ǰtip��
	int					m_nTipVirtualTop;	// ��ǰtip�е�����Top

	DUI_IMAGE_ATTRIBUTE_DEFINE(Seperator);	// �����зָ���ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(CheckBox);	// �������ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("img-sep", OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE("img-check", OnAttributeImageCheckBox)
		DUI_CUSTOM_ATTRIBUTE("font-title", OnAttributeFontTitle)
		DUI_COLOR_ATTRIBUTE("crtext", m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE("crhover", m_clrTextHover, FALSE)
		DUI_COLOR_ATTRIBUTE("crpush", m_clrTextDown, FALSE)
		DUI_COLOR_ATTRIBUTE("crtitle", m_clrTitle, FALSE)
		DUI_COLOR_ATTRIBUTE("crsep", m_clrSeperator, FALSE)
		DUI_INT_ATTRIBUTE("row-height", m_nRowHeight, FALSE)
		DUI_INT_ATTRIBUTE("header-height", m_nHeaderHeight, FALSE)
		DUI_INT_ATTRIBUTE("left-pos", m_nLeftPos, FALSE)
		DUI_INT_ATTRIBUTE("wrap", m_bTextWrap, FALSE)
		DUI_INT_ATTRIBUTE("down-row", m_bEnableDownRow, FALSE)
		DUI_INT_ATTRIBUTE("bk-transparent", m_nBkTransparent, FALSE)
		DUI_INT_ATTRIBUTE("grid-tip", m_bGridTooltip, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
