// OperateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyWave.h"
#include "OperateDlg.h"

// COperateDlg

IMPLEMENT_DYNCREATE(COperateDlg, CFormView)

COperateDlg::COperateDlg()
	: CFormView(COperateDlg::IDD)
	, m_Str_XPos(_T(""))
	, m_Str_XZoom(_T(""))
	, m_Str_YZoom(_T(""))
	, m_Str_YPos(_T(""))
	, m_str_SpecYzoom(_T("1.0"))
	, m_str_SpecYzoom2(_T("1.0"))
	, m_str_FFTSpecYzoom(_T("1.0"))
	, m_str_FFTSpecYzoom2(_T("1.0"))
	, m_str_MelSpecYzoom(_T("1.0"))
	, m_str_MelSpecYzoom2(_T("1.0"))
{

}

COperateDlg::~COperateDlg()
{
}

void COperateDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_XPOS, m_Str_XPos);
	DDX_Text(pDX, IDC_EDIT_XZOOM, m_Str_XZoom);
	DDX_Text(pDX,IDC_EDIT1,m_str_SpecYzoom);
	DDX_Text(pDX,IDC_EDIT2,m_str_SpecYzoom2);
	DDX_Text(pDX,IDC_EDIT3,m_str_FFTSpecYzoom);
	DDX_Text(pDX,IDC_EDIT4,m_str_FFTSpecYzoom2);
	DDX_Text(pDX,IDC_EDIT5,m_str_MelSpecYzoom);
	DDX_Text(pDX,IDC_EDIT6,m_str_MelSpecYzoom2);

	DDX_Control(pDX, IDC_SCROLLBAR_XSHOWPOS, m_X_ShowPos);
	DDX_Control(pDX, IDC_SLIDER_XZOOM, m_XZoom);
	DDX_Text(pDX, IDC_EDIT_YZOOM, m_Str_YZoom);
	DDX_Text(pDX, IDC_EDIT_YPOS, m_Str_YPos);
	DDX_Control(pDX, IDC_SCROLLBAR_YSHOWPOS, m_Y_ShowPos);
	DDX_Control(pDX, IDC_SLIDER_YZOOM, m_YZoom);
	DDX_Control(pDX, IDC_LIST_FILE_INFO, m_List_FileInfo);
	DDX_Control(pDX, IDC_SLIDER_SPECTRUM, m_SpecYzoom);
	DDX_Control(pDX, IDC_SLIDER_FFTSPECTRUM, m_FFTSpecYzoom);
	DDX_Control(pDX, IDC_SLIDER_MELSPECTRUM, m_MelSpecYzoom);
	DDX_Control(pDX, IDC_SLIDER_SPECTRUM2, m_SpecYzoom2);
	DDX_Control(pDX, IDC_SLIDER_FFTSPECTRUM2, m_FFTSpecYzoom2);
	DDX_Control(pDX, IDC_SLIDER_MELSPECTRUM2, m_MelSpecYzoom2);
}

BEGIN_MESSAGE_MAP(COperateDlg, CFormView)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY(NM_THEMECHANGED, IDC_SCROLLBAR_XSHOWPOS, &COperateDlg::OnNMThemeChangedScrollbarXshowpos)
END_MESSAGE_MAP()


// COperateDlg 诊断

#ifdef _DEBUG
void COperateDlg::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void COperateDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// COperateDlg 消息处理程序

void COperateDlg::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	// TODO: 在此添加专用代码和/或调用基类
    CMyWaveDoc* pDoc=( CMyWaveDoc*)GetDocument();
	pDoc->m_hWndCtrl =m_hWnd;
	m_pView=(CMyWaveView*)CView::FromHandle (pDoc->m_hWndView);

	//设置x轴滑块初始化
	m_XZoom.SetRange (1,500);//设置滑块的滑动范围上下限值
	m_XZoom.SetTicFreq (50);//设置对每一个滑块控件的增量，刻度线的频率
	m_XZoom.SetPageSize (50);//设置一个滑块控件的页大小
	m_XZoom.SetPos (100);//设置滑块的当前位置
    
	//显示x方向的缩放比例值
	m_Str_XZoom.Format (_T("%.2f"),1.0);
	UpdateData(FALSE);
	//显示x滚动条的值
	m_Str_XPos.Format(_T("%d"),0);
	UpdateData(FALSE);
   
	//设置Y轴滑块
	m_YZoom.SetRange (3,300);
	m_YZoom.SetTicFreq (5);
	m_YZoom.SetPageSize (5);
	m_YZoom.SetPos (3);
   
	m_Str_YZoom.Format (_T("%.2f"),0.01);
	UpdateData(FALSE);
	/*********************************************************/
	//设置频谱Y方向滑块
	m_SpecYzoom.SetRange(1,100);
	m_SpecYzoom.SetTicFreq(5);
	m_SpecYzoom.SetPageSize(1);
	m_SpecYzoom.SetPos(10);
	//m_SpecYzoom.SetPos(100);
	//设置频谱2Y方向滑块
	m_SpecYzoom2.SetRange(1,100);
	m_SpecYzoom2.SetTicFreq(5);
	m_SpecYzoom2.SetPageSize(1);
	m_SpecYzoom2.SetPos(10);
	//m_SpecYzoom2.SetPos(100);
	//设置二次FFT的Y方向滑块
	m_FFTSpecYzoom.SetRange(1,100);
	m_FFTSpecYzoom.SetTicFreq(5);
	m_FFTSpecYzoom.SetPageSize(1);
	m_FFTSpecYzoom.SetPos(10);
	//设置二次FFT2的Y方向滑块
	m_FFTSpecYzoom2.SetRange(1,100);
	m_FFTSpecYzoom2.SetTicFreq(5);
	m_FFTSpecYzoom2.SetPageSize(1);
	m_FFTSpecYzoom2.SetPos(10);
	//设置Mel频谱的Y方向滑块
	m_MelSpecYzoom.SetRange(1,100);
	m_MelSpecYzoom.SetTicFreq(5);
	m_MelSpecYzoom.SetPageSize(1);
	m_MelSpecYzoom.SetPos(10);
	//设置Mel频谱2的Y方向滑块
	m_MelSpecYzoom2.SetRange(1,100);
	m_MelSpecYzoom2.SetTicFreq(5);
	m_MelSpecYzoom2.SetPageSize(1);
	m_MelSpecYzoom2.SetPos(10);
	/*********************************************************/

	//设置y轴滚动条信息
	SCROLLINFO y_info;//滚动条的信息
	y_info.cbSize =sizeof(SCROLLINFO);//设置大小
	y_info.fMask =SIF_ALL;// 表示Get后将使用y_info结构的位置、页面大小等量；SIF_ALL:整个结构都有效
	y_info.nMax =65500;//滚动范围最大值
	y_info.nMin =0;//滚动范围最小值
	y_info.nPage =300;//页尺寸，用来确定比例滚动框的大小
	y_info.nPos =0;//滚动框的位置
	y_info.nTrackPos =0;//拖动时滚动框的位置。该参数只能查询，不能设置
	m_Y_ShowPos.SetScrollInfo (&y_info);
	m_Y_ShowPos.SetScrollPos (m_Y_ShowPos.GetScrollLimit ());
	m_Str_YPos.Format (_T("%d"),-32800);
	UpdateData(FALSE);

}

BOOL COperateDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
    if(pMsg->hwnd ==m_XZoom.m_hWnd && pMsg->message ==WM_MOUSEWHEEL)//滑块X轴方向
	{
		
		if(m_XZoom.GetPos ()>=1&&m_XZoom.GetPos ()<=500)
		{ 
			short zDelta=(short)HIWORD(pMsg->wParam);
			if(m_XZoom.GetPos ()==1&&zDelta>0)
				return TRUE;
			if(m_XZoom.GetPos()==500 && zDelta<0)
				return TRUE;
			pMsg->wParam =zDelta/120;
			int x_zoom=m_XZoom.GetPos ()-zDelta/120;
            double zoom=(float)x_zoom/100.0;
			m_pView->SetXZoom (zoom);////
			m_XZoom.SetPos (x_zoom);
			m_Str_XZoom.Format(_T("%.2f"),zoom);
			UpdateData(FALSE);
		}
	}
	else if(pMsg->hwnd==m_YZoom.m_hWnd && pMsg->message==WM_MOUSEWHEEL)//滑块Y轴方向
	{
		if(m_YZoom.GetPos()>=3 && m_YZoom.GetPos()<=300)
		{
			short zDelta = (short) HIWORD(pMsg->wParam);
			if(m_YZoom.GetPos()==3 && zDelta>0)
				return TRUE;
			if(m_YZoom.GetPos()==300 && zDelta<0)
				return TRUE;
			pMsg->wParam=zDelta/120;
			int y_zoom=m_YZoom.GetPos()-3*zDelta/120;
			double zoom=(float)y_zoom/300.0;
			m_pView->SetYZoom(zoom);
			m_YZoom.SetPos(y_zoom);
			m_Str_YZoom.Format(_T("%.2f"),zoom);
			UpdateData(FALSE);
		}
	}
	else if(pMsg->hwnd==m_SpecYzoom.m_hWnd && pMsg->message==WM_MOUSEWHEEL)//滑块Y轴方向
	{
		if(m_SpecYzoom.GetPos()>=1 && m_SpecYzoom.GetPos()<=100)
		{
			short zDelta = (short) HIWORD(pMsg->wParam);
			if(m_SpecYzoom.GetPos()==1 && zDelta>0)
				return TRUE;
			if(m_SpecYzoom.GetPos()==100 && zDelta<0)
				return TRUE;
			pMsg->wParam=zDelta/120;
			int y_zoom=m_SpecYzoom.GetPos()-1*zDelta/120;
			double zoom=(float)y_zoom/10.0;
			m_pView->SetSpecYzoom(zoom);
			m_SpecYzoom.SetPos(y_zoom);
			m_str_SpecYzoom.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);

		}
	}
	else if(pMsg->hwnd==m_SpecYzoom2.m_hWnd && pMsg->message==WM_MOUSEWHEEL)//滑块Y轴方向
	{
		if(m_SpecYzoom2.GetPos()>=1 && m_SpecYzoom2.GetPos()<=100)
		{
			short zDelta = (short) HIWORD(pMsg->wParam);
			if(m_SpecYzoom2.GetPos()==1 && zDelta>0)
				return TRUE;
			if(m_SpecYzoom2.GetPos()==100 && zDelta<0)
				return TRUE;
			pMsg->wParam=zDelta/120;
			int y_zoom=m_SpecYzoom2.GetPos()-1*zDelta/120;
			//double zoom=(float)y_zoom/100.0;
			double zoom=(float)y_zoom/10.0;
			m_pView->SetSpecYzoom2(zoom);
			m_SpecYzoom2.SetPos(y_zoom);
			m_str_SpecYzoom2.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);
		}
	}
	else if(pMsg->hwnd==m_FFTSpecYzoom.m_hWnd && pMsg->message==WM_MOUSEWHEEL)//滑块Y轴方向
	{
		if(m_FFTSpecYzoom.GetPos()>=1 && m_FFTSpecYzoom.GetPos()<=100)
		{
			short zDelta = (short) HIWORD(pMsg->wParam);
			if(m_FFTSpecYzoom.GetPos()==1 && zDelta>0)
				return TRUE;
			if(m_FFTSpecYzoom.GetPos()==100 && zDelta<0)
				return TRUE;
			pMsg->wParam=zDelta/120;
			int y_zoom=m_FFTSpecYzoom.GetPos()-1*zDelta/120;
			double zoom=(float)y_zoom/10.0;
			m_pView->SetFFTSpecYzoom(zoom);
			m_FFTSpecYzoom.SetPos(y_zoom);
			m_str_FFTSpecYzoom.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);
		}
	}
	else if(pMsg->hwnd==m_FFTSpecYzoom2.m_hWnd && pMsg->message==WM_MOUSEWHEEL)//滑块Y轴方向
	{
		if(m_FFTSpecYzoom2.GetPos()>=1 && m_FFTSpecYzoom2.GetPos()<=100)
		{
			short zDelta = (short) HIWORD(pMsg->wParam);
			if(m_FFTSpecYzoom2.GetPos()==1 && zDelta>0)
				return TRUE;
			if(m_FFTSpecYzoom2.GetPos()==100 && zDelta<0)
				return TRUE;
			pMsg->wParam=zDelta/120;
			int y_zoom=m_FFTSpecYzoom2.GetPos()-1*zDelta/120;
			double zoom=(float)y_zoom/10.0;
			m_pView->SetFFTSpecYzoom2(zoom);
			m_FFTSpecYzoom2.SetPos(y_zoom);
			m_str_FFTSpecYzoom2.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);
		}
	}
	else if(pMsg->hwnd==m_MelSpecYzoom.m_hWnd && pMsg->message==WM_MOUSEWHEEL)//滑块Y轴方向
	{
		if(m_MelSpecYzoom.GetPos()>=1 && m_MelSpecYzoom.GetPos()<=100)
		{
			short zDelta = (short) HIWORD(pMsg->wParam);
			if(m_MelSpecYzoom.GetPos()==1 && zDelta>0)
				return TRUE;
			if(m_MelSpecYzoom.GetPos()==100 && zDelta<0)
				return TRUE;
			pMsg->wParam=zDelta/120;
			int y_zoom=m_MelSpecYzoom.GetPos()-1*zDelta/120;
			double zoom=(float)y_zoom/10.0;
			m_pView->SetMelSpecYzoom(zoom);
			m_MelSpecYzoom.SetPos(y_zoom);
			m_str_MelSpecYzoom.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);
		}
	}
	else if(pMsg->hwnd==m_MelSpecYzoom2.m_hWnd && pMsg->message==WM_MOUSEWHEEL)//滑块Y轴方向
	{
		if(m_MelSpecYzoom2.GetPos()>=1 && m_MelSpecYzoom2.GetPos()<=100)
		{
			short zDelta = (short) HIWORD(pMsg->wParam);
			if(m_MelSpecYzoom2.GetPos()==1 && zDelta>0)
				return TRUE;
			if(m_MelSpecYzoom2.GetPos()==100 && zDelta<0)
				return TRUE;
			pMsg->wParam=zDelta/120;
			int y_zoom=m_MelSpecYzoom2.GetPos()-1*zDelta/120;
			double zoom=(float)y_zoom/10.0;
			m_pView->SetMelSpecYzoom2(zoom);
			m_MelSpecYzoom2.SetPos(y_zoom);
			m_str_MelSpecYzoom2.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);
		}
	}
	return CFormView::PreTranslateMessage(pMsg);
}

void COperateDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    if(pScrollBar!=NULL)
	{
		int ctrlID=pScrollBar->GetDlgCtrlID ();
		if(ctrlID==IDC_SCROLLBAR_XSHOWPOS)//拖动x轴滚动条时的响应
		{
			int max,min;
			int cur=pScrollBar->GetScrollPos ();
			pScrollBar->GetScrollRange (&min,&max);
			int maxpos=pScrollBar->GetScrollLimit ();//是滚动条的最大位置
			SCROLLINFO info;
			switch(nSBCode)
			{
			case SB_LEFT:                             //滚动到最左边
				cur=min;
				break;
			case SB_LINELEFT:                         //向左滚动
				cur-=20;
				if(cur<min)
					cur=min;
				break;
			case SB_LINERIGHT:                        //向右滚动
				cur+=20;
				if(cur>maxpos)
					cur=maxpos;
				break; 
			case SB_PAGELEFT:                         //向左滚动一页
				pScrollBar->GetScrollInfo (&info,SIF_ALL);
				cur=cur-info.nPage ;
				if(cur<min)
					cur=min;
				break;
			case SB_PAGERIGHT:                       //向右滚动一页
				pScrollBar->GetScrollInfo (&info,SIF_ALL);
				cur=cur+info.nPage ;
				if(cur>maxpos)
					cur=maxpos;
				break;
			case SB_RIGHT:                           //滚动到最右边
				cur=maxpos;
				break;
			case SB_THUMBTRACK:                     //将滚动块拖动到指定的位置。当前的位置由nTrackPos参数指定
				pScrollBar->GetScrollInfo (&info,SIF_ALL);
				cur=info.nTrackPos ;
				break;
			}
			pScrollBar->SetScrollPos (cur);
			m_Str_XPos.Format (_T("%d"),cur);
			UpdateData(FALSE);
			
		    m_pView->SetXPos(cur);//将cur值传给veiw
		}
		else if(ctrlID==IDC_SLIDER_XZOOM)//拖动x轴滑动控件时X滚动条的响应
		{
			int x_zoom=m_XZoom.GetPos ();
			double zoom=(float)x_zoom/100.0;
			m_pView->SetXZoom(zoom);
			m_Str_XZoom.Format (_T("%.2f"),zoom);

			int cur=m_X_ShowPos.GetScrollPos ();
			SCROLLINFO x_info;
			x_info.cbSize =sizeof(SCROLLINFO);
			x_info.fMask =SIF_ALL;
			x_info.nMax =m_pView->m_numOfPoint+(m_pView->rect.Width())/zoom;
			x_info.nMin =0;
			x_info.nPage =60;
			x_info.nPos =0;
			x_info.nTrackPos =0;
			m_X_ShowPos.SetScrollInfo (&x_info);
			m_X_ShowPos.SetScrollPos (cur);

			UpdateData(FALSE);
		}
	}
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void COperateDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    if(pScrollBar!=NULL)
	{
		int ctrlID=pScrollBar->GetDlgCtrlID();
		if(ctrlID==IDC_SCROLLBAR_YSHOWPOS)
		{
			int max,min;
			int cur=pScrollBar->GetScrollPos();
			pScrollBar->GetScrollRange(&min,&max);
			int maxpos=pScrollBar->GetScrollLimit();		//是滚动条的最大位置  比max略小 因为滚动框占一定空间
			SCROLLINFO info;
			switch(nSBCode)
			{		
			case SB_LEFT:									//滚动到最左边			
				cur=min;
				break;
			case SB_LINELEFT:
				cur-=300;
				if(cur<min)
					cur=min;
				break;
			case SB_LINERIGHT:
				cur+=300;
				if(cur>maxpos)
					cur=maxpos;
				break;
			case SB_PAGELEFT:
				pScrollBar->GetScrollInfo(&info,SIF_ALL);
				cur=cur-info.nPage;
				if(cur<min)
					cur=min;
				break;
			case SB_PAGERIGHT:
				pScrollBar->GetScrollInfo(&info,SIF_ALL);
				cur=cur+info.nPage;
				if(cur>maxpos)
					cur=maxpos;
				break;
			case SB_RIGHT:									//滚动到最右边
				cur=maxpos;
				break;
			case SB_THUMBTRACK:
				pScrollBar->GetScrollInfo(&info,SIF_ALL);
				cur=info.nTrackPos;
				break;
			}
			pScrollBar->SetScrollPos(cur);
			m_Str_YPos.Format(_T("%d"),maxpos-cur-32800);
			UpdateData(FALSE);

			m_pView->SetYPos(maxpos-cur);
		}
		else if(ctrlID==IDC_SLIDER_YZOOM)
		{
			int y_zoom=m_YZoom.GetPos();
			double zoom=(float)y_zoom/300.0;
			m_pView->SetYZoom(zoom);
			m_Str_YZoom.Format(_T("%.2f"),zoom);
			UpdateData(FALSE);
		}
		else if (ctrlID==IDC_SLIDER_SPECTRUM)
		{
			int y_zoom=m_SpecYzoom.GetPos();
			//double zoom=(float)y_zoom/100.0;
			double zoom=(float)y_zoom/10.0;
			m_pView->SetSpecYzoom(zoom);
			m_str_SpecYzoom.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);
		}
		else if (ctrlID==IDC_SLIDER_SPECTRUM2)
		{
			int y_zoom=m_SpecYzoom2.GetPos();
			//double zoom=(float)y_zoom/100.0;
			double zoom=(float)y_zoom/10.0;
			m_pView->SetSpecYzoom2(zoom);
			m_str_SpecYzoom2.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);
		}
		else if (ctrlID==IDC_SLIDER_FFTSPECTRUM)
		{
			int y_zoom=m_FFTSpecYzoom.GetPos();
			double zoom=(float)y_zoom/10.0;
			m_pView->SetFFTSpecYzoom(zoom);
			m_str_FFTSpecYzoom.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);
		}
		else if (ctrlID==IDC_SLIDER_FFTSPECTRUM2)
		{
			int y_zoom=m_FFTSpecYzoom2.GetPos();
			double zoom=(float)y_zoom/10.0;
			m_pView->SetFFTSpecYzoom2(zoom);
			m_str_FFTSpecYzoom2.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);
		}
		else if (ctrlID==IDC_SLIDER_MELSPECTRUM)
		{
			int y_zoom=m_MelSpecYzoom.GetPos();
			double zoom=(float)y_zoom/10.0;
			m_pView->SetMelSpecYzoom(zoom);
			m_str_MelSpecYzoom.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);
		}
		else if (ctrlID==IDC_SLIDER_MELSPECTRUM2)
		{
			int y_zoom=m_MelSpecYzoom2.GetPos();
			double zoom=(float)y_zoom/10.0;
			m_pView->SetMelSpecYzoom2(zoom);
			m_str_MelSpecYzoom2.Format(_T("%.1f"),zoom);
			UpdateData(FALSE);
		}
	}


	CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
}
void COperateDlg::UpdateFileInfoCtrl(WAVEFILEHEADER header,WAVEFILEHEADER header2,WAVEFILEHEADER header3)
{
	CMyWaveDoc* pDoc=(CMyWaveDoc*)GetDocument();

	int num=m_List_FileInfo.GetCount ();
	for(int i=0;i<num;i++)
		m_List_FileInfo.DeleteString (0);

	CString str;
	if(pDoc->m_strFileName!=_T(""))
	{
		str.Format (_T("文件名1：\t"));
		str+=pDoc->m_strFileName ;
		m_List_FileInfo.AddString (str);
		str.Format (_T("通道数:\t\t%d"),header.fmt .wavFormat .wChannels );
		m_List_FileInfo.AddString (str);
		str.Format(_T("采样频率:\t%dHZ"),header.fmt.wavFormat.dwSamplesPerSec);
		m_List_FileInfo.AddString(str);
		str.Format(_T("采样大小:\t%d位"),header.fmt.wavFormat.wBitsPerSample);
		m_List_FileInfo.AddString(str);
	}
	if(pDoc->m_strFileName2!=_T(""))
	{
		str.Format(_T("文件名2:\t"));
		str+=pDoc->m_strFileName2;
		m_List_FileInfo.AddString(str);
		str.Format(_T("通道数:\t\t%d"),header2.fmt.wavFormat.wChannels);
		m_List_FileInfo.AddString(str);
		str.Format(_T("采样频率:\t%dHZ"),header2.fmt.wavFormat.dwSamplesPerSec);
		m_List_FileInfo.AddString(str);
		str.Format(_T("采样大小:\t%d位"),header2.fmt.wavFormat.wBitsPerSample);
		m_List_FileInfo.AddString(str);
	}
	if(pDoc->m_strFileName3!=_T(""))
	{
		str.Format(_T("文件名3:\t"));
		str+=pDoc->m_strFileName3;
		m_List_FileInfo.AddString(str);
		str.Format(_T("通道数:\t\t%d"),header3.fmt.wavFormat.wChannels);
		m_List_FileInfo.AddString(str);
		str.Format(_T("采样频率:\t%dHZ"),header3.fmt.wavFormat.dwSamplesPerSec);
		m_List_FileInfo.AddString(str);
		str.Format(_T("采样大小:\t%d位"),header3.fmt.wavFormat.wBitsPerSample);
		m_List_FileInfo.AddString(str);
	}
	

}
void COperateDlg::OnNMThemeChangedScrollbarXshowpos(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
