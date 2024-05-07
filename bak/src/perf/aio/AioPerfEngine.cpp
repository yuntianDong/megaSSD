/*
 * file : AioPerfEngine.cpp
 */

#include <stdlib.h>
#include <assert.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "perf/AioPerfEngine.h"
#include "Logging.h"

#define INVALID_DEVHDLR					int(-1)
#define EPOLL_SIZE						1

AioPerfEngine::AioPerfEngine(void)
	:mpIOCBs(NULL),mMaxIOsPerOnce(MAX_IO_PER_CMD),mEventFd(INVALID_DEVHDLR)
{
	mpIOCBs		= (struct iocb*)calloc(mMaxIOsPerOnce,sizeof(struct iocb));
	memset(mpIOCBs,0,mMaxIOsPerOnce*sizeof(struct iocb));

	InitEpEvent();
}

AioPerfEngine::~AioPerfEngine(void)
{
	LOGDEBUG("~AioPerfEngine");

	if(NULL != mpIOCBs)
	{
		free(mpIOCBs);
		mpIOCBs = NULL;
	}

	if(NULL != mpContext)
	{
		LOGDEBUG("delete mpContext");
		delete mpContext;
		mpContext = NULL;
	}

	DeinitEpEvent();
}

void AioPerfEngine::InitEpEvent(void)
{
	mEventFd	= eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	mEpollFd	= epoll_create(EPOLL_SIZE);

	mpEPEvent	= (struct epoll_event*)calloc(1,sizeof(struct epoll_event));

	mpEPEvent->events 		= EPOLLIN | EPOLLET;
	mpEPEvent->data.ptr 	= NULL;

	if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mEventFd, mpEPEvent))
	{
		LOGERROR("Fail to add EventEd to Epoll!");
		assert(false);
	}
}

void AioPerfEngine::DeinitEpEvent(void)
{
	if(NULL != mpEPEvent)
	{
		free(mpEPEvent);
		mpEPEvent = NULL;
	}

	if(INVALID_DEVHDLR != mEventFd)
	{
		close(mEventFd);
		mEventFd = INVALID_DEVHDLR;
	}

	if(INVALID_DEVHDLR != mEpollFd)
	{
		close(mEpollFd);
		mEpollFd = INVALID_DEVHDLR;
	}
}

BasicPerfContext* AioPerfEngine::GetPerfContext(void)
{
	if(NULL == mpContext)
	{
		mpContext = (BasicPerfContext*)new AioPerfContext();
	}

	return mpContext;
}

void AioPerfEngine::GetIOEvent(uint32_t msTimeout)
{
	struct timespec timeout;

	timeout.tv_sec 	= msTimeout / MSEC_PER_SEC;
	timeout.tv_nsec = (msTimeout % MSEC_PER_SEC) * (NSEC_PER_SEC/MSEC_PER_SEC);

	AioPerfContext* pAioContext	= dynamic_cast<AioPerfContext*>(this->GetPerfContext());
	assert(NULL != pAioContext && NULL != pAioContext->mpPerfCfg);

	uint16_t numOfQ	= pAioContext->mpPerfCfg->mNumOfQueues;

	int count = io_getevents(pAioContext->mAIOCtx, 1, numOfQ, pAioContext->mpAIOEvents, &timeout);
	assert(0 <= count);

	this->GetPerfContext()->mIOCompleted += count;

	for(int i=0;i<count;i++)
	{
		this->io_completed(pAioContext->mpAIOEvents[i].res2,pAioContext->mpAIOEvents[i].obj);
	}
}

void AioPerfEngine::io_completed(bool error,void* data)
{
	if(error)
	{
		BasicPerfEngine::io_completed(error,data);

		if(NULL != this->GetPerfContext()->mpPerfCfg && true == this->GetPerfContext()->mpPerfCfg->mbErrPrint)
		{
			struct iocb*	iocb = (struct iocb*)data;
			LOGERROR( "IO[%16x:%8x] %2s HAS ERROR!!!",iocb->u.c.offset,iocb->u.c.nbytes,
					(IO_CMD_PREAD==iocb->aio_lio_opcode)?"RD":"WR");
		}
	}
}

bool AioPerfEngine::init_engine(void)
{
	LOGDEBUG("AioPerfEngine::init_engine");

	AioPerfContext* pAioContext	= dynamic_cast<AioPerfContext*>(this->GetPerfContext());

	assert(NULL != pAioContext && NULL != pAioContext->mpPerfCfg);

	uint16_t numOfEvts	= pAioContext->mpPerfCfg->mQueueDepth;
	pAioContext->mpAIOEvents	= (struct io_event*)calloc(numOfEvts, sizeof(struct io_event));
	memset(pAioContext->mpAIOEvents,0,numOfEvts * sizeof(struct io_event));

	if(NULL == pAioContext->mpAIOEvents)
	{
		LOGERROR("Fail to allocate io_event memory!");
		return false;
	}

	pAioContext->mAIOCtx	= 0;
	if(0 > io_setup(numOfEvts,&(pAioContext->mAIOCtx)))
	{
		LOGERROR("Fail to initialize LibAIO");
		return false;
	}

	return BasicPerfEngine::init_engine();;
}

void AioPerfEngine::clean_engine(void)
{
	LOGDEBUG("AioPerfEngine::clean_engine");

	AioPerfContext* pAioContext	= dynamic_cast<AioPerfContext*>(this->GetPerfContext());

	assert(NULL != pAioContext);

	io_destroy(pAioContext->mAIOCtx);

	if(NULL != pAioContext->mpAIOEvents)
	{
		free(pAioContext->mpAIOEvents);
		pAioContext->mpAIOEvents = NULL;
	}

	BasicPerfEngine::clean_engine();
}

void AioPerfEngine::check_io_complete(void)
{
	LOGDEBUG("AioPerfEngine::check_io_complete");
	if( 0 >= epoll_wait(mEpollFd,mpEPEvent,1,0) )
	{
		return;
	}

	uint64_t finishCnt = 0;
	if(sizeof(uint64_t) != read(mEventFd,&finishCnt,sizeof(uint64_t)))
	{
		LOGERROR("Fail to Read from mEventFd");
		assert(false);
	}

	if(finishCnt > 0)
	{
		GetIOEvent(0);
	}

	epoll_ctl(mEpollFd,EPOLL_CTL_MOD,mEventFd,mpEPEvent);
	return;
}

bool AioPerfEngine::submit_io(void)
{
	LOGDEBUG("AioPerfEngine::submit_io");

	AioPerfContext* pAioContext	= dynamic_cast<AioPerfContext*>(this->GetPerfContext());
	assert(NULL != pAioContext && NULL != pAioContext->mpPerfTask);

	uint32_t numOfIOs	= pAioContext->mpPerfCfg->mQueueDepth - GetInQueueCnt();
	numOfIOs	= (numOfIOs > mMaxIOsPerOnce)?mMaxIOsPerOnce:numOfIOs;
	for(uint32_t idx=0;idx<numOfIOs;idx++)
	{
		SysClkTimer TT1(mDebugTimer1,TU_NS);

		PerfOper	io_op;

		if(false == pAioContext->mpPerfTask->GetNextOper(io_op))
		{
			return false;
		}

		SysClkTimer TT2(mDebugTimer2,TU_NS);

		mpIOCBs[idx].aio_fildes			= (int)(pAioContext->mDevHdlr);
		mpIOCBs[idx].aio_reqprio 		= 0;
		mpIOCBs[idx].aio_lio_opcode 	= (OP_READ==io_op.opType)?IO_CMD_PREAD:IO_CMD_PWRITE;
		mpIOCBs[idx].u.c.buf 			= io_op.buf;
		mpIOCBs[idx].u.c.nbytes 		= io_op.nbufs;
		mpIOCBs[idx].u.c.offset 		= io_op.offset;

		io_set_eventfd(&(mpIOCBs[idx]), mEventFd);
	}

	SysClkTimer TT3(mDebugTimer3,TU_NS);

	int count = io_submit(pAioContext->mAIOCtx, numOfIOs, &mpIOCBs);
	if (count < 0)
	{
		LOGERROR("IO Submmit Error!");
		return false;
	}

	this->GetPerfContext()->mIOSubmitted += count;

	return true;
}

bool AioPerfEngine::verify_io(void)
{
	LOGDEBUG("AioPerfEngine::verify_io");

	return BasicPerfEngine::verify_io();
}

