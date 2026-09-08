// SpeakerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SpeakerDlg.h"
#include"MyWaveDoc.h"
#include"GaussianMixture.h"
#include"Matrix.h"
#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
// CSpeakerDlg 对话框
#include<vector>
//GaussianMixture gmm;

IMPLEMENT_DYNAMIC(CSpeakerDlg, CDialog)

CSpeakerDlg::CSpeakerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeakerDlg::IDD, pParent)
{
	dataOut=NULL;
	mfcc=NULL;

}

CSpeakerDlg::~CSpeakerDlg()
{
	if (dataOut!= NULL)
		delete []dataOut;
	if (mfcc != NULL)
		delete []mfcc;
}

void CSpeakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, name_list);
	DDX_Control(pDX, IDC_LIST3, result_list);
	DDX_Control(pDX, IDC_NAMEEDIT, m_name);
	DDX_Control(pDX, IDC_COMBO1, m_GMM_Size);
	DDX_Control(pDX, IDC_SLIDER1, Slider_identify);
	DDX_Control(pDX, IDC_SLIDER2, Slider_vertify);
	DDX_Control(pDX, IDC_RADIO1, m_accept);
	DDX_Control(pDX, IDC_RADIO2, m_reject);
}


BEGIN_MESSAGE_MAP(CSpeakerDlg, CDialog)
	ON_BN_CLICKED(IDC_ADDSP, &CSpeakerDlg::OnBnClickedAddsp)
	ON_BN_CLICKED(IDC_DELSP, &CSpeakerDlg::OnBnClickedDelsp)
	ON_LBN_SELCHANGE(IDC_LIST1, &CSpeakerDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_SETTIME, &CSpeakerDlg::OnBnClickedSettime)
	ON_BN_CLICKED(IDC_XL_GMM, &CSpeakerDlg::OnBnClickedXlGmm)
	ON_BN_CLICKED(IDC_SPEAKER_IDENTIFY, &CSpeakerDlg::OnBnClickedSpeakerIdentify)
	ON_BN_CLICKED(IDC_SPEAKER_VERIFY, &CSpeakerDlg::OnBnClickedSpeakerVerify)
	ON_BN_CLICKED(IDC_FEACHER, &CSpeakerDlg::OnBnClickedFeacher)
	ON_BN_CLICKED(IDC_LINK_WAV, &CSpeakerDlg::OnBnClickedLinkWav)
END_MESSAGE_MAP()


// CSpeakerDlg 消息处理程序
BOOL CSpeakerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	/////////////////////////////////////////初始化姓名列表
	CString filename;
	CFileFind finder;
	//BOOL bWorking=finder.FindFile(_T("F:\\程序\\毕业设计\\实验数据_603\\原始数据\\T3\\*.wav"));
	BOOL bWorking=finder.FindFile(_T("F:\\程序\\毕业设计\\timit训练\\*.wav"));
	while(bWorking)
	{
		bWorking=finder.FindNextFileW();
		filename=finder.GetFileTitle();
		name_list.AddString(filename);
	}
	/////////////////////////////////////////////////////////
	////////////显示说话人数目
	char num[4];
	int i=name_list.GetCount();
	itoa(i,num,10);

	GetDlgItem(IDC_STATIC_NUMBER)->SetWindowText((CString)num);


	////////////////////////////////////////
	Slider_vertify.SetRange(-100,0,false);
	Slider_vertify.SetPos(-50);
	Slider_vertify.SetTicFreq(5);

	//gmm.load();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CSpeakerDlg::OnBnClickedAddsp()//添加说话人
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;//用于获取姓名文本框的内容
	CString train_time;//用于获取训练时间编辑框的内容
	CString temp_str;

	bool flag=1;//标记输入的姓名是否合法
	m_name.GetWindowText(str);
	//名字文本框为空的情况//////////////////////////////////////////
	if(str=="")
	{
		MessageBoxW(_T("请输入待添加的说话人姓名"),_T("警告信息"),MB_ICONEXCLAMATION);
		flag=0;
	}
	
	int i;
	//输入名字重复的情况//////////////////////////////////////////////
	for(i=0;i<name_list.GetCount();i++)
	{
		name_list.GetText(i,temp_str);//获得第i个列表项
		if(temp_str==str)
		{
			MessageBoxW(_T("你输入的说话人姓名已存在，请改名"),_T("警告信息"),MB_ICONEXCLAMATION);
			flag=0;
		}
	}
	//正确输入姓名的情况
	if(flag==1)
	{

	}

}

void CSpeakerDlg::OnBnClickedDelsp()//删除说话人
{
	// TODO: 在此添加控件通知处理程序代码
}

void CSpeakerDlg::OnLbnSelchangeList1()//改变姓名列表
{
	// TODO: 在此添加控件通知处理程序代码
	CString buffer;
	name_list.GetText(name_list.GetCurSel(),buffer);//取列表框中的内容
	GetDlgItem(IDC_STATIC_USER)->SetWindowText(buffer);

}

void CSpeakerDlg::OnBnClickedSettime()//设置训练或测试时间
{
	// TODO: 在此添加控件通知处理程序代码
}
void CSpeakerDlg::ReadWave(CString m_tempFileName,CString m_tempFilePath)//读取wave文件
{
	char ch[256];
	memset(ch,0,256);
	//WideCharToMultiByte该函数映射一个unicode字符串到一个多字节字符串
	WideCharToMultiByte(CP_OEMCP,0,m_tempFilePath,-1,ch,m_tempFilePath.GetLength ()*2,NULL,NULL);

	 FILE* pfile;
	if((pfile=fopen(ch,"rb"))==NULL)
	{
		AfxMessageBox(_T("读取文件失败！"));
		return;
	}

	//读取文件
	DWORD id;
	fread(&id,sizeof(DWORD),1,pfile);
	if(id!=ID_RIFF)
	{
		AfxMessageBox(_T("Not RIFF format!"));
		return;
	}
	fread(&(header.riff .dwRiffSize ),sizeof(DWORD),1,pfile);
	fread(&id,sizeof(DWORD),1,pfile);
	if(id!=ID_WAVE)
	{
	   AfxMessageBox(_T("Not WAVE format!"));
		return;
	}
	fread(&id,sizeof(DWORD),1,pfile);
	if(id!=ID_fmt)
	{
	   AfxMessageBox(_T("Not FMT format!"));
		return;
	}
   fread(&(header.fmt),sizeof(FMT_BLOCK),1,pfile);
   if(header.fmt.dwFmtSize ==18)
   {
	   WORD addition;
	   fread(&(addition),sizeof(WORD),1,pfile);

   }
  //////////////////////////////////////////////////////////////////////对该处代码进行一下修改，使得两个语音库的文件都能读取
   fread(&id,sizeof(DWORD),1,pfile);
   if(id==ID_fact)
   { 
		fread(&(header.fact ),sizeof(FACT_BLOCK),1,pfile);
		fread(&id,sizeof(DWORD),1,pfile);
	}
   else if(id==ID_PAD)
   {
	   DWORD x;
	   fread(&x,sizeof(DWORD),1,pfile);
	   BYTE* temp=new BYTE[x];
	   fread(temp,sizeof(BYTE),x,pfile);
	   fread(&id,sizeof(DWORD),1,pfile);
	   delete [] temp;
   }
   /////////////有问题？？？？？？？？？？？？
	if(id!= ID_data)
	{
		AfxMessageBox(_T("Not DATA format!"));
		return;
	}
	/////////////////////////////////////////////////////////////////////////
	fread(&(header.data),sizeof(DATA_BLOCK),1,pfile);
    
	m_channels=header.fmt.wavFormat .wChannels ;//声道数
	WORD bitsPerSimple=header.fmt.wavFormat .wBitsPerSample ;//每个样本所需的bit数
	m_extent=(1l<<(bitsPerSimple-1));//数据体的总字节数（一字节8位）WORD 16位
	DWORD sum=header.data .dwDataSize ;
	WORD bytes=(bitsPerSimple+7)/8;

	if(bytes>2)
	{ 
		AfxMessageBox(_T("Too large bits per sample!"));
		return;
	}
	m_bytesPerSample=bytes;
	data_one.clear ();//清除之前的数据
	//data_left.clear ();
	//data_right.clear ();
	m_HZ=header.fmt.wavFormat.dwSamplesPerSec ;
	//time=sum/(m_channels*bytes*header.fmt .wavFormat .dwSamplesPerSec);
	m_numOfPoint=sum/(m_channels*bytes);

	if(m_channels==1)
	{
		if(bytes==1)
		{
			BYTE* temp=new BYTE[sum];
			fread(temp,sizeof(BYTE),sum,pfile);
			for(int i=0;i<sum;i++)
			{
				data_one.push_back (temp[i]);
			}
			delete []temp;
		}
		else if(bytes==2)
		{
			short* temp=new short[sum/2];
			fread(temp,sizeof(short),sum/2,pfile);
			for(int i=0;i<sum/2;i++)
			{
				data_one.push_back (temp[i]);
			}
			delete []temp;

		}
	}
	else if(m_channels==2)
	{
		if(bytes==1)
		{
			BYTE* temp=new BYTE[sum];
			fread(temp,sizeof(BYTE),sum,pfile);

			for(int i=0;i<sum;i+=2)
			{
				data_left.push_back (temp[i]);
					data_right.push_back(temp[i+1]);
			}
			delete []temp;
		}
	   else if(bytes==2)
		{
			short* temp=new short[sum/2];
			fread(temp,sizeof(WORD),sum/2,pfile);

			for(int i=0;i<sum/2;i+=2)
			{
				data_left.push_back(temp[i]);
				data_right.push_back(temp[i+1]);
			}
			delete []temp;
		}
	}
	else
	{
		AfxMessageBox(_T("Not Normal Channel's Number!"));
		return;
	}
}
void CSpeakerDlg::OnBnClickedSpeakerIdentify()
{
	// TODO: 在此添加控件通知处理程序代码
	//// CFileDialog dlg(true,NULL,L"",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,L"音频文件（*.wav）|*.wav|所有文件(*.*)|*.*||");
 ////   
	////if(IDOK==dlg.DoModal ())
	////{
	////	m_tempFileName=dlg.GetFileName ();
	////	m_tempFilePath=dlg.GetPathName ();
	////	CString filename(dlg.GetFileName(),dlg.GetFileName().GetLength()-3);
	////	filename+=_T("txt");
	////	ReadWave(m_tempFileName,m_tempFilePath);

	////	int m_nFrameSize=256;
	////    unsigned int num=data_one.size ();
	////	//unsigned int num=16000*8;//测试时间为10s,测试点数为8000*10
 ////       int nCount=(int)floor((double)((num-m_nFrameSize)/(m_nFrameSize/2))+1);
	////    unsigned filterNum=24,cepsNum=13;
	////	int i,j;
	////	double *lpc=NULL,*lpcc=NULL;
	////	 
	////	//为data重新分配空间
	////	if (dataOut!= NULL)
	////		delete []dataOut;
	////	if (mfcc != NULL)
	////		delete []mfcc;
	////	if (lpcc != NULL)
	////		delete []lpcc;
	////	if (lpc != NULL)
	////		delete []lpc;
	////		
	////	dataOut=new double[num];
	////	
	////	for( i=0;i<num;i++)
	////	{
	////		dataOut[i]=this->data_one[i];
	////	} 
	////	
	////	 // 获取MfCC	
	////	mfcc=new double[cepsNum*nCount];
	////	CSpeech::GetMFCC(m_nFrameSize, dataOut, num,m_HZ,filterNum,cepsNum,mfcc);
	////	GaussianMixture gmm;
	////	gmm.identify(mfcc,nCount,m_nFrameSize);
	////   
	////	////lpcc=new double [(ip+1)*nCount];
	////	////lpc=new double [(ip+1)*nCount];
	//// ////   CSpeech::GetLPC(m_nFrameSize,dataOut,num,lpc);//获取LPC  

	////	////CSpeech::GetLPCC(m_nFrameSize,dataOut,num,lpcc,lpc); //获取lpcc

	////	////GaussianMixture gmm;
	////	////gmm.identify(lpcc,nCount,m_nFrameSize);
	////	result_list.ResetContent();//其作用是清空链表中所有的数据，使链表成为空链。
	////	
	////    CString str;
	////	for(i=0;i<gmm.sample_num;i++)
	////	{
	////		str.Format(_T("%s,%f"),gmm.s[i].name,gmm.s[i].result);
	////		result_list.AddString((LPCTSTR)str);
	////	}

	////}


   	CString filename;
	//GaussianMixture g;

	CString m_tempFilePath,m_tempFileName,m_filename;
	double *dataOut=NULL;
	double *mfcc=NULL,*mfcc1=NULL,*lpc=NULL,*lpcc=NULL;
	int m_nFrameSize=256;
	unsigned int num=16000*4;//测试时间为10s,测试点数为8000*10
    int nCount=(int)floor((double)((num-m_nFrameSize)/(m_nFrameSize/2))+1);
	unsigned filterNum=24,cepsNum=13;

	int i,j,k;
	int sum=0;
	//ofstream fout(_T("F:\\程序\\毕业设计\\实验数据_603\\60s\\T3作为训练数据\\随机训练\\mfcc13\\随机_随机_32_T3_T2_test.txt"),ios::app);
	//ofstream fout(_T("F:\\程序\\毕业设计\\实验数据_603\\30s\\N3作为训练数据\\随机训练\\lpc\\lpc_随机_随机_32_N3_N3_test.txt"),ios::app);
	ofstream fout(_T("F:\\程序\\毕业设计\\timit实验1\\test16.txt"),ios::app);
	////for(k=0;k<name_list.GetCount();k++)
	////{
	////	
	////	name_list.GetText(k,filename);//获得第i个列表
	////	
	////	m_tempFileName=filename+_T(".wav");
	////	m_tempFilePath=_T("F:\\程序\\毕业设计\\timit测试\\")+m_tempFileName;
	////	ReadWave(m_tempFileName,m_tempFilePath);
	////	//unsigned int num=this->data_one.size();//测试时间为10s,测试点数为8000*10
 //// //      int nCount=(int)floor((double)((num-m_nFrameSize)/(m_nFrameSize/2))+1);
	   CString str1;
       for(k=1;k<=100;k++)
	   {
		    str1.Format(_T("%d"),k);
			m_tempFileName=str1+_T(".wav");
			//m_tempFilePath=_T("F:\\程序\\毕业设计\\实验数据_603\\随机训练数据\\T3\\")+m_tempFileName;
			m_tempFilePath=_T("F:\\程序\\毕业设计\\timit训练\\")+m_tempFileName;
			ReadWave(m_tempFileName,m_tempFilePath);
			filename=str1;
        filename=filename+_T(".txt");
		Detect(data_one);
		//unsigned int num=this->data_one.size();
		//int nCount=(int)floor((double)((num-m_nFrameSize)/(m_nFrameSize/2))+1);//帧数 
		unsigned int num=datas.size();
		int nCount=(int)floor((double)((num-m_nFrameSize)/(m_nFrameSize/2))+1);
        //filename=filename+_T(".txt");		 
		//为data重新分配空间
		if (dataOut!= NULL)
			delete []dataOut;
		if (mfcc != NULL)
			delete []mfcc;
		if (mfcc1 != NULL)
			delete []mfcc1;
		if (lpcc != NULL)
			delete []lpcc;
		if (lpc != NULL)
			delete []lpc;
			
	    //unsigned int num=data_one.size ();
		
		dataOut=new double[num];
		
		for( i=0;i<num;i++)
		{
			dataOut[i]=datas[i];
		/*	dataOut[i]=this->data_one[i];*/
		} 
		
	 //   lpcc=new double [(ip+1)*nCount];
		//lpc=new double [(ip+1)*nCount];
	 //   CSpeech::GetLPC(m_nFrameSize,dataOut,num,lpc);//获取LPC  

		////CSpeech::GetLPCC(m_nFrameSize,dataOut,num,lpcc,lpc); //获取lpcc

		//GaussianMixture gmm;
		//gmm.identify(lpc,nCount,m_nFrameSize);

		

		 // 获取MfCC
		mfcc=new double[cepsNum*nCount];
		CSpeech::GetMFCC(m_nFrameSize, dataOut, num,m_HZ,filterNum,cepsNum,mfcc);
		//mfcc1=new double[(cepsNum-1)*nCount];
		//for(i=0;i<nCount;i++)
		//	for(j=1;j<cepsNum;j++)			
		//		mfcc1[i*(cepsNum-1)+j-1]=mfcc[i*cepsNum+j];


		GaussianMixture gmm;
		gmm.identify(mfcc,nCount,m_nFrameSize);
		//gmm.identify(mfcc1,nCount,m_nFrameSize);//去掉0阶mel系数后的辨认
		//result_list.ResetContent();//其作用是清空链表中所有的数据，使链表成为空链。
		//
	 //   CString str;

		
		if(gmm.s[0].name!=filename)	
		//	fout<<k+1<<"   正确"<<endl;
		//else
		{
			fout<<k+1<<"    错误"<<gmm.s[0].result<<endl;
			sum++;
		}

	}
	fout<<"失败次数：  "<<sum<<endl;
     fout.close();
	
 }

void CSpeakerDlg::OnBnClickedSpeakerVerify()
{
	// TODO: 在此添加控件通知处理程序代码
	 CFileDialog dlg(true,NULL,L"",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,L"音频文件（*.wav）|*.wav|所有文件(*.*)|*.*||");
    
	if(IDOK==dlg.DoModal ())
	{
		m_tempFileName=dlg.GetFileName ();
		m_tempFilePath=dlg.GetPathName ();
		ReadWave(m_tempFileName,m_tempFilePath);
		int m_nFrameSize=256;
	    //unsigned int num=data_one.size ();
		unsigned int num=16000*8;
        int nCount=(int)floor((double)((num-m_nFrameSize)/(m_nFrameSize/2))+1);
	    unsigned filterNum=24,cepsNum=13;
		int i,j;
		 
		//为data重新分配空间
		if (dataOut!= NULL)
			delete []dataOut;
		if (mfcc != NULL)
			delete []mfcc;
			
		dataOut=new double[num];
		
		for( i=0;i<num;i++)
		{
			dataOut[i]=this->data_one[5000+i];
		} 
		
		mfcc=new double[cepsNum*nCount];

		 // 获取MfCC
		CSpeech::GetMFCC(m_nFrameSize, dataOut,num,m_HZ,filterNum,cepsNum,mfcc);
	   
		GaussianMixture gmm;
    	gmm.verify(mfcc,nCount,m_nFrameSize);
	    m_accept.SetCheck(0);  //把“被接受”单选按钮设置成未选定状态
		m_reject.SetCheck(0);   //把“被拒绝”单选按钮设置成未选定状态
		CString rm;

		for(i=0;i<gmm.sample_num;i++)
		{
			GetDlgItem(IDC_STATIC_USER)->GetWindowText(rm);
			if(rm==gmm.s[i].name)
			{
				double aa=double(Slider_vertify.GetPos());
				if(gmm.s[i].result>=aa)
				{
					m_accept.SetCheck(1);
			     }
				else
				{
					m_reject.SetCheck(1);
				}
			 }
		}
	}
}

void CSpeakerDlg::OnBnClickedFeacher()
{
	// TODO: 在此添加控件通知处理程序代码
   	CString filename;
	CString filename1,filename2;
	//GaussianMixture g;

	CString m_tempFilePath,m_tempFileName,m_filename;
	double *dataOut=NULL;
	double *mfcc=NULL,*lpcc=NULL,*lpc=NULL;
	int m_nFrameSize=256;
	
	//unsigned int num=16000*60;//训练时间
	
	unsigned filterNum=24,cepsNum=13;		
	int i,j,k;
	////for(k=0;k<name_list.GetCount();k++)
	////{
	////	name_list.GetText(k,filename);//获得第i个列表项
	////	
	////	m_tempFileName=filename+_T(".wav");
	////	//m_tempFilePath=_T("F:\\程序\\毕业设计\\实验数据_603\\随机训练数据\\T3\\")+m_tempFileName;
	////    m_tempFilePath=_T("F:\\程序\\毕业设计\\timit训练\\")+m_tempFileName;	
	////	ReadWave(m_tempFileName,m_tempFilePath);
	   CString str1;
       for(k=61;k<=100;k++)
	   {
		    str1.Format(_T("%d"),k);
			m_tempFileName=str1+_T(".wav");
			//m_tempFilePath=_T("F:\\程序\\毕业设计\\实验数据_603\\随机训练数据\\T3\\")+m_tempFileName;
			m_tempFilePath=_T("F:\\程序\\毕业设计\\timit训练\\")+m_tempFileName;
			ReadWave(m_tempFileName,m_tempFilePath);
			filename=str1;
        filename=filename+_T(".txt");
		Detect(data_one);
		//unsigned int num=this->data_one.size();
		//int nCount=(int)floor((double)((num-m_nFrameSize)/(m_nFrameSize/2))+1);//帧数 
		unsigned int num=datas.size();
		int nCount=(int)floor((double)((num-m_nFrameSize)/(m_nFrameSize/2))+1);
		//为data重新分配空间
		if (dataOut!= NULL)
			delete []dataOut;
		if (mfcc != NULL)
			delete []mfcc;
		if (lpcc != NULL)
			delete []lpcc;
		if (lpc != NULL)
			delete []lpc;
			
		dataOut=new double[num];
        //m_filename=_T("F:\\程序\\毕业设计\\实验数据_603\\随机训练数据\\N3_data\\")+filename;
		m_filename=_T("F:\\程序\\毕业设计\\timit实验1\\data\\")+filename;
		ofstream fout1(m_filename, ios::trunc );
		for( i=0;i<num;i++)
		{
			//dataOut[i]=this->data_one[i];
			dataOut[i]=datas[i];
			fout1<<dataOut[i]<<endl;
		} 
		fout1.close();
		//获取lpc
	 ////   lpcc=new double [(ip+1)*nCount];
		////lpc=new double [(ip+1)*nCount];
	 ////   CSpeech::GetLPC(m_nFrameSize,dataOut,num,lpc);//获取LPC

		//////保存lpc参数
		////filename2=(_T("F:\\程序\\毕业设计\\实验数据_603\\60s\\T3作为训练数据\\随机训练\\lpc\\T3_lpc\\")+filename);

		////ofstream fout2(filename2, ios::trunc );
		////for(i=0;i<nCount;i++)
		////{
		////	for (j = 1; j <ip+1; j++)
		////	{	
		////		fout2 <<lpc[i*(ip+1)+j]<< "  "; 
		////	}
		////	fout2<<endl;
		////}
		//////关闭输出流
		////fout2.close();
  ////      //获取lpcc
		////CSpeech::GetLPCC(
		////				m_nFrameSize,dataOut,num,lpcc,lpc);

  ////      filename1=(_T("F:\\程序\\毕业设计\\实验数据_603\\60s\\T3作为训练数据\\随机训练\\\lpcc\\T3_lpcc\\")+filename);

		////ofstream fout(filename1, ios::trunc );
		////for(i=0;i<nCount;i++)
		////{
		////	for (j = 1; j <ip+1; j++)
		////	{	
		////		fout <<lpcc[i*(ip+1)+j]<< "  "; 
		////	}
		////	fout<<endl;
		////}
		//////关闭输出流
		////fout.close();
		////

		mfcc=new double[cepsNum*nCount];
		 // 获取MfCC
		CSpeech::GetMFCC(m_nFrameSize, dataOut,num,m_HZ,filterNum,cepsNum,mfcc);
		//保存mfcc数据
		
		 //filename1=(_T("F:\\程序\\毕业设计\\实验数据_603\\60s\\T3作为训练数据\\随机训练\\mfcc13-1\\T3_mfcc\\")+filename);
		 
		 filename1=(_T("F:\\程序\\毕业设计\\timit实验1\\mfcc13\\")+filename);

			ofstream fout(filename1, ios::trunc );
			for(i=0;i<nCount;i++)
			{
				for (j = 0; j <cepsNum; j++)//从第0维开始保存，不取第0阶的mel系数
				{	
					fout <<mfcc[i*cepsNum+j]<< "  "; 
				}
				fout<<endl;
			}
			//关闭输出流
			fout.close();
		///////////////////////去掉第0阶mel系数
			filename2=(_T("F:\\程序\\毕业设计\\timit实验1\\mfcc12\\")+filename);

			ofstream fout2(filename2, ios::trunc );
			for(i=0;i<nCount;i++)
			{
				for (j = 1; j <cepsNum; j++)//从第1维开始保存，不取第0阶的mel系数
				{	
					fout2 <<mfcc[i*cepsNum+j]<< "  "; 
				}
				fout2<<endl;
			}
			//关闭输出流
			fout2.close();
	}
	if(dataOut!=NULL)
		delete []dataOut;
	if(mfcc!=NULL)
		delete []mfcc;
	if(lpcc!=NULL)
		delete []lpcc;
	if (lpc != NULL)
		delete []lpc;
}
void CSpeakerDlg::OnBnClickedXlGmm()//训练GMM
{
	// TODO: 在此添加控件通知处理程序代码
	int i;
	CString filename;
	GaussianMixture gmm;
	unsigned int nbData=0;
 	Matrix rawData;
	//for(i=0;i<name_list.GetCount();i++)
	//{
	//	nbData=0;
	//	name_list.GetText(i,filename);//获得第i个列表项
	for(i=1;i<=100;i++)
	{
		filename.Format(_T("%d"),i);
		filename=filename+_T(".txt");
        nbData=0;
		//rawData=gmm.loadDataFile(_T("F:\\程序\\毕业设计\\实验数据_603\\60s\\T3作为训练数据\\随机训练\\lpc\\T3_lpc\\")+filename);
		rawData=gmm.loadDataFile(_T("F:\\程序\\毕业设计\\timit实验1\\mfcc13\\")+filename);
		nbData +=rawData.RowSize();   
		 
		gmm.GMMs(rawData,16/*NBSTATES*/);
		//g.saveParams(_T("zyx.txt"));
		gmm.saveParams(_T("F:\\程序\\毕业设计\\timit实验1\\gmm_16\\")+filename); 
		//gmm.saveParams(_T("F:\\程序\\毕业设计\\实验数据_603\\60s\\T3作为训练数据\\随机训练\\lpc\\T3_lpc_gmm_16\\")+filename); 

	}
}

void CSpeakerDlg::OnBnClickedLinkWav()
{
	// TODO: 在此添加控件通知处理程序代码
    CMyWaveDoc pDoc;
	CString filename,m_tempFilePath,m_tempFileName,pfile;//记录保存在gmm文件夹中的文件名
	CFileFind finder;
	std::ifstream fich;
	int m=0;
	int i,j,k;
	CString str1,str2;
	data_left.clear();
    for(i=434;i<=440;i++)
	{
		this->data_left.clear();
		str1.Format(_T("%d"),i);
		for(j=8;j<=10;j++)
		{	
			str2.Format(_T("%d"),j);
			m_tempFileName=str2+_T(".wav");
			//m_tempFilePath=_T("F:\\程序\\毕业设计\\实验数据_603\\随机训练数据\\T3\\")+m_tempFileName;
			m_tempFilePath=_T("F:\\程序\\毕业设计\\TIMIT\\")+str1;
			m_tempFilePath=m_tempFilePath+_T("\\")+m_tempFileName;	
			ReadWave(m_tempFileName,m_tempFilePath);
			for(k=0;k<data_one.size();k++)
				data_left.push_back(data_one[k]);
		
		}
		//ofstream fout(_T("F:\\程序\\毕业设计\\timit_data\\")+str1+_T(".txt"),ios::trunc);
		//for(j=0;j<data_left.size();j++)
		//	fout<<data_left[j]<<endl;
		//fout.close();
		
		//  CFileDialog dlg(false, NULL, L"", 
		//OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"音频文件(*.wav)|*.wav|所有文件(*.*)|*.*||");

	 //  if(IDOK==dlg.DoModal())
	 // {
		//CString FileName=dlg.GetFileName();
		//CString FilePath=dlg.GetPathName();
		//FilePath+=_T(".wav");
		  CString  FileName=str1+_T(".wav");
		  CString FilePath=_T("F:\\程序\\毕业设计\\timit测试\\")+FileName;

		char chr[256];
		memset(chr,0,256);
		WideCharToMultiByte(CP_OEMCP,0,FilePath,-1,chr,FilePath.GetLength()*2,NULL,NULL);

		FILE* pfile;
		if((pfile=fopen(chr,"wb"))==NULL)
		{
			AfxMessageBox(_T("创建文件失败!"));
			return;
		}
		//WritetoFile(header,start,end,data_one,m_bytesPerSample,pfile);
		//pDoc.WritetoFile(header,0,this->data_left.size()-1,this->data_left,this->m_bytesPerSample,pfile);
		
		int sum=data_left.size();

		int size=sum*this->m_bytesPerSample+58;

		char ch='R';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='I';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='F';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		fwrite(&ch,sizeof(unsigned char),1,pfile);

		int i=size-8;
		fwrite(&i,sizeof(DWORD),1,pfile);

		ch='W';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='A';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='V';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='E';
		fwrite(&ch,sizeof(unsigned char),1,pfile);

		ch='f';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='m';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='t';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch=' ';
		fwrite(&ch,sizeof(unsigned char),1,pfile);

		int num=header.fmt.dwFmtSize;
		fwrite(&num,sizeof(DWORD),1,pfile);

		fwrite(&(header.fmt.wavFormat.wFormatTag),sizeof(WORD),1,pfile);
		fwrite(&(header.fmt.wavFormat.wChannels),sizeof(WORD),1,pfile);
		fwrite(&(header.fmt.wavFormat.dwSamplesPerSec),sizeof(DWORD),1,pfile);
		fwrite(&(header.fmt.wavFormat.dwAvgBytesPerSec),sizeof(DWORD),1,pfile);
		fwrite(&(header.fmt.wavFormat.wBlockAlign),sizeof(WORD),1,pfile);
		fwrite(&(header.fmt.wavFormat.wBitsPerSample),sizeof(WORD),1,pfile);
		if(num==18)
		{
			WORD temp=12;
			fwrite(&temp,sizeof(WORD),1,pfile);
		}

		ch='f';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='a';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='c';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='t';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		int four=4;
		fwrite(&four,sizeof(DWORD),1,pfile);
		DWORD temp1=123123;
		fwrite(&temp1,sizeof(DWORD),1,pfile);

		ch='d';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='a';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='t';
		fwrite(&ch,sizeof(unsigned char),1,pfile);
		ch='a';
		fwrite(&ch,sizeof(unsigned char),1,pfile);

		DWORD t=size-58;
		fwrite(&t,sizeof(DATA_BLOCK),1,pfile);

		if(this->m_bytesPerSample==1)
		{
			for(int k=0;k<sum;k++)
				fwrite(&data_left[k],sizeof(BYTE),1,pfile);
		}
		else
		{
			for(int k=0;k<sum;k++)
			{
				fwrite(&data_left[k],sizeof(WORD),1,pfile);
			}
		}
		fclose(pfile);
		//}
		}
}
void CSpeakerDlg::Detect(std::vector<short>& rawdata)
{
	datas.clear();
	 unsigned int num=rawdata.size ();
	 int FrameSize=256;
     int nCount=(int)floor((double)((num-FrameSize)/(FrameSize/2))+1);
     unsigned int sample_rate=nCount*FrameSize;
	 double *pDataOut=NULL;
	 int i,j;
	//为data重新分配空间
	//if (data != NULL)
	//	delete[] data;
	//if (dataout != NULL)
	//	delete[] dataout;
		
	double *data=new double[num];
	//ofstream fout(_T("陈峥嵘.txt"),ios::trunc);
	for( i=0;i<num;i++)
	{
		data[i]=rawdata[i];
		//fout<<data[i]<<endl;
	} 
	//fout.close();
	
	double *dataout=new double[nCount];
	pDataOut=new double [sample_rate];//保存分帧后的数据
	CSpeech::Frame(FrameSize,data,num,pDataOut);
	// 获取短时能量
   
		CSpeech::GetEnergy(
					FrameSize, pDataOut, sample_rate, dataout);

		double deltaEnergyLevel1 = -20; //短时能量的两个门限
		double deltaEnergyLevel2 = -10;

		
		for(i=0;i<nCount;i++)//求每一帧的能量均值
			dataout[i]=dataout[i]/FrameSize;
		
		double max=10*log10(dataout[0]),min=10*log10(dataout[0]);

		for(i=0;i<nCount;i++)//求每一帧的对数能量		
		{
			if(dataout[i]!=0)
			   dataout[i]=10*log10(dataout[i]);
			if(dataout[i]>max)
				max=dataout[i];
			if(dataout[i]<min)
				min=dataout[i];
		}
		//计算短时能量和门限		
        double energyLevel1 = max+deltaEnergyLevel1;
        double energyLevel2 = max+deltaEnergyLevel2;

		// 获取短时过零率
		double *zcr=new double[nCount];

		CSpeech::GetZero(
					FrameSize,pDataOut, sample_rate, zcr);
		double max_zero=zcr[0];
        for(i=1;i<nCount;i++)
		{
			if(zcr[i]>max_zero)
				max_zero=zcr[i];
		}	
		//ofstream fout2("zcr.txt",ios::trunc);
		//for(i=0;i<nCount;i++)
		//{
		//	fout2<<zcr[i]<<endl;
		//}
		//fout2.close();
		double zcrRatio=0.20;//短时过零率门限
		double zcrThreshold = max_zero*zcrRatio;
		///////////////////////////////////////////////////根据较高的门限找到起止点
		std::vector<short> voiceIndex;
        for(i=0;i<nCount;i++)
		{
			if(dataout[i]>energyLevel2)
				voiceIndex.push_back(i);
		}
  //      ofstream fout1("energy2.txt",ios::trunc);
		//for(i=0;i<voiceIndex.size();i++)
		//{
		//	fout1<<voiceIndex[i]<<endl;
		//}
		//fout1.close();
		//////////////////////////////////////////记录超过门限的语音段的起始点
		std::vector<short> sound;
		int k=1;
		int nums=voiceIndex.size();
		sound.push_back(voiceIndex[0]);
		for(i=1;i<nums-1;i++)
		{
			if(voiceIndex[i+1]-voiceIndex[i]>1)
			{
				sound.push_back(voiceIndex[i]);
				sound.push_back(voiceIndex[i+1]);
				k++;
			}
		}
		sound.push_back(voiceIndex[nums-1]);


		///////////////////////////////////////////% 忽略一些小的细节

		std::vector<short> sound2;
		for(i=0;i<sound.size()/2;i++)
		{
			if(sound[2*i+1]-sound[2*i]>=3)
			{
				sound2.push_back(sound[2*i]);
				sound2.push_back(sound[2*i+1]);
			}
		}
		//ofstream fout1("energy2.txt",ios::trunc);
		//for(i=0;i<sound2.size()/2;i++)
		//{
		//	fout1<<sound2[2*i]<<"   "<<sound2[2*i+1]<<endl;
		//}
		//fout1.close();

        //////////////////////////////////////////根据较低的门限进一步判断

		int head,tail;
		for(i=0;i<sound2.size()/2;i++)
		{
			head=sound2[2*i];
			while((head-1)>=0&&dataout[head-1]>energyLevel1)//判断前一帧大于能量阈值1吗，
				head=head-1;
			sound2[2*i]=head;
			tail=sound2[2*i+1];
			while((tail+1)<nCount&&dataout[tail+1]>energyLevel1)
				tail+=1;
			sound2[2*i+1]=tail;
		}	
		//ofstream fout2("energy2-2.txt",ios::trunc);
		//for(i=0;i<sound2.size()/2;i++)
		//{
		//	fout2<<sound2[2*i]<<"   "<<sound2[2*i+1]<<endl;
		//}
		//fout2.close();

		//////////////////////////////////////////根据zcr门限判断起止点
		for(i=0;i<sound2.size()/2;i++)
		{
			head=sound2[2*i];
			while((head-1)>=0&&zcr[head-1]>=zcrThreshold)
				head=head-1;
			sound2[2*i]=head;
			tail=sound2[2*i+1];
			while((tail+1)<nCount&&zcr[tail+1]>zcrThreshold)
				tail+=1;
			sound2[2*i+1]=tail;
		}
		//ofstream fout2("energy2-2.txt",ios::trunc);
		//for(i=0;i<sound2.size()/2;i++)
		//{
		//	fout2<<sound2[2*i]<<"   "<<sound2[2*i+1]<<endl;
		//}
		//fout2.close();

		/////////////////////////////////////去掉重复的语音帧
		std::vector <short> index;	
		std::vector<short> sound3;
		for(i=0;i<sound2.size()/2-1;i++)
		{
			if((sound2[2*i]==sound2[2*(i+1)]&&sound2[2*i+1]==sound2[2*(i+1)+1]))
		        index.push_back(i);
		}
		bool b;
		for(i=0;i<sound2.size()/2;i++)
		{
			
			b=TRUE;
			for(j=0;j<index.size();j++)
			{
				if(i==index[j])
				{
					b=FALSE;
					break;	
				}			 

			}	
			if(b==TRUE)
			{
				sound3.push_back(sound2[2*i]);
		        sound3.push_back(sound2[2*i+1]);
			}
        }

		
		//ofstream fout3("energy3.txt",ios::trunc);
		//for(i=0;i<sound3.size()/2;i++)
		//{
		//	fout3<<sound3[2*i]<<"   "<<sound3[2*i+1]<<endl;
		//}
		//fout3.close();
        ////////////////////////将序列变换成整段语音的样点序列
		std::vector <int> out;
		//std::vector<short> datas;//端点检测后的数据
		if(sound3.size()!=0)
		{
			for(i=0;i<sound3.size()/2;i++)
			{
				out.push_back(sound3[2*i]*(FrameSize-FrameSize/2));
				out.push_back(sound3[2*i+1]*(FrameSize-FrameSize/2)+FrameSize);      
			}		
			
			//for(i=0;i<sound3.size()/2;i++)
			//{
			//	for(j=sound3[2*i];j<=sound3[2*i+1];j++)
			//	{
			//		for(k=0;k<FrameSize;k++)
			//			datas.push_back(pDataOut[j*FrameSize+k]);
			//	}
			//		
			//}
		}
	 //  ofstream fout4("out.txt",ios::trunc);
		//for(i=0;i<out.size()/2;i++)
		//{
		//	fout4<<out[2*i]<<"   "<<out[2*i+1]<<endl;
		//}
		//fout4.close();
		if(out.size()!=0)
		{
			for(i=0;i<out.size()/2;i++)
			{
				for(j=out[2*i];j<=out[2*i+1];j++)
				{
					datas.push_back(data[j]);
				}
					
			}
		}
}