#include "MFCStub.h"
#ifndef  _CVQ_H_
#define  _CVQ_H_
#pragma once
//////////////////////////////////////////////////////////////////////////
// 閲囩敤鏀硅繘LBG绠楁硶锛屽疄鐜拌緭鍏ュ簭鍒楃殑鐭㈤噺閲忓寲
// 
// 鍒涘缓浜:		
// 鍒涘缓鏃ユ湡:	
// 淇鏀逛汉:
// 淇鏀规棩鏈:

// 榛樿ょ殑鏈澶ц凯浠ｆ℃暟
#define CVQ_MAX_REPEAT
// 鏃犵┓澶
#define CVQ_MAXIMUN	50000

class CVQ
{
public:
	CVQ(void);
	~CVQ(void);
public:
	//閲囩敤LBG绠楁硶杩涜孷Q
	static void LBG(
		const double * pInVector,        //杈撳叆鏍锋湰搴忓垪
		unsigned int   nInLen,           //杈撳叆鏍锋湰搴忓垪闀垮害
		double       * pOutCodeBook,     //杈撳嚭鐮佹湰
		unsigned int   nCodeNums,        //鐮佹湰闀垮害
		unsigned int   nMaxReapt,        //鏈澶ц凯浠ｆ℃暟
		double         fMinChange,       //鐣稿彉鏀硅繘闃堝
		double         fInitDistortion = CVQ_MAXIMUN  //鍒濆嬬暩鍙
		);
	//杩涜孡BG绠楁硶杩浠ｏ紝杈撳嚭鎬荤暩鍙
	static double LBGRepeat(
		const double * pInVector,        //杈撳叆鏍锋湰搴忓垪
		unsigned int   nInLen,           //杈撳叆鏍锋湰搴忓垪闀垮害
	  double * pCodeBook,              //杈撳叆/鍑虹爜鏈
		unsigned int   nCodeNums         //鐮佹湰闀垮害
		);
	//瀵硅緭鍏ユ牱鏈杩涜岀畝鍗曡仛绫
	static void EasyCluster(
		const double * pInVector,        //杈撳叆鏍锋湰搴忓垪
		unsigned int   nInLen,           //杈撳叆鏍锋湰搴忓垪闀垮害
	    double * pCodeBook,              //杈撳叆/鍑虹爜鏈
		unsigned int   nCodeNums         //鐮佹湰闀垮害
		);
	//瀹炵幇K鍧囧艰仛绫
	static void KMeansCluster(
		const double * pInVector,        //杈撳叆鏍锋湰搴忓垪
		unsigned int   nInLen,           //杈撳叆鏍锋湰搴忓垪闀垮害
	    double * pCodeBook,              //杈撳叆/鍑虹爜鏈
		unsigned int   nCodeNums         //鐮佹湰闀垮害
		);
	//瀵硅緭鍏ョ爜鏈锛屾寜鐓ф爣鍑嗙爜鏈杩涜屽垎绫
	static void Classify(
		const double * pInCodeBook,  //杈撳叆鐮佹湰
		unsigned int   nInNums,      //杈撳叆鐮佹湰涓鐮佸瓧鏁伴噺
		const double * pCodeBook,    //妯℃澘鐮佹湰
		unsigned int   nCodeNums,    //妯℃澘鐮佹湰涓鐮佸瓧鏁伴噺
		unsigned int * pKinds        //杈撳叆鐮佹湰涓鐨勭爜瀛楀瑰簲鐨勬ā鏉跨爜鏈涓鐮佸瓧鐨勪笅鏍
		);
	//璁＄畻鐮佹湰娆у紡璺濈
	static double GetDistance(
		const double* pCode1,
		const double* pCode2,
		unsigned int  nCodeNums
		);
private:
	//瀹炵幇K鍧囧艰仛绫昏凯浠ｏ紝杈撳嚭鎬荤暩鍙
	static double KMeansClusterRepeat(
		const double* pInVector,      //杈撳叆鏍锋湰搴忓垪
		unsigned int  nInLen,         //杈撳叆鏍锋湰搴忓垪闀垮害
		double*       pCodeBook,      //杈撳叆鐮佹湰
		unsigned int  nCodeNums,      //鐮佹湰闀垮害
		unsigned int* pFlag,          //鐢ㄤ簬鍒嗙被鐨勬暟缁勶紝鍐岾MeansCluster涓鍒嗛厤
		double*       pNewCodeBook    //杈撳嚭鏂扮爜鏈
		);
};

#endif 