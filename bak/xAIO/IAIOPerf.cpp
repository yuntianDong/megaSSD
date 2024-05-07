/*
 * file : IAIOPerf.cpp
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "IAIOPerf.h"

#define INVALID_DEVHDLR					int(-1)

int OpenDevice(const char* name)
{
	int fd	= INVALID_DEVHDLR;

	if( (fd = open(name,O_RDWR | O_DIRECT)) < 0 )
	{
		printf("ERROR: Cannot open device: %s\n",name);
		return INVALID_DEVHDLR;
	}

	return fd;
}

IAIOPerf::IAIOPerf(const char* devname)
	:mFD(INVALID_DEVHDLR),mpWorker(NULL),mQDepth(DEF_AIO_QDEPTH),mNumOfQ(DEF_AIO_NUMOFQ),
	 mDataPattern(DEF_AIO_PATTERN),mXferSize(DEF_AIO_XFERSIZE)
{
	mFD		 = OpenDevice(devname);
	if(INVALID_DEVHDLR != mFD)
	{
		mpWorker = new BasicPerfWorker((uint64_t)mFD);
		if(NULL != mpWorker)
		{
			mpWorker->SetPerfEngine(&mPerfEngine);
		}
	}
}

IAIOPerf::~IAIOPerf(void)
{
	LOGDEBUG("~IAIOPerf");

	if(NULL != mpWorker)
	{
		delete mpWorker;
		mpWorker = NULL;
	}

	if(INVALID_DEVHDLR != mFD)
	{
		close(mFD);
		mFD = INVALID_DEVHDLR;
	}
}

PerfResult* IAIOPerf::GetPerfResult(int errCode)
{
	PerfResult*	pRslt = new PerfResult();

	pRslt->mErrorCode	= errCode;
	pRslt->mIOPerSec	= mpWorker->GetStatIOPS();
	pRslt->mMBPerSec	= mpWorker->GetStatMBps();
	pRslt->mNumOfErrs	= mpWorker->GetIOErrCnt();

	return pRslt;
}

void IAIOPerf::ConfigPerfCfg(BasicPerfConfig &cfg,uint32_t duration)
{
	cfg.mNumOfQueues	= mNumOfQ;
	cfg.mQueueDepth		= mQDepth;
	cfg.mDuration		= duration;
	cfg.mbErrPrint		= true;
}

PerfResult* IAIOPerf::PerfItemExec(enTaskMode mode,uint64_t start,uint64_t length,uint32_t duration)
{
	BasicPerfTask 		task(start,length,mXferSize,mDataPattern,mode);
	BasicPerfConfig		config;

	ConfigPerfCfg(config,duration);

	mpWorker->SetPerfTask(&task);
	mpWorker->SetPerfConfig(&config);

	int errCode	= mpWorker->Execute();
	return GetPerfResult(errCode);
}

PerfResult* IAIOPerf::SeqWrite(uint64_t start,uint64_t length,uint32_t duration)
{
	return PerfItemExec(PERF_T_SEQ_WR,start,length,duration);
}

PerfResult* IAIOPerf::SeqRead(uint64_t start,uint64_t length,uint32_t duration)
{
	return PerfItemExec(PERF_T_SEQ_RD,start,length,duration);
}

PerfResult* IAIOPerf::RandWrite(uint64_t start,uint64_t length,uint32_t duration)
{
	return PerfItemExec(PERF_T_RND_WR,start,length,duration);
}

PerfResult* IAIOPerf::RandRead(uint64_t start,uint64_t length,uint32_t duration)
{
	return PerfItemExec(PERF_T_RND_RD,start,length,duration);
}
