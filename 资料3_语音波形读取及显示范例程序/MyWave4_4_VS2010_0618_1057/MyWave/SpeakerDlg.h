#pragma once
#include "afxwin.h"
#include "stdafx.h"
#include "resource.h"
#include<vector>
#include"Speech.h"
#include "afxcmn.h"
// CSpeakerDlg 对话框

class CSpeakerDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpeakerDlg)

public:
	CSpeakerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSpeakerDlg();

// 对话框数据
	enum { IDD = IDD_ADD_GMM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListBox name_list;
	CListBox result_list;
	CEdit m_name;
	CComboBox m_GMM_Size;
	CSliderCtrl Slider_identify;
	CSliderCtrl Slider_vertify;
public:
	afx_msg void OnBnClickedAddsp();
	afx_msg void OnBnClickedDelsp();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedSettime();

	afx_msg void OnBnClickedXlGmm();//训练GMM模型
	afx_msg void OnBnClickedSpeakerIdentify();	//说话人辨别
	afx_msg void OnBnClickedSpeakerVerify();//说话人确认
public: 
	void ReadWave(CString m_tempFileName,CString m_tempFilePath);
public:	
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
public:
	double * dataOut;
	double * mfcc;
    std::vector<short> datas;
public:
	CButton m_accept;
	CButton m_reject;

public:
	afx_msg void OnBnClickedFeacher();
public:
	afx_msg void OnBnClickedLinkWav();
public:
	void Detect(std::vector<short>& rawdata);
};
