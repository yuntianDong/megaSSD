/*
 * file : PerfWorker.cpp
 */

#include "perf/PerfWorker.h"

BasicPerfWorker::BasicPerfWorker(uint64_t devHdlr)
		:mDevHdlr(devHdlr),mWkID(0),mpPerfTask(NULL)
{
}

int BasicPerfWorker::Execute(void)
{
	int rtnCode	= PW_NO_ERROR;

	if(INVALID_DEV_HANDLER == mDevHdlr)
	{
		return PW_E_INVALID_DEVHDLR;
	}

	if(false == tearUp(*mpPerfEngine))
	{
		return PW_E_TEARUP_FAIL;
	}

	if(NULL == mpPerfTask)
	{
		return PW_E_INVALID_TASK;
	}

	rtnCode	= mpPerfEngine->Run(mDevHdlr,mWkID,mpPerfTask,mpPerfConfig,&mPerfStat);

	tearDown(*mpPerfEngine);

	return rtnCode;
}
