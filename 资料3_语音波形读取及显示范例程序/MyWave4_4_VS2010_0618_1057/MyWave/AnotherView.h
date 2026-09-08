#pragma once
#include"MyWaveDoc.h"

// CAnotherView 视图

class CAnotherView : public CView
{
	DECLARE_DYNCREATE(CAnotherView)

public:
	CAnotherView();           // 动态创建所使用的受保护的构造函数
	virtual ~CAnotherView();
	//坐标轴坐上与右下的坐标

public:
	enum Graph_Type  //绘图类型
	{
		eMODE_SPECTRUM,
		eMODE_PEAK
	};
public:
	CRect rect;                  //客户区的大小
	int m_left;
	int m_top;
	int m_bottom;
	int m_right;
	UINT m_nGraphType; // 绘图类型
	COLORREF m_clrBkgd;//背景颜色
	COLORREF m_clrLines;//画线的颜色
	COLORREF m_clrText;//文字的颜色


	double *pRealOut,*pImageOut,*pfMagnitudeOut,*pfFrequencyOut;
public:
	void DrawBkgd(CDC* pDC); // Draw background of created window
	void DrawCoordinate(CDC* pDC, int nOrigX, int nOrigY);//Draw coodinates for spectrum and peak mode
	void DrawGraphics(CDC* pDC); // Draw graphics for data
	void DrawSpectrum(CDC* pDC,unsigned int nNumSamples,double* pfMagnitude, double* pfFrequency);

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
 public:
	CMyWaveDoc* GetDocument() const; 
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


