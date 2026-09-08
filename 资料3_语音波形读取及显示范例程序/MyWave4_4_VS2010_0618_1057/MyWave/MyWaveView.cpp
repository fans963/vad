// MyWaveView.cpp : CMyWaveView 类的实现
//

#include "stdafx.h"
#include "MyWave.h"
#include<vector>
#include "MyWaveDoc.h"
#include "MyWaveView.h"
#include "memory.h"
#include<iomanip>//输出格式新版的不用加.h
#include <math.h>
#include<conio.h>
#include<dos.h>
#include"showFFT.h"
#define DOUBLE_PI    6.28318

#include"WaveConvertor.h"
#include"SpeakerDlg.h"
#include <iostream>
#include <fstream>
using namespace std;


#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <MMSystem.h>
#pragma comment(lib,"winmm.lib")
#include "Speech.h"
#include"GaussianMixture.h"

// CMyWaveView

IMPLEMENT_DYNCREATE(CMyWaveView, CView)

BEGIN_MESSAGE_MAP(CMyWaveView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(IDM_FILE1_TRAN, &CMyWaveView::OnFile1Tran)
	ON_UPDATE_COMMAND_UI(IDM_FILE1_TRAN, &CMyWaveView::OnUpdateFile1Tran)
	ON_COMMAND(IDM_FILE2_TRAN, &CMyWaveView::OnFile2Tran)
	ON_UPDATE_COMMAND_UI(IDM_FILE2_TRAN, &CMyWaveView::OnUpdateFile2Tran)
	ON_COMMAND(IDM_FILE3_TRAN, &CMyWaveView::OnFile3Tran)
	ON_UPDATE_COMMAND_UI(IDM_FILE3_TRAN, &CMyWaveView::OnUpdateFile3Tran)
	ON_COMMAND(IDM_STOP_TRAN, &CMyWaveView::OnStopTran)
	ON_COMMAND(IDM_RETURN_POS, &CMyWaveView::OnReturnPos)
	ON_COMMAND(IDM_XPOS_TRAN, &CMyWaveView::OnXposTran)
	ON_COMMAND(IDM_YPOS_TRAN, &CMyWaveView::OnYposTran)
	ON_UPDATE_COMMAND_UI(IDM_XPOS_TRAN, &CMyWaveView::OnUpdateXposTran)
	ON_UPDATE_COMMAND_UI(IDM_YPOS_TRAN, &CMyWaveView::OnUpdateYposTran)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(IDM_FILE1_PLAY, &CMyWaveView::OnFile1Play)
	ON_COMMAND(IDM_FILE2_PLAY, &CMyWaveView::OnFile2Play)
	ON_COMMAND(IDM_FILE3_PLAY, &CMyWaveView::OnFile3Play)
	ON_COMMAND(IDM_STOP_PLAY, &CMyWaveView::OnStopPlay)
	ON_COMMAND(IDM_CANCEL_ZONE, &CMyWaveView::OnCancelZone)

ON_COMMAND(ID_FFT, &CMyWaveView::OnFft)
ON_COMMAND(ID_SPECTRUM, &CMyWaveView::OnSpectrum)
ON_COMMAND(ID_LOGSPECTRUM, &CMyWaveView::OnLogspectrum)
ON_COMMAND(ID_CEPSTRUM, &CMyWaveView::OnCepstrum)
ON_COMMAND(ID_POWER, &CMyWaveView::OnPower)
ON_COMMAND(ID_ZERO, &CMyWaveView::OnZero)
ON_COMMAND(ID_VIBRATION, &CMyWaveView::OnVibration)
ON_COMMAND(ID_RCOR, &CMyWaveView::OnRcor)
ON_COMMAND(ID_MFCC, &CMyWaveView::OnMfcc)
ON_COMMAND(ID_ACF_Pitch, &CMyWaveView::OnPitch)
ON_COMMAND(ID_LPCTODATA, &CMyWaveView::OnLpctodata)
ON_COMMAND(ID_DRAWYPT, &CMyWaveView::OnDrawypt)
ON_UPDATE_COMMAND_UI(ID_DRAWYPT, &CMyWaveView::OnUpdateDrawypt)
ON_COMMAND(IDM_CEP1_TRAN, &CMyWaveView::OnCep1Tran)
ON_UPDATE_COMMAND_UI(IDM_CEP1_TRAN, &CMyWaveView::OnUpdateCep1Tran)
ON_COMMAND(IDM_FFT1_TRAN, &CMyWaveView::OnFft1Tran)
ON_UPDATE_COMMAND_UI(IDM_FFT1_TRAN, &CMyWaveView::OnUpdateFft1Tran)
ON_COMMAND(ID_SHOWPITCH, &CMyWaveView::OnShowpitch)
ON_UPDATE_COMMAND_UI(ID_SHOWPITCH, &CMyWaveView::OnUpdateShowpitch)
ON_COMMAND(IDM_MFCC1_TRAN, &CMyWaveView::OnMfcc1Tran)
ON_UPDATE_COMMAND_UI(IDM_MFCC1_TRAN, &CMyWaveView::OnUpdateMfcc1Tran)
ON_COMMAND(IDM_MFCC2_TRAN, &CMyWaveView::OnMfcc2Tran)
ON_UPDATE_COMMAND_UI(IDM_MFCC2_TRAN, &CMyWaveView::OnUpdateMfcc2Tran)

ON_COMMAND(ID_AMDF_Pitch, &CMyWaveView::OnAmdfPitch)
ON_COMMAND(ID_CEP_Pitch, &CMyWaveView::OnCepPitch)
ON_COMMAND(ID_MutiDisplay, &CMyWaveView::OnMutidisplay)
ON_UPDATE_COMMAND_UI(ID_MutiDisplay, &CMyWaveView::OnUpdateMutidisplay)
ON_COMMAND(ID_SpectrumDisplay, &CMyWaveView::OnSpectrumdisplay)
ON_UPDATE_COMMAND_UI(ID_SpectrumDisplay, &CMyWaveView::OnUpdateSpectrumdisplay)
ON_COMMAND(ID_MelcepDisplay, &CMyWaveView::OnMelcepdisplay)
ON_UPDATE_COMMAND_UI(ID_MelcepDisplay, &CMyWaveView::OnUpdateMelcepdisplay)
ON_COMMAND(ID_TwiceFFT, &CMyWaveView::OnTwicefft)
ON_COMMAND(ID_TwiceFFTDisplay, &CMyWaveView::OnTwicefftdisplay)
ON_UPDATE_COMMAND_UI(ID_TwiceFFTDisplay, &CMyWaveView::OnUpdateTwicefftdisplay)
ON_COMMAND(ID_LogSpecHorizonDisplay, &CMyWaveView::OnLogspechorizondisplay)
ON_UPDATE_COMMAND_UI(ID_LogSpecHorizonDisplay, &CMyWaveView::OnUpdateLogspechorizondisplay)
ON_COMMAND(ID_MelSpectrum, &CMyWaveView::OnMelspectrum)
ON_COMMAND(ID_MelSpectrumDisplay, &CMyWaveView::OnMelspectrumdisplay)
ON_UPDATE_COMMAND_UI(ID_MelSpectrumDisplay, &CMyWaveView::OnUpdateMelspectrumdisplay)
ON_COMMAND(ID_CepDetect, &CMyWaveView::OnCepdetect)
ON_COMMAND(ID_CepDetectDisplay, &CMyWaveView::OnCepdetectdisplay)
ON_UPDATE_COMMAND_UI(ID_CepDetectDisplay, &CMyWaveView::OnUpdateCepdetectdisplay)
ON_COMMAND(ID_CepDisplay, &CMyWaveView::OnCepdisplay)//文件2端点检测显示
ON_UPDATE_COMMAND_UI(ID_CepDisplay, &CMyWaveView::OnUpdateCepdisplay)
ON_COMMAND(ID_FFTDisplay, &CMyWaveView::OnFftdisplay)
ON_UPDATE_COMMAND_UI(ID_FFTDisplay, &CMyWaveView::OnUpdateFftdisplay)
ON_COMMAND(ID_ShowWave, &CMyWaveView::OnShowwave)
ON_UPDATE_COMMAND_UI(ID_ShowWave, &CMyWaveView::OnUpdateShowwave)
ON_COMMAND(ID_SpecFft1Tran, &CMyWaveView::OnSpecfft1tran)
ON_UPDATE_COMMAND_UI(ID_SpecFft1Tran, &CMyWaveView::OnUpdateSpecfft1tran)
ON_COMMAND(ID_MelSpec1Tran, &CMyWaveView::OnMelspec1tran)
ON_UPDATE_COMMAND_UI(ID_MelSpec1Tran, &CMyWaveView::OnUpdateMelspec1tran)
ON_COMMAND(ID_LogSpec1Tran, &CMyWaveView::OnLogspec1tran)
ON_UPDATE_COMMAND_UI(ID_LogSpec1Tran, &CMyWaveView::OnUpdateLogspec1tran)
ON_COMMAND(ID_Link, &CMyWaveView::OnLink)
ON_COMMAND(ID_SAVE_EFFECTIVE_MFCC, &CMyWaveView::OnSaveEffectiveMfcc)
ON_COMMAND(ID_CEPSTRUM2, &CMyWaveView::OnCepstrum2)
ON_COMMAND(ID_ShowWave2, &CMyWaveView::OnShowwave2)
ON_UPDATE_COMMAND_UI(ID_ShowWave2, &CMyWaveView::OnUpdateShowwave2)
ON_COMMAND(ID_CepDisplay2, &CMyWaveView::OnCepdisplay2)
ON_UPDATE_COMMAND_UI(ID_CepDisplay2, &CMyWaveView::OnUpdateCepdisplay2)
ON_COMMAND(IDM_CEP2_TRAN, &CMyWaveView::OnCep2Tran)
ON_UPDATE_COMMAND_UI(IDM_CEP2_TRAN, &CMyWaveView::OnUpdateCep2Tran)
ON_COMMAND(ID_TwiceFFT2, &CMyWaveView::OnTwicefft2)
ON_COMMAND(ID_FFTDisplay2, &CMyWaveView::OnFftdisplay2)
ON_UPDATE_COMMAND_UI(ID_TwiceFFTDisplay2, &CMyWaveView::OnUpdateTwicefftdisplay2)
ON_UPDATE_COMMAND_UI(ID_FFTDisplay2, &CMyWaveView::OnUpdateFftdisplay2)
ON_COMMAND(ID_TwiceFFTDisplay2, &CMyWaveView::OnTwicefftdisplay2)
ON_COMMAND(IDM_FFT2_TRAN, &CMyWaveView::OnFft2Tran)
ON_UPDATE_COMMAND_UI(IDM_FFT2_TRAN, &CMyWaveView::OnUpdateFft2Tran)
ON_COMMAND(ID_SpecFft2Tran, &CMyWaveView::OnSpecfft2tran)
ON_UPDATE_COMMAND_UI(ID_SpecFft2Tran, &CMyWaveView::OnUpdateSpecfft2tran)
ON_COMMAND(ID_LOGSPECTRUM2, &CMyWaveView::OnLogspectrum2)
ON_COMMAND(ID_LogSpecHorizonDisplay2, &CMyWaveView::OnLogspechorizondisplay2)
ON_UPDATE_COMMAND_UI(ID_LogSpecHorizonDisplay2, &CMyWaveView::OnUpdateLogspechorizondisplay2)
ON_COMMAND(ID_LogSpec2Tran, &CMyWaveView::OnLogspec2tran)
ON_UPDATE_COMMAND_UI(ID_LogSpec2Tran, &CMyWaveView::OnUpdateLogspec2tran)
ON_COMMAND(ID_MelSpectrum2, &CMyWaveView::OnMelspectrum2)
ON_COMMAND(ID_MelSpectrumDisplay2, &CMyWaveView::OnMelspectrumdisplay2)
ON_UPDATE_COMMAND_UI(ID_MelSpectrumDisplay2, &CMyWaveView::OnUpdateMelspectrumdisplay2)
ON_COMMAND(ID_MelcepDisplay2, &CMyWaveView::OnMelcepdisplay2)
ON_UPDATE_COMMAND_UI(ID_MelcepDisplay2, &CMyWaveView::OnUpdateMelcepdisplay2)
ON_COMMAND(ID_MelSpec2Tran, &CMyWaveView::OnMelspec2tran)
ON_UPDATE_COMMAND_UI(ID_MelSpec2Tran, &CMyWaveView::OnUpdateMelspec2tran)
ON_COMMAND(ID_CepDetect2, &CMyWaveView::OnCepdetect2)
ON_COMMAND(ID_CepDetectDisplay1, &CMyWaveView::OnCepdetectdisplay1)
ON_UPDATE_COMMAND_UI(ID_CepDetectDisplay1, &CMyWaveView::OnUpdateCepdetectdisplay1)
ON_COMMAND(ID_CepDetect1Tran, &CMyWaveView::OnCepdetect1tran)
ON_UPDATE_COMMAND_UI(ID_CepDetect1Tran, &CMyWaveView::OnUpdateCepdetect1tran)
ON_COMMAND(ID_CepDetect2Tran, &CMyWaveView::OnCepdetect2tran)
ON_UPDATE_COMMAND_UI(ID_CepDetect2Tran, &CMyWaveView::OnUpdateCepdetect2tran)
ON_COMMAND(ID_CepDetectDisplay2, &CMyWaveView::OnCepdetectdisplay2)
ON_UPDATE_COMMAND_UI(ID_CepDetectDisplay2, &CMyWaveView::OnUpdateCepdetectdisplay2)
ON_COMMAND(ID_Frmlen256, &CMyWaveView::OnFrmlen256)
ON_COMMAND(ID_Frmlen512, &CMyWaveView::OnFrmlen512)
ON_COMMAND(ID_Frmlen1024, &CMyWaveView::OnFrmlen1024)
ON_UPDATE_COMMAND_UI(ID_Frmlen256, &CMyWaveView::OnUpdateFrmlen256)
ON_UPDATE_COMMAND_UI(ID_Frmlen512, &CMyWaveView::OnUpdateFrmlen512)
ON_UPDATE_COMMAND_UI(ID_Frmlen1024, &CMyWaveView::OnUpdateFrmlen1024)
END_MESSAGE_MAP()

// CMyWaveView 构造/析构

CMyWaveView::CMyWaveView()
	: m_left(0)                     //坐标轴坐上与右下的坐标
	, m_right(0)
	, m_top(0)
	, m_bottom(0)
	, m_Xzoom(1.0)                  //X轴的缩放比
	, m_Yzoom(0.01)                 // Y轴的缩放比例例
	, m_Spec_Yzoom(1.0)
	, m_FFTSpec_Yzoom(1.0)
	, m_MelSpec_Yzoom(1.0)
	, m_Spec_Yzoom2(1.0)
	, m_FFTSpec_Yzoom2(1.0)
	, m_MelSpec_Yzoom2(1.0)
	, m_Xpos(0)                     //X轴开始绘制的位置
	, m_Ypos(0)                     //Y轴开始绘制的位置
//文件1相关变量	
	, m_extent(0)                   //3个文件中的最大值
	, m_numOfPoint(0)               //3个文件中的最大值
	, m_bShowFile1(TRUE)            //是否显示文件1波形
	, m_bShowFile2(TRUE)            //是否显示文件2波形
	, m_bShowFile3(TRUE)            //是否显示文件3波形
	,m_bShowCep1(FALSE)              //是否显示文件1的倒谱
	,m_bShowCep2(FALSE)				 //是否显示文件2的倒谱
	, m_bShowFft1(FALSE)//显示频谱
	, m_bShowFft2(FALSE)
	,m_bShowPitch(FALSE)//显示基音周期
	,m_lpctodata(FALSE)//显示Lpc合成的语音数据
	,m_showypt(FALSE)//显示语谱图
	,m_bShowLpc1(FALSE)//显示Lpc1
	,m_bShowLpc2(FALSE)//显示Lpc2
    ,m_bShowLpcc1(FALSE)//显示Lpcc1
	,m_bShowLpcc2(FALSE)//显示Lpcc2
    ,m_bShowMfcc1(FALSE)//显示Mfcc1
	,m_bShowMfcc2(FALSE)//显示Mfcc2
	,m_bMutiDisplay(FALSE)//显示“综合显示语谱图”
	,m_bSpecDisplay(FALSE)//显示“频谱语谱图”
	,m_bMfccDisplay(FALSE)//垂直显示“Mel倒谱”
	,m_bMfccDisplay2(FALSE)
	,m_bSpecFFTDisplay(FALSE)//显示频谱的fft变换
	,m_bSpecFFTDisplay2(FALSE)
	,m_bLogSpecHorizonDisplay(FALSE)//显示水平的对数功率谱
	,m_bLogSpecHorizonDisplay2(FALSE)
	,m_bMelSpecDisplay(FALSE)//显示mel频谱
	,m_bMelSpecDisplay2(FALSE)
	,m_bCepDetectDisplay(FALSE)//显示倒谱端点检测的结果
	,m_bCepDetectDisplay2(FALSE)
////////////区域播放
    , m_startPlayPos(0)             //文件1播放区域的起末点
	, m_endPlayPos(0)
    , m_startPlayPos2(0)             //文件2播放区域的起末点
	, m_endPlayPos2(0)
	, m_startPlayPos3(0)             //文件3播放区域的起末点
	, m_endPlayPos3(0)
//平移有关变量的初始化
    , m_bMouseDown(FALSE)
	, hasTransRect(FALSE)
	, m_transID(NO_OPERATION)
	, m_bOnlyXTran(TRUE)//初始状态下 允许x方向移动
	, m_bOnlyYTran(TRUE)//初始状态下，允许y方向移动
	, m_bStartDrawRect(FALSE)
	, m_bDrawing(FALSE)
	, m_bfrmlen256(FALSE)
	, m_bfrmlen512(TRUE)
	, m_bfrmlen1024(FALSE)
	, m_X_File1_Dispos(0)           //文件1平移时的平移量
	, m_Y_File1_Dispos(0)
	, m_X_File2_Dispos(0)           //文件2平移时的平移量
	, m_Y_File2_Dispos(0)
	, m_X_File3_Dispos(0)           //文件3平移时的平移量
	, m_Y_File3_Dispos(0)
	, m_X_Cep1_Dispos(0)           //文件1倒谱平移时的平移量
	, m_Y_Cep1_Dispos(0)
	, m_X_Cep2_Dispos(0)           //文件2倒谱平移时的平移量
	, m_Y_Cep2_Dispos(0)
	, m_X_IFFT1_Dispos(0)
	, m_Y_IFFT1_Dispos(0)
	, m_X_IFFT2_Dispos(0)
	, m_Y_IFFT2_Dispos(0)
	, m_X_PITCH1_Dispos(0)
	, m_Y_PITCH1_Dispos(0)
	, m_X_PITCH2_Dispos(0)
	, m_Y_PITCH2_Dispos(0)
	, m_X_Fft1_Dispos(0)           //文件1频谱平移时的平移量
	, m_Y_Fft1_Dispos(0)
	, m_X_Fft2_Dispos(0)           //文件2频谱平移时的平移量
	, m_Y_Fft2_Dispos(0)
	, m_X_Lpc1_Dispos(0)           //文件1Lpc平移时的平移量
	, m_Y_Lpc1_Dispos(0)
	, m_X_Lpc2_Dispos(0)           //文件2Lpc平移时的平移量
	, m_Y_Lpc2_Dispos(0)
	, m_X_Lpcc1_Dispos(0)           //文件1Lpcc平移时的平移量
	, m_Y_Lpcc1_Dispos(0)
	, m_X_Lpcc2_Dispos(0)           //文件2Lpcc平移时的平移量
	, m_Y_Lpcc2_Dispos(0)
	, m_X_Mfcc1_Dispos(0)           //文件1Mfcc平移时的平移量
	, m_Y_Mfcc1_Dispos(0)
	, m_X_Mfcc2_Dispos(0)           //文件2Mfcc平移时的平移量
	, m_Y_Mfcc2_Dispos(0)
	, m_X_MelSpec1_Dispos(0)		//文件1 Mel频谱平移时的平移量
	, m_Y_MelSpec1_Dispos(0)
	, m_X_MelSpec2_Dispos(0)		//文件2 Mel频谱平移时的平移量
	, m_Y_MelSpec2_Dispos(0)
	, m_X_LogSpec1_Dispos(0)		//文件1 对数功率谱平移时的平移量
	, m_Y_LogSpec1_Dispos(0)
	, m_X_LogSpec2_Dispos(0)		//文件2 对数功率谱平移时的平移量
	, m_Y_LogSpec2_Dispos(0)
	, m_X_SpecFFT1_Dispos(0)		//文件1 频谱的FFT平移时的平移量
	, m_Y_SpecFFT1_Dispos(0)
	, m_X_SpecFFT2_Dispos(0)		//文件2 频谱的FFT平移时的平移量
	, m_Y_SpecFFT2_Dispos(0)
	, m_old_info(_T(""))
	, m_rect_info(_T(""))           //矩形选择框信息
{
	// TODO: 在此处添加构造代码
    hCursorCross=AfxGetApp()->LoadStandardCursor (IDC_CROSS);
	hCursorTrans=AfxGetApp()->LoadStandardCursor (IDC_SIZEALL);
	hCursorLine=AfxGetApp()->LoadStandardCursor (IDC_SIZEWE);
	m_nFrameSize=WINSIZE;//设置默认帧大小512
	data=NULL;
	dataout=NULL;
	pitch=NULL;
	dataout_cep=NULL;
	dataout_spectrum=NULL;
	dataout_spectrumFFT=NULL;

	data_2=NULL;
	dataout_2=NULL;
	pitch_2=NULL;
	dataout_cep_2=NULL;
	dataout_spectrum_2=NULL;
	dataout_spectrumFFT_2=NULL;
}

CMyWaveView::~CMyWaveView()
{
	if (data != NULL)
		{
			delete[] data;
		}
	if (dataout != NULL)
		{
			delete[] dataout;
		}
	if (pitch != NULL)
		{
			delete[] pitch;
		}
	if (dataout_cep!=NULL)
	{
		delete []dataout_cep;
	}
	if (dataout_spectrum!=NULL)
	{
		delete []dataout_spectrum;
	}
	if (dataout_spectrumFFT!=NULL)
	{
		delete []dataout_spectrumFFT;
	}
}

BOOL CMyWaveView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMyWaveView 绘制

void CMyWaveView::OnDraw(CDC* pDC)
{
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	/***********************动态调整曲线显示***************************/
	memset(m_FlagArray,false,8*sizeof(bool));
	memset(m_YposArray,0,16*sizeof(int));
	m_FlagCount=0;
	if (m_bCepDetectDisplay==TRUE||m_bCepDetectDisplay2==TRUE)//倒谱端点检测
	{
		if ((pDoc->ifft_one.size()>0&&pDoc->tag.size()>0)||(pDoc->ifft_two.size()>0&&pDoc->tag_2.size()>0))
		{
			m_FlagArray[0]=true;
			m_FlagCount++;
		}
		
	}
	if (m_bShowFile1==TRUE||m_bShowFile2==TRUE)//原始波形
	{
		if (pDoc->data_one.size()>0||pDoc->data_one2.size()>0)
		{
			m_FlagArray[1]=true;
			m_FlagCount++;
		}
		
	}
	if (m_bShowFft1==TRUE||m_bShowFft2==TRUE)//频谱
	{
		if (dataout_spectrum!=NULL||dataout_spectrum_2!=NULL)
		{
			m_FlagArray[2]=true;
			m_FlagCount++;
		}
		
	}
	if (m_bSpecFFTDisplay==TRUE||m_bSpecFFTDisplay2==TRUE)//频谱的FFT
	{
		if (dataout_spectrumFFT!=NULL||dataout_spectrumFFT_2!=NULL)
		{
			m_FlagArray[3]=true;
			m_FlagCount++;
		}
		
	}
	if (m_bLogSpecHorizonDisplay==TRUE||m_bLogSpecHorizonDisplay2)//对数功率谱(对数振幅谱)
	{
		if (dataout!=NULL||dataout_2!=NULL)
		{
			m_FlagArray[4]=true;
			m_FlagCount++;
		}
	}
	if (m_bShowCep1==TRUE||m_bShowCep2==TRUE)//倒谱
	{
		if (dataout_cep!=NULL||dataout_cep_2!=NULL)
		{
			m_FlagArray[5]=true;
			m_FlagCount++;
		}
		
	}
	if (m_bMelSpecDisplay==TRUE||m_bMelSpecDisplay2==TRUE)//mel频谱 
	{
		if (pDoc->melspectrum!=NULL||pDoc->melspectrum2!=NULL)
		{
			m_FlagArray[6]=true;
			m_FlagCount++;
		}
	}
	if (m_bShowMfcc1==TRUE||m_bShowMfcc2==TRUE)//mfcc参数
	{
		if (pDoc->mfcc!=NULL||pDoc->mfcc2!=NULL)
		{
			m_FlagArray[7]=true;
			m_FlagCount++;
		}
	}
	int temp0,temp1;
	temp0=0;
	temp1=0;
	if (m_FlagArray[0])//显示端点检测曲线
	{
		m_YposArray[0][0]=10000;
		m_YposArray[0][1]=22000;
		temp0=m_YposArray[0][0];
		temp1=m_YposArray[0][1];
		if (m_FlagArray[1])
		{
			m_YposArray[1][0]=4000;
			temp0=temp1=4000;
		}
		if (m_FlagArray[2])
		{
			m_YposArray[2][0]=temp0-6000;
			m_YposArray[2][1]=m_YposArray[2][0]-3000;
			temp0=m_YposArray[2][0];
			temp1=m_YposArray[2][1];
		}
		if (m_FlagArray[3])
		{
			m_YposArray[3][0]=temp0-8000;
			m_YposArray[3][1]=m_YposArray[3][0]-3000;
			temp0=m_YposArray[3][0];
			temp1=m_YposArray[3][1];
		}
		if (m_FlagArray[4])
		{
			m_YposArray[4][0]=temp0-10000;
			m_YposArray[4][1]=m_YposArray[4][0]-3000;
			temp0=m_YposArray[4][0];
			temp1=m_YposArray[4][1];
		}
		if (m_FlagArray[5])
		{
			m_YposArray[5][0]=temp0-12000;
			m_YposArray[5][1]=m_YposArray[5][0]-3000;
			temp0=m_YposArray[5][0];
			temp1=m_YposArray[5][1];
		}
		if (m_FlagArray[6])
		{
			m_YposArray[6][0]=temp0-10000;
			m_YposArray[6][1]=m_YposArray[6][0]-3000;
			temp0=m_YposArray[6][0];
			temp1=m_YposArray[6][1];
		}
		if (m_FlagArray[7])
		{
			m_YposArray[7][0]=temp0-10000;
			m_YposArray[7][1]=m_YposArray[7][0]-3000;
			temp0=m_YposArray[7][0];
			temp1=m_YposArray[7][1];
		}
		
	} 
	else//不显示端点检测曲线
	{
		if (m_FlagArray[1])
		{
			m_YposArray[1][0]=26000;
			temp0=temp1=26000;
		}
		else
		{
			temp0=temp1=36000;
		}
		if (m_FlagArray[2])
		{
			m_YposArray[2][0]=temp0-6000;
			m_YposArray[2][1]=m_YposArray[2][0]-3000;
			temp0=m_YposArray[2][0];
			temp1=m_YposArray[2][1];
		}
		if (m_FlagArray[3])
		{
			m_YposArray[3][0]=temp0-8000;
			m_YposArray[3][1]=m_YposArray[3][0]-3000;
			temp0=m_YposArray[3][0];
			temp1=m_YposArray[3][1];
		}
		if (m_FlagArray[4])
		{
			m_YposArray[4][0]=temp0-10000;
			m_YposArray[4][1]=m_YposArray[4][0]-3000;
			temp0=m_YposArray[4][0];
			temp1=m_YposArray[4][1];
		}
		if (m_FlagArray[5])
		{
			m_YposArray[5][0]=temp0-12000;
			m_YposArray[5][1]=m_YposArray[5][0]-3000;
			temp0=m_YposArray[5][0];
			temp1=m_YposArray[5][1];
		}
		if (m_FlagArray[6])
		{
			m_YposArray[6][0]=temp0-10000;
			m_YposArray[6][1]=m_YposArray[6][0]-3000;
			temp0=m_YposArray[6][0];
			temp1=m_YposArray[6][1];
		}
		if (m_FlagArray[7])
		{
			m_YposArray[7][0]=temp0-10000;
			m_YposArray[7][1]=m_YposArray[7][0]-3000;
			temp0=m_YposArray[7][0];
			temp1=m_YposArray[7][1];
		}
	}

	/*****************************************************************/

	// TODO: 在此处为本机数据添加绘制代码 
	//显示波形
     if(m_bShowFile1&&pDoc->data_one.size()>0)//m_Xpos ：x轴开始绘制的位置；m_X_File1_Dispos：文件1平移时的平移量
	 {
		 DrawWave(&m_dcCompatible,pDoc->data_one ,m_extent,RGB(255,0,0),m_Xpos,m_X_File1_Dispos,m_Xzoom,m_Ypos,m_Y_File1_Dispos,m_Yzoom,m_startPlayPos,m_endPlayPos,m_YposArray[1][0],_T("语音曲线_1"));
	 }
     if(m_bShowFile2&&pDoc->data_one2.size()>0)
        DrawWave(&m_dcCompatible,pDoc->data_one2 ,m_extent,RGB(255,0,255),m_Xpos,m_X_File2_Dispos,m_Xzoom,m_Ypos,m_Y_File2_Dispos,m_Yzoom,m_startPlayPos2,m_endPlayPos2,m_YposArray[1][0],_T("语音曲线_2"));
     if(m_bShowFile3&&pDoc->data_one3.size()>0)
        DrawWave(&m_dcCompatible,pDoc->data_one3 ,m_extent,RGB(0,0,255),m_Xpos,m_X_File3_Dispos,m_Xzoom,m_Ypos,m_Y_File3_Dispos,m_Yzoom,m_startPlayPos3,m_endPlayPos3,m_YposArray[1][0],_T("语音曲线_3"));
     
      if(m_bShowFft1&&dataout_spectrum!=NULL)//显示频谱(水平显示频谱)
	  {
		  DrawFeature2(&m_dcCompatible,dataout_spectrum,pOutNum,m_extent,RGB(0,128,0),m_Xpos,m_X_Fft1_Dispos,m_Xzoom,m_Ypos,m_Y_Fft1_Dispos,m_Yzoom,0,0,m_YposArray[2][0],m_YposArray[2][1],_T("频谱曲线_1"),m_Spec_Yzoom);
	  }
	  if (m_bShowFft2&&dataout_spectrum_2!=NULL)
	  {
		  DrawFeature2(&m_dcCompatible,dataout_spectrum_2,pOutNum2,m_extent,RGB(0,128,0),m_Xpos,m_X_Fft2_Dispos,m_Xzoom,m_Ypos,m_Y_Fft2_Dispos,m_Yzoom,0,0,m_YposArray[2][0],m_YposArray[2][1],_T("频谱曲线_2"),m_Spec_Yzoom2);
	  }
	  if (m_bSpecFFTDisplay&&dataout_spectrumFFT!=NULL)//显示功率频谱的fft变换
	  {
		 DrawSpecFFT(&m_dcCompatible,dataout_spectrumFFT,pOutNum,m_extent,RGB(0,0,255),m_Xpos,m_X_SpecFFT1_Dispos,m_Xzoom,m_Ypos,m_Y_SpecFFT1_Dispos,m_Yzoom,0,0,m_YposArray[3][0],m_YposArray[3][1],_T("频谱(Hz)的频谱曲线_1"),m_FFTSpec_Yzoom);
	  }
	  if (m_bSpecFFTDisplay2&&dataout_spectrumFFT_2!=NULL)
	  {
		  DrawSpecFFT(&m_dcCompatible,dataout_spectrumFFT_2,pOutNum2,m_extent,RGB(0,0,255),m_Xpos,m_X_SpecFFT2_Dispos,m_Xzoom,m_Ypos,m_Y_SpecFFT2_Dispos,m_Yzoom,0,0,m_YposArray[3][0],m_YposArray[3][1],_T("频谱(Hz)的频谱曲线_2"),m_FFTSpec_Yzoom2);
	  }
	  if (m_bLogSpecHorizonDisplay&&dataout!=NULL)//显示对数功率谱（水平）
	  {
		  DrawLogSpecHorizon(&m_dcCompatible,dataout,pOutNum,m_extent,RGB(0,128,0),m_Xpos,m_X_LogSpec1_Dispos,m_Xzoom,m_Ypos,m_Y_LogSpec1_Dispos,m_Yzoom,0,0,m_YposArray[4][0],m_YposArray[4][1],_T("对数振幅谱(Hz)曲线_1"));
	  }
	  if (m_bLogSpecHorizonDisplay2&&dataout_2!=NULL)
	  {
		  DrawLogSpecHorizon(&m_dcCompatible,dataout_2,pOutNum2,m_extent,RGB(0,128,0),m_Xpos,m_X_LogSpec2_Dispos,m_Xzoom,m_Ypos,m_Y_LogSpec2_Dispos,m_Yzoom,0,0,m_YposArray[4][0],m_YposArray[4][1],_T("对数振幅谱(Hz)曲线_2"));
	  }
	  //if (m_bSpecDisplay)//垂直频谱(垂直显示频谱)
	  //{
		 // //DrawSpecYPT(&m_dcCompatible,dataout_spectrum,pOutNum,m_extent,RGB(255,255,255),m_Xpos,m_X_Fft1_Dispos,m_Xzoom,m_Ypos,m_Y_Fft1_Dispos,m_Yzoom,0,0);//绘制语谱图(功率谱-频谱)
		 // DrawSpecYPT_Thickset(&m_dcCompatible,dataout_spectrum,pOutNum,m_extent,RGB(255,255,255),m_Xpos,m_X_Fft1_Dispos,m_Xzoom,m_Ypos,m_Y_Fft1_Dispos,m_Yzoom,0,0);//绘制语谱图(功率谱-频谱)

	  //}
	  if(m_bShowCep1&&dataout_cep!=NULL)//显示倒谱
	  {
		  DrawFeature3(&m_dcCompatible,dataout_cep,pOutNum,m_extent,RGB(0,128,0),m_Xpos,m_X_Cep1_Dispos,m_Xzoom,m_Ypos,m_Y_Cep1_Dispos,m_Yzoom,0,0,m_YposArray[5][0],m_YposArray[5][1],_T("倒谱(Hz)曲线_1"));
	  }
	  if (m_bShowCep2&&dataout_cep_2!=NULL)//显示倒谱2
	  {
		  DrawFeature3(&m_dcCompatible,dataout_cep_2,pOutNum2,m_extent,RGB(0,0,128),m_Xpos,m_X_Cep2_Dispos,m_Xzoom,m_Ypos,m_Y_Cep2_Dispos,m_Yzoom,0,0,m_YposArray[5][0],m_YposArray[5][1],_T("倒谱(Hz)曲线_2"));
	  }

	  if (m_bCepDetectDisplay&&pDoc->ifft_one.size()>0&&pDoc->tag.size()>0)
	  {
		  DrawFFT(&m_dcCompatible,pDoc->ifft_one,m_extent,RGB(255,0,0),m_Xpos,m_X_IFFT1_Dispos,m_Xzoom,m_Ypos,m_Y_IFFT1_Dispos,m_Yzoom,0,0,m_YposArray[0][0],m_YposArray[0][1],_T("倒谱法端点检测结果_1"));

		  DrawPitch(&m_dcCompatible,pDoc->ifft_one,pDoc->f1,pDoc->p1,pDoc->f2,pDoc->p2,pDoc->f3,pDoc->p3,pDoc->tag,pDoc->divide,m_extent,RGB(0,0,0),m_Xpos,m_X_IFFT1_Dispos,m_Xzoom,m_Ypos,m_Y_IFFT1_Dispos,m_Yzoom,0,0,m_YposArray[0][0],m_YposArray[0][1]);
	  }

	  if (m_bCepDetectDisplay2&&pDoc->ifft_two.size()>0&&pDoc->tag_2.size()>0)
	  {
		  DrawFFT(&m_dcCompatible,pDoc->ifft_two,m_extent,RGB(255,0,0),m_Xpos,m_X_IFFT2_Dispos,m_Xzoom,m_Ypos,m_Y_IFFT2_Dispos,m_Yzoom,0,0,m_YposArray[0][0],m_YposArray[0][1],_T("倒谱法端点检测结果_2"));

		  DrawPitch(&m_dcCompatible,pDoc->ifft_two,pDoc->f1_2,pDoc->p1_2,pDoc->f2_2,pDoc->p2_2,pDoc->f3_2,pDoc->p3_2,pDoc->tag_2,pDoc->divide_2,m_extent,RGB(0,0,0),m_Xpos,m_X_IFFT2_Dispos,m_Xzoom,m_Ypos,m_Y_IFFT2_Dispos,m_Yzoom,0,0,m_YposArray[0][0],m_YposArray[0][1]);
	  }
	  //if(m_showypt)//显示语谱图
	  //{
		 // Drawypt_Thickset(&m_dcCompatible,dataout,pOutNum,m_extent,RGB(255,255,255),m_Xpos,0,m_Xzoom,m_Ypos,0,m_Yzoom,0,0);
		 // //Drawypt(&m_dcCompatible,dataout,pOutNum,m_extent,RGB(0,0,0),m_Xpos,0,m_Xzoom,m_Ypos,0,m_Yzoom,0,0);
	  //}
	  if(m_bShowMfcc1&&pDoc->mfcc!=NULL)//显示mel倒谱参数
      {
		  DrawFeature(&m_dcCompatible,pDoc->mfcc,mfccNum1,m_extent,RGB(128,0,0),m_Xpos,m_X_Mfcc1_Dispos,m_Xzoom,m_Ypos,m_Y_Mfcc1_Dispos,m_Yzoom,0,0,m_YposArray[7][0],m_YposArray[7][1],_T("Mel倒谱_1"));
	  }
	  if (m_bShowMfcc2&&pDoc->mfcc2!=NULL)
	  {
		  DrawFeature(&m_dcCompatible,pDoc->mfcc2,mfccNum2,m_extent,RGB(128,0,0),m_Xpos,m_X_Mfcc2_Dispos,m_Xzoom,m_Ypos,m_Y_Mfcc2_Dispos,m_Yzoom,0,0,m_YposArray[7][0],m_YposArray[7][1],_T("Mel倒谱_2"));
	  }

	  if (m_bMelSpecDisplay&&pDoc->melspectrum!=NULL)//显示mel频谱
	  {
		  DrawMelSpectrum(&m_dcCompatible,pDoc->melspectrum,melspecNum1,m_extent,RGB(0,128,0),m_Xpos,m_X_MelSpec1_Dispos,m_Xzoom,m_Ypos,m_Y_MelSpec1_Dispos,m_Yzoom,0,0,m_YposArray[6][0],m_YposArray[6][1],_T("mel频谱_1"),m_MelSpec_Yzoom);
	  }
	  if (m_bMelSpecDisplay2&&pDoc->melspectrum2!=NULL)
	  {
		  DrawMelSpectrum(&m_dcCompatible,pDoc->melspectrum2,melspecNum2,m_extent,RGB(0,128,0),m_Xpos,m_X_MelSpec2_Dispos,m_Xzoom,m_Ypos,m_Y_MelSpec2_Dispos,m_Yzoom,0,0,m_YposArray[6][0],m_YposArray[6][1],_T("mel频谱_2"),m_MelSpec_Yzoom2);
	  }
	  /*if(m_bShowMfcc2)
      {
		  DrawFeature(&m_dcCompatible,pDoc->mfcc2,mfccNum2,m_extent,RGB(255,128,0),m_Xpos,m_X_Mfcc2_Dispos,m_Xzoom,m_Ypos,m_Y_Mfcc2_Dispos,m_Yzoom,0,0);
	  }*/
	  //if (m_bMfccDisplay)//垂直显示mel倒谱
	  //{
		 // DrawMelcep(&m_dcCompatible,pDoc->mfcc,mfccNum1,m_extent,RGB(0,128,0),m_Xpos,m_X_Mfcc1_Dispos,m_Xzoom,m_Ypos,m_Y_Mfcc1_Dispos,m_Yzoom,0,0);
	  //}
	  //if (m_bMutiDisplay)//综合显示语谱图
	  //{
		 // DrawSpecYPT(&m_dcCompatible,dataout_spectrum,pOutNum,m_extent,RGB(0,0,0),m_Xpos,m_X_Fft1_Dispos,m_Xzoom,m_Ypos,m_Y_Fft1_Dispos,m_Yzoom,0,0);//绘制语谱图(功率谱-频谱)
		 // Drawypt(&m_dcCompatible,dataout,pOutNum,m_extent,RGB(0,0,0),m_Xpos,0,m_Xzoom,m_Ypos,0,m_Yzoom,0,0);
		 // DrawFeature2(&m_dcCompatible,dataout_cep,pOutNum,m_extent,RGB(0,128,0),m_Xpos,m_X_Cep1_Dispos,m_Xzoom,m_Ypos,m_Y_Cep1_Dispos,m_Yzoom,0,0);
		 // DrawMelcep(&m_dcCompatible,pDoc->mfcc,mfccNum1,m_extent,RGB(0,128,0),m_Xpos,m_X_Mfcc1_Dispos,m_Xzoom,m_Ypos,m_Y_Mfcc1_Dispos,m_Yzoom,0,0);
		 // //DrawWave(&m_dcCompatible,pDoc->data_one ,m_extent,RGB(255,0,0),m_Xpos,m_X_File1_Dispos,m_Xzoom,m_Ypos,m_Y_File1_Dispos,m_Yzoom,m_startPlayPos,m_endPlayPos);

	  //}



	 if(m_transID!=NO_OPERATION)//波形移动
	  {
		  SetTransRect();
		  DrawTransRect(&m_dcCompatible);
	  }
	   
	//BitBlt函数对指定的源设备环境区域中的像素进行位块（bit_block）转换，以传送到目标设备环境。
	pDC->BitBlt (0,0,rect.Width (),rect.Height() ,&m_dcCompatible,0,0,SRCCOPY);//SRCCOPY：将源矩形区域直接拷贝到目标矩形区域。 
	if(m_dcCompatible.m_hDC )
		m_dcCompatible.DeleteDC ();
	if(m_bitmap.m_hObject )
		m_bitmap.DeleteObject ();
	
}


// CMyWaveView 打印

BOOL CMyWaveView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMyWaveView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMyWaveView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清除过程
}


// CMyWaveView 诊断

#ifdef _DEBUG
void CMyWaveView::AssertValid() const
{
	CView::AssertValid();
}

void CMyWaveView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMyWaveDoc* CMyWaveView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyWaveDoc)));
	return (CMyWaveDoc*)m_pDocument;
}
#endif //_DEBUG


// CMyWaveView 消息处理程序

void CMyWaveView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if(m_dcCompatible.m_hDC )
		m_dcCompatible.DeleteDC ();
	if(m_bitmap.m_hObject )
		m_bitmap.DeleteObject ();
}

void CMyWaveView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
    GetClientRect(&rect);
	m_left=rect.left+12;
	m_top=rect.top +10;
	m_right=rect.right -12;
	m_bottom=rect.bottom -25;

}

void CMyWaveView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CMyWaveDoc* pDoc=(CMyWaveDoc*)GetDocument();
	pDoc->m_hWndView =m_hWnd;//取得视图句柄
}
/////////////////////////////////画坐标轴
void CMyWaveView::DrawAxis(CDC* pDC)
{
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDC->Rectangle(&rect);
	// 创建画笔对象
	CPen* pPenLine = new CPen;
	// 画线画笔
	pPenLine->CreatePen(PS_SOLID,1,RGB(100,100,100));
	// 创建画笔对象
	CPen* pPenDot = new CPen;
	// 网格画笔
	pPenDot->CreatePen(PS_DOT,1,RGB(100,100,100));
	// 选中当前画线画笔，并保存以前的画笔
	CGdiObject* pOldPen = pDC->SelectObject(pPenLine);
	
	// 绘制坐标轴
	pDC->MoveTo(m_left,m_top);
	// 垂直轴
	pDC->LineTo(m_left,m_bottom);
	pDC->LineTo(m_right,m_bottom);	

	// 绘制Y轴箭头
	pDC->MoveTo(m_right-5,m_bottom-5);
	pDC->LineTo(m_right,m_bottom);
	pDC->LineTo(m_right-5,m_bottom+5);
	
	// 绘制X轴箭头	
	pDC->MoveTo(m_left-5,m_top+5);
	pDC->LineTo(m_left,m_top);
	pDC->LineTo(m_left+5,m_top+5);
	
	pDC->MoveTo(m_left,m_top);
	pDC->LineTo(m_right,m_top);
	pDC->MoveTo(m_right,m_top);
	pDC->LineTo(m_right,m_bottom);
	
	CString str;
	//绘制X轴刻度  选择绿色画笔
    pDC->SelectObject(pPenDot);

	int xStep;
	int right=int((m_right-m_left)/m_Xzoom+m_Xpos);
	if(m_Xzoom>=1)
	{
		if(m_Xzoom>=1 && m_Xzoom<1.5)
			xStep=100;
		else if(m_Xzoom>=1.5 && m_Xzoom<2.5)
			xStep=50;
		else if(m_Xzoom>=2.5 && m_Xzoom<3.5 )
			xStep=20;
		else if(m_Xzoom>=3.5 && m_Xzoom<=5.0)
			xStep=10;
	}
	else 
	{
		if(m_Xzoom<1 && m_Xzoom>=0.75)
			xStep=100;
		else if(m_Xzoom<0.75 && m_Xzoom>=0.35)
			xStep=200;
		else if(m_Xzoom<0.35 && m_Xzoom>=0.14)
			xStep=500;
		else if(m_Xzoom<0.14 && m_Xzoom>=0.08)
			xStep=1000;
		else if(m_Xzoom<0.08 && m_Xzoom>=0.03)
			xStep=2000;
		else if(m_Xzoom<0.03 && m_Xzoom>=0.01)
			xStep=5000;
	}
	pDC->SetTextColor(RGB(0,0,0));
	int sum=m_numOfPoint;
	for (int i = 0; i <=sum ; i += xStep)
	{
		if(i>m_Xpos && i<=right)
		{
			str.Format(_T("%d"),i);
			int xPos=int(i*m_Xzoom-m_Xpos*m_Xzoom);		//k点与起始点之间的距离							
			pDC->MoveTo(xPos+m_left, m_bottom);
			pDC->LineTo(xPos+m_left, m_top);
			
			pDC->TextOut(xPos+m_left-10,m_bottom+6,str);
		}
	}

	// 绘制Y轴网格	
    int step;
	int top=int((m_bottom-m_top)/m_Yzoom+m_Ypos);//m_Ypos为当前画面(纵坐标的最小值)
	if(m_Yzoom<=0.05)
		step=2000;
	else if(m_Yzoom<=0.1)
		step=1000;
	else if (m_Yzoom<=0.2)
		step=500;
	else if(m_Yzoom<=0.5)
		step=200;
	else if (m_Yzoom<=0.75)
		step=100;
	else if(m_Yzoom<=1)
		step=50;

	for (int i = 800 ;i<=65600  ;i+=step)
	{
		if((i>m_Ypos)&&(i <= top))
		{
			int y=(int)(m_bottom-(i-m_Ypos)*m_Yzoom);//m_Ypos初值为0,当前屏幕纵坐标的最小值
			pDC->MoveTo(m_left,y);
			pDC->LineTo(m_right,y);
			str.Format(_T("%d"),i-m_extent);//m_extent=32800,输出(i-32800)
			//str.Format(_T("%d"),i-m_extent+26000);
		    //pDC->TextOut(m_left-5,y-8,str);
		}
	}

	/***********************绘制帧间隔*************************/

	/********************************************************/
	delete pPenLine;
	delete pPenDot;
    pDC->SelectObject(pOldPen);	
}

BOOL CMyWaveView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    //InitBK()只能放在此处 因为OnEraseBkgnd是先于OnDraw执行的 不放在此处的话DrawAxis中的值则未定义
	InitBK();						
	DrawAxis(&m_dcCompatible);

	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}
void CMyWaveView::InitBK()
{
	CDC* dc=GetDC();

	m_dcCompatible.CreateCompatibleDC(dc);
	m_bitmap.CreateCompatibleBitmap(dc,rect.Width(),rect.Height());
	m_dcCompatible.SelectObject(&m_bitmap);
	m_dcCompatible.BitBlt(0,0,rect.Width(),rect.Height(),dc,0,0,SRCCOPY);
	ReleaseDC(dc);
}

void CMyWaveView::DrawWave(CDC* pDC,const std::vector <short>& data,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,CString WaveName)
{
	//绘制曲线基准刻度
	if (data.size()==0)
	{
		return;
	}
	int y=(int)(m_bottom-(32800-k1-m_Ypos-YDispos)*m_Yzoom);
	pDC->MoveTo (m_left,y);
	pDC->LineTo (m_left+60,y);

	pDC->SetTextColor(RGB(0,0,0));
	pDC->TextOut(m_left+5,y-18,WaveName);


	CPen pen;
	pen.CreatePen (PS_SOLID,1,crColor);
	CPen* pOldPen=pDC->SelectObject (&pen);

	CPen line1,line2;
	line1.CreatePen(PS_SOLID,1,RGB(200,0,230));
	line2.CreatePen(PS_SOLID,1,RGB(0,220,200));

	int i=(int)(Xpos-(double)XDispos);//i为开始绘制时的压力点的坐标！！
	int j=0;//j表示当前帧号
	if(i<0)
		i=0;

	int sum=data.size ();
	int xPos=(int)i*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom+m_left;
	int xPos1=(int)(i+1)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom+m_left;
	//绘制波形
	// /*******************************************/
	int k=k1;

	/*******************************************/
	for(;xPos<=m_right && xPos1<=m_right && i+1<sum;)
	{
		if(data[i]-k+extent-(int)(YDispos)>=Ypos&&data[i]-k+extent-(int)YDispos<=Ypos+(int)(m_bottom-m_top)/Yzoom)
		{
			CPoint start(xPos,m_bottom-(int)((data[i]-k+extent-Ypos)*Yzoom)+YDispos*Yzoom);
			if(data[i+1]-k+extent-(int)(YDispos)>=Ypos&&data[i+1]-k+extent-(int)YDispos<=Ypos+(int)(m_bottom-m_top)/Yzoom)
			{
				CPoint end(xPos1,m_bottom-(int)((data[i+1]-k+extent-Ypos)*Yzoom)+YDispos*Yzoom);
				if(xPos>=m_left&&xPos1>=m_left)
				{
					if(i%(this->m_nFrameSize/2)==0)
					{
						j=i/(this->m_nFrameSize/2);
						if(j%2==0)//画分割线
						{
							pDC->SelectObject(line1);							
						}
						else
						{
							pDC->SelectObject(line2);
						}						
						pDC->MoveTo(xPos,m_bottom);
						pDC->LineTo(xPos,m_top);

						//画帧序号
						CString num;
						num.Format(_T("No:%d "),j+1);
						pDC->SetTextColor(RGB(0,0,0));
						pDC->TextOut(xPos,int(m_bottom-(data[i]-k+extent-Ypos)*Yzoom+(YDispos)*Yzoom),num);

					}
					pDC->SelectObject(pen);
					pDC->MoveTo (start);
					pDC->LineTo (end);
				}
			}
		}
		i++;
		xPos=(int)i*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom+m_left;
		xPos1=(int)(i+1)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom+m_left;
	}
	//绘制区域选择线
	if(startPlayPos!=endPlayPos)
	{
		CPen *old;
		CPen pen(PS_SOLID,2,RGB(0,0,0));
		old=pDC->SelectObject (&pen);

		int start=(int)(startPlayPos*Xzoom-Xpos*Xzoom)+(int)XDispos*Xzoom+m_left;
		int end=(int)(endPlayPos*Xzoom-Xpos*Xzoom)+(int)XDispos*Xzoom+m_left;

		pDC->MoveTo (start,m_top);
		pDC->LineTo (start,m_bottom);
		pDC->MoveTo (end,m_top);
		pDC->LineTo (end,m_bottom);
		pDC->SelectObject (old);

		pDC->TextOut (m_right-300,m_bottom-20,m_rect_info);
	}

	pDC->SelectObject (pOldPen);
	if(pen.m_hObject )
		pen.DeleteObject ();

}

////////////////////////////////////////////////////////////////////////显示特征参数波形，显示LPCC，MFCC等
void CMyWaveView::DrawFeature(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName)
  
{
	   int y=(int)(m_bottom-(32800-k2-m_Ypos-YDispos)*m_Yzoom);
	   pDC->MoveTo (m_left,y);
	   pDC->LineTo (m_left+60,y);
	  
	   pDC->SetTextColor(RGB(0,0,0));
	   pDC->TextOut(m_left+5,y-18,WaveName);

	   y=(int)(m_bottom-(32800-k1-m_Ypos-YDispos)*m_Yzoom);
	   pDC->MoveTo (m_left,y);
	   pDC->LineTo (m_left+60,y);
	   //pDC->TextOut(m_left,y+5,_T("22000"));

	   int k=k2;
	   /* double magnify;
	   magnify=4000/(maxForOneMfcc-minForOneMfcc);*/

	   CPen pen,line1,line2;
	   pen.CreatePen(PS_SOLID,1,crColor);
	   CPen* pOldPen=pDC->SelectObject(&pen);
	   CPen pen1,pen2;
	   pen1.CreatePen(PS_SOLID,1,RGB(123,25,234));
        
	  
	   int start=(int)(Xpos-(double)XDispos);//i为开始绘制时的压力点的坐标！！
	   if( start<0)
		    start=0;
       //int m=1;//用于标记画帧框的次序
	   unsigned int sum=sample_rate;//绘制点个数 
	   int i=(start/this->m_nFrameSize/2),j=(start/(this->m_nFrameSize/12)+1);
	   int xPos=0;
	   int xPos1=0;
     //绘制波形
    
	for(;i<(int)sample_rate/13;i++)
	{
		xPos=((int)j*(this->m_nFrameSize/12)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+(i*this->m_nFrameSize/2)*Xzoom+m_left;
	    xPos1=((int)(j+1)*(this->m_nFrameSize/12)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+(i*this->m_nFrameSize/2)*Xzoom+m_left;
	   if(i>0)
	   {	
		   if(i%2==1)		
		   {
			   k=k1;
		   }
			if(i%2==0)		
		   {
			   k=k2;
		   }
			if(i%8==0||i%8==3||i%8==4||i%8==7)			
				pDC->SelectObject(&pen);
			if(i%8==1||i%8==2||i%8==5||i%8==6)
				pDC->SelectObject(&pen1);
       }
		for(;j+1<13&&xPos<=m_right && xPos1<=m_right;)
		{	

		   if(data[i*13+j]+extent-(int)(YDispos)-k>=Ypos&&data[i*13+j]+extent-(int)YDispos-k<=Ypos+(int)(m_bottom-m_top)/Yzoom)
		   {  

			   CPoint start(xPos,m_bottom-(int)((data[i*13+j]+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
			   if(data[i*13+j+1]+extent-(int)(YDispos)-k>=Ypos&&data[i*13+j+1]+extent-(int)YDispos-k<=Ypos+(int)(m_bottom-m_top)/Yzoom)
			   {
				   CPoint end(xPos1,m_bottom-(int)((data[i*13+j+1]+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
				   if(xPos>=m_left&&xPos1>=m_left)
				   {

					   pDC->MoveTo (start);
					   pDC->LineTo (end);

				   }
			   }
		   }
		   j++;
		   xPos=((int)j*Xzoom*(this->m_nFrameSize/12)-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+(i*this->m_nFrameSize/2)*Xzoom+m_left;
		   xPos1=((int)(j+1)*Xzoom*(this->m_nFrameSize/12)-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+(i*this->m_nFrameSize/2)*Xzoom+m_left;	
	   }
	  j=0;
	}
 
	   pDC->SelectObject (pOldPen);
	   if(pen.m_hObject )
		   pen.DeleteObject ();
}
//////////////////////////////垂直显示Mel倒谱
void  CMyWaveView::DrawMelcep(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos)
{
	const int y_base=57200;								//曲线绘制起点纵坐标。
	if (((m_bottom-m_top)/m_Yzoom+m_Ypos-m_extent>y_base))//屏幕顶端坐标必须大于曲线绘制起点
	{	
		int x_start,x_end;								//x_start表示屏幕内绘制的第一个帧的下标
		int ncount=sample_rate/13;
		int y_step=ceil(900*m_Yzoom);					//相邻数据点在垂直方向上的实际间隔
		int x_base;										//“第一帧”中心x坐标

		/******************************确定振幅绘制放大比例***************************/
		int magnify;	//放大比例
		magnify=(int)(1.5*m_nFrameSize/(maxForOneFft-minForOneFft));
		/*****************************************************************************/


		/*******************************确定绘制范围**********************************/
		//前边界
		int times=m_Xpos/(this->m_nFrameSize/2);
		if (m_Xpos%(this->m_nFrameSize/2)==0&&m_Xpos!=0)
		{
			x_base=m_Xpos;
			x_start=times-1;
		}
		else
		{
			x_base=(times+1)*(this->m_nFrameSize/2);
			x_start=times;
		}
		//后边界
		times=(m_Xpos+(int)((m_right-m_left)/m_Xzoom))/(this->m_nFrameSize/2);
		if((int)((m_Xpos+(int)((m_right-m_left)/m_Xzoom)))%(this->m_nFrameSize/2)==0)
		{
			x_end=times-1;
		}
		else
		{
			x_end=times;
		}
		//防止越界
		if (x_end>ncount-1)
		{
			x_end=ncount-1;
		}
		/******************************************************************************/

		/******************开始绘制曲线(频率的幅度正方向暂时取x轴的反方向)************/

		//先绘第一帧的mel倒谱，减少不必要的判断
		CPoint base;//存放x_base,y_base基准屏幕坐标
		CPoint pre,next;
		base.x=(int)(m_Xzoom*x_base)-(int)(m_Xzoom*m_Xpos)+(int)(m_Xzoom*XDispos)+m_left;
		base.y=m_bottom-(int)((y_base-m_Ypos+m_extent-YDispos)*m_Yzoom);
		pre.x=base.x;
		pre.y=base.y;
		pDC->MoveTo(pre.x,pre.y);
	
		for(int k=0;k<13;k++)
		{
			next.x=base.x+(int)(m_Xzoom*magnify*data[x_start*13+k]);
			next.y=pre.y-y_step;
			if (next.x<m_left)
			{
				next.x=m_left;
			}
			if (next.x>m_right)
			{
				next.x=m_right;
			}
			pDC->LineTo(next);
			pre.x=next.x;
			pre.y=next.y;
			pDC->MoveTo(pre);
		}

		//绘制其他帧的mel倒谱
		for (int i=x_start+1;i<=x_end;i++)
		{
			base.x=(int)((x_base+(i-x_start)*(m_nFrameSize/2)-m_Xpos+XDispos)*m_Xzoom)+m_left;//第i帧中心处x坐标
			pre.x=base.x;
			pre.y=base.y;
			if (next.x<m_left)
			{
				next.x=m_left;
			}
			if (next.x>m_right)
			{
				next.x=m_right;
			}
			pDC->MoveTo(pre);
			for (int j=0;j<13;j++)
			{
				next.x=base.x+(int)(m_Xzoom*magnify*data[i*13+j]);
				next.y=pre.y-y_step;
				pDC->LineTo(next);
				pre.x=next.x;
				pre.y=next.y;
				pDC->MoveTo(pre);
			}
		}
	} 
}

///////////////////////////////////////////////////////////////////////显示mel频谱
void  CMyWaveView::DrawMelSpectrum(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName,double MelSpecYZoom)
{
	//绘制曲线基准刻度
	int y=(int)(m_bottom-(32800-k1-m_Ypos-YDispos)*m_Yzoom);
	pDC->MoveTo (m_left,y);
	pDC->LineTo (m_left+60,y);
	
	y=(int)(m_bottom-(32800-k2-m_Ypos-YDispos)*m_Yzoom);
	pDC->MoveTo (m_left,y);
	pDC->LineTo (m_left+60,y);

	pDC->SetTextColor(RGB(0,0,0));
	pDC->TextOut(m_left+5,y-18,WaveName);

	int k=k2;

	CPen pen,line1,line2;
	pen.CreatePen(PS_SOLID,1,crColor);
	CPen* pOldPen=pDC->SelectObject(&pen);
	CPen pen1,pen2;
	pen1.CreatePen(PS_SOLID,1,RGB(200,25,100));


	int start=(int)(Xpos-(double)XDispos);//i为开始绘制时的压力点的坐标！！
	if( start<0)
		start=0;
	//int m=1;//用于标记画帧框的次序
	unsigned int sum=sample_rate;//绘制点个数 
	int i=(start/this->m_nFrameSize/2),j=(start/(this->m_nFrameSize/23)+1);
	int xPos=0;
	int xPos1=0;
	//绘制波形

	for(;i<(int)sample_rate/24;i++)
	{
		xPos=((int)j*(this->m_nFrameSize/23)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+(i*this->m_nFrameSize/2)*Xzoom+m_left;
		xPos1=((int)(j+1)*(this->m_nFrameSize/23)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+(i*this->m_nFrameSize/2)*Xzoom+m_left;
		if(i>0)
		{	
			if(i%2==1)		
			{
				k=k1;
			}
			if(i%2==0)		
			{
				k=k2;
			}
		}
		if(i%8==0||i%8==3||i%8==4||i%8==7)			
			pDC->SelectObject(&pen);
		if(i%8==1||i%8==2||i%8==5||i%8==6)
			pDC->SelectObject(&pen1);

		for(;j+1<24&&xPos<=m_right && xPos1<=m_right;)
		{	

			if(data[i*24+j]*MelSpecYZoom+extent-(int)(YDispos)-k>=Ypos&&data[i*24+j]*MelSpecYZoom+extent-(int)YDispos-k<=Ypos+(int)(m_bottom-m_top)/Yzoom)
			{  
				CPoint start(xPos,m_bottom-(int)((data[i*24+j]*MelSpecYZoom+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
				if(data[i*24+j+1]*MelSpecYZoom+extent-(int)(YDispos)-k>=Ypos&&data[i*24+j+1]*MelSpecYZoom+extent-(int)YDispos-k<=Ypos+(int)(m_bottom-m_top)/Yzoom)
				{
					CPoint end(xPos1,m_bottom-(int)((data[i*24+j+1]*MelSpecYZoom+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
					
					if(xPos>=m_left&&xPos1>=m_left)
					{
						pDC->MoveTo (start);
						pDC->LineTo (end);
					}
				}
			}
			j++;
			xPos=((int)j*Xzoom*(this->m_nFrameSize/23)-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+(i*this->m_nFrameSize/2)*Xzoom+m_left;
			xPos1=((int)(j+1)*Xzoom*(this->m_nFrameSize/23)-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+(i*this->m_nFrameSize/2)*Xzoom+m_left;	
		}
		j=0;
	}

	pDC->SelectObject (pOldPen);
	if(pen.m_hObject )
		pen.DeleteObject ();
}


//////////////////////////////显示频谱，添加绘制起点参数
void CMyWaveView::DrawFeature2(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName,double SpecYZoom)
{      
	//绘制曲线基准刻度
	int y=(int)(m_bottom-(32800-k2-m_Ypos-YDispos)*m_Yzoom);
	pDC->MoveTo (m_left,y);
	pDC->LineTo (m_left+60,y);

	pDC->SetTextColor(RGB(0,0,0));
	pDC->TextOut(m_left+5,y-18,WaveName);

	y=(int)(m_bottom-(32800-k1-m_Ypos-YDispos)*m_Yzoom);
	pDC->MoveTo (m_left,y);
	pDC->LineTo (m_left+60,y);

    int k=k2;

    CPen pen,line1,line2;
	pen.CreatePen(PS_SOLID,1,crColor);
	CPen* pOldPen=pDC->SelectObject(&pen);
    CPen pen1,pen2;
	pen1.CreatePen(PS_SOLID,1,RGB(255,0,0));//pen1红色

	int start=(int)(Xpos-(double)XDispos);//i为开始绘制时的压力点的坐标！！
    if( start<0)
		start=0;
    int i=(start/this->m_nFrameSize/2),j=(start%(this->m_nFrameSize));			
    int xPos=0;
	int xPos1=0;
	for(;i<(int)sample_rate/(this->m_nFrameSize)&&xPos<=m_right && xPos1<=m_right;i++)
	{
		xPos=((int)j*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;
		xPos1=((int)(j+1)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;

		if(i%2==1)		
		{
			k=k1;
			pDC->SelectObject(&pen1);
		}
		if(i%2==0)		
		{
			k=k2;
			pDC->SelectObject(&pen);
		}


		for(;j+1<((this->m_nFrameSize)/2+1)&&xPos<=m_right && xPos1<=m_right;)
		{	

		   if(data[i*(this->m_nFrameSize)+j]*SpecYZoom+extent-(int)(YDispos)-k>=Ypos&&data[i*(this->m_nFrameSize)+j]*SpecYZoom+extent-(int)YDispos-k<=Ypos+(int)(m_bottom-m_top)/Yzoom)
		   {  
			   CPoint start(xPos,m_bottom-(int)((data[i*(this->m_nFrameSize)+j]*SpecYZoom+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
			   if(data[i*(this->m_nFrameSize)+j+1]*SpecYZoom+extent-(int)(YDispos)-k>=Ypos&&data[i*(this->m_nFrameSize)+j+1]*SpecYZoom+extent-(int)YDispos-k<=Ypos+(int)(m_bottom-m_top)/Yzoom)
			   {
				   CPoint end(xPos1,m_bottom-(int)((data[i*(this->m_nFrameSize)+j+1]*SpecYZoom+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
				   if(xPos>=m_left&&xPos1>=m_left)
				   {
					   pDC->MoveTo (start);
					   pDC->LineTo (end);
				   }
			   }
		   }
		   j++;
		   xPos=((int)j*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;
		   xPos1=((int)(j+1)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;	
	   }
	  j=0;
	}
 
	   pDC->SelectObject (pOldPen);
	   if(pen.m_hObject )
		   pen.DeleteObject ();
}

/////////////////////////////显示倒谱
void CMyWaveView::DrawFeature3(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName)
{   
	//绘制曲线基准刻度
	int y=(int)(m_bottom-(32800-k2-m_Ypos-YDispos)*m_Yzoom);
	pDC->MoveTo (m_left,y);
	pDC->LineTo (m_left+60,y);

	pDC->SetTextColor(RGB(0,0,0));
	pDC->TextOut(m_left+5,y-18,WaveName);

	y=(int)(m_bottom-(32800-k1-m_Ypos-YDispos)*m_Yzoom);
	pDC->MoveTo (m_left,y);
	pDC->LineTo (m_left+60,y);
	int k=k2;

	CPen pen,line1,line2;
	pen.CreatePen(PS_SOLID,1,crColor);
	CPen* pOldPen=pDC->SelectObject(&pen);
	CPen pen1,pen2;
	pen1.CreatePen(PS_SOLID,1,RGB(255,0,0));

	int start=(int)(Xpos-(double)XDispos);//i为开始绘制时的压力点的坐标！！
	if( start<0)
		start=0;
	int i=(start/this->m_nFrameSize/2),j=(start%(this->m_nFrameSize));			
	int xPos=0;
	int xPos1=0;
	for(;i<(int)sample_rate/(this->m_nFrameSize)&&xPos<=m_right && xPos1<=m_right;i++)
	{
		xPos=((int)j*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;
		xPos1=((int)(j+1)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;
		if(i>0)
		{	
			if(i%2==1)		
			{
				k=k1;	   
			}
			if(i%2==0)		
			{
				k=k2;
			}
			if(i%8==0||i%8==3||i%8==4||i%8==7)			
				pDC->SelectObject(&pen);
			if(i%8==1||i%8==1||i%8==5||i%8==6)
				pDC->SelectObject(&pen1);
		}


		for(;j+1<((this->m_nFrameSize)/2+1)&&xPos<=m_right && xPos1<=m_right;)
		{	

			if(data[i*(this->m_nFrameSize)+j]+extent-(int)(YDispos)-k>=Ypos&&data[i*(this->m_nFrameSize)+j]+extent-(int)YDispos-k<=Ypos+(int)(m_bottom-m_top)/Yzoom)
			{  

				CPoint start(xPos,m_bottom-(int)((data[i*(this->m_nFrameSize)+j]+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
				if(data[i*(this->m_nFrameSize)+j+1]+extent-(int)(YDispos)-k>=Ypos&&data[i*(this->m_nFrameSize)+j+1]+extent-(int)YDispos-k<=Ypos+(int)(m_bottom-m_top)/Yzoom)
				{
					CPoint end(xPos1,m_bottom-(int)((data[i*(this->m_nFrameSize)+j+1]+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
					if(xPos>=m_left&&xPos1>=m_left)
					{

						pDC->MoveTo (start);
						pDC->LineTo (end);

					}
				}
			}
			j++;
			xPos=((int)j*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;
			xPos1=((int)(j+1)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;	
		}
		j=0;
	}

	pDC->SelectObject (pOldPen);
	if(pen.m_hObject )
		pen.DeleteObject ();
}

///////////////////////////////////////////////////////////////////////////////////////显示基音周期
  void CMyWaveView::OnShowpitch()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowPitch=!m_bShowPitch;
	 Invalidate();
}
  void CMyWaveView::OnUpdateShowpitch(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bShowPitch);
}


/*////////////////////////与平移有关的操作函数///////////////////////////*/
   void CMyWaveView::OnFile1Tran()
   {
	   // TODO: 在此添加命令处理程序代码
        m_transID=_FILE1;
		Invalidate();

   }

   void CMyWaveView::OnUpdateFile1Tran(CCmdUI *pCmdUI)
   {
	   // TODO: 在此添加命令更新用户界面处理程序代码
	   pCmdUI->SetCheck (m_transID==_FILE1);
   }

   void CMyWaveView::OnFile2Tran()
   {
	   // TODO: 在此添加命令处理程序代码
	    m_transID=_FILE2;
		Invalidate();

   }

   void CMyWaveView::OnUpdateFile2Tran(CCmdUI *pCmdUI)
   {
	   // TODO: 在此添加命令更新用户界面处理程序代码
	   pCmdUI->SetCheck (m_transID==_FILE2);
   }

   void CMyWaveView::OnFile3Tran()
   {
	   // TODO: 在此添加命令处理程序代码
	    m_transID=_FILE3;
		Invalidate();

   }

   void CMyWaveView::OnUpdateFile3Tran(CCmdUI *pCmdUI)
   {
	   // TODO: 在此添加命令更新用户界面处理程序代码
	   pCmdUI->SetCheck (m_transID==_FILE3);
   }

   void CMyWaveView::OnStopTran()
   {
	   // TODO: 在此添加命令处理程序代码
	   m_transID=NO_OPERATION;
	   hasTransRect=FALSE;
	   Invalidate();
   }

   void CMyWaveView::OnReturnPos()
   {
	   // TODO: 在此添加命令处理程序代码
	   m_X_File1_Dispos=0;
	   m_Y_File1_Dispos=0;
	   m_X_File2_Dispos=0;
	   m_Y_File2_Dispos=0;
	   m_X_File3_Dispos=0;
	   m_Y_File3_Dispos=0;
	   m_X_Cep1_Dispos=0;
	   m_Y_Cep1_Dispos=0;
	   m_X_Fft1_Dispos=0;
	   m_Y_Fft1_Dispos=0;

	   m_X_Lpc1_Dispos=0;
	   m_Y_Lpc1_Dispos=0;
	   m_X_Lpc2_Dispos=0;
	   m_Y_Lpc2_Dispos=0;

	   m_X_Lpcc1_Dispos=0;
	   m_Y_Lpcc1_Dispos=0;
	   m_X_Lpcc2_Dispos=0;
	   m_Y_Lpcc2_Dispos=0;

	   m_X_Mfcc1_Dispos=0;
	   m_Y_Mfcc1_Dispos=0;
	   m_X_Mfcc2_Dispos=0;
	   m_Y_Mfcc2_Dispos=0;

	   //文件1的对数功率谱的平移量
	   m_X_LogSpec1_Dispos=0;
	   m_Y_LogSpec1_Dispos=0;
	   //文件1的频谱的FFT的平移量
	   m_X_SpecFFT1_Dispos=0;
	   m_Y_SpecFFT1_Dispos=0;
	   //文件1的Mel频谱的平移量
	   m_X_MelSpec1_Dispos=0;
	   m_Y_MelSpec1_Dispos=0;


	   Invalidate();
   }

   void CMyWaveView::OnXposTran()
   {
	   // TODO: 在此添加命令处理程序代码
	   m_bOnlyXTran=!m_bOnlyXTran;
   }
 void CMyWaveView::OnUpdateXposTran(CCmdUI *pCmdUI)
   {
	   // TODO: 在此添加命令更新用户界面处理程序代码
	   pCmdUI->SetCheck (m_bOnlyXTran==TRUE);
   }
   void CMyWaveView::OnYposTran()
   {
	   // TODO: 在此添加命令处理程序代码
	   m_bOnlyYTran=!m_bOnlyYTran;
   }

   void CMyWaveView::OnUpdateYposTran(CCmdUI *pCmdUI)
   {
	   // TODO: 在此添加命令更新用户界面处理程序代码
	    pCmdUI->SetCheck (m_bOnlyYTran==TRUE);
   }

   ////////////////////////////////////////////////////////////////新添加 的平移倒谱
   void CMyWaveView::OnCep1Tran()
  {
	// TODO: 在此添加命令处理程序代码
	   m_transID=_CEP1;
		Invalidate();
  }

  void CMyWaveView::OnUpdateCep1Tran(CCmdUI *pCmdUI)
 {
	// TODO: 在此添加命令更新用户界面处理程序代码
	 pCmdUI->SetCheck (m_transID==_CEP1);
 }
 void CMyWaveView::OnFft1Tran()
 {
	// TODO: 在此添加命令处理程序代码
	  m_transID=_FFT1;
		Invalidate();
 }

 void CMyWaveView::OnUpdateFft1Tran(CCmdUI *pCmdUI)
 { 
	// TODO: 在此添加命令更新用户界面处理程序代码
	 pCmdUI->SetCheck (m_transID==_FFT1);
 }
 
void CMyWaveView::OnMfcc1Tran()
{
	// TODO: 在此添加命令处理程序代码
		m_transID=_MFCC1;
		Invalidate();
}

void CMyWaveView::OnUpdateMfcc1Tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
		pCmdUI->SetCheck (m_transID==_MFCC1);
}

void CMyWaveView::OnMfcc2Tran()
{
	// TODO: 在此添加命令处理程序代码
		m_transID=_MFCC2;
		Invalidate();
}

void CMyWaveView::OnUpdateMfcc2Tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck (m_transID==_MFCC2);
}


   ////////////////////////////////////////////////////////////////
void CMyWaveView::SetTransRect() //确定移动区域
{
	CMyWaveDoc* pDoc=GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
		return;

	int max,min;
	int separateMin,separateMax;
	switch(m_transID)
	{
	case _FILE1:
		max=pDoc->maxForOneData +(pDoc->m_extent)-m_YposArray[1][0];
		min=pDoc->minForOneData +pDoc->m_extent-m_YposArray[1][0];
		separateMin=min-m_Y_File1_Dispos;
		separateMax=max-m_Y_File1_Dispos;
		break;
	case _FILE2:
		max=pDoc->maxForOneData2 +(pDoc->m_extent)-m_YposArray[1][0];
		min=pDoc->minForOneData2 +pDoc->m_extent-m_YposArray[1][0] ;
		separateMin=min-m_Y_File2_Dispos;
		separateMax=max-m_Y_File2_Dispos;
		break;
	case _FILE3:
		max=pDoc->maxForOneData3 +(pDoc->m_extent);
		min=pDoc->minForOneData3 +pDoc->m_extent ;
		separateMin=min-m_Y_File3_Dispos;
		separateMax=max-m_Y_File3_Dispos;	
		break;
	case _FFT1://新添加的，用于频谱1的平移
		max=this->maxForOneFft+pDoc->m_extent-m_YposArray[2][0];
		min=this->minForOneFft+pDoc->m_extent-m_YposArray[2][0];
  	    separateMin=min-m_Y_Fft1_Dispos;
		separateMax=max-m_Y_Fft1_Dispos;
		break;
	case _FFT2://新添加的，用于频谱2的平移
		max=this->maxForTwoFft+pDoc->m_extent-m_YposArray[2][0];
		min=this->minForTwoFft+pDoc->m_extent-m_YposArray[2][0];
		separateMin=min-m_Y_Fft2_Dispos;
		separateMax=max-m_Y_Fft2_Dispos;
		break;
	case _CEP1://新添加的，用于倒谱1的平移
		max=this->maxForOneCep+pDoc->m_extent-m_YposArray[5][1];
		min=this->minForOneCep+pDoc->m_extent-m_YposArray[5][0];
		
		separateMin=min-m_Y_Cep1_Dispos;
		separateMax=max-m_Y_Cep1_Dispos;
		break;	
	case _CEP2://新添加的，用于倒谱2的平移
		max=this->maxForTwoCep+pDoc->m_extent-m_YposArray[5][1];
		min=this->minForTwoCep+pDoc->m_extent-m_YposArray[5][0];

		separateMin=min-m_Y_Cep2_Dispos;
		separateMax=max-m_Y_Cep2_Dispos;
		break;	
	case _LPC1:
		max=this->maxForOneLpc+pDoc->m_extent;
		min=this->minForOneLpc+pDoc->m_extent;
		separateMin=min-m_Y_Lpc1_Dispos;
		separateMax=max-m_Y_Lpc1_Dispos;
		break;	
	case _LPC2:
	    max=this->maxForTwoLpc+pDoc->m_extent;
		min=this->minForTwoLpc+pDoc->m_extent;
		separateMin=min-m_Y_Lpc2_Dispos;
		separateMax=max-m_Y_Lpc2_Dispos;
		break;
    case _LPCC1:
		max=this->maxForOneLpcc+pDoc->m_extent;
		min=this->minForOneLpcc+pDoc->m_extent;
		separateMin=min-m_Y_Lpcc1_Dispos;
		separateMax=max-m_Y_Lpcc1_Dispos;
		break;	
	case _LPCC2:
	    max=this->maxForTwoLpcc+pDoc->m_extent;
		min=this->minForTwoLpcc+pDoc->m_extent;
		separateMin=min-m_Y_Lpcc2_Dispos;
		separateMax=max-m_Y_Lpcc2_Dispos;
		break;
	case _MFCC1:
		max=this->maxForOneMfcc+pDoc->m_extent-m_YposArray[7][1];
		min=this->minForOneMfcc+pDoc->m_extent-m_YposArray[7][0];
		
		separateMin=min-m_Y_Mfcc1_Dispos;
		separateMax=max-m_Y_Mfcc1_Dispos;
		break;
	case _MFCC2:
		max=this->maxForTwoMfcc+pDoc->m_extent-m_YposArray[7][1];
		min=this->minForTwoMfcc+pDoc->m_extent-m_YposArray[7][0];
		separateMin=min-m_Y_Mfcc2_Dispos;
		separateMax=max-m_Y_Mfcc2_Dispos;
		break;
	case _SpecFFT1:
		max=this->maxForOneSpecFFT+pDoc->m_extent-m_YposArray[3][0];
		min=this->minForOneSpecFFT+pDoc->m_extent-m_YposArray[3][0];
		separateMin=min-m_Y_SpecFFT1_Dispos;
		separateMax=max-m_Y_SpecFFT1_Dispos;
		break;
	case _SpecFFT2:
		max=this->maxForTwoSpecFFT+pDoc->m_extent-m_YposArray[3][0];
		min=this->minForTwoSpecFFT+pDoc->m_extent-m_YposArray[3][0];
		separateMin=min-m_Y_SpecFFT2_Dispos;
		separateMax=max-m_Y_SpecFFT2_Dispos;
		break;
	case _LogSpec1:
		max=this->maxForOneLogSpec+pDoc->m_extent-m_YposArray[4][1];
		min=this->minForOneLogSpec+pDoc->m_extent-m_YposArray[4][0];
		separateMin=min-m_Y_LogSpec1_Dispos;
		separateMax=max-m_Y_LogSpec1_Dispos;
		break;
	case _LogSpec2:
		max=this->maxForTwoLogSpec+pDoc->m_extent-m_YposArray[4][1];
		min=this->minForTwoLogSpec+pDoc->m_extent-m_YposArray[4][0];
		separateMin=min-m_Y_LogSpec2_Dispos;
		separateMax=max-m_Y_LogSpec2_Dispos;
		break;
	case _MelSpec1:
		max=this->maxForOneMelSpec+pDoc->m_extent-m_YposArray[6][0];
		min=this->minForOneMelSpec+pDoc->m_extent-m_YposArray[6][0];
		separateMin=min-m_Y_MelSpec1_Dispos;
		separateMax=max-m_Y_MelSpec1_Dispos;
		break;
	case _MelSpec2:
		max=this->maxForTwoMelSpec+pDoc->m_extent-m_YposArray[6][0];
		min=this->minForTwoMelSpec+pDoc->m_extent-m_YposArray[6][0];
		separateMin=min-m_Y_MelSpec2_Dispos;
		separateMax=max-m_Y_MelSpec2_Dispos;
		break;
	case _CepDetect1:
		max=pDoc->maxForOneIfft+pDoc->m_extent-m_YposArray[0][0];
		min=pDoc->minForOneIfft+pDoc->m_extent-m_YposArray[0][1];
		separateMin=min-m_Y_IFFT1_Dispos;
		separateMax=max-m_Y_IFFT1_Dispos;
		break;
	case _CepDetect2:
		max=pDoc->maxForTwoIfft+pDoc->m_extent-m_YposArray[0][0];
		min=pDoc->minForTwoIfft+pDoc->m_extent-m_YposArray[0][1];
		separateMin=min-m_Y_IFFT2_Dispos;
		separateMax=max-m_Y_IFFT2_Dispos;
		break;
	}
	
	if(separateMax>=m_Ypos&&separateMin<=m_Xpos+(int)(m_bottom-m_top)/m_Yzoom)
	{
		transRect.left =m_left;
		transRect.right =m_right;
		switch(m_transID)
		{
		case _FILE1:
		   {
			   int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_File1_Dispos*m_Yzoom;
			   if(top<m_top)
				   transRect.top =m_top;
			   else
				   transRect.top =top;
			   int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_File1_Dispos*m_Yzoom;
			   if(bottom>m_bottom)
				   transRect.bottom=m_bottom;
			   else
				   transRect.bottom=bottom;
			   hasTransRect=TRUE;
			   break;
		   }
		case _FILE2:
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_File2_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_File2_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _FILE3:
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_File3_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_File3_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _CEP1://新添加的
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_Cep1_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_Cep1_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _CEP2://新添加的
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_Cep2_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_Cep2_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _FFT1://新添加的
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_Fft1_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_Fft1_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _FFT2://新添加的
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_Fft2_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_Fft2_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _LPC1://新添加的LPC1
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_Lpc1_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_Lpc1_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _LPC2://新添加的LPC2
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_Lpc2_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_Lpc2_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}	
		case _LPCC1://新添加的LPCC1
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_Lpcc1_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_Lpcc1_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _LPCC2://新添加的LPC2
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_Lpcc2_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_Lpcc2_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _MFCC1://新添加的MFCC1
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_Mfcc1_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_Mfcc1_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _MFCC2://新添加的MFCC2
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_Mfcc2_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_Mfcc2_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _SpecFFT1://新添加的频谱的FFT
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_SpecFFT1_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_SpecFFT1_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _SpecFFT2://新添加的频谱的FFT
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_SpecFFT2_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_SpecFFT2_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _LogSpec1://新添加的对数功率谱
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_LogSpec1_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_LogSpec1_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _LogSpec2://新添加的对数功率谱
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_LogSpec2_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_LogSpec2_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _MelSpec1://新添加的Mel频谱
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_MelSpec1_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_MelSpec1_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _MelSpec2://新添加的Mel频谱
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_MelSpec2_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_MelSpec2_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _CepDetect1://新添加的cep端点检测1
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_IFFT1_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_IFFT1_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		case _CepDetect2://新添加的cep端点检测1
			{
				int top=m_bottom-(int)((max-m_Ypos)*m_Yzoom)+m_Y_IFFT2_Dispos*m_Yzoom;
				if(top<m_top)
					transRect.top =m_top;
				else
					transRect.top =top;
				int bottom=m_bottom-(int)((min-m_Ypos)*m_Yzoom)+m_Y_IFFT2_Dispos*m_Yzoom;
				if(bottom>m_bottom)
					transRect.bottom=m_bottom;
				else
					transRect.bottom=bottom;
				hasTransRect=TRUE;
				break;
			}
		default: 
			hasTransRect=FALSE;
			break;
			}
	}
	else
		hasTransRect=FALSE;

}
void CMyWaveView::DrawTransRect (CDC *pDC)////绘制移动区域
{
     if(hasTransRect)
	 {
		 CBrush *pBrush=CBrush::FromHandle ((HBRUSH)GetStockObject(NULL_BRUSH));
		 CPen penRect(PS_SOLID,2,RGB(200,100,100));
		 pDC->SelectObject (&penRect);
		 pDC->SelectObject (pBrush);
		 pDC->Rectangle (transRect);
	 }
}

void CMyWaveView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    m_bMouseDown=TRUE;
	m_bDrawing=FALSE;
	m_oldPoint=point;//记录选中区域的起始点
	m_prePoint=point;
	if(m_transID!=NO_OPERATION&& transRect.PtInRect (point))//PtInRect()这个函数判断指定的点是否位于矩形transRect内部 ,返回值为非零表示在里面
	{
           SetCursor(hCursorTrans);//鼠标显示为移动形式
	}
	else if(m_transID==NO_OPERATION)
		m_bStartDrawRect=TRUE;
	CView::OnLButtonDown(nFlags, point);
}

void CMyWaveView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    m_bMouseDown=FALSE;
	m_bStartDrawRect=FALSE;
	if(m_bDrawing==TRUE)
	{
       CClientDC dc(this);
	    
	   int nDrawMode=dc.SetROP2 (R2_NOT);//R2_NOT就是取反的意思，即前景色为背景色的反色，经常用R2_NOT来画橡皮线，因为两次取反可以还原背景色。
	   dc.SelectStockObject (NULL_BRUSH);
	   dc.Rectangle (m_oldPoint.x ,m_oldPoint.y,point.x,point.y );
	   dc.SetROP2 (nDrawMode);

	   //设置区域播放的两端
	   m_startPoint=m_oldPoint;//保存选中区域的起点
	   m_endPoint=point;       //保存选中区域的末点

	   Invalidate();
	}
	if(m_oldPoint.x>point.x)//起始点在末点右边，交换两个点的值
	{
		CPoint t=point;
		point=m_oldPoint;
		m_oldPoint=t;
	}
    m_startPlayPos=(m_oldPoint.x -m_left)/m_Xzoom-m_X_File1_Dispos+m_Xpos;
	m_endPlayPos=(point.x-m_left)/m_Xzoom-m_X_File1_Dispos+m_Xpos;
	m_startPlayPos2=(m_oldPoint.x -m_left)/m_Xzoom-m_X_File2_Dispos+m_Xpos;
	m_endPlayPos2=(point.x-m_left)/m_Xzoom-m_X_File2_Dispos+m_Xpos;
	m_startPlayPos3=(m_oldPoint.x -m_left)/m_Xzoom-m_X_File3_Dispos+m_Xpos;
	m_endPlayPos3=(point.x-m_left)/m_Xzoom-m_X_File3_Dispos+m_Xpos;
	CView::OnLButtonUp(nFlags, point);
}

void CMyWaveView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDC* pDC=GetDC();
	CString info;
	if(point.x >=m_left && point.x<=m_right && point.y >=m_top && point.y <=m_bottom)//point在视图区域内
	{
		SetCursor(hCursorCross);
		double ptx=(point.x-m_left+m_Xpos*m_Xzoom)/m_Xzoom;
		double pty=m_Ypos+(m_bottom-point.y)/m_Yzoom-m_extent;
		//鼠标不在移动方框之内时的显示
		info.Format(_T("序号:%d 值:%d"),(int)ptx,(int)pty);
		pDC->SetTextColor(RGB(255,255,255));
		pDC->TextOut(m_right-150,m_top+2,m_old_info);
		pDC->SetTextColor(RGB(0,0,0));
		pDC->TextOut(m_right-150,m_top+2,info);
		m_old_info=info;

		if(m_transID!=NO_OPERATION)//移动选中对象
		{
			if(transRect.PtInRect (point))
			{
				if(m_bMouseDown)
				{
					SetCursor(hCursorTrans);

					int x=point.x -m_oldPoint.x;
					int y=point.y -m_oldPoint.y;

					if (x!=0&&m_bOnlyXTran==TRUE)//x方向移动不为0，且允许x方向移动。
					{
						switch(m_transID)
						{
						case _FILE1:
						case _CEP1:
						case _FFT1:
						case _LPC1:
						case _LPCC1:
						case _MFCC1:
						case _SpecFFT1:
						case _LogSpec1:
						case _MelSpec1:
						case _CepDetect1:
							{
								m_X_File1_Dispos+=x/m_Xzoom;
								m_X_Cep1_Dispos+=x/m_Xzoom;
								m_X_Fft1_Dispos+=x/m_Xzoom;
								m_X_Lpc1_Dispos+=x/m_Xzoom;
								m_X_Lpcc1_Dispos+=x/m_Xzoom;
								m_X_Mfcc1_Dispos+=x/m_Xzoom;
								m_X_SpecFFT1_Dispos+=x/m_Xzoom;
								m_X_LogSpec1_Dispos+=x/m_Xzoom;
								m_X_MelSpec1_Dispos+=x/m_Xzoom;
								m_X_IFFT1_Dispos+=x/m_Xzoom;
								break;
							}
						case _FILE2:
						case _CEP2:
						case _FFT2:
						case _LPC2:
						case _LPCC2:
						case _MFCC2:
						case _SpecFFT2:
						case _LogSpec2:
						case _MelSpec2:
						case _CepDetect2:							
							{
								m_X_File2_Dispos+=x/m_Xzoom;
								m_X_Cep2_Dispos+=x/m_Xzoom;
								m_X_Fft2_Dispos+=x/m_Xzoom;
								m_X_Lpc2_Dispos+=x/m_Xzoom;
								m_X_Lpcc2_Dispos+=x/m_Xzoom;
								m_X_Mfcc2_Dispos+=x/m_Xzoom;
								m_X_SpecFFT2_Dispos+=x/m_Xzoom;
								m_X_LogSpec2_Dispos+=x/m_Xzoom;
								m_X_MelSpec2_Dispos+=x/m_Xzoom;
								m_X_IFFT2_Dispos+=x/m_Xzoom;
								break;
							}
						case _FILE3:
							{
								if(m_bOnlyXTran)
									m_X_File3_Dispos+=x/m_Xzoom;
								break;
							}
						}
					}//横向移动
					if (y!=0&&m_bOnlyYTran==TRUE)
					{
						switch(m_transID)
						{
						case _FILE1:
							if(m_bOnlyYTran)
								m_Y_File1_Dispos+=y/m_Yzoom;
							break;					 
						case _FILE2:
							if(m_bOnlyYTran)
								m_Y_File2_Dispos+=y/m_Yzoom;
							break;
						case _FILE3:
							if(m_bOnlyYTran)
								m_Y_File3_Dispos+=y/m_Yzoom;
							break;
						case _CEP1://新添加
							if(m_bOnlyYTran)
								m_Y_Cep1_Dispos+=y/m_Yzoom;
							break;
						case _CEP2://新添加
							if(m_bOnlyYTran)
								m_Y_Cep2_Dispos+=y/m_Yzoom;
							break;
						case _FFT1://新添加
							if(m_bOnlyYTran)
								m_Y_Fft1_Dispos+=y/m_Yzoom;
							break;	 
						case _FFT2://新添加
							if(m_bOnlyYTran)
								m_Y_Fft2_Dispos+=y/m_Yzoom;
							break;	 
						case _LPC1://新添加
							if(m_bOnlyYTran)
								m_Y_Lpc1_Dispos+=y/m_Yzoom;
							break;
						case _LPC2://新添加
							if(m_bOnlyYTran)
								m_Y_Lpc2_Dispos+=y/m_Yzoom;
							break;
						case _LPCC1://新添加
							if(m_bOnlyYTran)
								m_Y_Lpcc1_Dispos+=y/m_Yzoom;
							break;
						case _LPCC2://新添加
							if(m_bOnlyYTran)
								m_Y_Lpcc2_Dispos+=y/m_Yzoom;
							break;
						case _MFCC1://新添加MFCC
							if(m_bOnlyYTran)
								m_Y_Mfcc1_Dispos+=y/m_Yzoom;
							break;
						case _MFCC2://新添加MFCC
							if(m_bOnlyYTran)
								m_Y_Mfcc2_Dispos+=y/m_Yzoom;
							break;
						case _SpecFFT1://新添加频谱的FFT
							if(m_bOnlyYTran)
								m_Y_SpecFFT1_Dispos+=y/m_Yzoom;
							break;
						case _SpecFFT2://新添加频谱的FFT
							if(m_bOnlyYTran)
								m_Y_SpecFFT2_Dispos+=y/m_Yzoom;
							break;
						case _LogSpec1://新添加对数功率谱
							if(m_bOnlyYTran)
								m_Y_LogSpec1_Dispos+=y/m_Yzoom;
							break;
						case _LogSpec2://新添加对数功率谱
							if(m_bOnlyYTran)
								m_Y_LogSpec2_Dispos+=y/m_Yzoom;
							break;
						case _MelSpec1://新添加Mel频谱
							if(m_bOnlyYTran)
								m_Y_MelSpec1_Dispos+=y/m_Yzoom;
							break;
						case _MelSpec2://新添加Mel频谱
							if(m_bOnlyYTran)
								m_Y_MelSpec2_Dispos+=y/m_Yzoom;
							break;
						case _CepDetect1://新添加Mel频谱
							if(m_bOnlyYTran)
								m_Y_IFFT1_Dispos+=y/m_Yzoom;
							break;
						case _CepDetect2://新添加Mel频谱
							if(m_bOnlyYTran)
								m_Y_IFFT2_Dispos+=y/m_Yzoom;
							break;
						}
					}//纵向移动
					m_oldPoint=point;
					Invalidate();
				}
			}
		}
		else if(m_bStartDrawRect)//画选中区域
		{
			m_bDrawing=TRUE;
			if(m_bMouseDown)
			{
				int nDrawMode=pDC->SetROP2 (R2_NOT);

				pDC->SelectStockObject (NULL_BRUSH);
				pDC->Rectangle (m_oldPoint.x,m_oldPoint.y,m_prePoint.x,m_prePoint.y );
				pDC->Rectangle (m_oldPoint.x,m_oldPoint.y,point.x,point.y );

				m_prePoint=point;
				pDC->SetROP2 (nDrawMode);

				int width=abs(point.x -m_oldPoint.x );				
				if(m_bShowCep1)
					m_rect_info.Format (_T("实际点数约%05d,基因周期约%04f ms"),(int)(width/m_Xzoom),(float)(width/m_Xzoom/16));
				else
					m_rect_info.Format (_T("窗口象素宽度为%04d,实际包含点数约%05d"),width,(int)(width/m_Xzoom));	 

				pDC->TextOutW (m_right-300,m_bottom-20,m_rect_info);
			}

		}
	}
	ReleaseDC(pDC);
	CView::OnMouseMove(nFlags, point);
}
/************************************************************************************
播放音频相关的操作函数
************************************************************************************/

void CMyWaveView::OnFile1Play()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc=GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
		return;
	 
	if(pDoc->m_strFileName ==_T(" "))
		AfxMessageBox(_T("请先打开文件！"));
	else {
		if(m_startPlayPos==m_endPlayPos)
			PlaySound(pDoc->m_strFilePath ,NULL,SND_ASYNC);
		else 
			PlayZoneSound(pDoc->header ,pDoc->data_one ,m_startPlayPos,m_endPlayPos);
	}
}

void CMyWaveView::OnFile2Play()
{
	// TODO: 在此添加命令处理程序代码
		CMyWaveDoc* pDoc=GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
		return;
	 
	if(pDoc->m_strFileName2 ==_T(" "))
		AfxMessageBox(_T("请先打开文件！"));
	else {
		if(m_startPlayPos2==m_endPlayPos2)
			PlaySound(pDoc->m_strFilePath2 ,NULL,SND_ASYNC);
		else 
			PlayZoneSound(pDoc->header2 ,pDoc->data_one2 ,m_startPlayPos2,m_endPlayPos2);
	}
}

void CMyWaveView::OnFile3Play()
{
	// TODO: 在此添加命令处理程序代码
		CMyWaveDoc* pDoc=GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
		return;
	 
	if(pDoc->m_strFileName3 ==_T(" "))
		AfxMessageBox(_T("请先打开文件！"));
	else {
		if(m_startPlayPos3==m_endPlayPos3)
			PlaySound(pDoc->m_strFilePath3 ,NULL,SND_ASYNC);
		else 
			PlayZoneSound(pDoc->header3 ,pDoc->data_one3 ,m_startPlayPos3,m_endPlayPos3);
	}
}

void CMyWaveView::OnStopPlay()
{
	// TODO: 在此添加命令处理程序代码
	PlaySound(NULL,NULL,SND_ASYNC);
}

void CMyWaveView::OnCancelZone()//取消选中框
{
	// TODO: 在此添加命令处理程序代码
	m_startPoint.x=m_endPoint.x =0;
	m_startPlayPos=m_endPlayPos=0;
	m_startPlayPos2=m_endPlayPos2=0;
	m_startPlayPos3=m_endPlayPos3=0;
	Invalidate();
}
void CMyWaveView::PlayZoneSound(WAVEFILEHEADER head,const std::vector <short>&data,int start,int end)//播放区域音频
{
    if(start<0||end>=data.size ())
	{
		AfxMessageBox(_T("所选的区域超出范围"));
		return;
	}
	int rate=1000;//刺激率
	int sampleRate=8000;//采样率
	int NUMPTS=end-start+1;
	short* waveOut=new short[NUMPTS];//音频数据

	for(int i=start;i<=end;i++)
		waveOut[i-start]=data[i];

	HWAVEOUT  hWaveOut;
	WAVEHDR   WaveOutHdr;
	MMRESULT  result;

	//specify recording parameters
	WAVEFORMATEX pFormat;
	pFormat.wFormatTag =WAVE_FORMAT_PCM;
	pFormat.nChannels =head.fmt .wavFormat.wChannels ;
	pFormat.nSamplesPerSec =head.fmt.wavFormat.dwSamplesPerSec ;
	pFormat.nAvgBytesPerSec =head.fmt.wavFormat.dwAvgBytesPerSec ;
	pFormat.nBlockAlign =head.fmt.wavFormat.wBlockAlign ;
	pFormat.wBitsPerSample =head.fmt.wavFormat.wBitsPerSample ;
	pFormat.cbSize =0;


	result=waveOutOpen(&hWaveOut,WAVE_MAPPER,&pFormat,0L,0L,WAVE_FORMAT_DIRECT);
	if(result)
	{
		MessageBox(_T("Failed to open waveform output device!"));
		return;
	}

     //set up and prpare header for output
	WaveOutHdr.lpData =(LPSTR)waveOut;
	WaveOutHdr.dwBufferLength =NUMPTS*2;
	WaveOutHdr.dwBytesRecorded =0;
	WaveOutHdr.dwUser =0L;
	WaveOutHdr.dwFlags =0L;
	WaveOutHdr.dwLoops=1L;
	waveOutPrepareHeader(hWaveOut,&WaveOutHdr,sizeof(WAVEHDR));

	result=waveOutWrite(hWaveOut,&WaveOutHdr,sizeof(WAVEHDR));
	if(result)
	{
		MessageBox(_T("failed to play sound!"));
		return;
	}

	//wait until finished sound play
	do
	{}while(waveOutUnprepareHeader(hWaveOut,&WaveOutHdr,sizeof(WAVEHDR))==WAVERR_STILLPLAYING);

	waveOutClose(hWaveOut);
	
	delete[] waveOut;


}

//////////////////////////////////////////////////////////////////////
/////////频域分析
//////////////////////////////////////////////////////////////////////fft
void CMyWaveView::OnFft( )
{
	 //TODO: 在此添加命令处理程序代码
    CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 int i;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout != NULL)
		delete[] dataout;
		
	data=new double[sample_rate];
 
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
		double *pRealOut,*pImageOut;
	    pRealOut = new double[sample_rate];
	    pImageOut = new double[sample_rate];
    //加窗
    	CSpeech::AddWindow (this->m_nFrameSize ,data,num);
	// 进行FFT变换
	CWaveConvertor::ConvertToFFT(sample_rate,this->m_nFrameSize, data, pRealOut, pImageOut);

    //保存fft数据
	   CFileDialog dlg(false, NULL, L"", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"fft文件（*.fft）|*.fft|所有文件(*.*)|*.*||");

if(dlg.DoModal()==IDOK)
{
	CFileException fileException;
	//获取路径
	CString m_csFileName=dlg.GetPathName();
	CString FilePath=dlg.GetPathName ();
	FilePath+=_T(".fft");
 	m_csFileName+=_T(".fft");
  
	ofstream fout( m_csFileName, ios::app );
	
    for (unsigned int i = 0; i < sample_rate; i++)
	{	    
			fout <<i<<"    "<<pRealOut[i]<<"+"<<pImageOut[i]<<"i"<< endl; 
	}
	
    //关闭输出流 
    fout.close();
}
delete [] pRealOut;
delete [] pImageOut;
}

//////////////////////////////////////////////////////////////////////////功率谱
void CMyWaveView::OnSpectrum()
{

	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 int i;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout_spectrum != NULL)
		delete[] dataout_spectrum;
		
	data=new double[sample_rate];
    dataout_spectrum=new double[sample_rate];
	pOutNum=sample_rate;
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
	} 
	for (i=num;i<sample_rate;i++)
		data[i]=0;
	//加窗
  	CSpeech::AddWindow (this->m_nFrameSize ,data,num);
	// 获取功率谱	
	CWaveConvertor::ConvertToPowerSpectral(
					sample_rate, this->m_nFrameSize, data, dataout_spectrum);

	for(unsigned int i=0;i<nCount;i++)
	{
		for (unsigned int j = 0; j <this->m_nFrameSize; j++)
		{	
			if(maxForOneFft<dataout_spectrum[i*this->m_nFrameSize+j])
				maxForOneFft=dataout_spectrum[i*this->m_nFrameSize+j];
			if(minForOneFft>dataout_spectrum[i*this->m_nFrameSize+j])
				minForOneFft=dataout_spectrum[i*this->m_nFrameSize+j];			
		}
	}
}
//////////////////////////////////////////////////////////////////对数功率谱(对数振幅谱)
void CMyWaveView::OnLogspectrum()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	unsigned int num=pDoc->data_one.size ();
    int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
    unsigned int sample_rate=nCount*this->m_nFrameSize;
	int i;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout != NULL)
		delete[] dataout;
		
	data=new double[sample_rate];
    dataout=new double[sample_rate];
	pOutNum=sample_rate;
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
	} 
	for (i=num;i<sample_rate;i++)
		data[i]=0;
	//加窗
  	CSpeech::AddWindow (this->m_nFrameSize ,data,num);
	// 获取对数功率谱
	CWaveConvertor::ConvertToLogPowerSpectral(
					sample_rate, this->m_nFrameSize, data, dataout);

	maxForOneLogSpec=minForOneLogSpec=dataout[0];
	for(unsigned int i=0;i<sample_rate;i++)
	{
		
		if (maxForOneLogSpec<dataout[i])
		{
			maxForOneLogSpec=dataout[i];
		}
		if (minForOneLogSpec>dataout[i])
		{
			minForOneLogSpec=dataout[i];
		} 	
	}
	/***********************归一化*********************/
	double magnify=5000/(maxForOneLogSpec-minForOneLogSpec);
	for (int j=0;j<sample_rate;j++)
	{
		dataout[j]=(dataout[j]-minForOneLogSpec)*magnify;
	}

	maxForOneLogSpec=4000;
	minForOneLogSpec=0;
	/***************************************************/

}
///////////////////////////////////////////////////////////////////////倒谱
void CMyWaveView::OnCepstrum()
{	//修改了dataout及振幅倍数20000。
	
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 int i;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout_cep!=NULL)
	{
		delete []dataout_cep;
	}
		
	data=new double[sample_rate];
	dataout_cep=new double[sample_rate];

    pOutNum=sample_rate;
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
	} 
	for (i=num;i<sample_rate;i++)
		data[i]=0;
	
	//加窗
  	 CSpeech::AddWindow (this->m_nFrameSize ,data,num);
	// 获取倒谱
	CWaveConvertor::ConvertToCepStrum(
					sample_rate, this->m_nFrameSize, data, dataout_cep);//dataout--dataout_cep

      
	maxForOneCep=minForOneCep=dataout_cep[0]*10000;
	for(unsigned int i=0;i<nCount;i++)
	{
		for (unsigned int j = 0; j <this->m_nFrameSize; j++)
		{	
			dataout_cep[i*this->m_nFrameSize+j]=dataout_cep[i*this->m_nFrameSize+j]*10000;//原:100000/2		
			
			if(maxForOneCep<dataout_cep[i*this->m_nFrameSize+j])
				maxForOneCep=dataout_cep[i*this->m_nFrameSize+j];
			if(minForOneCep>dataout_cep[i*this->m_nFrameSize+j])
				minForOneCep=dataout_cep[i*this->m_nFrameSize+j];
		}
	}
}
///////////////////////////////////////////////////、、、、、、、、时域分析 
/////////////////////////短时能量
void CMyWaveView::OnPower()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	 unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 double *pDataOut=NULL;
	 int i,j;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout != NULL)
		delete[] dataout;
		
	data=new double[num];
	//ofstream fout(_T("陈峥嵘.txt"),ios::trunc);
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
		//fout<<data[i]<<endl;
	} 
	//fout.close();
	
	dataout=new double[nCount];
	pDataOut=new double [sample_rate];
	CSpeech::Frame(this->m_nFrameSize,data,num,pDataOut);
	// 获取短时能量
   
		CSpeech::GetEnergy(
					this->m_nFrameSize, pDataOut, sample_rate, dataout);

		double deltaEnergyLevel1 = -20; //短时能量的两个门限
		double deltaEnergyLevel2 = -10;

		
		for(i=0;i<nCount;i++)//求每一帧的能量均值
			dataout[i]=dataout[i]/this->m_nFrameSize;
		
		double max=10*log10(dataout[0]),min=10*log10(dataout[0]);

		for(i=0;i<nCount;i++)//求每一帧的对数能量		
		{
			if(dataout[i]!=0)
			   dataout[i]=10*log10(dataout[i]);
			if(dataout[i]>max)
				max=dataout[i];
			if(dataout[i]<min)
				min=dataout[i];
		}
		//计算短时能量和门限		
        double energyLevel1 = max+deltaEnergyLevel1;
        double energyLevel2 = max+deltaEnergyLevel2;

		// 获取短时过零率
		double *zcr=new double[nCount];

		CSpeech::GetZero(
					this->m_nFrameSize,pDataOut, sample_rate, zcr);
		double max_zero=zcr[0];
        for(i=1;i<nCount;i++)
		{
			if(zcr[i]>max_zero)
				max_zero=zcr[i];
		}	
		ofstream fout2("zcr.txt",ios::trunc);
		for(i=0;i<nCount;i++)
		{
			fout2<<zcr[i]<<endl;
		}
		fout2.close();
		double zcrRatio=0.20;//短时过零率门限
		double zcrThreshold = max_zero*zcrRatio;
		///////////////////////////////////////////////////根据较高的门限找到起止点
		std::vector<short> voiceIndex;
        for(i=0;i<nCount;i++)
		{
			if(dataout[i]>energyLevel2)
				voiceIndex.push_back(i);
		}
  //      ofstream fout1("energy2.txt",ios::trunc);
		//for(i=0;i<voiceIndex.size();i++)
		//{
		//	fout1<<voiceIndex[i]<<endl;
		//}
		//fout1.close();
		//////////////////////////////////////////记录超过门限的语音段的起始点
		std::vector<short> sound;
		int k=1;
		int nums=voiceIndex.size();
		sound.push_back(voiceIndex[0]);
		for(i=1;i<nums-1;i++)
		{
			if(voiceIndex[i+1]-voiceIndex[i]>1)
			{
				sound.push_back(voiceIndex[i]);
				sound.push_back(voiceIndex[i+1]);
				k++;
			}
		}
		sound.push_back(voiceIndex[nums-1]);


		///////////////////////////////////////////% 忽略一些小的细节

		std::vector<short> sound2;
		for(i=0;i<sound.size()/2;i++)
		{
			if(sound[2*i+1]-sound[2*i]>=3)
			{
				sound2.push_back(sound[2*i]);
				sound2.push_back(sound[2*i+1]);
			}
		}

        //////////////////////////////////////////根据较低的门限进一步判断

		int head,tail;
		for(i=0;i<sound2.size()/2;i++)
		{
			head=sound2[2*i];
			while((head-1)>=0&&dataout[head-1]>energyLevel1)//判断前一帧大于能量阈值1吗，
				head=head-1;
			sound2[2*i]=head;
			tail=sound2[2*i+1];
			while((tail+1)<nCount&&dataout[tail+1]>energyLevel1)
				tail+=1;
			sound2[2*i+1]=tail;
		}

		//////////////////////////////////////////根据zcr门限判断起止点
		for(i=0;i<sound2.size()/2;i++)
		{
			head=sound2[2*i];
			while((head-1)>=0&&zcr[head-1]>=zcrThreshold)
				head=head-1;
			sound2[2*i]=head;
			tail=sound2[2*i+1];
			while((tail+1)<nCount&&zcr[tail+1]>zcrThreshold)
				tail+=1;
			sound2[2*i+1]=tail;
		}

		/////////////////////////////////////去掉重复的语音帧
		std::vector <short> index;	
		std::vector<short> sound3;
		for(i=0;i<sound2.size()/2-1;i++)
		{
			if((sound2[2*i]==sound2[2*(i+1)]&&sound2[2*i+1]==sound2[2*(i+1)+1]))
		        index.push_back(i);
		}
		bool b;
		for(i=0;i<sound2.size()/2;i++)
		{
			
			b=TRUE;
			for(j=0;j<index.size();j++)
			{
				if(i==index[j])
				{
					b=FALSE;
					break;	
				}			 

			}	
			if(b==TRUE)
			{
				sound3.push_back(sound2[2*i]);
		        sound3.push_back(sound2[2*i+1]);
			}
        }

		
		ofstream fout3("energy3.txt",ios::trunc);
		for(i=0;i<sound3.size()/2;i++)
		{
			fout3<<sound3[2*i]<<"   "<<sound3[2*i+1]<<endl;
		}
		fout3.close();
        ////////////////////////将序列变换成整段语音的样点序列
		std::vector <short> out;
		std::vector<short> datas;//端点检测后的数据
		if(sound3.size()!=0)
		{
			//for(i=0;i<sound3.size()/2;i++)
			//{
			//	out.push_back(sound3[2*i]*(this->m_nFrameSize-this->m_nFrameSize/2));
			//	out.push_back(sound3[2*i+1]*(this->m_nFrameSize-this->m_nFrameSize/2)+this->m_nFrameSize/2-1);      
			//}		
			
			for(i=0;i<out.size()/2;i++)
			{
				for(j=sound3[2*i];j<=sound3[2*i+1];j++)
				{
					for(k=0;k<this->m_nFrameSize;k++)
						datas.push_back(pDataOut[j*this->m_nFrameSize+k]);
				}
					
			}
		}





	//保存短时能量数据
	   CFileDialog dlg(false, NULL, L"", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"短时能量文件（*.power）|*.power|所有文件(*.*)|*.*||");

if(dlg.DoModal()==IDOK)
{
	CFileException fileException;
	//获取路径
	CString m_csFileName=dlg.GetPathName();
	CString FilePath=dlg.GetPathName ();
	FilePath+=_T(".power");
 	m_csFileName+=_T(".power");
  
	ofstream fout( m_csFileName, ios::app );
	
    for (unsigned int i = 0; i < nCount; i++)
	{	
			fout <<dataout[i]<< endl; 
	}
    //关闭输出流 
    fout.close();
}
}
//////////////////////////////////短时过零率
void CMyWaveView::OnZero()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 int i;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout != NULL)
		delete[] dataout;
		
	data=new double[sample_rate];
    
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
	} 
	for (i=num;i<sample_rate;i++)
		data[i]=0;
	
	dataout=new double[nCount];
	// 对输入数据进行加窗处理
		CSpeech::AddWindow(this->m_nFrameSize, data, num);
	// 获取短时过零率
		CSpeech::GetZero(
					this->m_nFrameSize, data,sample_rate, dataout);
	//保存短时过零数据
	   CFileDialog dlg(false, NULL, L"", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"短时过零率文件（*.zero）|*.zero|所有文件(*.*)|*.*||");

if(dlg.DoModal()==IDOK)
{
	CFileException fileException;
	//获取路径
	CString m_csFileName=dlg.GetPathName();
	CString FilePath=dlg.GetPathName ();
	FilePath+=_T(".zero");
 	m_csFileName+=_T(".zero");
  
	ofstream fout( m_csFileName, ios::app );
	
    for (unsigned int i = 0; i < nCount; i++)
	{	
			fout <<dataout[i]<< endl; 
	}
}
 	
}
/////////////////////////////平均振幅
void CMyWaveView::OnVibration()
{
	// TODO: 在此添加命令处理程序代码
CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
     unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 int i;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout != NULL)
		delete[] dataout;
		
	data=new double[sample_rate];
    
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
	} 
	for (i=num;i<sample_rate;i++)
		data[i]=0;
	
	dataout=new double[nCount];
	// 对输入数据进行加窗处理
		CSpeech::AddWindow(this->m_nFrameSize, data, num);
	// 获取平均振幅
		CSpeech::GetAvgVibration(
					this->m_nFrameSize, data,sample_rate, dataout);
	//保存平均振幅数据
	   CFileDialog dlg(false, NULL, L"", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"平均振幅文件（*.Vibration）|*.Vibration|所有文件(*.*)|*.*||");

if(dlg.DoModal()==IDOK)
{
	CFileException fileException;
	//获取路径
	CString m_csFileName=dlg.GetPathName();
	CString FilePath=dlg.GetPathName ();
	FilePath+=_T(".Vibration");
 	m_csFileName+=_T(".Vibration");
  
	ofstream fout( m_csFileName, ios::app );
	
    for (unsigned int i = 0; i < nCount; i++)
	{	
			fout <<dataout[i]<< endl; 
	}
    //关闭输出流 
    fout.close();
}
 	

}
////////////////////////////////////////////////修正的相关函数
void CMyWaveView::OnRcor()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

     unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 int i;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout != NULL)
		delete[] dataout;
		
	data=new double[sample_rate];
    
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
	} 
	for (i=num;i<sample_rate;i++)
		data[i]=0;
	
	dataout=new double[nCount*(ip+1)];
	// 对输入数据进行加窗处理
		CSpeech::AddWindow(this->m_nFrameSize, data, num);
	// 获取自相关函数
		CSpeech::GetRcor(
					this->m_nFrameSize, data, sample_rate, dataout);

	//修正的相关函数数据
	   CFileDialog dlg(false, NULL, L"", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"修正的相关函数文件（*.rcor）|*.rcor|所有文件(*.*)|*.*||");

   if(dlg.DoModal()==IDOK)
  {
	CFileException fileException;
	//获取路径
	CString m_csFileName=dlg.GetPathName();
	CString FilePath=dlg.GetPathName ();
	FilePath+=_T(".rcor");
 	m_csFileName+=_T(".rcor");
  
	ofstream fout( m_csFileName, ios::app );
	
    for (unsigned int i = 0; i <nCount*(ip+1); i++)
	{	
			fout <<dataout[i]<< endl; 
	}
    //关闭输出流 
    fout.close();
   }
}
////////////////////////////////////////////基于短时自相关法的基音周期估值
void CMyWaveView::OnPitch()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
     unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 //unsigned int nCount=num/this->m_nFrameSize;
	 unsigned int nFs=pDoc->m_HZ;
	 int i,j;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout != NULL)
		delete[] dataout;
	if (pitch != NULL)
		delete[] pitch;
		
	data=new double[num];
	//ofstream fout(_T("C:\\卜祥贺.txt"),ios::trunc);
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
		//fout<<data[i]<<endl;
	} 
	//fout.close();

	dataout=new double[(nCount-1)*this->m_nFrameSize];
	pitch=new double[nCount-1];//保存获得基音周期
	//// 对输入数据进行分帧、加窗处理  矩形窗
	// CSpeech::Frame(
	//	 this->m_nFrameSize,		// 处理窗宽度
	//	 data,		                // 输入数据
	//	 num,		                // 输入数据长度
	//	 dataout		         	// 输出分帧后的数据
	//	);
	  
	    // 获取基音周期数据(基于自相关法的基音周期估值)
		CSpeech::GetACFPitch(this->m_nFrameSize, data,num, pitch,nFs,dataout);
	    
	    ///////////////////////////////////////////////////////////////////////////////////曹强版<<
        /////ACF
		//CSpeech::GetAcfFrequency(this->m_nFrameSize, data, num, pitch,nFs);
		////AMDF
		//CSpeech::GetAmdfFrequency(data,num, nFs,this->m_nFrameSize,pitch );
		////////////////////////////////////////////////////////////////////////////////  >>
	//基音周期数据
				CString m_csFileName(pDoc->m_strFileName,pDoc->m_strFileName.GetLength()-3);
		CString m_csPath(pDoc->m_strFilePath,pDoc->m_strFilePath.GetLength()-3);
		m_csFileName+=_T("txt");
        m_csPath+=_T("txt");
			
		maxForOneCep=minForOneCep=dataout[0]*10000;
		//CString filename=(_T("data\\lpc\\")+m_csFileName);
		ofstream fout( m_csPath, ios::trunc );
		double sum=0.0;
		pOutNum=(nCount-1)*this->m_nFrameSize;
		for (i = 0; i <nCount-1; i++)
		{	
			for(j=0;j<this->m_nFrameSize;j++)
			{	fout <<dataout[i*this->m_nFrameSize+j]<< endl; 
			    dataout[i*this->m_nFrameSize+j]=dataout[i*this->m_nFrameSize+j]*10000;
			
			if(maxForOneCep<dataout[i*this->m_nFrameSize+j])
				maxForOneCep=dataout[i*this->m_nFrameSize+j];
			if(minForOneCep>dataout[i*this->m_nFrameSize+j])
				minForOneCep=dataout[i*this->m_nFrameSize+j];
			}
				//sum+=pitch[i];
		}		//sum=sum/nCount;
		//fout <<sum<< endl;
		//关闭输出流 
		fout.close();
  
		//CString m_csFileName(pDoc->m_strFileName,pDoc->m_strFileName.GetLength()-3);
		//CString m_csPath(pDoc->m_strFilePath,pDoc->m_strFilePath.GetLength()-3);
		//m_csFileName+=_T("txt");
  //      m_csPath+=_T("txt");
		////CString filename=(_T("data\\lpc\\")+m_csFileName);
		//ofstream fout( m_csPath, ios::trunc );
		//double sum=0.0;
		//for (unsigned int i = 0; i <nCount-1; i++)
		//{	
		//		fout <<pitch[i]<< endl; 
		//		//sum+=pitch[i];
		//}
		////sum=sum/nCount;
		////fout <<sum<< endl;
		////关闭输出流 
		//fout.close();

}
///////////////////////////////////////////////////基于AMDF的基音周期估值
void CMyWaveView::OnAmdfPitch()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
     unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 //unsigned int nCount=num/this->m_nFrameSize;
	 unsigned int nFs=pDoc->m_HZ;
	 int i;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout != NULL)
		delete[] dataout;
	if (pitch != NULL)
		delete[] pitch;
		
	data=new double[num];
	//ofstream fout(_T("C:\\卜祥贺.txt"),ios::trunc);
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
		//fout<<data[i]<<endl;
	} 
	//fout.close();
	pitch=new double[nCount-1];//保存获得基音周期
	//分帧加矩形窗，求取基音周期时，ACF和AMDF方法时，使用矩形窗
	 CSpeech::Frame(
		 this->m_nFrameSize,		// 处理窗宽度
		 data,		                // 输入数据
		 num,		                // 输入数据长度
		 dataout		         	// 输出分帧后的数据
		);
		////基于短时平均幅度差函数AMDF法的基音周期估值
		CSpeech::AMDFPitch(
					this->m_nFrameSize, dataout,sample_rate,pitch);

	//基音周期数据
  
		CString m_csFileName(pDoc->m_strFileName,pDoc->m_strFileName.GetLength()-3);
		CString m_csPath(pDoc->m_strFilePath,pDoc->m_strFilePath.GetLength()-3);
		m_csFileName+=_T("txt");
        m_csPath+=_T("txt");
		//CString filename=(_T("data\\lpc\\")+m_csFileName);
		ofstream fout( m_csPath, ios::trunc );
		double sum=0.0;
		for (unsigned int i = 0; i <nCount-1; i++)
		{	
				fout <<pitch[i]<< endl; 
				//sum+=pitch[i];
		}
		//sum=sum/nCount;
		//fout <<sum<< endl;
		//关闭输出流 
		fout.close();
}
///////////////////////////////////////////////////////基于倒谱的基音周期估值
void CMyWaveView::OnCepPitch()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
     unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 //unsigned int nCount=num/this->m_nFrameSize;
	 unsigned int nFs=pDoc->m_HZ;
	 int i,j;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout != NULL)
		delete[] dataout;
	if (pitch != NULL)
		delete[] pitch;
		
	data=new double[num];
	//ofstream fout(_T("C:\\卜祥贺.txt"),ios::trunc);
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
		//fout<<data[i]<<endl;
	} 
	//fout.close();
	dataout=new double[nCount*this->m_nFrameSize];
	pitch=new double[nCount];//保存获得基音周期
	////基于倒谱法的基音周期估值，加汉明窗
	CSpeech::CepPitch(
					this->m_nFrameSize, data,num/*sample_rate*/,dataout,pitch);

	 

	//基音周期数据
  
		CString m_csFileName(pDoc->m_strFileName,pDoc->m_strFileName.GetLength()-3);
		CString m_csPath(pDoc->m_strFilePath,pDoc->m_strFilePath.GetLength()-3);
		m_csFileName+=_T("txt");
        m_csPath+=_T("txt");
			
		maxForOneCep=minForOneCep=dataout[0];
		//CString filename=(_T("data\\lpc\\")+m_csFileName);
		ofstream fout( m_csPath, ios::trunc );
		double sum=0.0;
		pOutNum=sample_rate;
		for (i = 0; i <nCount; i++)
		{	
			for(j=0;j<this->m_nFrameSize;j++)
			{	fout <<dataout[i*this->m_nFrameSize+j]<< endl; 
			    dataout[i*this->m_nFrameSize+j]=dataout[i*this->m_nFrameSize+j]*10000;
			
			if(maxForOneCep<dataout[i*this->m_nFrameSize+j])
				maxForOneCep=dataout[i*this->m_nFrameSize+j];
			if(minForOneCep>dataout[i*this->m_nFrameSize+j])
				minForOneCep=dataout[i*this->m_nFrameSize+j];
			}
				
		}		
		fout.close();

	

}

//////////////////////////////////////////////////用Lpc函数合成语音数据
void CMyWaveView::OnLpctodata()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	 unsigned int num=pDoc->data_one.size ();
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 int i;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout != NULL)
		delete[] dataout;
		
	data=new double[sample_rate];
    
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
	} 
	for (i=num;i<sample_rate;i++)
		data[i]=0;
	
	dataout=new double[sample_rate];
	// 对输入数据进行加窗处理
		CSpeech::AddWindow(this->m_nFrameSize, data,  sample_rate);
	// 利用LPC合成的语音数据
		CSpeech::LPCToDATA(
					this->m_nFrameSize, data, sample_rate, dataout);

	//保存合成的语音数据
	   CFileDialog dlg(false, NULL, L"", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"LPC合成数据（*.data）|*.data|所有文件(*.*)|*.*||");

if(dlg.DoModal()==IDOK)
{
	CFileException fileException;
	//获取路径
	CString m_csFileName=dlg.GetPathName();
	CString FilePath=dlg.GetPathName ();
	FilePath+=_T(".data");
 	m_csFileName+=_T(".data");
  
	ofstream fout( m_csFileName, ios::app );
	

    for (unsigned int i = 0; i <sample_rate; i++)
	{	
		 
			fout <<i<<"  "<<dataout[i]<< endl; 
	}
    //关闭输出流 
    fout.close();
}
m_lpctodata!=m_lpctodata;
Invalidate();
}

/////////////////////////////////////////Mel预测系数
void CMyWaveView::OnMfcc()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
if(pDoc->data_one.size ()!=0)
{
	 unsigned int num=pDoc->data_one.size ();	
     int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
     unsigned int sample_rate=nCount*this->m_nFrameSize;
	 unsigned filterNum=24,cepsNum=13;
	 int i;
	 
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (pDoc->mfcc != NULL)
		delete[] pDoc->mfcc;

	data=new double[num];

	 /*CString m_csFileName(pDoc->m_strFileName,pDoc->m_strFileName.GetLength()-3);
	 m_csFileName+=_T("txt");*/
	
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
	} 

	pDoc->mfcc=new double[cepsNum*nCount];//13*nCount

     // 获取MfCC
		CSpeech::GetMFCC(
					this->m_nFrameSize, data, num,pDoc->m_HZ,filterNum,cepsNum,pDoc->mfcc);
	//MFCC数据
		/**********************修改的代码**************************/
		mfccNum1=cepsNum*nCount;
		maxForOneMfcc=minForOneMfcc=pDoc->mfcc[0];
		for(unsigned int i=0;i<mfccNum1;i++)
		{
			if (maxForOneMfcc<pDoc->mfcc[i])
			{
				maxForOneMfcc=pDoc->mfcc[i];
			}
			if (minForOneMfcc>pDoc->mfcc[i])
			{
				minForOneMfcc=pDoc->mfcc[i];
			}
			//pDoc->mfcc[i]*=100;//把倍增留到绘制垂直mel倒谱函数中
		}
		/*double magnify=4000/(maxForOneMfcc-minForOneMfcc);
		for(unsigned int j=0;i<mfccNum1;i++)
		{
			pDoc->mfcc[j]=(int)((pDoc->mfcc[j]-minForOneMfcc)*magnify);
		}*/
		/**********************************************************/
	//CString FilePath(pDoc->m_strFilePath,pDoc->m_strFilePath.GetLength ()-3);
 //   FilePath+=_T("txt");

	//ofstream fout( FilePath, ios::trunc );
	//for(unsigned int i=0;i<nCount;i++)
	//{
	//	for (unsigned int j = 0; j <cepsNum; j++)
	//	{	
	//		fout <<pDoc->mfcc[i*cepsNum+j]<< "  "; 
	//	}
	//	fout<<endl;
	//}

 //   //关闭输出流
 //   fout.close();
}

}

void CMyWaveView::OnDrawypt()//控制语谱图是否显示
{
	// TODO: 在此添加命令处理程序代码
    m_showypt=!m_showypt;
	Invalidate();
}

void CMyWaveView::OnUpdateDrawypt(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	 pCmdUI->SetCheck (m_showypt);
	 
}
//画语谱图
struct poly_node
{
	CPoint *pt;			//指向多边形顶点数组
	unsigned int num;	//顶点数
	struct poly_node * next;	//指向下一个多边形结点
};
typedef struct poly_node pNode;
inline pNode* polygon_Judement(pNode* p,CPoint pre,CPoint next,int x_amplitudeEdge,int y_step)
{//改变方向
	/***************************判断图形属于哪一类*************************/
	if(pre.x>=x_amplitudeEdge&&next.x<x_amplitudeEdge)//三角形--next在左，pre在右。
	{	
		pNode* ptemp=(pNode*)malloc(sizeof(pNode));//申请多边形结点
		if (ptemp==NULL)
		{
			exit(OVERFLOW);
		}
		CPoint* pt_temp=new CPoint[3];		//申请3个顶点
		if (pt_temp==NULL)
		{
			exit(OVERFLOW);
		}
		ptemp->next=NULL;
		ptemp->num=3;		
		ptemp->pt=pt_temp;
		if (pre.x!=x_amplitudeEdge)			//next不在边界上
		{
			pt_temp[0].x=pre.x;
			pt_temp[0].y=pre.y;
			pt_temp[1].x=x_amplitudeEdge;
			pt_temp[1].y=pre.y;
			pt_temp[2].x=x_amplitudeEdge;
			pt_temp[2].y=pre.y-(int)(y_step*(pre.x-x_amplitudeEdge)/(pre.x-next.x));			
		} 
		else						//next在边界上
		{
			pt_temp[0].x=pre.x;
			pt_temp[0].y=pre.y;
			pt_temp[1].x=next.x;
			pt_temp[1].y=next.y;
			pt_temp[2].x=x_amplitudeEdge;
			pt_temp[2].y=pre.y;
		}
		p->next=ptemp;
		p=ptemp;
		p->next=NULL;

	}
	else if (pre.x<x_amplitudeEdge&&next.x>=x_amplitudeEdge)//三角形--pre在左，next在右。
	{
		pNode* ptemp=(pNode*)malloc(sizeof(pNode));//申请多边形结点
		if (ptemp==NULL)
		{
			exit(OVERFLOW);
		}
		CPoint* pt_temp=new CPoint[3];		//申请3个顶点
		if (pt_temp==NULL)
		{
			exit(OVERFLOW);
		}
		ptemp->next=NULL;
		ptemp->num=3;		
		ptemp->pt=pt_temp;
		if (pre.x!=x_amplitudeEdge)			//pre不在边界上
		{
			pt_temp[0].x=x_amplitudeEdge;
			pt_temp[0].y=next.y+(int)(y_step*(next.x-x_amplitudeEdge)/(next.x-pre.x));
			pt_temp[1].x=x_amplitudeEdge;
			pt_temp[1].y=next.y;
			pt_temp[2].x=next.x;
			pt_temp[2].y=next.y;			
		} 
		else						//pre在边界上
		{
			pt_temp[0].x=x_amplitudeEdge;
			pt_temp[0].y=next.y;
			pt_temp[1].x=pre.x;
			pt_temp[1].y=pre.y;
			pt_temp[2].x=next.x;
			pt_temp[2].y=next.y;
		}
		p->next=ptemp;
		p=ptemp;
		p->next=NULL;
	} 
	else//四边形--pre,next均位于右侧
	{
		if (pre.x>x_amplitudeEdge&&next.x>x_amplitudeEdge)
		{
			pNode* ptemp=(pNode*)malloc(sizeof(pNode));//申请多边形结点
			if (ptemp==NULL)
			{
				exit(OVERFLOW);
			}
			CPoint* pt_temp=new CPoint[4];		//申请4个顶点
			if (pt_temp==NULL)
			{
				exit(OVERFLOW);
			}
			ptemp->next=NULL;
			ptemp->num=4;		
			ptemp->pt=pt_temp;

			pt_temp[0].x=pre.x;
			pt_temp[0].y=pre.y;
			pt_temp[1].x=x_amplitudeEdge;
			pt_temp[1].y=pre.y;
			pt_temp[2].x=next.x;
			pt_temp[2].y=next.y;
			pt_temp[3].x=x_amplitudeEdge;
			pt_temp[3].y=next.y;

			p->next=ptemp;
			p=ptemp;
			p->next=NULL;
		}
		//均位于边界的左侧不处理
	}
	return p;
}

void  CMyWaveView::Drawypt(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos)
{
	const int y_base=30000;								//曲线绘制起点纵坐标。
	if (((m_bottom-m_top)/m_Yzoom+m_Ypos-m_extent>y_base)&&(sample_rate/2>=m_Xpos))//屏幕顶端坐标必须大于曲线绘制起点
	{	
		CPoint pre,next;
		int x_start,x_end;								//x_start表示屏幕内绘制的第一个帧的下标
		int ncount=sample_rate/this->m_nFrameSize;
		int y_step=ceil(120*m_Yzoom);					//相邻数据点在垂直方向上的实际间隔
		int x_base;										//“第一帧”中心x坐标

		/***************************创建多边形链表，并且初始化************************/
		int x_amplitudeEdge;							//需填充的振幅界限
		pNode* plist=(pNode*)malloc(sizeof(pNode));		//创建多边形链表
		if (plist==NULL)
		{
			exit(OVERFLOW);
		}
		plist->next=NULL;
		pNode* p=(pNode*)malloc(sizeof(pNode));			//指向链表最后一个结点
		if (p==NULL)
		{
			exit(OVERFLOW);
		}
		p=plist;
		p->next=NULL;
		/*****************************************************************************/


		/******************************确定振幅绘制放大比例***************************/
		double max;	//存放振幅的最大、最小值
		double magnify;	//放大比例
		max=data[0];
		for (int a=0;a<sample_rate;a++)
		{
			if (max<data[a])
			{
				max=data[a];
			}
		}
		magnify=1.5*m_nFrameSize/max;
		/*****************************************************************************/


		/*******************************确定绘制范围**********************************/
		//前边界
		int times=m_Xpos/(this->m_nFrameSize/2);
		if (m_Xpos%(this->m_nFrameSize/2)==0&&m_Xpos!=0)
		{
			x_base=m_Xpos;
			x_start=times-1;
		}
		else
		{
			x_base=(times+1)*(this->m_nFrameSize/2);
			x_start=times;
		}
		//后边界
		times=(m_Xpos+(int)((m_right-m_left)/m_Xzoom))/(this->m_nFrameSize/2);
		if((int)((m_Xpos+(int)((m_right-m_left)/m_Xzoom)))%(this->m_nFrameSize/2)==0)
		{
			x_end=times-1;
		}
		else
		{
			x_end=times;
		}
		//防止越界
		if (x_end>ncount-1)
		{
			x_end=ncount-1;
		}
		/******************************************************************************/

		/******************开始绘制曲线(频率的幅度正方向暂时取x轴的反方向)************/
		CPen pen;
		pen.CreatePen (PS_SOLID,1,crColor);
		CPen* pOldPen=pDC->SelectObject (&pen);
		//先绘第一帧的频谱，减少不必要的判断
		CPoint base;//存放x_base,y_base基准屏幕坐标
		base.x=(int)(m_Xzoom*x_base)-(int)(m_Xzoom*m_Xpos)+(int)(m_Xzoom*XDispos)+m_left;
		base.y=m_bottom-(int)((y_base-m_Ypos+m_extent-YDispos)*m_Yzoom);
		pre.x=base.x;
		pre.y=base.y;
		pDC->MoveTo(pre.x,pre.y);
		x_amplitudeEdge=base.x+(int)(5.0*magnify*m_Xzoom);	//振幅界限的实际坐标值(6*magnify)
		if (x_amplitudeEdge<m_left)
		{
			x_amplitudeEdge=m_left;
		}
		for(int k=0;k<=this->m_nFrameSize/2;k++)
		{
			next.x=base.x+(int)(m_Xzoom*magnify*data[x_start*this->m_nFrameSize+k]);
			next.y=pre.y-y_step;
			if (next.x<m_left)
			{
				next.x=m_left;
			}
			if (next.x>m_right)
			{
				next.x=m_right;
			}
			pDC->LineTo(next);
			//判断图形属于哪一类，并将多边形结点加入链表。
			p=polygon_Judement(p,pre,next,x_amplitudeEdge,y_step);
			pre.x=next.x;
			pre.y=next.y;
			pDC->MoveTo(pre);
		}
		
		//绘制其他帧的频谱
		for (int i=x_start+1;i<=x_end;i++)
		{
			base.x=(int)((x_base+(i-x_start)*(m_nFrameSize/2)-m_Xpos+XDispos)*m_Xzoom)+m_left;//第i帧中心处x坐标
			x_amplitudeEdge=base.x+(int)(5.0*magnify*m_Xzoom);//第i帧的振幅边界
			if (x_amplitudeEdge<m_left)
			{
				x_amplitudeEdge=m_left;
			}
			pre.x=base.x;
			pre.y=base.y;
			if (next.x<m_left)
			{
				next.x=m_left;
			}
			if (next.x>m_right)
			{
				next.x=m_right;
			}
			pDC->MoveTo(pre);
			for (int j=0;j<=this->m_nFrameSize/2;j++)
			{
				next.x=base.x+(int)(m_Xzoom*magnify*data[i*this->m_nFrameSize+j]);
				next.y=pre.y-y_step;
				pDC->LineTo(next);
				//判断图形属于哪一类，并将多边形结点加入链表。
				p=polygon_Judement(p,pre,next,x_amplitudeEdge,y_step);
				pre.x=next.x;
				pre.y=next.y;
				pDC->MoveTo(pre);
			}
		}
		/*****************************************************************************/
		pDC->SelectObject (pOldPen);
		if(pen.m_hObject )
			pen.DeleteObject ();
		/*********************************进行填充************************************/
		COLORREF colorrrefRGB=RGB(0,0,0);
	
		HBRUSH hBrush=CreateSolidBrush(colorrrefRGB);
		HGDIOBJ pOldBrush=pDC->SelectObject(hBrush);
		p=plist;
		while (p->next)
		{
			p=p->next;
			pDC->Polygon(p->pt,p->num);
		}
		pDC->SelectObject(pOldBrush);
		DeleteObject(hBrush);
		/****************************************************************************/

		/****************************释放申请的内存**********************************/
		pNode* q=NULL;
		p=plist;
		while (p->next)
		{
			q=p->next;
			p->next=p->next->next;
			delete [](q->pt);//先释放结点内的数组，再释放结点。
			free(q);
		}
		/******************************************************************/
	} 
}

void  CMyWaveView::Drawypt_Thickset(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos)
{
	const int y_base=10000;								//曲线绘制起点纵坐标。
	if (((m_bottom-m_top)/m_Yzoom+m_Ypos-m_extent>y_base)&&(sample_rate/2>=m_Xpos))//屏幕顶端坐标必须大于曲线绘制起点
	{	
		CPoint pre,next;
		int x_start,x_end;								//x_start表示屏幕内绘制的第一个帧的下标
		int ncount=sample_rate/this->m_nFrameSize;
		int y_step=ceil(150*m_Yzoom);					//相邻数据点在垂直方向上的实际间隔
		int x_base;										//“第一帧”中心x坐标

		/***************************创建多边形链表，并且初始化************************/
		int x_amplitudeEdge;							//需填充的振幅界限
		pNode* plist=(pNode*)malloc(sizeof(pNode));		//创建多边形链表
		if (plist==NULL)
		{
			exit(OVERFLOW);
		}
		plist->next=NULL;
		pNode* p=(pNode*)malloc(sizeof(pNode));			//指向链表最后一个结点
		if (p==NULL)
		{
			exit(OVERFLOW);
		}
		p=plist;
		p->next=NULL;
		/*****************************************************************************/


		/******************************确定振幅绘制放大比例***************************/
		double max;	//存放振幅的最大、最小值
		double magnify;	//放大比例
		max=data[0];
		for (int a=0;a<sample_rate;a++)
		{
			if (max<data[a])
			{
				max=data[a];
			}
		}
		magnify=1.5*m_nFrameSize/max;
		/*****************************************************************************/


		/*******************************确定绘制范围**********************************/
		//前边界
		int times=m_Xpos/(this->m_nFrameSize/8);
		if (m_Xpos%(this->m_nFrameSize/8)==0&&m_Xpos!=0&&m_Xpos!=64&&m_Xpos!=32)
		{
			x_base=m_Xpos;
			x_start=times-1;
		}
		else
		{
			x_base=(times+1)*(this->m_nFrameSize/8);
			x_start=times;
		}
		//后边界
		times=(m_Xpos+(int)((m_right-m_left)/m_Xzoom))/(this->m_nFrameSize/8);
		if((int)((m_Xpos+(int)((m_right-m_left)/m_Xzoom)))%(this->m_nFrameSize/8)==0)
		{
			x_end=times-1;
		}
		else
		{
			x_end=times;
		}
		//防止越界
		if (x_end>ncount-1)
		{
			x_end=ncount-1;
		}
		/******************************************************************************/

		/******************开始绘制曲线(频率的幅度正方向暂时取x轴的反方向)************/
		CPen pen;
		pen.CreatePen (PS_SOLID,1,crColor);
		CPen* pOldPen=pDC->SelectObject (&pen);

		//先绘第一帧的频谱，减少不必要的判断
		CPoint base;//存放x_base,y_base基准屏幕坐标
		base.x=(int)(m_Xzoom*x_base)-(int)(m_Xzoom*m_Xpos)+(int)(m_Xzoom*XDispos)+m_left;
		base.y=m_bottom-(int)((y_base-m_Ypos+m_extent-YDispos)*m_Yzoom);
		pre.x=base.x;
		pre.y=base.y;
		pDC->MoveTo(pre.x,pre.y);
		x_amplitudeEdge=base.x+(int)(5*magnify*m_Xzoom);	//振幅界限的实际坐标值(6*magnify)
		if (x_amplitudeEdge<m_left)
		{
			x_amplitudeEdge=m_left;
		}
		for(int k=0;k<=this->m_nFrameSize/2;k++)
		{
			next.x=base.x+(int)(m_Xzoom*magnify*data[x_start*this->m_nFrameSize+k]);
			next.y=pre.y-y_step;
			if (next.x<m_left)
			{
				next.x=m_left;
			}
			if (next.x>m_right)
			{
				next.x=m_right;
			}
			pDC->LineTo(next);
			//判断图形属于哪一类，并将多边形结点加入链表。
			p=polygon_Judement(p,pre,next,x_amplitudeEdge,y_step);
			pre.x=next.x;
			pre.y=next.y;
			pDC->MoveTo(pre);
		}

		//绘制其他帧的频谱
		for (int i=x_start+1;i<=x_end;i++)
		{
			//base.x=(int)((x_base+(i-x_start)*(m_nFrameSize/2)-m_Xpos+XDispos)*m_Xzoom)+m_left;//第i帧中心处x坐标
			base.x=(int)((x_base+(i-x_start)*(m_nFrameSize/8)-m_Xpos+XDispos)*m_Xzoom)+m_left;//第i帧中心处x坐标
			x_amplitudeEdge=base.x+(int)(5*magnify*m_Xzoom);//第i帧的振幅边界
			if (x_amplitudeEdge<m_left)
			{
				x_amplitudeEdge=m_left;
			}
			pre.x=base.x;
			pre.y=base.y;
			if (next.x<m_left)
			{
				next.x=m_left;
			}
			if (next.x>m_right)
			{
				next.x=m_right;
			}
			pDC->MoveTo(pre);
			for (int j=0;j<=this->m_nFrameSize/2;j++)
			{
				next.x=base.x+(int)(m_Xzoom*magnify*data[i*this->m_nFrameSize+j]);
				next.y=pre.y-y_step;
				pDC->LineTo(next);
				//判断图形属于哪一类，并将多边形结点加入链表。
				p=polygon_Judement(p,pre,next,x_amplitudeEdge,y_step);
				pre.x=next.x;
				pre.y=next.y;
				pDC->MoveTo(pre);
			}
		}
		/*****************************************************************************/
		pDC->SelectObject (pOldPen);
		if(pen.m_hObject )
			pen.DeleteObject ();
		/*********************************进行填充************************************/
		COLORREF colorrrefRGB=RGB(0,0,0);

		HBRUSH hBrush=CreateSolidBrush(colorrrefRGB);
		HGDIOBJ pOldBrush=pDC->SelectObject(hBrush);
		p=plist;
		while (p->next)
		{
			p=p->next;
			pDC->Polygon(p->pt,p->num);
		}
		pDC->SelectObject(pOldBrush);
		DeleteObject(hBrush);
		/****************************************************************************/

		/****************************释放申请的内存**********************************/
		pNode* q=NULL;
		p=plist;
		while (p->next)
		{
			q=p->next;
			p->next=p->next->next;
			delete [](q->pt);//先释放结点内的数组，再释放结点。
			free(q);
		}
		/******************************************************************/
	} 
}

void CMyWaveView::DrawSpecYPT(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos)
{
	const int y_base=3000;								//曲线绘制起点纵坐标。
	if (((m_bottom-m_top)/m_Yzoom+m_Ypos-m_extent>y_base)&&(sample_rate/2>=m_Xpos))//屏幕顶端坐标必须大于曲线绘制起点
	{	
		CPoint pre,next;
		int x_start,x_end;								//x_start表示屏幕内绘制的第一个帧的下标
		int ncount=sample_rate/this->m_nFrameSize;
		int y_step=ceil(110*m_Yzoom);					//相邻数据点在垂直方向上的实际间隔
		int x_base;										//“第一帧”中心x坐标

		/***************************创建多边形链表，并且初始化************************/
		int x_amplitudeEdge;							//需填充的振幅界限
		pNode* plist=(pNode*)malloc(sizeof(pNode));		//创建多边形链表
		if (plist==NULL)
		{
			exit(OVERFLOW);
		}
		plist->next=NULL;
		pNode* p=(pNode*)malloc(sizeof(pNode));			//指向链表最后一个结点
		if (p==NULL)
		{
			exit(OVERFLOW);
		}
		p=plist;
		p->next=NULL;
		/*****************************************************************************/


		/******************************确定振幅绘制放大比例***************************/
		double max,min;	//存放振幅的最大、最小值
		double magnify;	//放大比例
		max=min=data[0];
		for (int a=0;a<sample_rate;a++)
		{
			if (max<data[a])
			{
				max=data[a];
			}
			if (min>data[a])
			{
				min=data[a];
			}
		}
		magnify=(double)(2.0*m_nFrameSize)/(max-min);
		/*****************************************************************************/


		/*******************************确定绘制范围**********************************/
		//前边界
		int times=m_Xpos/(this->m_nFrameSize/2);
		if (m_Xpos%(this->m_nFrameSize/2)==0&&m_Xpos!=0)
		{
			x_base=m_Xpos;
			x_start=times-1;
		}
		else
		{
			x_base=(times+1)*(this->m_nFrameSize/2);
			x_start=times;
		}
		//后边界
		times=(m_Xpos+(int)((m_right-m_left)/m_Xzoom))/(this->m_nFrameSize/2);
		if((int)((m_Xpos+(int)((m_right-m_left)/m_Xzoom)))%(this->m_nFrameSize/2)==0)
		{
			x_end=times-1;
		}
		else
		{
			x_end=times;
		}
		//防止越界
		if (x_end>ncount-1)
		{
			x_end=ncount-1;
		}
		/******************************************************************************/
		CPen pen;
		pen.CreatePen (PS_SOLID,1,crColor);
		CPen* pOldPen=pDC->SelectObject (&pen);
		/******************开始绘制曲线(频率的幅度正方向暂时取x轴的反方向)************/

		//先绘第一帧的频谱，减少不必要的判断
		CPoint base;//存放x_base,y_base基准屏幕坐标
		base.x=(int)(m_Xzoom*x_base)-(int)(m_Xzoom*m_Xpos)+(int)(m_Xzoom*XDispos)+m_left;
		base.y=m_bottom-(int)((y_base-m_Ypos+m_extent-YDispos)*m_Yzoom);
		pre.x=base.x;
		pre.y=base.y;
		pDC->MoveTo(pre.x,pre.y);
		x_amplitudeEdge=base.x+(int)(150*magnify*m_Xzoom);	//振幅界限的实际坐标值(400*magnify)
		if (x_amplitudeEdge<m_left)
		{
			x_amplitudeEdge=m_left;
		}
		for(int k=0;k<=this->m_nFrameSize/2;k++)
		{
			next.x=base.x+(int)(m_Xzoom*magnify*(data[x_start*this->m_nFrameSize+k]-min));//归一化
			next.y=pre.y-y_step;
			if (next.x<m_left)
			{
				next.x=m_left;
			}
			if (next.x>m_right)
			{
				next.x=m_right;
			}
			pDC->LineTo(next);
			//判断图形属于哪一类，并将多边形结点加入链表。
			p=polygon_Judement(p,pre,next,x_amplitudeEdge,y_step);
			pre.x=next.x;
			pre.y=next.y;
			pDC->MoveTo(pre);
		}

		//绘制其他帧的频谱
		for (int i=x_start+1;i<=x_end;i++)
		{
			base.x=(int)((x_base+(i-x_start)*(m_nFrameSize/2)-m_Xpos+XDispos)*m_Xzoom)+m_left;//第i帧中心处x坐标
			x_amplitudeEdge=base.x+(int)(150*magnify*m_Xzoom);//第i帧的振幅边界
			if (x_amplitudeEdge<m_left)
			{
				x_amplitudeEdge=m_left;
			}
			pre.x=base.x;
			pre.y=base.y;
			if (next.x<m_left)
			{
				next.x=m_left;
			}
			if (next.x>m_right)
			{
				next.x=m_right;
			}
			pDC->MoveTo(pre);
			for (int j=0;j<=this->m_nFrameSize/2;j++)
			{
				next.x=base.x+(int)(m_Xzoom*magnify*(data[i*this->m_nFrameSize+j]-min));
				next.y=pre.y-y_step;
				pDC->LineTo(next);
				//判断图形属于哪一类，并将多边形结点加入链表。
				p=polygon_Judement(p,pre,next,x_amplitudeEdge,y_step);
				pre.x=next.x;
				pre.y=next.y;
				pDC->MoveTo(pre);
			}
		}
		/*****************************************************************************/
		pDC->SelectObject (pOldPen);
		if(pen.m_hObject )
			pen.DeleteObject ();
		/*********************************进行填充************************************/
		COLORREF colorrrefRGB=RGB(0,0,0);

		HBRUSH hBrush=CreateSolidBrush(colorrrefRGB);
		HGDIOBJ pOldBrush=pDC->SelectObject(hBrush);
		p=plist;
		while (p->next)
		{
			p=p->next;
			pDC->Polygon(p->pt,p->num);
		}
		pDC->SelectObject(pOldBrush);
		DeleteObject(hBrush);
		/****************************************************************************/

		/****************************释放申请的内存**********************************/
		pNode* q=NULL;
		p=plist;
		while (p->next)
		{
			q=p->next;
			p->next=p->next->next;
			delete [](q->pt);//先释放结点内的数组，再释放结点。
			free(q);
		}
		/******************************************************************/
	} 
}

void CMyWaveView::DrawSpecYPT_Thickset(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos)
{
	const int y_base=3000;								//曲线绘制起点纵坐标。
	if (((m_bottom-m_top)/m_Yzoom+m_Ypos-m_extent>y_base)&&(sample_rate/2>=m_Xpos))//屏幕顶端坐标必须大于曲线绘制起点
	{	
		CPoint pre,next;
		int x_start,x_end;								//x_start表示屏幕内绘制的第一个帧的下标
		int ncount=sample_rate/this->m_nFrameSize;
		int y_step=ceil(150*m_Yzoom);					//相邻数据点在垂直方向上的实际间隔
		int x_base;										//“第一帧”中心x坐标

		/***************************创建多边形链表，并且初始化************************/
		int x_amplitudeEdge;							//需填充的振幅界限
		pNode* plist=(pNode*)malloc(sizeof(pNode));		//创建多边形链表
		if (plist==NULL)
		{
			exit(OVERFLOW);
		}
		plist->next=NULL;
		pNode* p=(pNode*)malloc(sizeof(pNode));			//指向链表最后一个结点
		if (p==NULL)
		{
			exit(OVERFLOW);
		}
		p=plist;
		p->next=NULL;
		/*****************************************************************************/


		/******************************确定振幅绘制放大比例***************************/
		double max,min;	//存放振幅的最大、最小值
		double magnify;	//放大比例
		max=min=data[0];
		for (int a=0;a<sample_rate;a++)
		{
			if (max<data[a])
			{
				max=data[a];
			}
			if (min>data[a])
			{
				min=data[a];
			}
		}
		magnify=(double)(2.0*m_nFrameSize)/(max-min);
		/*****************************************************************************/


		/*******************************确定绘制范围**********************************/
		//前边界
		int times=m_Xpos/(this->m_nFrameSize/8);
		if (m_Xpos%(this->m_nFrameSize/8)==0&&m_Xpos!=0&&m_Xpos!=64&&m_Xpos!=32)
		{
			x_base=m_Xpos;
			x_start=times-1;
		}
		else
		{
			x_base=(times+1)*(this->m_nFrameSize/8);
			x_start=times;
		}
		//后边界
		times=(m_Xpos+(int)((m_right-m_left)/m_Xzoom))/(this->m_nFrameSize/8);
		if((int)((m_Xpos+(int)((m_right-m_left)/m_Xzoom)))%(this->m_nFrameSize/8)==0)
		{
			x_end=times-1;
		}
		else
		{
			x_end=times;
		}
		//防止越界
		if (x_end>ncount-1)
		{
			x_end=ncount-1;
		}
		/******************************************************************************/
		CPen pen;
		pen.CreatePen (PS_SOLID,1,crColor);
		CPen* pOldPen=pDC->SelectObject (&pen);
		/******************开始绘制曲线(频率的幅度正方向暂时取x轴的反方向)************/

		//先绘第一帧的频谱，减少不必要的判断
		CPoint base;//存放x_base,y_base基准屏幕坐标
		base.x=(int)(m_Xzoom*x_base)-(int)(m_Xzoom*m_Xpos)+(int)(m_Xzoom*XDispos)+m_left;
		base.y=m_bottom-(int)((y_base-m_Ypos+m_extent-YDispos)*m_Yzoom);
		pre.x=base.x;
		pre.y=base.y;
		pDC->MoveTo(pre.x,pre.y);
		x_amplitudeEdge=base.x+(int)(150*magnify*m_Xzoom);	//振幅界限的实际坐标值(400*magnify)
		if (x_amplitudeEdge<m_left)
		{
			x_amplitudeEdge=m_left;
		}
		for(int k=0;k<=this->m_nFrameSize/2;k++)
		{
			next.x=base.x+(int)(m_Xzoom*magnify*(data[x_start*this->m_nFrameSize+k]-min));//归一化
			next.y=pre.y-y_step;
			if (next.x<m_left)
			{
				next.x=m_left;
			}
			if (next.x>m_right)
			{
				next.x=m_right;
			}
			pDC->LineTo(next);
			//判断图形属于哪一类，并将多边形结点加入链表。
			p=polygon_Judement(p,pre,next,x_amplitudeEdge,y_step);
			pre.x=next.x;
			pre.y=next.y;
			pDC->MoveTo(pre);
		}

		//绘制其他帧的频谱
		for (int i=x_start+1;i<=x_end;i++)
		{
			base.x=(int)((x_base+(i-x_start)*(m_nFrameSize/8)-m_Xpos+XDispos)*m_Xzoom)+m_left;//第i帧中心处x坐标
			x_amplitudeEdge=base.x+(int)(150*magnify*m_Xzoom);//第i帧的振幅边界
			if (x_amplitudeEdge<m_left)
			{
				x_amplitudeEdge=m_left;
			}
			pre.x=base.x;
			pre.y=base.y;
			if (next.x<m_left)
			{
				next.x=m_left;
			}
			if (next.x>m_right)
			{
				next.x=m_right;
			}
			pDC->MoveTo(pre);
			for (int j=0;j<=this->m_nFrameSize/2;j++)
			{
				next.x=base.x+(int)(m_Xzoom*magnify*(data[i*this->m_nFrameSize+j]-min));
				next.y=pre.y-y_step;
				pDC->LineTo(next);
				//判断图形属于哪一类，并将多边形结点加入链表。
				p=polygon_Judement(p,pre,next,x_amplitudeEdge,y_step);
				pre.x=next.x;
				pre.y=next.y;
				pDC->MoveTo(pre);
			}
		}
		/*****************************************************************************/
		pDC->SelectObject (pOldPen);
		if(pen.m_hObject )
			pen.DeleteObject ();
		/*********************************进行填充************************************/
		COLORREF colorrrefRGB=RGB(0,0,0);

		HBRUSH hBrush=CreateSolidBrush(colorrrefRGB);
		HGDIOBJ pOldBrush=pDC->SelectObject(hBrush);
		p=plist;
		while (p->next)
		{
			p=p->next;
			pDC->Polygon(p->pt,p->num);
		}
		pDC->SelectObject(pOldBrush);
		DeleteObject(hBrush);
		/****************************************************************************/

		/****************************释放申请的内存**********************************/
		pNode* q=NULL;
		p=plist;
		while (p->next)
		{
			q=p->next;
			p->next=p->next->next;
			delete [](q->pt);//先释放结点内的数组，再释放结点。
			free(q);
		}
		/******************************************************************/
	} 
}

void CMyWaveView::DrawSpecFFT(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName,double SpecfftYZoom)//绘制频谱的fft
{
	//绘制曲线基准刻度
	int y=(int)(m_bottom-(32800-k2-m_Ypos-YDispos)*m_Yzoom);
	pDC->MoveTo (m_left,y);
	pDC->LineTo (m_left+60,y);

	pDC->SetTextColor(RGB(0,0,0));
	pDC->TextOut(m_left+5,y-18,WaveName);

	y=(int)(m_bottom-(32800-k1-m_Ypos-YDispos)*m_Yzoom);
	pDC->MoveTo (m_left,y);
	pDC->LineTo (m_left+60,y);

	int k=k2;

	CPen pen;
	pen.CreatePen(PS_SOLID,1,crColor);
	CPen* pOldPen=pDC->SelectObject(&pen);
	CPen pen1;
	pen1.CreatePen(PS_SOLID,1,RGB(0,128,128));

	int start=(int)(Xpos-(double)XDispos);//i为开始绘制时的压力点的坐标！！
	if( start<0)
		start=0;
	int i=(start/this->m_nFrameSize/2),j=(start%(this->m_nFrameSize));			
	int xPos=0;
	int xPos1=0;
	for(;i<(int)sample_rate/(this->m_nFrameSize)&&xPos<=m_right && xPos1<=m_right;i++)
	{
		xPos=((int)j*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;
		xPos1=((int)(j+1)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;
		if(i%2==1)		
		{
			k=k1;
			pDC->SelectObject(&pen1);
		}
		if(i%2==0)		
		{
			k=k2;
			pDC->SelectObject(&pen);
		}

		for(;j+1<((this->m_nFrameSize)/2+1)&&xPos<=m_right && xPos1<=m_right;)
		{	

			if(data[i*(this->m_nFrameSize)+j]*SpecfftYZoom+extent-(int)(YDispos)-k>=Ypos&&data[i*(this->m_nFrameSize)+j]*SpecfftYZoom+extent-(int)YDispos-k<=Ypos+(int)
				(m_bottom-m_top)/Yzoom)
			{  
				CPoint start(xPos,m_bottom-(int)((data[i*(this->m_nFrameSize)+j]/3*SpecfftYZoom+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
				if(data[i*(this->m_nFrameSize)+j+1]*SpecfftYZoom+extent-(int)(YDispos)-k>=Ypos&&data[i*(this->m_nFrameSize)+j+1]*SpecfftYZoom+extent-(int)YDispos-k<=Ypos+(int)(m_bottom-m_top)/Yzoom)
				{
					CPoint end(xPos1,m_bottom-(int)((data[i*(this->m_nFrameSize)+j+1]/3*SpecfftYZoom+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
					if(xPos>=m_left&&xPos1>=m_left)
					{

						pDC->MoveTo (start);
						pDC->LineTo (end);

					}
				}
			}
			j++;
			xPos=((int)j*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;
			xPos1=((int)(j+1)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;	
		}
		j=0;
	}

	pDC->SelectObject (pOldPen);
	if(pen.m_hObject )
		pen.DeleteObject ();
}

void  CMyWaveView::DrawLogSpecHorizon(CDC* pDC,double *data,unsigned int sample_rate,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName)
{
	//绘制曲线基准刻度
	int y=(int)(m_bottom-(32800-k2-m_Ypos-YDispos)*m_Yzoom);
	pDC->MoveTo (m_left,y);
	pDC->LineTo (m_left+60,y);
	
	pDC->SetTextColor(RGB(0,0,0));
	pDC->TextOut(m_left+5,y-18,WaveName);

	y=(int)(m_bottom-(32800-k1-m_Ypos-YDispos)*m_Yzoom);
	pDC->MoveTo (m_left,y);
	pDC->LineTo (m_left+60,y);

	int k=k2;

	CPen pen;
	pen.CreatePen(PS_SOLID,1,crColor);
	CPen* pOldPen=pDC->SelectObject(&pen);
	CPen pen1;
	pen1.CreatePen(PS_SOLID,1,RGB(255,0,0));

	int start=(int)(Xpos-(double)XDispos);//i为开始绘制时的压力点的坐标！！
	if( start<0)
		start=0;
	int i=(start/this->m_nFrameSize/2),j=(start%(this->m_nFrameSize));			
	int xPos=0;
	int xPos1=0;
	for(;i<(int)sample_rate/(this->m_nFrameSize)&&xPos<=m_right && xPos1<=m_right;i++)
	{
		xPos=((int)j*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;
		xPos1=((int)(j+1)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;
		if(i>0)
		{	
			if(i%2==1)		
			{
				k=k1;	   
			}
			if(i%2==0)		
			{
				k=k2;
			}
			if(i%8==0||i%8==3||i%8==4||i%8==7)			
				pDC->SelectObject(&pen);
			if(i%8==1||i%8==1||i%8==5||i%8==6)
				pDC->SelectObject(&pen1);
		}


		for(;j+1<((this->m_nFrameSize)/2+1)&&xPos<=m_right && xPos1<=m_right;)
		{	

			if(data[i*(this->m_nFrameSize)+j]+extent-(int)(YDispos)-k>=Ypos&&data[i*(this->m_nFrameSize)+j]+extent-(int)YDispos-k<=Ypos+(int)(m_bottom-m_top)/Yzoom)
			{  

				CPoint start(xPos,m_bottom-(int)(((data[i*(this->m_nFrameSize)+j])+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
				if(data[i*(this->m_nFrameSize)+j+1]+extent-(int)(YDispos)-k>=Ypos&&data[i*(this->m_nFrameSize)+j+1]+extent-(int)YDispos-k<=Ypos+(int)(m_bottom-m_top)/Yzoom)
				{
					CPoint end(xPos1,m_bottom-(int)(((data[i*(this->m_nFrameSize)+j+1])+extent-Ypos)*Yzoom)+YDispos*Yzoom+k*Yzoom);
					if(xPos>=m_left&&xPos1>=m_left)
					{

						pDC->MoveTo (start);
						pDC->LineTo (end);

					}
				}
			}
			j++;
			xPos=((int)j*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;
			xPos1=((int)(j+1)*Xzoom-(int)Xpos*Xzoom+(int)XDispos*Xzoom)+i*this->m_nFrameSize/2*Xzoom+m_left;	
		}
		j=0;
	}

	pDC->SelectObject (pOldPen);
	if(pen.m_hObject )
		pen.DeleteObject ();
}

void CMyWaveView::OnMutidisplay()
{
	m_bMutiDisplay=!m_bMutiDisplay;
	Invalidate();
}


void CMyWaveView::OnUpdateMutidisplay(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bMutiDisplay);
}


void CMyWaveView::OnSpectrumdisplay()
{
	m_bSpecDisplay=!m_bSpecDisplay;
	Invalidate();
}

void CMyWaveView::OnUpdateSpectrumdisplay(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bSpecDisplay);
}

void CMyWaveView::OnMelcepdisplay()//显示mel倒谱
{
	m_bShowMfcc1=!m_bShowMfcc1;
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (pDoc->mfcc!=NULL)//不为空才更新视图
	{
		Invalidate();
	} 
	else
	{
		if (m_bShowMfcc1==TRUE)
		{
			MessageBox(_T("未计算文件1的Mel倒谱！"));
		}
	}
}

void CMyWaveView::OnUpdateMelcepdisplay(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowMfcc1);
}

void CMyWaveView::OnTwicefft()//计算频率振幅谱的fft
{
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	unsigned int num=pDoc->data_one.size ();
	int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
	unsigned int sample_rate=nCount*this->m_nFrameSize;
	int i;
	//为data重新分配空间
	if (data != NULL)
		delete[] data;
	if (dataout_spectrum!=NULL)
	{
		delete[] dataout_spectrum;
	}
	if (dataout_spectrumFFT != NULL)
		delete[] dataout_spectrumFFT;
	
	data=new double[sample_rate];
	dataout_spectrumFFT=new double[sample_rate];
	dataout_spectrum=new double[sample_rate];
	
	pOutNum=sample_rate;
	for( i=0;i<num;i++)
	{
		data[i]=pDoc->data_one[i];
	} 
	for (i=num;i<sample_rate;i++)
		data[i]=0;
	//加窗
	CSpeech::AddWindow (this->m_nFrameSize ,data,num);
	// 获取功率谱
	CWaveConvertor::ConvertToPowerSpectral(sample_rate, this->m_nFrameSize, data, dataout_spectrum);
	//对频谱实部再做fft变换
	//已经加了窗，因为每一帧已有256个数据
	CWaveConvertor::ConvertToPowerSpectral(sample_rate, this->m_nFrameSize, dataout_spectrum, dataout_spectrumFFT);//第二次fft变换
	maxForOneFft=minForOneFft=dataout_spectrum[0];
	maxForOneSpecFFT=minForOneSpecFFT=dataout_spectrumFFT[0];
	for (int j=0;j<sample_rate;j++)
	{
		if (maxForOneFft<dataout_spectrum[j])
		{
			maxForOneFft=dataout_spectrum[j];
		}
		if (minForOneFft>dataout_spectrum[j])
		{
			minForOneFft=dataout_spectrum[j];
		}
		if (maxForOneSpecFFT<dataout_spectrumFFT[j])
		{
			maxForOneSpecFFT=dataout_spectrumFFT[j];
		}
		if (minForOneSpecFFT>dataout_spectrumFFT[j])
		{
			minForOneSpecFFT=dataout_spectrumFFT[j];
		}
	}
}

void CMyWaveView::OnTwicefftdisplay()
{
	m_bSpecFFTDisplay=!m_bSpecFFTDisplay;
	if (dataout_spectrumFFT!=NULL)
	{
		Invalidate();
	} 
	else
	{
		if (m_bSpecFFTDisplay==TRUE)
		{
			MessageBox(_T("未计算文件1的频谱的频谱！"));
		}
	}
}

void CMyWaveView::OnUpdateTwicefftdisplay(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bSpecFFTDisplay);
}

void CMyWaveView::OnLogspechorizondisplay()
{
	m_bLogSpecHorizonDisplay=!m_bLogSpecHorizonDisplay;
	if (dataout!=NULL)
	{
		Invalidate();
	} 
	else
	{
		if (m_bLogSpecHorizonDisplay==TRUE)
		{
			MessageBox(_T("未计算文件1的对数功率谱！"));
		}
	}
}


void CMyWaveView::OnUpdateLogspechorizondisplay(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bLogSpecHorizonDisplay);
}

//计算mel频谱及mfcc
void CMyWaveView::OnMelspectrum()
{
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if(pDoc->data_one.size ()!=0)
	{
		unsigned int num=pDoc->data_one.size ();	
		int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
		unsigned int sample_rate=nCount*this->m_nFrameSize;
		unsigned filterNum=24,cepsNum=13;
		int i;

		//为data重新分配空间
		if (data != NULL)
			delete[] data;
		if (pDoc->mfcc != NULL)
			delete[] pDoc->mfcc;
		if (pDoc->melspectrum!=NULL)
			delete[] pDoc->melspectrum;


		data=new double[num];

		for( i=0;i<num;i++)
		{
			data[i]=pDoc->data_one[i];
		} 

		pDoc->mfcc=new double[cepsNum*nCount];//存放mfcc
		pDoc->melspectrum=new double[filterNum*nCount];//存放mel频谱

		// 获取MfCC
		CSpeech::GetMelSpectrum(
			this->m_nFrameSize, data, num,pDoc->m_HZ,filterNum,cepsNum,pDoc->mfcc,pDoc->melspectrum);
		//MFCC数据
		/**********************修改的代码**************************/
		mfccNum1=cepsNum*nCount;
		melspecNum1=filterNum*nCount;
		maxForOneMfcc=minForOneMfcc=pDoc->mfcc[0];
		for(unsigned int i=0;i<mfccNum1;i++)
		{
			if (maxForOneMfcc<pDoc->mfcc[i])
			{
				maxForOneMfcc=pDoc->mfcc[i];
			}
			if (minForOneMfcc>pDoc->mfcc[i])
			{
				minForOneMfcc=pDoc->mfcc[i];
			}
		}
		//mfcc归一化并且放大4000倍
		double magnify=4000/(maxForOneMfcc-minForOneMfcc);
		for (unsigned int k=0;k<mfccNum1;k++)
		{
			pDoc->mfcc[k]=(pDoc->mfcc[k]-minForOneMfcc)*magnify;
		}
		maxForOneMfcc=4000;
		minForOneMfcc=0;

		maxForOneMelSpec=minForOneMelSpec=pDoc->melspectrum[0];
		for (unsigned int j=0;j<melspecNum1;j++)
		{
			if (maxForOneMelSpec<pDoc->melspectrum[j])
			{
				maxForOneMelSpec=pDoc->melspectrum[j];
			}
			if (minForOneMelSpec>pDoc->melspectrum[j])
			{
				minForOneMelSpec=pDoc->melspectrum[j];
			}
		}

	}

}


void CMyWaveView::OnMelspectrumdisplay()
{
	m_bMelSpecDisplay=!m_bMelSpecDisplay;
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (pDoc->melspectrum!=NULL)//不为空才更新视图
	{
		Invalidate();
	} 
	else
	{
		if (m_bMelSpecDisplay==TRUE)
		{
			MessageBox(_T("未计算文件1的Mel频谱！"));
		}
	}
}


void CMyWaveView::OnUpdateMelspectrumdisplay(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bMelSpecDisplay);
}


void CMyWaveView::OnCepdetect()//基于倒谱的端点检测
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc=(CMyWaveDoc*)GetDocument();
	pDoc->Pitch();
}


void CMyWaveView::OnCepdetectdisplay()
{
	// TODO: 在此添加命令处理程序代码
	m_bCepDetectDisplay=!m_bCepDetectDisplay;
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (pDoc->ifft_one.size()>0&&pDoc->tag.size()>0)
	{
		Invalidate();
	} 
	else
	{
		if (m_bCepDetectDisplay==TRUE)
		{
			MessageBox(_T("未计算文件1的端点检测结果！"));
		}
	}
}


void CMyWaveView::OnUpdateCepdetectdisplay(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bCepDetectDisplay);
}

void CMyWaveView::DrawFFT(CDC* pDC,const std::vector<std::vector<float> >& data,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPlayPos,int endPlayPos,int k1,int k2,CString WaveName)
{
	//绘制曲线基准刻度
	int y=(int)(m_bottom-(32800-k2-m_Ypos-YDispos)*m_Yzoom);

	pDC->SetTextColor(RGB(0,0,0));
	pDC->TextOut(m_left+5,y-35,WaveName);

	CPen pen,line1,line2;
	pen.CreatePen(PS_SOLID,1,crColor);
	CPen* pOldPen=pDC->SelectObject(&pen);

	line1.CreatePen(PS_SOLID,1,RGB(60,100,220));
	line2.CreatePen(PS_SOLID,1,crColor);
	int j=(int)(Xpos-(double)XDispos);
	int i=0;				//i为开始绘制时的压力点的坐标！！！

	if(j<0)
		j=0;
	int xPos=int(j*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);
	int xPos1=int((j+1)*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);
	
	
	int m=1;
	int n=k2;
	int x=0;
	for(;i<(int)data.size();)
	{
		if(i%2==1)
		{n=k1;pDC->SelectObject(line1);}
		if(i%2==0)
		{n=k2;pDC->SelectObject(line2);}
		for(;xPos<=m_right && xPos1<=m_right && j+1<(int)data[i].size()/2;)
		{
			if(data[i][j]+extent-(int)(YDispos)-n>=Ypos && data[i][j]+extent-(int)(YDispos)-n<=Ypos+(int)(m_bottom-m_top)/Yzoom)
			{
				
				CPoint start(xPos,int(m_bottom-((data[i][j]+extent-Ypos)*Yzoom)+YDispos*Yzoom+n*Yzoom));
								
				if(data[i][j+1]+extent-(int)(YDispos)-n>=Ypos && data[i][j+1]+extent-(int)(YDispos)-n<=Ypos+(int)(m_bottom-m_top)/Yzoom)
				{
					CPoint end(xPos1,int(m_bottom-((data[i][j+1]+extent-Ypos)*Yzoom)+YDispos*Yzoom+n*Yzoom));
					if(xPos>=m_left && xPos1>=m_left)
					{					
						pDC->MoveTo(start);
						pDC->LineTo(end);					
					}
				}				
			}
			j++;
			xPos=int((j+(m_nFrameSize/2)*i)*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);
			xPos1=int((j+1+(m_nFrameSize/2)*i)*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);					
		}
		i++;		
		j=0;
		xPos=int((j+(m_nFrameSize/2)*i)*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);
		xPos1=int((j+1+(m_nFrameSize/2)*i)*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);	
	}
	pDC->SelectObject(pOldPen);
	if(pen.m_hObject)
		pen.DeleteObject();
}

void CMyWaveView::DrawPitch(CDC* pDC,const std::vector<std::vector<float> >& data,const std::vector<int>& f1,const std::vector<float>& p1,const std::vector<int>& f2,const std::vector<float>& p2,const std::vector<int>& f3,const std::vector<float>& p3,const std::vector<int>& tag,const std::vector<float>& divide,int extent,COLORREF crColor,int Xpos,int XDispos,double Xzoom,int Ypos,int YDispos,double Yzoom,int startPos,int endPos,int k1,int k2)
{
	CPen pen,line1,line2;
	pen.CreatePen(PS_SOLID,1,crColor);
	CPen* pOldPen=pDC->SelectObject(&pen);

	CString num1;
	CString num2;
	int i=0,j=0;					

	int xPos=int(j*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);
	int xPos1=int((j+1)*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);

	
	int m=1;
	int n1,n2;
	
	for(;i<(int)data.size();)
	{
		if(i%2==1)
		{
			n1=k1;
			n2=n1+1000;
		}
		else
		{
			n1=k2;
			n2=n1+1000;
		}
		
		j=0;
		
		xPos=int((j+(m_nFrameSize/2)*i)*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);
		if(tag[i]!=0)
		{
			num1.Format(_T("No:%d "),i+1);
			pDC->SetTextColor(RGB(0,0,0));
			pDC->TextOut(xPos,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2)*Yzoom),num1);

			num2.Format(_T("T1=%d"),f1[i]+1);
			pDC->TextOut(xPos,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+2000)*Yzoom),num2);
			
			if(p2[i]<p3[i])
			{
				num2.Format(_T("T2=%d"),f2[i]+1);
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+2000)*Yzoom),num2);

				num2.Format(_T("f2=%d"),16000/(f2[i]+1));
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+4000)*Yzoom),num2);

				num2.Format(_T("p2=%6.3f"),p2[i]);
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+6000)*Yzoom),num2);
			}
			else
			{
				num2.Format(_T("T3=%d"),f3[i]+1);
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+2000)*Yzoom),num2);

				num2.Format(_T("f3=%d"),16000/(f3[i]+1));
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+4000)*Yzoom),num2);

				num2.Format(_T("p3=%6.3f"),p3[i]);
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+6000)*Yzoom),num2);
			}
			//pDC->SetTextColor(RGB(0,0,0));
			num2.Format(_T("f1=%d"),16000/(f1[i]+1));
			//pDC->SetTextColor(RGB(255,0,0));
			pDC->TextOut(xPos,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+4000)*Yzoom),num2);
			
			num2.Format(_T("p1=%6.3f"),p1[i]);			
			pDC->TextOut(xPos,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+6000)*Yzoom),num2);
			//pDC->TextOut(int((j+(frame_LEN/2)*i+frame_LEN/4)*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left),int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+4000)*Yzoom),num2);
			
			num2.Format(_T("k1/k2=%6.3f "),divide[i]);
			pDC->TextOut(xPos,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+8000)*Yzoom),num2);
		}
		else
		{
			num1.Format(_T("No:%d  ***"),i+1);
			pDC->SetTextColor(RGB(0,0,0));
			pDC->TextOut(xPos,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2)*Yzoom),num1);

			num2.Format(_T("T1=%d"),f1[i]+1);
			pDC->SetTextColor(RGB(96,0,0));
			pDC->TextOut(xPos,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+2000)*Yzoom),num2);
			
			if(p2[i]<p3[i])
			{
				num2.Format(_T("T2=%d"),f2[i]+1);
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+2000)*Yzoom),num2);

				num2.Format(_T("f2=%d"),16000/(f2[i]+1));
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+4000)*Yzoom),num2);

				num2.Format(_T("p2=%6.3f"),p2[i]);
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+6000)*Yzoom),num2);
			}
			else
			{
				num2.Format(_T("T3=%d"),f3[i]+1);
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+2000)*Yzoom),num2);

				num2.Format(_T("f3=%d"),16000/(f3[i]+1));
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+4000)*Yzoom),num2);

				num2.Format(_T("p3=%6.3f"),p3[i]);
				pDC->TextOut(xPos+70,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+6000)*Yzoom),num2);
			}

			num2.Format(_T("f1=%d"),16000/(f1[i]+1));
			//pDC->SetTextColor(RGB(255,0,0));
			pDC->TextOut(xPos,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+4000)*Yzoom),num2);
			
	
			
			num2.Format(_T("p1=%6.3f"),p1[i]);			
			pDC->TextOut(xPos,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+6000)*Yzoom),num2);
			

			
			num2.Format(_T("k1/k2=%6.3f "),divide[i]);
			pDC->TextOut(xPos,int(m_bottom-(data[i][j]+extent-Ypos)*Yzoom+(YDispos+n2+8000)*Yzoom),num2);
		}
		i++;		
	//	j=0;
		
		xPos=int((j+(m_nFrameSize/2)*i)*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);
		xPos1=int((j+1+(m_nFrameSize/2)*i)*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);	
		//pos=int((j+(frame_LEN/2)*i)*Xzoom-Xpos*Xzoom+XDispos*Xzoom+m_left);

	}	
	
	pDC->SelectObject(pOldPen);
	if(pen.m_hObject)
		pen.DeleteObject();
}

void CMyWaveView::OnCepdisplay()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowCep1=!m_bShowCep1;
	if (dataout_cep!=NULL)
	{
		Invalidate();
	} 
	else
	{
		if (m_bShowCep1==TRUE)
		{
			MessageBox(_T("未计算文件1的倒谱！"));
		}
	}
	

}


void CMyWaveView::OnUpdateCepdisplay(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bShowCep1);
}


void CMyWaveView::OnFftdisplay()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowFft1=!m_bShowFft1;
	if (dataout_spectrum!=NULL)//曲线数据已读取
	{
		Invalidate();
	}
	else
	{
		if (m_bShowFft1==TRUE)
		{
			MessageBox(_T("文件1的频谱未计算！"));
		}
	}
}


void CMyWaveView::OnUpdateFftdisplay(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bShowFft1);
}


void CMyWaveView::OnShowwave()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowFile1=!m_bShowFile1;
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (pDoc->data_one.size()>0)//曲线数据已读取
	{
		Invalidate();
	}
	else
	{
		if (m_bShowFile1==TRUE)
		{
			MessageBox(_T("不存在文件1，请打开文件1。"));
		}
	}
}


void CMyWaveView::OnUpdateShowwave(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bShowFile1);
}


void CMyWaveView::OnSpecfft1tran()
{
	// TODO: 在此添加命令处理程序代码
	m_transID=_SpecFFT1;
	Invalidate();
}


void CMyWaveView::OnUpdateSpecfft1tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_transID==_SpecFFT1);
}


void CMyWaveView::OnMelspec1tran()
{
	// TODO: 在此添加命令处理程序代码
	m_transID=_MelSpec1;
	Invalidate();
}


void CMyWaveView::OnUpdateMelspec1tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_transID==_MelSpec1);
}


void CMyWaveView::OnLogspec1tran()
{
	// TODO: 在此添加命令处理程序代码
	m_transID=_LogSpec1;
	Invalidate();
}


void CMyWaveView::OnUpdateLogspec1tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_transID==_LogSpec1);
}


void CMyWaveView::OnLink()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(false, NULL, L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, 

		L"音频文件(*.wav)|*.wav|所有文件(*.*)|*.*||");
	if(IDOK==dlg.DoModal())
	{
		CString FileName=dlg.GetFileName();
		CString FilePath=dlg.GetPathName();
		FilePath+=_T(".wav");

		CMyWaveDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc)
			return;
		int n1=pDoc->data_one.size();
		int n2=pDoc->data_one2.size();
		std::vector<short> data;
		int i;
		for(i=0;i<n1;i++)
			data.push_back(pDoc->data_one[i]);
		for(i=0;i<n2;i++)
			data.push_back(pDoc->data_one2[i]);

		////保存有效数据
		char chr[256];
		memset(chr,0,256);
		WideCharToMultiByte(CP_OEMCP,0,FilePath,-1,chr,FilePath.GetLength()

			*2,NULL,NULL);

		FILE* pfile;
		if((pfile=fopen(chr,"wb"))==NULL)
		{
			AfxMessageBox(_T("创建文件失败!"));
			return;
		}
		int sum=data.size();

		int size=sum*pDoc->m_bytesPerSample+58;

		char ch='R';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='I';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='F';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		fwrite(&ch,sizeof(unsigned char),1,pfile);

		int ii=size-8;
		fwrite(&ii,sizeof(DWORD),1,pfile);

		ch='W';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='A';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='V';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='E';
		fwrite(&ch,sizeof(unsigned char),1,pfile);

		ch='f';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='m';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='t';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch=' ';
		fwrite(&ch,sizeof(unsigned char),1,pfile);

		int  num1=pDoc->header.fmt.dwFmtSize;
		fwrite(&num1,sizeof(DWORD),1,pfile);

		fwrite(&(pDoc->header.fmt.wavFormat.wFormatTag),sizeof(WORD),1,pfile);
		fwrite(&(pDoc->header.fmt.wavFormat.wChannels),sizeof(WORD),1,pfile);
		fwrite(&(pDoc->header.fmt.wavFormat.dwSamplesPerSec),sizeof

			(DWORD),1,pfile);
		fwrite(&(pDoc->header.fmt.wavFormat.dwAvgBytesPerSec),sizeof

			(DWORD),1,pfile);
		fwrite(&(pDoc->header.fmt.wavFormat.wBlockAlign),sizeof(WORD),1,pfile);
		fwrite(&(pDoc->header.fmt.wavFormat.wBitsPerSample),sizeof(WORD),1,pfile);
		if(num1==18)
		{
			WORD temp=12;
			fwrite(&temp,sizeof(WORD),1,pfile);
		}

		ch='f';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='a';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='c';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='t';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		int four=4;
		fwrite(&four,sizeof(DWORD),1,pfile);
		DWORD temp1=123123;
		fwrite(&temp1,sizeof(DWORD),1,pfile);

		ch='d';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='a';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='t';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='a';
		fwrite(&ch,sizeof(unsigned char),1,pfile);

		DWORD t=size-58;
		fwrite(&t,sizeof(DATA_BLOCK),1,pfile);

		if(pDoc->m_bytesPerSample==1)
		{
			for(int k=0;k<sum;k++)
				fwrite(&data[k],sizeof(BYTE),1,pfile);
		}
		else
		{
			for(int k=0;k<sum;k++)
			{
				fwrite(&data[k],sizeof(WORD),1,pfile);
			}
		}
		fclose(pfile);

	}
}


void CMyWaveView::OnSaveEffectiveMfcc()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc *pDoc=(CMyWaveDoc*)GetDocument();

	if (pDoc->tag.size()>0&& pDoc->mfcc!=NULL)
	{
		CString FilePath(pDoc->m_strFilePath,pDoc->m_strFilePath.GetLength ()-3);
		FilePath+=_T("txt");

		ofstream fout( FilePath, ios::trunc );

		for (unsigned int i=0;i<pDoc->tag.size();i++)
		{
			if (pDoc->tag.at(i))
			{
				fout<<i<<" ";
				for (unsigned int j=0;j<13;j++)
				{
					fout<<pDoc->mfcc[i*13+j]<<" ";
				}
				fout<<"1"<<endl;
			}
			else
			{
				fout<<i<<" ";
				for (unsigned int j=0;j<13;j++)
				{
					fout<<pDoc->mfcc[i*13+j]<<" ";
				}
				fout<<"0"<<endl;
			}
			
		}
		//关闭输出流
		fout.close();
	}
}


void CMyWaveView::OnCepstrum2()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	unsigned int num=pDoc->data_one2.size ();
	int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
	unsigned int sample_rate=nCount*this->m_nFrameSize;
	int i;
	//为data重新分配空间
	if (data_2 != NULL)
		delete[] data_2;
	if (dataout_cep_2!=NULL)
	{
		delete []dataout_cep_2;
	}

	data_2=new double[sample_rate];
	dataout_cep_2=new double[sample_rate];

	pOutNum2=sample_rate;
	for( i=0;i<num;i++)
	{
		data_2[i]=pDoc->data_one2[i];
	} 
	for (i=num;i<sample_rate;i++)
		data_2[i]=0;

	//加窗
	CSpeech::AddWindow (this->m_nFrameSize ,data_2,num);
	// 获取倒谱
	CWaveConvertor::ConvertToCepStrum(
		sample_rate, this->m_nFrameSize, data_2, dataout_cep_2);//dataout--dataout_cep

	//保存倒谱数据	
	maxForTwoCep=minForTwoCep=dataout_cep_2[0]*10000;
	//ofstream fout( FilePath, ios::trunc );
	for(unsigned int i=0;i<nCount;i++)
	{
		for (unsigned int j = 0; j <this->m_nFrameSize; j++)
		{	


			//		fout <<dataout[i*this->m_nFrameSize+j]<< "   "; 
			dataout_cep_2[i*this->m_nFrameSize+j]=dataout_cep_2[i*this->m_nFrameSize+j]*10000;//原:100000/2		

			if(maxForTwoCep<dataout_cep_2[i*this->m_nFrameSize+j])
				maxForTwoCep=dataout_cep_2[i*this->m_nFrameSize+j];
			if(minForTwoCep>dataout_cep_2[i*this->m_nFrameSize+j])
				minForTwoCep=dataout_cep_2[i*this->m_nFrameSize+j];
		}
		//	fout<<endl;
	}
}


void CMyWaveView::OnShowwave2()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowFile2=!m_bShowFile2;
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (pDoc->data_one2.size()>0)//曲线数据已读取
	{
		Invalidate();
	}
	else
	{
		if (m_bShowFile2==TRUE)
		{
			MessageBox(_T("不存在文件2，请打开文件2。"));
		}
	}
}


void CMyWaveView::OnUpdateShowwave2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bShowFile2==true);
}


void CMyWaveView::OnCepdisplay2()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowCep2=!m_bShowCep2;
	if (dataout_cep_2!=NULL)
	{
		Invalidate();
	} 
	else
	{
		if (m_bShowCep2==TRUE)
		{
			MessageBox(_T("未计算文件2的倒谱！"));
		}
	}
	Invalidate();

}


void CMyWaveView::OnUpdateCepdisplay2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bShowCep2==TRUE);
}


void CMyWaveView::OnCep2Tran()
{
	// TODO: 在此添加命令处理程序代码
	m_transID=_CEP2;
	Invalidate(TRUE);

}


void CMyWaveView::OnUpdateCep2Tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_transID==_CEP2);
}

void CMyWaveView::OnTwicefft2()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	unsigned int num=pDoc->data_one2.size ();
	int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
	unsigned int sample_rate=nCount*this->m_nFrameSize;
	int i;
	//为data重新分配空间
	if (data_2 != NULL)
		delete[] data_2;
	if (dataout_spectrum_2!=NULL)
	{
		delete[] dataout_spectrum_2;
	}
	if (dataout_spectrumFFT_2 != NULL)
		delete[] dataout_spectrumFFT_2;

	data_2=new double[sample_rate];
	dataout_spectrumFFT_2=new double[sample_rate];
	dataout_spectrum_2=new double[sample_rate];

	pOutNum2=sample_rate;
	for( i=0;i<num;i++)
	{
		data_2[i]=pDoc->data_one2[i];
	} 
	for (i=num;i<sample_rate;i++)
		data_2[i]=0;
	//加窗
	CSpeech::AddWindow (this->m_nFrameSize ,data_2,num);
	// 获取功率谱
	CWaveConvertor::ConvertToPowerSpectral(sample_rate, this->m_nFrameSize, data_2, dataout_spectrum_2);
	//对频谱实部再做fft变换
	//已经加了窗，因为每一帧已有256个数据
	CWaveConvertor::ConvertToPowerSpectral(sample_rate, this->m_nFrameSize, dataout_spectrum_2, dataout_spectrumFFT_2);//第二次fft变换
	maxForTwoFft=minForTwoFft=dataout_spectrum_2[0];
	maxForTwoSpecFFT=minForTwoSpecFFT=dataout_spectrumFFT_2[0];
	for (int j=0;j<sample_rate;j++)
	{
		if (maxForTwoFft<dataout_spectrum_2[j])
		{
			maxForTwoFft=dataout_spectrum_2[j];
		}
		if (minForTwoFft>dataout_spectrum_2[j])
		{
			minForTwoFft=dataout_spectrum_2[j];
		}
		if (maxForTwoSpecFFT<dataout_spectrumFFT_2[j])
		{
			maxForTwoSpecFFT=dataout_spectrumFFT_2[j];
		}
		if (minForTwoSpecFFT>dataout_spectrumFFT_2[j])
		{
			minForTwoSpecFFT=dataout_spectrumFFT_2[j];
		}
	}
}


void CMyWaveView::OnFftdisplay2()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowFft2=!m_bShowFft2;
	if (dataout_spectrum_2!=NULL)//曲线数据已读取
	{
		Invalidate();
	}
	else
	{
		if (m_bShowFft2==TRUE)
		{
			MessageBox(_T("文件2的频谱未计算！"));
		}
	}
}

void CMyWaveView::OnUpdateFftdisplay2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bShowFft2==TRUE);
}

void CMyWaveView::OnTwicefftdisplay2()
{
	// TODO: 在此添加命令处理程序代码
	m_bSpecFFTDisplay2=!m_bSpecFFTDisplay2;
	if (dataout_spectrumFFT_2!=NULL)
	{
		Invalidate();
	} 
	else
	{
		if (m_bSpecFFTDisplay2==TRUE)
		{
			MessageBox(_T("未计算文件2的频谱的频谱！"));
		}
	}
}


void CMyWaveView::OnUpdateTwicefftdisplay2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bSpecFFTDisplay2==true);
}

void CMyWaveView::OnFft2Tran()
{
	// TODO: 在此添加命令处理程序代码
	m_transID=_FFT2;
	Invalidate(TRUE);
}


void CMyWaveView::OnUpdateFft2Tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_transID==TRUE);
}


void CMyWaveView::OnSpecfft2tran()
{
	// TODO: 在此添加命令处理程序代码
	m_transID=_SpecFFT2;
	Invalidate(TRUE);
}


void CMyWaveView::OnUpdateSpecfft2tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_transID==_SpecFFT2);
}


void CMyWaveView::OnLogspectrum2()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	unsigned int num=pDoc->data_one2.size ();
	int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
	unsigned int sample_rate=nCount*this->m_nFrameSize;
	int i;
	//为data重新分配空间
	if (data != NULL)
		delete[] data_2;
	if (dataout != NULL)
		delete[] dataout_2;

	data_2=new double[sample_rate];
	dataout_2=new double[sample_rate];
	pOutNum2=sample_rate;
	for( i=0;i<num;i++)
	{
		data_2[i]=pDoc->data_one2[i];
	} 
	for (i=num;i<sample_rate;i++)
		data_2[i]=0;
	//加窗
	CSpeech::AddWindow (this->m_nFrameSize ,data_2,num);
	// 获取对数功率谱
	CWaveConvertor::ConvertToLogPowerSpectral(
		sample_rate, this->m_nFrameSize, data_2, dataout_2);
	//保存对数功率谱(最大、最小值)
	
	if (dataout_2[0]<0)
	{
		dataout_2[0]=0;
	}
	maxForTwoLogSpec=minForTwoLogSpec=dataout_2[0];
	
	for(unsigned int i=0;i<sample_rate;i++)
	{
		if (dataout_2[i]<0)
		{
			dataout_2[i]=0;
		}
		if (maxForTwoLogSpec<dataout_2[i])
		{
			maxForTwoLogSpec=dataout_2[i];
		}
		if (minForTwoLogSpec>dataout_2[i])
		{
			minForTwoLogSpec=dataout_2[i];
		} 	
	}
	/***********************归一化*********************/
	double magnify=5000/(maxForTwoLogSpec-minForTwoLogSpec);
	for (int j=0;j<sample_rate;j++)
	{
		dataout_2[j]=(dataout_2[j]-minForTwoLogSpec)*magnify;
	}

	maxForTwoLogSpec=4000;
	minForTwoLogSpec=0;
}


void CMyWaveView::OnLogspechorizondisplay2()
{
	// TODO: 在此添加命令处理程序代码
	m_bLogSpecHorizonDisplay2=!m_bLogSpecHorizonDisplay2;
	if (dataout_2!=NULL)
	{
		Invalidate();
	} 
	else
	{
		if (m_bLogSpecHorizonDisplay2==TRUE)
		{
			MessageBox(_T("未计算文件2的对数功率谱！"));
		}
	}
}


void CMyWaveView::OnUpdateLogspechorizondisplay2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bLogSpecHorizonDisplay2==TRUE);
}


void CMyWaveView::OnLogspec2tran()
{
	// TODO: 在此添加命令处理程序代码
	m_transID=_LogSpec2;
	Invalidate(TRUE);
}


void CMyWaveView::OnUpdateLogspec2tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_transID==_LogSpec2);
}


void CMyWaveView::OnMelspectrum2()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if(pDoc->data_one2.size ()!=0)
	{
		unsigned int num=pDoc->data_one2.size ();	
		int nCount=(int)floor((double)((num-this->m_nFrameSize)/(this->m_nFrameSize/2))+1);
		unsigned int sample_rate=nCount*this->m_nFrameSize;
		unsigned filterNum=24,cepsNum=13;
		int i;

		//为data重新分配空间
		if (data_2 != NULL)
			delete[] data_2;
		if (pDoc->mfcc2 != NULL)
			delete[] pDoc->mfcc2;
		if (pDoc->melspectrum2!=NULL)
			delete[] pDoc->melspectrum2;


		data_2=new double[num];

		for( i=0;i<num;i++)
		{
			data_2[i]=pDoc->data_one2[i];
		} 

		pDoc->mfcc2=new double[cepsNum*nCount];//存放mfcc
		pDoc->melspectrum2=new double[filterNum*nCount];//存放mel频谱

		// 获取MfCC
		CSpeech::GetMelSpectrum(
			this->m_nFrameSize, data_2, num,pDoc->m_HZ2,filterNum,cepsNum,pDoc->mfcc2,pDoc->melspectrum2);
		//MFCC数据
		/**********************修改的代码**************************/
		mfccNum2=cepsNum*nCount;
		melspecNum2=filterNum*nCount;
		maxForTwoMfcc=minForTwoMfcc=pDoc->mfcc2[0];
		for(i=0;i<mfccNum2;i++)
		{
			if (maxForTwoMfcc<pDoc->mfcc2[i])
			{
				maxForTwoMfcc=pDoc->mfcc2[i];
			}
			if (minForTwoMfcc>pDoc->mfcc2[i])
			{
				minForTwoMfcc=pDoc->mfcc2[i];
			}
		}
		//mfcc归一化并且放大4000倍
		double magnify=4000/(maxForTwoMfcc-minForTwoMfcc);
		for (unsigned int k=0;k<mfccNum2;k++)
		{
			pDoc->mfcc2[k]=(pDoc->mfcc2[k]-minForTwoMfcc)*magnify;
		}
		maxForTwoMfcc=4000;
		minForTwoMfcc=0;

		maxForTwoMelSpec=minForTwoMelSpec=pDoc->melspectrum2[0];
		for (unsigned int j=0;j<melspecNum2;j++)
		{
			if (maxForTwoMelSpec<pDoc->melspectrum2[j])
			{
				maxForTwoMelSpec=pDoc->melspectrum2[j];
			}
			if (minForTwoMelSpec>pDoc->melspectrum2[j])
			{
				minForTwoMelSpec=pDoc->melspectrum2[j];
			}
		}
	
	}
}


void CMyWaveView::OnMelspectrumdisplay2()
{
	// TODO: 在此添加命令处理程序代码
	m_bMelSpecDisplay2=!m_bMelSpecDisplay2;
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (pDoc->melspectrum2!=NULL)//不为空才更新视图
	{
		Invalidate();
	} 
	else
	{
		if (m_bMelSpecDisplay2==TRUE)
		{
			MessageBox(_T("未计算文件2的Mel频谱！"));
		}
	}
}


void CMyWaveView::OnUpdateMelspectrumdisplay2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bMelSpecDisplay2==TRUE);
}


void CMyWaveView::OnMelcepdisplay2()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowMfcc2=!m_bShowMfcc2;
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (pDoc->mfcc2!=NULL)//不为空才更新视图
	{
		Invalidate();
	} 
	else
	{
		if (m_bShowMfcc2==TRUE)
		{
			MessageBox(_T("未计算文件2的Mel倒谱！"));
		}
	}
}


void CMyWaveView::OnUpdateMelcepdisplay2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bShowMfcc2==TRUE);
}


void CMyWaveView::OnMelspec2tran()
{
	// TODO: 在此添加命令处理程序代码
	m_transID=_MelSpec2;
	Invalidate();
}


void CMyWaveView::OnUpdateMelspec2tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_transID==_MelSpec2);
}


void CMyWaveView::OnCepdetect2()
{
	// TODO: 在此添加命令处理程序代码
	CMyWaveDoc* pDoc=(CMyWaveDoc*)GetDocument();
	pDoc->Pitch2();
}


void CMyWaveView::OnCepdetectdisplay1()
{
	// TODO: 在此添加命令处理程序代码
	m_bCepDetectDisplay=!m_bCepDetectDisplay;
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (pDoc->ifft_one.size()>0&&pDoc->tag.size()>0)
	{
		Invalidate();
	} 
	else
	{
		if (m_bCepDetectDisplay==TRUE)
		{
			MessageBox(_T("未计算文件1的端点检测结果！"));
		}
	}
}


void CMyWaveView::OnUpdateCepdetectdisplay1(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bCepDetectDisplay==TRUE);
}


void CMyWaveView::OnCepdetect1tran()
{
	// TODO: 在此添加命令处理程序代码
	m_transID=_CepDetect1;
	Invalidate(TRUE);
}


void CMyWaveView::OnUpdateCepdetect1tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_transID==_CepDetect1);
}


void CMyWaveView::OnCepdetect2tran()
{
	// TODO: 在此添加命令处理程序代码
	m_transID=_CepDetect2;
	Invalidate(TRUE);
}


void CMyWaveView::OnUpdateCepdetect2tran(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_transID==_CepDetect2);
}


void CMyWaveView::OnCepdetectdisplay2()
{
	// TODO: 在此添加命令处理程序代码
	m_bCepDetectDisplay2=!m_bCepDetectDisplay2;
	CMyWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (pDoc->ifft_two.size()>0&&pDoc->tag_2.size()>0)
	{
		Invalidate();
	} 
	else
	{
		if (m_bCepDetectDisplay2==TRUE)
		{
			MessageBox(_T("未计算文件2的端点检测结果！"));
		}
	}
}


void CMyWaveView::OnUpdateCepdetectdisplay2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bCepDetectDisplay2==TRUE);

}


void CMyWaveView::OnFrmlen256()
{
	// TODO: 在此添加命令处理程序代码
	m_nFrameSize=256;
	m_bfrmlen256=TRUE;
	m_bfrmlen512=FALSE;
	m_bfrmlen1024=FALSE;
	Invalidate();
}

void CMyWaveView::OnUpdateFrmlen256(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bfrmlen256==TRUE);
	
}

void CMyWaveView::OnFrmlen512()
{
	// TODO: 在此添加命令处理程序代码
	m_nFrameSize=512;
	m_bfrmlen512=TRUE;
	m_bfrmlen256=FALSE;
	m_bfrmlen1024=FALSE;
	Invalidate();
}

void CMyWaveView::OnUpdateFrmlen512(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bfrmlen512==TRUE);
}

void CMyWaveView::OnFrmlen1024()
{
	// TODO: 在此添加命令处理程序代码
	m_nFrameSize=1024;
	m_bfrmlen1024=TRUE;
	m_bfrmlen512=FALSE;
	m_bfrmlen256=FALSE;
	Invalidate();
}

void CMyWaveView::OnUpdateFrmlen1024(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bfrmlen1024==TRUE);
}
