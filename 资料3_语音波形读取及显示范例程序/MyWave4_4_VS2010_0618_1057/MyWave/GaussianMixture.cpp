//GMM类实现
#include"StdAfx.h"
#include<iostream>
#include<sstream>
#include<fstream>
#include "MyWaveDoc.h"
#include "Speech.h"
#include"VQ.h"
#include<cstdlib>
#include"MathLib.h"
#include"GaussianMixture.h"
using namespace std;

#define MIN 1.0e-2
#define DIFF_GMM_VALUE     1             //!< GMM归一阈值
#define MAXITER 20 //最大迭代次数
GaussianMixture::GaussianMixture()//构造函数
{
	priors=NULL;
	mu=NULL;
	sigma=NULL;
	s=NULL;
}
GaussianMixture::~GaussianMixture()//构造函数
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
GaussianMixture::GaussianMixture(GaussianMixture &g)//构造函数
{
	this->mu=g.mu;
	this->sigma=g.sigma;
	this->priors=g.priors;
	this->dim=g.dim;
	this->nState=g.nState;
}
Matrix GaussianMixture::loadDataFile(CString filename/*char filename[]*/)
{
	//load the dataset from a file 从文件中读取数据集
	Matrix result;
	Vector vecTmp;
	float valTmp;
	char tmp[1024];
	unsigned int l=0,c=0;

	std::ifstream f;
	f.open(filename);
	if(f.is_open())
	{
		//get number of row 获取行数
		while(!f.eof())
		{
			f.getline (tmp,1024);
			l++;
			if(l==1)
			{
				// get number of columns获取列数
				std::istringstream strm;
				strm.str(tmp);
				while(strm >> valTmp)
					c++;
			}
		}
		result.Resize (l-1,c);//then the matrix can be allocated 矩阵分配空间
		f.clear();
		f.seekg(0);//returns to beginning of the file返回文件开始
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
	//save the dataset to a file  保存数据集到文件中
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

	//save the result of regression 保存回归后的数据
	std::ofstream Mu_file(fileMu);//regressed data
	std::ofstream Sigma_file(fileSigma);//covariances matrices(matrix的复数)
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
bool GaussianMixture::loadParams(const char fileName[])//加载GMM参数
{
	//load coefficient of a GMM from a file(stored by saveParams Method or with matlab)
	//加载一个GMM模型的系数，通过saveParams函数或matlab保存
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
		sigma[s]==new double[dim];
		for(int i=0;i<dim;i++)
				fich>>sigma[s][i];
	}
	return true;
}
void GaussianMixture::saveParams(/*const char*/CString filename/*[]*/)//保存GMM的各参数
{
	//save the current GMM parameters,coefficents to a file
	//to be retrieved(取回)by the loadParams method
	std::ofstream file(filename);
	file<<dim<<" "<<nState<<std::endl;


	for(int i=0;i<nState;i++)//保存各个分量的加权值
		file<<priors[i]<<"  ";
	file<<std::endl;

	for(int s=0;s<nState;s++)//保存均值
	{
		for(int i=0;i<dim;i++)
			file<<mu[s][i]<<"  ";
		file<<std::endl;
	}

	for(int s=0;s<nState;s++)//保存协方差矩阵
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
	 
	const int dataSize =inData.ColumnSize();//示例中每个文件矢量的维数
	const int inSize   =inData.RowSize();//每个文件矢量个数
	const int outSize  =nbSteps;//示例中的139

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
	通过将数据分成时间片段（第1维数据表示时间）初始化高斯混合模型，然后计算每个片段的均值和协方差矩阵
	*/

	//Vector *mean=new Vector[nState];//nState 为混合分量的个数M
	//int nData=DataSet.RowSize();//矢量分量的个数
	//this->nState=nState;//混合高斯M
	//this->dim=DataSet.ColumnSize();//矢量维数D
	//float tmax=0;
	//Matrix index(nState,nData);
	//int *pop=new int[nState];
	//priors=new double[nState];//混合加权值wi
	//

	//Matrix unity(dim,dim);//defining unity matrix 单位矩阵 D*D
	//for(int k=0;k<dim;k++)unity(k,k)=1.0;

	//for(int n=0;n<nData;n++)//getting the max value for time 获取时间标号最大值
	//{
	//	if(DataSet(n,0)>tmax)tmax=DataSet(n,0);
	//}
	//for(int s=0;s<nState;s++)//clearing values
	//{
	//	mean[s].Resize(dim,true);
	//	mu[s]=new double[dim];//均值矢量
	//    sigma[s]=new double[dim];//协方差矩阵∑i
	//	pop[s]=0;
	//}

	///*
	//divide the dataset into slices of equal time (tmax/nState)and compute the mean of each slice
	//the pop table index to which slice belongs each sample
	//将数据集分成等分（tmax/nState），然后计算每一组的均值，pop表每个状态包含的矢量个数
	//*/
	//for(int n=0;n<nData;n++)
	//{
	//    int s = (int)((DataSet(n,0)/(tmax+1))*nState);
	//	mean[s]+=DataSet.GetRow(n);//mean 为nState* dim
	//	index(s,pop[s])=(float)n;//index用来标记状态s的pop[s]个矢量在数据集中是哪一行
	//	pop[s]+=1;
	//}

	//for(int s=0;s<nState;s++)
	//{
	//	mu.SetRow(mean[s]/(float)pop[s],s);//initiate the means computed before 初始化Ui,pop[s]为计算means[s]时的矢量数目,s为第s行
	//	sigma[s]=Matrix(dim,dim);
	//	priors[s]=1.0f/nState;//set equi-probables states

	//	for(int ind=0;ind<pop[s];ind++)
	//	{
	//		for(int i=0;i<dim;i++)
	//		{
	//			//computing covariance matrices 计算全协方差矩阵∑i
	//			for(int j=0;j<dim;j++)
	//				sigma[s](i,j)+=(DataSet((int)index(s,ind),i)-mu(s,i))\
	//				*(DataSet((int)index(s,ind),j)-mu(s,j));
	//			////对角协方差矩阵
	//			//sigma[s](i,i)+=(DataSet((int)index(s,ind),i)-mu(s,i))\
	//			//	*(DataSet((int)index(s,ind),i)-mu(s,i));
	//			
	//		}
	//	}
	//	sigma[s]*=1.0f/pop[s];
	//	sigma[s]+=unity*1e-5f;// prevents this matrix from being non-inversible确保该矩阵是可逆矩阵
	//}
	//ofstream fout("init_gmm.txt",ios::app);
	//fout<<"加权值"<<endl;
	//for(int i=0;i<nState;i++)
	//	fout<<priors[i]<<"  ";
	//fout<<endl;
	//fout<<"均值"<<endl;
	//for(int i=0;i<nState;i++)
	//{
	//	for(int j=0;j<dim;j++)
	//		fout<<mu(i,j)<<"   ";
	//	fout<<endl;
	//}
	//fout<<"协方差矩阵"<<endl;
	//for(int s=0;s<nState;s++)
	//{
	//	fout<<"状态"<<s<<"协方差矩阵"<<endl;
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
		Matrix  DataSet,                 //输入样本矩阵
		unsigned int nState,                      //高斯混合阶数M
		unsigned int   nMaxReapt,        //最大迭代次数
		double         fMinChange,      //畸变改进阈值
		double         fInitDistortion //初始畸变;
		)
{
	//Vector *mean=new Vector[nState];//nState 为混合分量的个数M
	//int nData=DataSet.RowSize();//矢量分量的个数
	//this->nState=nState;//混合高斯M
	//this->dim=DataSet.ColumnSize();//矢量维数D
	//float tmax=0;
	//Matrix index(nState,nData);
	//int *pop=new int[nState];
	//priors=new double[nState];//混合加权值wi
	//mu=new double*[nState];//均值矢量
	//sigma=new double*[nState];//协方差矩阵∑i

	//Matrix unity(dim,dim);//defining unity matrix 单位矩阵 D*D
	//for(int k=0;k<dim;k++)unity(k,k)=1.0;
 //   
	//for(int s=0;s<nState;s++)//clearing values
	//{
	//	mu[s]=new double[dim];
	//	sigma[s]=new double[dim];
	//	pop[s]=0;
	//}

	//Vector *pCodeBook=new Vector[nState];//LBG算法生成的码本
	//
	////当前总畸变
	//double fCurDistortion = fInitDistortion;
	////调用LBGRepeat返回的总畸变
	//double fRetDistortion = 0;
	////当前迭代次数
	//unsigned int nCurRepeat = 0;
	////当前畸变改进量
	//double fCurChange = fInitDistortion;
	//
	////若初始码本为空，则用输入序列的前nState个样本为初始码本

	//	for(int i=0;i<nState;i++)
	//		pCodeBook[i]=DataSet.GetRow (i*5);
 //    //		memcpy(pOutCodeBook, DataSet, sizeof(double) * nState);

	////调用LBGRepeat进行迭代生成初始码书
	//while(nCurRepeat < nMaxReapt && fMinChange < fCurChange)
	//{
	//	fRetDistortion = GaussianMixture::LBGRepeat (DataSet, pCodeBook, nState);

	//	//计算当前畸变改进量
	//	fCurChange = abs(fCurDistortion - fRetDistortion) / fCurDistortion;
	//	fCurDistortion = fRetDistortion;
	//	nCurRepeat++;
	//}

	//double tempDistance,fDistance;
	//unsigned int temp;
	//for(int i=0;i<nData;i++)//计算每个矢量到那个码字的距离最近，将特征矢量分成nState类
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
	//   	mean[temp]+=DataSet.GetRow(i);//mean 为nState* dim
	//	index(temp,pop[temp])=(float)i;//index用来标记状态s的pop[s]个矢量在数据集中是哪一行
	//	pop[temp]+=1;
	//}
	////初始化GMM参数
	//for(int s=0;s<nState;s++)
	//{
	//	mu.SetRow(mean[s]/(float)pop[s],s);//initiate the means computed before 初始化Ui,pop[s]为计算means[s]时的矢量数目
	//	sigma[s]=new double[dim];
	//	priors[s]=1.0f/nState;//set equi-probables states

	//	for(int ind=0;ind<pop[s];ind++)
	//	{
	//		for(int i=0;i<dim;i++)
	//		{
	//			//computing covariance matrices 计算全协方差矩阵∑i
	//			for(int j=0;j<dim;j++)
	//				sigma[s](i,j)+=(DataSet((int)index(s,ind),i)-mu[s][j])\
	//				*(DataSet((int)index(s,ind),j)-mu[s][j]);
	//			////对角协方差矩阵
	//			//sigma[s](i,i)+=(DataSet((int)index(s,ind),i)-mu(s,i))\
	//			//	*(DataSet((int)index(s,ind),i)-mu(s,i));
	//			
	//		}
	//	}
	//	sigma[s]*=1.0f/pop[s];
	//	sigma[s]+=unity*1e-5f;// prevents this matrix from being non-inversible确保该矩阵是可逆矩阵
	//}
 //    ofstream fout("init_gmm_lbg.txt",ios::trunc);
	//fout<<"加权值"<<endl;
	//for(int i=0;i<nState;i++)
	//	fout<<priors[i]<<"  ";
	//fout<<endl;
	//fout<<"均值"<<endl;
	//for(int i=0;i<nState;i++)
	//{
	//	for(int j=0;j<dim;j++)
	//		fout<<mu[i][j]<<"   ";
	//	fout<<endl;
	//}
	//fout<<"协方差矩阵"<<endl;
	//for(int s=0;s<nState;s++)
	//{
	//	fout<<"状态"<<s<<"协方差矩阵"<<endl;
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
					Matrix DataSet,		// 输入样本序列
					Vector * pCodeBook,	// 输入/出码本
					unsigned int nState	// 码本长度
					)
{
	//每个码书包含的样本的个数，相当于Matlab版中的N1
	unsigned int *pnSampleNums=new unsigned int[nState];
	//聚类是进行样本叠加值，相当于MATLAB版中的y(m)
	Vector* pfSampleSums=new Vector[nState];
	//保存总畸变
	double fDistortion = 0;
	//保存相邻的码矢下标
	unsigned int nNearCode = 0;
	//保存样本与码矢距离
	double fDistance = 0;
	//循环变量
	unsigned int i= 0, j =0;
    double tempDistance;
	unsigned int nData=DataSet.RowSize();
	unsigned int nColumnSize=DataSet.ColumnSize();
	//初始化聚类时每个码矢代表的样本数
	for(int s=0;s<nState;s++)
	{
		pfSampleSums[s].Resize(nColumnSize,false);
		pfSampleSums[s].Zero();
	}

	memset(pnSampleNums, 0, sizeof(unsigned int) * nState);

	//按照输入码本对输入样本序列进行分段
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
		//计算总畸变
		fDistortion+=fDistance;
		pfSampleSums[nNearCode] +=DataSet.GetRow(i);
		pnSampleNums[nNearCode]++;//保存每类中包含的矢量数目
	}
	//计算新码本
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
	//实现K均值聚类
void GaussianMixture::KMeansCluster(
		Matrix  DataSet,                 //输入样本矩阵
		unsigned int nState,                      //高斯混合阶数M
		double         fMinChange    //畸变改进阈值
		)
{
	Vector *mean=new Vector[nState];//nState 为混合分量的个数M
	int nData=DataSet.RowSize();//矢量分量的个数
	this->nState=nState;//混合高斯M
	this->dim=DataSet.ColumnSize();//矢量维数D
	int *pop=new int[nState];//记录每一类的数目	
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
    ///随机产生初始聚类中心
	for(s=0;s<nState;s++)//clearing values
	{
		for(j=0;j<this->dim;j++)
		{
			Center[s][j]=DataSet(s*10,j);
		}
	}
	
	//若初始码本为空，则用输入序列的前nState个样本为初始码本

	//调用KMeansClusterRepeat进行迭代生成初始码书
	int farIndex;
	double minDistance,distance,mmindist=1,lastmmindist;
	int iter=0;
	do{
		//fCurChange= GaussianMixture::KMeansClusterRepeat (DataSet, pCodeBook, nState,pNewCodeBook);
		   
		lastmmindist=mmindist;
		mmindist=0;
		memset(pop,0,sizeof(int)*nState);
		memset(clusterIndex,0,sizeof(int)*nData);
       
		for(j=0;j<nData;j++)//分类
		{
			farIndex=0;
			minDistance=getDistance(Center[0],DataSet.GetRow(j));
			////////////////分类过程，farIndex为向量所属质心索引
			for(i=1;i<nState;i++)//i表示现有质心索引
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


	   //更新聚类中心,现数值清零
		for(i=0;i<nState;i++)
		{
			for(j=0;j<dim;j++)
			{
				mu[i][j]=0;
				sigma[i][j]=0;
			}
		}
		//得到新的质心（如果结果不合理，可以继续更新过程）
		
		for(j=0;j<nData;j++)
		{
			for(i=0;i<nState;i++)
			{
				if(clusterIndex[j]==i)
				{
					for(m=0;m<dim;m++)
						mu[i][m]+=DataSet(j,m);//设置新的质心,用setRow()赋值
				}
			}				
		}
		////////////////////////M步骤

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
		//fout<<"权值"<<endl;
		//for(i=0;i<nState;i++)
		//{
		//	fout<<priors[i]<<"   ";			
		//}
		//fout<<endl;
		//fout<<"均值"<<endl;
		//for(i=0;i<nState;i++)
		//{
		//	for( j=0;j<dim;j++)
		//		fout<<mu[i][j]<<"   ";
		//	fout<<endl;
		//}
		//fout<<"方差"<<endl;
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
		fout<<"权值"<<endl;
		for(i=0;i<nState;i++)
		{
			fout<<priors[i]<<"   ";			
		}
		fout<<endl;
		fout<<"均值"<<endl;
		for(i=0;i<nState;i++)
		{
			for( j=0;j<dim;j++)
				fout<<mu[i][j]<<"   ";
			fout<<endl;
		}
		fout<<"方差"<<endl;
		for(i=0;i<nState;i++)
		{
			for( j=0;j<dim;j++)
				fout<<sigma[i][j]<<"   ";
			fout<<endl;
		}
		fout.close();*/
}
	//实现K均值聚类迭代，输出总畸变
double GaussianMixture::KMeansClusterRepeat(
		Matrix DataSet,		// 输入样本序列
		Vector * pCodeBook,	// 输入/出码本
		unsigned int nState,	// 码本长度
		Vector * pNewCodeBook
		)
{
	//每个码书包含的样本的个数，相当于Matlab版中的N1
	unsigned int *pnSampleNums=new unsigned int[nState];
	//聚类是进行样本叠加值，相当于MATLAB版中的y(m)
	Vector* pfSampleSums=new Vector[nState];
	//保存总畸变
	double fDistortion = 0;
	//保存相邻的码矢下标
	unsigned int nNearCode = 0;
	//保存样本与码矢距离
	double fDistance = 0;
	//循环变量
	unsigned int i= 0, j =0;
    double tempDistance;
	unsigned int nData=DataSet.RowSize();
	unsigned int nColumnSize=DataSet.ColumnSize();
	//初始化聚类时每个码矢代表的样本数
	for(int s=0;s<nState;s++)
	{
		pfSampleSums[s].Resize(nColumnSize,false);
		pfSampleSums[s].Zero();
	}

	memset(pnSampleNums, 0, sizeof(unsigned int) * nState);
	////初始化聚类时进行样本叠加值
	//memset(pfSampleSums, 0,sizeof(double) * nState);

	//按照输入码本对输入样本序列进行分段
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
		pnSampleNums[nNearCode]++;//保存每类中包含的矢量数目
	}
	//计算新码本
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
//计算|ci(m+1)-ci(m)|
	
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
		Matrix  DataSet,                 //输入样本矩阵
		unsigned int nState,                      //高斯混合阶数M
		int   *cSize,//每个聚类所含向量数
		int*clusterIndex//每个聚类所含向量索引
		)
{
	Vector *mean=new Vector[nState];//nState 为混合分量的个数M
	int nData=DataSet.RowSize();//矢量分量的个数
	this->nState=nState;//混合高斯M
	this->dim=DataSet.ColumnSize();//矢量维数D
	int *pop=new int[nState];//记录每一类的数目
	//priors=new float[nState];//混合加权值wi
	//mu.Resize(nState,dim);//均值矢量
	//sigma=new Matrix[nState];//协方差矩阵∑i

	
	int i,j,k,s,m;
	//Matrix unity(dim,dim);//defining unity matrix 单位矩阵 D*D
	//for( k=0;k<dim;k++)unity(k,k)=1.0;

    //int *clusterIndex=new int[nData];//所属类索引
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

	
///////////////////////////////////////////////求特征矢量集的均值
	for( i=0;i<nData;i++)
	{
		mean[0]+=DataSet.GetRow (i);
	}
	mean[0]=mean[0]/nData;
	//////////////////////////////////////////////////////////////////////////////方差
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
    
	double **Center=NULL;  // 保存临时质心
  	Center=new double*[nState];
	for(i=0;i<nState;i++)
	{
        Center[i]=new double[dim];
		memset(Center[i],0,sizeof(double)*dim);
	}
 
	 //for(s=0;s<dim;s++)
		//Center[0][s]=mu.GetRow(0).GetArray()[s];//保存已有的聚类中心
	 for(j=0;j<dim;j++)
	 {
		 mu[0][j]=mean[0][j]-sigma[0][j]*0.01;
		 mu[1][j]=mean[0][j]+sigma[0][j]*0.01;
	 }
	
  //int *num=new int[nData];//标记属于哪一类
  int farIndex;
  double minDistance,distance;
  double lastmmindist,mmindist=1.0;
  for(k=2;k<=nState;k=2*k)
  {	
		if(k!=2)
		{
			for(j=0;j<k;j++)
			  for(s=0;s<dim;s++)
			    mu[j][s]=Center[j][s];//保存已有的聚类中心
		}
      
		do{	
			for(j=0;j<k;j++)
			  for(s=0;s<dim;s++)
			   Center[j][s]= mu[j][s];//保存已有的聚类中心

            lastmmindist=mmindist;
			mmindist=0;
			memset(pop,0,sizeof(int)*nState);
			memset(clusterIndex,0,sizeof(int)*nData);
			for(j=0;j<nData;j++)//分类
			{
				farIndex=0;
				minDistance=getDistance(Center[0],DataSet.GetRow(j));
				////////////////分类过程，farIndex为向量所属质心索引
				for(i=1;i<k;i++)//i表示现有质心索引
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

		  //更新聚类中心,现数值清零
			for(i=0;i<k;i++)
			{
				for(j=0;j<dim;j++)
				{
					mu[i][j]=0;
					sigma[i][j]=0;
				}
				priors[i]=0;
			}
			//得到新的质心（如果结果不合理，可以继续更新过程）
			
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
			//fout<<"均值"<<endl;
			//for(i=0;i<k;i++)
			//{
			//	for( j=0;j<dim;j++)
			//		fout<<mu[i][j]<<"   ";
			//	fout<<endl;
			//}
			//fout<<"方差"<<endl;
			//for(i=0;i<k;i++)
			//{
			//	for( j=0;j<dim;j++)
			//		fout<<sigma[i][j]<<"   ";
			//	fout<<endl;
			//}
			//fout.close();

		}while(fabs(lastmmindist-mmindist)>0.0001*lastmmindist);
        


		//////////////////////////////////////////////////////////更新聚类中心
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
		//fout<<"均值"<<endl;
		//for(i=0;i<k*2;i++)
		//{
		//	for( j=0;j<dim;j++)
		//		fout<<Center[i][j]<<"   ";
		//	fout<<endl;
		//}
		//fout<<"方差"<<endl;
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
	perform Exceptation/Maximization on the given Dataset :(对所给数据集Dataset执行EM算法)
	EM算法包含两个主要方面：一是求期望(Exceptation),用E表示；一是最大化（maximization）,用M表示
	matrix DataSet(nSamples,Dimensions)
	the GaussianMixture Object must be initialisted before (混合高斯对象必须提前初始化)
	see initEM_TimeSplit method */

	int nData=DataSet.RowSize();
	int iter=0;
	float log_lik;
	float log_lik_threshold=0.001;
	float log_lik_old=-1e10f;
    float sum_log;
	//Matrix unity(dim,dim);
	//for(int k=0;k<dim;k++)unity(k,k)=1.0;

	////EM loop EM算法迭代
	// 
	//while(true)
	//{
	//	float * sum_p=new float[nData];
	//	Matrix pxi(nData,nState);//帧数*M，似然度
	//	Matrix pix(nData,nState);//后验概率
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
	//	//Expectation Computing 计算期望
	//	for(int i=0;i<nData;i++)
	//	{
	//		sum_p[i]=0;
	//		for(int j=0;j<nState;j++)
	//		{
	//			float p=pdfState(DataSet.GetRow(i),j);//P(x|i),pdfState返回值为bi(x),D维高斯密度函数
	//			if(p==0)
	//			{
	//				AfxMessageBox(_T("Error: Null probability. Abort.."));
	//				return -1;					
	//			}
	//			pxi(i,j)=p;
	//			//fout<<pxi(i,j)<<"  ";
	//			sum_p[i]+=p*priors[j];//p(x|λ),观察矢量xi在该λ GMM下的似然度
	//		}
	//		sum_log+=log(sum_p[i]);//训练特征矢量序列X={xt,t=1,...,T}，sum_log为似然概率P(X|λ)=P(x1|λ)*P(x2|λ)*...P(xT|λ)的对数
	//		//fout<<endl;
	//	}
	//	fout1<<"pix:"<<endl;
	//	for(int j=0;j<nState;j++)
	//	{
	//		for(int i=0;i<nData;i++){
	//			pix(i,j)=pxi(i,j)*priors[j]/sum_p[i];//then P(i|x) 求取训练数据在第i状态下的后验概率
 //            fout1<<pix(i,j)<<"   ";
	//	  }
	//	  fout1<<endl;
	//	}
	//	fout1.close();

	//	//here we compute the log likehood 计算对数似然函数
	//	log_lik=sum_log/nData; 
	//	if(fabs((log_lik/log_lik_old)-1)<log_lik_threshold)
	//	{
	//		return iter;
	//	}
	//	log_lik_old=log_lik;

	//	//Update step  M过程：对Q求各种参数的估计值
	//	pix.SumRow(E);//E为∑P(i|x) E为1*M
	//	
	//	for(int j=0;j<nState;j++)
	//	{
	//		priors[j]=E(j)/nData;//new priors 更新加权值wi=∑P(i|x)/T;
	//		Vector tmu(dim);
	//		Matrix tmsigma(dim,dim);
	//		tmsigma.Zero();
 //           Vector sum(dim);
	//		////////////////////////////////////////////////////////////////方法1,没调通
	//		//for(int i=0;i<nData;i++)//means updata loop更新均值
	//		//{

	//		//	tmu+=DataSet.GetRow(i)*pix(i,j);//∑P(i|x)*xt
	//		//	for(int k=0;k<dim;k++)
	//		//	{
	//		//		tmsigma(k,k)+=pow(DataSet(i,k),2)*pix(i,j);
	//		//	}
	//		//}
	//		////∑P(i|x)*xt*xt
	//		//mu.SetRow(tmu/E(j),j);
 //  //         sigma[j]=tmsigma/E[j];
 //  //         for(int i=0;i<dim;i++)
	//		//{
	//		//	sigma[j](i,i)=sigma[j](i,i)-pow(mu(j,i),2);
	//		//		
	//		//}
 //  //        sigma[j]=sigma[j]+unity*1e-5f;

	//	   ////////////////////////////////////////////////////////方法2 
	//	   for(int i=0;i<nData;i++)//means updata loop更新均值
	//		{
	//			tmu+=DataSet.GetRow(i)*pix(i,j);//∑P(i|x)*xt
	//		}
	//		mu.SetRow(tmu/E(j),j);
	//		for(int i=0;i<nData;i++)//covariances updates 更新协方差矩阵
	//		{
	//			for(int k=0;k<dim;k++)
	//			{
	//				Matrix Dif(dim,1);
	//				Dif.SetColumn((DataSet.GetRow(i)-mu.GetRow(j)),0);//xt-mui					
	//				tmsigma+=(Dif*Dif.Transpose())*pix(i,j);//∑(pix(i,j)*(xt-mui)2)
	//				///*tmsigma(j,k)+=pix(i,j)*pow(DataSet.GetRow(i).GetArray()[k],2)*/
	//			}
	//		}
	//		sigma[j]=tmsigma/E[j]+unity*1e-5f;
	//	}  
	//ofstream fout("KMeans_1_GMM.txt",ios::trunc);
	//	fout<<"加权值"<<endl;
	//for(int i=0;i<nState;i++)
	//	fout<<priors[i]<<"  ";
	//fout<<endl;
	//fout<<"均值"<<endl;
	//for(int i=0;i<nState;i++)
	//{
	//	for(int j=0;j<dim;j++)
	//		fout<<mu(i,j)<<"   ";
	//	fout<<endl;
	//}
	//fout<<"协方差矩阵"<<endl;
	//for(int s=0;s<nState;s++)
	//{
	//	fout<<"状态"<<s<<"协方差矩阵"<<endl;
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
	//获得一个给定观察矢量和给定状态的D维高斯函数
	//Matrix inv_sigma;
	//float det_sigma=1.0f;//矩阵的行列式
	//double p;
	//Vector dif;

	////sigma[state].Inverse(inv_sigma,&det_sigma);//求协方差矩阵的逆矩阵,det_sigma为行列式的值，本函数经验证是正确的
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
	//	p=exp(-0.5*p)/sqrt(pow(2*3.14159,dim/2)*pow((double)fabs(det_sigma),0.5));//fabs(x)函数是求浮点数x的绝对值
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
#define DIFF_CENTER_VALUE 1//k均值阈值
/** @function
********************************************************************************
<PRE>
函数名: KMeans()
功能: 通过k增值算法得到M组均值
用法:
参数:
[IN] DataSet: 特征向量矩阵
[IN] nState: 高斯混合数
[OUT] clusterIndex: 每个聚类所含向量索引
[OUT] cSize: 每个聚类所含向量数
返回:
调用: getDistance()
主调函数: InitGMM()
</PRE>
*******************************************************************************/
void GaussianMixture::KMeans(
		Matrix  DataSet,                 //输入样本矩阵
		unsigned int nState,                      //高斯混合阶数M
		int   *cSize,//每个聚类所含向量数
		int*clusterIndex//每个聚类所含向量索引
		)
{
	Vector *mean=new Vector[nState];//nState 为混合分量的个数M
	int nData=DataSet.RowSize();//矢量分量的个数
	this->nState=nState;//混合高斯M
	this->dim=DataSet.ColumnSize();//矢量维数D
	int *pop=new int[nState];//记录每一类的数目	
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
///////////////////////////////////////////////求特征矢量集的均值
	for( i=0;i<nData;i++)
	{
		mean[0]+=DataSet.GetRow (i);
	}
	mean[0]=mean[0]/nData;
	/////////////////////////////////////////////总的均值
	for(i=0;i<dim;i++)
		mu[0][i]=mean[0][i];
	//////////////////////////////////////////////////////////////////////////////方差
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
    
	double **Center=NULL;  // 保存临时质心
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
		// 新增分裂点：计算每个向量到每个质心的距离,寻找离现有质心最远的向量作为新质心
		//
		for(j=0;j<nData;j++)
		{		
			farIndex=0;
		    maxDistance=0;
			distance=0;
			//向量到每个质心的距离和,距离和最大的为新的质心
			for(i=0;i<k-1;i++)//i表示现有质心索引
				distance+=sqrt(getDistance(Center[i],DataSet.GetRow(j)));
			if(distance>maxDistance)
			{
				maxDistance=distance;
				farIndex=j;
			}
		}
		for(j=0;j<dim;j++)
			mu[k-1][j]=DataSet(farIndex,j);// 新增质心
			
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
			   Center[j][s]= mu[j][s];//保存已有的聚类中心

            lastmmindist=mmindist;
			mmindist=0;
			memset(pop,0,sizeof(int)*nState);
			memset(clusterIndex,0,sizeof(int)*nData);
			for(j=0;j<nData;j++)//分类
			{
				farIndex=0;
				minDistance=getDistance(Center[0],DataSet.GetRow(j));
				////////////////分类过程，farIndex为向量所属质心索引
				for(i=1;i<k;i++)//i表示现有质心索引
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

		  //更新聚类中心,现数值清零
			for(i=0;i<k;i++)
			{
				for(j=0;j<dim;j++)
				{
					mu[i][j]=0;
					sigma[i][j]=0;
				}
			}
			//得到新的质心（如果结果不合理，可以继续更新过程）
			
			for(j=0;j<nData;j++)
			{
				for(i=0;i<k;i++)
				{
					if(clusterIndex[j]==i)
					{
						for(m=0;m<dim;m++)
							mu[i][m]+=DataSet(j,m);//设置新的质心,用setRow()赋值
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
		//fout<<"均值"<<endl;
		//for(i=0;i<k;i++)
		//{
		//	for( j=0;j<dim;j++)
		//		fout<<mu[i][j]<<"   ";
		//	fout<<endl;
		//}
		//fout<<"方差"<<endl;
		//for(i=0;i<k;i++)
		//{
		//	for( j=0;j<dim;j++)
		//		fout<<sigma[i][j]<<"   ";
		//	fout<<endl;
		//}
		//fout.close();

		}while(fabs(lastmmindist-mmindist)>0.0001*lastmmindist);
        


		////////////////////////////////////////////////////////////更新聚类中心
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
函数名: InitGMM()
功能: 获得GMM初值
用法:
参数:
[IN]  DataSet: 特征向量矩阵
[IN] train_frame_num: 训练帧帧数
[IN] nState: 高斯混合数
[OUT] mu,sigma,priors
返回:
TRUE: 成功
FALSE：失败
调用: KMeans()
主调函数: GMMs()
</PRE>
*******************************************************************************/
bool GaussianMixture::InitGMM(
			  Matrix  DataSet,                 //输入样本矩阵
		      unsigned int nState )                     //高斯混合阶数M
{
    int i;
	int j;
	int s;
	int *clusterIndex=NULL;//向量所属质心索引
	int *clusterSize=NULL;//聚类所含向量数
	int nData=DataSet.RowSize();//矢量分量的个数
	this->nState=nState;//混合高斯M
	this->dim=DataSet.ColumnSize();//矢量维数D


	clusterIndex=new int[nData];//nData为训练帧数
	clusterSize=new int[nState];//聚类数M
	memset(clusterIndex,0,sizeof(int)*nData);
	memset(clusterSize,0,sizeof(int)*nState);
	priors=new double[nState];//混合加权值wi
	mu=new double*[nState];//均值矢量
	sigma=new double*[nState];//协方差矩阵∑i
	for(i=0;i<nState;i++)
	{
		priors[i]=0;
		mu[i]=new double[dim];
		sigma[i]=new double[dim];
	}
 
     KMeansCluster(DataSet,nState,100);//随机初始化化Kmeans
	/////////////////////
	//SeCession_KMeansCluster( DataSet, nState,clusterSize,clusterIndex);//分裂法和k均值结合
	///////////////////////////////////////////////////////////////////////////////
	//初始化均值,每次新添加一个质心迭代
	//KMeans(DataSet,nState,clusterSize,clusterIndex);
		//初始化加权系数w
	//for(i=0;i<nState;i++)
	//	priors[i]=(double)clusterSize[i]/nData;


	//ofstream fout("init_gmm_keans1.txt",ios::trunc);
	//fout<<"加权值"<<endl;
	//for( i=0;i<nState;i++)
	//	fout<<priors[i]<<"  ";
	//fout<<endl;
	//fout<<"每类含个数"<<endl;
	//for( i=0;i<nState;i++)
	//	fout<<clusterSize[i]<<"  ";
	//fout<<endl;
	//fout<<"均值"<<endl;
	//for( i=0;i<nState;i++)
	//{
	//	for(int j=0;j<dim;j++)
	//		fout<<mu(i,j)<<"   ";
	//	fout<<endl;
	//}
	//fout<<"协方差矩阵"<<endl;
	//for(int s=0;s<nState;s++)
	//{
	//	fout<<"状态"<<s<<"协方差矩阵"<<endl;
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
函数名: BuildGMM()
功能: 迭代下一轮GMM
用法:
参数:
[IN] DataSet: 特征向量矩阵
[IN] nState: 高斯混合数
返回: 
TRUE: 成功
FALSE: 失败
调用: GMM_density()
主调函数: GMMs()
</PRE>
*******************************************************************************/
bool GaussianMixture::BuildGMM(
			  Matrix  DataSet,                 //输入样本矩阵
		      unsigned int nState )                     //高斯混合阶数M
{
	int iter=0;//迭代次数
    int nData=DataSet.RowSize();
	int i,j,k;
	double **pxi=NULL;//M*帧数，似然度
	double **pix=NULL;//后验概率
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
	double * sum_pxi=new double[nData];//保存特征向量xt在M个状态下的概率总和sum(pxi)
	double * sum_pix=new double[nState];//在M个状态下的后验概率和sum(pix)
	//EM loop EM算法迭代
	//while(diff>=DIFF_GMM_VALUE)
	do{
		sum_log_old=sum_log;
		//Expectation Computing 计算期望
		//计算事后概率
		memset(sum_pxi,0,sizeof(double)*nData);
		for( i=0;i<nState;i++)
		{
			for( j=0;j<nData;j++)
		   {
			   //float p=pdfState(DataSet.GetRow(i),j);
				pxi[i][j]=priors[i]*GMM_density(DataSet.GetRow(j),i);//P(x|i),GMM_density返回值为bi(x),D维高斯密度函数;
				//p=log(pxi[i][j]);//测试数据
				sum_pxi[j]+=pxi[i][j];//p(x|λ),观察矢量xj在该λ GMM下的似然度，相当于w1*b1(xt)+.....+Wm*bm(Xt)
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
		//求取训练数据在第i个状态下的后验概率
		sum_log=0;
		memset(sum_pix,0,sizeof(double)*nState);
        for(j=0;j<nData;j++)
		{
			for(i=0;i<nState;i++)
			{
				pix[i][j]=pxi[i][j]/sum_pxi[j];
				sum_pix[i]+=pix[i][j];
			}			
			sum_log+=log(sum_pxi[j]);//训练特征矢量序列X={xt,t=1,...,T}，sum_log为似然概率P(X|λ)=P(x1|λ)*P(x2|λ)*...P(xT|λ)的对数
		}
		////here we compute the log likehood 计算对数似然函数
		sum_log=sum_log/nData;//按帧平均????????????????????
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

		//Update step  M过程：对Q求各种参数的估计值
		//
		//计算下一次迭代地GMM
		//将原来的数据清零
        for(i=0;i<nState;i++)
		{
			priors[i]=0;
			for(j=0;j<this->dim;j++)
			{
				mu[i][j]=0;
				sigma[i][j]=0;
			}
		}
		//////更新参数
		for(i=0;i<nState;i++)
		{
			priors[i]=sum_pix[i]/nData;//new priors 更新加权值wi=∑P(i|x)/T;
			//计算均值和协方差矩阵
			for(j=0;j<dim;j++)
			{
				for( k=0;k<nData;k++)//means updata loop更新均值
				{
					mu[i][j]=mu[i][j]+pix[i][k]*DataSet(k,j);//∑P(i|x)*xt
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
	//fout<<"加权值"<<endl;
	//for(i=0;i<nState;i++)
	//	fout<<priors[i]<<"  ";
	//fout<<endl;
	//fout<<"均值"<<endl;
	//for( i=0;i<nState;i++)
	//{
	//	for( j=0;j<dim;j++)
	//		fout<<mu[i][j]<<"   ";
	//	fout<<endl;
	//}
	//fout<<"协方差矩阵"<<endl;
	//for( i=0;i<nState;i++)
	//{
	//	fout<<"状态"<<i<<"协方差矩阵"<<endl;
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
函数名: GMM_density()
功能: 计算高斯概率密度
用法:
参数:
[IN] v: 特征向量
[IN] statr: 高斯混合模型阶数索引
返回: 高斯概率密度值
调用:
主调函数: BuildGMM()
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
		////对角矩阵的对角线中出现0，导致无穷大1#INF
		temp+=pow(v(k)-this->mu[s][k],2)/this->sigma[s][k];
		////sqrt出现-nan，因为开方值为负
		sqrt_Matrix_value*=sqrt(fabs(this->sigma[s][k]));
	}
	//temp值太大，导致exp(temp/-2)为0
	res=exp(temp/-2)/pow(2*3.14159,D/2)/sqrt_Matrix_value;
	return res;
}
/** @function
********************************************************************************
<PRE>
函数名: GMMs()
功能: GMM建模过程
用法:
参数:
[IN] DataSet: 特征向量矩阵
[IN] nState: 高斯混合数
返回:
TRUE: 成功
FALSE: 失败
调用: doRM(), GMM_density()
主调函数:
</PRE>
*******************************************************************************/
bool GaussianMixture::GMMs( Matrix  DataSet,unsigned int nState)
{	
 	if(!InitGMM(DataSet,nState))//初始化GMM
	{
         return false;
	}
	if(!BuildGMM(DataSet,nState))//EM迭代
	{
		return false;
	}

	return true;
}
void GaussianMixture::load()//把GmmFile中保存的每个文件中的数据取出来。
{
	CString filename;//记录保存在gmm文件夹中的文件名
	CFileFind finder;
	std::ifstream fich;
	//steep 1:找出有多少个样本
	int count=0;
	//BOOL bWorking=finder.FindFile (_T("F:\\程序\\毕业设计\\实验数据_603\\30s\\N3作为训练数据\\随机训练\\lpc\\N3_lpc_gmm_32\\*.txt"));
	//BOOL bWorking=finder.FindFile (_T("F:\\程序\\毕业设计\\实验数据_603\\60s\\T3作为训练数据\\随机训练\\mfcc13\\T3_gmm_32\\*.txt"));
	BOOL bWorking=finder.FindFile (_T("F:\\程序\\毕业设计\\timit实验\\gmm_32\\*.txt"));
	while(bWorking)
	{
		count++;
		bWorking=finder.FindNextFileW();
	}

	//创建样本，分配内存
	sample_num=count;
    s=new Sample[count];
	//steep 3:遍历每个样本，并载入
    CString m_gmmname=NULL;
	bWorking=finder.FindFile (_T("F:\\程序\\毕业设计\\timit实验\\gmm_32\\*.txt"));
	//bWorking=finder.FindFile (_T("F:\\程序\\毕业设计\\实验数据_603\\60s\\T3作为训练数据\\随机训练\\mfcc13\\T3_gmm_32\\*.txt"));
	int m=0;
	int i,j;
	while(bWorking)
	{
		s[m].name="";
		bWorking=finder.FindNextFileW();
		filename=(LPCTSTR)finder.GetFileTitle();
		m_gmmname=_T("F:\\程序\\毕业设计\\timit实验\\gmm_32\\")+filename+_T(".txt");
		//m_gmmname=_T("F:\\程序\\毕业设计\\实验数据_603\\60s\\T3作为训练数据\\随机训练\\mfcc13\\T3_gmm_32\\")+filename+_T(".txt");

		fich.open(m_gmmname.GetBuffer(0));

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
	load();//加载训练模型

	int i=0,j=0;
	double *mel=new double[s[0].dim];
    ////初始化辨认结果
	for(i=0;i<sample_num;i++)
	{
		
		s[i].p0=new double[fra_num];
		s[i].result=0.0;
	}
	for(j=0;j<fra_num;j++)
	{
		for(i=0;i<s[0].dim;i++)
			mel[i]=mfcc[j*s[0].dim+i];
			
		test(mel,j);//测试每一帧对所有训练模型的后验概率的log
	}
	//计算最终的辨认结果
	for(i=0;i<sample_num;i++)
	{	
		for(j=0;j<fra_num;j++)
		{		
			s[i].result+=s[i].p0[j];
		}
		s[i].result=s[i].result/fra_num;
	}
	// 对与每个说话人模型进行匹配最后得到的评分进行排序	
	for(int t=1;t<=sample_num-1;t++)   
	{
		for(i=0;i<sample_num-t;i++)
		{
			if(s[i].result<s[i+1].result)
			{
				int j=s[i].result;
				s[i].result=s[i+1].result;
				s[i+1].result=j;
				CString jj = s[i].name;
				s[i].name = s[i+1].name;
				s[i+1].name = jj; 
			}
		}
	}


	//AfxMessageBox(_T("说话人辨认：该说话人为 ")+s[0].name);

}
void GaussianMixture::verify(double  *mfcc,int fra_num,int WinSize)
{
	load();//加载训练模型
	int i=0,j=0;
	double *mel=new double[s[0].dim];
    ////初始化辨认结果
	for(i=0;i<sample_num;i++)
	{
		s[i].p0=new double[fra_num];
		s[i].result=0.0;
	}
	for(j=0;j<fra_num;j++)
	{
		for(i=0;i<s[0].dim;i++)
			mel[i]=mfcc[j*s[0].dim+i];
			
		test(mel,j);//测试每一帧对所有训练模型的后验概率的log
	}
	//计算最终的辨认结果
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
void GaussianMixture::test(double *mel,int n)//测试每一帧对所有训练模型的后验概率的log
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
