// MainFrm.h : CMainFrame 类的接口
//

#include "MyWave.h"
#pragma once
#define   View1     01
#define   View2     02
class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:
	//用于切分窗口
	CSplitterWnd m_wndSplitter;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
public:
	BOOL m_bShowView;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);//实现窗口分割
//public:
//	afx_msg void OnView1();
//	afx_msg void OnUpdateView1(CCmdUI *pCmdUI);
//	afx_msg void OnView2();
//	afx_msg void OnUpdateView2(CCmdUI *pCmdUI);

};


