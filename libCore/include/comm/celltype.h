#ifndef CELL_TYPE_H
#define CELL_TYPE_H

#pragma once
#ifdef WIN32
//#include <vld.h>
#endif

#include <math.h>

typedef unsigned int uint;
typedef unsigned int uint32;
typedef unsigned long ulong;

#ifdef WIN32
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
#include <sys/types.h>
typedef __int64_t int64;
typedef u_int64_t uint64;
#endif

typedef unsigned short uint16;
typedef unsigned char uint8;
typedef char int8;
typedef short int16;


#define MATH_PI 3.141592654f

#ifdef WIN32
inline uint F2L(float f) { return *((uint*)&f); }
inline float L2F(uint l) { return *((float*)&l); }
#else
union _FL
{
	int i;
	float f;
};
inline int F2L(float f) { _FL fl; fl.f = f; return fl.i;}
inline float L2F(long l) { _FL fl; fl.i = (int)l; return fl.f; }
#endif



#endif //CELL_TYPE_H

