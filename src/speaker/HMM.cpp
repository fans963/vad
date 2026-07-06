#include "MFCStub.h"
#include <QString>
#include <QVector>
#include"VQ.h"
#include"HMM.h"
///////////////////////////////////
//鏋勯犲嚱鏁
//////////////////////////////////
CHMM::CHMM()
{}
CHMM::~CHMM()
{}

//////////////////////////////////////////////////////////////////////////
// 瀹炵幇瀵笻MM妯″瀷鐨勮凯浠ｈ缁
// 
// 鍒涘缓浜:		
// 鍒涘缓鏃ユ湡:	
// 淇鏀逛汉:
// 淇鏀规棩鏈:
void CHMM::Train (
	    const double * pDataIn,   //杈撳叆閲囨牱搴忓垪
		unsigned int nInLen       //杈撳叆閲囨牱搴忓垪闀垮害
		)
{
	//鍓嶅悗鍚戠畻娉曡繑鍥炲弬鏁
	double fRate = 0;
	double* pa = NULL;
	double* pb = NULL;

	//瀵硅緭鍏ラ噰鏍峰簭鍒楄繘琛岀煝閲忛噺鍖栧舰鎴愮殑鐮佹湰
	double* pCodeBook = NULL;
	//杈撳叆鐮佹湰瀵瑰簲鐨凥MM妯″瀷涓嬫爣
	unsigned int* pCodeIndex = NULL;
	
	//鍒ゆ柇杈撳叆鏁版嵁鏄鍚﹀悎娉
	if(pDataIn != NULL && this->m_pA != NULL &&this->m_pB != NULL && this->m_pPi != NULL)
	{
		pa = new double[this->m_nCodeNums * this->m_nStatusNums];
		pb = new double[this->m_nCodeNums * this->m_nStatusNums];

		//瀵硅緭鍏ラ噰鏍峰簭鍒楄繘琛岀煝閲忛噺鍖
		pCodeBook = new double[this->m_nCodeNums];
		CVQ::KMeansCluster(pDataIn, nInLen, pCodeBook, this->m_nCodeNums);

		//鑾峰緱杈撳叆鐮佹湰瀵瑰簲鐨凥MM妯″瀷鐮佹湰涓嬫爣
		pCodeIndex = new unsigned int [this->m_nCodeNums];
		CVQ::Classify (pCodeBook, this->m_nCodeNums, this->m_pCodeBook, this->m_nCodeNums,pCodeIndex);

		//璋冪敤鍓嶅悗鍚戠畻娉曪紝璁＄畻pa,pb
		fRate = CHMM::ForwardBackward (pCodeIndex, this->m_nCodeNums ,
			this->m_pPi, this->m_pA, this->m_pB, this->m_nStatusNums, pa, pb);
        
		//璋冪敤BW绠楁硶杩涜屽弬鏁颁紭鍖
		CHMM::BaumWelch (pCodeIndex, this->m_nCodeNums ,
			this->m_pPi, this->m_pA, this->m_pB, this->m_nStatusNums, pa, pb, fRate);

		//鏇存柊鐮佹湰
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
// 瀹炵幇瀵笻MM妯″瀷寰楀垵濮嬪寲璁缁
// 
// 鍒涘缓浜:		 
// 鍒涘缓鏃ユ湡:	 
// 淇鏀逛汉:
// 淇鏀规棩鏈:
void CHMM::PrepareTrain (
		QString strWord,            //妯″瀷瀵瑰簲璇
		const double*  pDataIn,     //绗涓涓杈撳叆閲囨牱搴忓垪
		unsigned int   nInLen,      //杈撳叆閲囨牱搴忓垪闀垮害
		unsigned int   nFrameSize,  //鍒嗗抚瀹藉害
		unsigned int   nStatusNums, //HMM鐘舵佹暟
		unsigned int   nCodeNums    //杈撳叆鐮佹湰闀垮害
		)
{
	//璁＄畻姒傜巼
	double fRate = 0;
	//寰鐜鍙橀噺
	unsigned int i = 0, j =0;

	//璁剧疆妯″瀷淇℃伅
	this->m_strWord = strWord;
	this->m_nStatusNums = nStatusNums;
	this->m_nCodeNums = nCodeNums;
	this->m_nFrameSize = nFrameSize;

	//鍒濆嬪寲pI/pA/pB/pCodeBook
	this->m_pPi = new double[this->m_nStatusNums];
	this->m_pA = new double[this->m_nStatusNums * this->m_nStatusNums];
	this->m_pB = new double[this->m_nStatusNums * this->m_nCodeNums];
	this->m_pCodeBook = new double[this->m_nCodeNums];

	memset(this->m_pPi, 0, sizeof(double) * this->m_nStatusNums);
	memset(this->m_pA, 0, sizeof(double) * this->m_nStatusNums * this->m_nStatusNums);
	memset(this->m_pB, 0, sizeof(double) * this->m_nStatusNums * this->m_nCodeNums);
	memset(this->m_pCodeBook, 0, sizeof(double) * this->m_nCodeNums);

	//瀵筽I杩涜屽潎鍒
	fRate = (double) 1 / nStatusNums;
	for(i=0; i < nStatusNums; i++)
	{
		this->m_pPi[i] = fRate;
	}
	//瀵筽A[i][j]杩涜屽潎鍒
	for(i=0;i < nStatusNums; i++)
	{
		for(j=0; j < nStatusNums; j++)
			this->m_pA[i * nStatusNums + j] = fRate;
	}
	//瀵筽B[i][j]杩涜屽潎鍒
	fRate = (double) 1 / nCodeNums;
	for(i=0;i < nCodeNums; i++)
	{
		for(j=0; j < nCodeNums; j++)
			this->m_pA[i * nCodeNums + j] = fRate;
	}
	//褰㈡垚妯℃澘鐮佹湰
	CVQ::KMeansCluster (pDataIn, nInLen, this->m_pCodeBook, this->m_nCodeNums);

	////杩涜屽弬鏁颁紭鍖
	//this->Train(pDataIn, nInLen);

}
//////////////////////////////////////////////////////////////////////////
// 瀹炵幇鍓嶅悗鍚戠畻娉,瀵逛簬缁欏畾鐨凥MM鍙傛暟鍜岃傚療搴忓垪O锛
// 姹傝兘澶熶骇鐢熻傚療搴忓垪O鐨勬傜巼
// 
// 鍒涘缓浜:		
// 鍒涘缓鏃ユ湡:	
// 淇鏀逛汉:
// 淇鏀规棩鏈:
double CHMM::ForwardBackward (
		const unsigned int * pCodeBook,//杈撳叆瑙傚療鐮佹湰搴忓垪瀵瑰簲鐨凥MM鐮佹湰鐨勪笅鏍
		unsigned int nCodeNums,        //杈撳叆瑙傚療搴忓垪闀垮害
		const double* pPi,             //HMM鐨刾i鐭㈤噺
		const double* pA,              //HMM鐨凙鐭╅樀
		const double* pB,              //HMM鐨凚鐭╅樀
		unsigned int nStatusNums,      //HMM鐘舵佹暟
		double* pa,                    //鍓嶅悗鍚戠畻娉曚腑鐨刟
		double* pb                     //鍓嶅悗鍚戠畻娉曚腑鐨刡
							 )
{
	//鑳藉熶骇鐢熻傚療搴忓垪O(澶у啓鐨刼)鐨勬傜巼
	double fRate=0;
	//閫掓帹鏃跺仛绱绉
	double fSum=0;
	
	int t=0;
	int i=0;
	int j=0;

	//鍒濆嬪寲pa,pb
	memset(pa,0,sizeof(double)*(nCodeNums*nStatusNums));
	memset(pb,0,sizeof(double)*(nCodeNums*nStatusNums));

	//瀹炵幇鍓嶅悗鍚戠畻娉
/*********************************************************************************************闊╃邯搴嗙増鏈涔P204*/
	//鍒濆嬪寲
	for(i=0;i<nStatusNums;i++)
	{
		//璁＄畻鍏寮忥細pa[1][i]=pI[i]*pB[i][o1]   1<=i<=N   
		pa[0*nStatusNums+i]=pPi[i]*pB[i*nCodeNums+pCodeBook[0]];

		//璁＄畻鍏寮忥細pb[nCodeNums-1][i]=1;
		pb[(nCodeNums-1)*nStatusNums+i]=1;
	}
///********************************************************************************************************璧靛姏鐗*/
////鍒濆嬪寲鍒濆嬬姸鎬丼1鐨勬暟缁勫彉閲
//	pa[0*nStatusNums+0]=1;//pa[0][1]=1;
//	pb[(nCodeNums-1)*nStatusNums+nStatusNums-1]=1;//pb[T][N]=1
//	for(j=1;j<nStatusNums;j++)
//	{
//		//璁＄畻鍏寮忥細pa[0][j]=0  2<=j<=N   
//		pa[0*nStatusNums+j]=0;
//
//		//璁＄畻鍏寮忥細pb[nCodeNums-1][j]=0;j!=N
//		pb[(nCodeNums-1)*nStatusNums+j-1]=0;
//	}

	/****************************************************************************************/
	//鍓嶅悜閫掓帹
	for(t=1;t<nCodeNums;t++)
	{
		for(j=0;j<nStatusNums;j++)
		{
			fSum=0;

			for(i=0;i<nStatusNums;i++)
			{
				//绱绉 fSum+=pa[t-1][i]*pA[i][j]
				fSum+=pa[(t-1)*nStatusNums+i]*pA[i*nStatusNums+j];
			}
			//璁＄畻鍏寮忥細pa[t][j]=fSum*pB[i][ot]
			pa[t * nStatusNums + j] = fSum * pB[j * nCodeNums + pCodeBook[t]];
		}
	}

	//鍚庡悜閫掓帹
	for(t=nCodeNums-2;t>=0;t--)
	{
		for(i=0;i<nStatusNums;i++)
		{
			fSum=0;
			
			for(j=0;j<nStatusNums;j++)
			{
				//绱绉锛歠Sum+=pA[i][j]*pB[j][t+1]*pb[t+1][j]
				fSum+=pA[i*nStatusNums+j]*pB[j*nCodeNums+pCodeBook[t+1]]*pb[(t+1)*nStatusNums+j];
			}
			pb[t * nStatusNums + i] = fSum;
		}
	}
	//缁撴潫
	//璁＄畻姒傜巼锛屼护 t=nCodeNums-1
	t=nCodeNums-1;
	for(i=0;i<nStatusNums;i++)
	{
		//璁＄畻鍏寮 fRate+=pa[t][i]*pb[t][i];
		fRate+=pa[t*nStatusNums+i]*pb[t*nStatusNums+i];//姝ゆ椂pb[t*nStatusNums+i]=1;
	}
	return fRate;
}
//////////////////////////////////////////////////////////////////////////
// 瀹炵幇Viterbi绠楁硶,瀵逛簬缁欏畾鐨凥MM鍙傛暟鍜岃傚療搴忓垪O锛
// 杩斿洖鏈澶ф傜巼
// 
// 鍒涘缓浜:		
// 鍒涘缓鏃ユ湡:	
// 淇鏀逛汉:
// 淇鏀规棩鏈:
double CHMM::Viterbi(
		const unsigned int* pCodeBook, //杈撳叆瑙傚療鐮佹湰搴忓垪瀵瑰簲鐨凥MM鐮佹湰鐨勪笅鏍
		unsigned int nCodeNums,        //杈撳叆瑙傚療搴忓垪闀垮害
		const double* pPi,             //HMM鐨刾i鐭㈤噺
		const double* pA,              //HMM鐨凙鐭╅樀
		const double* pB,              //HMM鐨凚鐭╅樀
		unsigned int nStatusNums       //HMM鐘舵佹暟
		)
{
	//pa[t][i]涓簍鏃跺埢娌跨潃涓鏉¤矾寰剄1,q2,....,qt,涓攓t=st
	//浜х敓鍑鸿緭鍏ヨ傚療搴忓垪o1,o2,o3,.....,ot鐨勬渶澶ф傜巼
	//1 <= t <= nInLen,1 <= i <= nStatuesNums
	double* pa=NULL;
    double* pb=NULL;//鐢ㄤ簬淇濆瓨姣忎竴娆′娇pa(j)鏈澶х殑鐘舵乮
	//涓存椂璁＄畻
	double fTemp=0;
	double fMax=0;

	//鏈澶ф傜巼
	double fMaxRate=0;

	//寰鐜鍙橀噺
	//閲囨牱搴忓垪寰鐜
	unsigned int t=0;
	//鐘舵佸惊鐜
	unsigned int i=0,j=0;

	//鍒嗛厤鍜屽垵濮嬪寲pa
	pa=new double[nCodeNums*nStatusNums];
	memset(pa,0,sizeof(double)*(nCodeNums * nStatusNums));
    pb=new double[nCodeNums*nStatusNums];
	memset(pb,0,sizeof(double)*(nCodeNums * nStatusNums));
	//鍒濆嬪寲
	for(i=0;i< nStatusNums;i++)
	{
		//璁＄畻鍏寮忥細pa[1][i]=pi[i]*pB[i][1]
	    pa[0 * nStatusNums + i] = pPi[i] * pB[i * nCodeNums + pCodeBook[0]];
		pb[0 * nStatusNums + i]=0;
	 }
	//閫掓帹
	for(t=1;t< nCodeNums;i++)
	{	for(j=0;j< nStatusNums;j++)
		{
			//姹俻a[t-1][i]*pA[i][j]鐨勬渶澶у硷紝1 <= i <= N
			fMax=pa[(t-1)*nStatusNums+0]*pA[0*nStatusNums+j];

			for(i=0;i< nStatusNums;i++)
			{
				fTemp=pa[(t-1)* nStatusNums + i]*pA[0 * nStatusNums + j];
				fMax=( fMax < fTemp ? fTemp : fMax );
			}

			//璁＄畻pa[t][j]鍜宲b[t][j]
			//璁＄畻鍏寮忥細 pa[t][j]=fMax * pB[j][t];
			pa[t* nStatusNums + j]=fMax * pB[j * nCodeNums +pCodeBook[t]];
    	}	  
	}
	//缁堢粨
	//姹傛渶澶ф傜巼,fMaxRate=max(pa[nCodeNums-1][i])  1 <= i <=nStatusNums
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
// 瀹炵幇Baum Welch绠楁硶,瀵逛簬缁欏畾鐨勮傚療搴忓垪O锛
// 姹備娇P(O | HMM鍙傛暟)鏈澶х殑HMM鍙傛暟pi, A, B
// 
// 鍒涘缓浜:	
// 鍒涘缓鏃ユ湡:	
void CHMM::BaumWelch(
		const unsigned int* pCodeBook,	// 杈撳叆瑙傚療鐮佹湰搴忓垪瀵瑰簲鐨凥MM鐮佹湰鐨勪笅鏍
		unsigned int nCodeNums,			// 杈撳叆瑙傚療搴忓垪闀垮害
		double* pPi,					// HMM鐨刾i鐭㈤噺
		double* pA,						// HMM鐨凙鐭╅樀
		double* pB,						// HMM鐨凚鐭╅樀
		unsigned int nStatusNums,		// HMM鐘舵佹暟
		double* pa,						// 鍓嶅悗鍚戠畻娉曡＄畻鎵寰楃殑pa
		double* pb,						// 鍓嶅悗鍚戠畻娉曡＄畻鎵寰梡b
		double fRate					// 鍓嶅悗鍚戠畻娉曡＄畻鎵寰楁渶澶ф傜巼
		)
{
	//bw绠楁硶涓姒傜巼鐭╅樀
	//pRate[nCodeNums][nStatusNums][nStatusNums]
	double * pRate=NULL;
	//鏈灏忔傜巼
	double fMinRate=0.0001;
	unsigned int nOffSet = nStatusNums * nStatusNums;

	//寰鐜鍙橀噺
	unsigned int t = 0;
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int k = 0;

	//姹傛傜巼鏃剁殑鍒嗗瓙
	double fAvgUp = 0;
	//姹傛傜巼鏃剁殑鍒嗘瘝
	double fAvgDown = 0;
	//鏁扮粍鍏冪礌涓嬫爣
	unsigned int nIndex = 0;

	//鍒濆嬪寲姒傜巼鐭╅樀
	pRate = new double [nCodeNums * nOffSet];
	memset(pRate ,0 ,sizeof(double) * nCodeNums *nOffSet);

	//璁＄畻pRate[t][i][j],鍗硉鏃跺埢Markov閾惧勪簬i鐘舵侊紝t+1鏃跺埢澶勪簬j鐘舵佺殑姒傜巼
	for(t=0; t < (nCodeNums -1); t++)
	{
		for( i=0; i < nStatusNums; i++)
		{
			for(j=0; j < nStatusNums; j++)
			{
				//璁＄畻鍏寮忥細 pRate[t][i][j]=pa[t][i]*pA[i][j]*pB[j][O(t+1)]*pb[t+1][j]/fRate;
				pRate[t*nOffSet + i * nStatusNums + j]=
					(pa[t * nStatusNums + i] * pA[i * nStatusNums + j] * 
					pB[j * nCodeNums + pCodeBook[t+1]] * pb[(t+1) * nStatusNums + j])/fRate;
			}
		}
	}
	//浣跨敤閲嶄及鍏寮忚＄畻HMM鍙傛暟
	//璁＄畻Pi鐭㈤噺锛屽嵆0鏃跺埢鏃禡arkov閾惧勪簬i鐘舵侊紝t+1鏃跺埢澶勪簬鍏朵粬浠讳綍鐘舵佺殑姒傜巼
	for( i=0; i < nStatusNums; i++)
	{
		pPi[i]=0;

		for(j=0; j< nStatusNums; j++)
		{
			pPi[i] += pRate[0 * nOffSet + i * nStatusNums + j];
		}
	}

	//璁＄畻A鐭╅樀
	//pA[i][j] =(浠庣姸鎬丼i杩囨浮鍒癝j鐨勫钩鍧囨℃暟) / (浠庣姸鎬丼i鍚戝叾浠栫姸鎬佽浆绉荤殑骞冲潎娆℃暟)
	for(i=0; i < nStatusNums; i++)
	{
		for(j=0; j < nStatusNums; j++)
		{
			 //璁＄畻鐘舵丼i杩囨浮鍒癝j鐨勫钩鍧囨℃暟
			fAvgUp = 0;
			//浠庣姸鎬丼i鍚戝叾浠栫姸鎬佽浆绉荤殑骞冲潎娆℃暟
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

	//璁＄畻B鐭╅樀
	// pB[i][k]= (鍑虹幇鐘舵乮鍜岃傚療鍊糘t=Vk鐨勫钩鍧囨℃暟) / (澶勪簬鐘舵乮鐨勬℃暟)
    for(i=0; i < nStatusNums; i++)
	{
		for(k=0; k < nCodeNums; k++)
		{
			//璁＄畻鍑虹幇鐘舵乮鍜岃傚療鍊糘t=Vk鐨勫钩鍧囨℃暟
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
			//澶勪簬鐘舵乮鐨勬℃暟
			fAvgDown=0;
			for(t=0; t < nCodeNums; t++)
			{
				for(j=0; j < nStatusNums; j++)
				{
					fAvgDown += pRate[t * nOffSet + i * nStatusNums + j];
				}
			}
			//璁＄畻pB[i][k]
			nIndex = i * nCodeNums +pCodeBook[k];
			pB[nIndex]= fAvgUp / fAvgDown;
			pB[nIndex]= (pB[nIndex] > 0.0 ? pB[nIndex] : fMinRate);
		}
	}
    // 閲婃斁璧勬簮
	if (pRate != NULL)
	{
		delete[] pRate;
	} 
}