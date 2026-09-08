// MyWaveDoc.cpp : CMyWaveDoc 类的实现
//

#include "stdafx.h"
#include "MyWave.h"

#include "MyWaveDoc.h"
#include "Methods.h"
#include "OperateDlg.h"
#include<iostream>
#include<fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyWaveDoc

IMPLEMENT_DYNCREATE(CMyWaveDoc, CDocument)

BEGIN_MESSAGE_MAP(CMyWaveDoc, CDocument)
	ON_COMMAND(ID_FILE1_OPEN, &CMyWaveDoc::OnFile1Open)
	ON_COMMAND(ID_FILE1_SAVE_AS, &CMyWaveDoc::OnFile1SaveAs)
	ON_COMMAND(ID_FILE2_OPEN, &CMyWaveDoc::OnFile2Open)
	ON_COMMAND(ID_FILE2_SAVE_AS, &CMyWaveDoc::OnFile2SaveAs)
	ON_COMMAND(ID_FILE3_OPEN, &CMyWaveDoc::OnFile3Open)
	ON_COMMAND(ID_FILE3_SAVE_AS, &CMyWaveDoc::OnFile3SaveAs)
END_MESSAGE_MAP()


// CMyWaveDoc 构造/析构

CMyWaveDoc::CMyWaveDoc()
	: frame_len(512)
	, m_strFileName(_T(""))
	, m_strFilePath(_T(""))
	, m_tempFileName(_T(""))
	, m_tempFilePath(_T(""))
	, m_channels(0)
	, m_HZ(0)
	, m_time(0)
	, m_bytesPerSample(0)
	, m_extent(0)
	, m_numOfPoint(0)
	, maxForOneData(0)
	, minForOneData(0)
	, maxForLeftData(0)
	, minForLeftData(0)
	, maxForRightData(0)
	, minForRightData(0)
	, m_strFileName2(_T(""))
	, m_strFilePath2(_T(""))
	, m_channels2(0)
	, m_HZ2(0)
	, m_time2(0)
	, m_bytesPerSample2(0)
	, m_extent2(0)
	, m_numOfPoint2(0)
	, maxForOneData2(0)
	, minForOneData2(0)
	, maxForLeftData2(0)
	, minForLeftData2(0)
	, maxForRightData2(0)
	, minForRightData2(0)
	, m_strFileName3(_T(""))
	, m_strFilePath3(_T(""))
	, m_channels3(0)
	, m_HZ3(0)
	, m_time3(0)
	, m_bytesPerSample3(0)
	, m_extent3(0)
	, m_numOfPoint3(0)
	, maxForOneData3(0)
	, minForOneData3(0)
	, maxForLeftData3(0)
	, minForLeftData3(0)
	, maxForRightData3(0)
	, minForRightData3(0)
{
	// TODO: 在此添加一次性构造代码
	/////////////////////////////////////////////新加代码
	  lpc=NULL;  //线性预测系数
      lpcc=NULL;  //线性预测倒谱
      lpccmcc=NULL;//线性预测mel倒谱
      mfcc=NULL; 
	  lpc2=NULL;  //线性预测系数
      lpcc2=NULL;  //线性预测倒谱
      lpccmcc2=NULL;//线性预测mel倒谱
      mfcc2=NULL; 
	  melspectrum=NULL;//mel频谱
	  melspectrum2=NULL;


}

CMyWaveDoc::~CMyWaveDoc()
{
	if(lpc!=NULL)
		delete []lpc;
	if(lpcc!=NULL)
		delete []lpcc;
	if(lpccmcc!=NULL)
		delete []lpccmcc;
	if(mfcc!=NULL)
		delete []mfcc;
	if (melspectrum!=NULL)
	{
		delete []melspectrum;
	}
	if(lpc2!=NULL)
		delete []lpc2;
	if(lpcc2!=NULL)
		delete []lpcc2;
	if(lpccmcc2!=NULL)
		delete []lpccmcc2;
	if(mfcc2!=NULL)
		delete []mfcc2;
	if (melspectrum2!=NULL)
	{
		delete []melspectrum2;
	}
}

BOOL CMyWaveDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMyWaveDoc 序列化

void CMyWaveDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CMyWaveDoc 诊断

#ifdef _DEBUG
void CMyWaveDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMyWaveDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMyWaveDoc 命令
void CMyWaveDoc::ReadFromFile(WAVEFILEHEADER& header,std::vector<short>& data_one,std::vector<short>& data_left,std::vector<short>& data_right,
	int& channels,int& hz,int& time,int& bytesPerSample,int& extent,int& numOfPoint,
	int& max1,int& min1,int& maxL,int& minL,int& maxR,int& minR,
	CString& filename,CString& pathname,FILE* pfile)
{
	filename=m_tempFileName;
	pathname=m_tempFilePath;

	//WAVEFILEHEADER header;
	//读取头文件
	DWORD id;
	fread(&id,sizeof(DWORD),1,pfile);//RIFF文件头	
	if(id != ID_RIFF)
	{
		AfxMessageBox(_T("Not RIFF format!"));
		return;
	}
	fread(&(header.riff.dwRiffSize),sizeof(DWORD),1,pfile);//文件大小
	fread(&id,sizeof(DWORD),1,pfile);//WAVE文件头
	if(id != ID_WAVE)
	{
		AfxMessageBox(_T("Not WAVE format!"));
		return;
	}
	fread(&id,sizeof(DWORD),1,pfile);//FMT辨别码
	if(id != ID_fmt)
	{
		AfxMessageBox(_T("Not FMT format!"));
		return;
	}
	fread(&(header.fmt),sizeof(FMT_BLOCK),1,pfile);//FMT块大小
	if(header.fmt.dwFmtSize==18)
	{
		WORD addition;
		fread(&(addition),sizeof(WORD),1,pfile);//2字节附加信息
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
	if(id!= ID_data)
	{
		AfxMessageBox(_T("Not DATA format!"));
		return;
	}
	/////////////////////////////////////////////////////////////
	fread(&(header.data),sizeof(DATA_BLOCK),1,pfile);

	channels=header.fmt.wavFormat.wChannels;				//声道数
	WORD bitsPerSimple=header.fmt.wavFormat.wBitsPerSample;	//每个样本所需的bit数
	extent=(1l<<(bitsPerSimple-1));
	DWORD sum=header.data.dwDataSize;						//数据体的总字节数(一字节8位) WORD 16位
	WORD bytes=(bitsPerSimple+7)/8;


	if(bytes>2)
	{
		AfxMessageBox(_T("Too large bits per sample!"));
		return;
	}
	bytesPerSample=bytes;
	data_one.clear();		//清除之前的数据
	data_left.clear();
	data_right.clear();
	hz=header.fmt.wavFormat.dwSamplesPerSec;
	time=sum/(channels*bytes*header.fmt.wavFormat.dwSamplesPerSec);
	numOfPoint=sum/(channels*bytes);



	if(channels==1)
	{					
		if(bytes==1)
		{
			BYTE* temp=new BYTE[sum];
			fread(temp,sizeof(BYTE),sum,pfile);
			max1=min1=temp[0];
			for(int i=0;i<(int)sum;i++)
			{
				data_one.push_back(temp[i]);
				if(temp[i]>max1)
					max1=temp[i];
				else if(temp[i]<min1)
					min1=temp[i];
			}
			delete []temp;
		}	
		else if(bytes==2)
		{
			short* temp=new short[sum/2];
			fread(temp,sizeof(short),sum/2,pfile);
			max1=min1=temp[0];
			for(int i=0;i<(int)sum/2;i++)
			{
				data_one.push_back(temp[i]);
				if(temp[i]>max1)
					max1=temp[i];
				else if(temp[i]<min1)
					min1=temp[i];
			}
			delete []temp;
		}
	}
	else if(channels==2)
	{
		if(bytes==1)
		{
			BYTE* temp=new BYTE[sum];
			fread(temp,sizeof(BYTE),sum,pfile);
			maxL=minL=temp[0];
			maxR=minR=temp[1];
			for(int i=0;i<(int)sum;i+=2)
			{
				data_left.push_back(temp[i]);
				data_right.push_back(temp[i+1]);
				if(temp[i]>maxL)
					maxL=temp[i];
				else if(temp[i]<minL)
					minL=temp[i];
				if(temp[i+1]>maxR)
					maxR=temp[i+1];
				else if(temp[i+1]<minR)
					minR=temp[i+1];
			}
			delete []temp;
		}	
		else if(bytes==2)
		{
			short* temp=new short[sum/2];
			fread(temp,sizeof(WORD),sum/2,pfile);
			maxL=minL=temp[0];
			maxR=minR=temp[1];
			for(int i=0;i<(int)sum/2;i+=2)
			{
				data_left.push_back(temp[i]);
				data_right.push_back(temp[i+1]);
				if(temp[i]>maxL)
					maxL=temp[i];
				else if(temp[i]<minL)
					minL=temp[i];
				if(temp[i+1]>maxR)
					maxR=temp[i+1];
				else if(temp[i+1]<minR)
					minR=temp[i+1];
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

void CMyWaveDoc::WritetoFile(WAVEFILEHEADER head,int start,int end,std::vector<short>& data_one,
						int bytesPerSample,FILE* pfile)
{

	if(start==end)
	{
		start=0;
		end=data_one.size()-1;
	}

	int sum=end-start+1;

	int size=sum*bytesPerSample+58;

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

	int num=head.fmt.dwFmtSize;
	fwrite(&num,sizeof(DWORD),1,pfile);

	fwrite(&(head.fmt.wavFormat.wFormatTag),sizeof(WORD),1,pfile);
	fwrite(&(head.fmt.wavFormat.wChannels),sizeof(WORD),1,pfile);
	fwrite(&(head.fmt.wavFormat.dwSamplesPerSec),sizeof(DWORD),1,pfile);
	fwrite(&(head.fmt.wavFormat.dwAvgBytesPerSec),sizeof(DWORD),1,pfile);
	fwrite(&(head.fmt.wavFormat.wBlockAlign),sizeof(WORD),1,pfile);
	fwrite(&(head.fmt.wavFormat.wBitsPerSample),sizeof(WORD),1,pfile);
	if(num==18)//如果fmt的长度为18时，有2个附加值，16000Hz的采用语音库的文件取得16，所以在截取语音波形保存时，不添加额外的值
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

	if(bytesPerSample==1)
	{
		for(int i=0;i<sum;i++)
			fwrite(&data_one[start+i],sizeof(BYTE),1,pfile);
	}
	else
	{
		for(int i=0;i<sum;i++)
			fwrite(&data_one[start+i],sizeof(WORD),1,pfile);
	}
}
void CMyWaveDoc::OnFile1Open()
{
	// TODO: 在此添加命令处理程序代码
    CFileDialog dlg(true,NULL,L"",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,L"音频文件（*.wav）|*.wav|所有文件(*.*)|*.*||");
    
	if(IDOK==dlg.DoModal ())
	{
		m_tempFileName=dlg.GetFileName ();
		m_tempFilePath=dlg.GetPathName ();

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
		ReadFromFile(header,data_one,data_left,data_right,m_channels,m_HZ,m_time,m_bytesPerSample,m_extent,m_numOfPoint,
									 maxForOneData,minForOneData,maxForLeftData,minForLeftData,
									 maxForRightData,minForRightData,m_strFileName,m_strFilePath,pfile);

	
	fclose(pfile);
	int max_extent=m_extent;
    if(max_extent<m_extent2)
	  max_extent=m_extent2;
	  if(max_extent<m_extent3)
	  max_extent=m_extent3;

	
	int max_numOfPoint=m_numOfPoint;
 	if(max_numOfPoint<m_numOfPoint2)
		max_numOfPoint=m_numOfPoint2;
	if(max_numOfPoint<m_numOfPoint3)
		max_numOfPoint=m_numOfPoint3;
 

	CMyWaveView* pView=(CMyWaveView*)CView::FromHandle (m_hWndView);
	pView->m_extent =(max_extent/100+1)*100;
	pView->m_numOfPoint =max_numOfPoint;
	pView->Invalidate ();

	COperateDlg* view=(COperateDlg*)CView::FromHandle (m_hWndCtrl);
	SCROLLINFO x_info;
	x_info.cbSize =sizeof(SCROLLINFO);
	x_info.fMask =SIF_ALL;
	x_info.nMax =max_numOfPoint;
	x_info.nMin =0;
	x_info.nPage =60;
	x_info.nPos =0;
	x_info.nTrackPos =0;
	view->m_X_ShowPos .SetScrollInfo (&x_info);
	view->UpdateFileInfoCtrl(header,header2,header3);
	}

}

void CMyWaveDoc::OnFile1SaveAs()
{
	// TODO: 在此添加命令处理程序代码
  CFileDialog dlg(false, NULL, L"", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"音频文件(*.wav)|*.wav|所有文件(*.*)|*.*||");

	if(IDOK==dlg.DoModal())
	{
		CString FileName=dlg.GetFileName();
		CString FilePath=dlg.GetPathName();
		FilePath+=_T(".wav");

		char chr[256];
		memset(chr,0,256);
		WideCharToMultiByte(CP_OEMCP,0,FilePath,-1,chr,FilePath.GetLength()*2,NULL,NULL);

		FILE* pfile;
		if((pfile=fopen(chr,"wb"))==NULL)
		{
			AfxMessageBox(_T("创建文件失败!"));
			return;
		}
		CMyWaveView* pView=(CMyWaveView*)CView::FromHandle(m_hWndView);
		int start=pView->m_startPlayPos;
		int end=pView->m_endPlayPos;
		WritetoFile(header,start,end,data_one,m_bytesPerSample,pfile);

		fclose(pfile);
	}
}

void CMyWaveDoc::OnFile2Open()
{
	// TODO: 在此添加命令处理程序代码
	
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(true, NULL, L"", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"音频文件(*.wav)|*.wav|所有文件(*.*)|*.*||");

	if(IDOK==dlg.DoModal())
	{
		m_tempFileName=dlg.GetFileName();
		m_tempFilePath=dlg.GetPathName();

		char ch[256];
		memset(ch,0,256);
		WideCharToMultiByte(CP_OEMCP,0,m_tempFilePath,-1,ch,m_tempFilePath.GetLength()*2,NULL,NULL);

		FILE* pfile;
		if((pfile=fopen(ch,"rb"))==NULL)
		{
			AfxMessageBox(_T("读取文件失败!"));
			return;
		}

		ReadFromFile(header2,data_one2,data_left2,data_right2,m_channels2,m_HZ2,m_time2,m_bytesPerSample2,m_extent2,m_numOfPoint2,
									 maxForOneData2,minForOneData2,maxForLeftData2,minForLeftData2,
									 maxForRightData2,minForRightData2,m_strFileName2,m_strFilePath2,pfile);
		fclose(pfile);

		int max_extent=m_extent;
		if(max_extent<m_extent2)
			max_extent=m_extent2;
		if(max_extent<m_extent3)
			max_extent=m_extent3;

		int max_numOfPoint=m_numOfPoint;
		if(max_numOfPoint<m_numOfPoint2)
			max_numOfPoint=m_numOfPoint2;
		if(max_numOfPoint<m_numOfPoint3)
			max_numOfPoint=m_numOfPoint3;

		CMyWaveView* pView=(CMyWaveView*)CView::FromHandle(m_hWndView);
		pView->m_extent=(max_extent/100+1)*100;
		pView->m_numOfPoint=max_numOfPoint;
		pView->Invalidate();

		COperateDlg* view=(COperateDlg*)CView::FromHandle(m_hWndCtrl);
		SCROLLINFO x_info;
		x_info.cbSize=sizeof(SCROLLINFO);
		x_info.fMask=SIF_ALL;
		x_info.nMax=max_numOfPoint;
		x_info.nMin=0;
		x_info.nPage=60;
		x_info.nPos=0;
		x_info.nTrackPos=0;
		view->m_X_ShowPos.SetScrollInfo(&x_info);
		view->UpdateFileInfoCtrl(header,header2,header3);
	}
}

void CMyWaveDoc::OnFile2SaveAs()
{
	// TODO: 在此添加命令处理程序代码
	 CFileDialog dlg(false, NULL, L"", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"音频文件(*.wav)|*.wav|所有文件(*.*)|*.*||");

	if(IDOK==dlg.DoModal())
	{
		CString FileName=dlg.GetFileName();
		CString FilePath=dlg.GetPathName();
		FilePath+=_T(".wav");

		char chr[256];
		memset(chr,0,256);
		WideCharToMultiByte(CP_OEMCP,0,FilePath,-1,chr,FilePath.GetLength()*2,NULL,NULL);

		FILE* pfile;
		if((pfile=fopen(chr,"wb"))==NULL)
		{
			AfxMessageBox(_T("创建文件失败!"));
			return;
		}
		CMyWaveView* pView=(CMyWaveView*)CView::FromHandle(m_hWndView);
		int start=pView->m_startPlayPos2;
		int end=pView->m_endPlayPos2;
		WritetoFile(header2,start,end,data_one2,m_bytesPerSample2,pfile);

		fclose(pfile);
	}
}

void CMyWaveDoc::OnFile3Open()
{
	// TODO: 在此添加命令处理程序代码
		CFileDialog dlg(true, NULL, L"", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"音频文件(*.wav)|*.wav|所有文件(*.*)|*.*||");

	if(IDOK==dlg.DoModal())
	{
		m_tempFileName=dlg.GetFileName();
		m_tempFilePath=dlg.GetPathName();

		char ch[256];
		memset(ch,0,256);
		WideCharToMultiByte(CP_OEMCP,0,m_tempFilePath,-1,ch,m_tempFilePath.GetLength()*2,NULL,NULL);

		FILE* pfile;
		if((pfile=fopen(ch,"rb"))==NULL)
		{
			AfxMessageBox(_T("读取文件失败!"));
			return;
		}

		ReadFromFile(header3,data_one3,data_left3,data_right3,m_channels3,m_HZ3,m_time3,m_bytesPerSample3,m_extent3,m_numOfPoint3,
									 maxForOneData3,minForOneData3,maxForLeftData3,minForLeftData3,
									 maxForRightData3,minForRightData3,m_strFileName3,m_strFilePath3,pfile);
		
		fclose(pfile);

		int max_extent=m_extent;
		if(max_extent<m_extent2)
			max_extent=m_extent2;
		if(max_extent<m_extent3)
			max_extent=m_extent3;

		int max_numOfPoint=m_numOfPoint;
		if(max_numOfPoint<m_numOfPoint2)
			max_numOfPoint=m_numOfPoint2;
		if(max_numOfPoint<m_numOfPoint3)
			max_numOfPoint=m_numOfPoint3;

		CMyWaveView* pView=(CMyWaveView*)CView::FromHandle(m_hWndView);
		pView->m_extent=(max_extent/100+1)*100;
		pView->m_numOfPoint=max_numOfPoint;
		pView->Invalidate();

		COperateDlg* view=(COperateDlg*)CView::FromHandle(m_hWndCtrl);
		SCROLLINFO x_info;
		x_info.cbSize=sizeof(SCROLLINFO);
		x_info.fMask=SIF_ALL;
		x_info.nMax=max_numOfPoint;
		x_info.nMin=0;
		x_info.nPage=60;
		x_info.nPos=0;
		x_info.nTrackPos=0;
		view->m_X_ShowPos.SetScrollInfo(&x_info);
		view->UpdateFileInfoCtrl(header,header2,header3);
	}
}

void CMyWaveDoc::OnFile3SaveAs()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(false, NULL, L"", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"音频文件(*.wav)|*.wav|所有文件(*.*)|*.*||");

	if(IDOK==dlg.DoModal())
	{
		CString FileName=dlg.GetFileName();
		CString FilePath=dlg.GetPathName();
		FilePath+=_T(".wav");

		char chr[256];
		memset(chr,0,256);
		WideCharToMultiByte(CP_OEMCP,0,FilePath,-1,chr,FilePath.GetLength()*2,NULL,NULL);

		FILE* pfile;
		if((pfile=fopen(chr,"wb"))==NULL)
		{
			AfxMessageBox(_T("创建文件失败!"));
			return;
		}
		CMyWaveView* pView=(CMyWaveView*)CView::FromHandle(m_hWndView);
		//int start=pView->m_startPlayPos3;
		//int end=pView->m_endPlayPos3;
		int start=0;
		int end=data_one.size()+data_one2.size()+data_one3.size()-1;
		int i;
		for( i=0;i<data_one.size();i++)
			data_one3.push_back(data_one[i]);
		for( i=0;i<data_one2.size();i++)
			data_one3.push_back(data_one2[i]);
		//data_one3.push_back(data_one+data_one2+data_one3);
		WritetoFile(header3,start,end,data_one3,m_bytesPerSample3,pfile);
		fclose(pfile);
	}
}

void CMyWaveDoc::SetFrameLen(int x)
{
	frame_len=x;
}

void CMyWaveDoc::Pitch()
{
	if(data_one.size()!=0)
	{
		CString name;
		CString name1,name2;
		CString info;
		int n_frame=0;
		std::ifstream ifs;
		std::vector<float> vec;

		ifft_one.clear();
		id_max.clear();
		id_last.clear();
		f1.clear();
		p1.clear();
		f2.clear();
		p2.clear();
		u.clear();
		threshold.clear();
		k1.clear();
		k2.clear();
		divide.clear();
		pitch.clear();
		tag.clear();


		int len1=(int)data_one.size();
		//初始设帧长为frame_len=512		
		n_frame=(len1-frame_len)/(frame_len/2)+1;
		std::vector<float> temp;
		temp.clear();
		for(int i=0;i<len1;i++)
		{
			temp.push_back(data_one.at(i));
		}

		Methods methods1;

		methods1.QIFFT(len1,frame_len,frame_len/2,n_frame,temp,frame_len,ifft_one);





		std::vector<int> vec_id_max;//最大峰值能力团的下标id
		std::vector<int> vec_id_last;//最大峰值能力团的下标id

		Methods method;
		int sum=0;//保存选中帧的数量

		if(n_frame!=0){
			for(int i=0;i<n_frame;i++)
			{
				//初始化
				vec_id_max.clear();
				vec_id_last.clear();


				f1.push_back(0);
				f2.push_back(0);
				f3.push_back(0);
				p1.push_back(0);
				p2.push_back(0);
				p3.push_back(0);
				u.push_back(0);
				threshold.push_back(0);
				k1.push_back(0);
				k2.push_back(0);
				divide.push_back(0);
				pitch.push_back(0);
				tag.push_back(0);

				method.GetPitch(frame_len,ifft_one.at(i),vec_id_max,vec_id_last,f1[i],f2[i],f3[i],p1[i],p2[i],p3[i],u[i],threshold[i],k1[i],k2[i],divide[i],pitch[i],tag[i]);

				id_max.push_back(vec_id_max);
				id_last.push_back(vec_id_last);

				if(tag[i]==1)
				{
					sum++;
				}
			}


			for(int i=0;i<(int)ifft_one.size();i++)
			{
				for(int j=0;j<(int)ifft_one[i].size();j++)
				{
					ifft_one[i][j]*=10000;
				}
				ifft_one[i][0]=0;
			}


			maxForOneIfft=minForOneIfft=ifft_one[0][0];

			for(int i=0;i<(int)ifft_one.size();i++)
			{
				for(int j=0;j<(int)ifft_one[i].size();j++)
				{
					if(ifft_one[i].at(j)>maxForOneIfft)
						maxForOneIfft=ifft_one[i].at(j);
					if(ifft_one[i].at(j)<minForOneIfft)
						minForOneIfft=ifft_one[i].at(j);
				}
			}

		}
	}

}

void CMyWaveDoc::Pitch2()
{
	if(data_one2.size()!=0)
	{
		CString name;
		CString name1,name2;
		CString info;
		int n_frame=0;
		std::ifstream ifs;
		std::vector<float> vec;

		ifft_two.clear();
		id_max_2.clear();
		id_last_2.clear();
		f1_2.clear();
		p1_2.clear();
		f2_2.clear();
		p2_2.clear();
		u_2.clear();
		threshold_2.clear();
		k1_2.clear();
		k2_2.clear();
		divide_2.clear();
		pitch_2.clear();
		tag_2.clear();


		int len1=(int)data_one2.size();
		//初始设帧长为frame_len=512		
		n_frame=(len1-frame_len)/(frame_len/2)+1;
		std::vector<float> temp;
		temp.clear();
		for(int i=0;i<len1;i++)
		{
			temp.push_back(data_one2.at(i));
		}

		Methods methods1;

		methods1.QIFFT(len1,frame_len,frame_len/2,n_frame,temp,frame_len,ifft_two);





		std::vector<int> vec_id_max;//最大峰值能力团的下标id
		std::vector<int> vec_id_last;//最大峰值能力团的下标id

		Methods method;
		int sum=0;//保存选中帧的数量

		if(n_frame!=0){
			for(int i=0;i<n_frame;i++)
			{
				//初始化
				vec_id_max.clear();
				vec_id_last.clear();


				f1_2.push_back(0);
				f2_2.push_back(0);
				f3_2.push_back(0);
				p1_2.push_back(0);
				p2_2.push_back(0);
				p3_2.push_back(0);
				u_2.push_back(0);
				threshold_2.push_back(0);
				k1_2.push_back(0);
				k2_2.push_back(0);
				divide_2.push_back(0);
				pitch_2.push_back(0);
				tag_2.push_back(0);

				method.GetPitch(frame_len,ifft_two.at(i),vec_id_max,vec_id_last,f1_2[i],f2_2[i],f3_2[i],p1_2[i],p2_2[i],p3_2[i],u_2[i],threshold_2[i],k1_2[i],k2_2[i],divide_2[i],pitch_2[i],tag_2[i]);

				id_max_2.push_back(vec_id_max);
				id_last_2.push_back(vec_id_last);

				if(tag_2[i]==1)
				{
					sum++;
				}
			}


			for(int i=0;i<(int)ifft_two.size();i++)
			{
				for(int j=0;j<(int)ifft_two[i].size();j++)
				{
					ifft_two[i][j]*=10000;
				}
				ifft_two[i][0]=0;
			}


			maxForTwoIfft=minForTwoIfft=ifft_two[0][0];

			for(int i=0;i<(int)ifft_two.size();i++)
			{
				for(int j=0;j<(int)ifft_two[i].size();j++)
				{
					if(ifft_two[i].at(j)>maxForTwoIfft)
						maxForTwoIfft=ifft_two[i].at(j);
					if(ifft_two[i].at(j)<minForTwoIfft)
						minForTwoIfft=ifft_two[i].at(j);
				}
			}

		}
	}

}