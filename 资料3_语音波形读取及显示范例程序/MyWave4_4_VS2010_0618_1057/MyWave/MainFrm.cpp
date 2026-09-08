// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"


#include "MainFrm.h"
#include "MyWaveView.h"
#include"AnotherView.h"
#include  "OperateDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	//ON_COMMAND(ID_VIEW2, &CMainFrame::OnView2)
	//ON_UPDATE_COMMAND_UI(ID_VIEW2, &CMainFrame::OnUpdateView2)
	//ON_COMMAND(ID_VIEW1, &CMainFrame::OnView1)
	//ON_UPDATE_COMMAND_UI(ID_VIEW1, &CMainFrame::OnUpdateView1)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	this->m_bShowView=View1;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	// TODO: 如果不需要工具栏可停靠，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序




BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类

	CRect rect;
	GetClientRect(&rect);

	   if (!m_wndSplitter.CreateStatic(this, 2, 1))  // 创建2行1列分割
	   {
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	    }
	    // add the first splitter pane - the default view in column 0
	    // 创建第一个格子，在0列中使用默认的视图（由文档模板决定）
	    if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CMyWaveView), CSize(rect.Width(), rect.Height()-18), pContext))
	    {
		  TRACE0("Failed to create first pane\n");
		  return FALSE;
	     }	

	/*   if (!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CAnotherView), CSize(rect.Width(),50), pContext))
	    {
		  TRACE0("Failed to create first pane\n");
		  return FALSE;
	     }*/
	    if (!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(COperateDlg), CSize(rect.Width(),18), pContext)) 
	    {
		   TRACE0("Failed to create second pane\n");
		   return FALSE;
	     }

	return TRUE;
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
}

//void CMainFrame::OnView1()
//{
//	// TODO: 在此添加命令处理程序代码
//	this->m_bShowView =View1;
//		Invalidate();
//}
//
//void CMainFrame::OnUpdateView1(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck (this->m_bShowView =View1);
//}
//void CMainFrame::OnView2()
//{
//	// TODO: 在此添加命令处理程序代码
//	this->m_bShowView =View2;
//		Invalidate();
//}
//
//void CMainFrame::OnUpdateView2(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	 pCmdUI->SetCheck (this->m_bShowView =View2);
//	 
//}


