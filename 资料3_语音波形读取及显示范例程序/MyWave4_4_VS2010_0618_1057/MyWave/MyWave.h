// MyWave.h : MyWave 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CMyWaveApp:
// 有关此类的实现，请参阅 MyWave.cpp
//

class CMyWaveApp : public CWinApp
{
public:
	CMyWaveApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	//////////////////////////////////////////新加用于添加多视图
private:

       CView* m_pFirstView;
       CView* m_pAnotherView;
	   afx_msg void OnView1();
	   afx_msg void OnView2();

public:
	 /*BOOL m_bShowView;*/
	
};

extern CMyWaveApp theApp;