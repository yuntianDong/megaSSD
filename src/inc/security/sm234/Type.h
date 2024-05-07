/*******************************************************************************
 ******     Copyright (c) 2014--2018 OSR.Co.Ltd. All rights reserved.     ****** 
 *******************************************************************************/
#ifndef _OSR_Type_H_
#define _OSR_Type_H_

typedef unsigned char   U8;
typedef unsigned char   U_8;
typedef unsigned char   uint8_t;

typedef signed char     S8;  //ĳЩƽ̨�ϣ�charĬ����signed char�ģ���X86/WIN/VC++6.0����ĳЩƽ̨��charĬ����unsigned char�ģ���arm/linux�����Ա�����ʽ���塣
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
    //��Ҫ����Ϊ64λ������Ϊlong int����Ϊgcc/g++ѡ��64λ����ʱ��long int��64λ�ģ���32λ����ʱ��ȴ���32λ�ģ��ᵼ�³���
    typedef unsigned long long int   U64;
    typedef signed long long int     S64;
#elif defined(_WIN32)
    typedef unsigned _int64          U64;
    typedef signed _int64            S64;
#endif


#endif
