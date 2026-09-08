#ifndef _CWAVE_CONVERTOR_H_
#define _CWAVE_CONVERTOR_H_
#include <math.h>
#include <complex>
//#define PI 3.1415926
using namespace std;
#pragma once
//////////////////////////////////////////////////////////////////////////
// class CWaveConvertor
//
// 功能：	实现裸音频数据的FFT变换
// 创建人：
// 创建日期:
// 修改人：
// 修改日期：
// 版本


//////////////////////////////////////////////////////////////////////////

//负责实现语音裸数据的各种转换
class CWaveConvertor
{
public:
	CWaveConvertor(void);
public:
	~CWaveConvertor(void);
public:
	// inline void swap (complex<double> &a, complex<double> &b);
	//void bitrp (complex<double> Tdata[], int n);//反序
	//
	//void FFT(complex<double> x[], int n);
	//
	//void IFFT(complex<double> x[], int n);

	//实现信号的前向Fourier变换
	static void ConvertToFFT(
		unsigned int nSamples,      //样本数量
		unsigned int nShorts,       //短时点数
		const double* pRealIn,      //输入信号的实部
		double* pRealOut,           //输出的实部
		double* pImageOut           //输出的虚部
		);
	
	// 实现信号的逆向Fourier变换
	static void ConvertToRFT(
		unsigned int nSamples,		// 样本数量
		unsigned int nShorts,		// 短时点数
		const double* pRealIn,		// 输入信号
		double* pRealOut,			// 输出信号的实部
		double* pImageOut);			// 输出信号的虚部

	// 获取输入信号的功率谱
	static void ConvertToPowerSpectral(
		unsigned int nSamples,		// 样本数量
		unsigned int nShorts,		// 短时点数
		const double* pRealIn,		// 输入信号
		double* pDataOut			// 输出的功率谱
		);

	//获取输入信号的频谱(实部)
	//static void ConvertToSpectral( 
	//	unsigned int nSamples,		// 样本数量
	//	unsigned int nShorts,		// 短时点数
	//	const double* pRealIn,		// 输入信号
	//	double* pDataOut			// 输出频谱的实部
	//	);

	// 获取输入信号的对数功率谱
	static void ConvertToLogPowerSpectral(
		unsigned int nSamples,		// 样本数量
		unsigned int nShorts,		// 短时点数
		const double* pRealIn,		// 输入信号
		double* pDataOut			// 输出的对数功率谱
		);

	// 获取输入信号的倒谱
	static void ConvertToCepStrum(
		unsigned int nSamples,		// 样本数量
		unsigned int nShorts,		// 短时点数
		const double* pRealIn,		// 输入信号的
		double* pDataOut			// 输出的倒谱
		);
		// 对输入的8位信号，转化为double类型序列
	static void ConvertToDoubleMono(
		const byte* pDataIn,		// 输入样本序列
		unsigned int nCount,		// 样本数量
		double* pDataOut			// 输出double类型序列
		);

	// 对输入的16位信号，转化为double类型序列
	static void ConvertToDoubleMono(
		const int* pDataIn,				// 输入样本序列
		unsigned int nCount,		// 样本数量
		double* pDataOut			// 输出double类型序列
		);

	// 对输入的8位信号，转化为双声道double序列
	static void ConvertToDoubleStereo(
		const byte* pDataIn,				// 输入样本序列
		unsigned int nCount,		// 样本数量
		double* pDataOutLeft,		// 输出左声道double类型序列
		double* pDataOutRight		// 输出右声道double类型序列
		);

	// 对输入的16位信号，转化为双声道double序列
	static void ConvertToDoubleStereo(
		const int* pDataIn,				// 输入样本序列
		unsigned int nCount,		// 样本数量
		double* pDataOutLeft,		// 输出左声道double类型序列
		double* pDataOutRight		// 输出右声道double类型序列
		);

};
#endif /*_CWAVE_CONVERTOR_H_*/