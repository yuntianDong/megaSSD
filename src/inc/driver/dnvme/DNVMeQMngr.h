#ifndef _DNVME_QUEUE_MNGR_H
#define _DNVME_QUEUE_MNGR_H

#include <map>
#include "DNVMeQueue.h"

#define ADMIN_CQ_QID			0
#define ADMIN_SQ_QID			0

#define INVALID_QID				(uint16_t)-1

typedef pair<DNVMeCQ*,DNVMeSQ*> IOQPair;

typedef struct IOQPairInfo
{
	uint8_t 	prio;
	bool 		valid;
	uint32_t	qsize;
	uint16_t	counter;
	uint16_t 	cqid;
	IOQPair		cqsq;
}*LPIOQPairInfo;

class DNVMeQMngr
{
private:
	int 			mDevHdlr;

	DNVMeCQ*		mpAdminCQ;
	DNVMeSQ*		mpAdminSQ;

	typedef map<uint16_t,LPIOQPairInfo> IOQPairMap;
	IOQPairMap 		mIOQPairMap;

	LPIOQPairInfo CreateIOPairInfo(DNVMeCQ* cq,DNVMeSQ* sq,uint16_t cqid,uint32_t qsize,uint8_t prio);
	LPIOQPairInfo GetIOPairInfoWithCQID(uint16_t cqid);
	uint8_t 	GetValidIOQPairCount(void);

	bool		ExistIOSQinQMap(uint16_t sqid);

	DNVMeCQ* 		mCacheIOCQ;
	bool			mExistPendingIOCQ;
protected:
	DNVMeQMngr(int devHdlr);

	virtual double CalcScore(LPIOQPairInfo info);
public:
	static DNVMeQMngr*	gInstance;
	static DNVMeQMngr*	GetInstance(int devHdlr);

	virtual ~DNVMeQMngr(void);

	DNVMeSQ* GetAdminSQ(void)
	{
		if(NULL == mpAdminSQ)
		{
			mpAdminSQ = new DNVMeSQ(mDevHdlr,ADMIN_SQ_QID);
			if(false == mpAdminSQ->IsValidQ())
			{
				delete mpAdminSQ;
				mpAdminSQ = NULL;
			}
		}

		return mpAdminSQ;
	};
	DNVMeCQ* GetAdminCQ(void)
	{
		if(NULL == mpAdminCQ)
		{
			mpAdminCQ = new DNVMeCQ(mDevHdlr,ADMIN_CQ_QID);
			if(false == mpAdminCQ->IsValidQ())
			{
				delete mpAdminCQ;
				mpAdminCQ = NULL;
			}
		}
		return mpAdminCQ;
	};

	bool AddIOSQ(uint16_t sqID,uint16_t cqID,uint32_t qSize,uint8_t prio,Buffer* qBuf=NULL);
	bool AddIOCQ(uint16_t cqID,Buffer* qBuf=NULL);
	bool DelIOSQ(uint32_t qID);
	bool DelIOCQ(uint32_t qID);

	IOQPair GetNextIOQPair(void);

	void ResetAllPairQCount(void);
	void DeleteAllIOQueues(void);
	void DebugDump(void);

	IOQPair GetIOQPairByIndex(uint32_t index);
};

#endif
