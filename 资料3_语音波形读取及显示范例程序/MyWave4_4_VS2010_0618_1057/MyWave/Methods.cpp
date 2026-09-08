#include "StdAfx.h"
#include "Methods.h"
#include "Use.h"
//#include "AudioAnalysis.h"
//#include "AudioAnalysisDoc.h"
#include <math.h>
#include <fstream>
#include<iostream>
#include<complex>
#include<algorithm>

#define PI 3.1415926
#define factor 0.95 //预加重因子


Methods::Methods(void)
{
	frame_LEN=512;
}

Methods::~Methods(void)
{
}

//保存文件,一维数组
void  Methods::Save1(
					 std::vector<float>& file_data//文件数据
					 )
{
	CFileDialog dlg(false, NULL, L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"文件（*.txt）|*.txt|所有文件(*.*)|*.*||");

	if(dlg.DoModal()==IDOK)
	{
		CFileException fileException;
		//获取路径
		CString m_csFileName=dlg.GetPathName();
		CString FilePath=dlg.GetPathName ();
		FilePath+=_T(".txt");
 		m_csFileName+=_T(".txt");
	  
		std::ofstream fout( m_csFileName, std::ios::app );	
		
		for (int i = 0; i<(int)file_data.size(); i++)
		{			
			fout<<file_data.at(i)<<"\t";
		}
		//关闭输出流 
		fout.close();
	}
}
//保存文件,二维数组
void  Methods::Save2(
					 std::vector<std::vector<float> >& file_data//文件数据
					 )
{
	CFileDialog dlg(false, NULL, L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"文件（*.txt）|*.txt|所有文件(*.*)|*.*||");

	if(dlg.DoModal()==IDOK)
	{
		CFileException fileException;
		//获取路径
		CString m_csFileName=dlg.GetPathName();
		CString FilePath=dlg.GetPathName ();
		FilePath+=_T(".txt");
 		m_csFileName+=_T(".txt");
	  
		std::ofstream fout( m_csFileName, std::ios::app );	
		fout<<(int)file_data.size()<<std::endl;
		for (int i = 0; i<(int)file_data.size(); i++)
		{	
			//fout<<i<<std::endl;
			for(int j=0;j<(int)file_data[i].size();j++)
			{
				fout<<file_data[i][j]<<"\t";
			}
			fout<<std::endl;
		/*	fout<<"***************************************************************************"<<std::endl;
			fout<<std::endl;*/
		}
		//关闭输出流 
		fout.close();
	}
}
//预加重
void Methods::Pre_emphasis(
						   std::vector<float>& pre_emphasis_In,
						   std::vector<float>& pre_emphasis_Out
		                   )
{
	int len=(int)pre_emphasis_In.size();
	pre_emphasis_Out.push_back(pre_emphasis_In.at(0));
	for(int i=1;i<len;i++)
	{
		pre_emphasis_Out.push_back(pre_emphasis_In.at(i)-(float)factor*pre_emphasis_In.at(i-1));
	}

}
//分帧
void Methods::Frameset(
					   int len,//数据长度
					   int frame_len,//帧长
					   int frame_tran,//帧移
					   int n_frame,//每个文件所分的帧数
					   std::vector<float>& frame_In,//输入数据
					   std::vector<std::vector<float> >& frame_Out//输出数据
					   )
{
	len=(int)frame_In.size();
	n_frame=(len-frame_len)/(frame_len/2)+1;
	std::vector<float> vec;
	frame_Out.clear();
	for(int i=0;i<n_frame;i++)
	{
		for(int j=0;j<frame_len;j++)
		{			
			if(i*frame_tran+j<len)
				vec.push_back(frame_In.at(i*frame_tran+j));			
			else
				vec.push_back(0);
		}
		frame_Out.push_back(vec);
		vec.clear();
	}	
}

//汉明窗				
void Methods::HammingWindow(
							int n_frame,//每个文件所分的帧数
							int lenth,//窗长
							std::vector<std::vector<float> >& hamming_In,//输入数据
							std::vector<std::vector<float> >& hamming_Out//输出数据
							)
{
	std::vector<float> vec;
	hamming_Out.clear();
	if(hamming_In.size()!=0)
	{
		for(int i=0;i<n_frame&&i<(int)hamming_In.size();i++)
		{
			if(hamming_In[i].size()!=0)
			{
				for(int j=0;j<lenth&&j<(int)hamming_In[i].size();j++)
				{
					float a=hamming_In[i].at(j);
					vec.push_back(float(0.54-0.46*cos(2*PI*j/(lenth-1)))*a);

				}
			}
			hamming_Out.push_back(vec);
			vec.clear();
		}	
	}
}

//矩形窗
void Methods::RectWindow(
						 int n_frame,//每个文件所分的帧数
						 int lenth,//窗长
						 std::vector<std::vector<float> >& rect_In,//输入数据
						 std::vector<std::vector<float> >& rect_Out//输出数据)//矩形窗
						 )
{
	std::vector<float> vec;
	rect_Out.clear();
	if(rect_In.size()!=0)
	{
		for(int i=0;i<n_frame&&i<(int)rect_In.size();i++)
		{
			if(rect_In[i].size()!=0)
			{
				for(int j=0;j<lenth;j++)
				{
					vec.push_back(rect_In[i].at(j));
				}
				rect_Out.push_back(vec);
				vec.clear();
			}
		}
	}
}

//快速傅里叶变换,单帧
void Methods::FFT(
				  int M,
				  int N,
				  std::vector<float>& fft_In,//fft转换前
				  std::complex<float>* &X//fft转换后
				 // std::vector<float>& fft_Out//平方开根号
				  )
{ 
	int L,B,J,P,K,n;
	std::complex<float> W;
	std::complex<float> temp;
	//std::complex<float> *X;
	float *data;
	/*M=(int)ceil(log((double) fft_In.size())/log (2.0));
	N=(int)pow(2.0,M);*/
	/*M=9;
	N=512;*/
	//X=(std::complex<float> *)malloc(sizeof(std::complex<float>)*N);
	data=(float *)malloc(sizeof(float)*N);

	for(n=0;n<N;n++)
	{
		if(n<(int)fft_In.size())
		{
			data[n]=fft_In[n];
		}else
		{
			data[n]=0;
		}
	}
		
	//Reverse
	int I,N1;
	float T;
	n=N/2;
	J=n;
	N1=N-2;
	for(I=1;I<=N1;I++)
	{
		if(I<J)
		{
			T=data[I];
			data[I]=data[J];
			data[J]=T;
		}
		
		K=n;
		while(J>=K)
		{
			J=J-K;
			K=K/2;
		}
		J=J+K;			
	}	
	for(int i=0;i<N;i++)
	{
		X[i]=std::complex<float>(data[i],.0);
	}
//蝶形运算
	for(L=1;L<=M;L++)
	{
		B=(int)pow(2.0,L-1);
		for(J=0;J<=B-1;J++)
		{
			P=int(J*pow(2.0,M-L));
			W=exp(std::complex<float>(0,-2*PI*P/N));

			for(K=J;K<=N-1;K=K+(int)pow(2.0,L))
			{
				temp=X[K];
				X[K]=X[K]+X[K+B]*W;
				X[K+B]=temp-X[K+B]*W;				
			}
		}
	}
	/*for(int i=0;i<N;i++)
	{
		fft_Out.push_back(sqrt(pow(std::real(X[i]),2)+pow(std::imag(X[i]),2)));
	}*/
}
//快速傅里叶变换,整个文件
void Methods::QFFT(
				int len,//数据长度
				int frame_len,//帧长
				int frame_tran,//帧移
				int n_frame,//每个文件所分的帧数
				std::vector<float>& frame_In,//输入数据
				int lenth,//窗长
				std::vector<std::vector<float> >& Qfft_Out//输出数据
				)
{
	int M,N;	
	std::complex<float> *X;	
	std::vector<float> pre_emphasis_Out;
	std::vector<std::vector<float> > frame_Out;
	std::vector<std::vector<float> > hamming_Out;
	std::vector<float> fft_Out;
	//len=(int)frame_In.size();
	//n_frame=(len-512+256)*2/256;
	frame_Out.clear();
	hamming_Out.clear();
	fft_Out.clear();
	Pre_emphasis(frame_In,pre_emphasis_Out);
	Frameset(len,frame_len,frame_tran,n_frame,pre_emphasis_Out,frame_Out);
	HammingWindow(n_frame,lenth,frame_Out,hamming_Out);
	//Save2(hamming_Out);
	for(int i=0;i<n_frame&&i<(int)hamming_Out.size();i++)
	{
		M=(int)ceil(log((double) hamming_Out[i].size())/log (2.0));
		N=(int)pow(2.0,M);
		X=(std::complex<float> *)malloc(sizeof(std::complex<float>)*N);
		FFT(M,N,hamming_Out.at(i),X);
		for(int j=0;j<N;j++)
		{
			fft_Out.push_back(sqrt(pow(std::real(X[j]),2)+pow(std::imag(X[j]),2)));
		}
		Qfft_Out.push_back(fft_Out);
		fft_Out.clear();

		delete []X;
	}
}

//逆傅里叶变换,单帧
void Methods::IFFT(
				   int M,
				   int N,
				   std::complex<float>* &X
				   )
{
	std::complex<float> T;
	int K,I,J,N1,n;
	//N=512;
	//M=9;
	n=N/2;
	J=n;
	N1=N-2;
	for(I=1;I<=N1;I++)
	{
		if(I<J)
		{
			T=X[I];
			X[I]=X[J];
			X[J]=T;
		}
		
		K=n;
		while(J>=K)
		{
			J=J-K;
			K=K/2;
		}
		J=J+K;		
	}
	int B,P;
	std::complex<float> W;
	std::complex<float> temp;
	//蝶形运算
	for(int L=1;L<=M;L++)
	{
		B=(int)pow(2.0,L-1);
		for(J=0;J<=B-1;J++)
		{
			P=J*(int)pow(2.0,M-L);
			W=exp(std::complex<float>(0,2*PI*P/N));

			for(K=J;K<=N-1;K=K+(int)pow(2.0,L))
			{
				temp=X[K];
				X[K]=X[K]+X[K+B]*W;
				X[K+B]=temp-X[K+B]*W;				
			}
		}
	}
	for(int i=0;i<N;i++)
	{
		X[i]/=N;
	}	
}
//逆傅里叶变换，整个文件
void Methods::QIFFT(
					int len,//数据长度
					int frame_len,//帧长
					int frame_tran,//帧移
					int n_frame,//每个文件所分的帧数
					std::vector<float>& frame_In,//输入数据
					int lenth,//窗长
					std::vector<std::vector<float> >& Qifft_Out//输出数据
					)
{
	int M,N;
	float a;
	std::complex<float> *X;	
	/*std::complex<float> *Y;
	std::complex<float> *Z;*/
	std::vector<float> vec;
	//len=(int)frame_In.size();
	//n_frame=(len-512+256)*2/256;
	std::vector<float> pre_emphasis_Out;
	std::vector<std::vector<float> > frame_Out;
	std::vector<std::vector<float> > hamming_Out;

	
	pre_emphasis_Out.clear();
	frame_Out.clear();
	hamming_Out.clear();

	Pre_emphasis(frame_In,pre_emphasis_Out);
	//Save1(pre_emphasis_Out);
	Frameset(len,frame_len,frame_tran,n_frame,pre_emphasis_Out,frame_Out);
	//Frameset(len,frame_len,frame_tran,n_frame,frame_In,frame_Out);
	
	HammingWindow(n_frame,lenth,frame_Out,hamming_Out);
	//Save2(hamming_Out);
	for(int i=0;i<n_frame&&i<(int)hamming_Out.size();i++)
	{
		M=(int)ceil(log((double) hamming_Out[i].size())/log (2.0));
		N=(int)pow(2.0,M);
		X=(std::complex<float> *)malloc(sizeof(std::complex<float>)*N);
		/*Y=(std::complex<float> *)malloc(sizeof(std::complex<float>)*N);
		Z=(std::complex<float> *)malloc(sizeof(std::complex<float>)*N);*/
		FFT(M,N,hamming_Out.at(i),X);//X存放fft转换后的数据
		vec.clear();
		vec.push_back(log(sqrt(pow(std::real(X[1]),2)+pow(std::imag(X[1]),2))));
		for(int j=1;j<N;j++)
		{
			vec.push_back(log(sqrt(pow(std::real(X[j]),2)+pow(std::imag(X[j]),2))));//功率谱取对数
		}
		/*FFT(M,N,vec,Y);
		vec.clear();
		for(int j=0;j<N;j++)
		{
			vec.push_back(sqrt(pow(std::real(Y[j]),2)+pow(std::imag(Y[j]),2)));
		}
		FFT(M,N,vec,Z);
		vec.clear();
		for(int j=0;j<N;j++)
		{
			vec.push_back(std::real(Z[j]));
		}*/
		for(int j=0;j<N;j++)
		{
			X[j]=std::complex<float>(vec.at(j),0);//容器重新来存放对数功率谱X
		}
		vec.clear();//清空vec
		IFFT(M,N,X);//再对功率谱的对数求逆傅立叶变换		
		for(int m=0;m<N;m++)
		{
			//a=std::real(X[m]);//放大10000倍
			a=sqrt(pow(std::real(X[m]),2)+pow(std::imag(X[m]),2));
			vec.push_back(a);//再次启用vec容器
		}
		Qifft_Out.push_back(vec);//将vec加入容器Qifft_Out
		vec.clear();

		delete []X;
	}
}
//三角滤波器组
void Methods::Triangle_filter(
							  std::vector<std::vector<float> >& Triangle_Out,//输出数据
							  int frame_len//帧长
							  )
{
	int fh=16000;//采样频率16000hz
	int M=24,N=frame_len;
	int F=fh/N;
	float f[26],f_mel,delt_f_mel,x=0;	
	std::vector<float> vec;
	f_mel=log10((float)fh/2/700+1)*2595;
	delt_f_mel=f_mel/(M+1);
	for(int i=0;i<M+2;i++)
	{
		f[i]=floor((pow(10,(double)delt_f_mel*i/2595)-1)*700/fh*N)+1;
	}
	f[M+1]=N/2;
	for(int m=1;m<25;m++)
	{ 
		for(int k=1;k<=N;k++)
		{		
			//x=F*k;
			if((f[m-1]<=k)&&(k<=f[m]))
			{
				//x=f[m-1];
				vec.push_back((k-f[m-1])/(f[m]-f[m-1]));
			}
			else if((f[m]<=k)&&(k<=f[m+1]))
			{
				//x=f[m];
				vec.push_back((f[m+1]-k)/(f[m+1]-f[m]));
			}
			else
				vec.push_back(0);
		}
		Triangle_Out.push_back(vec);
		vec.clear();
	}
	//Save2(Triangle_Out);
}

void Methods::Cos(std::vector<std::vector<float> > &Cos_Out)
{
	std::vector<float> vec;
	for(int k=1;k<=13;k++)
	{
		float z=0;
		for(int j=1;j<=24;j++)
		{
			z=(float)cos((2*j-1)*k*PI/(2*24));
			vec.push_back(z);
		}
		Cos_Out.push_back(vec);
		vec.clear();
	}
}

//mel频率倒谱系数
void Methods::MFCC(
				   int len,//数据长度
				   int frame_len,//帧长
				   int frame_tran,//帧移
				   int n_frame,//每个文件所分的帧数
				   std::vector<float>& frame_In,//输入数据
				   int lenth,//窗长
				   std::vector<std::vector<float> >& mfcc_Out//输出数据
				   )
{
	//int M,N;
	//float a;
	//std::complex<float> *X;		
	std::vector<float> vec1;
	std::vector<float> vec2;
	std::vector<std::vector<float> > Triangle_Out;//三角滤波
	std::vector<float> S;//对数能量
	//std::vector<float> pre_emphasis_Out;
	std::vector<std::vector<float> > frame_Out;
	std::vector<std::vector<float> > hamming_Out;
	std::vector<std::vector<float> > FFT_Out;
	std::vector<std::vector<float> > Cos_Out;

	//vec1.clear();
	//vec2.clear();
	//Triangle_Out.clear();
	//pre_emphasis_Out.clear();
	//frame_Out.clear();
	//hamming_Out.clear();

	Triangle_filter(Triangle_Out,frame_len);
	Cos(Cos_Out);

	//Pre_emphasis(frame_In,pre_emphasis_Out);
	QFFT(len,frame_len,frame_tran,n_frame,frame_In,lenth,FFT_Out);
	//Save2(FFT_Out);
	
	for(int i=0;i<n_frame&&i<(int)FFT_Out.size();i++)
	{
		for(int m=0;m<24;m++)
		{
			float z=0;
			for(int j=0;j<(int)Triangle_Out[m].size()&&j<(int)FFT_Out[i].size();j++)
			{
				
				z+=FFT_Out[i].at(j)*Triangle_Out[m].at(j);
				//vec2.push_back(vec1.at(j)*triangle_data[m][j]);
			}
			z=log(z*z);
			S.push_back(z);
		}
		//S.push_back(vec2);
		//vec1.clear();
		//vec2.clear();
		for(int k=1;k<=13;k++)
		{
			float z=0;
			for(int j=1;j<=24;j++)
			{
				z+=S.at(j-1)*Cos_Out[k-1].at(j-1);
			}
			//z=sqrt((float)2/256)*z;
			vec1.push_back(z);
		}
		mfcc_Out.push_back(vec1);
		vec1.clear();
		S.clear();
	}
	//Save2(Triangle_Out);
}

//自相关函数
void Methods::AutoCorrelation(
							  int P,//LPC分析的阶数
							  std::vector<std::vector<float> >& auto_In,//输入数据
							  std::vector<std::vector<float> >& auto_Out//输出数据
							  )
{
	float u0,u;
	std::vector<float> vec;
	auto_Out.clear();
	vec.clear();
	P=12;
	
	for(int i=0;i<(int)auto_In.size();i++)
	{		
		u0=0.0;
		for(int j=0;j<(int)auto_In[i].size();j++)
			u0+=auto_In[i].at(j)*auto_In[i].at(j);
		if(u0>0)
		{
			for(int p=0;p<=P;p++)
			{
				u=0.0;
				for(int j=0;j<(int)auto_In[i].size()-p;j++)
				{
					u+=auto_In[i].at(j)*auto_In[i].at(j+p);
				}
				vec.push_back(u/u0);				
			}
		}
		else
		{
			for(int p=0;p<=P;p++)
			{
				vec.push_back(0);
			}
		}
		auto_Out.push_back(vec);
		vec.clear();		
	}	
}

//杜宾算法,单帧
void Methods::Durbin(
					 int P,//LPC分析的阶数
					 std::vector<float>& durbin_In,//输入数据
					 std::vector<float>& durbin_Out//输出数据
					 )
{
	std::vector<std::vector<float> > A;//Apk
	std::vector<float> variance;//方差
	std::vector<float> vec;
	
	durbin_Out.push_back(1);//a0
	variance.push_back(durbin_In.at(0));//ψ0

	durbin_Out.push_back(-durbin_In.at(1)/durbin_In.at(0));//a11
	variance.push_back((1-durbin_Out.at(1)*durbin_Out.at(1))*durbin_In.at(0));//ψ1

	vec.push_back(durbin_Out.at(1));
	A.push_back(vec);

	for(int p=2;p<=P;p++)
	{
		float a=0.0;
		vec.clear();
		for(int k=1;k<=p-1;k++)
		{
			a+=A[p-2].at(k-1)*durbin_In.at(p-k);		
		}
		a=-(a+durbin_In.at(p))/variance.at(p-1);
		for(int k=1;k<=p-1;k++)
		{
			vec.push_back(A[p-2].at(k-1)+a*A[p-2].at(p-k-1));
		}
		vec.push_back(a);
		A.push_back(vec);

		variance.push_back((1-a*a)*variance.at(p-1));
		durbin_Out.push_back(a);
	}
}

//LPC系数
void Methods::LPC(
				  int len,//数据长度
				  int frame_len,//帧长
				  int frame_tran,//帧移
				  int n_frame,//每个文件所分的帧数
				  std::vector<float>& frame_In,//输入数据
				  int lenth,//窗长
				  int P,//LPC分析的阶数
				  std::vector<std::vector<float> >& LPC_Out//输出数据
				  )
{
	std::vector<std::vector<float> > frame_Out;
	std::vector<std::vector<float> > hamming_Out;
	std::vector<std::vector<float> > auto_Out;
	std::vector<float> durbin_Out;

	frame_Out.clear();
	hamming_Out.clear();
	auto_Out.clear();
	durbin_Out.clear();

	Frameset(len,frame_len,frame_tran,n_frame,frame_In,frame_Out);
	HammingWindow(n_frame,lenth,frame_Out,hamming_Out);
	AutoCorrelation(P,hamming_Out,auto_Out);

	for(int i=0;i<n_frame&&i<(int)auto_Out.size();i++)
	{
		Durbin(P,auto_Out.at(i),durbin_Out);
		LPC_Out.push_back(durbin_Out);
		durbin_Out.clear();
	}
}

//短时能量
void Methods::Energy(
					 int len,//数据长度
					 int frame_len,//帧长
					 int frame_tran,//帧移
					 int n_frame,//每个文件所分的帧数		
					 int lenth,//窗长
					 std::vector<float>& energy_In,//输入数据
					 std::vector<float>& energy_Out//输出数据
					 )
{
	std::vector<std::vector<float> > frame_Out;
	std::vector<std::vector<float> > rect_Out;

	energy_Out.clear();

	Frameset(len,frame_len,frame_tran,n_frame,energy_In,frame_Out);//数据长度,帧长,帧移,每个文件所分的帧数,输入数据,输出数据
	RectWindow(n_frame,lenth,frame_Out,rect_Out);//每个文件所分的帧数,窗长,输入数据,输出数据

	for(int i=0;i<(int)rect_Out.size();i++)
	{
		float a=0;
		for(int j=0;j<(int)rect_Out[i].size();j++)
		{
			a+=rect_Out[i].at(j)*rect_Out[i].at(j);
		}
		energy_Out.push_back(a);
	}	
}

//短时平均幅度
void Methods::Magnitude(
					 int len,//数据长度
					 int frame_len,//帧长
					 int frame_tran,//帧移
					 int n_frame,//每个文件所分的帧数		
					 int lenth,//窗长
					 std::vector<float>& magnitude_In,//输入数据
					 std::vector<float>& magnitude_Out//输出数据
					 )
{
	std::vector<std::vector<float> > frame_Out;
	std::vector<std::vector<float> > rect_Out;

	magnitude_Out.clear();

	Frameset(len,frame_len,frame_tran,n_frame,magnitude_In,frame_Out);//数据长度,帧长,帧移,每个文件所分的帧数,输入数据,输出数据
	RectWindow(n_frame,lenth,frame_Out,rect_Out);//每个文件所分的帧数,窗长,输入数据,输出数据

	for(int i=0;i<(int)rect_Out.size();i++)
	{
		float a=0;
		for(int j=0;j<(int)rect_Out[i].size();j++)
		{
			a+=abs(rect_Out[i].at(j));
		}
		magnitude_Out.push_back(a);
	}	
}

//短时过零率
void Methods::Zero(
				   int len,//数据长度
				   int frame_len,//帧长
				   int frame_tran,//帧移
				   int n_frame,//每个文件所分的帧数		
				   int lenth,//窗长
				   std::vector<float>& zero_In,//输入数据
				   std::vector<float>& zero_Out//输出数据
				   )
{
	std::vector<std::vector<float> > frame_Out;
	std::vector<std::vector<float> > rect_Out;

	zero_Out.clear();

	Frameset(len,frame_len,frame_tran,n_frame,zero_In,frame_Out);//数据长度,帧长,帧移,每个文件所分的帧数,输入数据,输出数据
	RectWindow(n_frame,lenth,frame_Out,rect_Out);//每个文件所分的帧数,窗长,输入数据,输出数据

	for(int i=0;i<(int)rect_Out.size();i++)
	{
		float a=0;
		int sgn1=0,sgn2=0;
		for(int j=1;j<(int)rect_Out[i].size();j++)
		{
			if(rect_Out[i].at(j)>=0)
				sgn1=1;
			else
				sgn1=-1;
			if(rect_Out[i].at(j-1)>=0)
				sgn2=1;
			else
				sgn2=-1;
			a+=abs(sgn1-sgn2);		
		}
		a/=2;
		zero_Out.push_back(a);
	}	
}

//短时自相关
void Methods::Relevancy(
						int len,//数据长度
						int frame_len,//帧长
						int frame_tran,//帧移
						int n_frame,//每个文件所分的帧数		
						int lenth,//窗长
						int k,//短时相关最大延迟点数
						std::vector<float>& relevancy_In,//输入数据
						std::vector<std::vector<float> >& relevancy_Out//输出数据
						)
{
	std::vector<std::vector<float> > frame_Out;
	std::vector<std::vector<float> > rect_Out;
	std::vector<float> vec;

	relevancy_Out.clear();

	Frameset(len,frame_len,frame_tran,n_frame,relevancy_In,frame_Out);//数据长度,帧长,帧移,每个文件所分的帧数,输入数据,输出数据
	RectWindow(n_frame,lenth,frame_Out,rect_Out);//每个文件所分的帧数,窗长,输入数据,输出数据

	for(int i=0;i<(int)rect_Out.size();i++)
	{
		for(int m=1;m<=k;m++)
		{
			float a=0;
			for(int j=0;j<(int)rect_Out[i].size()-m;j++)
			{				
				a+=rect_Out[i].at(j)*rect_Out[i].at(j+m);
			}
			vec.push_back(a);
		}
		relevancy_Out.push_back(vec);
		vec.clear();
	}	
}
//平均幅度差函数法
void Methods::ADMF(
				   int len,//数据长度
				   int frame_len,//帧长
				   int frame_tran,//帧移
				   int n_frame,//每个文件所分的帧数		
				   int lenth,//窗长
				   int k,//短时相关最大延迟点数
				   std::vector<float>& AMDF_In,//输入数据
				   std::vector<std::vector<float> >& AMDF_Out//输出数据
				   )
{
	std::vector<std::vector<float> > frame_Out;
	std::vector<std::vector<float> > rect_Out;
	std::vector<float> vec;

	AMDF_Out.clear();

	Frameset(len,frame_len,frame_tran,n_frame,AMDF_In,frame_Out);//数据长度,帧长,帧移,每个文件所分的帧数,输入数据,输出数据
	RectWindow(n_frame,lenth,frame_Out,rect_Out);//每个文件所分的帧数,窗长,输入数据,输出数据

	for(int i=0;i<(int)rect_Out.size();i++)
	{
		for(int m=1;m<=k;m++)
		{
			float a=0;
			for(int j=0;j<(int)rect_Out[i].size()-m;j++)
			{				
				a+=abs(rect_Out[i].at(j+m)-rect_Out[i].at(j));
			}
			vec.push_back(a);
		}
		AMDF_Out.push_back(vec);
		vec.clear();
	}	
}
void Methods::BubbleSort(int frame_len,float **a)//冒泡排序,降序
{
	float t0,t1;
	for(int i=0;i<frame_len/2+1;i++)
	{
		for(int j=0;j<frame_len/2-i;j++)
		{
			if(a[j][1]<a[j+1][1])
			{
				t0=a[j][0];
				a[j][0]=a[j+1][0];
				a[j+1][0]=t0;

				t1=a[j][1];
				a[j][1]=a[j+1][1];
				a[j+1][1]=t1;
			}
		}
	}
}
void Methods::GetPitch(
					   int frame_len,//帧长
					   std::vector<float>& data_In,//某一帧倒谱数据
					   std::vector<int>& id_max,//最大峰值能量团的下标id
					   std::vector<int>& id_last,
						int& f1,//最大峰值的序号
						int& f2,//次大峰值的序号
						int& f3,//替换值的序号
						float& p1,//最大峰值的倒谱值
						float& p2,//次大峰值的倒谱值
						float& p3,//替换值的倒谱值
						float& u,//中位数
						float& threshold,//阈值
						float& k1,//p1-u
						float& k2,//p2-u
						float& divide,//k1/k2
						float& pitch,//基音周期
						int& tag//标签
						)
{
	int n=0,m=0,z=0;//n为最大峰值能量团个数
	//std::vector<int> id_last;
	float **s=new float *[frame_len];			
	for(int j=0;j<frame_len;j++)
	{
		s[j]=new float [2];
		s[j][0]=0;
		s[j][1]=0;
	}
	//初始化
	for(int j=0;j<frame_len/2+1;j++)
	{
		s[j][0]=(float)j;//下标号，从0开始
		s[j][1]=0;
	}
	//赋值,
	for(int j=0;j<frame_len/2+1;j++)
	{
		s[j][1]=data_In[j];
	}
	//前33个值忽略
	for(int j=0;j<33;j++)
	{
		s[j][1]=0;
	}
	//排序
	BubbleSort(frame_len,s);//冒泡排序,降序
	u=s[(frame_len/2+1-33)/2-1][1];//在224个值中，第112个数为其中位数，接近均值*******在96个值中，第48个数为其中位数
	threshold=s[int((frame_len/2+1-33)*0.2-1)][1];//排序位于前20%的数据为有别于波动背景的较大值,则波峰与波谷的阈值可设定为排序为第224×20%＝44的数值
	f1=(int)s[0][0];//最大峰值序号
	p1=s[0][1];//最大峰值的倒谱值
	int n_low=0;//孤立的谷点数量，当数量为1时就忽视它，仍然继续外扩，寻找最大能量团
	for(int j=f1;j>32;j--)//向前搜索
	{
		if(data_In[j]<threshold)
		{
			n_low++;
		}		
		if(data_In[j]>=threshold||(n_low<2&&(j-1)>=0&&(j-1)<(frame_len/2+1)&&data_In[j-1]>=threshold))
		{
			id_max.push_back(j);
			n++;
			n_low=0;
		}else
			break;
	}
	n_low=0;
	for(int j=f1+1;j<frame_len/2+1;j++)//向后搜索
	{
		if(data_In[j]<threshold)
		{
			n_low++;
		}
		if(data_In[j]>=threshold||(n_low<2&&(j+1)>=0&&(j+1)<(frame_len/2+1)&&data_In[j+1]>=threshold))
		{
			id_max.push_back(j);
			n++;
			n_low=0;
		}
		else
			break;
	}
	
	//if(n<=10)
	//{
	//	for(int j=0;j<10;j++)
	//	{
	//		if(find(id_max.begin(),id_max.end(),s[j][0])==id_max.end())
	//		{
	//			f2=s[j][0];
	//			p2=s[j][1];
	//			break;
	//		}
	//		else
	//		{
	//			f2=0;
	//			p2=0;
	//		}
	//	}
	//}
	int last=0;
	int step=25,id=0,k=1,line=10;
	//if(n>=10)
	//{
		for(int j=0;j<line;j++)
		{
			if(find(id_max.begin(),id_max.end(),s[j][0])!=id_max.end())
				z++;
			if(find(id_max.begin(),id_max.end(),s[j][0])==id_max.end())
			{
				last=s[j][0];
				if(last>(((f1+1)*2-15-1)<frame_len/2+1?((f1+1)*2-15-1):frame_len/2+1))
				{
					line=20;
				}
			}

		}
	//}
	
	std::vector<float> t;
	do{
		id_last.clear();
		t.clear();
		m=0;
		for(id=f1-1;id>f1-1-step&&id>32;id--)
		{
			if(find(id_max.begin(),id_max.end(),id)==id_max.end())
			{
				id_last.push_back(id);
				t.push_back(data_In[id]);
				m++;
			}
		}
		//搜索范围的右界N4，不能是128，256或512，只能是最大峰值P1对应的T1的2倍（双峰值对应的位置），并适当向左缩进15个点，即N4的右界至多是T1×2-15。
		//右界取T1×2－15，512(128或256)两者中的最小值。
		for(id=f1+1;id<f1+1+step&&id<(((f1+1)*2-15-1)<frame_len/2+1?((f1+1)*2-15-1):frame_len/2+1);id++)			
		{
			if(find(id_max.begin(),id_max.end(),id)==id_max.end())//若id不在容器id_max中，则添加之
			{
				id_last.push_back(id);
				t.push_back(data_In[id]);
				m++;
			}
		}
		sort(t.begin(),t.end());
		//if(/*z*/n>=10)
		//	line=20;
		for(id=0;id<line;id++)
		{
			if(m>0&&s[id][1]==t.at(m-1))
			{
				f2=(int)s[id][0];
				p2=s[id][1];
				break;
			}
			else
			{
				/*f2=0;
				p2=0;*/
			}
		}
		k++;
		step=25*k;
		
		if((f1-1-step)<33&&(f1-1+step)>frame_len/2)
			break;
	}
	while(id>=line);

	//设定排序后的前3%为保底值，对于帧长512，保底值Pd的序号为int(224*3%+0.5)=7(四舍五入)，即Pd=P7。P3取次大值与Pd之间的最小值，即P3=min{ P2，Pd}

	int pd;
	pd=(frame_len/2-33+1)*0.03+0.5-1;
	f3=s[pd][0];
	p3=s[pd][1];


	/*k1=p1-u;
	k2=p2-u;*/
	//K阈值＝(最大波峰的倒谱值－P512，基线)/(P3－P512，基线)≥2.25
	k1=p1-s[(frame_len/2-33+1)*3/4-1][1];
	k2=p2-s[(frame_len/2-33+1)*3/4-1][1];
	if(p2>p3)
	{
		k2=p3-s[(frame_len/2-33+1)*3/4-1][1];
	}
	
	divide=float(k1/k2);
	if(divide>=2.25)

	/*k1=p1;
	k2=p2;	
	divide=k1-k2;
	if(float(divide)>0.1)*/
	{
		tag=1;
		pitch=float(f1/16);//基音周期，单位ms
	}
	else
	{
		tag=0;
		pitch=0;
	}		
	//释放s
	for(int j=0;j<frame_len;j++)
	{
		delete []s[j];
	}
	delete []s;
	s=NULL;
}
//弧长
void Methods::LengthOfArc(
						  int len,//数据长度
						  int frame_len,//帧长
						  int frame_tran,//帧移
						  int n_frame,//每个文件所分的帧数		
						  int lenth,//窗长
						  std::vector<float>& data_In,//输入数据
						  std::vector<float>& arc_Out//输出数据
						  )
{
	std::vector<std::vector<float> > frame_Out;
	std::vector<std::vector<float> > rect_Out;

	arc_Out.clear();

	Frameset(len,frame_len,frame_tran,n_frame,data_In,frame_Out);//数据长度,帧长,帧移,每个文件所分的帧数,输入数据,输出数据
	RectWindow(n_frame,lenth,frame_Out,rect_Out);//每个文件所分的帧数,窗长,输入数据,输出数据

	for(int i=0;i<(int)rect_Out.size();i++)
	{
		float a=0;
		for(int j=1;j<(int)rect_Out[i].size();j++)
		{
			a+=sqrt((rect_Out[i].at(j)-rect_Out[i].at(j-1))*(rect_Out[i].at(j)-rect_Out[i].at(j-1))+1);
		}
		arc_Out.push_back(a);
	}	
}


