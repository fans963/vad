#include "MFCStub.h"
//娣峰悎楂樻柉妯″瀷绫
#include"Matrix.h"
#include"Vector.h"


#define NBSTATES 32// Number of states in the GMM    灏辨槸娣峰悎楂樻柉鍑芥暟鐨勪釜鏁癕
struct Sample
{
	QString name;
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

	/////////////淇濆瓨涓闂村彉閲忓
	//Matrix mu_temp;
	//Matrix *sigma_temp;
	//float *priors_temp;
public:
	 Sample *s;
	 int sample_num;

public:
	GaussianMixture();//鏋勯犲嚱鏁
	~GaussianMixture();//鏋愭瀯鍑芥暟
	GaussianMixture(GaussianMixture &g);//鏋勯犲嚱鏁
	//load the dataset from a file 浠庢枃浠朵腑鑾峰緱鍘熷嬫暟鎹
	Matrix loadDataFile(QString filename/*char filename[]*/);

	//save the dataset to a file灏嗘暟鎹淇濆瓨鍒版枃浠朵腑
	bool saveDataFile(const char filename[], Matrix data);

	//save the result of a regression 灏嗗洖褰掔殑缁撴灉淇濆瓨
	bool saveRegressionResult(const char fileMu[],const char fileSigma[],Matrix inData,Matrix outData,Matrix outSigma[]);

	//load the means,priors probabilities and covariances matrixs stored in a file
	//浠庢枃浠朵腑鍔犺浇鍧囧笺佹潈閲嶇郴鏁板拰鍗忔柟宸鐭╅樀
	bool loadParams(const char filename[]);

	//save current parameters in a file灏嗗綋鍓嶅潎鍊肩瓑鐨勫弬鏁颁繚瀛樺埌涓涓鏂囦欢涓
	//void saveParams(const char filename[]);
	void saveParams(QString filename);

	///*do a regression with current parameters:(鐢ㄥ綋鍓嶅弬鏁拌繘琛屽洖褰)
	//   - output a matrix of size nb row of in*nb components in outComponents
	//   - the SigmaOut pointer will point to an array of nb row of in or out covariances matrixxs
	//   -inComponents and outComponents are the index of the dimensions represented in the in and out matrixs
	//*/
	//Matrix doRegression(Matrix in,
	//	                Matrix* SigmaOut,
	//					Vector inComponents,
	//					Vector outComponents);

	//compute probabilty of vector v(corresponding to dimension given in the Component vector)
	//for the given state锛堣＄畻涓涓缁欏畾鐘舵佺殑姒傜巼锛
	float pfState(Vector v,Vector Components,int state);


	//same as above but v is og same dimension as the GMM
	float pdfState(Vector v,int state);

	 /* Spline fitting to rescale trajectories. */
     Matrix HermitteSplineFit(Matrix& inData, int nbSteps, Matrix& outData);

	/*init the GaussionMixture by spliting the dataset into 
	time(first dimension)slices and computing variances
	and means for each slices.
	once initialisation has been performed ,the nb of state is set*/
	void initEM_TimeSplit(int nState,Matrix Dataset);//鍒濆嬪寲妯″瀷鍙傛暟

	/*performs Exception/Maximization on the Dataset,
	  in order to obtain a nState GMM
	  Dataset is a Matrix(nSamples,nDimensions)	*/
	int doEM(Matrix DataSet);//EM绠楁硶杩浠

	//閲囩敤LBG绠楁硶杩涜屽垵濮嬪寲GMM
    void LBG(
		Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		unsigned int nState,                      //楂樻柉娣峰悎闃舵暟M
		unsigned int   nMaxReapt,        //鏈澶ц凯浠ｆ℃暟
		double         fMinChange,      //鐣稿彉鏀硅繘闃堝
		double         fInitDistortion //鍒濆嬬暩鍙;
		);
	//杩涜孡BG绠楁硶杩浠ｏ紝杈撳嚭鎬荤暩鍙
    double LBGRepeat( 
					Matrix DataSet,		// 杈撳叆鏍锋湰搴忓垪
					Vector * pCodeBook,	// 杈撳叆/鍑虹爜鏈
					unsigned int nState	// 鐮佹湰闀垮害
					);

	//瀹炵幇K鍧囧艰仛绫
	 void KMeansCluster(
		Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		unsigned int nState,                      //楂樻柉娣峰悎闃舵暟M
		double         fMinChange      //鐣稿彉鏀硅繘闃堝
		);
	//瀹炵幇K鍧囧艰仛绫昏凯浠ｏ紝杈撳嚭鎬荤暩鍙
	 double KMeansClusterRepeat(
		Matrix DataSet,		// 杈撳叆鏍锋湰搴忓垪
		Vector * pCodeBook,	// 杈撳叆/鍑虹爜鏈
		unsigned int nState	,// 鐮佹湰闀垮害
		Vector * pNewCodeBook
		);
	 //鍒嗚傛硶涓嶬鍧囧肩浉缁撳悎鐨勮仛绫
	 void SeCession_KMeansCluster(
		Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		unsigned int nState,                      //楂樻柉娣峰悎闃舵暟M
		int   *cSize,//姣忎釜鑱氱被鎵鍚鍚戦噺鏁
		int*clusterIndex//姣忎釜鑱氱被鎵鍚鍚戦噺绱㈠紩
		);

	 ////////////////////////////////////////////////////////////////////////
	  ///////////////////////////////
	 double getDistance(Vector Vin,Vector data);//鍚戦噺涔嬮棿鐨勮窛绂
	  double getDistance(double *Vin,Vector data);//鏁扮粍涓庡悜閲忎箣闂寸殑璺濈
	 //////////////////kmeans
	 void  KMeans(
	    Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		unsigned int nState,                      //楂樻柉娣峰悎闃舵暟M
		int   *cSize,//姣忎釜鑱氱被鎵鍚鍚戦噺鏁
		int*clusterIndex//姣忎釜鑱氱被鎵鍚鍚戦噺绱㈠紩
		);
	 /////////////////////鍒濆嬪寲GMM
	 bool  InitGMM(
			  Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		      unsigned int nState );                   //楂樻柉娣峰悎闃舵暟M
	 //GMM杩浠ｈ繃绋
     bool  BuildGMM(
			  Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		      unsigned int nState );                   //楂樻柉娣峰悎闃舵暟M
     // 楂樻柉瀵嗗害鍑芥暟
	 double GMM_density(Vector v,int state);
	 /////////////////////////////////////GMM寤烘ā涓诲嚱鏁
	  bool  GMMs(
			  Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		      unsigned int nState //楂樻柉娣峰悎闃舵暟M
			 ); 
	  void identify(double *mfcc,int fra_num,int WinSize);//璇磋瘽浜鸿鲸璁
	  void verify(double *mfcc,int fra_num,int WinSize);
	  void load();//鎶婅缁冪殑gmm妯″瀷璋冨叆
	  void test(double *mel,int i);
	  double logAdd(double f0,double f1);
};