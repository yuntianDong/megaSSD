/*
 * file : PerfContext.c
 */

#include <unistd.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "perf/PerfContext.h"
#include "Logging.h"

#define INVALID_DEVHDLR					-1

int InitBufPool(PerfContext* context,uint32_t bufSize)
{
	if(bufSize % BUFFER_ALIGN_SIZE)
	{
		return 0;
	}

	uint16_t idx=0;
	for(idx=0;idx<BUFPOOL_MAX_ITEMS;idx++)
	{
		int err = posix_memalign((void**)&(context->b.bufpool[idx]),BUFFER_ALIGN_SIZE,bufSize);
		if(err)
		{
			context->b.bufpool[idx]	= NULL;
			return 0;
		}
	}

	context->b.bufSize		= bufSize;

	return 1;
}

void DeinitBufPool(PerfContext* context)
{
	uint16_t idx=0;
	for(idx=0;idx<BUFPOOL_MAX_ITEMS;idx++)
	{
		if(NULL != context->b.bufpool[idx])
		{
			free(context->b.bufpool[idx]);
			context->b.bufpool[idx] = NULL;
		}
	}
}

int InitPerfContext(PerfContext* context,uint32_t wID)
{
	memset(context,0,sizeof(PerfContext));

	context->uWorkerID	= wID;

	return 1;
}

void DeinitPerfContext(PerfContext* context)
{
	DeinitBufPool(context);
}

void ConfigFileHandler(PerfContext* context,int fd)
{
	context->d.fd	= fd;
}

void ConfigIOQueue(PerfContext* context,uint32_t numOfQ,uint32_t qDepth)
{
	context->uNumOfQueue	= numOfQ;
	context->uQueueDepth	= qDepth;
}

void ConfigOptions(PerfContext* context,char bErrAborted,char bDataVerify)
{
	context->o.bErrAborted	= bErrAborted;
	context->o.bDataVerify	= bDataVerify;
}

int InitAIOContext(PerfContext* context,uint32_t numEvents)
{
	context->u.aio.events	= (struct io_event*)calloc(numEvents, sizeof(struct io_event));
	if(NULL == context->u.aio.events)
	{
		return 0;
	}
	context->u.aio.ctx		= 0;

	if(0 > io_setup(numEvents,&(context->u.aio.ctx)))
	{
		return 0;
	}

	context->u.aio.ppiocbs	= (struct iocb**)malloc(numEvents*sizeof(struct iocb*));
	if(NULL != context->u.aio.ppiocbs)
	{
		return 0;
	}

	return 1;
}

void DeinitAIOContext(PerfContext* context)
{
	io_destroy(context->u.aio.ctx);

	if(NULL != context->u.aio.events)
	{
		free(context->u.aio.events);
		context->u.aio.events = NULL;
	}

	if(NULL != context->u.aio.ppiocbs)
	{
		free(context->u.aio.ppiocbs);
		context->u.aio.ppiocbs	= NULL;
	}
}

void InitAIOCacheIOCB(PerfContext* context,PerfTask* task)
{
	struct iocb*	iocb	= &(context->u.aio.iocb);
	enTaskMode		mode	= task->eTaskMode;
	int			eventfd		= context->e.epoll.eventfd;

	int 	 fd	 = context->d.fd;
	//uint8_t* bufAddr 	= NULL;
	//uint32_t bufSize	= 0;
	uint64_t offset	 	= 0;
	uint32_t xferSize	= 0;

	//GetOpDataBuf(task,0,&bufAddr,&bufSize);
	GetOpOffset(task,0,&offset);
	GetOpXferSize(task,0,&xferSize);

	if(PERF_T_SEQ_RD == mode || PERF_T_RND_RD == mode)
	{
		io_prep_pread(iocb,fd,NULL,xferSize,offset);
	}
	else if(PERF_T_SEQ_WR == mode || PERF_T_RND_WR == mode)
	{
		io_prep_pwrite(iocb,fd,NULL,xferSize,offset);
	}
	else
	{
		assert(0);
	}
}

int InitEpollEvent(PerfContext* context,int evtFlags,uint32_t events)
{
	context->e.epoll.epollfd	= epoll_create(1);
	if(INVALID_DEVHDLR == context->e.epoll.epollfd)
	{
		LOGERROR("Fail to epoll_create()");
		return -1;
	}

	context->e.epoll.eventfd	= eventfd(0, evtFlags);
	if(INVALID_DEVHDLR == context->e.epoll.eventfd)
	{
		LOGERROR("Fail to eventfd()");
		return -1;
	}

	context->e.epoll.epollEvt.events	= events;
	context->e.epoll.epollEvt.data.ptr 	= NULL;

	if (epoll_ctl(context->e.epoll.epollfd, EPOLL_CTL_ADD,\
			context->e.epoll.eventfd, &(context->e.epoll.epollEvt)))
	{
		LOGERROR("Fail to epoll_ctl()");
		return -1;
	}

	return 1;
}

void DeinitEpollEvent(PerfContext* context)
{
	if(INVALID_DEVHDLR != context->e.epoll.eventfd)
	{
		close(context->e.epoll.eventfd);
		context->e.epoll.eventfd = INVALID_DEVHDLR;
	}

	if(INVALID_DEVHDLR != context->e.epoll.epollfd)
	{
		close(context->e.epoll.epollfd);
		context->e.epoll.epollfd = INVALID_DEVHDLR;
	}
}

int EpollWaitEvent(PerfContext* context,uint32_t timeout)
{
	int epollfd					= context->e.epoll.epollfd;
	int eventfd					= context->e.epoll.eventfd;
	struct epoll_event* epEvt	= &(context->e.epoll.epollEvt);

	if( 0 >= epoll_wait(epollfd,epEvt,1,timeout) )
	{
		return 0;
	}

	LOGDEBUG("epollfd=%d,eventfd=%d,epEvt=%llx",epollfd,eventfd,epEvt);

	uint64_t evtVal	= 0;
	if(sizeof(uint64_t) != read(eventfd,&evtVal,sizeof(uint64_t)))
	{
		LOGERROR("Fail to read()");
		return -1;
	}

	epoll_ctl(epollfd,EPOLL_CTL_MOD,eventfd,epEvt);

	LOGDEBUG("evtVal=%d",evtVal);
	return evtVal;
}

void BindingEvent(PerfContext* context,struct iocb* iocb)
{
	int eventfd		= context->e.epoll.eventfd;
	io_set_eventfd(iocb, eventfd);
}

void FillBufPool(PerfContext* context,uint32_t patVal)
{
	uint32_t idx,idx2;
	uint32_t bufSize	= context->b.bufSize;

	uint8_t* buf = context->b.bufpool[0];

	for(idx2=0;idx2<(bufSize / sizeof(uint32_t));idx2++)
	{
		*((uint32_t*)buf + idx2)	= patVal;
	}

	for(idx=1;idx<BUFPOOL_MAX_ITEMS;idx++)
	{
		memcpy(context->b.bufpool[idx],buf,bufSize);
	}
}

void GetBufFromPool(PerfContext* context,uint32_t bufIdx,uint8_t** bufAddr)
{
	*bufAddr	= context->b.bufpool[bufIdx];
}
