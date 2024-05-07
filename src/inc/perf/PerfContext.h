#ifndef _PERF_CONTEXT_H
#define _PERF_CONTEXT_H

#include <stdint.h>
#include <libaio.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>

#include "PerfTask.h"

#define  BUFPOOL_MAX_SIZE			1*1024*1024		// 256MB
#define  BUFPOOL_MAX_ITEMS			1024
#define	 BUFFER_ALIGN_SIZE			4096

typedef struct _stPerfContext
{
	uint32_t	uWorkerID;
	uint32_t	uNumOfQueue;
	uint32_t	uQueueDepth;
	uint32_t	uSectorSize;

	struct {
		char	bErrAborted	: 1;
		char	bDataVerify	: 1;
	}o;

	union
	{
		void*		dev;
		int			fd;
	}d;

	struct {
		uint8_t*	bufpool[BUFPOOL_MAX_ITEMS];
		uint32_t	bufSize;
	}b;

	union
	{
		struct
		{
			struct io_event		*events;
			io_context_t		ctx;
			struct iocb			iocb;		// cache iocb node
			struct iocb**		ppiocbs;
		}aio;
	}u;

	union
	{
		struct
		{
			int		epollfd;
			int		eventfd;
			struct epoll_event  epollEvt;
		}epoll;
	}e;
}PerfContext;

#ifdef __cplusplus
extern "C" {
#endif

int InitPerfContext(PerfContext* context,uint32_t wID);
void DeinitPerfContext(PerfContext* context);
void ConfigFileHandler(PerfContext* context,int fd);
void ConfigIOQueue(PerfContext* context,uint32_t numOfQ,uint32_t qDepth);
void ConfigOptions(PerfContext* context,char bErrAborted,char bDataVerify);
int InitAIOContext(PerfContext* context,uint32_t numEvents);
void DeinitAIOContext(PerfContext* context);
void InitAIOCacheIOCB(PerfContext* context,PerfTask* task);
int InitEpollEvent(PerfContext* context,int evtFlags,uint32_t events);
void DeinitEpollEvent(PerfContext* context);
int EpollWaitEvent(PerfContext* context,uint32_t timeout);
int EpollUptEvent(PerfContext* context);
void BindingEvent(PerfContext* context,struct iocb* iocb);

int InitBufPool(PerfContext* context,uint32_t bufSize);
void DeinitBufPool(PerfContext* context);
void FillBufPool(PerfContext* context,uint32_t patVal);
void GetBufFromPool(PerfContext* context,uint32_t bufIdx,uint8_t** bufAddr);

#ifdef __cplusplus
}
#endif

#endif
