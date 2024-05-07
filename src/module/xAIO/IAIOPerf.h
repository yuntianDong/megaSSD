#ifndef _IAIO_PERF_H
#define _IAIO_PERF_H

#include "perf/aio/AioPerfEngine.h"
#include "Logging.h"

class IAIOResult
{
private:
	PerfStat	mPerfStat;
	uint32_t	muXferSize;
public:
	IAIOResult(void)
		:muXferSize(0)
	{};

	IAIOResult(PerfStat* pPerfStat,uint32_t xferSize)
		:mPerfStat(*pPerfStat),muXferSize(xferSize)
	{};

	double GetMBps(void)
	{
		return CalcMBps(&mPerfStat,muXferSize);
	};

	double GetIOPS(void)
	{
		return CalcIOps(&mPerfStat);
	};

	uint32_t GetCmdErrCnt(void)
	{
		return GetIOCmdErrCnt(&mPerfStat);
	};

	uint32_t GetDMCErrCnt(void)
	{
		return GetIODMCErrCnt(&mPerfStat);
	};
};

class IAIOPerf
{
private:
	int				mFD;

	uint32_t		muXferSize;
	uint32_t		muDataPattern;
	uint32_t		muQueueDepth;
	uint32_t		muNumOfQueue;

	void	OpenFile(const char* filename,bool bDirect);
	IAIOResult* DoPerfTest(uint64_t offset,uint64_t size,bool bSeq,bool bRead,uint32_t execTime);
public:
	IAIOPerf(const char* filename,bool bDirect);
	virtual ~IAIOPerf(void);

	void SetXferSize(uint32_t val) {muXferSize = val;};
	uint32_t GetXferSize(void) {return muXferSize;};
	void SetDataPattern(uint32_t val) {muDataPattern = val;};
	uint32_t GetDataPattern(void) {return muDataPattern;};
	void SetQueueDepth(uint32_t val) {muQueueDepth = val;};
	uint32_t GetQueueDepth(void) {return muQueueDepth;};
	void SetNumOfQueue(uint32_t val) {muNumOfQueue = val;};
	uint32_t GetNumOfQueue(void) {return muNumOfQueue;};

	IAIOResult*	SeqWrite(uint64_t offset,uint64_t size,uint32_t execTime);
	IAIOResult*	SeqRead(uint64_t offset,uint64_t size,uint32_t execTime);
	IAIOResult*	RandWrite(uint64_t offset,uint64_t size,uint32_t execTime);
	IAIOResult*	RandRead(uint64_t offset,uint64_t size,uint32_t execTime);
};

#endif
