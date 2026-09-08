#pragma once


// CshowFFT 对话框
#include"Resource.h"
#include "MyWaveDoc.h"
#include "MyWaveView.h"
#include"Matrix.h"
#include<vector>
class CshowFFT : public CDialog
{
	DECLARE_DYNAMIC(CshowFFT)

public:
	CshowFFT(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CshowFFT();

// 对话框数据
	enum { IDD = IDD_fft};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:


	Matrix rawData;
	unsigned int nbData;
	 CRect rect;
	

	//解决闪屏问题
	//CBitmap m_bitmap;
	//CDC m_dcCompatible;
	//坐标轴坐上与右下的坐标
	int m_left;
	int m_top;
	int m_bottom;
	int m_right;
	

	afx_msg void OnSize(UINT nType, int cx, int cy);
	/////////////////////
	CWnd *pWnd;                  //获取控件窗口类指针
    CDC *pDC;                    //获取控件的CDC指针
	CDC dcMem;
	CBitmap m_bitmap;
	bool flag;
	double *mfcc;
	double *mfcc2;

public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButton1();
	void Draw(CDC* pDC,double *mfcc,int row,int col,COLORREF crColor);
	void loadFile(CString filename,double *mfcc,int &row,int &col);//读取文件
public:
	afx_msg void OnBnClickedButton2();
};
