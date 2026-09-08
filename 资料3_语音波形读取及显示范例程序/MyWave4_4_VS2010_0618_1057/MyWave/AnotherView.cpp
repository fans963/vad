// AnotherView.cpp : 实现文件
//

#include "stdafx.h"
#include "AnotherView.h"
#include"MyWave.h"
#include"WaveConvertor.h"
#include"Speech.h"
// CAnotherView

IMPLEMENT_DYNCREATE(CAnotherView, CView)

CAnotherView::CAnotherView()
{
	m_nGraphType = eMODE_SPECTRUM;
	m_clrBkgd = RGB(0, 0, 0);
	m_clrLines = RGB(75, 243, 167);
	m_clrText = RGB(98, 108, 104);
	pRealOut=NULL;
	pImageOut=NULL;
	pfMagnitudeOut=NULL;
	pfFrequencyOut=NULL;
}

CAnotherView::~CAnotherView()
{
	if(pRealOut!=NULL)
		delete []pRealOut;
    if(pImageOut!=NULL)
		delete []pImageOut;
	if(pfMagnitudeOut!=NULL)
		delete []pfMagnitudeOut;
	if(pfFrequencyOut!=NULL)
		delete []pfFrequencyOut;


}

BEGIN_MESSAGE_MAP(CAnotherView, CView)
END_MESSAGE_MAP()


// CAnotherView 绘图

void CAnotherView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
	DrawBkgd(pDC);
	DrawGraphics(pDC);
    DrawSpectrum( pDC,1024,pfMagnitudeOut,  pfFrequencyOut);


	
}
CMyWaveDoc* CAnotherView::GetDocument() const// 非调试版本是内联的
{
	return (CMyWaveDoc*)m_pDocument;
}

// CAnotherView 诊断

#ifdef _DEBUG
void CAnotherView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CAnotherView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAnotherView 消息处理程序
void CAnotherView::DrawBkgd(CDC* pDC)
{
    GetClientRect(&rect);
	pDC->Rectangle (&rect);
    

	m_left=rect.left+35;
	m_top=rect.top +20;
	m_right=rect.right -20;
	m_bottom=rect.bottom-30;

	//创建画笔对象
	CPen* pPenLine=new CPen;
	//画线画笔
	pPenLine->CreatePen (PS_SOLID,1,RGB(100,100,100));
	//创建画笔对象
	CPen* pPenDot=new CPen;
	//网格画笔
	pPenDot->CreatePen (PS_DOT,1,RGB(100,100,100));
	//选中当前画线画笔，并保存以前的画笔
	CGdiObject* pOldPen=pDC->SelectObject (pPenLine);
    


	//绘制坐标轴
	pDC->MoveTo(m_left,m_top);
	//垂直轴
	pDC->LineTo (m_left,m_bottom);
	pDC->LineTo (m_right,m_bottom);

	//绘制Y轴箭头
	pDC->MoveTo (m_right-5,m_bottom-5);
	pDC->LineTo (m_right,m_bottom);
	pDC->LineTo(m_right-5,m_bottom+5);

	//绘制X轴箭头
    pDC->MoveTo(m_left-5,m_top+5);
	pDC->LineTo(m_left,m_top);
	pDC->LineTo(m_left+5,m_top+5);

	pDC->MoveTo(m_left,m_top);
	pDC->LineTo(m_right,m_top);
	pDC->MoveTo(m_right,m_top);
	pDC->LineTo(m_right,m_bottom);

     pDC->TextOut(m_right-20, m_bottom - 20, _T("Hz"));
	 pDC->TextOut(m_left + 10, m_top + 10, _T("dB"));

	int nIndex=0;
	CString strText;
	 // Draw scales of x-axis  X轴标度
	int nCnt = 4;
	int i;
	for(i = 0; i < m_right; i += 50)
	{
		if(i > 0)
		{
			pDC->MoveTo(m_left+i,m_bottom + 3);
			pDC->LineTo(m_left + i, m_bottom + 7);
			strText.Format(_T("%d"), nIndex * nCnt * 50);
			pDC->TextOut(m_left + i - 8, m_bottom + 10, strText);
		}
		nIndex++;
	}

	// Draw scales of y-axis   Y轴标度
	nIndex = 0;
	for(i = 0; i <m_bottom-50; i += 20)
	{
		if(i > 0)
		{
			pDC->MoveTo(m_left, m_top+ i);
			pDC->LineTo(m_left + 3, m_top+ i);
			strText.Format(_T("-%3d"), nIndex * 20);
			pDC->TextOut(m_left-30, m_top+i - 5, strText);
		}
		nIndex++;
	}

}
void CAnotherView::DrawGraphics(CDC* pDC)
{
	CMyWaveDoc* pDoc=(CMyWaveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
		return;
     unsigned int m_nFrameSize=1024;
	 unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-m_nFrameSize)/(m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*m_nFrameSize;
	 int i;
	 double *data;
		
	data=new double[sample_rate];
    //for(i=0;i<num;i++)
		//data[i]=2+5*cos(2*PI*100*i+30*PI/180)+3*cos(2*PI*200*i-60*PI/180);
	//ofstream fout( "222.txt", ios::app );     
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i]; 
		//fout << pDoc->data_one[i]<< endl;
	}
	for(i=num;i<sample_rate;i++)
	{  data[i]=0;
	}
	
 //关闭输出流 
        //fout.close();   
	// 为FFT输出数据分配空间
		
	    pRealOut = new double[sample_rate];
	    pImageOut = new double[sample_rate];
	    pfMagnitudeOut = new double[sample_rate];
		pfFrequencyOut= new double[sample_rate];
		double *dataout;
		dataout=new double[sample_rate];
    //加窗
    	CSpeech::AddWindow (m_nFrameSize ,data,num);
	// 进行FFT变换
	   CWaveConvertor::ConvertToFFT(sample_rate,m_nFrameSize, data, pRealOut, pImageOut);
	  /* CWaveConvertor::ConvertToLogPowerSpectral(
					sample_rate, m_nFrameSize, data, dataout);*/
     
	   double fMaxMagnitude=0.0;
	   int nHalf=m_nFrameSize/2;// Number of positive frequencies
	   unsigned int nSamplingRate=pDoc->m_HZ;//采样频率
	 for(int i = 0; i < nHalf; i++)
    {
		// Calculate the amplitude value
		pfMagnitudeOut[i] = pRealOut[i] * pRealOut[i] + pImageOut[i] *  pImageOut[i];
		pfMagnitudeOut[i] = sqrt(pfMagnitudeOut[i]);
		
		// Note: The first point is the direct current component
		if(i == 0) pfMagnitudeOut[i] /= m_nFrameSize;
		else pfMagnitudeOut[i] /= nHalf;

		if(pfMagnitudeOut[i] > fMaxMagnitude)
		{
			fMaxMagnitude = pfMagnitudeOut[i];
		}
    }
	   for(int j=0;j<nHalf;j++)
	   {
		   pfFrequencyOut[j] = (float)nSamplingRate * j / float(m_nFrameSize);	
		   pfMagnitudeOut[j] = 20.f * log10(pfMagnitudeOut[j] / fMaxMagnitude);
	   }



	 




}
void CAnotherView::DrawSpectrum(CDC* pDC,unsigned int nNumSamples,double* pfMagnitude, double* pfFrequency)
{

	int nX, nY;
    CPen* pPenLine=new CPen;
	//画线画笔
	pPenLine->CreatePen (PS_SOLID,1,RGB(100,100,100));
	//创建画笔对象
	CPen* pPenDot=new CPen;
	//网格画笔
	pPenDot->CreatePen (PS_DOT,1,RGB(100,100,100));
	//选中当前画线画笔，并保存以前的画笔
	CGdiObject* pOldPen=pDC->SelectObject (pPenLine);
    
	//if(m_penLine.GetSafeHandle())
	//{
	//pOldPen = pDC->SelectObject(&m_penLine);
	//}
	
	for(int i = 0; i < nNumSamples; i++)
	{
		/*if(bLeft)
		{*/
			nX =m_left + abs(pfFrequency[i]) / 4;
			nY =m_top;
	/*	}*/
		/*else
		{
			nX = m_rcWindowR.left + abs(pfFrequency[i]) / 4 ;
			nY = m_rcWindowR.top;
		}*/

		if(i == 1)
		{
			pDC->MoveTo(nX, nY - (int)pfMagnitude[i]);
			continue;
		}
		pDC->LineTo(nX, nY - (int)pfMagnitude[i]);
	}

	//if(pOldPen) pDC->SelectObject(pOldPen);
}