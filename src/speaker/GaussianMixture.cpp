#include "MFCStub.h"
//GMM绫诲疄鐜
#include <QString>
#include <QVector>
#include<iostream>
#include<sstream>
#include<fstream>


#include"VQ.h"
#include<cstdlib>
#include"MathLib.h"
#include"GaussianMixture.h"
using namespace std;

#undef MIN
#define MIN 1.0e-2
#define DIFF_GMM_VALUE     1
#define MAXITER 20
GaussianMixture::GaussianMixture()//鏋勯犲嚱鏁
{
	priors=NULL;
	mu=NULL;
	sigma=NULL;
	s=NULL;
}
GaussianMixture::~GaussianMixture()//鏋勯犲嚱鏁
{
	//if(s!=NULL)
	//	delete s;
	//if(priors!=NULL)
	//    delete priors;
	//if(mu!=NULL)
	//	delete mu;
	//if(sigma!=NULL)
	//	delete sigma;
}
GaussianMixture::GaussianMixture(GaussianMixture &g)//鏋勯犲嚱鏁
{
	this->mu=g.mu;
	this->sigma=g.sigma;
	this->priors=g.priors;
	this->dim=g.dim;
	this->nState=g.nState;
}
Matrix GaussianMixture::loadDataFile(QString filename/*char filename[]*/)
{
	//load the dataset from a file 浠庢枃浠朵腑璇诲彇鏁版嵁闆
	Matrix result;
	Vector vecTmp;
	float valTmp;
	char tmp[1024];
	unsigned int l=0,c=0;

	std::ifstream f;
	f.open(filename.toStdString());
	if(f.is_open())
	{
		//get number of row 鑾峰彇琛屾暟
		while(!f.eof())
		{
			f.getline (tmp,1024);
			l++;
			if(l==1)
			{
				// get number of columns鑾峰彇鍒楁暟
				std::istringstream strm;
				strm.str(tmp);
				while(strm >> valTmp)
					c++;
			}
		}
		result.Resize (l-1,c);//then the matrix can be allocated 鐭╅樀鍒嗛厤绌洪棿
		f.clear();
		f.seekg(0);//returns to beginning of the file杩斿洖鏂囦欢寮濮
		for(unsigned int i=0;i<l;i++)
		{
			f.getline(tmp,1024);
			std::istringstream strm;
			strm.str(tmp);
			for(unsigned int j=0;j<c;j++)
				strm>>result(i,j);
		}
		f.close();
	}
	else
	{
       AfxMessageBox(_T("Error opening file!"));
	}
    return result;
}
bool GaussianMixture::saveDataFile(const char filename[],Matrix data)
{
	//save the dataset to a file  淇濆瓨鏁版嵁闆嗗埌鏂囦欢涓
	std::ofstream f(filename);
	if(f.is_open())
	{
		for(unsigned int j=0;j<data.RowSize ();j++)
		{
			for(unsigned int i=0;i<data.ColumnSize();i++)
				f<<data(j,i)<<"  ";
			f<<std::endl;
		}
		f.close();
	}
	return 1;
}
bool GaussianMixture::saveRegressionResult(const char fileMu[],const char fileSigma[],Matrix inData,Matrix outData,Matrix outSigma[])
{

	//save the result of regression 淇濆瓨鍥炲綊鍚庣殑鏁版嵁
	std::ofstream Mu_file(fileMu);//regressed data
	std::ofstream Sigma_file(fileSigma);//covariances matrices(matrix鐨勫嶆暟)
	for(unsigned int i=0;i<outData.RowSize();i++)
	{
		Mu_file<<inData(i,0)<< "  ";
		for(unsigned int j=0;j<outData.ColumnSize();j++)
			Mu_file<< outData(i,j)<<"  ";
		Mu_file<<std::endl;

		for(unsigned int k=0;k<outData.ColumnSize();k++)
		{
			for(unsigned int j=0;j<outData.ColumnSize();j++)
				Sigma_file<< outSigma[i](k,j)<<"  ";
		}
		Sigma_file<<std::endl;
	}
	return 1;
}
bool GaussianMixture::loadParams(const char fileName[])//鍔犺浇GMM鍙傛暟
{
	//load coefficient of a GMM from a file(stored by saveParams Method or with matlab)
	//鍔犺浇涓涓狦MM妯″瀷鐨勭郴鏁帮紝閫氳繃saveParams鍑芥暟鎴杕atlab淇濆瓨
	std::ifstream fich(fileName);
	if(!fich.is_open())
		return false;
	fich>>dim>>nState;
	priors= new double[nState];
	for(int s=0;s<nState;s++)
		fich>>priors[s];
	mu=new double*[nState];
	for(int i=0;i<nState;i++)
	{
		mu[i]=new double[dim];
		for(int j=0;j<dim;j++)
		fich>>mu[i][j];
	}
	sigma=new double*[nState];
	for(int s=0;s<nState;s++)
	{
		sigma[s]=new double[dim];
		for(int i=0;i<dim;i++)
				fich>>sigma[s][i];
	}
	return true;
}
void GaussianMixture::saveParams(/*const char*/QString filename/*[]*/)//淇濆瓨GMM鐨勫悇鍙傛暟
{
	//save the current GMM parameters,coefficents to a file
	//to be retrieved(鍙栧洖)by the loadParams method
	std::ofstream file(filename.toStdString());
	file<<dim<<" "<<nState<<std::endl;


	for(int i=0;i<nState;i++)//淇濆瓨鍚勪釜鍒嗛噺鐨勫姞鏉冨
		file<<priors[i]<<"  ";
	file<<std::endl;

	for(int s=0;s<nState;s++)//淇濆瓨鍧囧
	{
		for(int i=0;i<dim;i++)
			file<<mu[s][i]<<"  ";
		file<<std::endl;
	}

	for(int s=0;s<nState;s++)//淇濆瓨鍗忔柟宸鐭╅樀
	{
		for(int j=0;j<dim;j++)
		{
				file<<sigma[s][j]<<"  ";	
		}	
		file<<std::endl;
	}
}
Matrix GaussianMixture::HermitteSplineFit(Matrix& inData, int nbSteps, Matrix& outData)
{
	//spline fitting to rescale trajectories
	if(nbSteps<=0)
		return outData;
	 
	const int dataSize =inData.ColumnSize();//绀轰緥涓姣忎釜鏂囦欢鐭㈤噺鐨勭淮鏁
	const int inSize   =inData.RowSize();//姣忎釜鏂囦欢鐭㈤噺涓鏁
	const int outSize  =nbSteps;//绀轰緥涓鐨139

	outData.Resize(outSize,dataSize);
	for(int i=0;i<outSize;i++)
	{
		//find the nearest data pair
		const float cTime=float(i)/float(outSize-1)*float(inSize-1);
		int prev,next;
		float prevTime,nextTime;


		prev    =int(floor(cTime));
		next    =prev+1;
		prevTime=float(prev);
		nextTime=float(next);
		const float npbase=(cTime-prevTime)/(nextTime-prevTime);
		const float s1=npbase;
		const float s2=s1*npbase;
		const float s3=s2*npbase;
		const float h1=2.0f*s3-3.0f*s2+1.0f;
		const float h2=-2.0f*s3+3.0f*s2;
		const float h3=s3-2.0f*s2+s1;
		const float h4=s3-s2;
		//the first column is considered as a temporal value
		outData(i,0)=(float)i;
		for(int j=1;j<dataSize;j++)
		{
			const float p0=(prev>0? inData(prev-1,j):inData(prev,j));
			const float p1=inData(prev,j);
			const float p2=inData(next,j);
			const float p3=(next<inSize-1?inData(next+1,j):inData(next,j));
			const float t1=0.5f*(p2-p0);
			const float t2=0.5f*(p3-p1);
			outData(i,j)=p1*h1+p2*h2+t1*h3+t2*h4;
		}
	}
	return outData;
}
void GaussianMixture::initEM_TimeSplit(int nState,Matrix DataSet)
{
	/* init the GaussianMixture by splitting the dataset into time(first dimension)slices
	and computing variances and means for each slices.
	once initialisation has been performed ,the nb of state is set
	閫氳繃灏嗘暟鎹鍒嗘垚鏃堕棿鐗囨碉紙绗1缁存暟鎹琛ㄧず鏃堕棿锛夊垵濮嬪寲楂樻柉娣峰悎妯″瀷锛岀劧鍚庤＄畻姣忎釜鐗囨电殑鍧囧煎拰鍗忔柟宸鐭╅樀
	*/

	//Vector *mean=new Vector[nState];//nState 涓烘贩鍚堝垎閲忕殑涓鏁癕
	//int nData=DataSet.RowSize();//鐭㈤噺鍒嗛噺鐨勪釜鏁
	//this->nState=nState;//娣峰悎楂樻柉M
	//this->dim=DataSet.ColumnSize();//鐭㈤噺缁存暟D
	//float tmax=0;
	//Matrix index(nState,nData);
	//int *pop=new int[nState];
	//priors=new double[nState];//娣峰悎鍔犳潈鍊紈i
	//

	//Matrix unity(dim,dim);//defining unity matrix 鍗曚綅鐭╅樀 D*D
	//for(int k=0;k<dim;k++)unity(k,k)=1.0;

	//for(int n=0;n<nData;n++)//getting the max value for time 鑾峰彇鏃堕棿鏍囧彿鏈澶у
	//{
	//	if(DataSet(n,0)>tmax)tmax=DataSet(n,0);
	//}
	//for(int s=0;s<nState;s++)//clearing values
	//{
	//	mean[s].Resize(dim,true);
	//	mu[s]=new double[dim];//鍧囧肩煝閲
	//    sigma[s]=new double[dim];//鍗忔柟宸鐭╅樀鈭慽
	//	pop[s]=0;
	//}

	///*
	//divide the dataset into slices of equal time (tmax/nState)and compute the mean of each slice
	//the pop table index to which slice belongs each sample
	//灏嗘暟鎹闆嗗垎鎴愮瓑鍒嗭紙tmax/nState锛夛紝鐒跺悗璁＄畻姣忎竴缁勭殑鍧囧硷紝pop琛ㄦ瘡涓鐘舵佸寘鍚鐨勭煝閲忎釜鏁
	//*/
	//for(int n=0;n<nData;n++)
	//{
	//    int s = (int)((DataSet(n,0)/(tmax+1))*nState);
	//	mean[s]+=DataSet.GetRow(n);//mean 涓簄State* dim
	//	index(s,pop[s])=(float)n;//index鐢ㄦ潵鏍囪扮姸鎬乻鐨刾op[s]涓鐭㈤噺鍦ㄦ暟鎹闆嗕腑鏄鍝涓琛
	//	pop[s]+=1;
	//}

	//for(int s=0;s<nState;s++)
	//{
	//	mu.SetRow(mean[s]/(float)pop[s],s);//initiate the means computed before 鍒濆嬪寲Ui,pop[s]涓鸿＄畻means[s]鏃剁殑鐭㈤噺鏁扮洰,s涓虹瑂琛
	//	sigma[s]=Matrix(dim,dim);
	//	priors[s]=1.0f/nState;//set equi-probables states

	//	for(int ind=0;ind<pop[s];ind++)
	//	{
	//		for(int i=0;i<dim;i++)
	//		{
	//			//computing covariance matrices 璁＄畻鍏ㄥ崗鏂瑰樊鐭╅樀鈭慽
	//			for(int j=0;j<dim;j++)
	//				sigma[s](i,j)+=(DataSet((int)index(s,ind),i)-mu(s,i))\
	//				*(DataSet((int)index(s,ind),j)-mu(s,j));
	//			////瀵硅掑崗鏂瑰樊鐭╅樀
	//			//sigma[s](i,i)+=(DataSet((int)index(s,ind),i)-mu(s,i))\
	//			//	*(DataSet((int)index(s,ind),i)-mu(s,i));
	//			
	//		}
	//	}
	//	sigma[s]*=1.0f/pop[s];
	//	sigma[s]+=unity*1e-5f;// prevents this matrix from being non-inversible纭淇濊ョ煩闃垫槸鍙閫嗙煩闃
	//}
	//ofstream fout("init_gmm.txt",ios::app);
	//fout<<"鍔犳潈鍊"<<endl;
	//for(int i=0;i<nState;i++)
	//	fout<<priors[i]<<"  ";
	//fout<<endl;
	//fout<<"鍧囧"<<endl;
	//for(int i=0;i<nState;i++)
	//{
	//	for(int j=0;j<dim;j++)
	//		fout<<mu(i,j)<<"   ";
	//	fout<<endl;
	//}
	//fout<<"鍗忔柟宸鐭╅樀"<<endl;
	//for(int s=0;s<nState;s++)
	//{
	//	fout<<"鐘舵"<<s<<"鍗忔柟宸鐭╅樀"<<endl;
	//	for(int i=0;i<dim;i++)
	//	{
	//		for(int j=0;j<dim;j++)
	//			fout<<sigma[s](i,j)<<"   ";
	//		fout<<endl;
	//	}
	//}
	//fout.close();	
}
void GaussianMixture::LBG(
		Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		unsigned int nState,                      //楂樻柉娣峰悎闃舵暟M
		unsigned int   nMaxReapt,        //鏈澶ц凯浠ｆ℃暟
		double         fMinChange,      //鐣稿彉鏀硅繘闃堝
		double         fInitDistortion //鍒濆嬬暩鍙;
		)
{
	//Vector *mean=new Vector[nState];//nState 涓烘贩鍚堝垎閲忕殑涓鏁癕
	//int nData=DataSet.RowSize();//鐭㈤噺鍒嗛噺鐨勪釜鏁
	//this->nState=nState;//娣峰悎楂樻柉M
	//this->dim=DataSet.ColumnSize();//鐭㈤噺缁存暟D
	//float tmax=0;
	//Matrix index(nState,nData);
	//int *pop=new int[nState];
	//priors=new double[nState];//娣峰悎鍔犳潈鍊紈i
	//mu=new double*[nState];//鍧囧肩煝閲
	//sigma=new double*[nState];//鍗忔柟宸鐭╅樀鈭慽

	//Matrix unity(dim,dim);//defining unity matrix 鍗曚綅鐭╅樀 D*D
	//for(int k=0;k<dim;k++)unity(k,k)=1.0;
 //   
	//for(int s=0;s<nState;s++)//clearing values
	//{
	//	mu[s]=new double[dim];
	//	sigma[s]=new double[dim];
	//	pop[s]=0;
	//}

	//Vector *pCodeBook=new Vector[nState];//LBG绠楁硶鐢熸垚鐨勭爜鏈
	//
	////褰撳墠鎬荤暩鍙
	//double fCurDistortion = fInitDistortion;
	////璋冪敤LBGRepeat杩斿洖鐨勬荤暩鍙
	//double fRetDistortion = 0;
	////褰撳墠杩浠ｆ℃暟
	//unsigned int nCurRepeat = 0;
	////褰撳墠鐣稿彉鏀硅繘閲
	//double fCurChange = fInitDistortion;
	//
	////鑻ュ垵濮嬬爜鏈涓虹┖锛屽垯鐢ㄨ緭鍏ュ簭鍒楃殑鍓峮State涓鏍锋湰涓哄垵濮嬬爜鏈

	//	for(int i=0;i<nState;i++)
	//		pCodeBook[i]=DataSet.GetRow (i*5);
 //    //		memcpy(pOutCodeBook, DataSet, sizeof(double) * nState);

	////璋冪敤LBGRepeat杩涜岃凯浠ｇ敓鎴愬垵濮嬬爜涔
	//while(nCurRepeat < nMaxReapt && fMinChange < fCurChange)
	//{
	//	fRetDistortion = GaussianMixture::LBGRepeat (DataSet, pCodeBook, nState);

	//	//璁＄畻褰撳墠鐣稿彉鏀硅繘閲
	//	fCurChange = abs(fCurDistortion - fRetDistortion) / fCurDistortion;
	//	fCurDistortion = fRetDistortion;
	//	nCurRepeat++;
	//}

	//double tempDistance,fDistance;
	//unsigned int temp;
	//for(int i=0;i<nData;i++)//璁＄畻姣忎釜鐭㈤噺鍒伴偅涓鐮佸瓧鐨勮窛绂绘渶杩戯紝灏嗙壒寰佺煝閲忓垎鎴恘State绫
	//{
	//	fDistance =CVQ_MAXIMUN;
	//	for(int j=0; j <nState; j++)
	//	{
 //           tempDistance=0;
	//		for(int p=0;p<DataSet.ColumnSize();p++)
	//			tempDistance=tempDistance+(DataSet.GetRow(i)-pCodeBook[j]).GetArray()[p]*(DataSet.GetRow(i)-pCodeBook[j]).GetArray()[p];
	//		if(tempDistance< fDistance)
	//		{
	//			fDistance = tempDistance;
	//			temp= j;
	//		}
	//	}
	//   	mean[temp]+=DataSet.GetRow(i);//mean 涓簄State* dim
	//	index(temp,pop[temp])=(float)i;//index鐢ㄦ潵鏍囪扮姸鎬乻鐨刾op[s]涓鐭㈤噺鍦ㄦ暟鎹闆嗕腑鏄鍝涓琛
	//	pop[temp]+=1;
	//}
	////鍒濆嬪寲GMM鍙傛暟
	//for(int s=0;s<nState;s++)
	//{
	//	mu.SetRow(mean[s]/(float)pop[s],s);//initiate the means computed before 鍒濆嬪寲Ui,pop[s]涓鸿＄畻means[s]鏃剁殑鐭㈤噺鏁扮洰
	//	sigma[s]=new double[dim];
	//	priors[s]=1.0f/nState;//set equi-probables states

	//	for(int ind=0;ind<pop[s];ind++)
	//	{
	//		for(int i=0;i<dim;i++)
	//		{
	//			//computing covariance matrices 璁＄畻鍏ㄥ崗鏂瑰樊鐭╅樀鈭慽
	//			for(int j=0;j<dim;j++)
	//				sigma[s](i,j)+=(DataSet((int)index(s,ind),i)-mu[s][j])\
	//				*(DataSet((int)index(s,ind),j)-mu[s][j]);
	//			////瀵硅掑崗鏂瑰樊鐭╅樀
	//			//sigma[s](i,i)+=(DataSet((int)index(s,ind),i)-mu(s,i))\
	//			//	*(DataSet((int)index(s,ind),i)-mu(s,i));
	//			
	//		}
	//	}
	//	sigma[s]*=1.0f/pop[s];
	//	sigma[s]+=unity*1e-5f;// prevents this matrix from being non-inversible纭淇濊ョ煩闃垫槸鍙閫嗙煩闃
	//}
 //    ofstream fout("init_gmm_lbg.txt",ios::trunc);
	//fout<<"鍔犳潈鍊"<<endl;
	//for(int i=0;i<nState;i++)
	//	fout<<priors[i]<<"  ";
	//fout<<endl;
	//fout<<"鍧囧"<<endl;
	//for(int i=0;i<nState;i++)
	//{
	//	for(int j=0;j<dim;j++)
	//		fout<<mu[i][j]<<"   ";
	//	fout<<endl;
	//}
	//fout<<"鍗忔柟宸鐭╅樀"<<endl;
	//for(int s=0;s<nState;s++)
	//{
	//	fout<<"鐘舵"<<s<<"鍗忔柟宸鐭╅樀"<<endl;
	//	for(int i=0;i<dim;i++)
	//	{
	//		
	//			fout<<sigma[s][i]<<"   ";
	//		fout<<endl;
	//	}
	//}
	//fout.close();	

}

double GaussianMixture::LBGRepeat( 
					Matrix DataSet,		// 杈撳叆鏍锋湰搴忓垪
					Vector * pCodeBook,	// 杈撳叆/鍑虹爜鏈
					unsigned int nState	// 鐮佹湰闀垮害
					)
{
	//姣忎釜鐮佷功鍖呭惈鐨勬牱鏈鐨勪釜鏁帮紝鐩稿綋浜嶮atlab鐗堜腑鐨凬1
	unsigned int *pnSampleNums=new unsigned int[nState];
	//鑱氱被鏄杩涜屾牱鏈鍙犲姞鍊硷紝鐩稿綋浜嶮ATLAB鐗堜腑鐨剏(m)
	Vector* pfSampleSums=new Vector[nState];
	//淇濆瓨鎬荤暩鍙
	double fDistortion = 0;
	//淇濆瓨鐩搁偦鐨勭爜鐭涓嬫爣
	unsigned int nNearCode = 0;
	//淇濆瓨鏍锋湰涓庣爜鐭㈣窛绂
	double fDistance = 0;
	//寰鐜鍙橀噺
	unsigned int i= 0, j =0;
    double tempDistance;
	unsigned int nData=DataSet.RowSize();
	unsigned int nColumnSize=DataSet.ColumnSize();
	//鍒濆嬪寲鑱氱被鏃舵瘡涓鐮佺煝浠ｈ〃鐨勬牱鏈鏁
	for(int s=0;s<nState;s++)
	{
		pfSampleSums[s].Resize(nColumnSize,false);
		pfSampleSums[s].Zero();
	}

	memset(pnSampleNums, 0, sizeof(unsigned int) * nState);

	//鎸夌収杈撳叆鐮佹湰瀵硅緭鍏ユ牱鏈搴忓垪杩涜屽垎娈
	for(i=0; i <nData; i++)
	{
		fDistance =CVQ_MAXIMUN;
		for(j=0; j <nState; j++)
		{
            tempDistance=0;
			for(int p=0;p<DataSet.ColumnSize();p++)
				tempDistance=tempDistance+(DataSet.GetRow(i)-pCodeBook[j]).GetArray()[p]*(DataSet.GetRow(i)-pCodeBook[j]).GetArray()[p];
			if(tempDistance< fDistance)
			{
				fDistance = tempDistance;
				nNearCode = j;
			}
		}
		//璁＄畻鎬荤暩鍙
		fDistortion+=fDistance;
		pfSampleSums[nNearCode] +=DataSet.GetRow(i);
		pnSampleNums[nNearCode]++;//淇濆瓨姣忕被涓鍖呭惈鐨勭煝閲忔暟鐩
	}
	//璁＄畻鏂扮爜鏈
	ofstream fout("pCodeBook.txt",ios::app);
	for(i=0;i < nState; i++)
	{
		pCodeBook[i]= pfSampleSums[i] / pnSampleNums[i];
		for(int h=0;h<pCodeBook[i].RowSize();h++)
			fout<<pCodeBook[i].GetArray()[h]<<"  ";
		fout<<endl;
	}
	fout.close();
	delete [] pnSampleNums;
	delete [] pfSampleSums;

	return fDistortion;
}
	//瀹炵幇K鍧囧艰仛绫
void GaussianMixture::KMeansCluster(
		Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		unsigned int nState,                      //楂樻柉娣峰悎闃舵暟M
		double         fMinChange    //鐣稿彉鏀硅繘闃堝
		)
{
	Vector *mean=new Vector[nState];//nState 涓烘贩鍚堝垎閲忕殑涓鏁癕
	int nData=DataSet.RowSize();//鐭㈤噺鍒嗛噺鐨勪釜鏁
	this->nState=nState;//娣峰悎楂樻柉M
	this->dim=DataSet.ColumnSize();//鐭㈤噺缁存暟D
	int *pop=new int[nState];//璁板綍姣忎竴绫荤殑鏁扮洰	
	int i,j,k,s,m;
    int *clusterIndex=new int[nData];
	mu=new double*[nState];
	sigma=new double*[nState];
	double **Center=new double*[nState];
	for( s=0;s<nState;s++)//clearing values
	{
		Center[s]=new double[dim];
		mu[s]=new double[dim];
		sigma[s]=new double[dim];
		pop[s]=0;
	}
    ///闅忔満浜х敓鍒濆嬭仛绫讳腑蹇
	for(s=0;s<nState;s++)//clearing values
	{
		for(j=0;j<this->dim;j++)
		{
			Center[s][j]=DataSet(s*10,j);
		}
	}
	
	//鑻ュ垵濮嬬爜鏈涓虹┖锛屽垯鐢ㄨ緭鍏ュ簭鍒楃殑鍓峮State涓鏍锋湰涓哄垵濮嬬爜鏈

	//璋冪敤KMeansClusterRepeat杩涜岃凯浠ｇ敓鎴愬垵濮嬬爜涔
	int farIndex;
	double minDistance,distance,mmindist=1,lastmmindist;
	int iter=0;
	do{
		//fCurChange= GaussianMixture::KMeansClusterRepeat (DataSet, pCodeBook, nState,pNewCodeBook);
		   
		lastmmindist=mmindist;
		mmindist=0;
		memset(pop,0,sizeof(int)*nState);
		memset(clusterIndex,0,sizeof(int)*nData);
       
		for(j=0;j<nData;j++)//鍒嗙被
		{
			farIndex=0;
			minDistance=getDistance(Center[0],DataSet.GetRow(j));
			////////////////鍒嗙被杩囩▼锛宖arIndex涓哄悜閲忔墍灞炶川蹇冪储寮
			for(i=1;i<nState;i++)//i琛ㄧず鐜版湁璐ㄥ績绱㈠紩
			{
				distance=getDistance(Center[i],DataSet.GetRow(j));
				if(distance<minDistance)
				{
					minDistance=distance;						
					farIndex=i;						
				}
			}
			clusterIndex[j]=farIndex;
			mmindist+=minDistance;
			pop[farIndex]++;
		}


	   //鏇存柊鑱氱被涓蹇,鐜版暟鍊兼竻闆
		for(i=0;i<nState;i++)
		{
			for(j=0;j<dim;j++)
			{
				mu[i][j]=0;
				sigma[i][j]=0;
			}
		}
		//寰楀埌鏂扮殑璐ㄥ績锛堝傛灉缁撴灉涓嶅悎鐞嗭紝鍙浠ョ户缁鏇存柊杩囩▼锛
		
		for(j=0;j<nData;j++)
		{
			for(i=0;i<nState;i++)
			{
				if(clusterIndex[j]==i)
				{
					for(m=0;m<dim;m++)
						mu[i][m]+=DataSet(j,m);//璁剧疆鏂扮殑璐ㄥ績,鐢╯etRow()璧嬪
				}
			}				
		}
		////////////////////////M姝ラ

		for(i=0;i<nState;i++)
		{  
			for(m=0;m<dim;m++)
				mu[i][m]=mu[i][m]/pop[i];
			for(j=0;j<nData;j++)
			{
				if(clusterIndex[j]==i)
				{
					for(m=0;m<dim;m++)					
					{
						sigma[i][m]+=pow(DataSet(j,m)-mu[i][m],2);
					}
				}
			}
			for(m=0;m<dim;m++)
			{
				sigma[i][m]=sigma[i][m]/pop[i];
				if(sigma[i][m]<MIN)
					sigma[i][m]=MIN;
			}
			priors[i]=(double)pop[i]/nData;
		}
		for(i=0;i<nState;i++)
			for(j=0;j<dim;j++)
				Center[i][j]=mu[i][j];
		
		//ofstream fout("suiji_lpc_init.txt",ios::trunc);
		//fout<<"鏉冨"<<endl;
		//for(i=0;i<nState;i++)
		//{
		//	fout<<priors[i]<<"   ";			
		//}
		//fout<<endl;
		//fout<<"鍧囧"<<endl;
		//for(i=0;i<nState;i++)
		//{
		//	for( j=0;j<dim;j++)
		//		fout<<mu[i][j]<<"   ";
		//	fout<<endl;
		//}
		//fout<<"鏂瑰樊"<<endl;
		//for(i=0;i<nState;i++)
		//{
		//	for( j=0;j<dim;j++)
		//		fout<<sigma[i][j]<<"   ";
		//	fout<<endl;
		//}
		//fout.close();
		iter++;

	}while(fabs(lastmmindist-mmindist)>0.0001*lastmmindist);
		/*	ofstream fout("63_init.txt",ios::trunc);
		fout<<"鏉冨"<<endl;
		for(i=0;i<nState;i++)
		{
			fout<<priors[i]<<"   ";			
		}
		fout<<endl;
		fout<<"鍧囧"<<endl;
		for(i=0;i<nState;i++)
		{
			for( j=0;j<dim;j++)
				fout<<mu[i][j]<<"   ";
			fout<<endl;
		}
		fout<<"鏂瑰樊"<<endl;
		for(i=0;i<nState;i++)
		{
			for( j=0;j<dim;j++)
				fout<<sigma[i][j]<<"   ";
			fout<<endl;
		}
		fout.close();*/
}
	//瀹炵幇K鍧囧艰仛绫昏凯浠ｏ紝杈撳嚭鎬荤暩鍙
double GaussianMixture::KMeansClusterRepeat(
		Matrix DataSet,		// 杈撳叆鏍锋湰搴忓垪
		Vector * pCodeBook,	// 杈撳叆/鍑虹爜鏈
		unsigned int nState,	// 鐮佹湰闀垮害
		Vector * pNewCodeBook
		)
{
	//姣忎釜鐮佷功鍖呭惈鐨勬牱鏈鐨勪釜鏁帮紝鐩稿綋浜嶮atlab鐗堜腑鐨凬1
	unsigned int *pnSampleNums=new unsigned int[nState];
	//鑱氱被鏄杩涜屾牱鏈鍙犲姞鍊硷紝鐩稿綋浜嶮ATLAB鐗堜腑鐨剏(m)
	Vector* pfSampleSums=new Vector[nState];
	//淇濆瓨鎬荤暩鍙
	double fDistortion = 0;
	//淇濆瓨鐩搁偦鐨勭爜鐭涓嬫爣
	unsigned int nNearCode = 0;
	//淇濆瓨鏍锋湰涓庣爜鐭㈣窛绂
	double fDistance = 0;
	//寰鐜鍙橀噺
	unsigned int i= 0, j =0;
    double tempDistance;
	unsigned int nData=DataSet.RowSize();
	unsigned int nColumnSize=DataSet.ColumnSize();
	//鍒濆嬪寲鑱氱被鏃舵瘡涓鐮佺煝浠ｈ〃鐨勬牱鏈鏁
	for(int s=0;s<nState;s++)
	{
		pfSampleSums[s].Resize(nColumnSize,false);
		pfSampleSums[s].Zero();
	}

	memset(pnSampleNums, 0, sizeof(unsigned int) * nState);
	////鍒濆嬪寲鑱氱被鏃惰繘琛屾牱鏈鍙犲姞鍊
	//memset(pfSampleSums, 0,sizeof(double) * nState);

	//鎸夌収杈撳叆鐮佹湰瀵硅緭鍏ユ牱鏈搴忓垪杩涜屽垎娈
	for(i=0; i <nData; i++)
	{
		fDistance =CVQ_MAXIMUN;
		for(j=0; j <nState; j++)
		{
            tempDistance=0;
			for(int p=0;p<DataSet.ColumnSize();p++)
				tempDistance=tempDistance+(DataSet.GetRow(i)-pCodeBook[j]).GetArray()[p]*(DataSet.GetRow(i)-pCodeBook[j]).GetArray()[p];
			if(tempDistance< fDistance)
			{
				fDistance = tempDistance;
				nNearCode = j;
			}
		}
		pfSampleSums[nNearCode] +=DataSet.GetRow(i);
		pnSampleNums[nNearCode]++;//淇濆瓨姣忕被涓鍖呭惈鐨勭煝閲忔暟鐩
	}
	//璁＄畻鏂扮爜鏈
	ofstream fout("pNewCodeBook.txt",ios::app);
	for(i=0;i < nState; i++)
	{
		pNewCodeBook[i]= pfSampleSums[i] / pnSampleNums[i];
		fout<<i<<"   ";
		for(int h=0;h<pNewCodeBook[i].RowSize();h++)
			fout<<pNewCodeBook[i].GetArray()[h]<<"  ";
		fout<<endl;
	}
	fout.close();
//璁＄畻|ci(m+1)-ci(m)|
	
	for(i=0;i<nState;i++)
	{
		
		for(j=0;j<DataSet.ColumnSize();j++)
			fDistortion+=(pNewCodeBook[i].GetArray()[j]-pCodeBook[i].GetArray()[j])*(pNewCodeBook[i].GetArray()[j]-pCodeBook[i].GetArray()[j]);
		pCodeBook[i]=pNewCodeBook[i];
	}
	delete [] pnSampleNums;
	delete [] pfSampleSums;

	return fDistortion;
}
///////////////////////////////////////////////
double GaussianMixture::getDistance(Vector Vin,Vector data)
{
	int i=0;
	double distance=0;
	for(i=0;i<this->dim;i++)
		distance+=pow((Vin-data).GetArray()[i],2);
	return distance;

}
double GaussianMixture::getDistance(double *Vin,Vector data)
{
	int i=0;
	double distance=0;
	for(i=0;i<this->dim;i++)
		distance+=pow(Vin[i]-data.GetArray()[i],2);
	return distance;

}

//////////////////////////////////////////////////////////////////////////////
void GaussianMixture::SeCession_KMeansCluster(
		Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		unsigned int nState,                      //楂樻柉娣峰悎闃舵暟M
		int   *cSize,//姣忎釜鑱氱被鎵鍚鍚戦噺鏁
		int*clusterIndex//姣忎釜鑱氱被鎵鍚鍚戦噺绱㈠紩
		)
{
	Vector *mean=new Vector[nState];//nState 涓烘贩鍚堝垎閲忕殑涓鏁癕
	int nData=DataSet.RowSize();//鐭㈤噺鍒嗛噺鐨勪釜鏁
	this->nState=nState;//娣峰悎楂樻柉M
	this->dim=DataSet.ColumnSize();//鐭㈤噺缁存暟D
	int *pop=new int[nState];//璁板綍姣忎竴绫荤殑鏁扮洰
	//priors=new float[nState];//娣峰悎鍔犳潈鍊紈i
	//mu.Resize(nState,dim);//鍧囧肩煝閲
	//sigma=new Matrix[nState];//鍗忔柟宸鐭╅樀鈭慽

	
	int i,j,k,s,m;
	//Matrix unity(dim,dim);//defining unity matrix 鍗曚綅鐭╅樀 D*D
	//for( k=0;k<dim;k++)unity(k,k)=1.0;

    //int *clusterIndex=new int[nData];//鎵灞炵被绱㈠紩
	memset(clusterIndex,0,sizeof(int)*nData);
	//Vector *sigma=new Vector[nState];
	mu=new double*[nState];
	sigma=new double*[nState];
	for( s=0;s<nState;s++)//clearing values
	{
		mean[s].Resize(dim,true);
		mu[s]=new double[dim];
		sigma[s]=new double[dim];
		pop[s]=0;
	}

	
///////////////////////////////////////////////姹傜壒寰佺煝閲忛泦鐨勫潎鍊
	for( i=0;i<nData;i++)
	{
		mean[0]+=DataSet.GetRow (i);
	}
	mean[0]=mean[0]/nData;
	//////////////////////////////////////////////////////////////////////////////鏂瑰樊
  double temp=0;
  //ofstream fout("mean_lpcc.txt",ios::trunc);
  for(j=0;j<dim;j++)
   {
	   temp=0;
	    for(i=0;i<nData;i++)
		   temp+=pow(DataSet(i,j)-mean[0][j],2);
        sigma[0][j]=temp/nData;
		//fout<<mean[0][j]<<"    "<<sigma[0][j]<<endl;
   }  
         
  //fout.close();
    
	double **Center=NULL;  // 淇濆瓨涓存椂璐ㄥ績
  	Center=new double*[nState];
	for(i=0;i<nState;i++)
	{
        Center[i]=new double[dim];
		memset(Center[i],0,sizeof(double)*dim);
	}
 
	 //for(s=0;s<dim;s++)
		//Center[0][s]=mu.GetRow(0).GetArray()[s];//淇濆瓨宸叉湁鐨勮仛绫讳腑蹇
	 for(j=0;j<dim;j++)
	 {
		 mu[0][j]=mean[0][j]-sigma[0][j]*0.01;
		 mu[1][j]=mean[0][j]+sigma[0][j]*0.01;
	 }
	
  //int *num=new int[nData];//鏍囪板睘浜庡摢涓绫
  int farIndex;
  double minDistance,distance;
  double lastmmindist,mmindist=1.0;
  for(k=2;k<=nState;k=2*k)
  {	
		if(k!=2)
		{
			for(j=0;j<k;j++)
			  for(s=0;s<dim;s++)
			    mu[j][s]=Center[j][s];//淇濆瓨宸叉湁鐨勮仛绫讳腑蹇
		}
      
		do{	
			for(j=0;j<k;j++)
			  for(s=0;s<dim;s++)
			   Center[j][s]= mu[j][s];//淇濆瓨宸叉湁鐨勮仛绫讳腑蹇

            lastmmindist=mmindist;
			mmindist=0;
			memset(pop,0,sizeof(int)*nState);
			memset(clusterIndex,0,sizeof(int)*nData);
			for(j=0;j<nData;j++)//鍒嗙被
			{
				farIndex=0;
				minDistance=getDistance(Center[0],DataSet.GetRow(j));
				////////////////鍒嗙被杩囩▼锛宖arIndex涓哄悜閲忔墍灞炶川蹇冪储寮
				for(i=1;i<k;i++)//i琛ㄧず鐜版湁璐ㄥ績绱㈠紩
				{
 					distance=getDistance(Center[i],DataSet.GetRow(j));
					if(distance<minDistance)
					{
						minDistance=distance;						
						farIndex=i;
						
					}
				}
				clusterIndex[j]=farIndex;
				mmindist+=minDistance;
				pop[farIndex]++;
			}

		  //鏇存柊鑱氱被涓蹇,鐜版暟鍊兼竻闆
			for(i=0;i<k;i++)
			{
				for(j=0;j<dim;j++)
				{
					mu[i][j]=0;
					sigma[i][j]=0;
				}
				priors[i]=0;
			}
			//寰楀埌鏂扮殑璐ㄥ績锛堝傛灉缁撴灉涓嶅悎鐞嗭紝鍙浠ョ户缁鏇存柊杩囩▼锛
			
			for(j=0;j<nData;j++)
			{
				for(i=0;i<k;i++)
				{
					if(clusterIndex[j]==i)
					{
						for(m=0;m<dim;m++)
							mu[i][m]+=DataSet(j,m);
					}
				}				
			}
			for(i=0;i<k;i++)
			{  
				cSize[i]=pop[i];
				for(m=0;m<dim;m++)
					mu[i][m]=mu[i][m]/pop[i];
				for(j=0;j<nData;j++)
				{
					if(clusterIndex[j]==i)
					{
						for(m=0;m<dim;m++)					
						{
							sigma[i][m]+=pow(DataSet(j,m)-mu[i][m],2);
						}
					}
				}
				for(m=0;m<dim;m++)
				{
					sigma[i][m]=sigma[i][m]/pop[i];
					if(sigma[i][m]<MIN)
						sigma[i][m]=MIN;
				}		
				priors[i]=(double)pop[i]/nData;
			}
			//ofstream fout("lpcc_init.txt",ios::trunc);
			//fout<<"鍧囧"<<endl;
			//for(i=0;i<k;i++)
			//{
			//	for( j=0;j<dim;j++)
			//		fout<<mu[i][j]<<"   ";
			//	fout<<endl;
			//}
			//fout<<"鏂瑰樊"<<endl;
			//for(i=0;i<k;i++)
			//{
			//	for( j=0;j<dim;j++)
			//		fout<<sigma[i][j]<<"   ";
			//	fout<<endl;
			//}
			//fout.close();

		}while(fabs(lastmmindist-mmindist)>0.0001*lastmmindist);
        


		//////////////////////////////////////////////////////////鏇存柊鑱氱被涓蹇
		if(k<=nState/2)
		{
			for(i=0;i<k;i++)
			{
				for(j=0;j<dim;j++)
				{   
					Center[i*2][j]=mu[i][j]-sigma[i][j]*0.01; 
					Center[i*2+1][j]=mu[i][j]+sigma[i][j]*0.01;  
				
				}
			}
		}  
		
		// ofstream fout("init_gmm_lpcc.txt",ios::trunc);
		//fout<<"鍧囧"<<endl;
		//for(i=0;i<k*2;i++)
		//{
		//	for( j=0;j<dim;j++)
		//		fout<<Center[i][j]<<"   ";
		//	fout<<endl;
		//}
		//fout<<"鏂瑰樊"<<endl;
		//for(i=0;i<k;i++)
		//{
		//	for( j=0;j<dim;j++)
		//		fout<<sigma[i][j]<<"   ";
		//	fout<<endl;
		//}
		//fout.close();
		}
}
//////////////////////////////////////////////////////////////////////////////
int GaussianMixture::doEM(Matrix DataSet)
{
	/*
	perform Exceptation/Maximization on the given Dataset :(瀵规墍缁欐暟鎹闆咲ataset鎵ц孍M绠楁硶)
	EM绠楁硶鍖呭惈涓や釜涓昏佹柟闈锛氫竴鏄姹傛湡鏈(Exceptation),鐢‥琛ㄧず锛涗竴鏄鏈澶у寲锛坢aximization锛,鐢∕琛ㄧず
	matrix DataSet(nSamples,Dimensions)
	the GaussianMixture Object must be initialisted before (娣峰悎楂樻柉瀵硅薄蹇呴』鎻愬墠鍒濆嬪寲)
	see initEM_TimeSplit method */

	int nData=DataSet.RowSize();
	int iter=0;
	float log_lik;
	float log_lik_threshold=0.001;
	float log_lik_old=-1e10f;
    float sum_log;
	//Matrix unity(dim,dim);
	//for(int k=0;k<dim;k++)unity(k,k)=1.0;

	////EM loop EM绠楁硶杩浠
	// 
	//while(true)
	//{
	//	float * sum_p=new float[nData];
	//	Matrix pxi(nData,nState);//甯ф暟*M锛屼技鐒跺害
	//	Matrix pix(nData,nState);//鍚庨獙姒傜巼
	//	Vector E;

	//	iter++;
	//	if(iter>MAXITER)
	//	{
	//		AfxMessageBox(_T("EM stops here. Max number of iterations has been reached."));
	//		return MAXITER;	
	//	}
	//	sum_log=0;

	//   ofstream fout1("mywave_pix.txt",ios::trunc);
	//   //fout<<"pxi:"<<endl;
	//	//Expectation Computing 璁＄畻鏈熸湜
	//	for(int i=0;i<nData;i++)
	//	{
	//		sum_p[i]=0;
	//		for(int j=0;j<nState;j++)
	//		{
	//			float p=pdfState(DataSet.GetRow(i),j);//P(x|i),pdfState杩斿洖鍊间负bi(x),D缁撮珮鏂瀵嗗害鍑芥暟
	//			if(p==0)
	//			{
	//				AfxMessageBox(_T("Error: Null probability. Abort.."));
	//				return -1;					
	//			}
	//			pxi(i,j)=p;
	//			//fout<<pxi(i,j)<<"  ";
	//			sum_p[i]+=p*priors[j];//p(x|位),瑙傚療鐭㈤噺xi鍦ㄨノ GMM涓嬬殑浼肩劧搴
	//		}
	//		sum_log+=log(sum_p[i]);//璁缁冪壒寰佺煝閲忓簭鍒梄={xt,t=1,...,T}锛宻um_log涓轰技鐒舵傜巼P(X|位)=P(x1|位)*P(x2|位)*...P(xT|位)鐨勫规暟
	//		//fout<<endl;
	//	}
	//	fout1<<"pix:"<<endl;
	//	for(int j=0;j<nState;j++)
	//	{
	//		for(int i=0;i<nData;i++){
	//			pix(i,j)=pxi(i,j)*priors[j]/sum_p[i];//then P(i|x) 姹傚彇璁缁冩暟鎹鍦ㄧ琲鐘舵佷笅鐨勫悗楠屾傜巼
 //            fout1<<pix(i,j)<<"   ";
	//	  }
	//	  fout1<<endl;
	//	}
	//	fout1.close();

	//	//here we compute the log likehood 璁＄畻瀵规暟浼肩劧鍑芥暟
	//	log_lik=sum_log/nData; 
	//	if(fabs((log_lik/log_lik_old)-1)<log_lik_threshold)
	//	{
	//		return iter;
	//	}
	//	log_lik_old=log_lik;

	//	//Update step  M杩囩▼锛氬筈姹傚悇绉嶅弬鏁扮殑浼拌″
	//	pix.SumRow(E);//E涓衡垜P(i|x) E涓1*M
	//	
	//	for(int j=0;j<nState;j++)
	//	{
	//		priors[j]=E(j)/nData;//new priors 鏇存柊鍔犳潈鍊紈i=鈭慞(i|x)/T;
	//		Vector tmu(dim);
	//		Matrix tmsigma(dim,dim);
	//		tmsigma.Zero();
 //           Vector sum(dim);
	//		////////////////////////////////////////////////////////////////鏂规硶1,娌¤皟閫
	//		//for(int i=0;i<nData;i++)//means updata loop鏇存柊鍧囧
	//		//{

	//		//	tmu+=DataSet.GetRow(i)*pix(i,j);//鈭慞(i|x)*xt
	//		//	for(int k=0;k<dim;k++)
	//		//	{
	//		//		tmsigma(k,k)+=pow(DataSet(i,k),2)*pix(i,j);
	//		//	}
	//		//}
	//		////鈭慞(i|x)*xt*xt
	//		//mu.SetRow(tmu/E(j),j);
 //  //         sigma[j]=tmsigma/E[j];
 //  //         for(int i=0;i<dim;i++)
	//		//{
	//		//	sigma[j](i,i)=sigma[j](i,i)-pow(mu(j,i),2);
	//		//		
	//		//}
 //  //        sigma[j]=sigma[j]+unity*1e-5f;

	//	   ////////////////////////////////////////////////////////鏂规硶2 
	//	   for(int i=0;i<nData;i++)//means updata loop鏇存柊鍧囧
	//		{
	//			tmu+=DataSet.GetRow(i)*pix(i,j);//鈭慞(i|x)*xt
	//		}
	//		mu.SetRow(tmu/E(j),j);
	//		for(int i=0;i<nData;i++)//covariances updates 鏇存柊鍗忔柟宸鐭╅樀
	//		{
	//			for(int k=0;k<dim;k++)
	//			{
	//				Matrix Dif(dim,1);
	//				Dif.SetColumn((DataSet.GetRow(i)-mu.GetRow(j)),0);//xt-mui					
	//				tmsigma+=(Dif*Dif.Transpose())*pix(i,j);//鈭(pix(i,j)*(xt-mui)2)
	//				///*tmsigma(j,k)+=pix(i,j)*pow(DataSet.GetRow(i).GetArray()[k],2)*/
	//			}
	//		}
	//		sigma[j]=tmsigma/E[j]+unity*1e-5f;
	//	}  
	//ofstream fout("KMeans_1_GMM.txt",ios::trunc);
	//	fout<<"鍔犳潈鍊"<<endl;
	//for(int i=0;i<nState;i++)
	//	fout<<priors[i]<<"  ";
	//fout<<endl;
	//fout<<"鍧囧"<<endl;
	//for(int i=0;i<nState;i++)
	//{
	//	for(int j=0;j<dim;j++)
	//		fout<<mu(i,j)<<"   ";
	//	fout<<endl;
	//}
	//fout<<"鍗忔柟宸鐭╅樀"<<endl;
	//for(int s=0;s<nState;s++)
	//{
	//	fout<<"鐘舵"<<s<<"鍗忔柟宸鐭╅樀"<<endl;
	//	for(int i=0;i<dim;i++)
	//	{
	//		for(int j=0;j<dim;j++)
	//			fout<<sigma[s](i,j)<<"   ";
	//		fout<<endl;
	//	}
	//}
	// 
	//fout.close();	
	//}	



	return iter;
}
float GaussianMixture::pdfState(Vector Vin,int state)
{
	//get the probability density for a given state and a given vector
	//鑾峰緱涓涓缁欏畾瑙傚療鐭㈤噺鍜岀粰瀹氱姸鎬佺殑D缁撮珮鏂鍑芥暟
	//Matrix inv_sigma;
	//float det_sigma=1.0f;//鐭╅樀鐨勮屽垪寮
	//double p;
	//Vector dif;

	////sigma[state].Inverse(inv_sigma,&det_sigma);//姹傚崗鏂瑰樊鐭╅樀鐨勯嗙煩闃,det_sigma涓鸿屽垪寮忕殑鍊硷紝鏈鍑芥暟缁忛獙璇佹槸姝ｇ‘鐨
	//inv_sigma.Zero();
	//int i;
	//for(i=0;i<dim;i++)
	//{
	//	inv_sigma(i,i)=1/sigma[state](i,i);
	//    det_sigma*=sigma[state](i,i);
	//}
	//if(sigma[state].IsInverseOk())
	//{   
	//	p=0;
 //		dif=Vin-mu.GetRow(state);//x-ui
	//	for(i=0;i<dim;i++)
	//		p=(double)pow(dif.GetArray()[i],2)*inv_sigma(i,i);//(x-ui)*sigma'*(x-ui)
	//	p=exp(-0.5*p)/sqrt(pow(2*3.14159,dim/2)*pow((double)fabs(det_sigma),0.5));//fabs(x)鍑芥暟鏄姹傛诞鐐规暟x鐨勭粷瀵瑰
	//	if(p<1e-40)
	//		return 1e-40f;
	//	else 
	//		return (float)p;
	//}
	//else
	//{
	//	AfxMessageBox(_T("fail invert this sigma matrix."));
	//	return 0;
	//}
	return 0;
}
float GaussianMixture::pfState(Vector Vin, Vector Components, int state)
{
	/*
	compute the probability density function at vector Vin,
	(given along the dimensions Components),for a given state
	*/

	//Vector mu_s;
	//Matrix sig_s;
	//Matrix inv_sig_s;
	//float det_sig;
	//float p;
	//int dim_s;
	//dim_s=Components.Size();
	//mu.GetRow(state).GetSubVector(Components,mu_s);
	//sigma[state].GetMatrixSpace(Components,Components,sig_s);
	//sig_s.Inverse(inv_sig_s,&det_sig);
	//if(sig_s.IsInverseOk())
	//{
	//	p=(Vin-mu_s)*(inv_sig_s*(Vin-mu_s));
	//	p=exp(-0.5f*p)/sqrt(pow(2.0f*3.14159f,dim_s)*fabs(det_sig));
	//	return p;
	//}
	//else
	//{
	//	AfxMessageBox(_T("fail invert this sigma matrix."));
	//	return 0;
	//}
	return 0;
}
////////////////////////////////////////////////
#define DIFF_CENTER_VALUE 1//k鍧囧奸槇鍊
/** @function
********************************************************************************
<PRE>
鍑芥暟鍚: KMeans()
鍔熻兘: 閫氳繃k澧炲肩畻娉曞緱鍒癕缁勫潎鍊
鐢ㄦ硶:
鍙傛暟:
[IN] DataSet: 鐗瑰緛鍚戦噺鐭╅樀
[IN] nState: 楂樻柉娣峰悎鏁
[OUT] clusterIndex: 姣忎釜鑱氱被鎵鍚鍚戦噺绱㈠紩
[OUT] cSize: 姣忎釜鑱氱被鎵鍚鍚戦噺鏁
杩斿洖:
璋冪敤: getDistance()
涓昏皟鍑芥暟: InitGMM()
</PRE>
*******************************************************************************/
void GaussianMixture::KMeans(
		Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		unsigned int nState,                      //楂樻柉娣峰悎闃舵暟M
		int   *cSize,//姣忎釜鑱氱被鎵鍚鍚戦噺鏁
		int*clusterIndex//姣忎釜鑱氱被鎵鍚鍚戦噺绱㈠紩
		)
{
	Vector *mean=new Vector[nState];//nState 涓烘贩鍚堝垎閲忕殑涓鏁癕
	int nData=DataSet.RowSize();//鐭㈤噺鍒嗛噺鐨勪釜鏁
	this->nState=nState;//娣峰悎楂樻柉M
	this->dim=DataSet.ColumnSize();//鐭㈤噺缁存暟D
	int *pop=new int[nState];//璁板綍姣忎竴绫荤殑鏁扮洰	
	int i,j,k,s,m;
	memset(clusterIndex,0,sizeof(int)*nData);
	mu=new double*[nState];
	sigma=new double*[nState];
	for( s=0;s<nState;s++)//clearing values
	{
		mean[s].Resize(dim,true);
		mu[s]=new double[dim];
		sigma[s]=new double[dim];
		pop[s]=0;
	}
///////////////////////////////////////////////姹傜壒寰佺煝閲忛泦鐨勫潎鍊
	for( i=0;i<nData;i++)
	{
		mean[0]+=DataSet.GetRow (i);
	}
	mean[0]=mean[0]/nData;
	/////////////////////////////////////////////鎬荤殑鍧囧
	for(i=0;i<dim;i++)
		mu[0][i]=mean[0][i];
	//////////////////////////////////////////////////////////////////////////////鏂瑰樊
   double temp=0;
  //ofstream fout("mean_lpcc.txt",ios::trunc);
  //for(j=0;j<dim;j++)
  // {
	 //  temp=0;
	 //   for(i=0;i<nData;i++)
		//   temp+=pow(DataSet(i,j)-mean[0][j],2);
  //      sigma[0][j]=temp/nData;
		////fout<<mean[0][j]<<"    "<<sigma[0][j]<<endl;
  // }  
         
  //fout.close();
    
	double **Center=NULL;  // 淇濆瓨涓存椂璐ㄥ績
  	Center=new double*[nState];
	for(i=0;i<nState;i++)
	{
        Center[i]=new double[dim];
		memset(Center[i],0,sizeof(double)*dim);
	}
 
	int farIndex;
	double minDistance,distance,maxDistance=0;
	double lastmmindist,mmindist=1.0;
	for(k=2;k<nState;k++)
	{	

		//--------------------------------------------------------------------------------
		// 鏂板炲垎瑁傜偣锛氳＄畻姣忎釜鍚戦噺鍒版瘡涓璐ㄥ績鐨勮窛绂,瀵绘壘绂荤幇鏈夎川蹇冩渶杩滅殑鍚戦噺浣滀负鏂拌川蹇
		//
		for(j=0;j<nData;j++)
		{		
			farIndex=0;
		    maxDistance=0;
			distance=0;
			//鍚戦噺鍒版瘡涓璐ㄥ績鐨勮窛绂诲拰,璺濈诲拰鏈澶х殑涓烘柊鐨勮川蹇
			for(i=0;i<k-1;i++)//i琛ㄧず鐜版湁璐ㄥ績绱㈠紩
				distance+=sqrt(getDistance(Center[i],DataSet.GetRow(j)));
			if(distance>maxDistance)
			{
				maxDistance=distance;
				farIndex=j;
			}
		}
		for(j=0;j<dim;j++)
			mu[k-1][j]=DataSet(farIndex,j);// 鏂板炶川蹇
			
		//ofstream fout("kmeans_1.txt",ios::trunc);
		//for(i=0;i<k;i++)
		//{
		//	for(j=0;j<dim;j++)
		//	{
		//		fout<<mu[i][j]<<"   ";
		//	}
		//	fout<<endl;
		//}
		//fout.close();

		mmindist=0;
		do{	
			for(j=0;j<k;j++)
			  for(s=0;s<dim;s++)
			   Center[j][s]= mu[j][s];//淇濆瓨宸叉湁鐨勮仛绫讳腑蹇

            lastmmindist=mmindist;
			mmindist=0;
			memset(pop,0,sizeof(int)*nState);
			memset(clusterIndex,0,sizeof(int)*nData);
			for(j=0;j<nData;j++)//鍒嗙被
			{
				farIndex=0;
				minDistance=getDistance(Center[0],DataSet.GetRow(j));
				////////////////鍒嗙被杩囩▼锛宖arIndex涓哄悜閲忔墍灞炶川蹇冪储寮
				for(i=1;i<k;i++)//i琛ㄧず鐜版湁璐ㄥ績绱㈠紩
				{
 					distance=getDistance(Center[i],DataSet.GetRow(j));
					if(distance<minDistance)
					{
						minDistance=distance;						
						farIndex=i;						
					}
				}
				clusterIndex[j]=farIndex;
				mmindist+=minDistance;
				pop[farIndex]++;
			}

		  //鏇存柊鑱氱被涓蹇,鐜版暟鍊兼竻闆
			for(i=0;i<k;i++)
			{
				for(j=0;j<dim;j++)
				{
					mu[i][j]=0;
					sigma[i][j]=0;
				}
			}
			//寰楀埌鏂扮殑璐ㄥ績锛堝傛灉缁撴灉涓嶅悎鐞嗭紝鍙浠ョ户缁鏇存柊杩囩▼锛
			
			for(j=0;j<nData;j++)
			{
				for(i=0;i<k;i++)
				{
					if(clusterIndex[j]==i)
					{
						for(m=0;m<dim;m++)
							mu[i][m]+=DataSet(j,m);//璁剧疆鏂扮殑璐ㄥ績,鐢╯etRow()璧嬪
					}
				}				
			}


	
		for(i=0;i<k;i++)
		{  
			cSize[i]=pop[i];
			for(m=0;m<dim;m++)
				mu[i][m]=mu[i][m]/pop[i];
			for(j=0;j<nData;j++)
			{
				if(clusterIndex[j]==i)
				{
					for(m=0;m<dim;m++)					
					{
						sigma[i][m]+=pow(DataSet(j,m)-mu[i][m],2);
					}
				}
			}
			for(m=0;m<dim;m++)
			{
				sigma[i][m]=sigma[i][m]/pop[i];
				if(sigma[i][m]<MIN)
					sigma[i][m]=MIN;
			}
			//a[i]=(double)k[i]/kk;
		}
		//ofstream fout("lpcc_init.txt",ios::trunc);
		//fout<<"鍧囧"<<endl;
		//for(i=0;i<k;i++)
		//{
		//	for( j=0;j<dim;j++)
		//		fout<<mu[i][j]<<"   ";
		//	fout<<endl;
		//}
		//fout<<"鏂瑰樊"<<endl;
		//for(i=0;i<k;i++)
		//{
		//	for( j=0;j<dim;j++)
		//		fout<<sigma[i][j]<<"   ";
		//	fout<<endl;
		//}
		//fout.close();

		}while(fabs(lastmmindist-mmindist)>0.0001*lastmmindist);
        


		////////////////////////////////////////////////////////////鏇存柊鑱氱被涓蹇
		//if(k<=nState/2)
		//{
		//	for(i=0;i<k;i++)
		//	{
		//		for(j=0;j<dim;j++)
		//		{   
		//			Center[i*2][j]=mu[i][j]-sigma[i][j]*0.01; 
		//			Center[i*2+1][j]=mu[i][j]+sigma[i][j]*0.01;  
		//		
		//		}
		//	}
		//}  

   }


}
/** @function
********************************************************************************
<PRE>
鍑芥暟鍚: InitGMM()
鍔熻兘: 鑾峰緱GMM鍒濆
鐢ㄦ硶:
鍙傛暟:
[IN]  DataSet: 鐗瑰緛鍚戦噺鐭╅樀
[IN] train_frame_num: 璁缁冨抚甯ф暟
[IN] nState: 楂樻柉娣峰悎鏁
[OUT] mu,sigma,priors
杩斿洖:
true: 鎴愬姛
false锛氬け璐
璋冪敤: KMeans()
涓昏皟鍑芥暟: GMMs()
</PRE>
*******************************************************************************/
bool GaussianMixture::InitGMM(
			  Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		      unsigned int nState )                     //楂樻柉娣峰悎闃舵暟M
{
    int i;
	int j;
	int s;
	int *clusterIndex=NULL;//鍚戦噺鎵灞炶川蹇冪储寮
	int *clusterSize=NULL;//鑱氱被鎵鍚鍚戦噺鏁
	int nData=DataSet.RowSize();//鐭㈤噺鍒嗛噺鐨勪釜鏁
	this->nState=nState;//娣峰悎楂樻柉M
	this->dim=DataSet.ColumnSize();//鐭㈤噺缁存暟D


	clusterIndex=new int[nData];//nData涓鸿缁冨抚鏁
	clusterSize=new int[nState];//鑱氱被鏁癕
	memset(clusterIndex,0,sizeof(int)*nData);
	memset(clusterSize,0,sizeof(int)*nState);
	priors=new double[nState];//娣峰悎鍔犳潈鍊紈i
	mu=new double*[nState];//鍧囧肩煝閲
	sigma=new double*[nState];//鍗忔柟宸鐭╅樀鈭慽
	for(i=0;i<nState;i++)
	{
		priors[i]=0;
		mu[i]=new double[dim];
		sigma[i]=new double[dim];
	}
 
     KMeansCluster(DataSet,nState,100);//闅忔満鍒濆嬪寲鍖朘means
	/////////////////////
	//SeCession_KMeansCluster( DataSet, nState,clusterSize,clusterIndex);//鍒嗚傛硶鍜宬鍧囧肩粨鍚
	///////////////////////////////////////////////////////////////////////////////
	//鍒濆嬪寲鍧囧,姣忔℃柊娣诲姞涓涓璐ㄥ績杩浠
	//KMeans(DataSet,nState,clusterSize,clusterIndex);
		//鍒濆嬪寲鍔犳潈绯绘暟w
	//for(i=0;i<nState;i++)
	//	priors[i]=(double)clusterSize[i]/nData;


	//ofstream fout("init_gmm_keans1.txt",ios::trunc);
	//fout<<"鍔犳潈鍊"<<endl;
	//for( i=0;i<nState;i++)
	//	fout<<priors[i]<<"  ";
	//fout<<endl;
	//fout<<"姣忕被鍚涓鏁"<<endl;
	//for( i=0;i<nState;i++)
	//	fout<<clusterSize[i]<<"  ";
	//fout<<endl;
	//fout<<"鍧囧"<<endl;
	//for( i=0;i<nState;i++)
	//{
	//	for(int j=0;j<dim;j++)
	//		fout<<mu(i,j)<<"   ";
	//	fout<<endl;
	//}
	//fout<<"鍗忔柟宸鐭╅樀"<<endl;
	//for(int s=0;s<nState;s++)
	//{
	//	fout<<"鐘舵"<<s<<"鍗忔柟宸鐭╅樀"<<endl;
	//	for( i=0;i<dim;i++)
	//	{
	//		for( j=0;j<dim;j++)
	//			fout<<sigma[s](i,j)<<"   ";
	//		fout<<endl;
	//	}
	//}
	//fout.close();
	return true;
}
/** @function
********************************************************************************
<PRE>
鍑芥暟鍚: BuildGMM()
鍔熻兘: 杩浠ｄ笅涓杞瓽MM
鐢ㄦ硶:
鍙傛暟:
[IN] DataSet: 鐗瑰緛鍚戦噺鐭╅樀
[IN] nState: 楂樻柉娣峰悎鏁
杩斿洖: 
true: 鎴愬姛
false: 澶辫触
璋冪敤: GMM_density()
涓昏皟鍑芥暟: GMMs()
</PRE>
*******************************************************************************/
bool GaussianMixture::BuildGMM(
			  Matrix  DataSet,                 //杈撳叆鏍锋湰鐭╅樀
		      unsigned int nState )                     //楂樻柉娣峰悎闃舵暟M
{
	int iter=0;//杩浠ｆ℃暟
    int nData=DataSet.RowSize();
	int i,j,k;
	double **pxi=NULL;//M*甯ф暟锛屼技鐒跺害
	double **pix=NULL;//鍚庨獙姒傜巼
  	pxi=new double*[nState];
 	pix=new double*[nState];
	for(i=0;i<nState;i++)
	{
        pxi[i]=new double[nData];
		memset(pxi[i],0,sizeof(double)*nData);	
		pix[i]=new double[nData];
		memset(pix[i],0,sizeof(double)*nData);
	}
	double sum_log=0,sum_log_old=0;
	double * sum_pxi=new double[nData];//淇濆瓨鐗瑰緛鍚戦噺xt鍦∕涓鐘舵佷笅鐨勬傜巼鎬诲拰sum(pxi)
	double * sum_pix=new double[nState];//鍦∕涓鐘舵佷笅鐨勫悗楠屾傜巼鍜宻um(pix)
	//EM loop EM绠楁硶杩浠
	//while(diff>=DIFF_GMM_VALUE)
	do{
		sum_log_old=sum_log;
		//Expectation Computing 璁＄畻鏈熸湜
		//璁＄畻浜嬪悗姒傜巼
		memset(sum_pxi,0,sizeof(double)*nData);
		for( i=0;i<nState;i++)
		{
			for( j=0;j<nData;j++)
		   {
			   //float p=pdfState(DataSet.GetRow(i),j);
				pxi[i][j]=priors[i]*GMM_density(DataSet.GetRow(j),i);//P(x|i),GMM_density杩斿洖鍊间负bi(x),D缁撮珮鏂瀵嗗害鍑芥暟;
				//p=log(pxi[i][j]);//娴嬭瘯鏁版嵁
				sum_pxi[j]+=pxi[i][j];//p(x|位),瑙傚療鐭㈤噺xj鍦ㄨノ GMM涓嬬殑浼肩劧搴︼紝鐩稿綋浜巜1*b1(xt)+.....+Wm*bm(Xt)
			}
		}	
	 //   ofstream fout3("pxi_my1.txt",ios::trunc);
		//fout3<<"pxi:"<<endl;
		//for(i=0;i<nState;i++)
		//{
		//	for( j=0;j<nData;j++)
		//	{
		//	fout3<<pxi[i][j]<<"   ";				
		//	}
		//	fout3<<"////////////////////////////////////////////////////////////////////////"<<endl;
		//}
		//      
		//fout3.close();
		//姹傚彇璁缁冩暟鎹鍦ㄧ琲涓鐘舵佷笅鐨勫悗楠屾傜巼
		sum_log=0;
		memset(sum_pix,0,sizeof(double)*nState);
        for(j=0;j<nData;j++)
		{
			for(i=0;i<nState;i++)
			{
				pix[i][j]=pxi[i][j]/sum_pxi[j];
				sum_pix[i]+=pix[i][j];
			}			
			sum_log+=log(sum_pxi[j]);//璁缁冪壒寰佺煝閲忓簭鍒梄={xt,t=1,...,T}锛宻um_log涓轰技鐒舵傜巼P(X|位)=P(x1|位)*P(x2|位)*...P(xT|位)鐨勫规暟
		}
		////here we compute the log likehood 璁＄畻瀵规暟浼肩劧鍑芥暟
		sum_log=sum_log/nData;//鎸夊抚骞冲潎????????????????????
	 //   ofstream fout("pix_my1.txt",ios::trunc);
		//fout<<"pix:"<<endl;
		//for( i=0;i<nState;i++)
		//{
		//	for( j=0;j<nData;j++)
		//		{
		//			//pix(i,j)=pxi(i,j)/sum_p[i];
		//			//E[j]+=pix(i,j);
		//			fout<<pix[i][j]<<"   ";
		//		}
		//      fout<<endl;
		//}
		//fout.close();

		//Update step  M杩囩▼锛氬筈姹傚悇绉嶅弬鏁扮殑浼拌″
		//
		//璁＄畻涓嬩竴娆¤凯浠ｅ湴GMM
		//灏嗗師鏉ョ殑鏁版嵁娓呴浂
        for(i=0;i<nState;i++)
		{
			priors[i]=0;
			for(j=0;j<this->dim;j++)
			{
				mu[i][j]=0;
				sigma[i][j]=0;
			}
		}
		//////鏇存柊鍙傛暟
		for(i=0;i<nState;i++)
		{
			priors[i]=sum_pix[i]/nData;//new priors 鏇存柊鍔犳潈鍊紈i=鈭慞(i|x)/T;
			//璁＄畻鍧囧煎拰鍗忔柟宸鐭╅樀
			for(j=0;j<dim;j++)
			{
				for( k=0;k<nData;k++)//means updata loop鏇存柊鍧囧
				{
					mu[i][j]=mu[i][j]+pix[i][k]*DataSet(k,j);//鈭慞(i|x)*xt
					sigma[i][j]=sigma[i][j]+pow(DataSet(k,j),2)*pix[i][k];
				}
				mu[i][j]/=sum_pix[i];
				sigma[i][j]/=sum_pix[i];
				sigma[i][j]-=pow(mu[i][j],2);
				if(sigma[i][j]<MIN)
				sigma[i][j]=MIN;
			}
		}
		iter++;   
	}while(fabs(sum_log_old-sum_log)/(0.01+sum_log_old)>0.00001&&iter<10);
   
 //  ofstream fout("gmm_my.txt",ios::trunc);
	//fout<<"鍔犳潈鍊"<<endl;
	//for(i=0;i<nState;i++)
	//	fout<<priors[i]<<"  ";
	//fout<<endl;
	//fout<<"鍧囧"<<endl;
	//for( i=0;i<nState;i++)
	//{
	//	for( j=0;j<dim;j++)
	//		fout<<mu[i][j]<<"   ";
	//	fout<<endl;
	//}
	//fout<<"鍗忔柟宸鐭╅樀"<<endl;
	//for( i=0;i<nState;i++)
	//{
	//	fout<<"鐘舵"<<i<<"鍗忔柟宸鐭╅樀"<<endl;
	//	for( j=0;j<dim;j++)
	//	{
	//			fout<<sigma[i][j]<<"   ";
	//		fout<<endl;
	//	}
	//}
	//fout.close();
	return true;
}
/** @function
********************************************************************************
<PRE>
鍑芥暟鍚: GMM_density()
鍔熻兘: 璁＄畻楂樻柉姒傜巼瀵嗗害
鐢ㄦ硶:
鍙傛暟:
[IN] v: 鐗瑰緛鍚戦噺
[IN] statr: 楂樻柉娣峰悎妯″瀷闃舵暟绱㈠紩
杩斿洖: 楂樻柉姒傜巼瀵嗗害鍊
璋冪敤:
涓昏皟鍑芥暟: BuildGMM()
</PRE>
*******************************************************************************/
double GaussianMixture::GMM_density(Vector v,int s)
{
	int k;
	double res=0;
	double temp=0;
	double sqrt_Matrix_value=1;
	int D=v.RowSize();
	for(k=0;k<D;k++)
	{
		////瀵硅掔煩闃电殑瀵硅掔嚎涓鍑虹幇0锛屽艰嚧鏃犵┓澶1#INF
		temp+=pow(v(k)-this->mu[s][k],2)/this->sigma[s][k];
		////sqrt鍑虹幇-nan锛屽洜涓哄紑鏂瑰间负璐
		sqrt_Matrix_value*=sqrt(fabs(this->sigma[s][k]));
	}
	//temp鍊煎お澶э紝瀵艰嚧exp(temp/-2)涓0
	res=exp(temp/-2)/pow(2*3.14159,D/2)/sqrt_Matrix_value;
	return res;
}
/** @function
********************************************************************************
<PRE>
鍑芥暟鍚: GMMs()
鍔熻兘: GMM寤烘ā杩囩▼
鐢ㄦ硶:
鍙傛暟:
[IN] DataSet: 鐗瑰緛鍚戦噺鐭╅樀
[IN] nState: 楂樻柉娣峰悎鏁
杩斿洖:
true: 鎴愬姛
false: 澶辫触
璋冪敤: doRM(), GMM_density()
涓昏皟鍑芥暟:
</PRE>
*******************************************************************************/
bool GaussianMixture::GMMs( Matrix  DataSet,unsigned int nState)
{	
 	if(!InitGMM(DataSet,nState))//鍒濆嬪寲GMM
	{
         return false;
	}
	if(!BuildGMM(DataSet,nState))//EM杩浠
	{
		return false;
	}

	return true;
}
void GaussianMixture::load()//鎶奊mmFile涓淇濆瓨鐨勬瘡涓鏂囦欢涓鐨勬暟鎹鍙栧嚭鏉ャ
{
	QString filename;//璁板綍淇濆瓨鍦╣mm鏂囦欢澶逛腑鐨勬枃浠跺悕
	CFileFind finder;
	std::ifstream fich;
	//steep 1:鎵惧嚭鏈夊氬皯涓鏍锋湰
	int count=0;
	//bool bWorking=finder.FindFile (_T("F:\\绋嬪簭\\姣曚笟璁捐\\瀹為獙鏁版嵁_603\\30s\\N3浣滀负璁缁冩暟鎹甛\闅忔満璁缁僜\lpc\\N3_lpc_gmm_32\\*.txt"));
	//bool bWorking=finder.FindFile (_T("F:\\绋嬪簭\\姣曚笟璁捐\\瀹為獙鏁版嵁_603\\60s\\T3浣滀负璁缁冩暟鎹甛\闅忔満璁缁僜\mfcc13\\T3_gmm_32\\*.txt"));
	bool bWorking=finder.FindFile (_T("F:\\绋嬪簭\\姣曚笟璁捐\\timit瀹為獙\\gmm_32\\*.txt"));
	while(bWorking)
	{
		count++;
		bWorking=finder.FindNextFileW();
	}

	//鍒涘缓鏍锋湰锛屽垎閰嶅唴瀛
	sample_num=count;
    s=new Sample[count];
	//steep 3:閬嶅巻姣忎釜鏍锋湰锛屽苟杞藉叆
    QString m_gmmname=NULL;
	bWorking=finder.FindFile (_T("F:\\绋嬪簭\\姣曚笟璁捐\\timit瀹為獙\\gmm_32\\*.txt"));
	//bWorking=finder.FindFile (_T("F:\\绋嬪簭\\姣曚笟璁捐\\瀹為獙鏁版嵁_603\\60s\\T3浣滀负璁缁冩暟鎹甛\闅忔満璁缁僜\mfcc13\\T3_gmm_32\\*.txt"));
	int m=0;
	int i,j;
	while(bWorking)
	{
		s[m].name="";
		bWorking=finder.FindNextFileW();
		filename=finder.GetFileTitle();
		m_gmmname=_T("F:\\绋嬪簭\\姣曚笟璁捐\\timit瀹為獙\\gmm_32\\")+filename+_T(".txt");
		//m_gmmname=_T("F:\\绋嬪簭\\姣曚笟璁捐\\瀹為獙鏁版嵁_603\\60s\\T3浣滀负璁缁冩暟鎹甛\闅忔満璁缁僜\mfcc13\\T3_gmm_32\\")+filename+_T(".txt");

		fich.open(m_gmmname.toStdString());

	    fich>>dim>>nState;
		
		s[m].dim=dim;
		s[m].nState=nState;
		s[m].name=filename;
		s[m].priors=new double[nState];
		s[m].mu=new double[nState*dim];
		s[m].sigma=new double[nState*dim];

		for( i=0;i<nState;i++)
			fich>>s[m].priors[i];
		for(i=0;i<nState;i++)
		{
			for( j=0;j<dim;j++)
			fich>>s[m].mu[i*dim+j];
		}
		for( i=0;i<nState;i++)
		{
			for( j=0;j<dim;j++)
			   fich>>s[m].sigma[i*dim+j];
		}
		
		m++;
		fich.close();
	}

}
void GaussianMixture::identify(double  *mfcc,int fra_num,int WinSize)
{
	load();//鍔犺浇璁缁冩ā鍨

	int i=0,j=0;
	double *mel=new double[s[0].dim];
    ////鍒濆嬪寲杈ㄨょ粨鏋
	for(i=0;i<sample_num;i++)
	{
		
		s[i].p0=new double[fra_num];
		s[i].result=0.0;
	}
	for(j=0;j<fra_num;j++)
	{
		for(i=0;i<s[0].dim;i++)
			mel[i]=mfcc[j*s[0].dim+i];
			
		test(mel,j);//娴嬭瘯姣忎竴甯у规墍鏈夎缁冩ā鍨嬬殑鍚庨獙姒傜巼鐨刲og
	}
	//璁＄畻鏈缁堢殑杈ㄨょ粨鏋
	for(i=0;i<sample_num;i++)
	{	
		for(j=0;j<fra_num;j++)
		{		
			s[i].result+=s[i].p0[j];
		}
		s[i].result=s[i].result/fra_num;
	}
	// 瀵逛笌姣忎釜璇磋瘽浜烘ā鍨嬭繘琛屽尮閰嶆渶鍚庡緱鍒扮殑璇勫垎杩涜屾帓搴	
	for(int t=1;t<=sample_num-1;t++)   
	{
		for(i=0;i<sample_num-t;i++)
		{
			if(s[i].result<s[i+1].result)
			{
				int j=s[i].result;
				s[i].result=s[i+1].result;
				s[i+1].result=j;
				QString jj = s[i].name;
				s[i].name = s[i+1].name;
				s[i+1].name = jj; 
			}
		}
	}


	//AfxMessageBox(_T("璇磋瘽浜鸿鲸璁わ細璇ヨ磋瘽浜轰负 ")+s[0].name);

}
void GaussianMixture::verify(double  *mfcc,int fra_num,int WinSize)
{
	load();//鍔犺浇璁缁冩ā鍨
	int i=0,j=0;
	double *mel=new double[s[0].dim];
    ////鍒濆嬪寲杈ㄨょ粨鏋
	for(i=0;i<sample_num;i++)
	{
		s[i].p0=new double[fra_num];
		s[i].result=0.0;
	}
	for(j=0;j<fra_num;j++)
	{
		for(i=0;i<s[0].dim;i++)
			mel[i]=mfcc[j*s[0].dim+i];
			
		test(mel,j);//娴嬭瘯姣忎竴甯у规墍鏈夎缁冩ā鍨嬬殑鍚庨獙姒傜巼鐨刲og
	}
	//璁＄畻鏈缁堢殑杈ㄨょ粨鏋
	for(j=0;j<fra_num;j++)
	{	
		for(i=0;i<sample_num;i++)
		{		
			s[i].result+=s[i].p0[j];
		}
	}
	for(i=0;i<sample_num;i++)
	{
		s[i].result=s[i].result/fra_num;
	}
}
void GaussianMixture::test(double *mel,int n)//娴嬭瘯姣忎竴甯у规墍鏈夎缁冩ā鍨嬬殑鍚庨獙姒傜巼鐨刲og
{
	int i,j,m;
	int sum=0;
	int nstate=s[0].nState;
	int dim=s[0].dim;
    double *p=new double[nstate];
	for(m=0;m<sample_num;m++)
	{
		for(i=0;i<nstate;i++)
			p[i]=0;
		for(i=0;i<nstate;i++)
		{
			sum=0;
			for(j=0;j<dim;j++)
			{
				sum+=-pow((mel[j]-s[m].mu[i*dim+j]),2)/s[m].sigma[i*dim+j]-log(s[m].sigma[i*dim+j])-dim*log(2*3.14159);
				p[i]+=sum*0.5;
			}
			p[i]+=log(s[m].priors[i]);
		}
		sum=0;

		sum=logAdd(p[0],p[1]);
		for(i=2;i<nstate;i++)
			sum=logAdd(sum,p[i]);
		s[m].p0[n]=sum; 
	}
}
double GaussianMixture::logAdd(double f0,double f1)
{
	double t;
	if(f0<f1)
	{
		t=f1;
		f1=f0;
		f0=t;
	}
	return f0+log(1+exp(f1-f0));
}
