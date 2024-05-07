/*
 * file : PerfQueue.c
 */

#include <string.h>
#include <assert.h>
#include "perf/PerfQueue.h"
#include "Logging.h"

void InitPerfQNode(PerfQNode* qHeadList,uint32_t totalCnt)
{
	uint32_t idx;
	for(idx=0;idx<totalCnt;idx++)
	{
		PerfQNode* qNode = &(qHeadList[idx]);
		qNode->index = idx;
		qNode->next = &(qHeadList[(idx + 1) % totalCnt]);
		qNode->prev = &(qHeadList[(idx - 1 + totalCnt) % totalCnt]);
	}
}

uint32_t GetFreeQNodeCount(PerfQueue* queue)
{
	return queue->freeQ.count;
}

uint32_t GetDataQNodeCount(PerfQueue* queue)
{
	return queue->dataQ.count;
}

uint32_t GetUsingQNodeCount(PerfQueue* queue)
{
	return queue->usingQ.count;
}

uint32_t GetDoneQNodeCount(PerfQueue* queue)
{
	return queue->doneQ.count;
}

void GetQNode(PerfQueue* queue,PerfQNode** qNode,uint32_t qIdx)
{
	*qNode	= &(queue->qNodeLst[qIdx]);
}

void FetchFreeQNode(PerfQueue* queue,PerfQNode** pqNode)
{
	if(0 >= GetFreeQNodeCount(queue) && NULL != queue->freeQ.head)
	{
		assert(0);
	}

	PerfQNode* qNode	= queue->freeQ.head;
	assert(qNode != NULL);
	if(qNode->prev == qNode)
	{
		queue->freeQ.head	= NULL;
	}
	else
	{
		qNode->prev->next	= qNode->next;
		qNode->next->prev	= qNode->prev;
		queue->freeQ.head	= qNode->next;
	}

	queue->freeQ.count			-= 1;
	qNode->next	= qNode->prev	= NULL;

	*pqNode	= qNode;
}

void FetchDataQNode(PerfQueue* queue,PerfQNode** pqNode)
{
	if(0 >= GetDataQNodeCount(queue) && NULL != queue->dataQ.head)
	{
		assert(0);
	}

	PerfQNode* qNode	= queue->dataQ.head;
	assert(qNode != NULL);
	if(qNode->prev == qNode)
	{
		queue->dataQ.head	= NULL;
	}
	else
	{
		qNode->prev->next	= qNode->next;
		qNode->next->prev	= qNode->prev;
		queue->dataQ.head	= qNode->next;
	}

	queue->dataQ.count			-= 1;
	qNode->next	= qNode->prev	= NULL;

	*pqNode	= qNode;
}

void FetchUsingQNode(PerfQueue* queue,PerfQNode** pqNode)
{
	if(0 >= GetUsingQNodeCount(queue) && NULL != queue->usingQ.head)
	{
		assert(0);
	}

	PerfQNode* qNode	= queue->usingQ.head;
	assert(qNode != NULL);
	if(qNode->prev == qNode)
	{
		queue->usingQ.head	= NULL;
	}
	else
	{
		qNode->prev->next	= qNode->next;
		qNode->next->prev	= qNode->prev;
		queue->usingQ.head	= qNode->next;
	}

	queue->usingQ.count			-= 1;
	qNode->next	= qNode->prev	= NULL;

	*pqNode	= qNode;
}

void FetchDoneQNode(PerfQueue* queue,PerfQNode** pqNode)
{
	if(0 >= GetDoneQNodeCount(queue) && NULL != queue->doneQ.head)
	{
		assert(0);
	}

	PerfQNode* qNode	= queue->doneQ.head;
	assert(qNode != NULL);
	if(qNode->prev == qNode)
	{
		queue->doneQ.head	= NULL;
	}
	else
	{
		qNode->prev->next	= qNode->next;
		qNode->next->prev	= qNode->prev;
		queue->doneQ.head	= qNode->next;
	}

	queue->doneQ.count			-= 1;
	qNode->next	= qNode->prev	= NULL;

	*pqNode	= qNode;
}

void InsertFreeQNode(PerfQueue* queue,PerfQNode* qNode)
{
	if(NULL == queue->freeQ.head)
	{
		queue->freeQ.head			= qNode;
		qNode->next	= qNode->prev	= qNode;
	}
	else
	{
		qNode->prev	= queue->freeQ.head->prev;
		qNode->next	= queue->freeQ.head;
		queue->freeQ.head->prev->next	= qNode;
		queue->freeQ.head->prev			= qNode;
	}

	qNode->qType		= QT_FREEQ;
	queue->freeQ.count	+= 1;
}

void InsertDataQNode(PerfQueue* queue,PerfQNode* qNode)
{
	if(NULL == queue->dataQ.head)
	{
		queue->dataQ.head			= qNode;
		qNode->next	= qNode->prev	= qNode;
	}
	else
	{
		qNode->prev	= queue->dataQ.head->prev;
		qNode->next	= queue->dataQ.head;
		queue->dataQ.head->prev->next	= qNode;
		queue->dataQ.head->prev			= qNode;
	}

	qNode->qType		= QT_DATAQ;
	queue->dataQ.count	+= 1;
}

void InsertUsingQNode(PerfQueue* queue,PerfQNode* qNode)
{
	if(NULL == queue->usingQ.head)
	{
		queue->usingQ.head			= qNode;
		qNode->next	= qNode->prev	= qNode;
	}
	else
	{
		qNode->prev	= queue->usingQ.head->prev;
		qNode->next	= queue->usingQ.head;
		queue->usingQ.head->prev->next	= qNode;
		queue->usingQ.head->prev			= qNode;
	}

	qNode->qType		= QT_USINGQ;
	queue->usingQ.count	+= 1;
}

void InsertDoneQNode(PerfQueue* queue,PerfQNode* qNode)
{
	if(NULL == queue->doneQ.head)
	{
		queue->doneQ.head			= qNode;
		qNode->next	= qNode->prev	= qNode;
	}
	else
	{
		qNode->prev	= queue->doneQ.head->prev;
		qNode->next	= queue->doneQ.head;
		queue->doneQ.head->prev->next	= qNode;
		queue->doneQ.head->prev			= qNode;
	}

	qNode->qType		= QT_DONEQ;
	queue->doneQ.count	+= 1;
}

void RemoveFreeQNode(PerfQueue* queue,PerfQNode* qNode)
{
	assert(qNode->qType == QT_FREEQ);

	if(qNode->prev == qNode)
	{
		if(1 != queue->freeQ.count)
		{
			LOGERROR("queue->freeQ.count = %d",queue->freeQ.count);
			assert(queue->freeQ.count == 1);
		}
		queue->freeQ.head	= NULL;
	}
	else
	{
		qNode->next->prev	= qNode->prev;
		qNode->prev->next	= qNode->next;

		if(queue->freeQ.head == qNode)
		{
			queue->freeQ.head	= qNode->next;
		}
	}

	queue->freeQ.count	-= 1;
}

void RemoveDataQNode(PerfQueue* queue,PerfQNode* qNode)
{
	assert(qNode->qType == QT_DATAQ);

	if(qNode->prev == qNode)
	{
		assert(queue->dataQ.count == 1);
		queue->dataQ.head	= NULL;
	}
	else
	{
		qNode->next->prev	= qNode->prev;
		qNode->prev->next	= qNode->next;

		if(queue->dataQ.head == qNode)
		{
			queue->dataQ.head	= qNode->next;
		}
	}

	queue->dataQ.count	-= 1;
}

void RemoveUsingQNode(PerfQueue* queue,PerfQNode* qNode)
{
	assert(qNode->qType == QT_USINGQ);

	if(qNode->prev == qNode)
	{
		assert(queue->usingQ.count == 1);
		queue->usingQ.head	= NULL;
	}
	else
	{
		qNode->next->prev	= qNode->prev;
		qNode->prev->next	= qNode->next;

		if(queue->usingQ.head == qNode)
		{
			queue->usingQ.head	= qNode->next;
		}
	}

	queue->usingQ.count	-= 1;
}

void RemoveDoneQNode(PerfQueue* queue,PerfQNode* qNode)
{
	assert(qNode->qType == QT_DONEQ);

	if(qNode->prev == qNode)
	{
		assert(queue->doneQ.count == 1);
		queue->doneQ.head	= NULL;
	}
	else
	{
		qNode->next->prev	= qNode->prev;
		qNode->prev->next	= qNode->next;

		if(queue->doneQ.head == qNode)
		{
			queue->doneQ.head	= qNode->next;
		}
	}

	queue->doneQ.count	-= 1;
}

void InitQueue(PerfQueue* queue,char bEnVerify)
{
	memset(queue,0,sizeof(PerfQueue));
	InitPerfQNode(queue->qNodeLst,MAX_QNODE_IN_QUEUE);

	queue->bENDoneQ	= bEnVerify;

	GetQNode(queue,&(queue->freeQ.head),0);
	queue->freeQ.count = MAX_QNODE_IN_QUEUE;

	queue->dataQ.head = NULL;
	queue->dataQ.count = 0;

	queue->usingQ.head = NULL;
	queue->usingQ.count = 0;

	queue->doneQ.head = NULL;
	queue->doneQ.count = 0;
}

uint16_t GetPendingNodeCnt(PerfQueue* queue)
{
	return GetDataQNodeCount(queue);
}

uint16_t GetFreeNodeCnt(PerfQueue* queue)
{
	return GetFreeQNodeCount(queue);
}

uint32_t GetNextAddingQNodeIndex(PerfQueue* queue)
{
	return queue->freeQ.head->index;
}

int AddQueueIOCBNode(PerfQueue* queue,struct iocb* iocb)
{
	if(0 >= GetFreeQNodeCount(queue))
	{
		return 0;
	}

	PerfQNode*	qNode	= NULL;
	FetchFreeQNode(queue,&qNode);
	memset(qNode,0,sizeof(PerfQNode));
	memcpy(&(qNode->u.iocb),iocb,sizeof(struct iocb));
	qNode->u.iocb.data	= qNode;
	GetCurrentTimestamp(&(qNode->addTimestamp));
	InsertDataQNode(queue,qNode);

	return 1;
}

int AddQueueIONVMNode(PerfQueue* queue,uint8_t* cmd,uint16_t cmdLen)
{
	if(0 >= GetFreeQNodeCount(queue))
	{
		return 0;
	}

	PerfQNode*	qNode	= NULL;
	FetchFreeQNode(queue,&qNode);
	memset(qNode,0,sizeof(PerfQNode));
	memcpy(qNode->u.ionvm.cmd,cmd,cmdLen);
	GetCurrentTimestamp(&(qNode->addTimestamp));
	InsertDataQNode(queue,qNode);

	return 1;
}

int FetchIOCBNodeToSubmit(PerfQueue* queue,struct iocb** iocb)
{
	if(0 >= GetDataQNodeCount(queue))
	{
		return 0;
	}

	PerfQNode*	qNode	= NULL;
	FetchDataQNode(queue,&qNode);
	*iocb	= &(qNode->u.iocb);
	GetCurrentTimestamp(&(qNode->subTimestamp));
	InsertUsingQNode(queue,qNode);

	return 1;
}

int FetchIONVMNodeToSubmit(PerfQueue* queue,uint8_t** cmd,uint16_t* cmdLen)
{
	if(0 >= GetDataQNodeCount(queue))
	{
		return 0;
	}

	PerfQNode*	qNode	= NULL;
	FetchDataQNode(queue,&qNode);
	*cmd		= qNode->u.ionvm.cmd;
	*cmdLen		= NVME_IO_CMD_SIZE;
	GetCurrentTimestamp(&(qNode->subTimestamp));
	InsertUsingQNode(queue,qNode);

	return 1;
}

int FetchIOCBNodeToVerify(PerfQueue* queue,struct iocb** iocb)
{
	if(0 >= GetDoneQNodeCount(queue))
	{
		return 0;
	}

	PerfQNode*	qNode	= NULL;
	FetchDoneQNode(queue,&qNode);
	*iocb	= &(qNode->u.iocb);
	GetCurrentTimestamp(&(qNode->chkTimestamp));
	InsertFreeQNode(queue,qNode);

	return 1;
}

int FetchIONVMNodeToVerify(PerfQueue* queue,uint8_t** cmd,uint16_t* cmdLen)
{
	if(0 >= GetDoneQNodeCount(queue))
	{
		return 0;
	}

	PerfQNode*	qNode	= NULL;
	FetchDoneQNode(queue,&qNode);
	*cmd		= qNode->u.ionvm.cmd;
	*cmdLen		= NVME_IO_CMD_SIZE;
	GetCurrentTimestamp(&(qNode->chkTimestamp));
	InsertFreeQNode(queue,qNode);

	return 1;
}

int MarkQNodeCompleted(PerfQueue* queue,PerfQNode* qNode)
{
	GetCurrentTimestamp(&(qNode->cmpTimestamp));

	RemoveUsingQNode(queue,qNode);

	if(queue->bENDoneQ)
	{
		InsertDoneQNode(queue,qNode);
	}
	else
	{
		InsertFreeQNode(queue,qNode);
	}

	return 1;
}
