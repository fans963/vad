#include "MFCStub.h"
#pragma once 
/////////////////////////////////////////////////////
//瀹炵幇鍩轰簬VQ鐨凥MM妯″瀷
//
//鍒涘缓鏃ユ湡锛2012.10.23
//
//
//////////////////////////////////////////////////////
//榛樿ょ姸鎬佹暟
#define CHMM_STATUS_NUMS   6
//璇嗗埆瀛楅暱搴
#define CHMM_WORD_LIN     10
class CHMM
{
public:
	CHMM();
	~CHMM(void);
public:
	//瀵煎叆HMM妯″瀷淇℃伅
	bool LoadModel(QString strFileName);
	//瀵煎嚭HMM妯″瀷淇℃伅
	bool SaveModel(QString strFileName);
	//閲婃斁妯″瀷鍗犵敤璧勬簮
	void Dispose();
	//瀹炵幇瀵笻MM妯″瀷鐨勮凯浠ｈ缁
	void Train(
		const double * pDataIn,   //杈撳叆閲囨牱搴忓垪
		unsigned int nInLen       //杈撳叆閲囨牱搴忓垪闀垮害
		);
	//瀹炵幇瀵笻MM妯″瀷鐨勫垵濮嬪寲
	void PrepareTrain(
		QString strWord,            //妯″瀷瀵瑰凡璇
		const double*  pDataIn,     //绗涓涓杈撳叆閲囨牱搴忓垪
		unsigned int   nInLen,      //杈撳叆閲囨牱搴忓垪闀垮害
		unsigned int   nFrameSize,  //鍒嗗抚瀹藉害
		unsigned int   nStatusNums, //HMM鐘舵佹暟
		unsigned int   nCodeNums    //杈撳叆鐮佹湰闀垮害
		);
private:
	//瀹炵幇鍓嶅悗鍚戠畻娉曪紝骞惰緭鍑烘渶澶ф傜巼
	double ForwardBackward(
		const unsigned int * pCodeBook,//杈撳叆瑙傚療鐮佹湰搴忓垪瀵瑰簲鐨凥MM鐮佹湰鐨勪笅鏍
		unsigned int nCodeNums,        //杈撳叆瑙傚療搴忓垪闀垮害
		const double* pPi,             //HMM鐨刾i鐭㈤噺
		const double* pA,              //HMM鐨凙鐭╅樀
		const double* pB,              //HMM鐨凚鐭╅樀
		unsigned int nStatusNums,      //HMM鐘舵佹暟
		double* pa,                    //鍓嶅悗鍚戠畻娉曚腑鐨刟
		double* pb                     //鍓嶅悗鍚戠畻娉曚腑鐨刡
		);
	//瀹炵幇Viterbi绠楁硶
	double Viterbi(
		const unsigned int* pCodeBook, //杈撳叆瑙傚療鐮佹湰搴忓垪瀵瑰簲鐨凥MM鐮佹湰鐨勪笅鏍
		unsigned int nCodeNums,        //杈撳叆瑙傚療搴忓垪闀垮害
		const double* pPi,             //HMM鐨刾i鐭㈤噺
		const double* pA,              //HMM鐨凙鐭╅樀
		const double* pB,              //HMM鐨凚鐭╅樀
		unsigned int nStatusNums       //HMM鐘舵佹暟
		);
	//瀹炵幇Baum-Welch绠楁硶
	void BaumWelch(
		const unsigned int* pCodeBook,	// 杈撳叆瑙傚療鐮佹湰搴忓垪瀵瑰簲鐨凥MM鐮佹湰鐨勪笅鏍
		unsigned int nCodeNums,			// 杈撳叆瑙傚療搴忓垪闀垮害
		double* pPi,					// HMM鐨刾i鐭㈤噺
		double* pA,						// HMM鐨凙鐭╅樀
		double* pB,						// HMM鐨凚鐭╅樀
		unsigned int nStatusNums,		// HMM鐘舵佹暟
		double* pa,						// 鍓嶅悗鍚戠畻娉曡＄畻鎵寰楃殑pa
		double* pb,						// 鍓嶅悗鍚戠畻娉曡＄畻鎵寰梡b
		double fRate					// 鍓嶅悗鍚戠畻娉曡＄畻鎵寰楁渶澶ф傜巼
		);

public:
	    double* m_pPi;                 //鍒濆嬬姸鎬佸垎甯冨簭鍒,t=1鏃堕棿鏃秖1 =Si鐨勬傜巼pi
		double* m_pA;                  //鐘舵佽浆绉绘傜巼鐭╅樀锛岀敱q(i-1)涓簊(k)杞绉诲埌q(i)涓簊(h)鐨勬傜巼
		double* m_pB;                  //瑙傚療鐭㈤噺澶勪簬鏌愮姸鎬佺殑姒傜巼
		double* m_pCodeBook;           //鐮佹湰

		unsigned int m_nStatusNums;    //HMM鐘舵佹暟閲
		unsigned int m_nCodeNums;      //杈撳叆鐮佹湰闀垮害
		QString      m_strWord;        //妯″瀷瀵瑰簲鐨勮瘝
		unsigned int m_nFrameSize;     //姹傚掕氨鏃舵墍杩涜岀殑鐭鏃跺揩閫烣FT鐨勭煭鏃剁偣鏁
};