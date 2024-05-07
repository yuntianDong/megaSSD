#ifndef _PERF_STAT_H
#define _PERF_STAT_H

class BasicPerfStat
{
private:
	uint64_t	mTotalIO;
	uint32_t	mDuration;
	uint64_t	mTotalKB;
	uint32_t	mIOErrors;

	friend class BasicPerfEngine;
public:
	BasicPerfStat(void)
		:mTotalIO(0),mDuration(0),mTotalKB(0),mIOErrors(0)
	{
	};

	uint32_t GetIOErrCnt(void) {return mIOErrors;};

	double GetStatIOPS(void)
	{
		return (double)mTotalIO / (double)(mDuration/1000);
	}

	double GetStatMBps(void)
	{
		return (double)mTotalKB / (double)(mDuration);
	}
};

#endif
