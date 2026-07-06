#include "MFCStub.h"
//GMM寤烘ā鏂板姞鏂囦欢锛岀煩闃电被
#ifndef MATRIX_H
#define MATRIX_H

#include <QString>
#include <QVector>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include <iostream>
#include <fstream>
#include"Macros.h"
#include"Vector.h"
using namespace std;
#ifdef USE_T_EXTENSIONS
template <unsigned int ROW> class TMatrix;
#endif

class Matrix
{
	friend class Vector;
#ifdef USE_T_EXTENSIONS
template <unsigned int ROW> class TMatrix;
#endif
protected:
	static int bInverseOk;

	unsigned int row;
	unsigned int column;
	float    *_;
	
public:
	inline Matrix()//鏋勯犲嚱鏁
	{
		row    = 0;
		column = 0;
		_      = NULL;
	}
	inline virtual ~Matrix()//鏋愭瀯鍑芥暟
	{
		Release();
	}

	inline Matrix(const Matrix &matrix)//鐢ㄥ凡鐭ュ硅薄鍒濆嬪綋鍓嶅硅薄
	{
		row     =0;
		column  =0;
		_       =NULL;
		Resize(matrix.row ,matrix.column,false);
		for(unsigned int i=0;i<row;i++)
			for(unsigned int j=0;j<column;j++)
				_[i*column+j]=matrix._ [i*column+j];
	}
	 inline Matrix(unsigned int rowSize,unsigned int colSize,bool clear=true)
	 {
		 row     =0;
		 column  =0;
		 _       =NULL;
		 Resize(rowSize,colSize,false);
		 if(clear)
			 Zero();
	 }
	 inline Matrix(const float _[],unsigned int rowSize, unsigned int colSize)
	 {
		 row       =0;
		 column    =0;
		 _         =NULL;
		 Resize(rowSize,colSize,false);
		 for(unsigned int j=0;j<rowSize;j++)
			 for(unsigned int i=0;i<colSize;i++)
				this->_[j*column+i]=_[j*column+i];
	 }

#ifdef USE_T_EXTENSIONS
	 template<unsigned int ROW>inline Matrix(const TMatrix<ROW>& matrix)
	 {
		 row    =0;
		 column =0;
		 _      =NULL;
		 Resize(ROW,ROW,false);
		 for(unsigned int i=0;i<row;i++)
			for(unsigned int j=0;j<column;j++)
				_[i*column+j]=matrix._ [i*column+j];
	 }
#endif

	 inline Matrix& Zero()//鐭╅樀璧嬪间负闆剁煩闃
	 {
		  for(unsigned int j=0;j<row;j++)
			 for(unsigned int i=0;i<column;i++)
				_[j*column+i]=0.0f;
		  return *this;
	 }
	 inline unsigned int RowSize ()const//杩斿洖鐭╅樀鐨勮屾暟
	 {
		 return row;
	 }
	 inline unsigned int ColumnSize ()const//杩斿洖鐭╅樀鐨勫垪鏁
	 {
		 return column;
	 }
	 inline float *Array()const//杩斿洖鐭╅樀鏁版嵁
	 {
		 return _;
	 }	 
	 inline float& operator() (const unsigned int row,const unsigned int col)//杩斿洖鎸囧畾浣嶇疆鐨勬暟鎹
	 {
		 if((row<this->row)&&(col<this->column))
			 return _[row*column+col];
		 return Vector::undef;
	 }
	 inline Vector GetRow(const unsigned int row)const//杩斿洖鎸囧畾琛岀殑鏁版嵁,涓轰竴涓猇ector
	 {
		 Vector result(column,false);
		 return GetRow(row,result);
	 }
	 inline Vector& GetRow(const unsigned int row,Vector& result)const
	 {
		 result.Resize(column,false);
		 for(unsigned int i=0;i<column;i++)
			 result._[i]=_[row*column+i];
		 return result;
	 }

	 inline Vector GetColumn(const unsigned int col)const//杩斿洖鎸囧畾鍒楃殑鏁版嵁
	 {
		 Vector result(row,false);
		 return GetRow(col,result);
	 }
	 inline Vector& GetColumn(const unsigned int col,Vector& result)const
	 {
		 result.Resize(row,false);
		 if(col<column)
		 {
			 for(unsigned int i=0;i<row;i++)
				 result._[i]=_[i*column+col];
		 }
		 else
			 result.Zero();
		 return result;
	 }
	 inline Matrix GetColumnSpace(const unsigned int col,const unsigned int len)const//杩斿洖浠庢寚瀹氬垪col寮濮嬬殑鍏眑en鍒椼佽屾暟涓簉ow鐨勬暟鎹鐭╅樀
	 {
		 if(len>0)
		 {
			 Matrix result(row,len,false);
			 return GetColumnSpace(col,len,result);
		 }
		 else
			 return Matrix();
	 }
	 inline Matrix& GetColumnSpace(const unsigned int col,const unsigned int len,Matrix& result)const
	 {
		 if(len>0)
		 {
			 const unsigned int end=col+len-1;//璁＄畻鍒板摢鍒椾负姝
			 const unsigned int size=len;//鑾峰彇鏁版嵁鐨勫垪鏁
			 result.Resize(row,size,false);

			 if(col<column)
			 {
				 const unsigned int k=(end+1<=column ?end+1:column);//瀹為檯缁撴潫鍒楃殑鏍囪瘑

				 for(unsigned int i=col;i<k;i++)
					 for(unsigned int j=0;j<row;j++)
						 result._[j*size+(i-col)]=_[j*column+i];
					 for(unsigned int i=k;i<end+1;i++)
						 for(unsigned int j=0;j<row;j++)
							 result._[j*size+(i-col)]=0.0f;
			 }
			 else
				 result.Zero();
		 }
		 else
			 result.Resize(0,0,false);
		 return result;
	 }

	 inline Matrix GetRowSpace(const unsigned int row,const unsigned int len)const//杩斿洖浠庢寚瀹氳宺ow寮濮嬬殑鍏眑en琛屻佸垪鏁颁负column鐨勬暟鎹鐭╅樀
	 {
		 if(len>0)
		 {
			 Matrix result(len,column,false);
			 return GetRowSpace(row,len,result);
		 }
		 else
			 return Matrix();
	 }
	 inline Matrix& GetRowSpace(const unsigned int row,const unsigned int len,Matrix & result)const
	 {
		 if(len>0)
		 {
			 const unsigned int end=row+len-1;
			 const unsigned int size=len;
			 result.Resize(size,column,false);

			 if(row<this->row )
			 {
				 const unsigned int k=(end+1<=this->row ? end+1:this->row);
				  
				 for(unsigned int i=row;i<k;i++)
					 for(unsigned int j=0;j<column;i++)
						 result._[(i-row)*column+j]=_[i*column+j];
				 for(unsigned int i=k;i<end+1;i++)
					 for(unsigned int j=0;j<column;i++)
						 result._[(i-row)*column+j]=0.0f;
			 }
			 else
				 result.Zero();		
		 } 
		 else
			 result.Resize(0,0,false);
		 return result;		 
	 }

	 inline Matrix& SetRow(const Vector& vector,const unsigned int row)//鐢ㄧ粰瀹歷ector瀵硅薄鐨勫兼浛鎹㈡寚瀹氳岀殑鏁版嵁
	 {
		 if(row<this->row)
		 {
			 unsigned int ki=(column<=vector.row? column:vector.row);
			 for(unsigned int i=0;i<ki;i++)
				 _[row*column+i]=vector._[i];
		 }
		 return *this;
	 }
     inline Matrix& SetColumn(const Vector& vector,const unsigned int col)//鐢ㄧ粰瀹歷ector瀵硅薄鐨勫兼浛鎹㈡寚瀹氬垪鐨勬暟鎹
	 {
		 if(col<this->column)
		 {
			 unsigned int kj=(row<=vector.row? row:vector.row);
			 for(unsigned int j=0;j<kj;j++)
				 _[j*column+col]=vector._[j];
		 }
		 return *this;
	 }

	 inline Matrix& SetColumnSpace(const Matrix &matrix, const unsigned int col)//缁欏畾涓涓狹atrix鐭╅樀锛屾浛鎹㈠綋鍓嶇煩闃典腑缁欏畾鍒梒ol寮濮嬬殑鏁版嵁
	 {
		 if(col<this->column)
		 {
			 const unsigned int kj=(row<=matrix.row? row:matrix.row);
			 const unsigned int ki=(col+matrix.column<=this->column ? col+matrix.column:this->column);

			 for(unsigned int j=0;j<kj;j++)
				 for(unsigned int i=col;i<ki;i++)
					 _[j*column+i]=matrix._[j*matrix.column+(i-col)];
		 }
		 return *this;
	 }

	 inline Matrix& SetRowSpace(const Matrix& matrix,const unsigned int row)//缁欏畾涓涓狹atrix鐭╅樀锛屾浛鎹㈠綋鍓嶇煩闃典腑缁欏畾琛宺ow寮濮嬬殑鏁版嵁
	 {
		 if(row<this->row)
		 {
			 const unsigned int ki=(row+matrix.row<=this->row ?row+matrix.row :this->row);
			 const unsigned int kj=(matrix.column<=column ? matrix.column :column);

			 for(unsigned int i=row;i<ki;i++)
				 for(unsigned int j=0;j<kj;j++)
					 _[i*column+j]=matrix._[(i-row)*matrix.column+j];
		 }
		 return *this;
	 }

	 inline Matrix GetRowSpace(const Vector &ids)const//鐢╲ector瀵硅薄鎸囨槑瑕佸彇鐭╅樀涓鏁版嵁鐨勬昏屾暟璺熷埌搴曞彇鍝琛
	 {
		 Matrix result(ids.Size(),column);
		 return GetRowSpace(ids,result);
	 }
	 inline Matrix& GetRowSpace(const Vector &ids,Matrix &result)const
	 {
		 const unsigned int k=ids.Size();//k浠ｈ〃涓鍏辫佸彇澶氬皯琛
		 result.Resize(k,column);

		 for(unsigned int i=0;i<k;i++)
		 {
			 const unsigned int g=(unsigned int)(fabs(ROUND(ids._[i])));
			 if(g<row)//濡傛灉g灏忎簬row,鍙栫琯琛岀殑鏁版嵁
			 {
				 for(unsigned int j=0;j<column;j++)
				 result._ [i*column+j]=_[g*column+j];
			 }
			 else
				 for(unsigned int j=0;j<column;j++)
				     result._ [i*column+j]=0.0f;
		 }
		 return result;
	 }

	 inline Matrix GetColumnSpace(const Vector &ids)const//鐢╲ector瀵硅薄鎸囨槑瑕佸彇鐭╅樀涓鏁版嵁鐨勬诲垪鏁拌窡鍒板簳鍙栧摢鍒
	 {
		 Matrix result(row,ids.Size());
		 return GetColumnSpace(ids,result);
	 }
	 inline Matrix& GetColumnSpace(const Vector &ids,Matrix &result)const
	 {
		 const unsigned int k=ids.Size();//k浠ｈ〃涓鍏辫佸彇澶氬皯鍒
		 result.Resize(row,k);

		 for(unsigned int i=0;i<k;i++)
		 {
			 const unsigned int g=(unsigned int)(fabs(ROUND(ids._[i])));
			 if(g<column)//濡傛灉g灏忎簬column,鍙栫琯鍒楃殑鏁版嵁
			 {
				 for(unsigned int j=0;j<row;j++)
				 result._ [j*k+i]=_[j*column+g];
			 }
			 else
				 for(unsigned int j=0;j<row;j++)
				     result._ [j*k+i]=0.0f;
		 }
		 return result;
	 }

	 inline Matrix GetMatrixSpace(const Vector &rowIds,const Vector &colIds)const//rowIds纭瀹氬彇澶氬皯琛屽彇鍝琛岋紝colIds纭瀹氬彇澶氬皯鍒楀彇鍝鍒
	 {
		 Matrix result(rowIds.Size(),colIds.Size());
		 return GetMatrixSpace(rowIds,colIds,result);
	 }
	 inline Matrix& GetMatrixSpace(const Vector &rowIds,const Vector &colIds,Matrix &result)const
	 {
		 unsigned int k1=rowIds.Size();//鎬诲叡鍙栧氬皯琛
		 unsigned int k2=colIds.Size();//鎬诲叡鍙栧氬皯鍒
		 result.Resize(k1,k2);

		 for(unsigned int i=0;i<k1;i++)
		 {
			 const unsigned int g1=(unsigned int)(fabs(ROUND(rowIds._[i])));
			 if(g1<row)
			 {
				 for(unsigned int j=0;j<k2;j++)
				 {
					 unsigned int g2=(unsigned int )(fabs(ROUND(colIds.Size())));
					 if(g2<column)
					 {
						 result._[i*k2+j]=_[g1*column+g2];
					 }
					 else
						 result._[i*k2+j]=0.0f;
				 }
			 }
			 else
				 for(unsigned int j=0;j<k2;j++)
					 result._[i*k2+j]=0.0f;
		 }
		 return result;
	 }
///////////////////////////////////////////////////////////////////杩愮畻绗﹂噸杞
	 inline Matrix operator -()const//褰撳墠鐭╅樀鍏冪礌鍘荤浉鍙嶆暟-------------------閲嶈浇鍙栧弽鎿嶄綔绗 "-"
	 {
		 Matrix result(row,column,false);
		 for(unsigned int i=0;i<row;i++)
			 for(unsigned int j=0;j<column;j++)
				 result._[i*column+j]=_[i*column+j];
		 return result;
	 }

	 inline virtual Matrix& operator =(const Matrix & matrix)//---------鐭╅樀涔嬮棿--------閲嶈浇璧嬪兼搷浣滅 "="
	 {
		 Resize(matrix.row,matrix.column,false);
		 for(unsigned int i=0;i<row;i++)
			 for(unsigned int j=0;j<column;j++)
				 _[i*column+j]=matrix._[i*column+j];
		 return *this;
	 }
	  inline  Matrix& operator +=(const Matrix & matrix)//---------鐭╅樀涔嬮棿--------閲嶈浇鎿嶄綔绗 "+="
	  {
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
			  for(unsigned int j=0;j<kj;j++)
				  _[i*column+j]+=matrix._[i*column+j];
		  return *this;
	  }

	  inline virtual Matrix& operator -=(const Matrix & matrix)//---------鐭╅樀涔嬮棿--------閲嶈浇鎿嶄綔绗 "-="
	  {
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
			  for(unsigned int j=0;j<kj;j++)
				  _[i*column+j]-=matrix._[i*column+j];
		  return *this;
	  }
	  inline virtual Matrix& operator ^=(const Matrix & matrix)//----------鐭╅樀涔嬮棿-------閲嶈浇鎿嶄綔绗 "^="
	  {
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
			  for(unsigned int j=0;j<kj;j++)
				  _[i*column+j]*=matrix._[i*column+j];
		  return *this;
	  }
	  inline virtual Matrix& operator /=(const Matrix & matrix)//---------鐭╅樀涔嬮棿--------閲嶈浇鎿嶄綔绗 "/="
	  {
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
			  for(unsigned int j=0;j<kj;j++)
				  _[i*column+j]/=matrix._[i*column+j];
		  return *this;
	  }
	  inline Matrix& operator +=(float scalar)//---------------鐭╅樀涓庢暟鍊--------閲嶈浇杩愮畻绗"+="
	  {
		   for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  _[i*column+j]+=scalar;
		   return *this;
	  }
	  inline Matrix& operator -=(float scalar)//---------------鐭╅樀涓庢暟鍊--------閲嶈浇杩愮畻绗"-="
	  {
		   for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  _[i*column+j]-=scalar;
		   return *this;
	  }
	  inline Matrix& operator *=(float scalar)//---------------鐭╅樀涓庢暟鍊--------閲嶈浇杩愮畻绗"*="
	  {
		   for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  _[i*column+j]*=scalar;
		   return *this;
	  }
	  inline Matrix& operator /=(float scalar)//---------------鐭╅樀涓庢暟鍊--------閲嶈浇杩愮畻绗"/="
	  {
		  scalar=1.0f/scalar;
		   for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  _[i*column+j]*=scalar;
		   return *this;
	  }
	  inline Matrix operator +(const Matrix &matrix)const//涓や釜鐭╅樀瀵瑰簲鍏冪礌鐩稿姞锛岀粨鏋滅煩闃佃窡鍓嶄竴涓鐭╅樀鐨勮屾暟鍒楁暟鐩稿悓---鐭╅樀涔嬮棿----閲嶈浇杩愮畻绗"+"
	  {
		  Matrix result(row,column,false);
		  return Add(matrix,result);
	  }
	  inline Matrix& Add(const Matrix& matrix,Matrix& result)const
	  {
		  result.Resize(row,column,false);
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
		  {
			  for(unsigned int j=0;j<kj;j++)
				  result._[i*column+j]=_[i*column+j]+matrix._[i*column+j];
			  for(unsigned int j=kj;j<column;j++)
				  result._[i*column+j]=_[i*column+j];
		  }
		  for(unsigned int i=ki;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  result._[i*column+j]=_[i*column+j];
		  return result;
	  }
	  inline Matrix operator -(const Matrix &matrix)const//涓や釜鐭╅樀瀵瑰簲鍏冪礌鐩稿噺锛岀粨鏋滅煩闃佃窡鍓嶄竴涓鐭╅樀鐨勮屾暟鍒楁暟鐩稿悓---鐭╅樀涔嬮棿----閲嶈浇杩愮畻绗"-"
	  {
		  Matrix result(row,column,false);
		  return Sub(matrix,result);
	  }
	  inline Matrix& Sub(const Matrix& matrix,Matrix& result)const
	  {
		  result.Resize(row,column,false);
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
		  {
			  for(unsigned int j=0;j<kj;j++)
				  result._[i*column+j]=_[i*column+j]-matrix._[i*column+j];
			  for(unsigned int j=kj;j<column;j++)
				  result._[i*column+j]=_[i*column+j];
		  }
		  for(unsigned int i=ki;i<row;i++)
			  for(unsigned int j=0;j<column;i++)
				  result._[i*column+j]=_[i*column+j];
		  return result;
	  }
	  inline Matrix operator ^(const Matrix &matrix)const//涓や釜鐭╅樀瀵瑰簲鍏冪礌鐩镐箻锛岀粨鏋滅煩闃佃窡鍓嶄竴涓鐭╅樀鐨勮屾暟鍒楁暟鐩稿悓---鐭╅樀涔嬮棿----閲嶈浇杩愮畻绗"^"
	  {
		  Matrix result(row,column,false);
		  return PMult(matrix,result);
	  }
	  inline Matrix& PMult(const Matrix& matrix,Matrix& result)const
	  {
		  result.Resize(row,column,false);
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
		  {
			  for(unsigned int j=0;j<kj;j++)
				  result._[i*column+j]=_[i*column+j]*matrix._[i*column+j];
			  for(unsigned int j=kj;j<column;j++)
				  result._[i*column+j]=_[i*column+j];
		  }
		  for(unsigned int i=ki;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  result._[i*column+j]=_[i*column+j];
		  return result;
	  }
	  inline Matrix operator /(const Matrix &matrix)const//涓や釜鐭╅樀瀵瑰簲鍏冪礌鐩搁櫎锛岀粨鏋滅煩闃佃窡鍓嶄竴涓鐭╅樀鐨勮屾暟鍒楁暟鐩稿悓---鐭╅樀涔嬮棿----閲嶈浇杩愮畻绗"/"
	  {
		  Matrix result(row,column,false);
		  return PDiv(matrix,result);
	  }
	  inline Matrix& PDiv(const Matrix& matrix,Matrix& result)const
	  {
		  result.Resize(row,column,false);
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
		  {
			  for(unsigned int j=0;j<kj;j++)
				  result._[i*column+j]=_[i*column+j]/matrix._[i*column+j];
			  for(unsigned int j=kj;j<column;j++)
				  result._[i*column+j]=_[i*column+j];
		  }
		  for(unsigned int i=ki;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  result._[i*column+j]=_[i*column+j];
		  return result;
	  }


	  inline Matrix operator +(float scalar)const//鐭╅樀姣忎釜鍏冪礌閮藉姞涓涓鏁------------鐭╅樀鍜屾暟----閲嶈浇杩愮畻绗"+"
	  {
		  Matrix result(row,column,false);
		  return Add(scalar,result);
	  }
	  inline Matrix& Add(float scalar,Matrix & result)const
	  {
		  result.Resize(row,column,false);
		  for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  result._[i*column+j]=_[i*column+j]+scalar;
		  return result;
	  }
	  inline Matrix operator -(float scalar)const//鐭╅樀姣忎釜鍏冪礌閮藉噺涓涓鏁------------鐭╅樀鍜屾暟----閲嶈浇杩愮畻绗"-"
	  {
		  Matrix result(row,column,false);
		  return Sub(scalar,result);
	  }
	  inline Matrix& Sub(float scalar,Matrix & result)const
	  {
		  result.Resize(row,column,false);
		  for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;i++)
				  result._[i*column+j]=_[i*column+j]-scalar;
		  return result;
	  }
	  inline Matrix operator *(float scalar)const//鐭╅樀姣忎釜鍏冪礌閮戒箻涓涓鏁------------鐭╅樀鍜屾暟----閲嶈浇杩愮畻绗"*"
	  {
		  Matrix result(row,column,false);
		  return Mult(scalar,result);
	  }
	  inline Matrix& Mult(float scalar,Matrix & result)const
	  {
		  result.Resize(row,column,false);
		  for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  result._[i*column+j]=_[i*column+j]*scalar;
		  return result;
	  }
  	  inline Matrix operator /(float scalar)const//鐭╅樀姣忎釜鍏冪礌閮介櫎涓涓鏁------------鐭╅樀鍜屾暟----閲嶈浇杩愮畻绗"/"
	  {
		  Matrix result(row,column,false);
		  return Div(scalar,result);
	  }
	  inline Matrix& Div(float scalar,Matrix & result)const
	  {
		  scalar=1.0f/scalar;
		  result.Resize(row,column,false);
		  for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  result._[i*column+j]=_[i*column+j]*scalar;
		  return result;
	  }

	  inline bool operator ==(const Matrix& matrix)const//閲嶈浇杩愮畻绗"=="
	  {
		  if((row!=matrix.row) ||(column!=matrix.column))
			  return false;
		  for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  if(_[i*column+j]!=matrix._[i*column+j])
					  return false;
		  return true;
	  }
	  inline bool operator !=(Matrix& matrix)const//閲嶈浇杩愮畻绗"!="
	  {
		  return !(*this==matrix);
	  }
	  
	  inline Vector operator *(const Vector &vector)const//鐭╅樀涓庣煝閲------閲嶈浇杩愮畻绗"*"
	  {
		  Vector result(row,false);
		  return Mult(vector,result);
	  }
	  inline Vector Mult(const Vector &vector)const
	  {
		  Vector result(row,false);
		  return Mult(vector,result);
	  }
	  inline Vector& Mult(const Vector &vector, Vector &result)const
	  {
		  result.Resize(row,false);
		  const unsigned int kj=(column<=vector.row ?column:vector.row);
		  for(unsigned int i=0;i<row;i++)
		  {
			  result._[i]=0.0f;
			  for(unsigned int j=0;j<kj;j++)
				  result._[i]+=_[i*column+j]*vector._[j];
		  }
		  return result;
	  }
	  inline Matrix operator *(const Matrix &matrix)const//鐭╅樀涓庣煩闃------閲嶈浇杩愮畻绗"*"
	  {
		  Matrix result(row,matrix.column,false);
		  return Mult(matrix,result);
	  }
	  inline Matrix &Mult(const Matrix &matrix, Matrix &result)const
	  {
		  result.Resize(row,matrix.column,false);
		  const unsigned int rrow=result.row;
		  const unsigned int rcol=result.column;
		  const unsigned int kk=(column<=matrix.row ?column :matrix.row);
		  for(unsigned int j=0;j<rrow;j++)
		  {
			  for(unsigned int i=0;i<rcol;i++)
			  {
				  result._[j*rcol+i]=0.0f;
				  for(unsigned int k=0;k<kk;k++)
					  result._[j*rcol+i]+=_[j*column+k]*matrix._[k*rcol+i];
			  }
		  }
		  return result;
	  }
	  inline Matrix& Identity()//鐭╅樀瀵硅掔嚎涓婁负1,鍗曚綅鐭╅樀
	  {
		  const unsigned int k=(row>column ?column:row);
		  Zero();
		  for(unsigned int i=0;i<k;i++)
			  _[i*column+i]=1.0f;
		  return *this;
	  }

	  inline Matrix& Diag(const Vector &vector)//瀵硅掔煩闃
	  {
		  const unsigned int k=(row>column? column:row);
		  const unsigned int k2=(k>vector.row ? vector.row:k);//k2=min(min(row,column),vector.row)

		  Zero();
		  for(unsigned int i=0;i<k2;i++)
			  _[i*column+i]=vector._[i];
		  return *this;
	  }

	  inline Matrix& Random()//闅忔満鐢熸垚鐭╅樀鐨勬暟鍊
	  {
		  for(unsigned int j=0;j<row;j++)
			  for(unsigned int i=0;i<column;i++)
				  _[j*column+i]=((float)rand())/((float)(RAND_MAX+1.0));
		  return *this;
	  }
      
	  inline Matrix Transpose()const//姹傜煩闃电殑杞缃鐭╅樀
	  {
		  Matrix result(row,column,false);
		  return Transpose(result);        
	  }
	  inline Matrix& Transpose(Matrix &result)const
	  {
		  result.Resize(column,row,false);
		  for(unsigned int j=0;j<row;j++)
			  for(unsigned int i=0;i<column;i++)
				  result._[i*row+j]=_[j*column+i];
		  return result;
	  }

	  inline Matrix VCat(const Matrix& matrix)//鐭╅樀绔栫洿鏂瑰悜杩炴帴
	  {
		  Matrix result;
		  return VCat(matrix,result);
	  }
	  inline Matrix& VCat(const Matrix& matrix,Matrix& result)
	  {
		  unsigned int k1=(column> matrix.column ?column:matrix.column);
		  result.Resize(row+matrix.row,k1,false);
		  for(unsigned int j=0;j<row;j++)
		  {
			  for(unsigned int i=0;i<column;i++)
				  result._[j*k1+i]=_[j*column+i];
			  for(unsigned int i=column;i<k1;i++)
				  result._[j*k1+i]=0.0f;
		  }
		  for(unsigned int j=0;j<matrix.row;j++)
		  {
			  for(unsigned int i=0;i<matrix.column;i++)
				  result._[(row+j)*k1+i]=matrix._[j*matrix.column+i];
			  for(unsigned int i=matrix.column;i<k1;i++)
				  result._[(row+j)*k1+i]=0.0f;
		  }
		  return result;
	  }

	  inline Matrix HCat(const Matrix& matrix)//鐭╅樀姘村钩鏂瑰悜杩炴帴
	  {
		  Matrix result;
		  return HCat(matrix,result);
	  }
	  inline Matrix& HCat(const Matrix&matrix,Matrix& result)
	  {
		  unsigned int k1=(row>matrix.row? row:matrix.row);
		  unsigned int k2=column+matrix.column;
		  result.Resize(k1,k2,false);

		  for(unsigned int j=0;j<row;j++)
			  for(unsigned int i=0;i<column;i++)
				  result._[j*k2+i]=_[j*column+i];
		  for(unsigned int j=row;j<k1;j++)
			  for(unsigned int i=0;i<column;i++)
				   result._[j*k2+i]=0.0f;

		  for(unsigned int j=0;j<matrix.row;j++)
			  for(unsigned int i=0;i<matrix.column;i++)
				  result._[j*k2+i+column]=matrix._[j*matrix.column+i];
		  for(unsigned int j=matrix.row;j<k1;j++)
			  for(unsigned int i=0;i<matrix.column;i++)
				   result._[j*k2+i+column]=0.0f;
		  
		  return result; 			 
	  }

	  inline static int IsInverseOk()//鍒ゆ柇鏄姹傞
	  {
		  return bInverseOk;
	  }

	  inline Matrix Inverse(float * determinant=NULL)const//鐭╅樀姹傞
	  {
		  Matrix result;
		  return Inverse(result,determinant);
	  }
	  inline Matrix& Inverse(Matrix& result,float* determinant=NULL)const
	  {
		  bInverseOk=true;
		  if(row==column)//Square matrix鏂归樀姹傞嗙煩闃
		  {
			  if(determinant!=NULL)*determinant=1.0f;
			  result.Resize(row,column,false);
			  const unsigned int n=row;
			  Matrix MM(*this);
			 /*  int column=2;
			  int row=2;
			 const unsigned int n=row;
			  Matrix MM;
			  MM.Resize(row,column,false);
			 
			  MM._[0]=1;
			  MM._[1]=2;
			  MM._[2]=3;
			  MM._[3]=4;*/ 
			 /*ofstream fout1("sigma_mywave.txt",ios::trunc);
			  for(int i=0;i<row;i++)
			  {
				  for(int j=0;j<column;j++)
				  {
					
					  fout1<<MM._[i*column+j]<<"  ";
				  }
				  fout1<<endl;

			  }
			  fout1.close();*/
			 
			  result.Identity();//n*n鐨勫崟浣嶇煩闃 
			  for(unsigned int i=0;i<n;i++)
			  {
				  float pivot=MM._[i*column+i];
				  if(fabs(pivot)<=EPSILON)
				  {
					  for(unsigned int j=i+1;j<n;j++)
					  {
						  if((pivot=MM._[j*column+i])!=0.0f)//琛屼笌琛屼氦鎹
						  {
							  MM.SwapRow(i,j);
							  result.SwapRow(i,j);
							  break;
						  }
					  }
					  if(fabs(pivot)<=EPSILON)
					  {
						  bInverseOk=false;
						  if(determinant!=NULL) *determinant=0.0f;
						  return result;
					  }
				  }
				  if(determinant!=NULL) *determinant *=pivot;
				  pivot=1.0f/pivot;
				  for(unsigned int j=0;j<n;j++)
				  {
					  MM._[i*column+j] *=pivot;
					  result._[i*column+j]*=pivot;
				  }
				  for(unsigned int k=0;k<n;k++)
				  {
					  if(k!=i)
					  {
						  const float mki=MM._[k*column+i];
						  for(unsigned int j=0;j<n;j++)//绗琸琛屽噺鍘 绗琲琛* MM._[k*column+i]
						  {
							  MM._[k*column+j] -=MM._[i*column+j] *mki;
							  result._[k*column+j]-=result._[i*column+j]*mki;
						  }
					  }
				  }

			  }

		  }
		  else
		  {
			  if(determinant!=NULL)*determinant=0.0f;
			  if(row>column)
			  {
				  Matrix MT,SQ,SQInv;
				  Transpose(MT);
				  MT.Mult(*this,SQ);
				  SQ.Inverse(SQInv);
				  SQInv.Mult(MT,result);
			  }
			  else
			  {
				  Matrix MT,SQ,SQInv;
				  Transpose(MT);
				  MT.Mult(MT,SQ);
				  SQ.Inverse(SQInv);
				  SQInv.Mult(MT,result);
			  }
		  }

		 /* ofstream fout( "Inv_sigma_mywave.txt", ios::trunc );
		  
		  for(int i=0;i<row;i++)
		  {
			  for(int j=0;j<column;j++)
				  fout<< result._[i*column+j]<<"  ";
			  fout<<endl;
		  }
		  fout.close();*/
		  return result;
	  }
     inline Matrix& SwapRow(unsigned int j1,unsigned int j2)//浜ゆ崲涓よ屾暟鎹
	 {
		 if((j1<row)&&(j2<row))
		 {
			 float tmp;
			 for(unsigned int i=0;i<column;i++)
			 {
				 tmp            =_[j1*column+i];
				 _[j1*column+i] =_[j2*column+i];
				 _[j2*column+i] =tmp;
			 }
		 }
		 return *this;
	 }
	 inline Matrix& SwapColumn(unsigned int i1,unsigned int i2)//浜ゆ崲涓ゅ垪鏁版嵁
	 {
		 if((i1<column)&&(i2<column))
		 {
			 float tmp;
			 for(unsigned int j=0;j<row;j++)
			 {
				 tmp            =_[j*column+i1];
				 _[j*column+i1] =_[j*column+i2];
				 _[j*column+i2] =tmp;
			 }
		 }
		 return *this;
	 }

  void Print() const
  {
    std::cout << "Matrix " <<row<<"x"<<column<<std::endl;;
    for (unsigned int j = 0; j < row; j++){
      for (unsigned int i = 0; i < column; i++)
        std::cout << _[j*column+i] <<" ";
      std::cout << std::endl;
    }
  }


/////////////////////////////////////////////////////////////
  //涓閮ㄥ垎娌℃湁瀹炵幇鐨勫嚱鏁帮紝杩樻病鏈夌敤鍒
  Vector SumRow(){
    Vector result(column);
    return SumRow(result);
  }
  Vector & SumRow(Vector & result){
    result.Resize(column,false);
    result.Zero();
    for(unsigned int i=0;i<column;i++){
      for(unsigned int j=0;j<row;j++){
        result._[i] += _[j*column+i];
      }      
    }
    return result;  
  }

  Vector SumColumn(){
    Vector result(row);
    return SumColumn(result);
  }
  Vector & SumColumn(Vector & result){
    result.Resize(row,false);
    result.Zero();
    for(unsigned int j=0;j<row;j++){
      for(unsigned int i=0;i<column;i++){
        result._[j] += _[j*column+i];
      }      
    }
    return result;  
  }
  

	  ////////////////////////////////////////////////////
protected:
	inline void Release()
	{
		if(_!=NULL)
			delete []_;
		row=0;
		column=0;
		_     =NULL;
	}
public:
	inline virtual void Resize(unsigned int rowSize,unsigned int colSize,bool copy=true)
	{
		if((row!=rowSize) ||(column!=colSize))
		{
			if((colSize)&&(colSize))
			{
				float *arr=new float [rowSize*colSize];
				if(copy)
				{
					unsigned int mj=(row<rowSize ? row:rowSize);
					unsigned int mi=(column<colSize? column
						:colSize);

					for(unsigned int j=0;j<mj;j++)
					{
						for(unsigned int i=0;i<mi;i++)
							arr[j*colSize+i]=_[j*column+i];
						for(unsigned int i=mi;i<colSize;i++)
							arr[j*colSize+i]=0.0f;
					}
					for(unsigned int j=mj;j<row;j++)
						 for(unsigned int i = 0; i < colSize; i++)
                             arr[j*colSize+i] = 0.0f;   
				}
				if(_!=NULL)
					delete []_;
				_   =arr;
				row =rowSize;
				column=colSize;
			}
			else 
				Release();
		}
	}
};
#endif
