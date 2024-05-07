/*
 * file : PerfEngine.cpp
 */

#include <assert.h>

#include "perf/PerfEngine.h"
#include "Logging.h"

BasicPerfEngine::BasicPerfEngine(void)
		:mpContext(NULL),mbIsBlocked(false),mbNeedVerify(false),
		 mbStopWhenError(false),mbTimeBased(false)
{
	mDebugTimer1 = mDebugTimer2 = mDebugTimer3;
}

BasicPerfEngine::~BasicPerfEngine(void)
{
	LOGDEBUG("~BasicPerfEngine");
	if(NULL != mpContext)
	{
		LOGDEBUG("delete mpContext");

		delete mpContext;
		mpContext=NULL;
	}
}

bool BasicPerfEngine::IsCmdQueueEmpty(void)
{
	return mpContext->mIOSubmitted <= mpContext->mIOCompleted;
}

bool BasicPerfEngine::IsCmdQueueFull(void)
{
	return GetQueueMaxIO() <= GetInQueueCnt();
}

uint32_t BasicPerfEngine::GetInQueueCnt(void)
{
	return (uint32_t)(mpContext->mIOSubmitted - mpContext->mIOCompleted);
}

uint32_t BasicPerfEngine::GetQueueMaxIO(void)
{
	return mpContext->mpPerfCfg->mQueueDepth;
}

BasicPerfContext* BasicPerfEngine::GetPerfContext(void)
{
	if(NULL == mpContext)
	{
		mpContext = new BasicPerfContext();
	}

	return mpContext;
}

void BasicPerfEngine::io_completed(bool error,void* data)
{
	if(true == error)
	{
		this->GetPerfContext()->mIOErrors++;
	}
}

bool BasicPerfEngine::init_engine(void)
{
	return true;
}

void BasicPerfEngine::clean_engine(void)
{
	return;
}

void BasicPerfEngine::check_io_complete(void)
{
	return;
}

bool BasicPerfEngine::submit_io(void)
{
	return true;
}

bool BasicPerfEngine::verify_io(void)
{
	return true;
}

int BasicPerfEngine::Run(uint64_t devHdlr,uint32_t wId,BasicPerfTask* task,
		BasicPerfConfig* cfg,BasicPerfStat* stat)
{
	LOGDEBUG("BasicPerfEngine::Run");

	int rtnCode	= PW_NO_ERROR;

	this->GetPerfContext()->mDevHdlr		= devHdlr;
	this->GetPerfContext()->mWorkerID		= wId;
	this->GetPerfContext()->mpPerfTask		= task;
	this->GetPerfContext()->mpPerfCfg		= cfg;

	if(true != this->init_engine())
	{
		LOGERROR("Fail to Init Engine!");
		return PW_E_FAIL_INIT_ENGINE;
	}

	bool bRtn = submit_io();
	uint64_t	eclapsed	= 0;
	uint64_t	timer1,timer2;
	timer1=timer2=0;
	while( (true == bRtn) && (eclapsed <= ((uint64_t)(cfg->mDuration) * (uint64_t)NSEC_PER_SEC)) )
	{
		SysClkTimer timer(eclapsed,TU_NS);

		if( false == IsCmdQueueEmpty() )
		{
			SysClkTimer t1(timer1,TU_NS);
			this->check_io_complete();
		}

		if(true == mbNeedVerify)
		{
			if(false == this->verify_io() && true == mbStopWhenError)
			{
				LOGERROR("IO Verification Error!");
				rtnCode = PW_E_FAIL_IO_VERIFY;
				break;
			}
		}

		if(false == IsCmdQueueFull())
		{
			SysClkTimer t2(timer2,TU_NS);
			bRtn = this->submit_io();
		}
	}

	LOGINFO("%lld = %lld + %lld",eclapsed,timer1,timer2);
	LOGINFO("%lld, %lld, %lld",mDebugTimer1,mDebugTimer2,mDebugTimer3);

	while(false == mbTimeBased && false == IsCmdQueueEmpty())
	{
		SysClkTimer timer(eclapsed,TU_NS);

		this->check_io_complete();

		if(true == mbNeedVerify)
		{
			if(false == this->verify_io() && true == mbStopWhenError)
			{
				LOGERROR("IO Verification Error!");
				rtnCode = PW_E_FAIL_IO_VERIFY;
				break;
			}
		}
	}

	stat->mTotalIO	= this->GetPerfContext()->mIOCompleted;
	stat->mDuration	= eclapsed / ((uint64_t)NSEC_PER_SEC/MSEC_PER_SEC);
	stat->mTotalKB	= this->GetPerfContext()->mIOCompleted * task->GetXferSize();
	stat->mIOErrors	= this->GetPerfContext()->mIOErrors;

	this->clean_engine();

	return rtnCode;
};
