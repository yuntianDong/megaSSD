/*
 * file : utPerfQueue.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "perf/PerfQueue.h"

class utPerfQueue : public testing::Test
{
};

TEST_F(utPerfQueue,CheckInitQueue)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE);
	EXPECT_EQ(GetDataQNodeCount(&queue),0);
	EXPECT_EQ(GetUsingQNodeCount(&queue),0);
	EXPECT_EQ(GetDoneQNodeCount(&queue),0);

	PerfQNode*	qNode = NULL;
	GetQNode(&queue,&qNode,0);
	EXPECT_EQ(qNode->qType,QT_FREEQ);
	EXPECT_EQ(qNode->prev->index,MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(qNode->next->index,1);

	EXPECT_EQ(queue.freeQ.head->index,0);
}

TEST_F(utPerfQueue,CheckFree2Data)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	PerfQNode*	qNode = NULL;
	FetchFreeQNode(&queue,&qNode);
	EXPECT_EQ(qNode->index,0);
	InsertDataQNode(&queue,qNode);

	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(GetDataQNodeCount(&queue),1);
	EXPECT_EQ(queue.freeQ.head->index,1);
	EXPECT_EQ(queue.dataQ.head->index,0);
	EXPECT_EQ(queue.freeQ.head->prev->index,MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(queue.freeQ.head->next->index,2);
	EXPECT_EQ(queue.dataQ.head->prev->index,0);
	EXPECT_EQ(queue.dataQ.head->next->index,0);

	uint32_t	qIndex = 1;
	while(0 < GetFreeQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		FetchFreeQNode(&queue,&qNode);
		EXPECT_EQ(qNode->index,qIndex);
		InsertDataQNode(&queue,qNode);

		qIndex += 1;
	}

	EXPECT_EQ(qIndex,MAX_QNODE_IN_QUEUE);
	EXPECT_EQ(GetFreeQNodeCount(&queue),0);
	EXPECT_EQ(GetDataQNodeCount(&queue),MAX_QNODE_IN_QUEUE);
}

TEST_F(utPerfQueue,CheckData2Using)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	while(0 < GetFreeQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		FetchFreeQNode(&queue,&qNode);
		InsertDataQNode(&queue,qNode);
	}

	PerfQNode*	qNode = NULL;
	FetchDataQNode(&queue,&qNode);
	EXPECT_EQ(qNode->index,0);
	InsertUsingQNode(&queue,qNode);

	EXPECT_EQ(GetDataQNodeCount(&queue),MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(GetUsingQNodeCount(&queue),1);
	EXPECT_EQ(queue.dataQ.head->index,1);
	EXPECT_EQ(queue.usingQ.head->index,0);
	EXPECT_EQ(queue.dataQ.head->prev->index,MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(queue.dataQ.head->next->index,2);
	EXPECT_EQ(queue.usingQ.head->prev->index,0);
	EXPECT_EQ(queue.usingQ.head->next->index,0);

	uint32_t	qIndex = 1;
	while(0 < GetDataQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		FetchDataQNode(&queue,&qNode);
		EXPECT_EQ(qNode->index,qIndex);
		InsertUsingQNode(&queue,qNode);

		qIndex += 1;
	}

	EXPECT_EQ(qIndex,MAX_QNODE_IN_QUEUE);
	EXPECT_EQ(GetDataQNodeCount(&queue),0);
	EXPECT_EQ(GetUsingQNodeCount(&queue),MAX_QNODE_IN_QUEUE);
}

TEST_F(utPerfQueue,CheckUsing2Done)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	while(0 < GetFreeQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		FetchFreeQNode(&queue,&qNode);
		InsertUsingQNode(&queue,qNode);
	}

	PerfQNode*	qNode = NULL;
	FetchUsingQNode(&queue,&qNode);
	EXPECT_EQ(qNode->index,0);
	InsertDoneQNode(&queue,qNode);

	EXPECT_EQ(GetUsingQNodeCount(&queue),MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(GetDoneQNodeCount(&queue),1);
	EXPECT_EQ(queue.usingQ.head->index,1);
	EXPECT_EQ(queue.doneQ.head->index,0);
	EXPECT_EQ(queue.usingQ.head->prev->index,MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(queue.usingQ.head->next->index,2);
	EXPECT_EQ(queue.doneQ.head->prev->index,0);
	EXPECT_EQ(queue.doneQ.head->next->index,0);

	uint32_t	qIndex = 1;
	while(0 < GetUsingQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		FetchUsingQNode(&queue,&qNode);
		EXPECT_EQ(qNode->index,qIndex);
		InsertDoneQNode(&queue,qNode);

		qIndex += 1;
	}

	EXPECT_EQ(qIndex,MAX_QNODE_IN_QUEUE);
	EXPECT_EQ(GetUsingQNodeCount(&queue),0);
	EXPECT_EQ(GetDoneQNodeCount(&queue),MAX_QNODE_IN_QUEUE);
}

TEST_F(utPerfQueue,CheckDone2Free)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	while(0 < GetFreeQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		FetchFreeQNode(&queue,&qNode);
		InsertDoneQNode(&queue,qNode);
	}

	PerfQNode*	qNode = NULL;
	FetchDoneQNode(&queue,&qNode);
	EXPECT_EQ(qNode->index,0);
	InsertFreeQNode(&queue,qNode);

	EXPECT_EQ(GetDoneQNodeCount(&queue),MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(GetFreeQNodeCount(&queue),1);
	EXPECT_EQ(queue.doneQ.head->index,1);
	EXPECT_EQ(queue.freeQ.head->index,0);
	EXPECT_EQ(queue.doneQ.head->prev->index,MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(queue.doneQ.head->next->index,2);
	EXPECT_EQ(queue.freeQ.head->prev->index,0);
	EXPECT_EQ(queue.freeQ.head->next->index,0);

	uint32_t	qIndex = 1;
	while(0 < GetDoneQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		FetchDoneQNode(&queue,&qNode);
		EXPECT_EQ(qNode->index,qIndex);
		InsertFreeQNode(&queue,qNode);

		qIndex += 1;
	}

	EXPECT_EQ(qIndex,MAX_QNODE_IN_QUEUE);
	EXPECT_EQ(GetDoneQNodeCount(&queue),0);
	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE);
}

TEST_F(utPerfQueue,CheckRemoveFree)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE);

	uint32_t	qIndex = 0;
	while(0 < GetFreeQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		GetQNode(&queue,&qNode,qIndex);
		RemoveFreeQNode(&queue,qNode);

		qIndex += 1;

		EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE - qIndex);
		if(qIndex == MAX_QNODE_IN_QUEUE)
		{
			EXPECT_TRUE(queue.freeQ.head==NULL);
		}
		else
		{
			EXPECT_EQ(queue.freeQ.head->index,qIndex);
		}
	}
}

TEST_F(utPerfQueue,CheckRemoveData)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	while(0 < GetFreeQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		FetchFreeQNode(&queue,&qNode);
		InsertDataQNode(&queue,qNode);
	}

	EXPECT_EQ(GetDataQNodeCount(&queue),MAX_QNODE_IN_QUEUE);

	uint32_t	qIndex = 0;
	while(0 < GetDataQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		GetQNode(&queue,&qNode,qIndex);
		RemoveDataQNode(&queue,qNode);

		qIndex += 1;

		EXPECT_EQ(GetDataQNodeCount(&queue),MAX_QNODE_IN_QUEUE - qIndex);
		if(qIndex == MAX_QNODE_IN_QUEUE)
		{
			EXPECT_TRUE(queue.dataQ.head==NULL);
		}
		else
		{
			EXPECT_EQ(queue.dataQ.head->index,qIndex);
		}
	}
}

TEST_F(utPerfQueue,CheckRemoveUsing)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	while(0 < GetFreeQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		FetchFreeQNode(&queue,&qNode);
		InsertUsingQNode(&queue,qNode);
	}

	EXPECT_EQ(GetUsingQNodeCount(&queue),MAX_QNODE_IN_QUEUE);

	uint32_t	qIndex = 0;
	while(0 < GetUsingQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		GetQNode(&queue,&qNode,qIndex);
		RemoveUsingQNode(&queue,qNode);

		qIndex += 1;

		EXPECT_EQ(GetUsingQNodeCount(&queue),MAX_QNODE_IN_QUEUE - qIndex);
		if(qIndex == MAX_QNODE_IN_QUEUE)
		{
			EXPECT_TRUE(queue.usingQ.head==NULL);
		}
		else
		{
			EXPECT_EQ(queue.usingQ.head->index,qIndex);
		}
	}
}

TEST_F(utPerfQueue,CheckRemoveDone)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	while(0 < GetFreeQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		FetchFreeQNode(&queue,&qNode);
		InsertDoneQNode(&queue,qNode);
	}

	EXPECT_EQ(GetDoneQNodeCount(&queue),MAX_QNODE_IN_QUEUE);

	uint32_t	qIndex = 0;
	while(0 < GetDoneQNodeCount(&queue))
	{
		PerfQNode*	qNode = NULL;
		GetQNode(&queue,&qNode,qIndex);
		RemoveDoneQNode(&queue,qNode);

		qIndex += 1;

		EXPECT_EQ(GetDoneQNodeCount(&queue),MAX_QNODE_IN_QUEUE - qIndex);
		if(qIndex == MAX_QNODE_IN_QUEUE)
		{
			EXPECT_TRUE(queue.doneQ.head==NULL);
		}
		else
		{
			EXPECT_EQ(queue.doneQ.head->index,qIndex);
		}
	}
}

TEST_F(utPerfQueue,CheckAddFunc)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	struct iocb	iocb;
	AddQueueIOCBNode(&queue,&iocb);

	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(GetDataQNodeCount(&queue),1);

	uint8_t cmd[64];
	AddQueueIONVMNode(&queue,cmd,64);
	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE-2);
	EXPECT_EQ(GetDataQNodeCount(&queue),2);
}

TEST_F(utPerfQueue,CheckIOCB)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	struct iocb	iocb;
	AddQueueIOCBNode(&queue,&iocb);

	struct iocb* fetched = NULL;
	FetchIOCBNodeToSubmit(&queue,&fetched);

	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(GetDataQNodeCount(&queue),0);
	EXPECT_EQ(GetUsingQNodeCount(&queue),1);

	PerfQNode*	qNode = NULL;
	GetQNode(&queue,&qNode,0);
	MarkQNodeCompleted(&queue,qNode);
	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(GetDataQNodeCount(&queue),0);
	EXPECT_EQ(GetUsingQNodeCount(&queue),0);
	EXPECT_EQ(GetDoneQNodeCount(&queue),1);

	FetchIOCBNodeToVerify(&queue,&fetched);
	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE);
	EXPECT_EQ(GetDataQNodeCount(&queue),0);
	EXPECT_EQ(GetUsingQNodeCount(&queue),0);
	EXPECT_EQ(GetDoneQNodeCount(&queue),0);
}

TEST_F(utPerfQueue,CheckIONVM)
{
	PerfQueue	queue;
	InitQueue(&queue,1);

	uint8_t	cmd[64];
	AddQueueIONVMNode(&queue,cmd,64);

	uint8_t* pCmd = NULL;
	uint16_t cmdLen = 0;
	FetchIONVMNodeToSubmit(&queue,&pCmd,&cmdLen);

	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(GetDataQNodeCount(&queue),0);
	EXPECT_EQ(GetUsingQNodeCount(&queue),1);

	PerfQNode*	qNode = NULL;
	GetQNode(&queue,&qNode,0);
	MarkQNodeCompleted(&queue,qNode);
	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE-1);
	EXPECT_EQ(GetDataQNodeCount(&queue),0);
	EXPECT_EQ(GetUsingQNodeCount(&queue),0);
	EXPECT_EQ(GetDoneQNodeCount(&queue),1);

	FetchIONVMNodeToVerify(&queue,&pCmd,&cmdLen);
	EXPECT_EQ(GetFreeQNodeCount(&queue),MAX_QNODE_IN_QUEUE);
	EXPECT_EQ(GetDataQNodeCount(&queue),0);
	EXPECT_EQ(GetUsingQNodeCount(&queue),0);
	EXPECT_EQ(GetDoneQNodeCount(&queue),0);
}
