/*
 * file : IAIOPerf.cpp
 */
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "IAIOPerf.h"
#include "Logging.h"

#define INVALID_DEVHDLR			-1

#define DEF_ARG_SIZE		1*1024*1024*1024
#define DEF_ARG_XFERSIZE	128*1024
#define DEF_ARG_PATTERN		0xCA5775AC
#define DEF_ARG_NUMOFQ		4
#define DEF_ARG_QDEPTH		32


#define AIO_ENGINE_OPS		{\
			.setup		= aio_setup,\
			.cleanup	= aio_cleanup,\
			.prepare_io = aio_prepare_io,\
			.submit_io	= aio_submit_io,\
			.check_io	= aio_check_io,\
			.verify_io	= aio_verify_io,\
	};\

void IAIOPerf::OpenFile(const char* filename,bool bDirect)
{
	int	flags	= O_RDWR;
	if(true == bDirect)
	{
		flags |= O_DIRECT;
	}

	mFD	= open(filename,flags);
	if( 0 > mFD )
	{
		LOGERROR("ERROR: Cannot open device: %s\n",filename);
		mFD	= INVALID_DEVHDLR;
	}
}

IAIOPerf::IAIOPerf(const char* filename,bool bDirect)
	:mFD(INVALID_DEVHDLR)
{
	OpenFile(filename,bDirect);

	muXferSize		= DEF_ARG_XFERSIZE;
	muDataPattern	= DEF_ARG_PATTERN;
	muQueueDepth	= DEF_ARG_QDEPTH;
	muNumOfQueue	= DEF_ARG_NUMOFQ;
}

IAIOPerf::~IAIOPerf(void)
{
	if(INVALID_DEVHDLR != mFD)
	{
		close(mFD);
		mFD	= INVALID_DEVHDLR;
	}
}

IAIOResult* IAIOPerf::DoPerfTest(uint64_t offset,uint64_t size,bool bSeq,
		bool bRead,uint32_t execTime)
{
	assert(INVALID_DEVHDLR != mFD);

	PerfParam		perfParam;
	InitPerfParam(&perfParam,muNumOfQueue,muQueueDepth,execTime);
	SetErrAborted(&perfParam,1);

	InitPerfTask(&(perfParam.sPerfTask));

	if(true == bSeq && true == bRead)
	{
		FillTaskWSeqRead(&(perfParam.sPerfTask),offset,size,muXferSize,muDataPattern);
	}
	else if(true == bSeq and false == bRead)
	{
		FillTaskWSeqWrite(&(perfParam.sPerfTask),offset,size,muXferSize,muDataPattern);
	}
	else if(false == bSeq and true == bRead)
	{
		FillTaskWRandRead(&(perfParam.sPerfTask),offset,size,muXferSize,muDataPattern);
	}
	else
	{
		FillTaskWRandWrite(&(perfParam.sPerfTask),offset,size,muXferSize,muDataPattern);
	}

	PerfStat		perfStat;
	InitPerfStat(&perfStat);

	PerfEngineOp	ioEngine = AIO_ENGINE_OPS;

	int rtn = perf_main_loop(0,mFD,&perfParam,&ioEngine,&perfStat);
	if(0 > rtn)
	{
		LOGERROR("Fail to call perf_main_loop, errCode = %d",rtn);
		assert(0);
	}

	return new IAIOResult(&perfStat,muXferSize);
}

IAIOResult*	IAIOPerf::SeqWrite(uint64_t offset,uint64_t size,uint32_t execTime)
{
	return DoPerfTest(offset,size,true,false,execTime);
}

IAIOResult*	IAIOPerf::SeqRead(uint64_t offset,uint64_t size,uint32_t execTime)
{
	return DoPerfTest(offset,size,true,true,execTime);
}

IAIOResult*	IAIOPerf::RandWrite(uint64_t offset,uint64_t size,uint32_t execTime)
{
	return DoPerfTest(offset,size,false,false,execTime);
}

IAIOResult*	IAIOPerf::RandRead(uint64_t offset,uint64_t size,uint32_t execTime)
{
	return DoPerfTest(offset,size,false,true,execTime);
}

