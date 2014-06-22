// Panel�ؼ����˿ؼ���һ���ؼ�����
#pragma once

#include "Panel.h"

using namespace  std;

// ����Ϣ
struct TreeColumnInfo
{
	CRect	rcHeader;		// ��ͷλ����Ϣ
	CString strTitle;		// ����
	Color	clrText;		// ������ɫ
	int		nWidth;			// �п��
};

// ��Ԫ����Ϣ
struct TreeItemInfo
{
	CRect	rcItem;			// λ����Ϣ
	CString strTitle;		// ����
	CString strContent;		// ����
	int		nImageIndex;	// ͼƬ����
	Image * pImage;			// ͼƬ����
	CSize	sizeImage;		// ͼƬ��С
	int		nImageCount;	// ͼƬ����
	Color	clrText;		// ������ɫ
	CString strLink;		// ���ӵ�����
	CString strLinkAction;	// ���ӵĶ���
	BOOL	bNeedTitleTip;	// �Ƿ���Ҫ��ʾtitle tip(titleʵ�ʿ�ȴ�����ʾ���)
	BOOL	bNeedContentTip;// �Ƿ���Ҫ��ʾcontent tip(contentʵ�ʿ�ȴ�����ʾ���)
	BOOL	bUseTitleFont;	// �Ƿ�ʹ�ñ���������ʾ����
	BOOL	bShowCollapse;	// �Ƿ��ڴ˵�Ԫ����ʾ�ڵ�����ͼƬ
};

#define HTREEITEM	int

// ����Ϣ
struct TreeNodeInfo
{
	HTREEITEM hParentNode;	// ���ڵ���
	HTREEITEM hNode;		// �ڵ���
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
	BOOL	bCollapse;		// �Ƿ��۵�
	BOOL	bHide;			// �Ƿ�����
	vector<TreeItemInfo> vecItemInfo;
};

class CDuiTreeCtrl : public CDuiPanel
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiTreeCtrl, "treectrl")

public:
	CDuiTreeCtrl(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiTreeCtrl(void);

	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadNode(HTREEITEM hParentNode, TiXmlElement* pXmlElem);

	BOOL InsertColumn(int nColumn, CString strTitle, int nWidth = -1, Color clrText = Color(0, 0, 0, 0));
	HTREEITEM InsertNode(HTREEITEM hParentNode, CString strId, CString strTitle, BOOL bCollapse = FALSE,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""),
		int nRightImageIndex = -1, CString strRightImage = _T(""),
		int nCheck = -1);
	HTREEITEM InsertNode(HTREEITEM hParentNode, TreeNodeInfo &nodeInfo);
	BOOL SetSubItem(HTREEITEM hNode, int nItem, CString strTitle, CString strContent = _T(""), BOOL bUseTitleFont = FALSE,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL SetSubItemLink(HTREEITEM hNode, int nItem, CString strLink, CString strLinkAction = _T(""),
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL SetSubItemCollapse(HTREEITEM hNode, int nItem, CString strImage = _T(""), int nImageCount = 0);
	BOOL DeleteNode(HTREEITEM hNode);
	int  GetNodeCount() { return m_vecRowInfo.size(); }
	int  GetNodeRow(HTREEITEM hNode);
	int	 GetNodeLastChildRow(HTREEITEM hNode);
	BOOL HaveChildNode(HTREEITEM hNode);
	int  GetNodeLevel(HTREEITEM hNode);
	HTREEITEM GetNodeWithId(CString strId);
	TreeNodeInfo* GetNodeInfo(HTREEITEM hNode);
	TreeItemInfo* GetItemInfo(HTREEITEM hNode, int nItem);
	void SetItemInfo(HTREEITEM hNode, int nItem, TreeItemInfo* pItemInfo);
	void SetNodeColor(HTREEITEM hNode, Color clrText);
	void ToggleNode(HTREEITEM hNode);
	void SetNodeCheck(HTREEITEM hNode, int nCheck);
	int  GetNodeCheck(HTREEITEM hNode);
	void ClearNodes();
	void HideChildNodes(HTREEITEM hItem);
	void RefreshNodeRows();

	BOOL SetSeperator(UINT nResourceID = 0, CString strType= TEXT("PNG"));
	BOOL SetSeperator(CString strImage = TEXT(""));
	BOOL SetCheckBoxImage(UINT nResourceID = 0, CString strType= TEXT("PNG"));
	BOOL SetCheckBoxImage(CString strImage = TEXT(""));
	BOOL SetCollapseImage(UINT nResourceID = 0, CString strType= TEXT("PNG"));
	BOOL SetCollapseImage(CString strImage = TEXT(""));
	BOOL SetToggleImage(UINT nResourceID = 0, CString strType= TEXT("PNG"));
	BOOL SetToggleImage(CString strImage = TEXT(""));

	BOOL PtInRow(CPoint point, TreeNodeInfo& rowInfo);
	BOOL PtInRowCheck(CPoint point, TreeNodeInfo& rowInfo);
	BOOL PtInRowCollapse(CPoint point, TreeNodeInfo& rowInfo);
	int  PtInRowItem(CPoint point, TreeNodeInfo& rowInfo);

	void SetGridTooltip(HTREEITEM hNode, int nItem, CString strTooltip);
	void ClearGridTooltip();

protected:
	vector<TreeColumnInfo> m_vecColumnInfo;
	vector<TreeNodeInfo> m_vecRowInfo;

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);

	HRESULT OnAttributeFontTitle(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImageSeperator(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImageCheckBox(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImageCollapse(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImageToggle(const CStringA& strValue, BOOL bLoading);

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
	int					m_nBkTransparent;	// ����͸����
	BOOL				m_bSingleLine;		// ��ʾ��������
	BOOL				m_bTextWrap;		// �����Ƿ���

	Image*				m_pImageSeperator;	// �зָ���ͼƬ
	CSize				m_sizeSeperator;	// �зָ��ߴ�С
	Image*				m_pImageCheckBox;	// ����ͼƬ
	CSize				m_sizeCheckBox;		// ����ͼƬ��С
	Image*				m_pImageCollapse;	// ������ͼƬ
	CSize				m_sizeCollapse;		// ������ͼƬ��С
	Image*				m_pImageToggle;		// ���ڵ�ͼƬ
	CSize				m_sizeToggle;		// ���ڵ�ͼƬ��С

	int					m_nHoverRow;		// ��ǰ����ƶ���������
	int					m_nDownRow;			// ��ǰ�����������
	BOOL				m_bEnableDownRow;	// ������ʾ��ǰ�����

	int					m_nFirstViewRow;	// ��ǰ��ʾ���ĵ�һ�е����
	int					m_nVirtualTop;		// ��ǰ������λ�ö�Ӧ�������topλ��
	int					m_nVisibleRowCount;	// ��ǰ����ʾ������(���۵���)

	BOOL				m_bGridTooltip;		// �Ƿ���ʾ��Ԫ���Tooltip
	int					m_nTipNode;			// ��ǰtip�ڵ�
	int					m_nTipItem;			// ��ǰtip��
	int					m_nTipVirtualTop;	// ��ǰtip�е�����Top

	HTREEITEM			m_nNodeIndex;		// �����������

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("img-sep", OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE("img-check", OnAttributeImageCheckBox)
		DUI_CUSTOM_ATTRIBUTE("img-collapse", OnAttributeImageCollapse)
		DUI_CUSTOM_ATTRIBUTE("img-toggle", OnAttributeImageToggle)
		DUI_CUSTOM_ATTRIBUTE("font-title", OnAttributeFontTitle)
		DUI_COLOR_ATTRIBUTE("crtext", m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE("crhover", m_clrTextHover, FALSE)
		DUI_COLOR_ATTRIBUTE("crpush", m_clrTextDown, FALSE)
		DUI_COLOR_ATTRIBUTE("crtitle", m_clrTitle, FALSE)
		DUI_COLOR_ATTRIBUTE("crsep", m_clrSeperator, FALSE)
		DUI_INT_ATTRIBUTE("row-height", m_nRowHeight, FALSE)
		DUI_INT_ATTRIBUTE("left-pos", m_nLeftPos, FALSE)
		DUI_INT_ATTRIBUTE("wrap", m_bTextWrap, FALSE)
		DUI_INT_ATTRIBUTE("down-row", m_bEnableDownRow, FALSE)
		DUI_INT_ATTRIBUTE("bk-transparent", m_nBkTransparent, FALSE)
		DUI_INT_ATTRIBUTE("grid-tip", m_bGridTooltip, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
