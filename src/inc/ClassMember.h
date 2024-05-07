#ifndef _CLASS_MEMBER_H
#define _CLASS_MEMBER_H

#include <stdint.h>
#include <iostream>
using namespace std;
#include <exception>

#include "utility/ByteHdlr.h"

#ifndef UNUSED
#define UNUSED(expr) do { (void)(expr); } while (0)
#endif

template <class T,class T2=T>
class ClassMember
{
protected:
	T*		mpClsMem;
	bool	mbNeedConvert;
public:
	ClassMember(T* pClsMember,bool needConvert=false)
		:mpClsMem(pClsMember),mbNeedConvert(needConvert)
	{
	}

	virtual T2 Get(void) const
	{
		T tmpVal  = *mpClsMem;
		if(true == mbNeedConvert)
		{
			return *(T2*)(ConvertEndian(&tmpVal,sizeof(T)));
		}
		return tmpVal;
	};

	virtual void Set(const T2 val)
	{
		T2 tmpVal  = val;
		if(true == mbNeedConvert)
		{
			tmpVal	  = *(T2*)(ConvertEndian(&tmpVal,sizeof(T2)));
		}
		*mpClsMem = (T)tmpVal;
	}
};

template <class T1,class T2=T1>
class ClassMemberBitMask : public ClassMember<T1,T2>
{
private:
	uint32_t	mBitOffset;
	uint32_t	mBitLength;
public:
	ClassMemberBitMask(T1* pClsMember,uint32_t bitOffset,uint32_t bitLength,bool needConvert=false)
		:ClassMember<T1,T2>(pClsMember,needConvert),mBitOffset(bitOffset),mBitLength(bitLength)
	{
	}

	virtual T2 Get(void) const
	{
		T1 tmpVal1 = *(this->mpClsMem);
		if(true == this->mbNeedConvert)
		{
			tmpVal1 = *(T1*)ConvertEndian(&tmpVal1,sizeof(T1));
		}

		return (T2)((tmpVal1 >> mBitOffset) & ((1 << mBitLength) -1));
	}

	virtual void Set(const T2 val)
	{
		uint32_t bitMask	= ((1 << mBitLength) -1) << mBitOffset;
		T1 tmpVal1 = *(this->mpClsMem);

		if(true == this->mbNeedConvert)
		{
			tmpVal1 = *(T1*)ConvertEndian(&tmpVal1,sizeof(T1));
		}

		T1 tmpVal2 = (tmpVal1 & ~bitMask) | (((T1)val << mBitOffset) & bitMask);

		if(true == this->mbNeedConvert)
		{
			(*(this->mpClsMem)) = *(T1*)ConvertEndian(&tmpVal2,sizeof(T1));
		}
		else
		{
			(*(this->mpClsMem)) = tmpVal2;
		}
	}
};

template <class T>
class ClassMemberOnlyRead : public ClassMember<T>
{
public:
	ClassMemberOnlyRead(T* pClsMember,bool needConvert=false)
		:ClassMember<T>(pClsMember,needConvert)
	{
	}

	void Set(const T val) {UNUSED(val);return;};
};

template <class T>
class ClassMemberOnlyWrite : public ClassMember<T>
{
public:
	ClassMemberOnlyWrite(T* pClsMember,bool needConvert=false)
		:ClassMember<T>(pClsMember,needConvert)
	{
	}

	T Get(void) const {return (-1);};
};

#endif
