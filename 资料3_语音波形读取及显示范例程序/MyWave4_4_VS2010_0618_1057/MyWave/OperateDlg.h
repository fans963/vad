#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include"MyWaveView.h"
#include "MyWaveDoc.h"


// COperateDlg 窗体视图

class COperateDlg : public CFormView
{
	DECLARE_DYNCREATE(COperateDlg)

protected:
	COperateDlg();           // 动态创建所使用的受保护的构造函数
	virtual ~COperateDlg();

public:
	enum { IDD = IDD_DIALOG1 };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
private:
	CMyWaveView* m_pView;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
public:
	public:
	void UpdateFileInfoCtrl(WAVEFILEHEADER header,WAVEFILEHEADER header2,WAVEFILEHEADER header3);//列表框中显示文件信息
	DECLARE_MESSAGE_MAP()
public:
	CString m_Str_XPos;//显示X轴位置
public:
	CString m_Str_XZoom;//X轴方向的缩放比例
public:
	CScrollBar m_X_ShowPos;//X轴滚动条控件
public:
	CSliderCtrl m_XZoom;//X轴方向缩放滑块控件
public:
	CString m_Str_YZoom;
public:
	CString m_Str_YPos;
public:
	CScrollBar m_Y_ShowPos;//Y轴滚动条控件
public:
	CSliderCtrl m_YZoom;//Y轴方向缩放滑块控件
public:
	CListBox m_List_FileInfo;//显示文件信息的列表框

//添加的系统函数

public:
	virtual void OnInitialUpdate();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	afx_msg void OnNMThemeChangedScrollbarXshowpos(NMHDR *pNMHDR, LRESULT *pResult);
	
	CMyWaveDoc m_pDoc;
	CSliderCtrl m_SpecYzoom;
	CSliderCtrl m_FFTSpecYzoom;
	CSliderCtrl m_MelSpecYzoom;
	CSliderCtrl m_SpecYzoom2;
	CSliderCtrl m_FFTSpecYzoom2;
	CSliderCtrl m_MelSpecYzoom2;
	CString m_str_SpecYzoom;
	CString m_str_SpecYzoom2;
	CString m_str_FFTSpecYzoom;
	CString m_str_FFTSpecYzoom2;
	CString m_str_MelSpecYzoom;
	CString m_str_MelSpecYzoom2;
};


