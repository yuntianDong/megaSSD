/*******************************************************************************
 ******     Copyright (c) 2014--2018 OSR.Co.Ltd. All rights reserved.     ****** 
 *******************************************************************************/
#ifndef _OSR_Type_H_
#define _OSR_Type_H_

typedef unsigned char   U8;
typedef unsigned char   U_8;
typedef unsigned char   uint8_t;

typedef signed char     S8;  //某些平台上，char默认是signed char的，如X86/WIN/VC++6.0；但某些平台上char默认是unsigned char的，如arm/linux，所以必须显式定义。
typedef signed char     S_8;
typedef signed char     int8_t;

typedef unsigned short  U16;
typedef unsigned short  U_16;
typedef unsigned short  uint16_t;

typedef signed short    S16;
typedef signed short    S_16;
typedef signed short    int16_t;

typedef unsigned int    U32;
typedef unsigned int    U_32;
typedef unsigned int    uint32_t;

typedef signed int      S32;
typedef signed int      int32_t;

#if defined(__linux)
    //不要定义为64位整型数为long int，因为gcc/g++选择64位编译时，long int是64位的；而32位编译时，却变成32位的，会导致出错
    typedef unsigned long long int   U64;
    typedef signed long long int     S64;
#elif defined(_WIN32)
    typedef unsigned _int64          U64;
    typedef signed _int64            S64;
#endif


#endif
