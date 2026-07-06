#include "MFCStub.h"
#include <QString>
#include <QVector>
#include"VQ.h"
#include<math.h>

//////////////////////////////////////////////////////////////////////////
// 鏋勯犲嚱鏁
// 
// 鍒涘缓浜:		 
// 鍒涘缓鏃ユ湡:	 
// 淇鏀逛汉:
// 淇鏀规棩鏈:
CVQ::CVQ(void)
{
}

//////////////////////////////////////////////////////////////////////////
// 鏋愰犲嚱鏁
// 
// 鍒涘缓浜:		 
// 鍒涘缓鏃ユ湡:	 
// 淇鏀逛汉:
// 淇鏀规棩鏈:
CVQ::~CVQ(void)
{
}
//////////////////////////////////////////////////////////////////////////
// 閲囩敤LBG绠楁硶杩涜孷Q
// 
// 鍒涘缓浜:		
// 鍒涘缓鏃ユ湡:	
// 淇鏀逛汉:
// 淇鏀规棩鏈:
void CVQ::LBG( 
			  const double* pInVector,		// 杈撳叆鏍锋湰搴忓垪
			  unsigned int nInLen,			// 杈撳叆鏍锋湰搴忓垪闀垮害
			  double* pOutCodeBook,			// 杈撳嚭鐮佹湰
			  unsigned int nCodeNums,		// 鐮佹湰闀垮害
			  unsigned int nMaxReapt,		// 鏈澶ц凯浠ｆ℃暟
			  double fMinChange,			// 鐣稿彉鏀硅繘闃堝
			  double fInitDistortion		// 鍒濆嬬暩鍙
			  )
{
	//褰撳墠鎬荤暩鍙
	double fCurDistortion = fInitDistortion;
	//璋冪敤LBGRepeat杩斿洖鐨勬荤暩鍙
	double fRetDistortion = 0;
	//褰撳墠杩浠ｆ℃暟
	unsigned int nCurRepeat = 0;
	//褰撳墠鐣稿彉鏀硅繘閲
	double fCurChange = fInitDistortion;
	
	//鑻ュ垵濮嬬爜鏈涓虹┖锛屽垯鐢ㄨ緭鍏ュ簭鍒楃殑鍓峮CodeNums涓鏍锋湰涓哄垵濮嬬爜鏈
	if( pOutCodeBook == NULL)
	{
		memcpy(pOutCodeBook, pInVector, sizeof(double) * nCodeNums);
	}

	//璋冪敤LBGRepeat杩涜岃凯浠
	while(nCurRepeat < nMaxReapt && fMinChange < fCurChange)
	{
		fRetDistortion = CVQ::LBGRepeat (pInVector, nInLen, pOutCodeBook, nCodeNums);

		//璁＄畻褰撳墠鐣稿彉鏀硅繘閲
		fCurChange = abs(fCurDistortion - fRetDistortion) / fCurDistortion;
		fCurDistortion = fRetDistortion;
	}

}
//////////////////////////////////////////////////////////////////////////
// 杩涜孡BG绠楁硶杩浠ｏ紝杈撳嚭鎬荤暩鍙
// 
// 鍒涘缓浜:		 
// 鍒涘缓鏃ユ湡:	 
// 淇鏀逛汉:
// 淇鏀规棩鏈:
double CVQ::LBGRepeat( 
					const double* pInVector,		// 杈撳叆鏍锋湰搴忓垪
					unsigned int nInLen,			// 杈撳叆鏍锋湰搴忓垪闀垮害
					double* pCodeBook,				// 杈撳叆/鍑虹爜鏈
					unsigned int nCodeNums			// 鐮佹湰闀垮害
					)
{
	//鑱氱被鏄姣忎釜鐮佺煝浠ｈ〃鐨勬牱鏈鏁帮紝鐩稿綋浜嶮atlab鐗堜腑鐨凬1
	unsigned int *pnSampleNums=new unsigned int[nCodeNums];
	//鑱氱被鏄杩涜屾牱鏈鍙犲姞鍊硷紝鐩稿綋浜嶮ATLAB鐗堜腑鐨剏(m)
	double* pfSampleSums=new double[nCodeNums];
	//淇濆瓨鎬荤暩鍙
	double fDistortion = 0;
	//淇濆瓨鐩搁偦鐨勭爜鐭涓嬫爣
	unsigned int nNearCode = 0;
	//淇濆瓨鏍锋湰涓庣爜鐭㈣窛绂
	double fDistance = 0;
	//寰鐜鍙橀噺
	unsigned int i= 0, j =0;

	//鍒濆嬪寲鑱氱被鏃舵瘡涓鐮佺煝浠ｈ〃鐨勬牱鏈鏁
	memset(pnSampleNums, 0, sizeof(unsigned int) * nCodeNums);
	//鍒濆嬪寲鑱氱被鏃惰繘琛屾牱鏈鍙犲姞鍊
	memset(pfSampleSums, 0,sizeof(double) * nCodeNums);

	//鎸夌収杈撳叆鐮佹湰瀵硅緭鍏ユ牱鏈搴忓垪杩涜屽垎娈
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
		//璁＄畻鎬荤暩鍙
		fDistortion+=fDistance;
		pfSampleSums[nNearCode] +=pInVector[i];
		pnSampleNums[nNearCode]++;
	}
	//璁＄畻鏂扮爜鏈
	for(i=0;i < nCodeNums; i++)
	{
		pCodeBook[i]= pfSampleSums[i] / pnSampleNums[i];
	}
	delete [] pnSampleNums;
	delete [] pfSampleSums;

	return fDistortion;
}
//////////////////////////////////////////////////////////////////////////
// 瀵硅緭鍏ユ牱鏈杩涜岀畝鍗曡仛绫
// 鍏堝硅緭鍏ユ牱鏈杩涜屽垎娈碉紝鐒跺悗绠鍗曠殑姹傛瘡涓鍒嗘电殑鍧囧间綔涓虹爜鏈
// 浜х敓鐨勭爜鏈涓庢椂闂磘鐩稿叧
//
// 鍒涘缓浜:		
// 鍒涘缓鏃ユ湡:	
// 淇鏀逛汉:
// 淇鏀规棩鏈:
void CVQ::EasyCluster (
		const double * pInVector,        //杈撳叆鏍锋湰搴忓垪
		unsigned int   nInLen,           //杈撳叆鏍锋湰搴忓垪闀垮害
	    double * pCodeBook,              //杈撳叆/鍑虹爜鏈
		unsigned int   nCodeNums         //鐮佹湰闀垮害
					   )
{
	unsigned int nSegLen = nInLen / nCodeNums;
	unsigned int nOffSet = 0;

	//寰鐜鍙橀噺
	unsigned int i=0,j=0;

	//鍒濆嬪寲鐮佹湰
	memset(pCodeBook, 0, sizeof(double) * nCodeNums);

    for(i=0; i < nCodeNums; i++)
	{
		nOffSet = i * nSegLen;

		for(j=0; j < nSegLen; j++)
			pCodeBook[i] += pInVector[nOffSet + j];
		
		//瀵圭爜鐭㈡眰鍧囧
		pCodeBook[i] = pCodeBook[i] /  nSegLen;
	}
}
//////////////////////////////////////////////////////////////////////////
// 瀹炵幇K鍧囧艰仛绫
//
// 鍒涘缓浜:		 
// 鍒涘缓鏃ユ湡:	 
// 淇鏀逛汉:
// 淇鏀规棩鏈:
void CVQ::KMeansCluster (
	    const double * pInVector,        //杈撳叆鏍锋湰搴忓垪
		unsigned int   nInLen,           //杈撳叆鏍锋湰搴忓垪闀垮害
	    double * pCodeBook,              //杈撳叆/鍑虹爜鏈
		unsigned int   nCodeNums         //鐮佹湰闀垮害
						 )
{
	//pInVector(i)鏄鍚﹀睘浜巔CodeBook(j)
	unsigned int * pFlag = NULL;
	//杩浠ｄ腑浣跨敤鍒扮殑鐮佹湰
	double * pNewCodeBook = NULL;

	//寰鐜鍙橀噺
	unsigned int i=0,j=0;

	//杩浠ｈ繑鍥炵殑鐣稿彉
	double fDistortion = 0;

	//瀵圭爜鏈杩涜屽崌搴忔帓搴
	double fMinCode = 0;
	unsigned int nMinIndex = 0;

	if(pInVector != NULL && pCodeBook != NULL)
	{
		//鍒嗛厤鍖栨爣蹇楃煩闃
		pFlag = new unsigned int [nCodeNums * nInLen];
		//鍒嗛厤杩浠ｇ爜鏈
		pNewCodeBook = new double [nCodeNums];

		//浣跨敤EasyCluster璁惧畾鍒濆嬬爜鏈
		CVQ::EasyCluster (pInVector, nInLen, pCodeBook, nCodeNums);
	

	    //杩浠ｈ仛绫
	    fDistortion = 1;
	    while(fDistortion > 0)
	    {
	         fDistortion = CVQ::KMeansClusterRepeat (
		  	     pInVector, nInLen, pCodeBook, nCodeNums, pFlag, pNewCodeBook);

		     //鎷疯礉鏂扮爜鏈
		     memcpy(pCodeBook, pNewCodeBook, sizeof(double) * nCodeNums);
	     }

	     //瀵圭爜鏈杩涜屽崌搴忔帓鍒
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
        //閲婃斁鎵鍗犺祫婧
		delete [] pNewCodeBook;
		delete [] pFlag;
	}
}
//////////////////////////////////////////////////////////////////////////
// 瀹炵幇K鍧囧艰仛绫荤殑杩浠ｈ繃绋嬶紝 杈撳嚭鐮佷功鐨勭暩鍙樼▼搴
//
// 鍒涘缓浜:		 
// 鍒涘缓鏃ユ湡:	 
// 淇鏀逛汉:
// 淇鏀规棩鏈:
double CVQ::KMeansClusterRepeat (
		const double* pInVector,      //杈撳叆鏍锋湰搴忓垪
		unsigned int  nInLen,         //杈撳叆鏍锋湰搴忓垪闀垮害
		double*       pCodeBook,      //杈撳叆鐮佹湰
		unsigned int  nCodeNums,      //鐮佹湰闀垮害
		unsigned int* pFlag,          //鐢ㄤ簬鍒嗙被鐨勬暟缁勶紝鍐岾MeansCluster涓鍒嗛厤
		double*       pNewCodeBook    //杈撳嚭鏂扮爜鏈
		 )
{
	//鐣稿彉
	double fDistortion = 0;
	//鏍锋湰鍜岀爜鏈鐨勬渶灏忚窛绂
	double fMinDis = 0;
	//璁＄畻鏍锋湰鍜岀爜鏈鐨勮窛绂
	double fDis = 0;
	//鏈灏忚窛绂诲瑰簲鐨勭爜鏈涓嬫爣
	unsigned int nIndex = 0;
	 
	//鐮佹湰鍖呭惈鏍锋湰鏁
	unsigned int nSamplesCount = 0;
	//鐮佹湰涓鏍锋湰绱绉
	double fSamplesSum = 0;

	//寰鐜鍙橀噺
	unsigned int i =0, j = 0;

	if(pInVector != NULL)
	{
		//鍒濆嬪寲杈撳嚭鏂扮爜鏈
		memset(pNewCodeBook, 0, sizeof(double) * nCodeNums);
		//鍒濆嬪寲鏍囧織鐭╅樀
		memset(pFlag, 0, sizeof(unsigned int )* nCodeNums * nInLen);
		 
		//鑱氱被
		for( i=0; i< nInLen; i++)
		{
			//鍒濆嬫渶灏忓
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

			//璁惧畾pInVector[i]灞炰簬涓蹇僷CodeBook[nIndex]
			pFlag[nIndex * nInLen + i] =1;   
		}

		//璁＄畻鏂扮爜鏈
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

			//褰撳爢澶у皬涓0鏃讹紝瀵瑰簲鐮佺煝涓0
			pNewCodeBook[i] =(nSamplesCount == 0? 0: (fSamplesSum / nSamplesCount));
			fDistortion += abs(pCodeBook[i] - pNewCodeBook[i]);
		}
		//璁＄畻鐣稿彉
		fDistortion = fDistortion / nCodeNums;
	}
	return fDistortion;
}
//////////////////////////////////////////////////////////////////////////
// 瀵硅緭鍏ョ爜鏈锛屾寜鐓ф爣鍑嗙爜鏈杩涜屽垎绫
//
// 鍒涘缓浜:		
// 鍒涘缓鏃ユ湡:	
// 淇鏀逛汉:
// 淇鏀规棩鏈:
void CVQ::Classify(
				   const double *pInCodeBook,  //杈撳叆鐮佹湰
				   unsigned int nInNums,       //杈撳叆鐮佹湰涓鐮佸瓧鏁伴噺
				   const double *pCodeBook,    //妯℃澘鐮佹湰
				   unsigned int nCodeNums,     //妯℃澘鐮佹湰涓鐮佸瓧鏁伴噺
				   unsigned int *pKinds        //杈撳叆鐮佸瓧涓鐨勭爜瀛楀瑰簲鐨勬ā鏉跨爜鏈涓鐮佸瓧鐨勪笅鏍
				   ) 
{
	//鐮佹湰鐨勬渶灏忚窛绂
	double fMinDis = 0;
	//鏈灏忚窛绂诲瑰簲鐨勬ā鏉跨爜瀛楃殑涓嬫爣
	unsigned int nMinIndex = 0;
	//璁＄畻鐮佹湰璺濈
	double fDis = 0;

	//寰鐜鍙橀噺
	unsigned int i = 0, j=0;
	
	if(pInCodeBook != NULL && pCodeBook !=NULL)
	{
		//鍒濆嬪寲鐮佸瓧涓嬫爣
		memset(pKinds, 0, sizeof(unsigned int) * nInNums);

		for(i=0; i < nInNums; i++)
		{
			//鍒濆嬪寲鏈灏忚窛绂
			fMinDis = abs(pInCodeBook[i] - pCodeBook[0]);
			nMinIndex = 0;

			//璁＄畻鏈灏忚窛绂
			for( j=0; j < nCodeNums; j++)
			{
				fDis = abs(pInCodeBook[i] - pCodeBook[j]);
				if(fDis < fMinDis)
				{
					fMinDis = fDis;
					nMinIndex = j;
				}
			}
			//淇濆瓨鐮佸瓧涓嬫爣
			pKinds[i] = nMinIndex;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// 璁＄畻涓や釜鐮佹湰鐨勬у紡璺濈
double CVQ::GetDistance(const double *pCode1, const double *pCode2, unsigned int nCodeNums) 
{
	double fSum = 0;
	for(unsigned int i=0; i< nCodeNums; i++)
	{
		fSum += (pCode1[i]-pCode2[i]) * (pCode1[i]-pCode2[i]);
	}
	return abs(fSum);
}
