#ifndef _PAIR_ARRAY_H
#define _PAIR_ARRAY_H

#include <stdio.h>
#include <string>

template <class T1,class T2>
class PairArray
{
private:
	T1*	mpArray1;
	T2* mpArray2;

	uint32_t mMaxCount;
	uint32_t mCurPoint;
public:
	PairArray(uint32_t maxItems)
	{
		mpArray1 = (T1*)malloc(sizeof(T1)*maxItems);
		mpArray2 = (T2*)malloc(sizeof(T2)*maxItems);

		memset(mpArray1,0,sizeof(T1)*maxItems);
		memset(mpArray2,0,sizeof(T2)*maxItems);

		mMaxCount= maxItems;
		mCurPoint= 0;
	};

	virtual ~PairArray(void)
	{
		if(NULL != mpArray1)
		{
			free(mpArray1);
			mpArray1 = NULL;
		}

		if(NULL != mpArray2)
		{
			free(mpArray2);
			mpArray2 = NULL;
		}
	}

	bool Push(T1 v1,T2 v2)
	{
		if(mCurPoint >= mMaxCount)
		{
			return false;
		}
		mpArray1[mCurPoint]	= v1;
		mpArray2[mCurPoint++] = v2;
	};

	bool Pull(T1& v1,T2& v2)
	{
		if(0 == mCurPoint)
		{
			return false;
		}

		v1 = mpArray1[mCurPoint];
		v2 = mpArray2[mCurPoint--];
	};

	void Reset(void) {mCurPoint = 0;};
};

#endif
