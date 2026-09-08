#ifndef _CLIB_HEADER
#define _CLIB_HEADER
//头文件一共58个双字节(32位)
//WAVE文件RIFF数据块 
struct RIFF_HEADER
{
	//char szRiffID[4];  // 'R','I','F','F'
	//DWORD szRiffID;
	DWORD dwRiffSize;
	//char szRiffFormat[4]; // 'W','A','V','E'
	//DWORD szRiffFormat;
};

//WAVE文件FMT数据块
struct WAVE_FORMAT
{
	WORD wFormatTag;		//编码方式，一般为0x0001
	WORD wChannels;			//声道数 1或2
	DWORD dwSamplesPerSec;	//采样频率
	DWORD dwAvgBytesPerSec;	//每秒所需字节数
	WORD wBlockAlign;		//数据块对齐单位(每个采样需要的字节数)
	WORD wBitsPerSample;	//每个采样需要的bit数 
};
struct FMT_BLOCK
{
	//char  szFmtID[4];	// 'f','m','t',' '
	//DWORD szFmtID;
	DWORD dwFmtSize;	//一般情况下为16，此时最后附加信息没有；如果为18则最后多了2个字节的附加信息
	WAVE_FORMAT wavFormat;
};

//WAVE文件FACT数据块
struct FACT_BLOCK
{
	//char  szFactID[4]; // 'f','a','c','t'
	//DWORD szFactID;
	DWORD dwFactSize;
	DWORD dwFactData;
};

//WAVE文件DATA数据块
struct DATA_BLOCK
{
	//char szDataID[4]; // 'd','a','t','a'
	//DWORD szDataID;
	DWORD dwDataSize;
};

//文件头
struct WAVEFILEHEADER
{
	RIFF_HEADER riff;
	FMT_BLOCK	fmt;
	FACT_BLOCK fact;
	DATA_BLOCK data;
};
#endif
