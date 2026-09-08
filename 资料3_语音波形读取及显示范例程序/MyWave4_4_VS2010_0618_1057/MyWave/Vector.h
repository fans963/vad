//GMM建模新加文件
#ifndef VECTOR_H
#define VECTOR_H

#include "Macros.h"
#include <math.h>
#include <iostream>

#ifndef NULL
#define NULL 0
#endif

#ifdef USE_T_EXTENSIONS
template<unsigned int ROW> class TVector
#endif

class Vector
{
	friend class Matrix;
#ifdef USE_T_EXTENSIONS
	template<unsigned int ROW> friend class TVector;
#endif
protected:

//public:
	static float undef;
	unsigned int row;
	float        *_;

public:
	inline Vector()//构造函数
	{
		row=0;
		_ =NULL;
	}
	inline virtual ~Vector()//析构函数
	{
		Release();
	}
	inline Vector(const Vector &vector)//通过一个Vector对象赋值
	{
		row=0;
		_=NULL;
		Resize(vector.row,false);
		for(unsigned i=0;i<row;i++)
			_[i]=vector._ [i];
	}
	inline Vector(unsigned int size,bool clear=true)//给定row值，_[]赋值为0
	{
		row=0;
		_=NULL;
		Resize(size,false);
		if(clear)
	       Zero();
	}
	inline Vector(const float _[],unsigned int size)//给定row和_[]进行对象初始化
	{
		row      = 0;
		this->_  =NULL;
		Resize(size,false);
		for(unsigned int i=0;i<size;i++)
			this->_ [i]=_[i];
	}
	unsigned int RowSize()
	{
		return row;
	}

#ifdef USE_T_EXTENSIONS
	template<unsigned int ROW> inline Vector(const TVector<ROW>&vector)
	{
		row=0;
		_ =NULL;
		Resize(ROW,false);
		for(unsigned int i=0;i<ROW;i++)
			_[i]=vector._[i];
	}
#endif

	inline Vector &Zero()//_[]赋值为0
	{
		for(unsigned int i=0;i<row;i++)
			_[i]=0.0f;
		return *this;
	}

	inline Vector& One()//_[]赋值为1
	{
		for(unsigned int i=0;i<row;i++)
			_[i]=1.0f;
		return *this;
	}

	inline Vector& Random()//_[]赋值为随机数
	{
		for(unsigned int i=0;i<row;i++)
			_[i]=((float)rand())/((float)(RAND_MAX+1.0));
		return * this;
	}
	inline unsigned int Size() const{ return row; }//返回row的值
	inline float * GetArray() const{ return _; }//返回_[]的值
	inline float & operator[](const unsigned int row)//通过[],返回给定下标的数字元素
	{
		if(row<this->row)
			return _[row];
		return undef;
	}
	
	inline float & operator()(const unsigned int row)//通过（），返回给定下标的数字元素
	{
		if(row<this->row)
			return _[row];
		return undef;
	}

	inline Vector operator -() const //对_[]进行求反
	{
		Vector result(row,false);
		for(unsigned int i=0;i<row;i++)
			result._[i]=-_[i];
		return result;
	}

	inline Vector &Set(Vector &vector)//将当前对象设置为一个已知对象
	{
		return (*this)=vector;
	}
	inline void Set_(int n,double sum[])
	{
		Resize(n,false);
		for(int i=0;i<n;i++ )
			_[i]=sum[i];
		//return  _[i]=sum;
	}

	inline Vector& operator = (const Vector &vector)//用一个已知vector对象赋值，长度可能不相等
	{
		Resize(vector.row ,false);
		const unsigned int  k=(row<=vector.row ? row:vector.row );//k 取row和vector.row中最小的
		for(unsigned int i=0;i<k;i++)
			_[i]=vector._[i];
		for(unsigned int i=k;i<row;i++)
			_[i]=0;
		return *this;
	}

//////////////////////////////////////对象与数之间"+=" "-=" "^=" "/="/////////////////////////////////////////
	inline Vector & operator +=(const Vector &vector)//对+=进行重定义   ---对象之间-----加
	{
		const unsigned int k=(row<=vector.row ? row:vector.row );//k 取row和vector.row中最小的
		for(unsigned int i=0;i<k;i++)
			_[i] +=vector._ [i];
		return *this;
	}

	inline Vector & operator -=(const Vector &vector)//对-=进行重定义 ----对象之间----减
	{
		const unsigned int k=(row<=vector.row ? row:vector.row );//k 取row和vector.row中最小的
		for(unsigned int i=0;i<k;i++)
			_[i] -=vector._ [i];
		return *this;
	}

	inline Vector & operator ^=(const Vector &vector)//对^=进行重定义 ----对象之间----乘
	{
		const unsigned int k=(row<=vector.row ? row:vector.row );//k 取row和vector.row中最小的
		for(unsigned int i=0;i<k;i++)
			_[i] *=vector._ [i];
		return *this;
	}
	inline Vector & operator /=(const Vector &vector)//对/=进行重定义 -----对象之间---除
	{
		const unsigned int k=(row<=vector.row ? row:vector.row );//k 取row和vector.row中最小的
		for(unsigned int i=0;i<k;i++)
			_[i] /=vector._ [i];
		return *this;
	}
//////////////////////////////////////对象之间"+=" "-=" "^=" "/="/////////////////////////////////////////完

//////////////////////////////////////对象与数之间"+=" "-=" "^=" "/="///////////////////////////////////////
	inline Vector & operator +=(float scalar) //对+=进行重定义   ---对象与数之间-----加
	{
		for(unsigned int i=0;i<row;i++)
			_[i]+=scalar;
		return *this;
	}
	inline Vector & operator -=(float scalar) //对-=进行重定义   ---对象与数之间-----减
	{
		for(unsigned int i=0;i<row;i++)
			_[i]-=scalar;
		return *this;
	}
	inline Vector & operator *=(float scalar) //对*=进行重定义   ---对象与数之间-----乘
	{
		for(unsigned int i=0;i<row;i++)
			_[i]*=scalar;
		return *this;
	}
	inline Vector & operator /=(float scalar) //对/=进行重定义   ---对象与数之间-----除
	{
		scalar=1.0f/scalar;//只计算一次
		for(unsigned int i=0;i<row;i++)
			_[i]*=scalar;
		return *this;
	}
//////////////////////////////////////对象与数之间"+=" "-=" "^=" "/="/////////////////////////////////////////完

//////////////////////////////////////对象之间"+" "-" "^" "/"///////////////////////////////////////////////////
	inline Vector  operator + (const Vector &vector) const//对 "+" 进行重定义   ---对象之间-----加
	{
		Vector result(row,false);
		return Add(vector,result);
	}
	inline Vector& Add(const Vector & vector,Vector & result) const
	{
		result.Resize(row,false);
		const unsigned int k=(row<=vector.row?row:vector.row);
		for(unsigned int i=0;i<k;i++)
			result._[i]=_[i]+vector._[i];
		for(unsigned int i=k;i<row;i++)
			result._[i]=_[i];
		return result;
	}

	inline Vector operator - (const Vector &vector) const//对 "-" 进行重定义   ---对象之间-----减
	{
		Vector result(row,false);
		return Sub(vector,result);
	}
	inline Vector& Sub(const Vector & vector,Vector & result) const
	{
		result.Resize(row,false);
		const unsigned int k=(row<=vector.row?row:vector.row);
		for(unsigned int i=0;i<k;i++)
			result._[i]=_[i]-vector._[i];
		for(unsigned int i=k;i<row;i++)
			result._[i]=_[i];
		return result;
	}

	inline Vector  operator ^ (const Vector &vector) const//对 "^" 进行重定义   ---对象之间-----乘
	{
		Vector result(row,false);
		return PMult(vector,result);
	}
	inline Vector& PMult(const Vector & vector,Vector & result) const
	{
		result.Resize(row,false);
		const unsigned int k=(row<=vector.row?row:vector.row);
		for(unsigned int i=0;i<k;i++)
			result._[i]=_[i]*vector._[i];
		for(unsigned int i=k;i<row;i++)
			result._[i]=_[i];
		return result;
	}
	inline Vector  operator /(const Vector &vector) const//对 "/" 进行重定义   ---对象之间-----除
	{
		Vector result(row,false);
		return PDiv(vector,result);
	}
	inline Vector& PDiv(const Vector & vector,Vector & result) const
	{
		result.Resize(row,false);
		const unsigned int k=(row<=vector.row?row:vector.row);
		for(unsigned int i=0;i<k;i++)
			result._[i]=_[i]/vector._[i];
		for(unsigned int i=k;i<row;i++)
			result._[i]=_[i];
		return result;
	}
////////////////////////////////对象之间"+" "-" "^" "/"///////////////////////////////////////////////完

/////////////////////////////////对象与数之间"+" "-" "^" "/"///////////////////////////////////////////////   
	inline Vector operator +(float scalar)const //对 "+" 进行重定义   ---对象之间-----加
	{
		Vector result(row,false);
		return Add(scalar,result);
	}
	inline Vector& Add(float scalar,Vector &result)const
	{
		result.Resize(row,false);
		for(unsigned int i=0;i<row;i++)
			result._[i]=_[i]+scalar;
		return result;
	}
	inline Vector operator -(float scalar)const //对 "-" 进行重定义   ---对象之间-----减
	{
		Vector result(row,false);
		return Sub(scalar,result);
	}
	inline Vector& Sub(float scalar,Vector &result)const
	{
		result.Resize(row,false);
		for(unsigned int i=0;i<row;i++)
			result._[i]=_[i]-scalar;
		return result;
	}
	inline Vector operator *(float scalar)const //对 "*" 进行重定义   ---对象之间-----乘
	{
		Vector result(row,false);
		return Mult(scalar,result);
	}
	inline Vector& Mult(float scalar,Vector &result)const
	{
		result.Resize(row,false);
		for(unsigned int i=0;i<row;i++)
			result._[i]=_[i]*scalar;
		return result;
	}
	inline Vector operator /(float scalar)const //对 "/" 进行重定义   ---对象之间-----除
	{
		Vector result(row,false);
		return Div(scalar,result);
	}
	inline Vector& Div(float scalar,Vector &result)const
	{
		result.Resize(row,false);
		scalar=1.0f/scalar;
		for(unsigned int i=0;i<row;i++)
			result._[i]=_[i]*scalar;
		return result;
	}


/////////////////////////////////对象与数之间"+" "-" "^" "/"///////////////////////////////////////////////
	inline float operator * (const Vector &vector) const//???????????????????不知道用来做什么
	{ 
		return this->Dot(vector);
	}
	inline float Dot(const Vector &vector) const
	{
		float result=0.0f;
		unsigned int k=(row<=vector.row ? row : vector.row);
		for(unsigned int i=0;i<k;i++)
			result +=_[i]*vector._[i];
		return result;
	}
////////////////////////////////////////////////////////////////////////////////////////
	inline bool operator ==(const Vector& vector)const//判断当前对象与指定对象相等吗
	{
		if(row!=vector.row )return false;
		for(unsigned int i=0;i<row;i++)
			if(_[i]!=vector._[i])return false;
		return true;		
	}

	inline bool operator !=(const Vector&vector)const//判断当前对象与给定对象不等吗
	{
		return !(*this==vector);
	}

	inline float Sum()const//vector对象各元素求和
	{
		float result=0.0f;
		for(unsigned int i=0;i<row;i++)
			result +=_[i];
		return result;
	}
	inline float Norm() const//求|vector|
	{
		return sqrtf(Norm2());
	}

	inline float Norm2()const//求vector对象各元素的平方和
	{
		float result=0.0f;
		for(unsigned int i=0;i<row;i++)
			result+=_[i]*_[i];
		return result;
	}
	inline void Normalize()//vector对象中元素归一化
	{
		float scalar=1.0f/Norm();
		(*this)*=scalar;
	}

	inline float Distance(const Vector& vector)const//计算两个vector对象之间的距离
	{
		return (*this-vector).Norm();
	}
	inline float Distance2(const Vector& vector)const
	{
		return (*this-vector).Norm2();
	}
    
	inline Vector& SetSubVector(unsigned int startPos,const Vector &vector)//当前对象的startPos开始的K个元素用vector的前K代替
	{
		if(startPos<row)
		{
			unsigned int k= (row-startPos <vector.row ? row-startPos :vector.row);
			for(unsigned int i=0;i<k;i++)
				_[startPos+i]=vector._[i];
		}
		return *this;
	}

	inline Vector  GetSubVector(unsigned int startPos,unsigned int len)
	{
		Vector result(len,false);
		return GetSubVector(startPos,len,result);
	}

	inline Vector & GetSubVector(unsigned int startPos,unsigned int len,Vector & result)
	{
		result.Resize(len,false);
		if(startPos<row)
		{
			const unsigned int k=(row-startPos <=len ?row-startPos:len);
			for(unsigned int i=0;i<k;i++)
				result[i]=_[startPos+i];
			for(unsigned int i=k;i<len;i++)
				result[i]=0.0f;
		}
		else
			result.Zero ();
		return result;
	}



	inline float Max()//求_[]最大值
	{
		if(row==0)
			return 0.0f;
		float res=_[0];
		for(unsigned int i=1;i<row;i++)
			if(_[i]>res)
				res=_[i];
		return res;
	}

	inline int MaxId()//求最大值的下标
	{
		if(row==0)
			return -1;
		float mx=_[0];
		int res=0;
		for(unsigned int i=1;i<row;i++)
			if(_[i]>mx)
				res=i;
		return res;
	}

	inline Vector Abs()//求_[]各元素的绝对值
	{
		Vector result(row);
		return Abs(result);
	}
	inline Vector& Abs( Vector &result)const
	{
		result.Resize(row,false);
		for(unsigned int i=0;i<row;i++)
			result._[i]=fabs(_[i]);
		return result;
	}

	inline Vector& GetSubVector(const Vector &ids,Vector &result)const
	{
		const unsigned int k=ids.Size();
		result.Resize(k);
		for(unsigned int i=0;i<k;i++)
		{
			const unsigned int g=(unsigned int)(fabs(ROUND(ids._[i])));
			if(g<row)
				result._[i]=_[g];
			else
				result._[i]=0.0f;
		}
		return result;
	}

	void Print() const
	{
		std::cout<<"Vector" <<row<<std::endl;
		for(unsigned int i=0;i<row;i++)
			std::cout<<_[i]<<"   ";
		std::cout<<std::endl;
	}
protected:
	inline void Release()
	{
		if(_ !=NULL)
			delete []_;
		row=0;
		_=NULL;
	}
public:
	inline virtual void Resize(unsigned int size,bool copy=true)
	{
		if(row!=size)
		{
			if(size)
			{
				float * arr=new float[size];
				if(copy)
				{
					unsigned int m=(row<size ? row:size);
				    for(unsigned int i=0;i<m;i++)
						arr[i]=_[i];
				    for(unsigned int i=m;i<size;i++)
					    arr[i]=0.0f;
				}
				if(_!=NULL)
					delete []_;
				_=arr;
				row=size;
			}
			else 
				Release();
		}
	}
	

};
#endif
