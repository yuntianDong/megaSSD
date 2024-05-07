#ifndef _CLICK_TIMER_H
#define _CLICK_TIMER_H

#include <time.h>

#define MSEC_PER_SEC		1000
#define USEC_PER_SEC		1000000
#define NSEC_PER_SEC		1000000000

typedef enum _enTUnit
{
	TU_MS,
	TU_US,
	TU_NS,
}enTUnit;

class ClkTimer
{
private:
	clockid_t			mClkID;
	uint64_t*			mpEclapsed;

	struct timespec		mTsStart;
	struct timespec		mTsFinish;

	uint64_t			mMultiSec;
	uint64_t			mDivisNSec;

public:
	ClkTimer(clockid_t clkID,uint64_t& eclapsed,enTUnit unit=TU_US)
		:mClkID(clkID)
	{
		mpEclapsed = &eclapsed;

		clock_gettime(clkID,&mTsStart);
		clock_gettime(clkID,&mTsFinish);

		if(TU_MS == unit)
		{
			mMultiSec	= (uint64_t)MSEC_PER_SEC;
		}
		else if(TU_US == unit)
		{
			mMultiSec	= (uint64_t)USEC_PER_SEC;
		}
		else if(TU_NS == unit)
		{
			mMultiSec	= (uint64_t)NSEC_PER_SEC;
		}

		mDivisNSec	= (uint64_t)NSEC_PER_SEC / mMultiSec;
	};

	virtual ~ClkTimer(void)
	{
		clock_gettime(mClkID,&mTsFinish);

		if( (mTsFinish.tv_nsec - mTsStart.tv_nsec) < 0 )
		{
			*mpEclapsed	+= (mTsFinish.tv_sec - mTsStart.tv_sec - 1) * mMultiSec +
					(uint64_t)(NSEC_PER_SEC+mTsFinish.tv_nsec-mTsStart.tv_nsec) / mDivisNSec;
		}
		else
		{
			*mpEclapsed += (mTsFinish.tv_sec - mTsStart.tv_sec) * mMultiSec +
					(uint64_t)(mTsFinish.tv_nsec-mTsStart.tv_nsec) / mDivisNSec;
		}
	}
};

class SysClkTimer : public ClkTimer
{
public:
	SysClkTimer(uint64_t& eclapsed,enTUnit unit=TU_US)
		:ClkTimer(CLOCK_MONOTONIC,eclapsed,unit)
	{
	}
};

class ProcessClkTimer : public ClkTimer
{
public:
	ProcessClkTimer(uint64_t& eclapsed,enTUnit unit=TU_US)
		:ClkTimer(CLOCK_PROCESS_CPUTIME_ID,eclapsed,unit)
	{
	}
};

class ThreadClkTimer : public ClkTimer
{
public:
	ThreadClkTimer(uint64_t& eclapsed,enTUnit unit=TU_US)
		:ClkTimer(CLOCK_THREAD_CPUTIME_ID,eclapsed,unit)
	{
	}
};

#endif
