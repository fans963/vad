#include "StdAfx.h"
#include "WaveConvertor.h"

#include <math.h>
#include <complex>
#include<iomanip>
#define PI 3.14159
#include <iostream>
#include <fstream>

using namespace std;
//////////////////////////////////////////////////////////////////////////
// class CWaveConvertor
//
// 功能：	实现裸音频数据的FFT变换
// 创建人：
// 创建日期:
// 修改人：
// 修改日期：
// 版本

///////////////////////////////////////////////////////////////////////////
//class CWaveConvertor// 负责实现音频裸数据的各种转换
//构造函数
CWaveConvertor::CWaveConvertor(void)
{
}
//析构函数
CWaveConvertor::~CWaveConvertor(void)
{
}
inline void swap (complex<double> &a, complex<double> &b)
{
     complex<double> t;
     t = a;
     a = b;
     b = t;
}

////////////////////////////////////////////////////////////////反序
void bitrp (complex<double> Tdata[], int n)
{
     // 位反转置换 Bit-reversal Permutation
     int i, j, a, b, p;
     for (i = 1, p = 0; i < n; i *= 2)
	{
         p ++;							//统计输入数据阶次
	}
     for (i = 0; i < n; i ++)
	{
         a = i;
         b = 0;
         for (j = 0; j < p; j ++)
		{
             b = (b << 1) + (a & 1);     // b左移一位，a最高位移至最左边
             a >>= 1;					 // a左移一位
		}
         if ( b > i)
		{
			swap(Tdata[b],Tdata[i]);
		}
	}
	 ////////////////////////////////////////////////////反序的另一种方法
	/* int LH=n/2,j=LH,n1=n-2;
	 int i,k;
     for (i = 1; i < n1; i++)
	{
       
		 if(i<j)
		 {
            swap(Tdata[j],Tdata[i]);
		 }
			
		 k=LH;
		 while(j>=k)
		 {
			 j=j-k;
			 k=k/2;
		 }
	j=j+k;
	}*/
}
////////////////////////////////////////////////////傅里叶变换
bool FFT(complex<double> x[], int n)
{
	double angle;
	int i,l,j,k;
	int count=0;
	int B=1;
	int P;
	complex<double> temp;
	complex<double> *weight;
	weight=new complex<double>[n/2];
	i=n;
	while (i > 1)
	{
         if (i % 2)
		{
             return false;
		}
         i /= 2;
	}
	//weight[0]=complex<double>(1.0,0);
	for (i=0;i<n/2;i++)
	{
		//angle=-(i-1)*PI*2/n;//与IFFT区别多个负号
		angle=-i*PI*2/n;//与IFFT区别多个负号
		weight[i]=complex<double>(cos(angle),sin(angle));//计算旋转因子
	}

	bitrp(x,n);		//对输入进行倒序操作

	//ofstream fout1("after_bitrp_my.txt",ios::trunc);
	// for(i=0;i<n;i++)
	//	 fout1<<x[i].real()<<endl;
	// fout1.close();
	for (i=1;i<n;i<<=1)
	{
		count++;
	}
	int p=n;
	for (l=1;l<=count;l++)		//L级运算
	{
		p/=2;
		for (j=0;j<=B-1;j++)	//每个蝶形单元的两个输入数据相距B=2^(L-1)个点
		{
			P=p*j;//P=j*2的B-L方，n=2的B次方
			for (k=j;k<=n-1;k+=(B*2))
			{
				temp=x[k];
				x[k]=x[k]+x[k+B]*weight[P];//比IFFT少乘个0.5
				x[k+B]=temp-x[k+B]*weight[P];//比IFFT少乘个0.5
			}
		}
		B*=2;
	}
	delete []weight;
	return true;
}
////////////////////////////////////////////////////////逆傅里叶变换

bool IFFT(complex<double> x[], int n)
{
	double angle;
	int i,l,j,k;
	int count=0;
	int B=1;
	int P;
	complex<double> temp;
	complex<double> *weight;
	weight=new complex<double>[n/2];
	i=n;
	while (i > 1)
	{
         if (i % 2)
		{
             return false;
		}
         i /= 2;
	}

	for (i=0;i<n/2;i++)
	{
		angle=i*PI*2/n;
		weight[i]=complex<double>(cos(angle),sin(angle));
	}

	bitrp(x,n);		//对输入进行倒序操作

	for (i=1;i<n;i<<=1)
	{
		count++;
	}
	int p=n;
	for (l=1;l<=count;l++)		//L级运算
	{
		p/=2;
		for (j=0;j<=B-1;j++)	//每个蝶形单元的两个输入数据相距B=2^(L-1)个点
		{
			P=p*j;
			for (k=j;k<=n-1;k+=(B*2))
			{
				temp=x[k];
				x[k]=x[k]*0.5+x[k+B]*weight[P]*0.5;//IFFT本来最后要乘以1/n，为了防止溢出，可在每一级运算中，
				x[k+B]=temp*0.5-x[k+B]*weight[P]*0.5;//输出之路分别乘以1/2，实现系数的分组担忧
			}
		}
		B*=2;
	}
	delete []weight;
	return true;
}
//////////////////////////////////////////////////////////////////////////
//// 对输入数据进行短时快速FFT变换，输入数据为已经加窗的数据
void CWaveConvertor::ConvertToFFT( 
								 unsigned int nSamples,		// 样本数量
								 unsigned int nShorts,		// 短时点数
								 const double* pRealIn,		// 输入的实部，不可为空
								 double* pRealOut,			// 输出的实部
								 double* pImageOut			// 输出的虚部
								 )		
{   int i,k;
    
	complex<double>* pDataOut,*temp;
	int nCount=nSamples/nShorts;
	pDataOut=new complex<double>[nCount*nShorts];
	temp=new complex<double>[nShorts];
	for(k=0;k<nCount;k++)
	{   memset(temp, 0, sizeof(double) *nShorts);
		for(i=0;i<nShorts;i++)
     		temp[i]=complex<double>(pRealIn[k*nShorts+i],0);
		/*for(i=nShorts;i<nWinSize;i++)
			temp[i]=complex<double>(0,0);*/
	    FFT(temp,nShorts);
		for(i=0;i<nShorts;i++)
			pDataOut[k*nShorts+i]=temp[i];
	}
	// 初始化输出数据
	memset(pRealOut, 0, sizeof(double) * nCount*nShorts);
	memset(pImageOut, 0, sizeof(double) * nCount*nShorts);
  
	//ofstream fout( "fft_my.txt", ios::trunc );
  
     
	for (i = 0; i < nCount*nShorts; i++)
	{
		pRealOut[i]=pDataOut[i].real();
		//fout<<pDataOut[i].real()<<" + "<<pDataOut[i].imag()<<"i"<<endl;
		pImageOut[i]=pDataOut[i].imag();
	} 
	//关闭输出流 
      //fout.close();
}

////////////////////////////////////////////////////////////////////////////
//// 对输入数据进行Reverse Fourier转化，输入数据为已经加窗的数据
void CWaveConvertor::ConvertToRFT( 
								  unsigned int nSamples,		// 样本数量nCount*1024
								  unsigned int nShorts,			// 短时点数1024
								  const double* pRealIn,		// 输入的实部，不可为空
								  double* pRealOut,				// 输出的实部
								  double* pImageOut				// 输出的虚部
								  )	
{
	int i,k;
    
	complex<double>* pDataOut,*temp;
	int nCount=nSamples/nShorts;
	pDataOut=new complex<double>[nSamples];
	temp=new complex<double>[nShorts];
	for(k=0;k<nCount;k++)
	{   memset(temp, 0, sizeof(double) *nShorts);
		for(i=0;i<nShorts;i++)
     		temp[i]=complex<double>(pRealIn[k*nShorts+i],0);
	    IFFT(temp,nShorts);
		for(i=0;i<nShorts;i++)
			pDataOut[k*nShorts+i]=temp[i];
	}
	
	// 初始化输出数据
	memset(pRealOut, 0, sizeof(double) * nSamples);
	memset(pImageOut, 0, sizeof(double) * nSamples);
   
	for (i = 0; i < nSamples; i++)
	{
		pRealOut[i]=pDataOut[i].real();
		pImageOut[i]=pDataOut[i].imag();

		
	}
}

//////////////////////////////////////////////////////////////////////////
// 获取输入信号的功率谱
void CWaveConvertor::ConvertToPowerSpectral( 
	unsigned int nSamples,			// 样本数量
	unsigned int nShorts,		// 短时点数
	const double* pRealIn,			// 输入信号
	double* pDataOut				// 输出的功率谱
	)
{   int nCount=nSamples/nShorts;
	double* pRealOut = NULL;
	double* pImageOut = NULL;

	// 输入数据是否有效
	if (nSamples > 0 && pRealIn != NULL)
	{
		// 为FFT输出数据分配空间
		pRealOut = new double[nCount*nShorts];
		pImageOut = new double[nCount*nShorts];

		// 进行FFT变换
		CWaveConvertor::ConvertToFFT(nSamples, nShorts, pRealIn, pRealOut, pImageOut);
		for (unsigned int i = 0; i < nCount*nShorts; i++)
		{  
			pDataOut[i] =sqrt(pRealOut[i] * pRealOut[i] + pImageOut[i] * pImageOut[i]);
		}
		delete[] pRealOut;
		delete[] pImageOut;
	}
}

//获取输入信号的频谱(实部)
//void CWaveConvertor::ConvertToSpectral( 
//	unsigned int nSamples,			// 样本数量
//	unsigned int nShorts,			// 短时点数
//	const double* pRealIn,			// 输入信号
//	double* pDataOut				// 输出频谱的实部
//	)
//{   
//	//int nCount=nSamples/nShorts;
//	//double* pRealOut = NULL;
//	double* pImageOut = NULL;
//
//	// 输入数据是否有效
//	if (nSamples > 0 && pRealIn != NULL)
//	{
//		// 为FFT输出数据分配空间
//		//pRealOut = new double[nCount*nShorts];
//		//pImageOut = new double[nCount*nShorts];
//		pImageOut = new double[nSamples];
//
//
//		// 进行FFT变换
//		//CWaveConvertor::ConvertToFFT(nSamples, nShorts, pRealIn, pRealOut, pImageOut);
//		CWaveConvertor::ConvertToFFT(nSamples, nShorts, pRealIn, pDataOut, pImageOut);
//
//		//delete[] pRealOut;
//		delete[] pImageOut;
//	}
//}

//////////////////////////////////////////////////////////////////////////
// 获取输入信号的对数功率谱
void CWaveConvertor::ConvertToLogPowerSpectral( 
	unsigned int nSamples,			// 样本数量
	unsigned int nShorts,			// 短时点数
	const double* pRealIn,			// 输入信号
	double* pDataOut				// 输出的对数功率谱
	)
{
	int nCount=nSamples/nShorts;
	// 先转换为功率谱
	CWaveConvertor::ConvertToPowerSpectral(nSamples, nShorts, pRealIn, pDataOut);

	// 转化为对数功率谱	
	for (unsigned int i = 0; i < nCount*nShorts; i++)
	{
		if (pDataOut[i]<1)
		{
			pDataOut[i]=1;
		}
		pDataOut[i] = log(pDataOut[i]);
	} 
}

//////////////////////////////////////////////////////////////////////////
// 获取输入信号的倒谱
void CWaveConvertor::ConvertToCepStrum( 
									   unsigned int nSamples,			// 样本数量
									   unsigned int nShorts,		// 短时点数
									   const double* pRealIn,			// 输入信号
									   double* pDataOut					// 输出的倒谱
									   )
{
	int nCount=nSamples/nShorts;
	// 保存对数功率谱
	double* pTemp = new double[nCount*nShorts];
	// 保存虚部数据
	double* pImageOut = new double[nCount*nShorts];
    memset(pImageOut, 0, sizeof(double) * nCount*nShorts);
	// 获取对数功率谱
	CWaveConvertor::ConvertToLogPowerSpectral(nSamples, nShorts, pRealIn, pTemp);

	// 再次进行傅立叶逆变换，得到倒谱
	CWaveConvertor::ConvertToRFT(nCount*nShorts,nShorts, pTemp, pDataOut, pImageOut);
	
	delete[] pTemp;
	delete[] pImageOut;
}
//////////////////////////////////////////////////////////////////////////
// 对输入的8位信号，转化为double类型序列
void CWaveConvertor::ConvertToDoubleMono( 
										const byte* pDataIn,				// 输入样本序列
										unsigned int nCount,		// 样本数量
										double* pDataOut			// 输出double类型序列
										)
{
	int nAdjust = 127;

	if (pDataIn != NULL)
	{
		// 填零
		memset(pDataOut, 0, sizeof(double) * nCount);

		// 8位信号减127
		for (unsigned int i = 0; i < nCount; i++)
		{
			pDataOut[i] = pDataIn[i] - nAdjust;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 对输入的16位信号，转化为double类型序列
void CWaveConvertor::ConvertToDoubleMono( 
										const int* pDataIn,				// 输入样本序列
										unsigned int nCount,		// 样本数量
										double* pDataOut			// 输出double类型序列
										)
{
	char* pDataTemp = NULL;
	unsigned int nOffset = 0;

	if (pDataIn != NULL)
	{
		pDataTemp = (char*) pDataIn;
		// 填零
		nCount /= 2;
		memset(pDataOut, 0, sizeof(double) * nCount);

		for (unsigned int i = 0; i < nCount; i++)
		{
			nOffset = i * 2;

			// 对高低位倒置的16位信号进行计算
			pDataOut[i] = pDataTemp[nOffset] + pDataTemp[nOffset + 1] * 0x100;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 对输入的8位信号，转化为双声道double序列
void CWaveConvertor::ConvertToDoubleStereo(
										const byte* pDataIn,				// 输入样本序列
										unsigned int nCount,		// 样本数量
										double* pDataOutLeft,		// 输出左声道double类型序列
										double* pDataOutRight		// 输出右声道double类型序列
										)
{
	double* pDataTemp = NULL;

	if (pDataIn != NULL)
	{
		pDataTemp = new double[nCount];

		// 信号类型先转化为double类型
		CWaveConvertor::ConvertToDoubleMono(pDataIn, nCount, pDataTemp);

		//提取左右声道
		for (unsigned int i = 0; i < nCount; i += 2)
		{
			pDataOutLeft[i / 2] = pDataTemp[i];
			pDataOutRight[i / 2] = pDataTemp[i + 1];
		}

		delete[] pDataTemp;
	}
}

// 对输入的16位信号，转化为双声道double序列
void CWaveConvertor::ConvertToDoubleStereo(
										const int* pDataIn,				// 输入样本序列
										unsigned int nCount,		// 样本数量
										double* pDataOutLeft,		// 输出左声道double类型序列
										double* pDataOutRight		// 输出右声道double类型序列
										)
{
	double* pDataTemp = NULL;

	if (pDataIn != NULL)
	{
		pDataTemp = new double[nCount];

		// 信号类型先转化为double类型
		CWaveConvertor::ConvertToDoubleMono(pDataIn, nCount, pDataTemp);

		//提取左右声道
		for (unsigned int i = 0; i < nCount; i += 2)
		{
			pDataOutLeft[i / 2] = pDataTemp[i];
			pDataOutRight[i / 2] = pDataTemp[i + 1];
		}

		delete[] pDataTemp;
	}
}
