#include "MFCStub.h"
//GMM寤烘ā鏂板姞鏂囦欢
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
	inline Vector()//鏋勯犲嚱鏁
	{
		row=0;
		_ =NULL;
	}
	inline virtual ~Vector()//鏋愭瀯鍑芥暟
	{
		Release();
	}
	inline Vector(const Vector &vector)//閫氳繃涓涓猇ector瀵硅薄璧嬪
	{
		row=0;
		_=NULL;
		Resize(vector.row,false);
		for(unsigned i=0;i<row;i++)
			_[i]=vector._ [i];
	}
	inline Vector(unsigned int size,bool clear=true)//缁欏畾row鍊硷紝_[]璧嬪间负0
	{
		row=0;
		_=NULL;
		Resize(size,false);
		if(clear)
	       Zero();
	}
	inline Vector(const float _[],unsigned int size)//缁欏畾row鍜宊[]杩涜屽硅薄鍒濆嬪寲
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

	inline Vector &Zero()//_[]璧嬪间负0
	{
		for(unsigned int i=0;i<row;i++)
			_[i]=0.0f;
		return *this;
	}

	inline Vector& One()//_[]璧嬪间负1
	{
		for(unsigned int i=0;i<row;i++)
			_[i]=1.0f;
		return *this;
	}

	inline Vector& Random()//_[]璧嬪间负闅忔満鏁
	{
		for(unsigned int i=0;i<row;i++)
			_[i]=((float)rand())/((float)(RAND_MAX+1.0));
		return * this;
	}
	inline unsigned int Size() const{ return row; }//杩斿洖row鐨勫
	inline float * GetArray() const{ return _; }//杩斿洖_[]鐨勫
	inline float & operator[](const unsigned int row)//閫氳繃[],杩斿洖缁欏畾涓嬫爣鐨勬暟瀛楀厓绱
	{
		if(row<this->row)
			return _[row];
		return undef;
	}
	
	inline float & operator()(const unsigned int row)//閫氳繃锛堬級锛岃繑鍥炵粰瀹氫笅鏍囩殑鏁板瓧鍏冪礌
	{
		if(row<this->row)
			return _[row];
		return undef;
	}

	inline Vector operator -() const //瀵筥[]杩涜屾眰鍙
	{
		Vector result(row,false);
		for(unsigned int i=0;i<row;i++)
			result._[i]=-_[i];
		return result;
	}

	inline Vector &Set(Vector &vector)//灏嗗綋鍓嶅硅薄璁剧疆涓轰竴涓宸茬煡瀵硅薄
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

	inline Vector& operator = (const Vector &vector)//鐢ㄤ竴涓宸茬煡vector瀵硅薄璧嬪硷紝闀垮害鍙鑳戒笉鐩哥瓑
	{
		Resize(vector.row ,false);
		const unsigned int  k=(row<=vector.row ? row:vector.row );//k 鍙杛ow鍜寁ector.row涓鏈灏忕殑
		for(unsigned int i=0;i<k;i++)
			_[i]=vector._[i];
		for(unsigned int i=k;i<row;i++)
			_[i]=0;
		return *this;
	}

//////////////////////////////////////瀵硅薄涓庢暟涔嬮棿"+=" "-=" "^=" "/="/////////////////////////////////////////
	inline Vector & operator +=(const Vector &vector)//瀵+=杩涜岄噸瀹氫箟   ---瀵硅薄涔嬮棿-----鍔
	{
		const unsigned int k=(row<=vector.row ? row:vector.row );//k 鍙杛ow鍜寁ector.row涓鏈灏忕殑
		for(unsigned int i=0;i<k;i++)
			_[i] +=vector._ [i];
		return *this;
	}

	inline Vector & operator -=(const Vector &vector)//瀵-=杩涜岄噸瀹氫箟 ----瀵硅薄涔嬮棿----鍑
	{
		const unsigned int k=(row<=vector.row ? row:vector.row );//k 鍙杛ow鍜寁ector.row涓鏈灏忕殑
		for(unsigned int i=0;i<k;i++)
			_[i] -=vector._ [i];
		return *this;
	}

	inline Vector & operator ^=(const Vector &vector)//瀵筤=杩涜岄噸瀹氫箟 ----瀵硅薄涔嬮棿----涔
	{
		const unsigned int k=(row<=vector.row ? row:vector.row );//k 鍙杛ow鍜寁ector.row涓鏈灏忕殑
		for(unsigned int i=0;i<k;i++)
			_[i] *=vector._ [i];
		return *this;
	}
	inline Vector & operator /=(const Vector &vector)//瀵/=杩涜岄噸瀹氫箟 -----瀵硅薄涔嬮棿---闄
	{
		const unsigned int k=(row<=vector.row ? row:vector.row );//k 鍙杛ow鍜寁ector.row涓鏈灏忕殑
		for(unsigned int i=0;i<k;i++)
			_[i] /=vector._ [i];
		return *this;
	}
//////////////////////////////////////瀵硅薄涔嬮棿"+=" "-=" "^=" "/="/////////////////////////////////////////瀹

//////////////////////////////////////瀵硅薄涓庢暟涔嬮棿"+=" "-=" "^=" "/="///////////////////////////////////////
	inline Vector & operator +=(float scalar) //瀵+=杩涜岄噸瀹氫箟   ---瀵硅薄涓庢暟涔嬮棿-----鍔
	{
		for(unsigned int i=0;i<row;i++)
			_[i]+=scalar;
		return *this;
	}
	inline Vector & operator -=(float scalar) //瀵-=杩涜岄噸瀹氫箟   ---瀵硅薄涓庢暟涔嬮棿-----鍑
	{
		for(unsigned int i=0;i<row;i++)
			_[i]-=scalar;
		return *this;
	}
	inline Vector & operator *=(float scalar) //瀵*=杩涜岄噸瀹氫箟   ---瀵硅薄涓庢暟涔嬮棿-----涔
	{
		for(unsigned int i=0;i<row;i++)
			_[i]*=scalar;
		return *this;
	}
	inline Vector & operator /=(float scalar) //瀵/=杩涜岄噸瀹氫箟   ---瀵硅薄涓庢暟涔嬮棿-----闄
	{
		scalar=1.0f/scalar;//鍙璁＄畻涓娆
		for(unsigned int i=0;i<row;i++)
			_[i]*=scalar;
		return *this;
	}
//////////////////////////////////////瀵硅薄涓庢暟涔嬮棿"+=" "-=" "^=" "/="/////////////////////////////////////////瀹

//////////////////////////////////////瀵硅薄涔嬮棿"+" "-" "^" "/"///////////////////////////////////////////////////
	inline Vector  operator + (const Vector &vector) const//瀵 "+" 杩涜岄噸瀹氫箟   ---瀵硅薄涔嬮棿-----鍔
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

	inline Vector operator - (const Vector &vector) const//瀵 "-" 杩涜岄噸瀹氫箟   ---瀵硅薄涔嬮棿-----鍑
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

	inline Vector  operator ^ (const Vector &vector) const//瀵 "^" 杩涜岄噸瀹氫箟   ---瀵硅薄涔嬮棿-----涔
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
	inline Vector  operator /(const Vector &vector) const//瀵 "/" 杩涜岄噸瀹氫箟   ---瀵硅薄涔嬮棿-----闄
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
////////////////////////////////瀵硅薄涔嬮棿"+" "-" "^" "/"///////////////////////////////////////////////瀹

/////////////////////////////////瀵硅薄涓庢暟涔嬮棿"+" "-" "^" "/"///////////////////////////////////////////////   
	inline Vector operator +(float scalar)const //瀵 "+" 杩涜岄噸瀹氫箟   ---瀵硅薄涔嬮棿-----鍔
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
	inline Vector operator -(float scalar)const //瀵 "-" 杩涜岄噸瀹氫箟   ---瀵硅薄涔嬮棿-----鍑
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
	inline Vector operator *(float scalar)const //瀵 "*" 杩涜岄噸瀹氫箟   ---瀵硅薄涔嬮棿-----涔
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
	inline Vector operator /(float scalar)const //瀵 "/" 杩涜岄噸瀹氫箟   ---瀵硅薄涔嬮棿-----闄
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


/////////////////////////////////瀵硅薄涓庢暟涔嬮棿"+" "-" "^" "/"///////////////////////////////////////////////
	inline float operator * (const Vector &vector) const//???????????????????涓嶇煡閬撶敤鏉ュ仛浠涔
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
	inline bool operator ==(const Vector& vector)const//鍒ゆ柇褰撳墠瀵硅薄涓庢寚瀹氬硅薄鐩哥瓑鍚
	{
		if(row!=vector.row )return false;
		for(unsigned int i=0;i<row;i++)
			if(_[i]!=vector._[i])return false;
		return true;		
	}

	inline bool operator !=(const Vector&vector)const//鍒ゆ柇褰撳墠瀵硅薄涓庣粰瀹氬硅薄涓嶇瓑鍚
	{
		return !(*this==vector);
	}

	inline float Sum()const//vector瀵硅薄鍚勫厓绱犳眰鍜
	{
		float result=0.0f;
		for(unsigned int i=0;i<row;i++)
			result +=_[i];
		return result;
	}
	inline float Norm() const//姹倈vector|
	{
		return sqrtf(Norm2());
	}

	inline float Norm2()const//姹倂ector瀵硅薄鍚勫厓绱犵殑骞虫柟鍜
	{
		float result=0.0f;
		for(unsigned int i=0;i<row;i++)
			result+=_[i]*_[i];
		return result;
	}
	inline void Normalize()//vector瀵硅薄涓鍏冪礌褰掍竴鍖
	{
		float scalar=1.0f/Norm();
		(*this)*=scalar;
	}

	inline float Distance(const Vector& vector)const//璁＄畻涓や釜vector瀵硅薄涔嬮棿鐨勮窛绂
	{
		return (*this-vector).Norm();
	}
	inline float Distance2(const Vector& vector)const
	{
		return (*this-vector).Norm2();
	}
    
	inline Vector& SetSubVector(unsigned int startPos,const Vector &vector)//褰撳墠瀵硅薄鐨剆tartPos寮濮嬬殑K涓鍏冪礌鐢╲ector鐨勫墠K浠ｆ浛
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



	inline float Max()//姹俖[]鏈澶у
	{
		if(row==0)
			return 0.0f;
		float res=_[0];
		for(unsigned int i=1;i<row;i++)
			if(_[i]>res)
				res=_[i];
		return res;
	}

	inline int MaxId()//姹傛渶澶у肩殑涓嬫爣
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

	inline Vector Abs()//姹俖[]鍚勫厓绱犵殑缁濆瑰
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
