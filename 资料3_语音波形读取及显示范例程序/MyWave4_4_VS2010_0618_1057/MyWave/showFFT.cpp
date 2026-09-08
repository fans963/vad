// showFFT.cpp : 实现文件
//

#include "stdafx.h"
#include "showFFT.h"
#include <math.h>

#include<complex>
#include <iostream>
#include <fstream>
using namespace std;
#include "Speech.h"
#include"GaussianMixture.h"

#define PI          3.1415926
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
int n;
// CshowFFT 对话框
unsigned long int fundamental_frequency;
IMPLEMENT_DYNAMIC(CshowFFT, CDialog)

CshowFFT::CshowFFT(CWnd* pParent /*=NULL*/)
	: CDialog(CshowFFT::IDD, pParent)
{
	//pi=4*atan((double)1);
	//vector=NULL;
	nbData=0;
	flag=0;
	rawData.Zero();
	nbData=0;
	mfcc=NULL;
	mfcc2=NULL;

}

CshowFFT::~CshowFFT()
{
	if(mfcc!=NULL)
		delete []mfcc;
	if(mfcc2!=NULL)
		delete []mfcc2;
}

void CshowFFT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

}


BEGIN_MESSAGE_MAP(CshowFFT, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//ON_WM_ERASEBKGND()
	//ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CshowFFT::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CshowFFT::OnBnClickedButton2)
END_MESSAGE_MAP()


// CshowFFT 消息处理程序

void CshowFFT::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	CString str;
	int i, j;
	int x, y;
	//强制更新绘图, 不可少, 否则绘图会出错
			//使static控件区域无效
	pWnd->InvalidateRect(rect,true);
	//		//更新窗口, 此时才真正向系统发送重绘消息, 没有这句你可以试下, 绝对出问题
	pWnd->UpdateWindow();

	//BitBlt函数对指定的源设备环境区域中的像素进行位块（bit_block）转换，以传送到目标设备环境。
	
	CPen *pPenRed = new CPen(); //创建画笔对象
	pPenRed ->CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); //红色画笔
	CPen *pPen = NULL;
	//选中当前红色画笔,并保存以前的画笔
	CGdiObject *pOldPen = pDC ->SelectObject(pPenRed);
	pDC ->MoveTo(20,20); //绘制坐标轴
	pDC ->LineTo(20,rect.Height()-30); //竖起轴
	pDC ->LineTo(rect.Width()-20,rect.Height()-30); //水平轴
	//写X轴刻度值
	for(i =0; i <= 15; i ++)
	{
		str.Format(_T("%d"), i);
		pDC ->TextOut(17 + 30 * i, rect.Height()-20, str);
		//绘制X轴刻度
		pDC ->MoveTo(i * 30 + 20,rect.Height()-20);
		pDC ->LineTo(i * 30 + 20,rect.Height()-25);
	}

	//写Y轴刻度值
	double temp=(rect.Height()-30);
	for(i =-100; i <100; i +=5)
	{
		str.Format(_T("%d"), i);
		pDC ->TextOut(2, (rect.Height()-30)/2- i/5*temp/40 - 5, str);
		//绘制Y轴刻度
		pDC ->MoveTo(25, (rect.Height()-30)/2-i/5*temp/40);
		pDC ->LineTo(20, (rect.Height()-30)/2- i/5*temp/40);
	}
	//绘制X箭头
	pDC ->MoveTo(rect.Width()-30, rect.Height()-33);
	pDC ->LineTo(rect.Width()-20, rect.Height()-30);
	pDC ->LineTo(rect.Width()-30, rect.Height()-27);
	//str.Format(_T("%d"), i);
	pDC ->TextOut(rect.Width()-50, rect.Height()-20, _T("维数"));
	//绘制Y箭头
	pDC ->MoveTo(17, 30);
	pDC ->LineTo(20, 20);                   //绘制左边箭头
	pDC ->LineTo(23, 30); //绘制右边箭头
    pDC ->TextOut(0, 0, _T("幅度"));

	CPen pen[5];
	pen[0].CreatePen(PS_SOLID,1,RGB(255,128,0));//创建实线画笔
 // //pen[1].CreatePen(PS_DASH,1,RGB(0,255,0));//创建虚线画笔
 // //pen[2].CreatePen(PS_DOT,1,RGB(0,0,255));//点线
 // //pen[3].CreatePen(PS_DASHDOT,1,RGB(0,255,255));//点划线
 // //pen[4].CreatePen(PS_DASHDOTDOT,1,RGB(255,0,255));//双点划线

 //   pOldPen=pDC->SelectObject(&pen[0]);
	////double mel[10][13]={-5.02393 , 1.09326 , 0.469189 , -0.230786,  -0.421278 , -0.48329 , 0.379981 , -1.17665 , 0.419627 , -0.55132 , 0.792557 , -0.750468 , 0.440839,  
	////                 -3.18789,  0.574233 , 0.363863  ,-0.41863,  0.184026,  -0.986378 , 0.641031 , -0.977108 , 0.780213 , -0.723344 , 0.606649,  -0.47774 , 0.398427,
	////                 -1.08149,  0.240622 , -0.0530125 , -0.410149 , 0.48412,  -1.05859 , 0.396024 , -1.35695 , 0.683337,  -0.645063 , 0.573949,  -0.526931,  0.44615  ,
	////				-4.1076 , -0.315441,  1.66769 , -0.603667,  -0.287212  ,-1.08607 , 1.74764,  -0.116758,  1.25056 , -0.806836,  0.731041,  -0.666783,  0.503674  ,
	////				-0.185922 , -1.14281 , 1.58545  ,-1.79472,  1.4451 , -1.78163 , 1.21371,  -1.52813 , 1.36814,  -1.13224,  0.960691,  -0.867547,  0.713254,  
	////				0.0134018 , -0.643831 , 0.777136,  -1.18025,  1.0251 , -1.32931 , 0.648754 , -1.41926 , 1.07004,  -0.957907,  0.842127,  -0.789113 , 0.657462 ,
	////				-2.47029 , -0.483514 , 1.25038,  -0.793065 , 0.288599 , -1.17497 , 0.867681 , -1.13251 , 0.729142 , -0.749238 , 0.759627,  -0.636995 , 0.468214,  
	////				-7.3641 , 2.58705,  -0.939359 , 1.381,  -1.49534 , -0.174683 , 0.444334,  -0.00411949 , 0.634653 , -0.799292,  0.700762 , -0.44865 , 0.382454  ,
	////				-8.71041 , 0.452299 , 1.93055 , 0.29595,  -1.33304 , -1.03522 , 2.05733 , 0.871409 , 1.12284 , -0.94152 , 0.68228  ,-0.361778,  0.381892  ,
	////				-1.96748 , 0.55674 , 0.286376,  -1.04215 , 0.913598 , -1.17614 , 0.431087,  -1.50029 , 0.7738 , -0.716038 , 0.695785,  -0.648616 , 0.423955  };
	////
	////for(j=0;j<10;j++) 
	////{
	////	for(i=0;i<13;i++)
	////	{
	////		if(i==0)
	////			pDC->MoveTo(20 + 30 * i,temp/2-mel[j][i]/5*temp/8);
	////		pDC->LineTo(20 + 30 * i,temp/2-mel[j][i]/5*temp/8);

	////	}
	////}


	// GaussianMixture g;		
	// CString  FileName;  

	// CFileDialog dlg(true,NULL,L"",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,L"MFCC特征（*.txt）|*.txt|所有文件(*.*)|*.*||");
 //   
	//if(IDOK==dlg.DoModal ())
	//{
	//	FileName=dlg.GetPathName ();
 //       
	//	rawData=g.loadDataFile(FileName);
	//	nbData +=rawData.RowSize();        
	//}
	//
	
		//double temp=(rect.Height()-30);
	//int i,j;
	//if(flag==1)
	//{
	//for(i=0;i<nbData;i++)
	//{
	//	for(j=0;j<rawData.ColumnSize();j++)
	//	{
	//	   if(j==0)
	//		  pDC->MoveTo(20 + 30 * j,temp/2-rawData(i,j)/5*temp/40);
	//	pDC->LineTo(20 + 30 * j,temp/2-rawData(i,j)/5*temp/40);

	//	}
	//}
	//}
	//pDC->BitBlt(rect.left ,rect.top,rect.Width(),rect.Height(),&dcMem,0,0,SRCCOPY); //SRCCOPY：将源矩形区域直接拷贝到目标矩形区域。
	//恢复以前的画笔
	pDC ->SelectObject(pOldPen);
	delete pPenRed;
	//delete []pen;
	if(pPen != NULL)
	delete pPen;
}

void CshowFFT::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	//GetClientRect(&rcClient);
	//m_left=rcClient.left+12;
	//m_top=rcClient.top +10;
	//m_right=rcClient.right -12;
	//m_bottom=rcClient.bottom -25;

}
BOOL CshowFFT::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pWnd =GetDlgItem(IDC_STATIC);
	pWnd->GetWindowRect(&rect);
     pDC = pWnd->GetDC();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CshowFFT::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

 	double valTmp;
	char tmp[1024];
	unsigned int l=0,c=0;
    int row=0,col=0;
	CString filename;
	 CFileDialog dlg(true,NULL,L"",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,L"MFCC特征（*.txt）|*.txt|所有文件(*.*)|*.*||");
    
	if(IDOK==dlg.DoModal ())
	{
		filename=dlg.GetPathName ();
		//loadFile(filename,mfcc,row,col); 
	ifstream f;
	f.open(filename);

	if(f.is_open())
	{
		//get number of row 获取行数
		while(!f.eof())
		{
			f.getline (tmp,1024);
			l++;
			if(l==1)
			{
				// get number of columns获取列数
				std::istringstream strm;
				strm.str(tmp);
				while(strm >> valTmp)
					c++;
			}
		}
		//result.Resize (l-1,c);//then the matrix can be allocated 矩阵分配空间
		row=l;
		col=c;
		mfcc=new double[row*col];
		f.clear();
		f.seekg(0);//returns to beginning of the file返回文件开始

		for(unsigned int i=0;i<row;i++)
		{
			f.getline(tmp,1024);
			std::istringstream strm;
			strm.str(tmp);
			for(unsigned int j=0;j<col;j++)
			{
				strm>>mfcc[i*col+j];
			}
		}
		f.close();
	}
	else
	{
	   AfxMessageBox(_T("Error opening file!"));
	}
		flag=1;
	}
	//int i,j;
	//double temp=0.0;
	//fstream fout("test.txt",ios::trunc);
	//for(i=0;i<row;i++)
	//{
	//	for(j=0;j<col;j++)
	//		fout<<(double)mfcc[i*col+j]<<" ";
	//}
	//fout.close();
	Draw(pDC,mfcc,row,col,RGB(0,255,0));
	
}
void CshowFFT::Draw(CDC* pDC,double *mfcc,int row,int col,COLORREF crColor)

{
	double temp=(rect.Height()-30);
	int i,j;
	CPen pen;
	pen.CreatePen(PS_SOLID,1,crColor);//创建实线画笔
 // //pen[1].CreatePen(PS_DASH,1,RGB(0,255,0));//创建虚线画笔
 // //pen[2].CreatePen(PS_DOT,1,RGB(0,0,255));//点线
 // //pen[3].CreatePen(PS_DASHDOT,1,RGB(0,255,255));//点划线
 // //pen[4].CreatePen(PS_DASHDOTDOT,1,RGB(255,0,255));//双点划线

    CGdiObject *pOldPen=pDC->SelectObject(&pen);
	
	if(flag==1)
	{
		for(i=0;i<row;i++)
		{			
			for(j=0;j<col;j++)
			{
			   if(j==0)				  
				   pDC->MoveTo(20 + 30 * j,temp/2-mfcc[i*col+j]/5*temp/40);
			pDC->LineTo(20 + 30 * j,temp/2-mfcc[i*col+j]/5*temp/40); 
			//pDC->SetPixel(20 + 30 * j,temp/2-mfcc[i*col+j]/5*temp/40,crColor);

			}
		}
	}
	pDC ->SelectObject(pOldPen);

	
}
void CshowFFT::loadFile(CString filename,double *mfcc,int &row,int &col)
{	

	double valTmp;
	char tmp[1024];
	unsigned int l=0,c=0;
	ifstream f;
	f.open(filename);
	if(f.is_open())
	{
		//get number of row 获取行数
		while(!f.eof())
		{
			f.getline (tmp,1024);
			l++;
			if(l==1)
			{
				// get number of columns获取列数
				std::istringstream strm;
				strm.str(tmp);
				while(strm >> valTmp)
					c++;
			}
		}
		//result.Resize (l-1,c);//then the matrix can be allocated 矩阵分配空间
		row=l;
		col=c;
		mfcc=new double[row*col];
		f.clear();
		f.seekg(0);//returns to beginning of the file返回文件开始

		for(unsigned int i=0;i<row;i++)
		{
			f.getline(tmp,1024);
			std::istringstream strm;
			strm.str(tmp);
			for(unsigned int j=0;j<col;j++)
			{
				strm>>mfcc[i*col+j];
			}
		}
		f.close();
	}
	else
	{
	   AfxMessageBox(_T("Error opening file!"));
	}
}
void CshowFFT::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	 double valTmp;
	char tmp[1024];
	unsigned int l=0,c=0;
    int row=0,col=0;
	CString filename;
	 CFileDialog dlg(true,NULL,L"",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,L"MFCC特征（*.txt）|*.txt|所有文件(*.*)|*.*||");
    
	if(IDOK==dlg.DoModal ())
	{
		filename=dlg.GetPathName ();
		//loadFile(filename,mfcc,row,col); 
	ifstream f;
	f.open(filename);

	if(f.is_open())
	{
		//get number of row 获取行数
		while(!f.eof())
		{
			f.getline (tmp,1024);
			l++;
			if(l==1)
			{
				// get number of columns获取列数
				std::istringstream strm;
				strm.str(tmp);
				while(strm >> valTmp)
					c++;
			}
		}
		//result.Resize (l-1,c);//then the matrix can be allocated 矩阵分配空间
		row=l;
		col=c;
		mfcc2=new double[row*col];
		f.clear();
		f.seekg(0);//returns to beginning of the file返回文件开始

		for(unsigned int i=0;i<row;i++)
		{
			f.getline(tmp,1024);
			std::istringstream strm;
			strm.str(tmp);
			for(unsigned int j=0;j<col;j++)
			{
				strm>>mfcc2[i*col+j];
			}
		}
		f.close();
	}
	else
	{
	   AfxMessageBox(_T("Error opening file!"));
	}
		flag=1;
	}
	//int i,j;
	//double temp=0.0;
	//fstream fout("test.txt",ios::trunc);
	//for(i=0;i<row;i++)
	//{
	//	for(j=0;j<col;j++)
	//		fout<<(double)mfcc[i*col+j]<<" ";
	//}
	//fout.close();
	Draw(pDC,mfcc2,row,col,RGB(0,0,255));
}
