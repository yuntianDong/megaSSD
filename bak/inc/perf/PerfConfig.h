#ifndef _PERF_CONFIG_H
#define _PERF_CONFIG_H

class BasicPerfConfig
{
public:
	uint32_t	mNumOfQueues;
	uint32_t	mQueueDepth;
	uint32_t	mDuration;

	bool		mbErrPrint;

	BasicPerfConfig(void)
		:mNumOfQueues(0),mQueueDepth(0),mDuration(0),mbErrPrint(false)
	{
	};

	virtual uint32_t GetMaxOutStandingIO(void)
	{
		return (uint32_t)mNumOfQueues * (uint32_t)mQueueDepth;
	}
};

#endif
