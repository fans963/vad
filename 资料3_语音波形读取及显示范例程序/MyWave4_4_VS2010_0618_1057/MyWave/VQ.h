#ifndef  _CVQ_H_
#define  _CVQ_H_
#pragma once
//////////////////////////////////////////////////////////////////////////
// 采用改进LBG算法，实现输入序列的矢量量化
// 
// 创建人:		
// 创建日期:	
// 修改人:
// 修改日期:

// 默认的最大迭代次数
#define CVQ_MAX_REPEAT
// 无穷大
#define CVQ_MAXIMUN	50000

class CVQ
{
public:
	CVQ(void);
	~CVQ(void);
public:
	//采用LBG算法进行VQ
	static void LBG(
		const double * pInVector,        //输入样本序列
		unsigned int   nInLen,           //输入样本序列长度
		double       * pOutCodeBook,     //输出码本
		unsigned int   nCodeNums,        //码本长度
		unsigned int   nMaxReapt,        //最大迭代次数
		double         fMinChange,       //畸变改进阈值
		double         fInitDistortion = CVQ_MAXIMUN  //初始畸变
		);
	//进行LBG算法迭代，输出总畸变
	static double LBGRepeat(
		const double * pInVector,        //输入样本序列
		unsigned int   nInLen,           //输入样本序列长度
	  double * pCodeBook,              //输入/出码本
		unsigned int   nCodeNums         //码本长度
		);
	//对输入样本进行简单聚类
	static void EasyCluster(
		const double * pInVector,        //输入样本序列
		unsigned int   nInLen,           //输入样本序列长度
	    double * pCodeBook,              //输入/出码本
		unsigned int   nCodeNums         //码本长度
		);
	//实现K均值聚类
	static void KMeansCluster(
		const double * pInVector,        //输入样本序列
		unsigned int   nInLen,           //输入样本序列长度
	    double * pCodeBook,              //输入/出码本
		unsigned int   nCodeNums         //码本长度
		);
	//对输入码本，按照标准码本进行分类
	static void Classify(
		const double * pInCodeBook,  //输入码本
		unsigned int   nInNums,      //输入码本中码字数量
		const double * pCodeBook,    //模板码本
		unsigned int   nCodeNums,    //模板码本中码字数量
		unsigned int * pKinds        //输入码本中的码字对应的模板码本中码字的下标
		);
	//计算码本欧式距离
	static double GetDistance(
		const double* pCode1,
		const double* pCode2,
		unsigned int  nCodeNums
		);
private:
	//实现K均值聚类迭代，输出总畸变
	static double KMeansClusterRepeat(
		const double* pInVector,      //输入样本序列
		unsigned int  nInLen,         //输入样本序列长度
		double*       pCodeBook,      //输入码本
		unsigned int  nCodeNums,      //码本长度
		unsigned int* pFlag,          //用于分类的数组，再KMeansCluster中分配
		double*       pNewCodeBook    //输出新码本
		);
};

#endif 