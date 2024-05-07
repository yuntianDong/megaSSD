/*
 * file : DNVMeQMngr.cpp
 */
#include <float.h>
#include "driver/dnvme/DNVMeQMngr.h"

#define IOQ_PRIO_MAX			3

DNVMeQMngr*	DNVMeQMngr::gInstance	= NULL;

DNVMeQMngr*	DNVMeQMngr::GetInstance(int devHdlr)
{
	if(NULL == gInstance)
	{
		gInstance = new DNVMeQMngr(devHdlr);
	}

	return gInstance;
}

DNVMeQMngr::DNVMeQMngr(int devHdlr)
	:mDevHdlr(devHdlr),mpAdminCQ(NULL),mpAdminSQ(NULL),mCacheIOCQ(NULL),mExistPendingIOCQ(false)
{
}

DNVMeQMngr::~DNVMeQMngr(void)
{
	if(NULL != mpAdminSQ)
	{
		delete mpAdminSQ;
		mpAdminSQ = NULL;
	}

	if(NULL != mpAdminCQ)
	{
		delete mpAdminCQ;
		mpAdminCQ = NULL;
	}

	DeleteAllIOQueues();
}

LPIOQPairInfo DNVMeQMngr::CreateIOPairInfo(DNVMeCQ* cq,DNVMeSQ* sq,uint16_t cqid,uint32_t qsize,uint8_t prio)
{
	LPIOQPairInfo	ioQPI	= new IOQPairInfo();
	ioQPI->counter		= 0;
	ioQPI->cqid			= cqid;
	ioQPI->prio			= prio;
	ioQPI->qsize		= qsize;
	ioQPI->valid		= true;
	ioQPI->cqsq			= IOQPair(cq,sq);

	return ioQPI;
}

LPIOQPairInfo DNVMeQMngr::GetIOPairInfoWithCQID(uint16_t cqid)
{
	LPIOQPairInfo ioq=NULL;
	IOQPairMap::iterator  it;

	for(it=mIOQPairMap.begin();it!=mIOQPairMap.end();it++)
	{
		ioq = LPIOQPairInfo(it->second);
		if(ioq->cqid == cqid)
			break;
	}

	return ioq;
}

void DNVMeQMngr::DeleteAllIOQueues(void)
{
	IOQPairMap::iterator  it;
	for(it=mIOQPairMap.begin();it!=mIOQPairMap.end();it++)
	{
		LPIOQPairInfo ioq = LPIOQPairInfo(it->second);

		IOQPair qPair	= ioq->cqsq;
		if(NULL != qPair.first)
		{
			delete qPair.first;
		}
		if(NULL != qPair.second)
		{
			delete qPair.second;
		}

		ioq->valid	= false;
		ioq->cqsq	= IOQPair(NULL,NULL);
	}

	mIOQPairMap.clear();

	if(true == mExistPendingIOCQ && NULL != mCacheIOCQ)
	{
		delete mCacheIOCQ;
		mCacheIOCQ = NULL;
	}
	mExistPendingIOCQ	= false;
}

uint8_t DNVMeQMngr::GetValidIOQPairCount(void)
{
	uint8_t count = 0;
	LPIOQPairInfo ioq=NULL;
	IOQPairMap::iterator  it;

	for(it=mIOQPairMap.begin();it!=mIOQPairMap.end();it++)
	{
		ioq = LPIOQPairInfo(it->second);
		if(ioq->valid == true)
			count++;
	}

	return count;
}

bool DNVMeQMngr::ExistIOSQinQMap(uint16_t sqid)
{
	IOQPairMap::iterator  it;
	it = mIOQPairMap.find(sqid);

	return (it==mIOQPairMap.end());
}

bool DNVMeQMngr::AddIOSQ(uint16_t sqID,uint16_t cqID,uint32_t qSize,uint8_t prio,Buffer* qBuf)
{
	if(false == mExistPendingIOCQ)
	{
		return false;
	}

	if(NULL == mCacheIOCQ || cqID != mCacheIOCQ->GetQueueID())
	{
		return false;
	}

	DNVMeSQ* sq	= new DNVMeSQ(mDevHdlr,sqID);
	if(false == sq->IsValidQ())
	{
		delete sq;
		sq = NULL;

		return false;
	}

	if(NULL != qBuf)
	{
		sq->SetQueueBuf(qBuf);
	}

	LPIOQPairInfo ioQPI	= CreateIOPairInfo(mCacheIOCQ,sq,cqID,qSize,prio);
	uint16_t sqid	= sq->GetQueueID();

	mIOQPairMap.insert(pair<uint16_t,LPIOQPairInfo>(sqid,ioQPI));

	mExistPendingIOCQ	= false;
	mCacheIOCQ			= NULL;
	return true;
}

bool DNVMeQMngr::AddIOCQ(uint16_t cqID,Buffer* qBuf)
{
	if(true == mExistPendingIOCQ)
		return false;

	DNVMeCQ* cq	= new DNVMeCQ(mDevHdlr,cqID);
	if(false == cq->IsValidQ())
	{
		delete cq;
		cq = NULL;

		return false;
	}

	if(NULL != qBuf)
	{
		cq->SetQueueBuf(qBuf);
	}

	mCacheIOCQ	= cq;
	mExistPendingIOCQ	= true;
	return true;
}

bool DNVMeQMngr::DelIOSQ(uint32_t qID)
{
	if(not ExistIOSQinQMap(qID))
		return false;

	LPIOQPairInfo ioq	= mIOQPairMap[qID];

	if(true == ioq->valid)
	{
		return false;
	}

	if(NULL != ioq->cqsq.second)
	{
		delete ioq->cqsq.second;
		ioq->cqsq.second = NULL;
	}

	if(NULL != ioq->cqsq.first)
	{
		delete ioq->cqsq.first;
		ioq->cqsq.first = NULL;
	}

	return (1==mIOQPairMap.erase(qID));
}

bool DNVMeQMngr::DelIOCQ(uint32_t qID)
{
	bool bResult		= false;
	LPIOQPairInfo ioq	= NULL;

	IOQPairMap::iterator  it;
	for(it=mIOQPairMap.begin();it!=mIOQPairMap.end();it++)
	{
		ioq = LPIOQPairInfo(it->second);
		if(qID == ioq->cqid)
		{
			if(NULL != ioq->cqsq.first)
			{
				delete ioq->cqsq.first;
			}
			ioq->cqsq.first	= NULL;
			ioq->counter	= 0;
			ioq->cqid		= INVALID_QID;
			ioq->valid		= false;
			bResult			= true;
		}
	}

	return bResult;
}

double DNVMeQMngr::CalcScore(LPIOQPairInfo ioq)
{
	if(false == ioq->valid)
	{
		return DBL_MAX;
	}

	uint8_t 	prio	= ioq->prio;
	uint32_t	qsize	= ioq->qsize;
	uint16_t	counter	= ioq->counter;

	if(0==counter) counter	= 1;

	return double((uint32_t)(IOQ_PRIO_MAX-prio)*qsize/(uint32_t)counter);
}

IOQPair DNVMeQMngr::GetNextIOQPair(void)
{
	LPIOQPairInfo ioq=NULL;
	IOQPairMap::iterator  it;
	pair<double,LPIOQPairInfo> matchedIO2Q	= make_pair(0,(LPIOQPairInfo)NULL);

	for(it=mIOQPairMap.begin();it!=mIOQPairMap.end();it++)
	{
		ioq = LPIOQPairInfo(it->second);
		if(true == ioq->valid)
		{
			double score	= CalcScore(ioq);

			if(DBL_MAX != score && matchedIO2Q.first < score)
			{
				matchedIO2Q.first	= score;
				matchedIO2Q.second	= ioq;
			}
		}
	}

	if(it == mIOQPairMap.end() || NULL == matchedIO2Q.second)
		return IOQPair(NULL,NULL);

	ioq	= matchedIO2Q.second;
	(ioq->counter)++;

	return ioq->cqsq;
}

void DNVMeQMngr::ResetAllPairQCount(void)
{
	LPIOQPairInfo ioq=NULL;
	IOQPairMap::iterator  it;

	for(it=mIOQPairMap.begin();it!=mIOQPairMap.end();it++)
	{
		ioq = LPIOQPairInfo(it->second);
		ioq->counter	= 0;
	}
}

void DumpIOQPairInfo(LPIOQPairInfo ioq)
{
	printf("cqid : %d \n",ioq->cqid);
	printf("counter : %d \n",ioq->counter);
	printf("prio : %d \n",ioq->prio);
	printf("qsize : %d \n",ioq->qsize);
	printf("valid : %d \n",ioq->valid);
}

void DNVMeQMngr::DebugDump(void)
{
	LPIOQPairInfo ioq=NULL;
	map<uint16_t,LPIOQPairInfo>::iterator  it;

	printf("Dump IOQ Manager Info!\n");
	printf("IOQ Size:%d\n",(int)mIOQPairMap.size());

	for(it=mIOQPairMap.begin();it!=mIOQPairMap.end();it++)
	{
		uint16_t qid = (uint16_t)it->first;
		ioq = LPIOQPairInfo(it->second);

		printf("sqid :%d\n",qid);
		DumpIOQPairInfo(ioq);
	}

	return;
}

IOQPair DNVMeQMngr::GetIOQPairByIndex(uint32_t index)
{
	map<uint16_t,LPIOQPairInfo>::iterator  it = mIOQPairMap.begin();

	if(index >= mIOQPairMap.size())
	{
		return IOQPair(NULL,NULL);
	}

	while(index--){it++;};

	return LPIOQPairInfo(it->second)->cqsq;
}
