// MyWaveDoc.h : CMyWaveDoc 类的接口
//


#pragma once
#include<vector>
#define ID_RIFF	mmioFOURCC('R', 'I', 'F', 'F')
#define ID_WAVE	mmioFOURCC('W', 'A', 'V', 'E')
#define ID_fmt	mmioFOURCC('f', 'm', 't', ' ')
#define ID_fact	mmioFOURCC('f', 'a', 'c', 't')
#define ID_PAD	mmioFOURCC('P', 'A', 'D', ' ')
#define ID_data	mmioFOURCC('d', 'a', 't', 'a')


class CMyWaveDoc : public CDocument
{
public: // 仅从序列化创建
	CMyWaveDoc();
	DECLARE_DYNCREATE(CMyWaveDoc)

	// 属性
public:

	int frame_len;
	CString m_tempFileName;
	CString m_tempFilePath;
	std::vector <float>data;
	//第一个文件的数据
	WAVEFILEHEADER header;
	CString m_strFileName;
	CString m_strFilePath;
	std::vector <short>data_left;
	std::vector <short>data_right;
	std::vector <short>data_one;
	int m_channels;           //声道数
	int m_HZ;                 //采样频率
	int m_time;               //文件持续的时间
	int m_bytesPerSample;     //每个样本的字节数（1或2）
	int m_extent;             //波形的最大幅度 16位则为32768
	int m_numOfPoint;         //总数据数
	int maxForOneData;//单声道 用于平移时记录振幅的最大值
	int minForOneData;//单声道 用于平移时记录振幅的最小值
	int maxForLeftData;//双声道 用于记录左声道的振幅最大值
	int minForLeftData;//双声道 用于记录左声道的振幅最小值
	int maxForRightData;//双声道 用于记录右声道的振幅最大值
	int minForRightData;//双声道 用于记录右声道的振幅最小值
	////////////////////////////////////////////////////////////////////////新加的
	double * lpc;  //线性预测系数
	double *lpcc;  //线性预测倒谱
	double *lpccmcc;//线性预测mel倒谱
	double *mfcc;   //mel倒谱
	double *melspectrum;//mel频谱

	//倒谱数据
	std::vector<std::vector<float> > ifft_one;
	float maxForOneIfft;
	float minForOneIfft;
	std::vector<std::vector<float> > ifft_pitch;
	//基于倒谱的端点检测数据
	std::vector<std::vector<int>> id_max;//最大峰值能力团的下标id
	std::vector<std::vector<int>> id_last;//最大峰值能力团的下标id
	std::vector<int> f1;//最大值序号
	std::vector<float> p1;//最大值的倒谱值
	std::vector<int> f2;//次大值序号
	std::vector<float> p2;//次大值的倒谱值
	std::vector<int> f3;//替换值序号
	std::vector<float> p3;//替换值的倒谱值
	std::vector<float> u;//中位数
	std::vector<float> threshold;//阈值
	std::vector<float> k1;//p1-u
	std::vector<float> k2;//p2-u
	std::vector<float>  divide;//k1/k2
	std::vector<int> tag;//标签，是否选中
	std::vector<float> pitch;//基音周期

	//第二个文件的数据
	WAVEFILEHEADER header2;
	CString m_strFileName2;
	CString m_strFilePath2;
	std::vector<short> data_left2;
	std::vector<short> data_right2;
	std::vector<short> data_one2;//一个声道时一个数据集
	int m_channels2;		//声道数
	int m_HZ2;				//采样频率
	int m_time2;			//文件的持续时间(s)
	int m_bytesPerSample2;	//每个样本的字节数(1或2)
	int m_extent2;			//波形的最大幅度 16位则为32768
	int m_numOfPoint2;		//总数据数
	int maxForOneData2;
	int minForOneData2;
	int maxForLeftData2;
	int minForLeftData2;
	int maxForRightData2;
	int minForRightData2;

	////////新添加
	double * lpc2;  //线性预测系数
	double *lpcc2;  //线性预测倒谱
	double *lpccmcc2;//线性预测mel倒谱
	double *mfcc2;   //mel倒谱
	double *melspectrum2;//mel频谱

	//倒谱数据
	std::vector<std::vector<float> > ifft_two;
	float maxForTwoIfft;
	float minForTwoIfft;
	std::vector<std::vector<float> > ifft_pitch2;
	//基于倒谱的端点检测数据
	std::vector<std::vector<int>> id_max_2;//最大峰值能力团的下标id
	std::vector<std::vector<int>> id_last_2;//最大峰值能力团的下标id
	std::vector<int> f1_2;//最大值序号
	std::vector<float> p1_2;//最大值的倒谱值
	std::vector<int> f2_2;//次大值序号
	std::vector<float> p2_2;//次大值的倒谱值
	std::vector<int> f3_2;//替换值序号
	std::vector<float> p3_2;//替换值的倒谱值
	std::vector<float> u_2;//中位数
	std::vector<float> threshold_2;//阈值
	std::vector<float> k1_2;//p1-u
	std::vector<float> k2_2;//p2-u
	std::vector<float>  divide_2;//k1/k2
	std::vector<int> tag_2;//标签，是否选中
	std::vector<float> pitch_2;//基音周期




	//第三个文件的数据
	WAVEFILEHEADER header3;
	CString m_strFileName3;
	CString m_strFilePath3;
	std::vector<short> data_left3;
	std::vector<short> data_right3;
	std::vector<short> data_one3;
	int m_channels3;		//声道数
	int m_HZ3;				//采样频率
	int m_time3;			//文件的持续时间(s)
	int m_bytesPerSample3;	//每个样本的字节数(1或2)
	int m_extent3;			//波形的最大幅度 16位则为32768
	int m_numOfPoint3;		//总数据数
	int maxForOneData3;
	int minForOneData3;
	int maxForLeftData3;
	int minForLeftData3;
	int maxForRightData3;
	int minForRightData3;
	// 操作
public:
	HWND	m_hWndView;			// 视图窗口句柄
	HWND	m_hWndCtrl;			// 控制窗口的句柄
	//private:
public:
	void ReadFromFile(WAVEFILEHEADER& header,std::vector<short>& data_one,std::vector<short>& data_left,std::vector<short>& data_right,
		int& channels,int& hz,int& time,int& bytesPerSample,int& extent,int& numOfPoint,
		int& max1,int& min1,int& maxL,int& minL,int& maxR,int& minR,
		CString& filename,CString& pathname,FILE* pfile);

	static void WritetoFile(WAVEFILEHEADER header,int start,int end,std::vector<short>& data_one,
		int bytesPerSample,FILE* pFile);

	void SetFrameLen(int x);
	void Pitch();
	void Pitch2();

	// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	// 实现
public:
	virtual ~CMyWaveDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFile1Open();
public:
	afx_msg void OnFile1SaveAs();
public:
	afx_msg void OnFile2Open();
public:
	afx_msg void OnFile2SaveAs();
public:
	afx_msg void OnFile3Open();
public:
	afx_msg void OnFile3SaveAs();

};


