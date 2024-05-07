/*
 * file : AioPerfEngine.c
 */

#include "perf/aio/AioPerfEngine.h"
#include "Logging.h"

#define	MAX_PENDING_IOS			32
#define MIN_PENDING_IOS			8
#define MAX_EVENT_HDLR_CNT		8

int aio_setup(PerfContext* context,PerfParam* param)
{
	LOGDEBUG("aio_setup");
	uint32_t numEvts = param->uQueueDepth;

	InitAIOContext(context,numEvts);
	InitAIOCacheIOCB(context,&(param->sPerfTask));
	InitBufPool(context,param->sPerfTask.lXferMax);
	FillBufPool(context,param->sPerfTask.lDataPat);

	return 1;
}

void aio_cleanup(PerfContext* context)
{
	LOGDEBUG("aio_cleanup");
	DeinitAIOContext(context);
	DeinitBufPool(context);
}

int aio_prepare_io(PerfContext* context,PerfQueue* queue,PerfTask* task)
{
	uint32_t 	qDepth		= context->uQueueDepth;
	uint32_t	freeNodeCnt	= GetFreeNodeCnt(queue);
	uint32_t	dataNodeCnt = GetPendingNodeCnt(queue);
	if( 0 < freeNodeCnt && qDepth * 2 < dataNodeCnt)
	{
		return 1;
	}

	LOGDEBUG("aio_prepare_io");
	uint32_t 	hdlrCmdCnt	= GetUsingQNodeCount(queue);
	int	        count		= qDepth - dataNodeCnt - hdlrCmdCnt;
	count = (count > (int)freeNodeCnt)?(int)freeNodeCnt:count;
	count = (0 >= count)?1:count;

	uint32_t		wID		= context->uWorkerID;

	while(count--)
	{
		struct iocb*	iocb	= &(context->u.aio.iocb);
		uint32_t		bufIdx	= GetNextAddingQNodeIndex(queue);

		GetOpOffset(task,wID,(uint64_t*)&(iocb->u.c.offset));
		GetBufFromPool(context,bufIdx,(uint8_t**)&(iocb->u.c.buf));
		if(task->opt.bRandXfer)
		{
			GetOpXferSize(task,wID,(uint32_t*)&(iocb->u.c.nbytes));
		}
		if(task->opt.bAddrOverlay)
		{
			UptOpDataBuf(task,(uint8_t*)iocb->u.c.buf,iocb->u.c.nbytes,iocb->u.c.offset,context->uSectorSize);
		}

		AddQueueIOCBNode(queue,iocb);
	}

	return 1;
}

int aio_submit_io(PerfContext* context,PerfQueue* queue,PerfStat* stat)
{
	uint32_t maxPendingIOs	= context->uQueueDepth;
	uint32_t numPendingNodes= GetPendingNodeCnt(queue);
	uint32_t numPendingIOs	= NumOfPendingIO(stat);

	uint32_t count			= maxPendingIOs - numPendingIOs;
	count	= (count > numPendingNodes)?numPendingNodes:count;
	if( 0 >= count || ( maxPendingIOs > (numPendingNodes + numPendingIOs) ) )
	{
		return 1;
	}

	LOGDEBUG("aio_submit_io");

	struct iocb**	iocb	= context->u.aio.ppiocbs;
	if(NULL == iocb)
	{
		LOGERROR("Fail to Allocate Memory!");
		return -1;
	}
	uint32_t idx;
	for(idx=0;idx<(uint32_t)count;idx++)
	{
		if(0 >= FetchIOCBNodeToSubmit(queue,&(iocb[idx])))
		{
			LOGERROR("Fail to call FetchOneIOCBNode()");
			return -1;
		}

		LOGDEBUG("aio_fildes=%d,aio_submit_io:op=%d,offset=%llx,length=%llx,buf=%llx,resfd = %d",
				iocb[idx]->aio_fildes,iocb[idx]->aio_lio_opcode,iocb[idx]->u.c.offset,
				iocb[idx]->u.c.nbytes,iocb[idx]->u.c.buf,iocb[idx]->u.c.resfd);
		LOGDEBUG("iocb[idx]->u.c.buf[0] = %x",*(uint32_t*)(iocb[idx]->u.c.buf));
	}

	int rtn = io_submit(context->u.aio.ctx, count, iocb);
	if(0 >= rtn)
	{
		LOGERROR("Fail to call io_submit(),rtn=%d",rtn);
		return -1;
	}

	IncCntIOSubmitted(stat,rtn);

	return 1;
}

void io_completed_callback(PerfQueue* queue,int error,PerfQNode* qNode,PerfStat* stat)
{
	if(error)
	{
		IncCntIOError(stat,1);
	}

	IncCntIOCompleted(stat,1);
	MarkQNodeCompleted(queue,qNode);
}

int aio_check_io(PerfContext* context,PerfQueue* queue,PerfStat* stat)
{
	LOGDEBUG("aio_check_io");
	struct timespec timeout;

	timeout.tv_sec 	= 0;
	timeout.tv_nsec = 100000;

	int count = io_getevents(context->u.aio.ctx, 1, MAX_EVENT_HDLR_CNT, context->u.aio.events, &timeout); //&timeout);
	if(0 >= count)
	{
		return 0;
	}

	int idx;
	for(idx=0;idx<count;idx++)
	{
		io_completed_callback(queue,context->u.aio.events[idx].res2,(PerfQNode*)(context->u.aio.events[idx].obj),stat);
	}

	return 1;
}

int aio_verify_io(PerfContext* context,PerfQueue* queue,PerfStat* stat)
{
	LOGDEBUG("aio_verify_io");
	return 1;
}

