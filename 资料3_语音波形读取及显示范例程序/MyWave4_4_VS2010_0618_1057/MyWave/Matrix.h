//GMM建模新加文件，矩阵类
#ifndef MATRIX_H
#define MATRIX_H

#include "stdafx.h"
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
	inline Matrix()//构造函数
	{
		row    = 0;
		column = 0;
		_      = NULL;
	}
	inline virtual ~Matrix()//析构函数
	{
		Release();
	}

	inline Matrix(const Matrix &matrix)//用已知对象初始当前对象
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

	 inline Matrix& Zero()//矩阵赋值为零矩阵
	 {
		  for(unsigned int j=0;j<row;j++)
			 for(unsigned int i=0;i<column;i++)
				_[j*column+i]=0.0f;
		  return *this;
	 }
	 inline unsigned int RowSize ()const//返回矩阵的行数
	 {
		 return row;
	 }
	 inline unsigned int ColumnSize ()const//返回矩阵的列数
	 {
		 return column;
	 }
	 inline float *Array()const//返回矩阵数据
	 {
		 return _;
	 }	 
	 inline float& operator() (const unsigned int row,const unsigned int col)//返回指定位置的数据
	 {
		 if((row<this->row)&&(col<this->column))
			 return _[row*column+col];
		 return Vector::undef;
	 }
	 inline Vector GetRow(const unsigned int row)const//返回指定行的数据,为一个Vector
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

	 inline Vector GetColumn(const unsigned int col)const//返回指定列的数据
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
	 inline Matrix GetColumnSpace(const unsigned int col,const unsigned int len)const//返回从指定列col开始的共len列、行数为row的数据矩阵
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
			 const unsigned int end=col+len-1;//计算到哪列为止
			 const unsigned int size=len;//获取数据的列数
			 result.Resize(row,size,false);

			 if(col<column)
			 {
				 const unsigned int k=(end+1<=column ?end+1:column);//实际结束列的标识

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

	 inline Matrix GetRowSpace(const unsigned int row,const unsigned int len)const//返回从指定行row开始的共len行、列数为column的数据矩阵
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

	 inline Matrix& SetRow(const Vector& vector,const unsigned int row)//用给定vector对象的值替换指定行的数据
	 {
		 if(row<this->row)
		 {
			 unsigned int ki=(column<=vector.row? column:vector.row);
			 for(unsigned int i=0;i<ki;i++)
				 _[row*column+i]=vector._[i];
		 }
		 return *this;
	 }
     inline Matrix& SetColumn(const Vector& vector,const unsigned int col)//用给定vector对象的值替换指定列的数据
	 {
		 if(col<this->column)
		 {
			 unsigned int kj=(row<=vector.row? row:vector.row);
			 for(unsigned int j=0;j<kj;j++)
				 _[j*column+col]=vector._[j];
		 }
		 return *this;
	 }

	 inline Matrix& SetColumnSpace(const Matrix &matrix, const unsigned int col)//给定一个Matrix矩阵，替换当前矩阵中给定列col开始的数据
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

	 inline Matrix& SetRowSpace(const Matrix& matrix,const unsigned int row)//给定一个Matrix矩阵，替换当前矩阵中给定行row开始的数据
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

	 inline Matrix GetRowSpace(const Vector &ids)const//用vector对象指明要取矩阵中数据的总行数跟到底取哪行
	 {
		 Matrix result(ids.Size(),column);
		 return GetRowSpace(ids,result);
	 }
	 inline Matrix& GetRowSpace(const Vector &ids,Matrix &result)const
	 {
		 const unsigned int k=ids.Size();//k代表一共要取多少行
		 result.Resize(k,column);

		 for(unsigned int i=0;i<k;i++)
		 {
			 const unsigned int g=(unsigned int)(fabs(ROUND(ids._[i])));
			 if(g<row)//如果g小于row,取第g行的数据
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

	 inline Matrix GetColumnSpace(const Vector &ids)const//用vector对象指明要取矩阵中数据的总列数跟到底取哪列
	 {
		 Matrix result(row,ids.Size());
		 return GetColumnSpace(ids,result);
	 }
	 inline Matrix& GetColumnSpace(const Vector &ids,Matrix &result)const
	 {
		 const unsigned int k=ids.Size();//k代表一共要取多少列
		 result.Resize(row,k);

		 for(unsigned int i=0;i<k;i++)
		 {
			 const unsigned int g=(unsigned int)(fabs(ROUND(ids._[i])));
			 if(g<column)//如果g小于column,取第g列的数据
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

	 inline Matrix GetMatrixSpace(const Vector &rowIds,const Vector &colIds)const//rowIds确定取多少行取哪行，colIds确定取多少列取哪列
	 {
		 Matrix result(rowIds.Size(),colIds.Size());
		 return GetMatrixSpace(rowIds,colIds,result);
	 }
	 inline Matrix& GetMatrixSpace(const Vector &rowIds,const Vector &colIds,Matrix &result)const
	 {
		 unsigned int k1=rowIds.Size();//总共取多少行
		 unsigned int k2=colIds.Size();//总共取多少列
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
///////////////////////////////////////////////////////////////////运算符重载
	 inline Matrix operator -()const//当前矩阵元素去相反数-------------------重载取反操作符 "-"
	 {
		 Matrix result(row,column,false);
		 for(unsigned int i=0;i<row;i++)
			 for(unsigned int j=0;j<column;j++)
				 result._[i*column+j]=_[i*column+j];
		 return result;
	 }

	 inline virtual Matrix& operator =(const Matrix & matrix)//---------矩阵之间--------重载赋值操作符 "="
	 {
		 Resize(matrix.row,matrix.column,false);
		 for(unsigned int i=0;i<row;i++)
			 for(unsigned int j=0;j<column;j++)
				 _[i*column+j]=matrix._[i*column+j];
		 return *this;
	 }
	  inline  Matrix& operator +=(const Matrix & matrix)//---------矩阵之间--------重载操作符 "+="
	  {
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
			  for(unsigned int j=0;j<kj;j++)
				  _[i*column+j]+=matrix._[i*column+j];
		  return *this;
	  }

	  inline virtual Matrix& operator -=(const Matrix & matrix)//---------矩阵之间--------重载操作符 "-="
	  {
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
			  for(unsigned int j=0;j<kj;j++)
				  _[i*column+j]-=matrix._[i*column+j];
		  return *this;
	  }
	  inline virtual Matrix& operator ^=(const Matrix & matrix)//----------矩阵之间-------重载操作符 "^="
	  {
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
			  for(unsigned int j=0;j<kj;j++)
				  _[i*column+j]*=matrix._[i*column+j];
		  return *this;
	  }
	  inline virtual Matrix& operator /=(const Matrix & matrix)//---------矩阵之间--------重载操作符 "/="
	  {
		  const unsigned int ki=(row<=matrix.row ?row:matrix.row);
		  const unsigned int kj=(column<=matrix.column ? column: matrix.column);

		  for(unsigned int i=0;i<ki;i++)
			  for(unsigned int j=0;j<kj;j++)
				  _[i*column+j]/=matrix._[i*column+j];
		  return *this;
	  }
	  inline Matrix& operator +=(float scalar)//---------------矩阵与数值--------重载运算符"+="
	  {
		   for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  _[i*column+j]+=scalar;
		   return *this;
	  }
	  inline Matrix& operator -=(float scalar)//---------------矩阵与数值--------重载运算符"-="
	  {
		   for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  _[i*column+j]-=scalar;
		   return *this;
	  }
	  inline Matrix& operator *=(float scalar)//---------------矩阵与数值--------重载运算符"*="
	  {
		   for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  _[i*column+j]*=scalar;
		   return *this;
	  }
	  inline Matrix& operator /=(float scalar)//---------------矩阵与数值--------重载运算符"/="
	  {
		  scalar=1.0f/scalar;
		   for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  _[i*column+j]*=scalar;
		   return *this;
	  }
	  inline Matrix operator +(const Matrix &matrix)const//两个矩阵对应元素相加，结果矩阵跟前一个矩阵的行数列数相同---矩阵之间----重载运算符"+"
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
	  inline Matrix operator -(const Matrix &matrix)const//两个矩阵对应元素相减，结果矩阵跟前一个矩阵的行数列数相同---矩阵之间----重载运算符"-"
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
	  inline Matrix operator ^(const Matrix &matrix)const//两个矩阵对应元素相乘，结果矩阵跟前一个矩阵的行数列数相同---矩阵之间----重载运算符"^"
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
	  inline Matrix operator /(const Matrix &matrix)const//两个矩阵对应元素相除，结果矩阵跟前一个矩阵的行数列数相同---矩阵之间----重载运算符"/"
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


	  inline Matrix operator +(float scalar)const//矩阵每个元素都加一个数------------矩阵和数----重载运算符"+"
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
	  inline Matrix operator -(float scalar)const//矩阵每个元素都减一个数------------矩阵和数----重载运算符"-"
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
	  inline Matrix operator *(float scalar)const//矩阵每个元素都乘一个数------------矩阵和数----重载运算符"*"
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
  	  inline Matrix operator /(float scalar)const//矩阵每个元素都除一个数------------矩阵和数----重载运算符"/"
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

	  inline bool operator ==(const Matrix& matrix)const//重载运算符"=="
	  {
		  if((row!=matrix.row) ||(column!=matrix.column))
			  return false;
		  for(unsigned int i=0;i<row;i++)
			  for(unsigned int j=0;j<column;j++)
				  if(_[i*column+j]!=matrix._[i*column+j])
					  return false;
		  return true;
	  }
	  inline bool operator !=(Matrix& matrix)const//重载运算符"!="
	  {
		  return !(*this==matrix);
	  }
	  
	  inline Vector operator *(const Vector &vector)const//矩阵与矢量------重载运算符"*"
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
	  inline Matrix operator *(const Matrix &matrix)const//矩阵与矩阵------重载运算符"*"
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
	  inline Matrix& Identity()//矩阵对角线上为1,单位矩阵
	  {
		  const unsigned int k=(row>column ?column:row);
		  Zero();
		  for(unsigned int i=0;i<k;i++)
			  _[i*column+i]=1.0f;
		  return *this;
	  }

	  inline Matrix& Diag(const Vector &vector)//对角矩阵
	  {
		  const unsigned int k=(row>column? column:row);
		  const unsigned int k2=(k>vector.row ? vector.row:k);//k2=min(min(row,column),vector.row)

		  Zero();
		  for(unsigned int i=0;i<k2;i++)
			  _[i*column+i]=vector._[i];
		  return *this;
	  }

	  inline Matrix& Random()//随机生成矩阵的数值
	  {
		  for(unsigned int j=0;j<row;j++)
			  for(unsigned int i=0;i<column;i++)
				  _[j*column+i]=((float)rand())/((float)(RAND_MAX+1.0));
		  return *this;
	  }
      
	  inline Matrix Transpose()const//求矩阵的转置矩阵
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

	  inline Matrix VCat(const Matrix& matrix)//矩阵竖直方向连接
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

	  inline Matrix HCat(const Matrix& matrix)//矩阵水平方向连接
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

	  inline static int IsInverseOk()//判断是求逆
	  {
		  return bInverseOk;
	  }

	  inline Matrix Inverse(float * determinant=NULL)const//矩阵求逆
	  {
		  Matrix result;
		  return Inverse(result,determinant);
	  }
	  inline Matrix& Inverse(Matrix& result,float* determinant=NULL)const
	  {
		  bInverseOk=true;
		  if(row==column)//Square matrix方阵求逆矩阵
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
			 
			  result.Identity();//n*n的单位矩阵 
			  for(unsigned int i=0;i<n;i++)
			  {
				  float pivot=MM._[i*column+i];
				  if(fabs(pivot)<=EPSILON)
				  {
					  for(unsigned int j=i+1;j<n;j++)
					  {
						  if((pivot=MM._[j*column+i])!=0.0f)//行与行交换
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
						  for(unsigned int j=0;j<n;j++)//第k行减去 第i行* MM._[k*column+i]
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
     inline Matrix& SwapRow(unsigned int j1,unsigned int j2)//交换两行数据
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
	 inline Matrix& SwapColumn(unsigned int i1,unsigned int i2)//交换两列数据
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
  //一部分没有实现的函数，还没有用到
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
