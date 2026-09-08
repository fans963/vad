//混合高斯模型类
#include"Matrix.h"
#include"Vector.h"


#define NBSTATES 32// Number of states in the GMM    就是混合高斯函数的个数M
struct Sample
{
	CString name;
	double *priors;
	double *mu;
	double *sigma;
    int nState;
	int dim;
	
	//double pp;
	double *p0;
	double result;
};

class GaussianMixture
{
private:
	int nState,dim;
	//Matrix mu;
	//Matrix *sigma;
	double **mu;
	double **sigma;
	double *priors;

	/////////////保存中间变量值
	//Matrix mu_temp;
	//Matrix *sigma_temp;
	//float *priors_temp;
public:
	 Sample *s;
	 int sample_num;

public:
	GaussianMixture();//构造函数
	~GaussianMixture();//析构函数
	GaussianMixture(GaussianMixture &g);//构造函数
	//load the dataset from a file 从文件中获得原始数据
	Matrix loadDataFile(CString filename/*char filename[]*/);

	//save the dataset to a file将数据保存到文件中
	bool saveDataFile(const char filename[], Matrix data);

	//save the result of a regression 将回归的结果保存
	bool saveRegressionResult(const char fileMu[],const char fileSigma[],Matrix inData,Matrix outData,Matrix outSigma[]);

	//load the means,priors probabilities and covariances matrixs stored in a file
	//从文件中加载均值、权重系数和协方差矩阵
	bool loadParams(const char filename[]);

	//save current parameters in a file将当前均值等的参数保存到一个文件中
	//void saveParams(const char filename[]);
	void saveParams(CString filename);

	///*do a regression with current parameters:(用当前参数进行回归)
	//   - output a matrix of size nb row of in*nb components in outComponents
	//   - the SigmaOut pointer will point to an array of nb row of in or out covariances matrixxs
	//   -inComponents and outComponents are the index of the dimensions represented in the in and out matrixs
	//*/
	//Matrix doRegression(Matrix in,
	//	                Matrix* SigmaOut,
	//					Vector inComponents,
	//					Vector outComponents);

	//compute probabilty of vector v(corresponding to dimension given in the Component vector)
	//for the given state（计算一个给定状态的概率）
	float pfState(Vector v,Vector Components,int state);


	//same as above but v is og same dimension as the GMM
	float pdfState(Vector v,int state);

	 /* Spline fitting to rescale trajectories. */
     Matrix HermitteSplineFit(Matrix& inData, int nbSteps, Matrix& outData);

	/*init the GaussionMixture by spliting the dataset into 
	time(first dimension)slices and computing variances
	and means for each slices.
	once initialisation has been performed ,the nb of state is set*/
	void initEM_TimeSplit(int nState,Matrix Dataset);//初始化模型参数

	/*performs Exception/Maximization on the Dataset,
	  in order to obtain a nState GMM
	  Dataset is a Matrix(nSamples,nDimensions)	*/
	int doEM(Matrix DataSet);//EM算法迭代

	//采用LBG算法进行初始化GMM
    void LBG(
		Matrix  DataSet,                 //输入样本矩阵
		unsigned int nState,                      //高斯混合阶数M
		unsigned int   nMaxReapt,        //最大迭代次数
		double         fMinChange,      //畸变改进阈值
		double         fInitDistortion //初始畸变;
		);
	//进行LBG算法迭代，输出总畸变
    double LBGRepeat( 
					Matrix DataSet,		// 输入样本序列
					Vector * pCodeBook,	// 输入/出码本
					unsigned int nState	// 码本长度
					);

	//实现K均值聚类
	 void KMeansCluster(
		Matrix  DataSet,                 //输入样本矩阵
		unsigned int nState,                      //高斯混合阶数M
		double         fMinChange      //畸变改进阈值
		);
	//实现K均值聚类迭代，输出总畸变
	 double KMeansClusterRepeat(
		Matrix DataSet,		// 输入样本序列
		Vector * pCodeBook,	// 输入/出码本
		unsigned int nState	,// 码本长度
		Vector * pNewCodeBook
		);
	 //分裂法与K均值相结合的聚类
	 void SeCession_KMeansCluster(
		Matrix  DataSet,                 //输入样本矩阵
		unsigned int nState,                      //高斯混合阶数M
		int   *cSize,//每个聚类所含向量数
		int*clusterIndex//每个聚类所含向量索引
		);

	 ////////////////////////////////////////////////////////////////////////
	  ///////////////////////////////
	 double getDistance(Vector Vin,Vector data);//向量之间的距离
	  double getDistance(double *Vin,Vector data);//数组与向量之间的距离
	 //////////////////kmeans
	 void  KMeans(
	    Matrix  DataSet,                 //输入样本矩阵
		unsigned int nState,                      //高斯混合阶数M
		int   *cSize,//每个聚类所含向量数
		int*clusterIndex//每个聚类所含向量索引
		);
	 /////////////////////初始化GMM
	 bool  InitGMM(
			  Matrix  DataSet,                 //输入样本矩阵
		      unsigned int nState );                   //高斯混合阶数M
	 //GMM迭代过程
     bool  BuildGMM(
			  Matrix  DataSet,                 //输入样本矩阵
		      unsigned int nState );                   //高斯混合阶数M
     // 高斯密度函数
	 double GMM_density(Vector v,int state);
	 /////////////////////////////////////GMM建模主函数
	  bool  GMMs(
			  Matrix  DataSet,                 //输入样本矩阵
		      unsigned int nState //高斯混合阶数M
			 ); 
	  void identify(double *mfcc,int fra_num,int WinSize);//说话人辨认
	  void verify(double *mfcc,int fra_num,int WinSize);
	  void load();//把训练的gmm模型调入
	  void test(double *mel,int i);
	  double logAdd(double f0,double f1);
};