/*
 * file : PerfEngine.c
 */

#include "perf/PerfEngine.h"
#include "perf/PerfUtility.h"
#include "Logging.h"

int perf_main_loop(uint32_t wID,int fd,PerfParam* param,PerfEngineOp* ops,PerfStat* stat)
{
	int rtn 			 = 1;
	uint64_t	eclapsed = 0;
	struct timespec		tsStart;
	struct timespec		tsFinish;

	uint32_t	uTotalExec	= param->uExecTime;
	char		bDataVerify	= param->opt.bDataVerify;
	char		bErrAborted	= param->opt.bErrAborted;

	PerfContext	mPerfContext;
	InitPerfContext(&mPerfContext,wID);
	ConfigFileHandler(&mPerfContext,fd);
	ConfigIOQueue(&mPerfContext,param->uNumOfQueue,param->uQueueDepth);
	ConfigOptions(&mPerfContext,param->opt.bErrAborted,param->opt.bDataVerify);

	PerfQueue	mPerfQueue;
	InitQueue(&mPerfQueue,bDataVerify);

	InitPerfStat(stat);

	if(0 == ops->setup(&mPerfContext,param))
	{
		return -1;
	}

	GetCurrentTimestamp(&tsStart);
	while(eclapsed < uTotalExec)
	{
		LOGDEBUG("%03d - %03d - %03d - %03d : %03d",
				GetFreeQNodeCount(&mPerfQueue),
				GetDataQNodeCount(&mPerfQueue),
				GetUsingQNodeCount(&mPerfQueue),
				GetDoneQNodeCount(&mPerfQueue),
				NumOfPendingIO(stat));

		if(0 > ops->prepare_io(&mPerfContext,&mPerfQueue,&(param->sPerfTask)))
		{
			rtn = -1;
			LOGERROR("Fail to call ops->prepare_io()");
			if(1 == bErrAborted){break;};
		}

		if(0 > ops->submit_io(&mPerfContext,&mPerfQueue,stat))
		{
			rtn = -1;
			LOGERROR("Fail to call ops->submit_io()");
			if(1 == bErrAborted){break;};
		}

		if(0 > ops->check_io(&mPerfContext,&mPerfQueue,stat))
		{
			rtn = -1;
			LOGERROR("Fail to call ops->check_io()");
			if(1 == bErrAborted){break;};
		}

		if(1 == bDataVerify && 0 > ops->verify_io(&mPerfContext,&mPerfQueue,stat))
		{
			rtn = -1;
			LOGERROR("Fail to call ops->verify_io()");
			if(1 == bErrAborted){break;};
		}

		GetCurrentTimestamp(&tsFinish);
		eclapsed = GetTimeEclapsed(&tsFinish,&tsStart) / NSEC_PER_SEC;
		LOGDEBUG("eclapsed=%d,uTotalExec=%d",eclapsed,uTotalExec);
	}

	while(0 < NumOfPendingIO(stat) && eclapsed < (uTotalExec+5))
	{
		LOGDEBUG("uIOCompleted=%d,uIOSubmitted=%d",stat->uIOCompleted,stat->uIOSubmitted);
		if(0 > ops->check_io(&mPerfContext,&mPerfQueue,stat))
		{
			rtn = -1;
			LOGERROR("Fail to call ops->check_io()");
			if(1 == bErrAborted){break;};
		}

		if(1 == bDataVerify && 0 > ops->verify_io(&mPerfContext,&mPerfQueue,stat))
		{
			rtn = -1;
			LOGERROR("Fail to call ops->verify_io()");
			if(1 == bErrAborted){break;};
		}

		GetCurrentTimestamp(&tsFinish);
		eclapsed = GetTimeEclapsed(&tsFinish,&tsStart) / NSEC_PER_SEC;
	}

	GetCurrentTimestamp(&tsFinish);
	UptEclapsedTime(stat,GetTimeEclapsed(&tsFinish,&tsStart));

	if(0 != NumOfPendingIO(stat))
	{
		LOGERROR("There is unfinished IO...");
		LOGERROR("uIOCompleted=%d,uIOSubmitted=%d",stat->uIOCompleted,stat->uIOSubmitted);
	}

	ops->cleanup(&mPerfContext);
	return rtn;
}
