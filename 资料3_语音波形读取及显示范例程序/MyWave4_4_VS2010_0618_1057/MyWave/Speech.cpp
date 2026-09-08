#include "StdAfx.h"
#include "Speech.h"
#include<math.h>
#include<vector>
#include"WaveConvertor.h"
//#define ip       16
#include <iostream>
#include <fstream>
using namespace std;
struct FP{        //用于设置mfcc提取时的参数
	unsigned int overlap ;		    // Frame overlap
      int        tbfNum ;			// Number of triangular band-pass filters
      int        cepsNum;			// Dimension of cepstrum
      int        useDelta;			// 0 (12fea), 1 (24fea), 2 (36fea)
      bool       useEnergy ;		// 0, 1
      int        useCMS;			// Cepstral Mean Substraction, 0, 1(cms of all), 2(overlap(cms)= 24), 3(original+cms)
      int        testNum;			// test sentence number, others is train sentences.
      int        useVTLN;			// Vocal Track Length Normalization, 1 , 0
      int        alpha  ;			//For VTLN
      int        upSampling ;		// 1, 2

};
double Max(unsigned int nInLen, const double *pData)
{
	unsigned int i;
	double m;
	m=pData[0];
	for(i=1;i<nInLen;i++)
		if(pData[i]>m)
			m=pData[i];
	return m;
}
double Min(unsigned int nInLen,const double *pData)
{
	unsigned int i;
	double n;
	n=pData[0];
	for(i=1;i<nInLen;i++)
		if(pData[i]<n)
			n=pData[i];
	return n;
}

CSpeech::CSpeech(void)
{
}

CSpeech::~CSpeech(void)
{
}
//////////////////////////////////////////////////////////////////////////

// 对输入数据分帧加窗
void CSpeech::AddWindow( 
						unsigned int nWinSize,		// 处理窗宽度
						double* pData,				// 输入数据
						unsigned int nInLen			// 输入数据长度
						)
{
    double *pDataOut;
	unsigned int i ;
	double a=0.95;
	pDataOut=new double[nInLen];
	memcpy(pDataOut, pData, sizeof(double) * nInLen);
	
	pDataOut[0]=pData[0];
	for(i=1;i<nInLen;i++)//预加重
	{
		pDataOut[i]=pData[i]-a*pData[i-1];
	}
   // 计算处理次数
	unsigned int nCount = (int)((double)((nInLen-nWinSize)/(nWinSize/2))+1);
	memset(pData,0,sizeof(double)*nCount*nWinSize);
	unsigned int nOffSet = 0;
    for (i = 0; i < nCount; i++)
	{
		// 对输入数据分帧加窗
		if(i!=0)
		 nOffSet = i * nWinSize/2;      				
		
		for (unsigned int j = 0; j < nWinSize; j++)
		{
			pData[2*nOffSet + j] = 
				pDataOut[nOffSet + j]* CSpeech::HammingWinFunc(j, nWinSize); 
		}
	}

}
////////////////////////////////////////////////////
 	// 对输入数据分帧
 void CSpeech::Frame(
		unsigned int nWinSize,		// 处理窗宽度
		const double* pDataIn,		// 输入数据
		unsigned int nInLen,		// 输入数据长度
		double* pDataOut			// 输出分帧后的数据
		)
 {
     int inc,len;
	 len=nWinSize;
	 inc=nWinSize/2;
	 int fh=(int)floor((double)((nInLen-len)/inc)+1);
	 int i,j,n=0;
	 double * pDataIn1=new double[nInLen];
	 //预加重
	 pDataIn1[0]=pDataIn[0];
	 for(i=1;i<nInLen;i++)
		 pDataIn1[i]=pDataIn[i]/*-0.95*pDataIn[i-1]*/;

	 memset(pDataOut, 0, sizeof(double)*fh*len);
	 for(i=0;i<fh;i++)
	 {
		 for(j=0;j<len;j++)
		 {
			 pDataOut[i*len+j]=pDataIn1[n];
			 n=n+1;
		 }
		 n=n-len+inc;
	 }
 }
///////////////////////////////////////////////////////////////////////////////
	// 对输入数据分帧加窗
 void CSpeech::AddWindow(
		unsigned int nWinSize,		// 处理窗宽度
		const double* pDataIn,		// 输入数据
		unsigned int nInLen,		// 输入数据长度
		double* pDataOut			// 输出数据
		)
 {
	 // 拷贝输入数据到输出数据
	memcpy(pDataOut, pDataIn, sizeof(double) * nInLen);
	// 对输入数据加窗
	CSpeech::AddWindow(nWinSize, pDataOut, nInLen);
 }
 
//////////////////////////////////////////////////////////////////////////
// 输入信号序列的短时平均过零率，输入数据为加窗后的数据
void CSpeech::GetZero( 
					  unsigned int nWinSize,		/*处理窗口大小*/
					  const double* pDataIn,		/*输入信号序列*/
					  unsigned int nInLen,			/*输入信号序列长度*/
					  double* pDataOut				/*输出过零率序列*/
					  )
{
	// 保存窗口函数值
	unsigned int nOutLen = (nInLen /nWinSize);
    int i,j;	
	double *temp=new double[nWinSize];
	// 清空输出序列
	memset(pDataOut, 0, sizeof(double)* nOutLen);
	for(i=0;i<nOutLen;i++)
	{
		for(j=0;j<nWinSize;j++)
		   temp[j]=pDataIn[i*nWinSize+j];
		for(j=0;j<nWinSize-1;j++)
			pDataOut[i]+=abs(Sgn(temp[j]) - Sgn(temp[j + 1]));
		pDataOut[i]=pDataOut[i]/(2*nOutLen);//平均短时过零率
	}
}

//////////////////////////////////////////////////////////////////////////
// 计算输入信号序列的短时能量，输入数据为加窗后的数据
void CSpeech::GetEnergy( 
						unsigned int nWinSize,			/*处理窗口大小*/
						const double* pDataIn,				/*输入信号序列*/
						unsigned int nInLen,			/*输入信号序列长度*/
						double* pDataOut				/*输出短时能量序列*/
						)
{
	 //保存窗口函数值
	unsigned int nOutLen = nInLen /nWinSize;
    int i,j,k=0;	
	double *temp=new double[nWinSize];
	// 清空输出序列
	memset(pDataOut, 0, sizeof(double)* nOutLen);
	for(i=0;i<nOutLen;i++)
	{
		for(j=0;j<nWinSize;j++)
		   temp[j]=pDataIn[i*nWinSize+j];
		for(j=0;j<nWinSize;j++)
			pDataOut[i]+=temp[j]*temp[j];
	}
}

//////////////////////////////////////////////////////////////////////////
// 计算输入信号序列的平均振幅，输入数据为加窗后的数据
void CSpeech::GetAvgVibration(
							  unsigned int nWinSize,		/*处理窗口大小*/
							  const double* pDataIn,				/*输入信号序列*/
							  unsigned int nInLen,			/*输入信号序列长度*/
							  double* pDataOut				/*输出平均振幅序列*/
							  )
{
	// 保存窗口函数值
	// double nWinValue = 0;
	unsigned int nOutLen = nInLen / nWinSize;
	unsigned int nOffSet = 0;

	// 清空输出序列
	memset(pDataOut, 0, sizeof(double)* nOutLen);

	// 若输出序列和输出序列可用
	if (pDataIn != NULL && pDataOut != NULL)
	{
		// 利用海明窗作为窗口函数计算平均振幅
		for (unsigned int i = 0; i < nOutLen; i ++)
		{
			nOffSet = i * nWinSize;

			// 计算当前窗口的平均振幅
			for (unsigned int j = 0; j < nWinSize; j ++)
			{
				pDataOut[i] += abs(pDataIn[nOffSet + j]);
			}

		}
	}
}

////////////////////////////////////////////////////修正的相关函数
void CSpeech::GetRcor(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/		
	    double* pDataOut				/*输出的自相关系数*/
		)
 { 
	unsigned int i,k,j;
 	unsigned int nOutLen = nInLen / nWinSize;
    double u,sum;
	  //清空输出序列
     memset(pDataOut, 0, sizeof(double)* nOutLen*(ip));
/////////////////////////////求向量的自相关函数（MATLAB仿真p100）由于求LPC时用的自相关	
    for(i=0;i< nOutLen;i++)
	{
		 sum=0;
		 for(j=0;j<nWinSize;j++)
			sum=sum+pDataIn[j+i*nWinSize]*pDataIn[j+i* nWinSize];
		 for(k=0;k<ip;k++)               /*ip=14为最大延迟点数*/
            {
				u=0.0;
                for(j=k;j<nWinSize-1;j++)		
				 u=u+pDataIn[j+1+i* nWinSize]*pDataIn[i* nWinSize+j-k];
				pDataOut[i*(ip)+k]=u/sum;
		}		
	}
 }
////////////////////////////////////////////////////基于短时平均幅度差函数AMDF法的基音周期估值
void CSpeech::AMDFPitch(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* pitch				/*输出的基音周期值*/
		)
 { 

	 double *A;
     double sum,m;
	 int i, k,j;
	 unsigned int nCount=nInLen/nWinSize;//帧数
	 A=new double[nWinSize];	
	  memset(pitch, 0, sizeof(double)* ( nCount-1));
	 
	 for(i=0;i<nCount-1;i++)
	 {
		 //ofstream fout( "AMDF.txt", ios::trunc );
		for( k=0;k<nWinSize;k++)
	    {
		   sum=0;
		   for(j=0;j<nWinSize;j++)
			  sum=sum+abs(pDataIn[i*nWinSize+j]-pDataIn[i*nWinSize+j+k]);
		   A[k]=sum;
		   //fout<<A[k]<<"  ";
		}  
		//fout.close ();	
		m=Min(nWinSize,A);//找出最大值
		//for(j=0;j<nWinSize;j++)
		//{
		//	A[j]=A[j]/m;
		//	fout<<j<<"  "<<A[j]<<endl;
		//} 
	    double  min=A[29];//去掉前面10个数值较大的项
        int a=0;
	    for(j=30;j<nWinSize;j++)
	    {
			if(A[j]<min)
			{
				min=A[j];
				a=j+1;
			}
	    }
		//CSpeech::MaxCor(A,nWinSize,pitch[i]);//获得这一帧中去除前十项后最大的值的下标和最大值    
		pitch[i]=a;
	    pitch[i]=pitch[i]/16;//算出对应的周期
	 }
 
 }
void CSpeech::CepPitch(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
		double *Cep,     /*输出的倒谱值*/
	    double* pitch	/*输出基音周期*/			
	)
{
	int i,j;
	int nCount=(nInLen-nWinSize)/(nWinSize/2)+1;//帧数
	double *temp=new double[nWinSize];
	//====== Step 1:预加重.
	double *pDataIn1=new double[nInLen];
	double *pDataOut=new double[nWinSize];
	pDataIn1[0]=pDataIn[0];
	for(i=1;i<nInLen;i++)
	{
		pDataIn1[i]=pDataIn[i]-0.95*pDataIn[i-1];
	}
	int kk=0;
	double max=0;
    for(i=0;i<nCount;i++)
	{   
		//====== Step 2:分帧.
			 
		for(j=0;j<nWinSize;j++,kk++)		     
		{			
			temp[j]=pDataIn1[kk];	 //取一帧数据 
			
		}	 
		
		kk=kk-nWinSize/2;
		//====== Step 3:加窗.
		//////////////////////////////////加汉明窗
		//ofstream fout1( "data.txt", ios::trunc );	
        for(j=0;j<nWinSize;j++)   
		{			
			 temp[j]=temp[j]*(0.54-0.46*cos(2*pi*j/(nWinSize-1)));
			 //fout1<<j+1<<"   "<<temp[j]<<endl;
	    } 
		//fout1.close();
		 memset(pDataOut,0,sizeof(double)*nWinSize);
         //求倒谱
		 CWaveConvertor::ConvertToCepStrum( 
								   nWinSize,			// 样本数量
								   nWinSize,		// 短时点数
								   temp,			// 输入信号
								   pDataOut					// 输出的倒谱
								   );
         //CSpeech::MaxCor( pDataOut,nWinSize,pitch[i]);//获得这一帧中去除前十项后最大的值的下标和最大值  
		  max=pDataOut[31];
		  pitch[i]=31;
		 for(j=31;j<nWinSize/2;j++)
		 {
			 if(max<pDataOut[j])
			 { 
				 max=pDataOut[j];
				 pitch[i]=j;
			 }
		 }
		 //pitch[i]=pitch[i]/16;
		 if(pitch[i]<32)
			 pitch[i]=0;
		 //将两边的移到中间，中间的移到两边，这样使得最大值在中心位置
		 for(j=0;j<nWinSize/2;j++)
			 Cep[i*nWinSize+j]=pDataOut[nWinSize/2-1-j];
		 for(j=nWinSize/2;j<nWinSize;j++)
			 Cep[i*nWinSize+j]=pDataOut[(j+1)-nWinSize/2];
		 



		 //////////////////////////////////////////////////////////////////检测基音周期？？？？？？未（2013年4月18号下午16:45）
		//////double  max=pDataOut[0];//去掉前面10个数值较大的项
  //////      int a=0;
	 //////   for(j=1;j<nWinSize/2;j++)
	 //////   {
		//////	if(pDataOut[j]>max)
		//////	{
		//////		max=pDataOut[j];
		//////		a=j+1;
		//////	}
	 //////   }
		//////if(max>0.08&&(a-1)>(int)(16000/500))
		//////{/*pitch[i]=a;
  //////       pitch[i]=pitch[i]/16;*/
		//////	pitch[i]=16000/(a+30);
		//////}
		//////else
		//////	pitch[i]=0;
		 //ofstream fout( "cep2.txt", ios::trunc );
		 //for(j=0;j<nWinSize;j++)
			// fout<<j+1<<"   "<<pitch[j]<<endl;
		 //fout.close();



	 }



}
////////////////////////////////////////////////////基于短时自相关法的基音周期估值
void CSpeech::GetACFPitch(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* pitch,			/*输出的基音周期数值*/
		unsigned int nFs,
		double *acf                 /*  相关系数*/
		)
 { 
	int i,j;
	int nCount=(nInLen-nWinSize)/(nWinSize/2)+1;//帧数
	double *temp=new double[nWinSize];
	//====== Step 1:预加重.
	double *pDataIn1=new double[nInLen];
	double *pDataOut=new double[nWinSize*nCount];
	double max,min,ht;//最大值，最小值
	double coeff=0.70;//中心削波函数系数取0.7
	double th0;//求中心削波函数门限
	pDataIn1[0]=pDataIn[0];
		
	for(i=1;i<nInLen;i++)
	{
		pDataIn1[i]=pDataIn[i]-0.95*pDataIn[i-1];
	}
	int kk=0,k;
    for(k=0;k<nCount;k++)
	{   
		//====== Step 2:分帧.
			 
		for(j=0;j<nWinSize;j++,kk++)		     
		{			
			temp[j]=pDataIn1[kk];	 //取一帧数据 
			
		}	 
		
		kk=kk-nWinSize/2;
		max=Max(nWinSize,temp);//选择该帧最大语音幅度
		for(j=0;j<nWinSize;j++)		     
		{			
			temp[j]=temp[j]/max;	 //归一化
			
		}

		max=Max(nWinSize,temp);
		min=Min(nWinSize,temp);
		ht=(max+min)/2;
		for(j=0;j<nWinSize;j++)		     
		{			
			temp[j]=temp[j]-ht;	 
			
		}
		max=Max(nWinSize,temp);

		th0=max*coeff;//保留小数点后5位有效数字，求中心削波函数门限

		for(i=0;i<nWinSize;i++)
		{
			if(temp[i]>=th0)
				temp[i]=temp[i]-th0;
			else 
				if (temp[i]<=(-th0))
				temp[i]=temp[i]+th0;
			else
				temp[i]=0;
			pDataOut[k*nWinSize+i]=temp[i];
		}
	}
	//pitch=new double[nCount-1];
	double sum;
    memset(pitch, 0, sizeof(double)* (nCount-1));
	memset(acf, 0, sizeof(double)* (nCount-1)*nWinSize);
	//ofstream fout( "pitch.txt", ios::trunc);
	double * temps=new double[nWinSize];//暂时保存每帧的自相系数
    for(i=0;i< nCount-1;i++)
	{
		for(k=0;k<nWinSize;k++)
		{
			sum=0.0;
			for(j=0;j<nWinSize;j++)
				sum=sum+pDataOut[i*nWinSize+j]*pDataOut[i*nWinSize+j+k];//中心或三电平削波处理后的数据
			//tempOut[i*nWinSize+k]=sum;
			temps[k]=sum;
			acf[i*nWinSize+k]=temps[k];
		}
		//ofstream fout( "after_zixiangguan.txt", ios::trunc);
		if(temps[0]!=0)
		{
			for(k=0;k<nWinSize;k++)
		    {
				temps[k]=temps[k]/temps[0];//归一化自相关函数
				acf[i*nWinSize+k]=temps[k];
			}
		}
		else
		{
			for(k=0;k<nWinSize;k++)
		    {
				acf[i*nWinSize+k]=temps[k];
			}
		}
		////fout.close();
		///////////////////////////////获取最大值（根据pitch_test.m）这种方法没有用中心削波处理
		CSpeech::MaxCor(temps,nWinSize,pitch[i]);//获得这一帧中去除前十项后最大的值的下标和最大值       
	    pitch[i]=pitch[i]/16;//算出对应的周期
		//fout<<pitch[i]<<endl;
		if(pitch[i]<2.2)
			pitch[i]=0;
	}

	//归一化的求取
//unsigned int i,k,j;
//    double u,sum,ht;
//	double *temp,*tempOut;
//	temp=new double[nInLen];//临时存放数据
//	double m,n;//最大值，最小值
//	 // 清空输出序列
//	double *pDataOut=new double[(nOutLen-1)*nWinSize];
//     memset(pDataOut, 0, sizeof(double)* (nOutLen-1)*nWinSize);
//	 tempOut=new double [(nOutLen-1)*nWinSize];
//	 m=Max(nInLen,pDataIn);
//	
//	for(i=0;i<nInLen;i++)
//	{
//		temp[i]=pDataIn[i]/m;//数据归一化
//	}
//	m=Max(nInLen,temp);//找到最大的正值
//	n=Min(nInLen,temp);//找到最小的负值
//	//为保证幅度值与横坐标轴对称,采用计算公式是n+(m-n)/2，合并为（m+n）/2
//	ht=(m+n)/2;
//	
//	for(i=0;i<nInLen;i++)//数据中心下移，保持和横坐标轴对称
//	{
//		temp[i]=temp[i]-ht;		
//	}
//	
//
//	double coeff=0.70;//中心削波函数系数取0.7
//	double th0=Max(nInLen,temp)*coeff;//保留小数点后5位有效数字，求中心削波函数门限
//////////////////////////////////////////////////////中心削波
//	
//    for(i=0;i<nInLen;i++)
//	{
//		if(temp[i]>=th0)
//			temp[i]=temp[i]-th0;
//		else 
//			if (temp[i]<=(-th0))
//			temp[i]=temp[i]+th0;
//		else
//			temp[i]=0;
//		
//		
//	}
/////////////////////////////////////////////三电平削波
//	//for(i=0;i<nInLen;i++)
//	//{
//	//	if(temp[i]>th0)
//	//		temp[i]=1;
//	//	else 
//	//		if (temp[i]<(-th0))
//	//		temp[i]=-1;
//	//	else
//	//		temp[i]=0;
//	//	
//	//}
//
//
//	m=Max(nInLen,temp);
//
//	//ofstream fout1( "after_zhongxue.txt", ios::trunc);
//    for(i=0;i<nInLen;i++)//中心削波函数幅度的归一化
//	{
//		temp[i]=temp[i]/m;
//	    //fout1<<temp[i]<<"  ";
//	}
//   //fout1.close ();
//   
//	//对削波后的函数计算自相关
////////////////////////////////////自相关函数（与MATLAB仿真第47页程序相同）
//	/*double *MaxCor=new double[nOutLen-1];*/
//	pitch=new double[nOutLen-1];
//      memset(pitch, 0, sizeof(double)* (nOutLen-1));
//	//ofstream fout( "pitch.txt", ios::trunc);
//	double * temps=new double[nWinSize];//暂时保存每帧的自相系数
//    for(i=0;i< nOutLen-1;i++)
//	{
//		for(k=0;k<nWinSize;k++)
//		{
//			sum=0.0;
//			for(j=0;j<nWinSize;j++)
//				sum=sum+temp[i*nWinSize+j]*temp[i*nWinSize+j+k];//中心或三电平削波处理后的数据
//				//sum=sum+pDataIn[i*nWinSize+j]*pDataIn[i*nWinSize+j+k];//未中心削波处理的数据
//			//sum=floor((sum*10000.000f+0.5)/100000.000f);
//			tempOut[i*nWinSize+k]=sum;
//			temps[k]=sum;
//		}
//		//ofstream fout( "after_zixiangguan.txt", ios::trunc);
//  //      for(k=0;k<nWinSize;k++)
//		//{
//		//     //temps[k]=temps[k];
//		//	if(temps[0]!=0)
//		//	 fout<<k+1<<"    "<<temps[k]/temps[0]<<"  "<<endl;
//		//	else
//		//		fout<<k+1<<"    "<<temps[k]<<"  "<<endl;
//		//     
//		//}
//		//fout.close();
//		///////////////////////////////获取最大值（根据pitch_test.m）这种方法没有用中心削波处理
//		CSpeech::MaxCor(temps,nWinSize,pitch[i]);//获得这一帧中去除前十项后最大的值的下标和最大值       
//	    pitch[i]=pitch[i]/16;//算出对应的周期
//		//fout<<pitch[i]<<endl;
//		if(pitch[i]<2.2)
//			pitch[i]=0;
//	}

	//
	////获得基音周期后去除野点，中值平滑处理

	//double *zhouqi=new double[ nOutLen-1];
	//double *tempzhouqi=new double[5];
	//for(i=0;i<nOutLen-1;i++)
	//{
	//	if(i==0)
	//	{
	//		tempzhouqi[i*5+0]=0;
	//		tempzhouqi[i*5+1]=0;
	//	    for(j=2;j<5;j++)
	//		  tempzhouqi[j]=pitch[i+j-2];
	//	}
	//    else if(i==1)
	//	{
	//		tempzhouqi[i*5+0]=0;
	//		for(j=1;j<5;j++)
	//		  tempzhouqi[j]=pitch[i+j-2];
	//	}
	//    else if(i==(nOutLen-1-2))
	//	{
	//		for(j=0;j<4;j++)
	//			tempzhouqi[j]=pitch[i+j-2];
	//		tempzhouqi[j]=0;
	//	}
	//	else if(i==(nOutLen-1-1))
	//	{
	//		for(j=0;j<3;j++)
	//			tempzhouqi[j]=pitch[i+j-2];
	//		tempzhouqi[j]=0;
	//		tempzhouqi[j+1]=0;
	//	}
	//	else
	//	{
	//		for(j=0;j<5;j++)
	//		  tempzhouqi[j]=pitch[i+j-2];

	//	}
	//	//for(int k=0;k<5;k++)
	//	//	fout<<tempzhouqi[k]<<"  ";
	//	//fout<<endl;
 //       zhouqi[i]= Medfilt(tempzhouqi,5);	
        
	//}
	//for(i=0;i<nOutLen-1;i++)
	//	fout<<zhouqi[i]<<endl;
 //    fout.close();



////////////////////////自相关函数（与MATLAB仿真第100页结果相同）

	//CSpeech::GetRcor(nWinSize,		    /*处理窗口大小*/
	//                 temp,		   	   /*输入信号序列*/
	//                 nInLen,    		/*输入信号序列长度*/
	//                 pDataOut			/*输出的自相关系数*/
	//	);

 }
 ///////////////////////////////////////////////////////////////
void CSpeech::GetWindowCoef(double* pWndCoef, UINT nWndType)//窗函数
{
	int i = 0;
	switch(nWndType)
	{
	case eWND_RECTANGULAR://矩形窗
		for(i = 0; i < FRM_SIZE; i++)
			pWndCoef[i] = 1;
		break;

	case eWND_HAMMING://汉明窗
		for(i = 0; i < FRM_SIZE; i++)
			pWndCoef[i] = (0.54 - 0.46 * cos(i * PI_2 / (FRM_SIZE - 1)));
		break;

	case eWND_HANNING://汉宁窗
		for(i = 0; i < FRM_SIZE; i++)
			pWndCoef[i] = (0.50 - 0.50 * cos(i * PI_2 / (FRM_SIZE - 1)));
	case eWND_BLACKMAN://Blackman窗
		for(i = 0; i < FRM_SIZE; i++)
			pWndCoef[i] = (0.42 - 0.50 * cos(i * PI_2 / (FRM_SIZE - 1))+0.08*cos(i * PI_2 / (FRM_SIZE - 1)));
		break;
	}
}

void CSpeech::FrameProcessing(const double* pIndata, int nFrame, unsigned int m_nFrameSize,double* pOut, double* pWcoef)//分帧
{
	for(int i = 0; i < m_nFrameSize; i++)
	{
		pOut[i] = pWcoef[i] * (double)pIndata[nFrame * FRM_SHIFT + i];
	}
}

 /////////////////////////////////////////////////////////////////////////////////////////////////曹强版ACF方法
 void CSpeech::GetAcfFrequency(unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* pitch,			/*输出的自相关系数*/
		unsigned int nFs)
 {
	double dWcoef[FRM_SIZE];
	double dAC[FRM_SIZE];
	double dWnd[FRM_SIZE];
	double dFreq[FRM_SIZE];
	int nFrmCnt = (nInLen - FRM_SIZE) / FRM_SHIFT + 1;//帧数
	
	int m_nFrameSize;

	if((nInLen) < FRM_SIZE)
	{
		nFrmCnt = 1;
		m_nFrameSize = nInLen;
	}
	else
	{
		m_nFrameSize = FRM_SIZE;//帧长
	}

	// Get window coefficient
	GetWindowCoef(dWcoef, eWND_RECTANGULAR);//矩形窗
	//ofstream fout("dfreq.txt",ios::trunc);
	// Calc frequency for every frame data
	for(int i = 0; i < nFrmCnt; i++)
	{		
		FrameProcessing(pDataIn, i, m_nFrameSize,dWnd, dWcoef);//每一帧加矩形窗，dwnd保存每一帧加窗后的数据
		ACF(dWnd, dAC,m_nFrameSize);//自相关
		pitch[i] = AcfFrequency(dAC, nFs,m_nFrameSize);
		//fout<<dFreq[i]<<endl;
	}
	//fout.close();
 }
 double CSpeech::ACF(double* pIn, double* pOut,unsigned int m_nFrameSize) // Calc auto correlation function
 {
	int n = 0;
	int k = 0;
	double dMaxValue = 0.0;

	for(n = 0; n < m_nFrameSize / 2; n++)
	{
		pOut[n] = 0.0;
		for(k = 0; k < m_nFrameSize / 2; k++)
		{
			pOut[n] += pIn[k] * pIn[k + n];
		}
		pOut[n] = pOut[n] / m_nFrameSize / 2;
		
		if(fabs(pOut[n]) > dMaxValue)
		{
			dMaxValue = pOut[n];
		}
	}
	return dMaxValue;
 }

 double CSpeech::AcfFrequency(double *pAC, int nFs,unsigned int m_nFrameSize)
{
	double dS1 = 0.0;
	double dS2 = 0.0;
	double dSlope = 0.0;
	double dPeekIndex = 0.0;
	double dPeekValue = 0.0;
	double dEnergy = 0.0;
	double dPitch = 0.0;
	
	// Get energy value
	dEnergy = 10 * log10(pAC[0]);

	if(dEnergy > THRESHOLD_ENERGY)
	{
		double dSlope = pAC[1] / pAC[0];

		// Voiced
		if(dSlope > 0.5)
		{
			for(int i = 0; i < m_nFrameSize / 2 - 1; i++)
			{
				dS1 = pAC[i + 1] - pAC[i];
				dS2 = pAC[i + 2] - pAC[i + 1];

				// Peak-picking     峰值选取
				if(dS1 > 0 && dS2 < 0)
				{
					if(pAC[i + 1] > dPeekValue)
					{
						dPeekIndex = i + 1;
						dPeekValue = pAC[i + 1];
					}
				}
			}

			// Calc pitch-Hz       计算基音频率
			if(dPeekIndex > 1)
			{
				dPitch = (double)nFs / dPeekIndex;//基音频率
				//dPitch = (double)dPeekIndex /nFs ;//基音周期

			}
		}
		else // Unvoiced
		{
			dPitch = 0.0;
		}
	}

	return dPitch;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////AMDF方法
 double CSpeech::AMDF(double *pIn, double *pOut, unsigned int m_nFrameSize)
{
	int n = 0;
	int k = 0;
	double dMaxValue = 0.0;
	int nFrameSize=0;
	if(nFrameSize <= 0)
	{
		nFrameSize = m_nFrameSize;
	}

	for(n = 0; n < nFrameSize / 2; n++)
	{
		pOut[n] = 0.0;
		for(k = 0; k < nFrameSize / 2; k++)
		{
			pOut[n] += fabs(pIn[k] - pIn[k + n]);
		}
		pOut[n] = pOut[n] / nFrameSize / 2;

		if(fabs(pOut[n]) > dMaxValue)
		{
			dMaxValue = pOut[n];
		}
	}

	return dMaxValue;
}

double CSpeech::GetAmdfFrequency(const double * pInData,unsigned int dwSize, int nFs,unsigned int m_nFrameSize, double* pitch)
{
	double dWcoef[FRM_SIZE];
	double dAC[FRM_SIZE];
	double dWnd[FRM_SIZE];
	double dFreq[FRM_SIZE];
	int nFrmCnt = (dwSize  - FRM_SIZE) / FRM_SHIFT + 1;
	
	if((dwSize) < FRM_SIZE)
	{
		nFrmCnt = 1;
		m_nFrameSize = dwSize ;
	}
	else
	{
		m_nFrameSize = FRM_SIZE;
	}

	// Get window coefficient
	GetWindowCoef(dWcoef, eWND_RECTANGULAR);
	ofstream fout("dfreq.txt",ios::trunc);
	// Calc frequency for every frame data
	for(int i = 0; i < nFrmCnt; i++)
	{		
		FrameProcessing(pInData, i ,m_nFrameSize,dWnd, dWcoef);
		AMDF(dWnd, dAC,m_nFrameSize);
		dFreq[i] = AmdfFrequency(dAC, nFs,m_nFrameSize);
		fout<<dFreq[i]<<endl;
	}
	fout.close();

	// Analyse all frequencies and select a valid one
	/*for(int j = 0; j < nFrmCnt; j++)
	{
		return AnalyseFreq(dFreq, nFrmCnt);
	}*/
	return 0;
}

double CSpeech::AmdfFrequency(double* pAC, int nFs,unsigned int m_nFrameSize)
{
	double dS1 = 0.0;
	double dS2 = 0.0;
	double dSlope = 0.0;
	double dValleyIndex = 0.0;
	double dValleyValue = 0x7fffffff;
	double dEnergy = 0.0;
	double dPitch = 0.0;
	
	// Get energy value
	dEnergy = 10 * log10(pAC[0]);

	//if(dEnergy > THRESHOLD_ENERGY)
	{
		double dSlope = pAC[1] / pAC[0];

		// Voiced
		if(dSlope > 0.5)
		{
			for(int i = 0; i < m_nFrameSize / 2 - 1; i++)
			{
				dS1 = pAC[i + 1] - pAC[i];
				dS2 = pAC[i + 1] - pAC[i + 2];

				// Valley-picking
				if(dS1 < 0 && dS2 < 0)
				{
					if(pAC[i + 1] < dValleyValue && pAC[i + 1] > 1)
					{
						dValleyValue = pAC[i + 1];
						dValleyIndex = i + 1;
					}
				}
			}

			// Calc pitch-Hz
			if(dValleyIndex > 1)
			{
				dPitch = (double)nFs / dValleyIndex;
			}
		}
		else // Unvoiced
		{
			dPitch = 0.0;
		}
	}

	return dPitch;
}
/////////////////////////////////////////////////////////////////////////////////////////////// ACF+AMDF结合     （曹强版）

 void CSpeech::MaxCor(double * data,unsigned int len,double &num)
 {
	 unsigned int i;
	double  maxcor=data[9];//去掉前面10个数值较大的项
	 for(i=10;i<len;i++)
	 {
		 if(data[i]>maxcor)
		 {
			 maxcor=data[i];
			 num=i+1;
		 }
	 }
 }
 double CSpeech::Medfilt(double *data,int n)//中值平滑
 {
	 double middle;
      // 循环变量
     int  i;
     int  j;
 
     // 中间变量
     double bTemp;
 
     // 用冒泡法对数组进行排序
     for (j = 0; j < n - 1; j ++)
     {
         for (i = 0; i < n - j - 1; i ++)
         {
             if (data[i] > data[i + 1])
            {
              // 互换
             bTemp = data[i];
             data[i] = data[i + 1];
             data[i + 1] = bTemp;
			 }
		 } 
	 }
 
    // 计算中值
    if ((n & 1) > 0)
   {
      // 数组有奇数个元素，返回中间一个元素
      bTemp = data[(n + 1) / 2-1];
    }
   else
   {
      // 数组有偶数个元素，返回中间两个元素平均值
      bTemp = (data[n / 2-1] + data[n / 2]) / 2;
   }
 
   // 返回中值
    return bTemp;




 }
//////////////////////////////////////////////////////////////////LPC
 /*
 //函数功能：
 //该函数的功能是用自相关法求使信号s均方预测误差为最小的预测系数
 */
 ///////////////////////////////////////////////////////////////////
void CSpeech::GetLPC(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* lpc			       /*输出的LPC预测系数*/
	)
{
	//////////////////////////////////////matlab仿真P100（成功）
	 //int ip=lpc_num;
	 int i,j,k,h,m,b,p=0;
	 double u,sum,*Rp;	
	 double *Ep,*kk;
	 double a[ip][ip];
	 int nCount=(nInLen-nWinSize)/(nWinSize/2)+1;
	 Rp=new double[ip];
	 Ep=new double[ip];
	 kk=new double[ip];
     memset(lpc, 0, sizeof(double)* nCount*(ip+1));

	 double *pDataIn1=new double[nInLen];
     ////////////////////////预加重
	 pDataIn1[0]=pDataIn[0];
	 for(i=1;i<nInLen;i++)
	 {
		pDataIn1[i]=pDataIn[i]-0.95*pDataIn[i-1];
	 }
	 double *data=new double[nWinSize];
	 for(b=0;b<nCount;b++)//取一帧
	 {
		memset(Rp, 0, sizeof(double)* ip); //初始化为0
		memset(Ep, 0, sizeof(double)* ip);//Ep为p阶最佳线性预测反滤波能量
	    memset(kk, 0, sizeof(double)*ip);//kk为自相关系数
	    memset(a, 0, sizeof(double)*ip*ip);
        sum=0;
		////////////////////////////////////////////////////////////////求每一帧的自相关系数
		for(i=0;i<nWinSize;i++,p++)
			data[i]=pDataIn1[p];
		p=p-nWinSize/2;
	    for(j=0;j<nWinSize;j++)
		    sum=sum+data[j]*data[j];
        for(k=0;k<ip;k++)               /*ip=14为最大延迟点数*/
        {
				u=0.0;
                for(j=k;j<nWinSize-1;j++)		
				 u=u+data[j+1]*data[j-k];
				Rp[k]=u;
		 }
	
	////////////////////////////////////////////////////////Levinson-Durbin（莱文森-杜宾）递推算法求自相关法
	   kk[0]=Rp[0]/sum;
	   a[0][0]=kk[0]; 
	   Ep[0]=(1-kk[0]*kk[0])*sum;
	   if(ip>1)
		 for (j=1;j<ip;j++)
		 {   
			 sum=0;
			 for(i=0,h=j;i<j&&h>0;i++,h--)
			 {
				 sum=sum+a[i][j-1]*Rp[h-1];
			 }
			 kk[j]=(Rp[j]-sum)/Ep[j-1];
			 
			 a[j][j]=kk[j];
			 Ep[j]=(1-kk[j]*kk[j])*Ep[j-1];
			 for(m=0;m<j;m++)
			 {
				 a[m][j]=a[m][j-1]-kk[j]*a[j-1-m][j-1];
			     
			 }
          }	
	  
		 lpc[b*(ip+1)]=1.0;
		 for(i=0;i<ip;i++)
		 {
			 lpc[b*(ip+1)+i+1]=-a[i][ip-1];
		 } 
		  
	 }   
	 if(Ep!=NULL)
		 delete Ep;
	  if(Rp!=NULL)
		 delete Rp;
	   if(kk!=NULL)
		 delete kk;
}
//////////////////////用lpc函数所求系数合成语音信号//////////////////////////////////

void CSpeech:: LPCToDATA(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* pDataOut				/*用lpc函数所求系数合成语音信号*/
	)
{
	 /////////////////////////////////////////////////////// 用lpc函数所求系数合成语音信号
	int i,j,k; 
	int nCount=nInLen/nWinSize;
	double sum,*lpc;
	lpc=new double[nCount*(ip+1)];
    memset(pDataOut,0,sizeof(double)*nInLen);
	memset(lpc,0,sizeof(double)*nCount*(ip+1));

    CSpeech::GetLPC(nWinSize,pDataIn,nInLen,lpc);//获得Lpc系数
	
	 // ofstream fout3("lpcdata.txt",ios::app); 
	  for(k=0;k<nCount;k++)
	  {
	    for(i=0;i<ip;i++)
	   {
		  pDataOut[k*nWinSize+i]=pDataIn[k*nWinSize+i];
		//  fout3<<i<<"   "<<pDataOut[k*nWinSize+i]<<endl;
	   }
	   for(i=ip;i<nWinSize;i++)
	  {
		 sum=0;
		 for(j=1;j<=ip;j++)
			 sum=sum-lpc[k*(ip+1)+j]*pDataOut[k*nWinSize+i-j];
		 pDataOut[k*nWinSize+i]=sum;
		// fout3<<k*nWinSize+i<<"   "<<pDataOut[k*nWinSize+i]<<endl;
	  }
	  //  fout3.close ();
	  }

}
////////////////////////////////////////////////////////////////////求LPC倒谱
void CSpeech::GetLPCC(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
	    double* lpcc ,	 /*输出的LPCC*/
		double *lpc)		  
{

	double ss=0.0;
	int i,n,m;
	unsigned int nCount=floor(double(nInLen-nWinSize)/(nWinSize/2)+1);
    // 清空输出序列
     memset(lpcc, 0, sizeof(double)* nCount*(ip+1));
	 //CSpeech::GetLPC(nWinSize,pDataIn,nInLen,lpc);//获取LPC

	 int n_lpc=ip;//Lpc系数个数，即Lpc阶数，不包括lpc(0)=1;
	 int n_lpcc=ip;//倒谱系数个数
     
	 for(i=0;i<nCount;i++)
	 {
	    lpcc[i*(ip+1)+1]=lpc[i*(ip+1)+1];
	    //计算倒谱lpcc[2]到lpcc[n_lpcc-1]
	    for(n=2;n<=n_lpc;n++)
	   {
		  lpcc[i*(ip+1)+n]=lpc[i*(ip+1)+n];
		  for(m=1;m<=n-1;m++)
		     lpcc[i*(ip+1)+n]=lpcc[i*(ip+1)+n]+lpc[i*(ip+1)+m]*lpcc[i*(ip+1)+n-m]*(n-m)/n;			  
	   }
	 
     //计算lpcc[n_lpc]到lpcc[n_lpcc]
	  for(n=n_lpc+1;n<=n_lpcc;n++)
	  {
		   lpcc[i*(ip+1)+n]=0;
		   for(m=1;m<=n_lpc;m++)
		   {
			   lpcc[i*(ip+1)+n]=lpc[i*(ip+1)+n]+lpc[i*(ip+1)+m]*lpcc[i*(ip+1)+n-m]*(n-m)/n;
			   
		   }
	   }
	 }
}
////////////////////////////////////////赵力————由LPC倒谱系数计算Mel倒谱系数(P306)
void  CSpeech::GetLPCCMCC( 
						unsigned int nWinSize,		    /*处理窗口大小*/
	                    const double* pDataIn,			/*输入信号序列*/
	                    unsigned int nInLen,			/*输入信号序列长度*/
		                double * mel                    /*Mel倒谱系数*/
					   )
{   
	int nCount=floor(double(nInLen-nWinSize)/(nWinSize/2)+1);
	double sum,*lpcc,*lpc;
	lpcc=new double[nCount*(ip+1)];
	memset(lpcc,0,sizeof(double)*nCount*(ip+1));
    CSpeech::GetLPCC (nWinSize,pDataIn,nInLen,lpcc,lpc);//获取LPCC
	
	memset(mel, 0, sizeof(double)* nCount*(ip+1));//初始化mel
	
	int i,k,n;
	double *u0=new double[ip+2];
	double *u1=new double[ip+2];
	double alpha=ALPHA;
	//ofstream fout("1mel.txt",ios::app);
	for(i=0;i<nCount;i++)
	{
	   /*************************
	   *Calculation mel[0]
	   *************************/
	   u0[ip+1]=0.0;
	  
	   for(n=ip;n>=0;--n)
	   {
		  u0[n]=lpcc[i*(ip+1)+n]+alpha*u0[n+1];
		  
	   }
	   
	   mel[i*(ip+1)]=u0[0];
	   //fout<<"0     "<<mel[i*(ip+1)]<<endl;
	   /*************************
	   *Calculation mel[1]
	   *************************/
	   u1[ip+1]=0.0;
	   for(n=ip;n>=0;--n)
		  u1[n]=(1-alpha*alpha)*u0[n+1]+alpha*u1[n+1];
	   mel[i*(ip+1)+1]=u1[0];
        //fout<<"1     "<<mel[i*(ip+1)+1]<<endl;
	   /*************************
	   *Calculation mel[2],mel[3],............,mel[ip]
	   *************************/
	   for(k=2;k<=ip;++k)
	   {
		   for(n=0;n<=ip+1;++n)
			  u0[n]=u1[n];
		   for(n=ip;n>=0;--n)
			  u1[n]=u0[n+1]+alpha*(u1[n+1]-u0[n]);
		   mel[i*(ip+1)+k]=u1[0];
		    //fout<<k<<"     "<<mel[i*(ip+1)+k]<<endl;
	   }
	}
	//fout.close();
	if(lpcc!=NULL)
		delete lpcc;
}
//////////////////////***********张智星版本MFCC***********////////////////////F:\语音识别\书籍\sap\sap\wave2mfcc.m
/////////////////////////////////////////////////////////////////////////MFCC
void CSpeech::GetMFCC(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
		unsigned int fs,                //采样频率
		unsigned int filterNum,         //滤波器个个数
		unsigned int cepsNum,           //mel系数的个数
	    double* mfcc			           /*输出MFCC*/
	)
{	

		
	//int F[22] = {0,100,200,300,400,500,600,700,800,900,1000,
	//	1149,1320,1516,1741,2000,2297,2639,3031,3482,4000,4000};  //临界频带滤波器:  中心频率
	//double cos_par[20] = {0.0785398163, 0.2356194490, 0.3926990817, 0.5497787144, 0.7068583471,
	//	0.8639379797, 1.0210176124, 1.1780972451, 1.3351768778, 1.4922565105,
	//	1.6493361431, 1.8064157758, 1.9634954085, 2.1205750412, 2.2776546739,
	//	2.4347343065, 2.5918139392, 2.7488935719, 2.9059732046, 3.0630528372};  //来源: (j-1/2)*pi/20   k=1,2,3……  j=1,2,3……

	//    //int BW[m_FiltNum+1]={100,100,100,100,100,100,100,100,100,100,124,
 //    //   160,184,211,242,278,320,367,422,484,556,
 //    //   639,734,843,969,1112};//滤波器的带宽	
	////int F[26] = {0,100,200,300,400,500,600,700,800,900,1000,1149,1320,1516,1741,2000,2297,2639,3031,3482,4000,
 ////                4595,5278,6063,6964,8000};//滤波器的中心频率
	////double cos_par[24] = {0.0654498469,0.1963495406,0.3272492343,0.4581489281,0.5890486219,0.7199483156,0.8508480094,0.9817477031,
	////                      1.1126473969,1.2435470906,1.3744467844,1.5053464781,1.6362461719,1.7671458656,1.8980455594,2.0289452531,
	////                      2.1598449469,2.2907446406,2.4216443344,2.5525440281,2.6834437219,2.8143434156,2.9452431094,3.0761428031};  //来源: (j-1/2)*pi/20   k=1,2,3……  j=1,2,3……
	//
	//double *fftMag=new double[nWinSize];
 //   double *mel=new double[cepsNum];
	//
	//
	//int j,i,k,kk=0;
	//int nCount=(nInLen-nWinSize)/(nWinSize/2)+1;
	//double *pDataOut=new double[nWinSize];
	//double *pDataIn1=new double[nInLen];
	////预加重
	// //ofstream fout1("trigle.txt",ios::trunc);
	//pDataIn1[0]=pDataIn[0];
	//for(i=1;i<nInLen;i++)
	//{
	//	pDataIn1[i]=pDataIn[i]-0.97*pDataIn[i-1];
	//	//fout1<<pDataIn1[i]<<endl;
	//}
	////fout1.close();
	//for(j=0;j<nCount;j++)
	//{
	//	memset(mel,0,sizeof(double)*cepsNum);
	//	memset(fftMag,0,sizeof(double)*nWinSize);
	//	for(i=0;i<nWinSize;i++,kk++)
	//	{	
	//		fftMag[i]=pDataIn1[kk];	 //取一帧数据   			 
	//	 } 
	//	kk=kk-nWinSize/2;//帧移
	//	for(i=0;i<nWinSize;i++)   
	//	{			
	//		 fftMag[i]=fftMag[i]*(0.54-0.46*cos(2*pi*i/(nWinSize-1)));	 //取一帧数据  
	//    } 

	//	 memset(pDataOut,0,sizeof(double)*nWinSize); 

	//	 //====== Step 4: fast fourier transform.
	//     // 先转换为功率谱 
	//	 //ofstream fout("fft_my.txt",ios::app);
	//     CWaveConvertor::ConvertToPowerSpectral(nWinSize, nWinSize, fftMag, pDataOut);
	//	 //ofstream fout1("fft_my.txt",ios::trunc);
	//	 //for(i=0;i<nWinSize;i++)
 //  //            fout1<<pDataOut[i]<<"  ";
 //  //       fout1.close();


	//   int Fi_up,Fi_down; 
	//   double delt_f =(double)fs / nWinSize; //?? 8000.0，换成8000结果不一样？？？
	//   double MFCC_Y[20], MFCC_k = 0; 
	//    //double MFCC_Y[24], MFCC_k = 0; 
	//   int delt_F_up, delt_F_down;
	//    ofstream fout1("trigle.txt",ios::trunc);
	//for(i = 1; i < 21; i ++) 
	//{
	//	MFCC_Y[i-1] = 0; 
	//	delt_F_up = F[i] - MFCC_k; 
	//	Fi_up = (int)(((double) delt_F_up)/delt_f);
	//	delt_F_down = F[i+1] - (MFCC_k + (Fi_up+1)*delt_f);
	//	Fi_down = (int)(((double) delt_F_down)/delt_f);
	//	for(k = 0; k < Fi_up; k++)
	//	{
	//		fout1<< (MFCC_k - F[i-1])/ (F[i] - F[i-1])<<"    ";
	//		MFCC_Y[i-1] += (MFCC_k - F[i-1]) * pDataOut[k] / (F[i] - F[i-1]); 
	//		MFCC_k += delt_f;
	//	}
	//	fout1<<endl;
	//	for(k = 0; k < Fi_down; k++)
	//	{
	//		fout1<<(F[i+1] - MFCC_k)/ (F[i+1] - F[i])<<"    ";
	//		MFCC_Y[i-1] += (F[i+1] - MFCC_k) * pDataOut[k] / (F[i+1] - F[i]);  
	//		MFCC_k += delt_f;
	//	}
	//	fout1<<endl;
	//	MFCC_k -= delt_f*(Fi_down-1);
	//}
	//fout1.close();
	//  //   ofstream fout1("m_my.txt",ios::trunc);
	//	 //for(i=0;i<20;i++)
 //  //            fout1<<MFCC_Y[i]<<"  ";
 //  //       fout1.close();

	////	for(i=0;i<20;i++)
	////	MFCC_Y[i]=log(MFCC_Y[i]);  //将滤波器组的输出取对数 
	////DCT变换

	//for(i = 0; i < cepsNum; i++) 
	//{
	//	mel[i] = 0;
	//	for(k = 0; k < 20; k++) 
	//	{
	//		mel[i] += log(MFCC_Y[k]) * (cos((double)(i+1) * cos_par[k]));  // mel[1] ……mel[13] 13维 即为提取出来的mfcc参数
	//	}
	//	mfcc[j*cepsNum+i]=mel[i];
	//}
 //}





///////////////////////////////////////////////////////////////////////////////////////////////////////
//	//FP fp;//用于设置提取时参数 
//	//fp.overlap = 0;		    //Frame overlap
// //   fp.tbfNum = 20;			// Number of triangular band-pass filters
// //   fp.cepsNum = 12;			// Dimension of cepstrum
// //   fp.useDelta = 0;			//0 (12fea), 1 (24fea), 2 (36fea)
// //   fp.useEnergy = 1;			// 0, 1
// //   fp.useCMS = 0;				// Cepstral Mean Substraction, 0, 1(cms of all), 2(overlap(cms)= 24), 3(original+cms)
// //   fp.testNum = 4;			// test sentence number, others is train sentences.
// //   fp.alpha = 1;				// For VTLN
// //   fp.upSampling = 2;		    //1, 2
    int i,j;
	int nCount=floor(double(nInLen-nWinSize)/(nWinSize/2)+1);
	double *pDataOut;
	pDataOut=new double[nWinSize];
	//====== Step 1:预加重.
	double *pDataIn1=new double[nInLen];
	pDataIn1[0]=pDataIn[0];
	for(i=1;i<nInLen;i++)
	{
		pDataIn1[i]=pDataIn[i]-0.95*pDataIn[i-1];
	}
	double *fftMag=new double[nWinSize];
	double * filtmag=new double[nWinSize];
	double *tbfCoef=new double[filterNum];	   
	double *mfcc1=new double[cepsNum];
     
	Freq *freq;//三角滤波器，为struct类型
	freq=getTriFilterParam(nWinSize,fs,filterNum);//获取三角滤波器的下、中、上限频率Parameters for triangular filter bank c(l)=h(l-1)=o(l+1)
	
    memset(mfcc,0,sizeof(double)*(cepsNum)*nCount);
	//double *parameter0,*parameter1,*parameter2;
	//parameter0=new double[(cepsNum +1)*nCount];//MFCC_enery
	//parameter1=new double[cepsNum*2*nCount];//MFCC_Delta
	//parameter2=new double[cepsNum*3*nCount];//MFCC_Delta_Delta
    //std::vector <double> parameter[]=new std::vector <double>[nCount];//用于保存mfcc特征参数和后来添加的参数

	 
	 double temp_sm=0.0;
	 int kk=0;
     for(i=0;i<nCount;i++)
	 {   
		 //====== Step 2:分帧.
		 for(j=0;j<nWinSize;j++,kk++)		     
		 {			
			 fftMag[j]=pDataIn1[kk];	 //取一帧数据  
		 }	
		 kk=kk-nWinSize/2;
		 //====== Step 3:加窗.
		 //////////////////////////////////加汉明窗
        for(j=0;j<nWinSize;j++)   
		{			
			 fftMag[j]=fftMag[j]*(0.54-0.46*cos(2*pi*j/(nWinSize-1)));	 //取一帧数据  	
	    } 
		 memset(pDataOut,0,sizeof(double)*nWinSize);

		 //====== Step 4: fast fourier transform.
	     // 先转换为功率谱 
	     CWaveConvertor::ConvertToPowerSpectral(nWinSize, nWinSize, fftMag, pDataOut);
		 pDataOut[0]=0;
         memset(tbfCoef,0,sizeof(double)*filterNum);

         //======Step 5: triangular bandpass filter.
         triBandFilter(nWinSize,pDataOut,filterNum, freq,filtmag,tbfCoef);//返回mel[L]

		 // ====== Step 6: cosine transform. (Using DCT to get L order mel-scale-cepstrum parameters.)
		 melCepstrum(nWinSize,cepsNum, filterNum, tbfCoef,mfcc1); //进行DCT变换   //17号上午修改后通过

		 for(j=0;j<cepsNum;j++)
		 {
			 mfcc[i*cepsNum+j]=mfcc1[j];
			/* parameter0[i*(cepsNum +1)+j]=mfcc[j];
			 parameter1[i*cepsNum*2+j]=mfcc[j];
			 parameter2[i*cepsNum*3+j]=mfcc[j];*/
		 }
		
	 }
/////////////////////////////////////////////////////////////////////////////////////////////////////////
	////====== Add energy
	// double enery,logenery;
	// int useEnergy=1;
	//if(useEnergy ==1)
	//{  
	//	for(i=0;i<nCount;i++)
	//	{
	//		enery=0;
	//		logenery=0;
	//		for(j=0;j<nWinSize;j++)
	//			enery+=pDataIn[i*nWinSize+j]*pDataIn[i*nWinSize+j];
	//		logenery=10*log10(enery/nWinSize);
 //           parameter0[i*(cepsNum +1)+cepsNum]=logenery;
	//		ofstream fout("mfcc_energy.txt",ios::app);
	//	
	//		for(j=0;j<cepsNum+1;j++)
	//			fout<<j<<"    "<<parameter0[i*(cepsNum +1)+j]<<endl;
	//		fout.close ();
	//	}
	//	
	//}

}
////////////////////////////////////////////////////计算mel频谱及mfcc
void CSpeech::GetMelSpectrum(
	unsigned int nWinSize,		    /*处理窗口大小*/
	const double* pDataIn,			/*输入信号序列*/
	unsigned int nInLen,			/*输入信号序列长度*/
	unsigned int fs,               //采样频率
	unsigned int filterNum,         //滤波器个个数
	unsigned int cepsNum,           //mel系数的个数
	double* mfcc,					//输出MFCC
	double* melSpetrum				//输出mel频谱
	)
{
	int i,j;
	int nCount=floor(double(nInLen-nWinSize)/(nWinSize/2)+1);
	double *pDataOut;
	pDataOut=new double[nWinSize];
	//====== Step 1:预加重.
	double *pDataIn1=new double[nInLen];
	pDataIn1[0]=pDataIn[0];
	for(i=1;i<nInLen;i++)
	{
		pDataIn1[i]=pDataIn[i]-0.95*pDataIn[i-1];
	}
	double *fftMag=new double[nWinSize];
	double * filtmag=new double[nWinSize];
	double *tbfCoef=new double[filterNum];	   
	double *mfcc1=new double[cepsNum];

	Freq *freq;//三角滤波器，为struct类型
	freq=getTriFilterParam(nWinSize,fs,filterNum);//获取三角滤波器的下、中、上限频率Parameters for triangular filter bank c(l)=h(l-1)=o(l+1)

	memset(mfcc,0,sizeof(double)*(cepsNum)*nCount);

	double temp_sm=0.0;
	int kk=0;
	for(i=0;i<nCount;i++)
	{   
		//====== Step 2:分帧.
		for(j=0;j<nWinSize;j++,kk++)		     
		{			
			fftMag[j]=pDataIn1[kk];	 //取一帧数据  
		}	
		kk=kk-nWinSize/2;
		//====== Step 3:加窗.
		//////////////////////////////////加汉明窗
		for(j=0;j<nWinSize;j++)   
		{			
			fftMag[j]=fftMag[j]*(0.54-0.46*cos(2*pi*j/(nWinSize-1)));	 //取一帧数据  	
		} 
		memset(pDataOut,0,sizeof(double)*nWinSize);

		//====== Step 4: fast fourier transform.
		// 先转换为功率谱 
		CWaveConvertor::ConvertToPowerSpectral(nWinSize, nWinSize, fftMag, pDataOut);
		pDataOut[0]=0;
		memset(tbfCoef,0,sizeof(double)*filterNum);

		//======Step 5: triangular bandpass filter.
		triBandFilter(nWinSize,pDataOut,filterNum, freq,filtmag,tbfCoef);//返回mel[L]

		for (j=0;j<filterNum;j++)//获得mel频谱
		{
			melSpetrum[i*filterNum+j]=sqrt(exp(tbfCoef[j]));
		}
		// ====== Step 6: cosine transform. (Using DCT to get L order mel-scale-cepstrum parameters.)
		melCepstrum(nWinSize,cepsNum, filterNum, tbfCoef,mfcc1); //进行DCT变换

		for(j=0;j<cepsNum;j++)
		{
			mfcc[i*cepsNum+j]=mfcc1[j];
		}

	}
}

////////////////////////////////////////////////////
void CSpeech::GetMFCC1(
		unsigned int nWinSize,		    /*处理窗口大小*/
	    const double* pDataIn,			/*输入信号序列*/
	    unsigned int nInLen,			/*输入信号序列长度*/
		unsigned int fs,                //采样频率
		unsigned int filterNum,         //滤波器个个数
		unsigned int cepsNum,           //mel系数的个数
	    double* mfcc			           /*输出MFCC*/
	)
{    int i,j;
	int nCount=floor(double(nInLen-nWinSize)/(nWinSize/2)+1);
	double *pDataOut;
	pDataOut=new double[nWinSize];
	//====== Step 1:预加重.
	double *pDataIn1=new double[nInLen];
	pDataIn1[0]=pDataIn[0];
	for(i=1;i<nInLen;i++)
	{
		pDataIn1[i]=pDataIn[i]-0.95*pDataIn[i-1];
	}
	double *fftMag=new double[nWinSize];
	double * filtmag=new double[nWinSize];
	double *tbfCoef=new double[filterNum];	   
	double *mfcc1=new double[cepsNum];
     
	//Freq *freq;//三角滤波器，为struct类型
	//freq=getTriFilterParam(nWinSize,fs,filterNum);//获取三角滤波器的下、中、上限频率Parameters for triangular filter bank c(l)=h(l-1)=o(l+1)
    
    memset(mfcc,0,sizeof(double)*(cepsNum)*nCount);
	//double *parameter0,*parameter1,*parameter2;
	//parameter0=new double[(cepsNum +1)*nCount];//MFCC_enery
	//parameter1=new double[cepsNum*2*nCount];//MFCC_Delta
	//parameter2=new double[cepsNum*3*nCount];//MFCC_Delta_Delta
    //std::vector <double> parameter[]=new std::vector <double>[nCount];//用于保存mfcc特征参数和后来添加的参数

	 
	 double temp_sm=0.0,*temp=new double[filterNum];
	 int kk=0;
	 int k;
     for(i=0;i<nCount;i++)
	 {   
		 //====== Step 2:分帧.
		 for(j=0;j<nWinSize;j++,kk++)		     
		 {			
			 fftMag[j]=pDataIn1[kk];	 //取一帧数据  
		 }	
		 kk=kk-nWinSize/2;
		 //====== Step 3:加窗.
		 //////////////////////////////////加汉明窗
        for(j=0;j<nWinSize;j++)   
		{			
			 fftMag[j]=fftMag[j]*(0.54-0.46*cos(2*pi*j/(nWinSize-1)));	 //取一帧数据  	
	    } 
		 memset(pDataOut,0,sizeof(double)*nWinSize);

		 //====== Step 4: fast fourier transform.
	     // 先转换为功率谱 
	     CWaveConvertor::ConvertToPowerSpectral(nWinSize, nWinSize, fftMag, pDataOut);
         memset(tbfCoef,0,sizeof(double)*filterNum);

         //======Step 5: triangular bandpass filter.
         //triBandFilter(nWinSize,pDataOut,filterNum, freq,filtmag,tbfCoef);//返回mel[L]
 
		 memset(temp,0,sizeof(double)*filterNum);

		for(k=0;k<filterNum;k++)
	   {
		   if(k<16)			
		   {
			   for(j=0;j<5;j++)			
				{
					temp[k]=temp[k]+fftMag[k*5+j]*filtmag[k*5+j];	
					 //fout<<filtmag[j-1]<<"       ";	
				}
				tbfCoef[k]=log(temp[k]*temp[k]);
		   }
		   else
		   {
			   for(j=0;j<6;j++)			
				{
					temp[k]=temp[k]+fftMag[80+(k-16)*6+j]*filtmag[80+(k-16)*6+j];	
					 //fout<<filtmag[j-1]<<"       ";	
				}
				tbfCoef[k]=log(temp[k]*temp[k]);

		   }
			//fout<<tbfCoef[i]<<endl;
        }

		 // ====== Step 6: cosine transform. (Using DCT to get L order mel-scale-cepstrum parameters.)
		 melCepstrum(nWinSize,cepsNum, filterNum, tbfCoef,mfcc1); //进行DCT变换   //17号上午修改后通过

		 for(j=0;j<cepsNum;j++)
		 {
			 mfcc[i*cepsNum+j]=mfcc1[j];
			/* parameter0[i*(cepsNum +1)+j]=mfcc[j];
			 parameter1[i*cepsNum*2+j]=mfcc[j];
			 parameter2[i*cepsNum*3+j]=mfcc[j];*/
		 }
		
	 }
}
//////////////////////////////////////////////////////
//实际频率到mel频率的转换
double CSpeech::freq2mel(double freq)
{
	double mel= 2595*log10(1+freq/700);
	return mel;
}
 double CSpeech::mel2freq(double mel )//mel频率到实际频率的转换
 {
	 double freq= 700*(pow(10,(mel/2595))-1);
	 return freq;
 }
//获取三角形滤波器的下限、中限、上限频率
Freq*  CSpeech::getTriFilterParam(int framesize, //窗口大小     -------------------(测试通过)10月16号晚
									 unsigned int fs,//采样频率
									 int filterNum)//滤波器的数
{
	double maxMelFreq = freq2mel(fs/2);//相当于B(fh),f1=0
    double sideWidth=maxMelFreq/(filterNum+1);
	int i;
	int index;
	double *temp;
	temp=new double[filterNum+2];
	Freq *freq;
	for(index=0;index<filterNum+2;index++)
		temp[index]=floor(mel2freq(index*sideWidth)/fs*framesize)+1;//floor()返回小于或者等于指定表达式的最大整数
	freq=new Freq[filterNum];
	//ofstream fout("freq.txt",ios::trunc);
	for(i=0,index=0;i<filterNum;i++,index++)
	{
		freq[i].start=temp[index];
		freq[i].center=temp[index+1];
		freq[i].stop =temp[index+2];
		//fout<<i<<"   "<<freq[i].start <<"    "<<freq[i].center <<"   "<<freq[i].stop <<endl;
	}
	freq[filterNum-1].stop =framesize/2;
	//fout.close();
	if(temp!=NULL)
		delete []temp;
    return freq;
}
//获取三角带通滤波器(Triangular band-pass filters)的输出mel[L].------------------------------------------(测试通过)10月16号晚
void CSpeech::triBandFilter(
					  int framesize,
					  double *fftMag,//傅里叶变换后的abs
					  int filterNum,//滤波器的数目
					  Freq* freq,//滤波器的三个限值
					  double* filtmag,//保存wl(k)
					  double * tbfCoef//返回值
					  )
{
	double *fstart,*fcenter,*fstop;
    fstart=new double[filterNum];
	fcenter=new double[filterNum];
	fstop=new double[filterNum];
	
    memset(filtmag,0,sizeof(double)*framesize/2);
	memset(tbfCoef,0,sizeof(double)*filterNum);//保存返回值，为DCT变换后的值
	double * temp=new double[filterNum];//保存中间计算结果
	memset(temp,0,sizeof(double)*filterNum);

	int i,j;
	
	for(i=0;i<filterNum;i++)
	{
		fstart[i]=freq[i].start;
		fcenter[i]=freq[i].center ;
		fstop[i]=freq[i].stop ;		
	}
	
	//ofstream fout("tbfCoef.txt",ios::trunc);
	
	for(i=0;i<filterNum;i++)
	{	
		for(j=fstart[i];j<=fcenter[i];j++)			 
			filtmag[j-1] = (j-fstart[i])/(fcenter[i]-fstart[i]);
	    for(j=fcenter[i]+1;j<=fstop[i];j++)
			filtmag[j-1] = (fstop[i]-j)/(fstop[i]-fcenter[i]);  
		for(j=fstart[i];j<=fstop[i];j++)			
		{
			temp[i]=temp[i]+fftMag[j-1]*filtmag[j-1];	
		     //fout<<filtmag[j-1]<<"       ";	
		}
		tbfCoef[i]=log(temp[i]*temp[i]);
		//fout<<tbfCoef[i]<<endl;
    } 
	//fout.close();
	
	if(temp!=NULL)
		delete []temp;
	if(fstop!=NULL)
		delete []fstop;	
	if(fcenter!=NULL)
		delete []fcenter;
    if(fstart!=NULL)
		delete []fstart;	
	
}
// TBF coefficients to MFCC
void  CSpeech:: melCepstrum( unsigned int nWinSize,
							  int L,//mfcc系数个数
						       int filterNum,//滤波器个数
						       double* tbfCoef,//DCT转换后的数据
							   double* mfcc//保存返回值
						   )
{
	int i,j;
	double a,b; 	
	memset(mfcc,0,sizeof(double)*L);
	for(i=1;i<=L;i++)
	{  
		for(j=0;j<filterNum;j++)			
			mfcc[i-1]=mfcc[i-1]+tbfCoef[j]*cos(i*PI/filterNum*(j+0.5));
		//mfcc[i-1]=sqrt((double)2/nWinSize)*mfcc[i-1];
	}
} 
////////////////////////////////////////////////////////////////////MFCC参数进行Delta运算
//double * CSpeech:: deltaFunction( int deltaWindow,   
//							      double parameter
//							  )
//{
//	double *temp;
//	return temp;
//
//}
//////////////////////***********张智星版本MFCC（完）***********////////////////////
///////////////////////////////赵力版LPC、LPCC、LPCCMFCC求法//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//void  CSpeech::correl( const double* pDataIn,		        	/*输入的加窗后的信号序列*/
//		               double* rcor,               /*信号的自相关函数*/
//					   unsigned int nWinSize,          /*数据窗的长度*/
//					   int ipnum                           /*LPC分析的阶数*/	               
//)
//{
//  int i,j;
//  double u0,u;
//  u0=0.0;
//  for(i=0;i<nWinSize;i++)
//	  u0+=pDataIn[i]*pDataIn[i];
//  if(u0>0)
//  {
//	  for(i=1;i<=ipnum;i++)
//	  {
//		  u=0.0;
//		  for(j=0;j<nWinSize-1;j++)
//			  u+=pDataIn[j]*pDataIn[j+i];
//		  rcor[i]=u/u0;
//	  }
//  }
//  else
//  {
//	  for(i=1;i<=ipnum;i++)
//		  rcor[i]=0;
//  }
//  rcor[0]=u0;
//
//}
	//////////////////////////////////////赵力————由自相关函数计算LPC预测系数和反射系数
//void  CSpeech::corref(int ipnum,                          /*LPC分析的阶数*/
//		               double * cor,                    /*信号的自相关函数cor[1]~cor[ip]*/
//					   double * alf,                    /*LPC系数（alf[1]~alf[n],注意：alf[0]=1）*/
//					   double *ref,                     /*反射系数(ref[1]~ref[ip],符号与常规的定义相反)*/
//					   double resid                     /*LPC预测残差*/
//		)
//{
//	int i,j;
//	double refi,alfsave;//refi相当于Ki,
//	ref[1]=cor[1];
//	alf[1]=(-ref[1]);
//	alf[0]=1.0;
//	resid=1-ref[1]*ref[1];
//	for(i=2;i<=ipnum;i++)
//	{
//		refi=cor[i];
//		for(j=1;j<i;j++)
//			refi+=alf[j]*cor[i-j];
//	    refi=refi/(resid);
//		ref[i]=refi;
//		alf[i]=(-refi);
//	    for(j=1;2*j<=i;j++)
//		{
//			alfsave=alf[j];
//			alf[j]=alfsave-refi*alf[i-j];
//			if(2*j!=i)
//				alf[i-j]-=refi*alfsave;
//		}
//		resid=(resid)*(1.0-refi*refi);
//
//	}
//}
//     /////////////////////////////////////赵力————由LPC预测系数计算LPC倒谱系数
//void  CSpeech::alfcep( int ipnum,                /*LPC分析的阶数*/
//		                double *alf,           /*LPC预测系数（alf[1]~alf[ip]）*/
//						double *cep,           /*LPC倒谱系数（cep[1]~cep[n],注意：cep[0]在本程序中未用）*/
//						int n                  /*LPC倒谱系数的阶数*/
//		)
//{
//	int m,i,j;
//	double ss;
//	m=0;
//
//
//}
//	