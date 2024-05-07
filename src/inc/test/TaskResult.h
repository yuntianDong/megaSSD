#ifndef _TASK_RESULT_H
#define _TASK_RESULT_H

#include <stdint.h>
#include <string.h>
#include <assert.h>

#define	TAGT_ADDR_MAX_LEN		32
#define	DEF_RSLT_INDEX			0xFFFF

typedef struct _stRsltDesc
{
	uint16_t	index;
	int			result;
	uint32_t	execTime;
	uint8_t		slotID;
	char		tagtAddr[TAGT_ADDR_MAX_LEN];
	bool		valid;
}stRsltDesc;

class TaskResult
{
protected:
	stRsltDesc*		mpRsltDesc;
	bool			mbNeedRelease;
public:
	TaskResult(void)
		:mpRsltDesc(NULL),mbNeedRelease(true)
	{
		mpRsltDesc	= new stRsltDesc();
	};
	TaskResult(stRsltDesc* pRslt)
		:mpRsltDesc(pRslt),mbNeedRelease(false)
	{};

	virtual ~TaskResult(void)
	{
		if(true == mbNeedRelease && NULL != mpRsltDesc)
		{
			delete mpRsltDesc;
			mpRsltDesc = NULL;
		}
	}

	stRsltDesc*	GetDataPoint(void) {return mpRsltDesc;};

	bool IsValid(void)
	{
		return mpRsltDesc->valid;
	}

	void Dump(void)
	{
		printf("index    = %d\n",mpRsltDesc->index);
		printf("result   = %d\n",mpRsltDesc->result);
		printf("execTime = %d\n",mpRsltDesc->execTime);
		printf("slotID   = %d\n",mpRsltDesc->slotID);
		printf("tagtAddr = %s\n",mpRsltDesc->tagtAddr);
		printf("valid    = %s\n",(true == mpRsltDesc->valid)?"TRUE":"FALSE");
	}

	void SetRsltIndex(uint16_t index)
	{
		assert(NULL != mpRsltDesc);
		mpRsltDesc->index	= index;
	};
	void SetTestResult(int rslt)
	{
		assert(NULL != mpRsltDesc);
		mpRsltDesc->result	= rslt;
		mpRsltDesc->valid	= true;
	};
	void SetTestExecTime(uint32_t execTime)
	{
		assert(NULL != mpRsltDesc);
		mpRsltDesc->execTime	= execTime;
	};
	void SetTargetPhyAddr(const char* tagtAddr)
	{
		assert(NULL != mpRsltDesc);
		strncpy(mpRsltDesc->tagtAddr,tagtAddr,TAGT_ADDR_MAX_LEN);
	};
	void SetSlotID(uint8_t slotID)
	{
		assert(NULL != mpRsltDesc);
		mpRsltDesc->slotID	= slotID;
	};
	uint16_t	GetRsltIndex(void)
	{
		assert(NULL != mpRsltDesc);
		return mpRsltDesc->index;
	};
	int			GetTestResult(void)
	{
		assert(NULL != mpRsltDesc);
		return mpRsltDesc->result;
	};
	uint32_t	GetTestExecTime(void)
	{
		assert(NULL != mpRsltDesc);
		return mpRsltDesc->execTime;
	};
	const char*	GetTargetPhyAddr(void)
	{
		assert(NULL != mpRsltDesc);
		return (const char*)(mpRsltDesc->tagtAddr);
	};
	uint8_t GetSlotID(void)
	{
		assert(NULL != mpRsltDesc);
		return mpRsltDesc->slotID;
	};
};

#endif
