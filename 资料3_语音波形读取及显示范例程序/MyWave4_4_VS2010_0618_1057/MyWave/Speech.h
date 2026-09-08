#pragma once

#include<math.h>
#include<vector>
// 定义pi值
#ifndef pi
#define pi   3.14159265
#define ALPHA   0.41  /*0.41at 12KHZ sampling*/
#endif
#define SFREMQ 22050

#define PI 3.14159265
#define PI_2 6.2831853
#define BPS8 8
#define BPS16 16
#define FRM_SIZE 256
#define FRM_SHIFT 128
#define THRESHOLD_ENERGY 30
/////////////////////////////////////////////////////////////////////////
struct Freq{                         //mel倒谱中三角滤波器
      double start;
	  double center;
	  double stop;
};

//////////////////////////////////////////////////////////////////////////
// class CSpeech
//
// 功能：	进行语音参数分析
// 创建人：	陈文凯 (chwkai@gmail.com)
// 创建日期:2005年5月21日
// 修改人：
// 修改日期：
// 版本
class CSpeech
{
public:
	CSpeech(void);
public:
	~CSpeech(void);
public:
	enum WINDOW_TYPE//窗函数类型
	{
		eWND_RECTANGULAR, // Rectangular window
		eWND_HAMMING, // Hamming window
		eWND_HANNING ,// Hanning window
		eWND_BLACKMAN//Blackman window
	};
    // 用于判断是否过零
	// sgn(nValue) = 1, nValue >= 0
	//			   = -1, nValue < 0
	inline static int Sgn(double nValue)
	{
		return (nValue >= 0 ? 1 : -1);
	}

	// 返回海明窗计算的窗口函数
	// w(n) = (0.54 - 0.46 * cos((2 * pi * n) / (N - 1))) , 0 <= n <= N - 1
	//      = 0
	// 其中N为窗口长度，n为采样在窗口中的位置
	inline static double HammingWinFunc(unsigned int n,unsigned int nWinSize)
	{
		return ((0<=n && n<nWinSize)?(0.54-0.46*cos((2*pi*n)/(nWinSize-1))):0);
	}
    //对输入数据加窗
	static void AddWindow(
		unsigned int nWinSize,     //处理窗宽度
		double* pData,             //输入数据
		unsigned int nInLen        //输入数据长度
		);
	// 对输入数据分帧
	static void Frame(
		unsigned int nWinSize,		// 处理窗宽度
		const double* pDataIn,		// 输入数据
		unsigned int nInLen,		// 输入数据长度
		double* pDataOut			// 输出数据
		);
	// 对输入数据加窗
	static void AddWindow(
		unsigned int nWinSize,		// 处理窗宽度
		const double* pDataIn,		// 输入数据
		unsigned int nInLen,		// 输入数据长度
		double* pDataOut			// 输出数据
		);
      ///////////////////////////////////窗函数
	static void GetWindowCoef(double* pWndCoef,
		                       UINT nWndType);
	//////////////////分帧函数
	static void FrameProcessing(const double* pIndata,//输入数据
		                        int nFrame, //第几帧
								unsigned int m_nFrameSize,//窗长
								double* pOut,//输出数据
								double* pWcoef);//窗函数
	///////////////////////////////////////////////////////////////////////////////////////////////////////////ACF法求基音周期
    static void GetAcfFrequency(unsigned int nWinSize,		    /*处理窗口大小*/
	                            const double* pDataIn,			/*输入信号序列*/
	                            unsigned int nInLen,			/*输入信号序列长度*/
	                            double* pitch,			/*输出的自相关系数*/
		                        unsigned int nFs);
    static double ACF(double* pIn, double* pOut,unsigned int m_nFrameSize); // Calc auto correlation function
    static double AcfFrequency(double *pAC, int nFs,unsigned int m_nFrameSize);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////AMDF法求基音周期
	static double GetAmdfFrequency(const double * pInData,unsigned int dwSize, int nFs,unsigned int m_nFrameSize, double* pitch); // Get base frequency by AMDF	
	static double AMDF(double* pIn, double* pOut, unsigned int m_nFrameSize); // Calc average magnitude difference function
	static double AmdfFrequency(double* pAC, int nFs,unsigned int m_nFrameSize); // Get frequency of a frame by valley-picking
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////ACF+AMDF
	double GetAcfAmdfFrequency(const double* pInData, DWORD dwSize, int nFs); // Get base frequency by ACF and AMDF
	double AcfAmdf(double* pIn, double* pOut,unsigned int m_nFrameSize); // Calc auto correlation and average magnitude difference function
	double AcfAmdfFrequency(double* pAC, int nFs, unsigned int m_nFrameSize); // Get frequency of a frame by peak-picking


		// 计算输入信号序列的短时平均过零率，输入数据为加窗后的数据
	static void GetZero(
		unsigned int nWinSize,		/*处理窗口大小*/
		const double* pDataIn,			/*输入信号序列*/
		unsigned int nInLen,		/*输入信号序列长度*/
		double* pDataOut			/*输出过零率序列*/
		);

	// 计算输入信号序列的短时能量，输入数据为加窗后的数据
	static void GetEnergy(
		unsigned int nWinSize,		/*处理窗口大小*/
		const double* pDataIn,			/*输入信号序列*/
		unsigned int nInLen,		/*输入信号序列长度*/
		double* pDataOut			/*输出短时能量序列*/
		);

	// 计算输入信号序列的平均振幅，输入数据为加窗后的数据
	static void GetAvgVibration(
		unsigned int nWinSize,		/*处理窗口大小*/
		const double* pDataIn,		/*输入信号序列*/
		unsigned int nInLen,		/*输入信号序列长度*/
		double* pDataOut			/*输出平均振幅序列*/
		);
	//求修正的自相关函数
	static void GetRcor(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* pDataOut				/*输出的自相关系数*/
	);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////赵力————计算信号的自相关函数
	//static void correl(
	//	const double* pDataIn,			/*输入的加窗后的信号序列*/
	//	double* pDataOut,                /*信号的自相关函数*/
	//	unsigned int nWinSize,           /*数据窗的长度*/
	//	int ipnum                           /*LPC分析的阶数*/	               
	//	);
	////////////////////////////////////////赵力————由自相关函数计算LPC预测系数和反射系数
	//static void corref(
	//	int ipnum,                          /*LPC分析的阶数*/
	//	double * cor,                    /*信号的自相关函数cor[1]~cor[ip]*/
	//    double * alf,                    /*LPC系数（alf[1]~alf[n],注意：alf[0]=1）*/
	//	double *ref,                     /*反射系数(ref[1]~ref[ip],符号与常规的定义相反)*/
	//	double resif                     /*LPC预测残差*/
	//	);
 //    /////////////////////////////////////赵力————由LPC预测系数计算LPC倒谱系数
	//static void alfcep( int ipnum,                /*LPC分析的阶数*/
	//	                double *alf,           /*LPC预测系数（alf[1]~alf[ip]）*/
	//					double *cep,           /*LPC倒谱系数（cep[1]~cep[n],注意：cep[0]在本程序中未用）*/
	//					int n                  /*LPC倒谱系数的阶数*/
	//	);
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//基于短时自相关法的基音周期估值
	static void GetACFPitch(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* pitch,			/*输出的自相关系数*/
		unsigned int nFs,
		double *acf
	);
    //基于短时平均幅度差函数AMDF法的基音周期估值
	static void AMDFPitch(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* pDataOut				/*输出的自相关系数*/
	);
	    //基于倒谱法的基音周期估值
	static void CepPitch(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* pDataOut,			/*输出的倒谱*/
		double * pitch              /*输出基音周期值*/
	);

	//求LPC预测系数
	static void GetLPC(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* lpc			/*输出的LPC预测系数*/
	);
	//用lpc函数所求系数合成语音信号
	static void LPCToDATA(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* pDataOut				/*用lpc函数所求系数合成语音信号*/
	);
	//求LPC倒谱
	static void GetLPCC(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* lpcc,				/*输出的LPCC*/
		double* lpc
	);
	////////////////////////////////////////赵力————由LPC倒谱系数计算Mel倒谱系数
	static void GetLPCCMCC(
						unsigned int nWinSize,		    /*处理窗口大小*/
	                    const double* pDataIn,			/*输入信号序列*/
	                    unsigned int nInLen,			/*输入信号序列长度*/
		                double * mel                    /*Mel倒谱系数*/
					   );
	//求Melc
	static void GetMFCC(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
		unsigned int fs,               //采样频率
		unsigned int filterNum,         //滤波器个个数
		unsigned int cepsNum,           //mel系数的个数
	    double* mel			/*输出MFCC*/
	);
	//求mel频谱及Melc
	static void GetMelSpectrum(
		unsigned int nWinSize,		    /*处理窗口大小*/
		const double* pDataIn,			/*输入信号序列*/
		unsigned int nInLen,			/*输入信号序列长度*/
		unsigned int fs,               //采样频率
		unsigned int filterNum,         //滤波器个个数
		unsigned int cepsNum,           //mel系数的个数
		double* mfcc,					//输出MFCC
		double* melSpetrum				//输出mel频谱
		);

		//求Melc
	static void GetMFCC1(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
		unsigned int fs,               //采样频率
		unsigned int filterNum,         //滤波器个个数
		unsigned int cepsNum,           //mel系数的个数
	    double* mel			/*输出MFCC*/
	);
  /*static void  getMelScaleBand(double *in,int bankNum,int sample,int FFTnum);*/
 //三角滤波,len为powersample数字长度,num为mei滤波个数
//函数返回mel频率下的滤波器内的对数能量double*(长度为num)
//static double* _mfcc_filter(double *powersample,int len,int num);
////离散余弦变换,lnpower(长度为len)为滤波器内积对数能量,num与mel滤波器一致,dctnum为dct变换级数
////函数返回时double*的长度为dctnum,代表dct变换后的倒频参数
//static double* _mfcc_DCT(double *lnpower,int len,int dctnum);
/////////////////////////////////////////////////////////////////////////////////////////////////////张智星版本
//static double lin2melFreq(double linFreq);//线性频率到mel频率的转换
//static double mel2linFreq(double melFreq);//mel频率到线性频率的转换
static double freq2mel(double freq);//实际频率到mel频率的转换
static double mel2freq(double mel );//mel频率到实际频率的转换
//获取三角形滤波器的下限、中限、上限频率
static Freq* getTriFilterParam(int frameSize, //窗口大小
									 unsigned int fs,//采样频率
									 int filterNum);//滤波器的数
//获取三角带通滤波器(Triangular band-pass filters)的输出mel[L]
static  void triBandFilter(
					  int framesize,
					  double *fftMag,//傅里叶变换后的abs
					  int filterNum,//滤波器的数目
					  Freq* freq,//滤波器的三个限值
					  double* filtmag,//保存wl(k)
					  double *tbfCoef//保存返回值
					  );
//DCT到Mfcc转换
static void melCepstrum(
						   unsigned int nWinSize,
						   int L,//mfcc系数个数
						   int filterNum,//滤波器个数
						   double* tbfCoef,
						   double* mfcc//保存返回值
						   );
//static double * deltaFunction( int deltaWindow,    /////////MFCC参数进行Delta运算
//							   double parameter
//							  );
static void MaxCor(double * data,unsigned int len,double & num);//获取一帧自相关系数的最大值和下标
static double Medfilt(double *data,int n);//中值平滑
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////张智星版本（完）