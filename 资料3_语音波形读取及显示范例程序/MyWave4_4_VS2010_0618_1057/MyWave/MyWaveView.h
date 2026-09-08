// MyWaveView.h : CMyWaveView 类的接口
//


#pragma once
#include "MyWaveDoc.h"
#include<vector>
#define NO_OPERATION 101 //没有选择移动对象
#define _FILE1       102//文件1
#define _FILE2       103//文件2
#define _FILE3       104//文件3
#define _CEP1        105//文件1的倒谱
#define _CEP2        106//文件2的倒谱
#define _CEP3        107//文件3的倒谱
#define _FFT1        108//文件1的频谱
#define _FFT2        109//文件2的频谱
#define _FFT3        110//文件3的频谱
#define _LPC1        111
#define _LPC2        112
#define _LPC3        113
#define _LPCC1        114
#define _LPCC2        115
#define _LPCC3        116
#define _MFCC1       117
#define _MFCC2       118
#define _MFCC3       119
#define _LogSpec1    120
#define _LogSpec2    121
#define _LogSpec3    122
#define _SpecFFT1    123
#define _SpecFFT2    124
#define _SpecFFT3    125
#define _MelSpec1    126
#define _MelSpec2    127
#define _MelSpec3    128
#define _CepDetect1  129
#define _CepDetect2  130


class CMyWaveView : public CView
{
protected: // 仅从序列化创建
	CMyWaveView();
	DECLARE_DYNCREATE(CMyWaveView)//

// 属性
public:
	CMyWaveDoc* GetDocument() const;
    CRect rect;                  //客户区的大小
	int m_extent;							//3个文件中的最大值
	int m_numOfPoint;						//3个文件中的最大值
     
	//鼠标形状
	HCURSOR hCursorCross;//在坐标轴中的鼠标样式
	HCURSOR hCursorTrans;//在平移时的鼠标样式
	HCURSOR hCursorLine;//移动选取线时的鼠标样式
	//区域播放的位置
	CPoint m_startPoint;
	CPoint m_endPoint;
    //文件1的播放位置
	int m_startPlayPos;
	int m_endPlayPos;
	//文件2的播放位置
	int m_startPlayPos2;
	int m_endPlayPos2;
	//文件3的播放位置
	int m_startPlayPos3;
	int m_endPlayPos3;
private:
	//解决闪屏问题
	CBitmap m_bitmap;
	CDC m_dcCompatible;
	//坐标轴坐上与右下的坐标
	int m_left;
	int m_top;
	int m_bottom;
	int m_right;

	double m_Xzoom,m_Yzoom;			//X Y轴的缩放比例
	/*******************三条曲线缩放比例****************/
	double m_Spec_Yzoom;//频谱
	double m_FFTSpec_Yzoom;//二次频谱
	double m_MelSpec_Yzoom;//Mel频谱

	double m_Spec_Yzoom2;//频谱曲线2
	double m_FFTSpec_Yzoom2;//二次频谱2
	double m_MelSpec_Yzoom2;//Mel频谱2
	/************************************************/
	int m_Xpos;						//X轴开始绘制的位置
	int m_Ypos;						//Y轴开始绘制的位置
    
	//控制是否显示文件1
	BOOL m_bShowFile1;             
	BOOL m_bShowFile2;
	BOOL m_bShowFile3;
	//////////////////////////////////新添加 控制是否显示倒谱
	BOOL m_bShowCep1;
	BOOL m_bShowCep2;

	BOOL m_bShowFft1;
	BOOL m_bShowFft2;

    BOOL m_bShowPitch;
	BOOL m_bShowPitch2;

	BOOL m_bShowLpc1;
    BOOL m_bShowLpc2;
	BOOL m_bShowLpc3;
	BOOL m_bShowLpcc1;
    BOOL m_bShowLpcc2;
    BOOL m_bShowLpcc3;
    BOOL m_bShowMfcc1;
    BOOL m_bShowMfcc2;
	BOOL m_bMutiDisplay;
	BOOL m_bSpecDisplay;
	BOOL m_bMfccDisplay;

	BOOL m_bMutiDisplay2;
	BOOL m_bSpecDisplay2;
	BOOL m_bMfccDisplay2;

	BOOL m_bSpecFFTDisplay;//控制是否显示频谱的fft
	BOOL m_bLogSpecHorizonDisplay;//控制是否显示水平的对数功率谱
	BOOL m_bMelSpecDisplay;//控制是否显示mel频谱
	BOOL m_bCepDetectDisplay;//控制显示基于倒谱的端点检测

	BOOL m_bSpecFFTDisplay2;//控制是否显示频谱的fft
	BOOL m_bLogSpecHorizonDisplay2;//控制是否显示水平的对数功率谱
	BOOL m_bMelSpecDisplay2;//控制是否显示mel频谱
	BOOL m_bCepDetectDisplay2;//控制显示基于倒谱的端点检测
	BOOL m_bfrmlen256;//控制显示设置帧长的选项
	BOOL m_bfrmlen512;
	BOOL m_bfrmlen1024;



	bool m_FlagArray[8];//记录8条曲线的显示标志
	int m_YposArray[8][2];//记录8条曲线的y方向起始位置,k1,k2
	int m_FlagCount;//当前显示的曲线数目


	//控制是否显示特征参数波形
	BOOL  m_lpctodata;

	//控制是否显示语谱图
    BOOL  m_showypt;

	
//与平移相关的变量
	int m_transID; //记录移动对象的ID号
	BOOL hasTransRect;//是否有平移的选中框
	BOOL m_bOnlyXTran;//是否只沿X轴平移
	BOOL m_bOnlyYTran;//是否只沿Y轴平移
     
	CRect transRect;//平移框
	BOOL m_bMouseDown;//鼠标是否按下
    BOOL  m_bStartDrawRect;
	BOOL m_bDrawing;
	//文件1平移时的平移量
	int m_X_File1_Dispos;
	int m_Y_File1_Dispos;
	//文件2平移时的平移量
	int m_X_File2_Dispos;
	int m_Y_File2_Dispos;
	//文件3平移时的平移量
	int m_X_File3_Dispos;
	int m_Y_File3_Dispos;
    //文件1的倒谱的平移量
	int m_X_Cep1_Dispos;
	int m_Y_Cep1_Dispos;

	int m_X_Cep2_Dispos;
	int m_Y_Cep2_Dispos;
	//文件1的频谱的平移量
	int m_X_Fft1_Dispos;
	int m_Y_Fft1_Dispos;

	int m_X_Fft2_Dispos;
	int m_Y_Fft2_Dispos;

	/**********************新添加曲线偏移************************/
	//文件1的对数功率谱的平移量
	int m_X_LogSpec1_Dispos;
	int m_Y_LogSpec1_Dispos;

	int m_X_LogSpec2_Dispos;
	int m_Y_LogSpec2_Dispos;
	//文件1的频谱的FFT的平移量
	int m_X_SpecFFT1_Dispos;
	int m_Y_SpecFFT1_Dispos;

	int m_X_SpecFFT2_Dispos;
	int m_Y_SpecFFT2_Dispos;
	//文件1的Mel频谱的平移量
	int m_X_MelSpec1_Dispos;
	int m_Y_MelSpec1_Dispos;

	int m_X_MelSpec2_Dispos;
	int m_Y_MelSpec2_Dispos;

	/************************************************************/
	//文件1倒谱的平移量,另一条曲线
	int m_X_IFFT1_Dispos;
	int m_Y_IFFT1_Dispos;

	int m_X_IFFT2_Dispos;
	int m_Y_IFFT2_Dispos;

	//文件1基音周期检测结果曲线的平移量
	int m_X_PITCH1_Dispos;
	int m_Y_PITCH1_Dispos;

	int m_X_PITCH2_Dispos;
	int m_Y_PITCH2_Dispos;

	//文件1的LPC的平移量
	int m_X_Lpc1_Dispos;
	int m_Y_Lpc1_Dispos;
	//文件2的LPC的平移量
	int m_X_Lpc2_Dispos;
	int m_Y_Lpc2_Dispos;
	//文件1的LPCC的平移量
	int m_X_Lpcc1_Dispos;
	int m_Y_Lpcc1_Dispos;
	//文件2的LPCC的平移量
	int m_X_Lpcc2_Dispos;
	int m_Y_Lpcc2_Dispos;
	//文件1的MFCC的平移量
	int m_X_Mfcc1_Dispos;
	int m_Y_Mfcc1_Dispos;
	//文件2的MFCC的平移量
	int m_X_Mfcc2_Dispos;
	int m_Y_Mfcc2_Dispos;

	CPoint m_oldPoint;   //鼠标按下时的点
	CPoint m_prePoint;   //鼠标按下移动中的点
	CString m_old_info;
	CString m_rect_info; //矩形选择框信息

//与平移有关的操作
	void SetTransRect();//确定一定区域
	void DrawTransRect(CDC *pDC);//绘制移动区域
	/////////////////////////////fft
	
	unsigned int pOutNum;
	unsigned int pOutNum2;
	unsigned int lpcNum1;
	unsigned int lpcNum2;
	unsigned int lpccNum1;
	unsigned int lpccNum2;
	unsigned int mfccNum1;
	unsigned int mfccNum2;
	unsigned int melspecNum1;	//ncount*filterNum
	unsigned int melspecNum2;

   /////////////////////////////////////////////////////////////频域分析所用变量
   // 每帧所包含样本数
	int m_nFrameSize;
	double* data;
	double* dataout;		//存放对数功率谱
	double* pitch;
	
	double* dataout_cep;	//存放倒谱数据
	double* dataout_spectrum;//存放频谱数据
	double* dataout_spectrumFFT;//存放频率振幅谱fft的数据

	/*************************************************/
	double* data_2;
	double* dataout_2;		//存放对数功率谱
	double* pitch_2;

	double* dataout_cep_2;	//存放倒谱数据
	double* dataout_spectrum_2;//存放频谱数据
	double* dataout_spectrumFFT_2;//存放频率振幅谱fft的数据

	/*************************************************/
	/////////////////////////////////////////////////////////////倒谱平移相关变量
    double maxForOneCep;
	double minForOneCep;
	double maxForOneFft;
	double minForOneFft;

	double maxForTwoCep;
	double minForTwoCep;
	double maxForTwoFft;
	double minForTwoFft;
	//////////////////////////////////////////////////////LPC平移相关变量
	double maxForOneLpc;
	double minForOneLpc;

	double maxForTwoLpc;
	double minForTwoLpc;
	///////////////////////////////////////////////////////////LPCC平移变量
	double maxForOneLpcc;
	double minForOneLpcc;

	double maxForTwoLpcc;
	double minForTwoLpcc;
  	/////////////////////////////////////////////////////MFCC平移相关变量
	double maxForOneMfcc;
	double minForOneMfcc;

	double maxForTwoMfcc;
	double minForTwoMfcc;
	
	//mel频谱最大、最小值
	double maxForOneMelSpec;		
	double minForOneMelSpec;

	double maxForTwoMelSpec;		
	double minForTwoMelSpec;

	//频谱fft的最大、最小值
	double maxForOneSpecFFT;
	double minForOneSpecFFT;

	double maxForTwoSpecFFT;
	double minForTwoSpecFFT;
	//对数功率谱的最大、最小值
	double maxForOneLogSpec;
	double minForOneLogSpec;

	double maxForTwoLogSpec;
	double minForTwoLogSpec;


	/////////////////////////////////////////////散点图

// 操作    
public:
   void	DrawAxis(CDC* pDC);//显示坐标
   void InitBK();
   void DrawWave(CDC* pDC,const std::vector <short>& data,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,CString WaveName);
   void PlayZoneSound(WAVEFILEHEADER head,const std::vector <short>&data,int start,int end);//播放区域音频
  //绘制特征参数
   void  DrawFeature(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName);//显示特征参数波形
  //绘制频谱 
   void  DrawFeature2(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName,double SpecYZoom);//显示频谱波形
   void  DrawFeature3(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName);//显示倒谱波形

   void  Drawypt(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos);//显示语谱图(对数功率谱)
   void	 DrawSpecYPT(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos);//绘制语谱图(功率谱-频谱)
   void	 DrawSpecYPT_Thickset(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos);//绘制语谱图(功率谱-频谱)
   void  DrawMelcep(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos);//绘制垂直的mel倒谱
   void  Drawypt_Thickset(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos);
    //绘制二次频谱
   void	 DrawSpecFFT(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName,double SpecfftYZoom);//绘制频谱的fft
   void  DrawLogSpecHorizon(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName);//绘制对数功率谱
   void  DrawMelSpectrum(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName,double MelSpecYZoom);//绘制mel频谱
   void  DrawFFT(CDC* pDC,const std::vector<std::vector<float> >& data,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName);//绘制频谱(实际绘制倒谱)
   void  DrawPitch(CDC* pDC,const std::vector<std::vector<float> >& data,const std::vector<int>& f1,const std::vector<float>& p1,const std::vector<int>& f2,const std::vector<float>& p2,const std::vector<int>& f3,const std::vector<float>& p3,const std::vector<int>& tag,const std::vector<float>& divide,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPos,int endPos,int k1,int k2);
   //绘制倒谱端点检测的结果
public:
   void SetXPos(int x)
	{
		m_Xpos=x;
		Invalidate();
	}
	void SetXZoom(double d)
	{
		m_Xzoom=d;
		Invalidate();
	}
	void SetYPos(int x)
	{
		m_Ypos=x;
		Invalidate();
	}
	void SetYZoom(double d)
	{
		m_Yzoom=d;
		Invalidate();
	}
	/************************设置三条曲线放缩比**********************/
	void SetSpecYzoom(double d)
	{
		m_Spec_Yzoom=d;
		Invalidate();
	}

	void SetSpecYzoom2(double d)
	{
		m_Spec_Yzoom2=d;
		Invalidate();
	}

	void SetFFTSpecYzoom(double d)
	{
		m_FFTSpec_Yzoom=d;
		Invalidate();
	}

	void SetFFTSpecYzoom2(double d)
	{
		m_FFTSpec_Yzoom2=d;
		Invalidate();
	}
	void SetMelSpecYzoom(double d)
	{
		m_MelSpec_Yzoom=d;
		Invalidate();
	}

	void SetMelSpecYzoom2(double d)
	{
		m_MelSpec_Yzoom2=d;
		Invalidate();
	}
	/************************************************************/

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMyWaveView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	virtual void OnInitialUpdate();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg void OnFile1Tran();
public:
	afx_msg void OnUpdateFile1Tran(CCmdUI *pCmdUI);
public:
	afx_msg void OnFile2Tran();
public:
	afx_msg void OnUpdateFile2Tran(CCmdUI *pCmdUI);
public:
	afx_msg void OnFile3Tran();
public:
	afx_msg void OnUpdateFile3Tran(CCmdUI *pCmdUI);
public:
	afx_msg void OnStopTran();
public:
	afx_msg void OnReturnPos();
public:
	afx_msg void OnXposTran();
public:
	afx_msg void OnYposTran();
public:
	afx_msg void OnUpdateXposTran(CCmdUI *pCmdUI);
public:
	afx_msg void OnUpdateYposTran(CCmdUI *pCmdUI);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg void OnFile1Play();
public:
	afx_msg void OnFile2Play();
public:
	afx_msg void OnFile3Play();
public:
	afx_msg void OnStopPlay();
public:
	afx_msg void OnCancelZone();
public:
	void testdata(const std::vector <short>&data);
	//void FFT(const std::vector <short>&data, int n, bool isInverse );
public:
	afx_msg void OnSpectrum();//功率谱
public:
	afx_msg void OnLogspectrum();//对数功率谱
public:
	afx_msg void OnCepstrum();//倒谱
///////////////////////////////////////////////////时域分析
public:
	afx_msg void OnPower();//短时能量
public:
	afx_msg void OnZero();//短时过零率
public:
	afx_msg void OnVibration();//平均振幅
public:
	afx_msg void OnRcor();
public:
	afx_msg void OnMfcc();
public:
	afx_msg void OnFft();
public:
	afx_msg void OnPitch();
public:
	afx_msg void OnLpctodata();
public:
	afx_msg void OnDrawypt();
public:
	afx_msg void OnUpdateDrawypt(CCmdUI *pCmdUI);
public:
	afx_msg void OnCep1Tran();
public:
	afx_msg void OnUpdateCep1Tran(CCmdUI *pCmdUI);
public:
	afx_msg void OnFft1Tran();
public:
	afx_msg void OnUpdateFft1Tran(CCmdUI *pCmdUI);
public:
	afx_msg void OnShowpitch();
public:
	afx_msg void OnUpdateShowpitch(CCmdUI *pCmdUI);
public:
	afx_msg void OnMfcc1Tran();
public:
	afx_msg void OnUpdateMfcc1Tran(CCmdUI *pCmdUI);
public:
	afx_msg void OnMfcc2Tran();
public:
	afx_msg void OnUpdateMfcc2Tran(CCmdUI *pCmdUI);
public:
	afx_msg void OnAmdfPitch();
public:
	afx_msg void OnCepPitch();
	afx_msg void OnMutidisplay();
	afx_msg void OnUpdateMutidisplay(CCmdUI *pCmdUI);
	afx_msg void OnSpectrumdisplay();
	afx_msg void OnUpdateSpectrumdisplay(CCmdUI *pCmdUI);
	afx_msg void OnMelcepdisplay();
	afx_msg void OnUpdateMelcepdisplay(CCmdUI *pCmdUI);
	afx_msg void OnTwicefft();
	afx_msg void OnTwicefftdisplay();
	afx_msg void OnUpdateTwicefftdisplay(CCmdUI *pCmdUI);
	afx_msg void OnLogspechorizondisplay();
	afx_msg void OnUpdateLogspechorizondisplay(CCmdUI *pCmdUI);
	afx_msg void OnMelspectrum();
	afx_msg void OnMelspectrumdisplay();
	afx_msg void OnUpdateMelspectrumdisplay(CCmdUI *pCmdUI);
	afx_msg void OnCepdetect();
	afx_msg void OnCepdetectdisplay();
	afx_msg void OnUpdateCepdetectdisplay(CCmdUI *pCmdUI);
	afx_msg void OnCepdisplay();
	afx_msg void OnUpdateCepdisplay(CCmdUI *pCmdUI);
	afx_msg void OnFftdisplay();
	afx_msg void OnUpdateFftdisplay(CCmdUI *pCmdUI);
	afx_msg void OnShowwave();
	afx_msg void OnUpdateShowwave(CCmdUI *pCmdUI);
	afx_msg void OnSpecfft1tran();
	afx_msg void OnUpdateSpecfft1tran(CCmdUI *pCmdUI);
	afx_msg void OnMelspec1tran();
	afx_msg void OnUpdateMelspec1tran(CCmdUI *pCmdUI);
	afx_msg void OnLogspec1tran();
	afx_msg void OnUpdateLogspec1tran(CCmdUI *pCmdUI);
	afx_msg void OnLink();
	afx_msg void OnSaveEffectiveMfcc();
	afx_msg void OnCepstrum2();
	afx_msg void OnShowwave2();
	afx_msg void OnUpdateShowwave2(CCmdUI *pCmdUI);
	afx_msg void OnCepdisplay2();
	afx_msg void OnUpdateCepdisplay2(CCmdUI *pCmdUI);
	afx_msg void OnCep2Tran();
	afx_msg void OnUpdateCep2Tran(CCmdUI *pCmdUI);
	afx_msg void OnTwicefft2();
	afx_msg void OnFftdisplay2();
	afx_msg void OnUpdateTwicefftdisplay2(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFftdisplay2(CCmdUI *pCmdUI);
	afx_msg void OnTwicefftdisplay2();
	afx_msg void OnFft2Tran();
	afx_msg void OnUpdateFft2Tran(CCmdUI *pCmdUI);
	afx_msg void OnSpecfft2tran();
	afx_msg void OnUpdateSpecfft2tran(CCmdUI *pCmdUI);
	afx_msg void OnLogspectrum2();
	afx_msg void OnLogspechorizondisplay2();
	afx_msg void OnUpdateLogspechorizondisplay2(CCmdUI *pCmdUI);
	afx_msg void OnLogspec2tran();
	afx_msg void OnUpdateLogspec2tran(CCmdUI *pCmdUI);
	afx_msg void OnMelspectrum2();
	afx_msg void OnMelspectrumdisplay2();
	afx_msg void OnUpdateMelspectrumdisplay2(CCmdUI *pCmdUI);
	afx_msg void OnMelcepdisplay2();
	afx_msg void OnUpdateMelcepdisplay2(CCmdUI *pCmdUI);
	afx_msg void OnMelspec2tran();
	afx_msg void OnUpdateMelspec2tran(CCmdUI *pCmdUI);
	afx_msg void OnCepdetect2();
	afx_msg void OnCepdetectdisplay1();
	afx_msg void OnUpdateCepdetectdisplay1(CCmdUI *pCmdUI);
	afx_msg void OnCepdetect1tran();
	afx_msg void OnUpdateCepdetect1tran(CCmdUI *pCmdUI);
	afx_msg void OnCepdetect2tran();
	afx_msg void OnUpdateCepdetect2tran(CCmdUI *pCmdUI);
	afx_msg void OnCepdetectdisplay2();
	afx_msg void OnUpdateCepdetectdisplay2(CCmdUI *pCmdUI);
	afx_msg void OnFrmlen256();
	afx_msg void OnFrmlen512();
	afx_msg void OnFrmlen1024();
	afx_msg void OnUpdateFrmlen256(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFrmlen512(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFrmlen1024(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // MyWaveView.cpp 中的调试版本
inline CMyWaveDoc* CMyWaveView::GetDocument() const
   { return reinterpret_cast<CMyWaveDoc*>(m_pDocument); }
#endif

