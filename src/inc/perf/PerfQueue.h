#ifndef _PERF_QUEUE_H
#define _PERF_QUEUE_H

#include <stdint.h>
#include <libaio.h>

#include "PerfUtility.h"

#define	QNODE_MAX_SIZE				64
#define NVME_IO_CMD_SIZE			64
#define MAX_QNODE_IN_QUEUE			1024

typedef enum _stQueueType
{
	QT_FREEQ,
	QT_DATAQ,
	QT_USINGQ,
	QT_DONEQ
}QueueType;

typedef struct _stPerfQNode
{
	union
	{
		struct{
			uint8_t cmd[NVME_IO_CMD_SIZE];
		}ionvm;

		struct iocb	iocb;
	}u;

	struct timespec	addTimestamp;		// timestamp of added into Queue
	struct timespec subTimestamp;		// timestamp of cmd submitted
	struct timespec	cmpTimestamp;		// timestamp of cmd completed
	struct timespec	chkTimestamp;		// timestamp of cmd verified

	QueueType		qType;
	uint32_t		index;
	struct _stPerfQNode*		next;
	struct _stPerfQNode*		prev;
}PerfQNode;

typedef struct _stPerfQueue
{
	char	bENDoneQ;

	PerfQNode	qNodeLst[MAX_QNODE_IN_QUEUE];

	struct{
		PerfQNode*	head;
		uint32_t	count;
	}freeQ;

	struct{
		PerfQNode*	head;
		uint32_t	count;
	}dataQ;

	struct{
		PerfQNode*	head;
		uint32_t	count;
	}usingQ;

	struct{
		PerfQNode*	head;
		uint32_t	count;
	}doneQ;
}PerfQueue;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t GetFreeQNodeCount(PerfQueue* queue);
uint32_t GetDataQNodeCount(PerfQueue* queue);
uint32_t GetUsingQNodeCount(PerfQueue* queue);
uint32_t GetDoneQNodeCount(PerfQueue* queue);
void GetQNode(PerfQueue* queue,PerfQNode** qNode,uint32_t qIdx);
void FetchFreeQNode(PerfQueue* queue,PerfQNode** pqNode);
void FetchDataQNode(PerfQueue* queue,PerfQNode** pqNode);
void FetchUsingQNode(PerfQueue* queue,PerfQNode** pqNode);
void FetchDoneQNode(PerfQueue* queue,PerfQNode** pqNode);
void InsertFreeQNode(PerfQueue* queue,PerfQNode* qNode);
void InsertDataQNode(PerfQueue* queue,PerfQNode* qNode);
void InsertUsingQNode(PerfQueue* queue,PerfQNode* qNode);
void InsertDoneQNode(PerfQueue* queue,PerfQNode* qNode);
void RemoveFreeQNode(PerfQueue* queue,PerfQNode* qNode);
void RemoveDataQNode(PerfQueue* queue,PerfQNode* qNode);
void RemoveUsingQNode(PerfQueue* queue,PerfQNode* qNode);
void RemoveDoneQNode(PerfQueue* queue,PerfQNode* qNode);

void InitQueue(PerfQueue* queue,char bEnVerify);
uint16_t GetFreeNodeCnt(PerfQueue* queue);
uint16_t GetPendingNodeCnt(PerfQueue* queue);
uint32_t GetNextAddingQNodeIndex(PerfQueue* queue);
int AddQueueIOCBNode(PerfQueue* queue,struct iocb* iocb);
int AddQueueIONVMNode(PerfQueue* queue,uint8_t* cmd,uint16_t cmdLen);
int FetchIOCBNodeToSubmit(PerfQueue* queue,struct iocb** iocb);
int FetchIONVMNodeToSubmit(PerfQueue* queue,uint8_t** cmd,uint16_t* cmdLen);
int FetchIOCBNodeToVerify(PerfQueue* queue,struct iocb** iocb);
int FetchIONVMNodeToVerify(PerfQueue* queue,uint8_t** cmd,uint16_t* cmdLen);
int MarkQNodeCompleted(PerfQueue* queue,PerfQNode* qNode);

#ifdef __cplusplus
}
#endif

#endif
