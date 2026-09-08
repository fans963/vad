#include "stdafx.h"
#include"VQ.h"
#include<math.h>

//////////////////////////////////////////////////////////////////////////
// 构造函数
// 
// 创建人:		 
// 创建日期:	 
// 修改人:
// 修改日期:
CVQ::CVQ(void)
{
}

//////////////////////////////////////////////////////////////////////////
// 析造函数
// 
// 创建人:		 
// 创建日期:	 
// 修改人:
// 修改日期:
CVQ::~CVQ(void)
{
}
//////////////////////////////////////////////////////////////////////////
// 采用LBG算法进行VQ
// 
// 创建人:		
// 创建日期:	
// 修改人:
// 修改日期:
void CVQ::LBG( 
			  const double* pInVector,		// 输入样本序列
			  unsigned int nInLen,			// 输入样本序列长度
			  double* pOutCodeBook,			// 输出码本
			  unsigned int nCodeNums,		// 码本长度
			  unsigned int nMaxReapt,		// 最大迭代次数
			  double fMinChange,			// 畸变改进阈值
			  double fInitDistortion		// 初始畸变
			  )
{
	//当前总畸变
	double fCurDistortion = fInitDistortion;
	//调用LBGRepeat返回的总畸变
	double fRetDistortion = 0;
	//当前迭代次数
	unsigned int nCurRepeat = 0;
	//当前畸变改进量
	double fCurChange = fInitDistortion;
	
	//若初始码本为空，则用输入序列的前nCodeNums个样本为初始码本
	if( pOutCodeBook == NULL)
	{
		memcpy(pOutCodeBook, pInVector, sizeof(double) * nCodeNums);
	}

	//调用LBGRepeat进行迭代
	while(nCurRepeat < nMaxReapt && fMinChange < fCurChange)
	{
		fRetDistortion = CVQ::LBGRepeat (pInVector, nInLen, pOutCodeBook, nCodeNums);

		//计算当前畸变改进量
		fCurChange = abs(fCurDistortion - fRetDistortion) / fCurDistortion;
		fCurDistortion = fRetDistortion;
	}

}
//////////////////////////////////////////////////////////////////////////
// 进行LBG算法迭代，输出总畸变
// 
// 创建人:		 
// 创建日期:	 
// 修改人:
// 修改日期:
double CVQ::LBGRepeat( 
					const double* pInVector,		// 输入样本序列
					unsigned int nInLen,			// 输入样本序列长度
					double* pCodeBook,				// 输入/出码本
					unsigned int nCodeNums			// 码本长度
					)
{
	//聚类是每个码矢代表的样本数，相当于Matlab版中的N1
	unsigned int *pnSampleNums=new unsigned int[nCodeNums];
	//聚类是进行样本叠加值，相当于MATLAB版中的y(m)
	double* pfSampleSums=new double[nCodeNums];
	//保存总畸变
	double fDistortion = 0;
	//保存相邻的码矢下标
	unsigned int nNearCode = 0;
	//保存样本与码矢距离
	double fDistance = 0;
	//循环变量
	unsigned int i= 0, j =0;

	//初始化聚类时每个码矢代表的样本数
	memset(pnSampleNums, 0, sizeof(unsigned int) * nCodeNums);
	//初始化聚类时进行样本叠加值
	memset(pfSampleSums, 0,sizeof(double) * nCodeNums);

	//按照输入码本对输入样本序列进行分段
	for(i=0; i < nInLen; i++)
	{
		fDistance =CVQ_MAXIMUN;
		for(j=0; j < nCodeNums; j++)
		{
			if(abs(pInVector[i]-pCodeBook[j]) < fDistance)
			{
				fDistance = abs(pInVector[i]-pCodeBook[j]);

				nNearCode = j;
			}
		}
		//计算总畸变
		fDistortion+=fDistance;
		pfSampleSums[nNearCode] +=pInVector[i];
		pnSampleNums[nNearCode]++;
	}
	//计算新码本
	for(i=0;i < nCodeNums; i++)
	{
		pCodeBook[i]= pfSampleSums[i] / pnSampleNums[i];
	}
	delete [] pnSampleNums;
	delete [] pfSampleSums;

	return fDistortion;
}
//////////////////////////////////////////////////////////////////////////
// 对输入样本进行简单聚类
// 先对输入样本进行分段，然后简单的求每个分段的均值作为码本
// 产生的码本与时间t相关
//
// 创建人:		
// 创建日期:	
// 修改人:
// 修改日期:
void CVQ::EasyCluster (
		const double * pInVector,        //输入样本序列
		unsigned int   nInLen,           //输入样本序列长度
	    double * pCodeBook,              //输入/出码本
		unsigned int   nCodeNums         //码本长度
					   )
{
	unsigned int nSegLen = nInLen / nCodeNums;
	unsigned int nOffSet = 0;

	//循环变量
	unsigned int i=0,j=0;

	//初始化码本
	memset(pCodeBook, 0, sizeof(double) * nCodeNums);

    for(i=0; i < nCodeNums; i++)
	{
		nOffSet = i * nSegLen;

		for(j=0; j < nSegLen; j++)
			pCodeBook[i] += pInVector[nOffSet + j];
		
		//对码矢求均值
		pCodeBook[i] = pCodeBook[i] /  nSegLen;
	}
}
//////////////////////////////////////////////////////////////////////////
// 实现K均值聚类
//
// 创建人:		 
// 创建日期:	 
// 修改人:
// 修改日期:
void CVQ::KMeansCluster (
	    const double * pInVector,        //输入样本序列
		unsigned int   nInLen,           //输入样本序列长度
	    double * pCodeBook,              //输入/出码本
		unsigned int   nCodeNums         //码本长度
						 )
{
	//pInVector(i)是否属于pCodeBook(j)
	unsigned int * pFlag = NULL;
	//迭代中使用到的码本
	double * pNewCodeBook = NULL;

	//循环变量
	unsigned int i=0,j=0;

	//迭代返回的畸变
	double fDistortion = 0;

	//对码本进行升序排序
	double fMinCode = 0;
	unsigned int nMinIndex = 0;

	if(pInVector != NULL && pCodeBook != NULL)
	{
		//分配化标志矩阵
		pFlag = new unsigned int [nCodeNums * nInLen];
		//分配迭代码本
		pNewCodeBook = new double [nCodeNums];

		//使用EasyCluster设定初始码本
		CVQ::EasyCluster (pInVector, nInLen, pCodeBook, nCodeNums);
	

	    //迭代聚类
	    fDistortion = 1;
	    while(fDistortion > 0)
	    {
	         fDistortion = CVQ::KMeansClusterRepeat (
		  	     pInVector, nInLen, pCodeBook, nCodeNums, pFlag, pNewCodeBook);

		     //拷贝新码本
		     memcpy(pCodeBook, pNewCodeBook, sizeof(double) * nCodeNums);
	     }

	     //对码本进行升序排列
	    for( i=0; i < nCodeNums; j++)
	   {
		   fMinCode = pCodeBook[i];
		   for( j=i+1; j< nCodeNums; j++)
		   {
			   if(pCodeBook[j] < fMinCode)
			   {
			       fMinCode = pCodeBook[j];
			       nMinIndex = j;
			    }
		    }
		    pCodeBook[nMinIndex] = pCodeBook[i];
		    pCodeBook[i] = fMinCode;
		}   
        //释放所占资源
		delete [] pNewCodeBook;
		delete [] pFlag;
	}
}
//////////////////////////////////////////////////////////////////////////
// 实现K均值聚类的迭代过程， 输出码书的畸变程度
//
// 创建人:		 
// 创建日期:	 
// 修改人:
// 修改日期:
double CVQ::KMeansClusterRepeat (
		const double* pInVector,      //输入样本序列
		unsigned int  nInLen,         //输入样本序列长度
		double*       pCodeBook,      //输入码本
		unsigned int  nCodeNums,      //码本长度
		unsigned int* pFlag,          //用于分类的数组，再KMeansCluster中分配
		double*       pNewCodeBook    //输出新码本
		 )
{
	//畸变
	double fDistortion = 0;
	//样本和码本的最小距离
	double fMinDis = 0;
	//计算样本和码本的距离
	double fDis = 0;
	//最小距离对应的码本下标
	unsigned int nIndex = 0;
	 
	//码本包含样本数
	unsigned int nSamplesCount = 0;
	//码本中样本累积
	double fSamplesSum = 0;

	//循环变量
	unsigned int i =0, j = 0;

	if(pInVector != NULL)
	{
		//初始化输出新码本
		memset(pNewCodeBook, 0, sizeof(double) * nCodeNums);
		//初始化标志矩阵
		memset(pFlag, 0, sizeof(unsigned int )* nCodeNums * nInLen);
		 
		//聚类
		for( i=0; i< nInLen; i++)
		{
			//初始最小值
			fMinDis = abs(pInVector[i] - pCodeBook[0]);
			nIndex = 0;

			for(j=0; j < nCodeNums; j++)
			{
				fDis = abs(pInVector[i] - pCodeBook[j]);
				
				if(fDis < fMinDis)
				{
					fMinDis = fDis;
					nIndex = j;
				}
			}

			//设定pInVector[i]属于中心pCodeBook[nIndex]
			pFlag[nIndex * nInLen + i] =1;   
		}

		//计算新码本
		for(i=0; i < nCodeNums; i++)
		{
			fSamplesSum = 0;
			nSamplesCount =0;

			for(j=0;j < nInLen; j++)
			{
				if(pFlag[i * nInLen + j] == 1)
				{
					nSamplesCount++;
					fSamplesSum += pInVector[j];
				}
			}

			//当堆大小为0时，对应码矢为0
			pNewCodeBook[i] =(nSamplesCount == 0? 0: (fSamplesSum / nSamplesCount));
			fDistortion += abs(pCodeBook[i] - pNewCodeBook[i]);
		}
		//计算畸变
		fDistortion = fDistortion / nCodeNums;
	}
	return fDistortion;
}
//////////////////////////////////////////////////////////////////////////
// 对输入码本，按照标准码本进行分类
//
// 创建人:		
// 创建日期:	
// 修改人:
// 修改日期:
void CVQ::Classify(
				   const double *pInCodeBook,  //输入码本
				   unsigned int nInNums,       //输入码本中码字数量
				   const double *pCodeBook,    //模板码本
				   unsigned int nCodeNums,     //模板码本中码字数量
				   unsigned int *pKinds        //输入码字中的码字对应的模板码本中码字的下标
				   ) 
{
	//码本的最小距离
	double fMinDis = 0;
	//最小距离对应的模板码字的下标
	unsigned int nMinIndex = 0;
	//计算码本距离
	double fDis = 0;

	//循环变量
	unsigned int i = 0, j=0;
	
	if(pInCodeBook != NULL && pCodeBook !=NULL)
	{
		//初始化码字下标
		memset(pKinds, 0, sizeof(unsigned int) * nInNums);

		for(i=0; i < nInNums; i++)
		{
			//初始化最小距离
			fMinDis = abs(pInCodeBook[i] - pCodeBook[0]);
			nMinIndex = 0;

			//计算最小距离
			for( j=0; j < nCodeNums; j++)
			{
				fDis = abs(pInCodeBook[i] - pCodeBook[j]);
				if(fDis < fMinDis)
				{
					fMinDis = fDis;
					nMinIndex = j;
				}
			}
			//保存码字下标
			pKinds[i] = nMinIndex;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// 计算两个码本的欧式距离
double CVQ::GetDistance(const double *pCode1, const double *pCode2, unsigned int nCodeNums) 
{
	double fSum = 0;
	for(unsigned int i=0; i< nCodeNums; i++)
	{
		fSum += (pCode1[i]-pCode2[i]) * (pCode1[i]-pCode2[i]);
	}
	return abs(fSum);
}
