#pragma once 
/////////////////////////////////////////////////////
//实现基于VQ的HMM模型
//
//创建日期：2012.10.23
//
//
//////////////////////////////////////////////////////
//默认状态数
#define CHMM_STATUS_NUMS   6
//识别字长度
#define CHMM_WORD_LIN     10
class CHMM
{
public:
	CHMM();
	~CHMM(void);
public:
	//导入HMM模型信息
	BOOL LoadModel(CString strFileName);
	//导出HMM模型信息
	BOOL SaveModel(CString strFileName);
	//释放模型占用资源
	void Dispose();
	//实现对HMM模型的迭代训练
	void Train(
		const double * pDataIn,   //输入采样序列
		unsigned int nInLen       //输入采样序列长度
		);
	//实现对HMM模型的初始化
	void PrepareTrain(
		CString strWord,            //模型对已词
		const double*  pDataIn,     //第一个输入采样序列
		unsigned int   nInLen,      //输入采样序列长度
		unsigned int   nFrameSize,  //分帧宽度
		unsigned int   nStatusNums, //HMM状态数
		unsigned int   nCodeNums    //输入码本长度
		);
private:
	//实现前后向算法，并输出最大概率
	double ForwardBackward(
		const unsigned int * pCodeBook,//输入观察码本序列对应的HMM码本的下标
		unsigned int nCodeNums,        //输入观察序列长度
		const double* pPi,             //HMM的pi矢量
		const double* pA,              //HMM的A矩阵
		const double* pB,              //HMM的B矩阵
		unsigned int nStatusNums,      //HMM状态数
		double* pa,                    //前后向算法中的a
		double* pb                     //前后向算法中的b
		);
	//实现Viterbi算法
	double Viterbi(
		const unsigned int* pCodeBook, //输入观察码本序列对应的HMM码本的下标
		unsigned int nCodeNums,        //输入观察序列长度
		const double* pPi,             //HMM的pi矢量
		const double* pA,              //HMM的A矩阵
		const double* pB,              //HMM的B矩阵
		unsigned int nStatusNums       //HMM状态数
		);
	//实现Baum-Welch算法
	void BaumWelch(
		const unsigned int* pCodeBook,	// 输入观察码本序列对应的HMM码本的下标
		unsigned int nCodeNums,			// 输入观察序列长度
		double* pPi,					// HMM的pi矢量
		double* pA,						// HMM的A矩阵
		double* pB,						// HMM的B矩阵
		unsigned int nStatusNums,		// HMM状态数
		double* pa,						// 前后向算法计算所得的pa
		double* pb,						// 前后向算法计算所得pb
		double fRate					// 前后向算法计算所得最大概率
		);

public:
	    double* m_pPi;                 //初始状态分布序列,t=1时间时q1 =Si的概率pi
		double* m_pA;                  //状态转移概率矩阵，由q(i-1)为s(k)转移到q(i)为s(h)的概率
		double* m_pB;                  //观察矢量处于某状态的概率
		double* m_pCodeBook;           //码本

		unsigned int m_nStatusNums;    //HMM状态数量
		unsigned int m_nCodeNums;      //输入码本长度
		CString      m_strWord;        //模型对应的词
		unsigned int m_nFrameSize;     //求倒谱时所进行的短时快速FFT的短时点数
};