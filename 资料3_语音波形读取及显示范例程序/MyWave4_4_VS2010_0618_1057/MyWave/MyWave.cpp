// MyWave.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "MyWave.h"
#include "MainFrm.h"

#include "MyWaveDoc.h"
#include "MyWaveView.h"
#include"AnotherView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyWaveApp

BEGIN_MESSAGE_MAP(CMyWaveApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMyWaveApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_VIEW1, &CMyWaveApp::OnView1)
	ON_COMMAND(ID_VIEW2, &CMyWaveApp::OnView2)
END_MESSAGE_MAP()


// CMyWaveApp 构造

CMyWaveApp::CMyWaveApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	//m_bShowView=View1;
}


// 唯一的一个 CMyWaveApp 对象

CMyWaveApp theApp;


// CMyWaveApp 初始化

BOOL CMyWaveApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMyWaveDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CMyWaveView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	///////////////////////////////////////////////////////////////////////////////////////////////为多视图窗口新添加的代码
	//yuan@add view init code here
	CView* m_pActiveView = ((CFrameWnd*)m_pMainWnd)->GetActiveView();
	m_pFirstView = m_pActiveView;
	m_pAnotherView = new CAnotherView();
	
	//文档和视图关联
	CDocument* m_pDoc = ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();
	CCreateContext context;
	context.m_pCurrentDoc = m_pDoc;
	//创建视图

	UINT m_IDFORANOTHERVIEW = AFX_IDW_PANE_FIRST + 1;
	CRect rect;
	m_pAnotherView->Create(NULL, NULL, WS_CHILD, rect, m_pMainWnd, m_IDFORANOTHERVIEW, &context);
	////////////////////////////////////////////////////////////////////////////////////////////////

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);		//初始最大化
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生
	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CMyWaveApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CMyWaveApp 消息处理程序


void CMyWaveApp::OnView1()
{
	// TODO: 在此添加命令处理程序代码
	UINT temp = ::GetWindowLong(m_pAnotherView->m_hWnd, GWL_ID);
	::SetWindowLong(m_pAnotherView->m_hWnd, GWL_ID, ::GetWindowLong(m_pFirstView->m_hWnd, GWL_ID));
	::SetWindowLong(m_pFirstView->m_hWnd, GWL_ID, temp);
	
	m_pAnotherView->ShowWindow(SW_HIDE);
	m_pFirstView->ShowWindow(SW_SHOW);
	
	((CFrameWnd*)m_pMainWnd)->SetActiveView(m_pFirstView); 
	((CFrameWnd*) m_pMainWnd)->RecalcLayout();
	//CMainFrame::m_bShowView=View1;
	m_pFirstView->Invalidate();
}

void CMyWaveApp::OnView2()
{
	// TODO: 在此添加命令处理程序代码
	UINT temp = ::GetWindowLong(m_pAnotherView->m_hWnd, GWL_ID);
	::SetWindowLong(m_pAnotherView->m_hWnd, GWL_ID, ::GetWindowLong(m_pFirstView->m_hWnd, GWL_ID));
	::SetWindowLong(m_pFirstView->m_hWnd, GWL_ID, temp);
	
	m_pFirstView->ShowWindow(SW_HIDE);
	m_pAnotherView->ShowWindow(SW_SHOW);      
	
	((CFrameWnd*)m_pMainWnd)->SetActiveView(m_pAnotherView); 

	((CFrameWnd*) m_pMainWnd)->RecalcLayout();
	//CMainFrame::m_bShowView=View2;
	m_pAnotherView->Invalidate();

}


