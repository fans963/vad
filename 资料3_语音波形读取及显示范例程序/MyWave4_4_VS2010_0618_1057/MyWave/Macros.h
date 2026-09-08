//GMM建模新加的文件
#ifndef _MATHMACROS_H
#define _MATHMACROS_H
#include<math.h>

#ifndef PIf
#define PIf  3.14159265358979323846f
#endif

#ifndef DEG2RAD
#define DEG2RAD(x) ((x)*(PIf/180.0f))//度数化成角度
#endif

#ifndef RAD2DEG 
#define RAD2DEG(x) ((x)*(180.0f/PIf))//角度化成度数
#endif

#ifndef RND 
#define RND(x) (float((x)*((double)rand())/((double)(RAND_MAX+1.0))))
#endif

#ifndef MIN 
#define MIN(x,y) (((x)<(y))?(x):(y))//求最小值
#endif

#ifndef MAX 
#define MAX(x,y) (((x)>(y))?(x):(y))//求最大值
#endif

#ifndef TRUNC
#define TRUNC(x,mn,mx) (MIN(MAX((x),(mn)),(mx)))//求前两个数中最大的，然后跟第三个数再求最小值
#endif

#ifndef SIGN2
#define SIGN2(x) (((x)< 0.0f)?(-1.0f):(1.0f))
#endif


#ifndef SIGN2
#define SIGN2(a,b) ((b) >= 0.0f ? fabs(a) : -fabs(a))
#endif

#ifndef ROUND
#define ROUND(x) (floor((x)+0.5f))//floor()返回小于或者等于指定表达式的最大整数
#endif

#ifndef EPSILON
#define EPSILON   (1e-6)
#endif

inline float hypot_s(float a,float b)
{
	float r;
	if(fabs(a) > fabs(b))
	{
		r=b/a;
		r=fabs(a) * sqrtf(1.0f +r*r);
	}
	else if(b!=0.0f)
	{
		r=a/b;
		r=fabs(b)*sqrtf(1.0f + r*r);
	}
	else
	{
		r=0.0f;
	}
	return r;
}

#endif  