#ifndef _I_AIO_PERF_H
#define _I_AIO_PERF_H

#include "perf/PerfWorker.h"
#include "perf/AioPerfEngine.h"

#define DEF_AIO_QDEPTH		32
#define DEF_AIO_NUMOFQ		4
#define DEF_AIO_PATTERN		0xCA7557AC
#define DEF_AIO_XFERSIZE	128*1024

class PerfResult
{
private:
	int			mErrorCode;
	double		mIOPerSec;
	double		mMBPerSec;
	uint32_t	mNumOfErrs;

	friend class IAIOPerf;
public:
	PerfResult(void)
		:mErrorCode(0),mIOPerSec(0.0),mMBPerSec(0.0),mNumOfErrs(0)
	{};

	int	GetErrorCode(void) {return mErrorCode;};
	double GetIOPS(void) {return mIOPerSec;};
	double GetMBPs(void) {return mMBPerSec;};
	uint32_t GetIOErrors(void) {return mNumOfErrs;};
};

class IAIOPerf
{
private:
	int					mFD;
	BasicPerfWorker*	mpWorker;
	AioPerfEngine		mPerfEngine;

	uint32_t			mQDepth;
	uint32_t			mNumOfQ;
	uint32_t			mDataPattern;
	uint32_t			mXferSize;

	PerfResult* GetPerfResult(int errCode);
	void ConfigPerfTask(BasicPerfTask &task,uint64_t start,uint64_t length,uint32_t xferSize);
	void ConfigPerfCfg(BasicPerfConfig &cfg,uint32_t duration);

	PerfResult* PerfItemExec(enTaskMode mode,uint64_t start,uint64_t length,uint32_t duration);
public:
	IAIOPerf(const char* devname);
	virtual ~IAIOPerf(void);

	void SetQDepth(uint32_t qDepth) {mQDepth = qDepth;};
	void SetNumOfQ(uint32_t numOfQ) {mNumOfQ = numOfQ;};
	void SetDataPattern(uint32_t pattern) {mDataPattern = pattern;};
	void SetXferSize(uint32_t xferSize) {mXferSize = xferSize;};

	void SetStopWhenError(bool Enabled){mPerfEngine.SetStopWhenError(Enabled);};
	void SetTimeBased(bool Enabled){mPerfEngine.SetTimeBased(Enabled);};
	void SetNeedVerify(bool Enabled){mPerfEngine.SetNeedVerify(Enabled);};

	bool GetStopWhenError(void) {return mPerfEngine.GetStopWhenError();};
	bool GetTimeBased(void) {return mPerfEngine.GetTimeBased();};
	bool GetNeedVerify(void) {return mPerfEngine.GetNeedVerify();};

	uint32_t GetQDepth(void) {return mQDepth;};
	uint32_t GetNumOfQ(void) {return mNumOfQ;};
	uint32_t GetDataPattern(void) {return mDataPattern;};
	uint32_t GetXferSize(void) {return mXferSize;};

	PerfResult* SeqWrite(uint64_t start,uint64_t length,uint32_t duration);
	PerfResult* SeqRead(uint64_t start,uint64_t length,uint32_t duration);
	PerfResult* RandWrite(uint64_t start,uint64_t length,uint32_t duration);
	PerfResult* RandRead(uint64_t start,uint64_t length,uint32_t duration);
};

#endif
