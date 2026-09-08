#pragma once
#include<vector>
#include<complex>
class Methods
{
public:
	Methods(void);
public:
	~Methods(void);

public:
	int frame_LEN;
	
public:
//保存文件,一维数组
	void Save1(
		std::vector<float>& file_data//文件数据
		);
//保存文件,二维数组
	void Save2(
		std::vector<std::vector<float> >& file_data//文件数据
		);
//预加重
	void Pre_emphasis(
		std::vector<float>& pre_emphasis_In,
		std::vector<float>& pre_emphasis_Out
		);
//分帧
	void Frameset(
		int len,//数据长度
		int frame_len,//帧长
		int frame_tran,//帧移
		int n_frame,//每个文件所分的帧数
		std::vector<float>& frame_In,//输入数据
		std::vector<std::vector<float> >& frame_Out//输出数据
		);
//汉明窗
	void HammingWindow(
		int n_frame,//每个文件所分的帧数
		int lenth,//窗长
		std::vector<std::vector<float> >& hamming_In,//输入数据
		std::vector<std::vector<float> >& hamming_Out//输出数据
		);
//矩形窗
	void RectWindow(
		int n_frame,//每个文件所分的帧数
		int lenth,//窗长
		std::vector<std::vector<float> >& rect_In,//输入数据
		std::vector<std::vector<float> >& rect_Out//输出数据
		);
//快速傅里叶变换,单帧
	void FFT(
		int M,
		int N,
		std::vector<float>& fft_In,//fft转换前
		std::complex<float>* &X//fft转换后
		);
//快速傅里叶变换,整个文件
	void QFFT(
		int len,//数据长度
		int frame_len,//帧长
		int frame_tran,//帧移
		int n_frame,//每个文件所分的帧数
		std::vector<float>& frame_In,//输入数据
		int lenth,//窗长
		std::vector<std::vector<float> >& Qfft_Out//输出数据
		);
//逆傅里叶变换,单帧
	void IFFT(
		int M,
		int N,
		std::complex<float>* &X
		);
//逆傅里叶变换，整个文件
	void QIFFT(
		int len,//数据长度
		int frame_len,//帧长
		int frame_tran,//帧移
		int n_frame,//每个文件所分的帧数
		std::vector<float>& frame_In,//输入数据
		int lenth,//窗长
		std::vector<std::vector<float> >& Qifft_Out//输出数据
		);
//三角滤波器组
	void Triangle_filter(
		std::vector<std::vector<float> >& Triangle_Out,//输出数据
		int frame_len//帧长
		);
//DCT
	void Cos(
		std::vector<std::vector<float> > &Cos_Out
		);
//mel频率倒谱系数
	void MFCC(
		int len,//数据长度
		int frame_len,//帧长
		int frame_tran,//帧移
		int n_frame,//每个文件所分的帧数
		std::vector<float>& frame_In,//输入数据
		int lenth,//窗长
		std::vector<std::vector<float> >& mfcc_Out//输出数据
		);
//自相关函数
	void AutoCorrelation(
		int P,//LPC分析的阶数
		std::vector<std::vector<float> >& auto_In,//输入数据
		std::vector<std::vector<float> >& auto_Out//输出数据
		);
//杜宾算法,单帧
	void Durbin(
		int P,//LPC分析的阶数
		std::vector<float>& durbin_In,//输入数据
		std::vector<float>& durbin_Out//输出数据
		);
//LPC系数
	void LPC(
		int len,//数据长度
		int frame_len,//帧长
		int frame_tran,//帧移
		int n_frame,//每个文件所分的帧数
		std::vector<float>& frame_In,//输入数据
		int lenth,//窗长
		int P,//LPC分析的阶数
		std::vector<std::vector<float> >& LPC_Out//输出数据
		);
//短时能量
	void Energy(
		int len,//数据长度
		int frame_len,//帧长
		int frame_tran,//帧移
		int n_frame,//每个文件所分的帧数		
		int lenth,//窗长
		std::vector<float>& energy_In,//输入数据
		std::vector<float>& energy_Out//输出数据
		);
//短时平均幅度
	void Magnitude(
		int len,//数据长度
		int frame_len,//帧长
		int frame_tran,//帧移
		int n_frame,//每个文件所分的帧数		
		int lenth,//窗长
		std::vector<float>& magnitude_In,//输入数据
		std::vector<float>& magnitude_Out//输出数据
		);
//短时过零率
	void Zero(
		int len,//数据长度
		int frame_len,//帧长
		int frame_tran,//帧移
		int n_frame,//每个文件所分的帧数		
		int lenth,//窗长
		std::vector<float>& zero_In,//输入数据
		std::vector<float>& zero_Out//输出数据
		);
//短时自相关
	void Relevancy(
		int len,//数据长度
		int frame_len,//帧长
		int frame_tran,//帧移
		int n_frame,//每个文件所分的帧数		
		int lenth,//窗长
		int k,//短时相关最大延迟点数
		std::vector<float>& relevancy_In,//输入数据
		std::vector<std::vector<float> >& relevancy_Out//输出数据
		);
//平均幅度差函数法
	void ADMF(
		int len,//数据长度
		int frame_len,//帧长
		int frame_tran,//帧移
		int n_frame,//每个文件所分的帧数		
		int lenth,//窗长
		int k,//短时相关最大延迟点数
		std::vector<float>& AMDF_In,//输入数据
		std::vector<std::vector<float> >& AMDF_Out//输出数据
		);
	
//计算基音周期，单帧
	void GetPitch(
		int frame_len,//帧长
		std::vector<float>& data_In,//某一帧倒谱数据
		std::vector<int>& id_max,//最大峰值能量团的下标id
		std::vector<int>& id_last,
		int& f1,//最大峰值的序号
		int& f2,//次大峰值的序号
		int& f3,//替换值的序号
		float& p1,//最大峰值的倒谱值
		float& p2,//次大峰值的倒谱值
		float& p3,//替换值的倒谱值
		float& u,//中位数
		float& threshold,//阈值
		float& k1,//p1-u
		float& k2,//p2-u
		float& divide,//k1/k2
		float& pitch,//基音周期
		int& tag//标签
		);
//冒泡排序,降序
	void BubbleSort(int frame_len,float **a);
	//弧长
	void LengthOfArc(
		int len,//数据长度
		int frame_len,//帧长
		int frame_tran,//帧移
		int n_frame,//每个文件所分的帧数		
		int lenth,//窗长
		std::vector<float>& data_In,//输入数据
		std::vector<float>& arc_Out//输出数据
		);
};

