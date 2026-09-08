#include"StdAfx.h"
#include"VQ.h"
#include"HMM.h"
///////////////////////////////////
//构造函数
//////////////////////////////////
CHMM::CHMM()
{}
CHMM::~CHMM()
{}

//////////////////////////////////////////////////////////////////////////
// 实现对HMM模型的迭代训练
// 
// 创建人:		
// 创建日期:	
// 修改人:
// 修改日期:
void CHMM::Train (
	    const double * pDataIn,   //输入采样序列
		unsigned int nInLen       //输入采样序列长度
		)
{
	//前后向算法返回参数
	double fRate = 0;
	double* pa = NULL;
	double* pb = NULL;

	//对输入采样序列进行矢量量化形成的码本
	double* pCodeBook = NULL;
	//输入码本对应的HMM模型下标
	unsigned int* pCodeIndex = NULL;
	
	//判断输入数据是否合法
	if(pDataIn != NULL && this->m_pA != NULL &&this->m_pB != NULL && this->m_pPi != NULL)
	{
		pa = new double[this->m_nCodeNums * this->m_nStatusNums];
		pb = new double[this->m_nCodeNums * this->m_nStatusNums];

		//对输入采样序列进行矢量量化
		pCodeBook = new double[this->m_nCodeNums];
		CVQ::KMeansCluster(pDataIn, nInLen, pCodeBook, this->m_nCodeNums);

		//获得输入码本对应的HMM模型码本下标
		pCodeIndex = new unsigned int [this->m_nCodeNums];
		CVQ::Classify (pCodeBook, this->m_nCodeNums, this->m_pCodeBook, this->m_nCodeNums,pCodeIndex);

		//调用前后向算法，计算pa,pb
		fRate = CHMM::ForwardBackward (pCodeIndex, this->m_nCodeNums ,
			this->m_pPi, this->m_pA, this->m_pB, this->m_nStatusNums, pa, pb);
        
		//调用BW算法进行参数优化
		CHMM::BaumWelch (pCodeIndex, this->m_nCodeNums ,
			this->m_pPi, this->m_pA, this->m_pB, this->m_nStatusNums, pa, pb, fRate);

		//更新码本
		for(unsigned int i=0; i < this->m_nCodeNums; i++)
		{
			this->m_pCodeBook [i] = this->m_pCodeBook [i] *0.7 +pCodeBook[i] * 0.3;
		}
        
		delete [] pa;
		delete [] pb;
		delete [] pCodeBook;
		delete [] pCodeIndex;
	}
}
//////////////////////////////////////////////////////////////////////////
// 实现对HMM模型得初始化训练
// 
// 创建人:		 
// 创建日期:	 
// 修改人:
// 修改日期:
void CHMM::PrepareTrain (
		CString strWord,            //模型对应词
		const double*  pDataIn,     //第一个输入采样序列
		unsigned int   nInLen,      //输入采样序列长度
		unsigned int   nFrameSize,  //分帧宽度
		unsigned int   nStatusNums, //HMM状态数
		unsigned int   nCodeNums    //输入码本长度
		)
{
	//计算概率
	double fRate = 0;
	//循环变量
	unsigned int i = 0, j =0;

	//设置模型信息
	this->m_strWord = strWord;
	this->m_nStatusNums = nStatusNums;
	this->m_nCodeNums = nCodeNums;
	this->m_nFrameSize = nFrameSize;

	//初始化pI/pA/pB/pCodeBook
	this->m_pPi = new double[this->m_nStatusNums];
	this->m_pA = new double[this->m_nStatusNums * this->m_nStatusNums];
	this->m_pB = new double[this->m_nStatusNums * this->m_nCodeNums];
	this->m_pCodeBook = new double[this->m_nCodeNums];

	memset(this->m_pPi, 0, sizeof(double) * this->m_nStatusNums);
	memset(this->m_pA, 0, sizeof(double) * this->m_nStatusNums * this->m_nStatusNums);
	memset(this->m_pB, 0, sizeof(double) * this->m_nStatusNums * this->m_nCodeNums);
	memset(this->m_pCodeBook, 0, sizeof(double) * this->m_nCodeNums);

	//对pI进行均分
	fRate = (double) 1 / nStatusNums;
	for(i=0; i < nStatusNums; i++)
	{
		this->m_pPi[i] = fRate;
	}
	//对pA[i][j]进行均分
	for(i=0;i < nStatusNums; i++)
	{
		for(j=0; j < nStatusNums; j++)
			this->m_pA[i * nStatusNums + j] = fRate;
	}
	//对pB[i][j]进行均分
	fRate = (double) 1 / nCodeNums;
	for(i=0;i < nCodeNums; i++)
	{
		for(j=0; j < nCodeNums; j++)
			this->m_pA[i * nCodeNums + j] = fRate;
	}
	//形成模板码本
	CVQ::KMeansCluster (pDataIn, nInLen, this->m_pCodeBook, this->m_nCodeNums);

	////进行参数优化
	//this->Train(pDataIn, nInLen);

}
//////////////////////////////////////////////////////////////////////////
// 实现前后向算法,对于给定的HMM参数和观察序列O，
// 求能够产生观察序列O的概率
// 
// 创建人:		
// 创建日期:	
// 修改人:
// 修改日期:
double CHMM::ForwardBackward (
		const unsigned int * pCodeBook,//输入观察码本序列对应的HMM码本的下标
		unsigned int nCodeNums,        //输入观察序列长度
		const double* pPi,             //HMM的pi矢量
		const double* pA,              //HMM的A矩阵
		const double* pB,              //HMM的B矩阵
		unsigned int nStatusNums,      //HMM状态数
		double* pa,                    //前后向算法中的a
		double* pb                     //前后向算法中的b
							 )
{
	//能够产生观察序列O(大写的o)的概率
	double fRate=0;
	//递推时做累积
	double fSum=0;
	
	int t=0;
	int i=0;
	int j=0;

	//初始化pa,pb
	memset(pa,0,sizeof(double)*(nCodeNums*nStatusNums));
	memset(pb,0,sizeof(double)*(nCodeNums*nStatusNums));

	//实现前后向算法
/*********************************************************************************************韩纪庆版本书P204*/
	//初始化
	for(i=0;i<nStatusNums;i++)
	{
		//计算公式：pa[1][i]=pI[i]*pB[i][o1]   1<=i<=N   
		pa[0*nStatusNums+i]=pPi[i]*pB[i*nCodeNums+pCodeBook[0]];

		//计算公式：pb[nCodeNums-1][i]=1;
		pb[(nCodeNums-1)*nStatusNums+i]=1;
	}
///********************************************************************************************************赵力版*/
////初始化初始状态S1的数组变量
//	pa[0*nStatusNums+0]=1;//pa[0][1]=1;
//	pb[(nCodeNums-1)*nStatusNums+nStatusNums-1]=1;//pb[T][N]=1
//	for(j=1;j<nStatusNums;j++)
//	{
//		//计算公式：pa[0][j]=0  2<=j<=N   
//		pa[0*nStatusNums+j]=0;
//
//		//计算公式：pb[nCodeNums-1][j]=0;j!=N
//		pb[(nCodeNums-1)*nStatusNums+j-1]=0;
//	}

	/****************************************************************************************/
	//前向递推
	for(t=1;t<nCodeNums;t++)
	{
		for(j=0;j<nStatusNums;j++)
		{
			fSum=0;

			for(i=0;i<nStatusNums;i++)
			{
				//累积 fSum+=pa[t-1][i]*pA[i][j]
				fSum+=pa[(t-1)*nStatusNums+i]*pA[i*nStatusNums+j];
			}
			//计算公式：pa[t][j]=fSum*pB[i][ot]
			pa[t * nStatusNums + j] = fSum * pB[j * nCodeNums + pCodeBook[t]];
		}
	}

	//后向递推
	for(t=nCodeNums-2;t>=0;t--)
	{
		for(i=0;i<nStatusNums;i++)
		{
			fSum=0;
			
			for(j=0;j<nStatusNums;j++)
			{
				//累积：fSum+=pA[i][j]*pB[j][t+1]*pb[t+1][j]
				fSum+=pA[i*nStatusNums+j]*pB[j*nCodeNums+pCodeBook[t+1]]*pb[(t+1)*nStatusNums+j];
			}
			pb[t * nStatusNums + i] = fSum;
		}
	}
	//结束
	//计算概率，令 t=nCodeNums-1
	t=nCodeNums-1;
	for(i=0;i<nStatusNums;i++)
	{
		//计算公式 fRate+=pa[t][i]*pb[t][i];
		fRate+=pa[t*nStatusNums+i]*pb[t*nStatusNums+i];//此时pb[t*nStatusNums+i]=1;
	}
	return fRate;
}
//////////////////////////////////////////////////////////////////////////
// 实现Viterbi算法,对于给定的HMM参数和观察序列O，
// 返回最大概率
// 
// 创建人:		
// 创建日期:	
// 修改人:
// 修改日期:
double CHMM::Viterbi(
		const unsigned int* pCodeBook, //输入观察码本序列对应的HMM码本的下标
		unsigned int nCodeNums,        //输入观察序列长度
		const double* pPi,             //HMM的pi矢量
		const double* pA,              //HMM的A矩阵
		const double* pB,              //HMM的B矩阵
		unsigned int nStatusNums       //HMM状态数
		)
{
	//pa[t][i]为t时刻沿着一条路径q1,q2,....,qt,且qt=st
	//产生出输入观察序列o1,o2,o3,.....,ot的最大概率
	//1 <= t <= nInLen,1 <= i <= nStatuesNums
	double* pa=NULL;
    double* pb=NULL;//用于保存每一次使pa(j)最大的状态i
	//临时计算
	double fTemp=0;
	double fMax=0;

	//最大概率
	double fMaxRate=0;

	//循环变量
	//采样序列循环
	unsigned int t=0;
	//状态循环
	unsigned int i=0,j=0;

	//分配和初始化pa
	pa=new double[nCodeNums*nStatusNums];
	memset(pa,0,sizeof(double)*(nCodeNums * nStatusNums));
    pb=new double[nCodeNums*nStatusNums];
	memset(pb,0,sizeof(double)*(nCodeNums * nStatusNums));
	//初始化
	for(i=0;i< nStatusNums;i++)
	{
		//计算公式：pa[1][i]=pi[i]*pB[i][1]
	    pa[0 * nStatusNums + i] = pPi[i] * pB[i * nCodeNums + pCodeBook[0]];
		pb[0 * nStatusNums + i]=0;
	 }
	//递推
	for(t=1;t< nCodeNums;i++)
	{	for(j=0;j< nStatusNums;j++)
		{
			//求pa[t-1][i]*pA[i][j]的最大值，1 <= i <= N
			fMax=pa[(t-1)*nStatusNums+0]*pA[0*nStatusNums+j];

			for(i=0;i< nStatusNums;i++)
			{
				fTemp=pa[(t-1)* nStatusNums + i]*pA[0 * nStatusNums + j];
				fMax=( fMax < fTemp ? fTemp : fMax );
			}

			//计算pa[t][j]和pb[t][j]
			//计算公式： pa[t][j]=fMax * pB[j][t];
			pa[t* nStatusNums + j]=fMax * pB[j * nCodeNums +pCodeBook[t]];
    	}	  
	}
	//终结
	//求最大概率,fMaxRate=max(pa[nCodeNums-1][i])  1 <= i <=nStatusNums
	fMaxRate = pa[(nCodeNums - 1) * nStatusNums + 0];
	for( i=0; i< nStatusNums; i++)
	{
		fTemp = pa[(nCodeNums - 1) * nStatusNums + i];
		fMaxRate =(fMaxRate < fTemp ? fTemp : fMaxRate);
	}

	delete []pa;
	
	return fMaxRate;
}
//////////////////////////////////////////////////////////////////////////
// 实现Baum Welch算法,对于给定的观察序列O，
// 求使P(O | HMM参数)最大的HMM参数pi, A, B
// 
// 创建人:	
// 创建日期:	
void CHMM::BaumWelch(
		const unsigned int* pCodeBook,	// 输入观察码本序列对应的HMM码本的下标
		unsigned int nCodeNums,			// 输入观察序列长度
		double* pPi,					// HMM的pi矢量
		double* pA,						// HMM的A矩阵
		double* pB,						// HMM的B矩阵
		unsigned int nStatusNums,		// HMM状态数
		double* pa,						// 前后向算法计算所得的pa
		double* pb,						// 前后向算法计算所得pb
		double fRate					// 前后向算法计算所得最大概率
		)
{
	//bw算法中概率矩阵
	//pRate[nCodeNums][nStatusNums][nStatusNums]
	double * pRate=NULL;
	//最小概率
	double fMinRate=0.0001;
	unsigned int nOffSet = nStatusNums * nStatusNums;

	//循环变量
	unsigned int t = 0;
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int k = 0;

	//求概率时的分子
	double fAvgUp = 0;
	//求概率时的分母
	double fAvgDown = 0;
	//数组元素下标
	unsigned int nIndex = 0;

	//初始化概率矩阵
	pRate = new double [nCodeNums * nOffSet];
	memset(pRate ,0 ,sizeof(double) * nCodeNums *nOffSet);

	//计算pRate[t][i][j],即t时刻Markov链处于i状态，t+1时刻处于j状态的概率
	for(t=0; t < (nCodeNums -1); t++)
	{
		for( i=0; i < nStatusNums; i++)
		{
			for(j=0; j < nStatusNums; j++)
			{
				//计算公式： pRate[t][i][j]=pa[t][i]*pA[i][j]*pB[j][O(t+1)]*pb[t+1][j]/fRate;
				pRate[t*nOffSet + i * nStatusNums + j]=
					(pa[t * nStatusNums + i] * pA[i * nStatusNums + j] * 
					pB[j * nCodeNums + pCodeBook[t+1]] * pb[(t+1) * nStatusNums + j])/fRate;
			}
		}
	}
	//使用重估公式计算HMM参数
	//计算Pi矢量，即0时刻时Markov链处于i状态，t+1时刻处于其他任何状态的概率
	for( i=0; i < nStatusNums; i++)
	{
		pPi[i]=0;

		for(j=0; j< nStatusNums; j++)
		{
			pPi[i] += pRate[0 * nOffSet + i * nStatusNums + j];
		}
	}

	//计算A矩阵
	//pA[i][j] =(从状态Si过渡到Sj的平均次数) / (从状态Si向其他状态转移的平均次数)
	for(i=0; i < nStatusNums; i++)
	{
		for(j=0; j < nStatusNums; j++)
		{
			 //计算状态Si过渡到Sj的平均次数
			fAvgUp = 0;
			//从状态Si向其他状态转移的平均次数
			fAvgDown = 0;

			for(t=0; t < nCodeNums-1; t++)
			{
				fAvgUp += pRate[t * nOffSet + i * nStatusNums +j];

				for(k=0; k < nStatusNums; k++)
				{
					fAvgDown += pRate[t * nOffSet + i * nStatusNums + k];
				}
			}
			nIndex = i * nStatusNums +j;
			pA[nIndex]= fAvgUp / fAvgDown;
			pA[nIndex]=(pA[nIndex] > 0.0 ? pA[nIndex] : fMinRate);			
		}
	}

	//计算B矩阵
	// pB[i][k]= (出现状态i和观察值Ot=Vk的平均次数) / (处于状态i的次数)
    for(i=0; i < nStatusNums; i++)
	{
		for(k=0; k < nCodeNums; k++)
		{
			//计算出现状态i和观察值Ot=Vk的平均次数
			fAvgUp = 0;
			
			for(t=0; t < nCodeNums; t++)
			{
				if(pCodeBook[t] == pCodeBook[k])
				{
					for(j=0; j< nStatusNums; j++)
					{
						fAvgUp += pRate[t * nOffSet + i * nStatusNums +j];
					}
				}
			}
			//处于状态i的次数
			fAvgDown=0;
			for(t=0; t < nCodeNums; t++)
			{
				for(j=0; j < nStatusNums; j++)
				{
					fAvgDown += pRate[t * nOffSet + i * nStatusNums + j];
				}
			}
			//计算pB[i][k]
			nIndex = i * nCodeNums +pCodeBook[k];
			pB[nIndex]= fAvgUp / fAvgDown;
			pB[nIndex]= (pB[nIndex] > 0.0 ? pB[nIndex] : fMinRate);
		}
	}
    // 释放资源
	if (pRate != NULL)
	{
		delete[] pRate;
	} 
}