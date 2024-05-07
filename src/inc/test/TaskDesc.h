#ifndef _TASK_DESC_H
#define _TASK_DESC_H

#include <stdint.h>
#include <assert.h>
#include "DUTInfo.h"

#define TASK_NAME_LEN				40
#define	TASK_CMDLINE_LEN			240

#define TASK_SN_LEN					21
#define TASK_SN_REQ_SIZE			3
#define TASK_SN_EXC_SIZE			3
#define TASK_MN_LEN					41
#define TASK_MN_REQ_SIZE			3
#define TASK_MN_EXC_SIZE			3
#define TASK_FR_LEN					9
#define TASK_FR_REQ_SIZE			3
#define TASK_FR_EXC_SIZE			3
#define TASK_GR_LEN					9
#define TASK_GR_REQ_SIZE			3
#define TASK_GR_EXC_SIZE			3

#define TASK_GB_REQ_SIZE			3
#define TASK_GB_EXC_SIZE			3
#define TASK_SS_REQ_SIZE			3
#define TASK_SS_EXC_SIZE			3

#define UNDEF_TASK_INDEX			0xFFFF
#define UNDEF_TASK_NAME				"UNDEF"
#define UNDEF_TASK_CMDLINE			"UNDEF"
#define UNDEF_TASK_TIMEOUT			0
#define UNDEF_TASK_LOOPCOUNT		0

typedef enum _enCondKey
{
	CK_SN,
	CK_MN,
	CK_FR,
	CK_GR,
	CK_GB,
	CK_SS
}enCondKey;

typedef struct _stTaskCond
{
	uint8_t		idxReqSN;
	char		reqSN[TASK_SN_REQ_SIZE][TASK_SN_LEN];
	uint8_t		idxExcSN;
	char		excSN[TASK_SN_EXC_SIZE][TASK_SN_LEN];
	uint8_t		idxReqMN;
	char		reqMN[TASK_MN_REQ_SIZE][TASK_MN_LEN];
	uint8_t		idxExcMN;
	char		excMN[TASK_MN_EXC_SIZE][TASK_MN_LEN];
	uint8_t		idxReqFR;
	char		reqFR[TASK_FR_REQ_SIZE][TASK_FR_LEN];
	uint8_t		idxExcFR;
	char		excFR[TASK_FR_EXC_SIZE][TASK_FR_LEN];
	uint8_t		idxReqGR;
	char		reqGR[TASK_GR_REQ_SIZE][TASK_GR_LEN];
	uint8_t		idxExcGR;
	char		excGR[TASK_GR_EXC_SIZE][TASK_GR_LEN];

	uint8_t		idxReqGB;
	uint32_t	reqGB[TASK_GB_REQ_SIZE];
	uint8_t		idxExcGB;
	uint32_t	excGB[TASK_GB_EXC_SIZE];
	uint8_t		idxReqSS;
	uint32_t	reqSS[TASK_SS_REQ_SIZE];
	uint8_t		idxExcSS;
	uint32_t	excSS[TASK_SS_EXC_SIZE];
}stTaskCond;

typedef struct _stTaskDesc
{
	uint16_t	index;
	char		name[TASK_NAME_LEN];
	char		cmdline[TASK_CMDLINE_LEN];
	uint32_t	execTimeout;
	uint32_t	loopCount;
	stTaskCond	taskCond;
	uint64_t	resv1;
}stTaskDesc;

class TaskDesc
{
protected:
	stTaskDesc*		mpTaskDesc;
	bool			mbNeedRelease;

	uint16_t GetCondItemLength(enCondKey key);
	bool	IsCondStrType(enCondKey key);
	void	AdvanceTaskCondIndex(enCondKey key,bool bExcluded);
	bool	GetIdxOfTaskCond(enCondKey key,bool bExcluded,uint8_t& index);

	bool	HasValOfTaskCond(enCondKey key,bool bExcluded);
	bool	FindValOfTaskCond(enCondKey key,bool bExcluded,const char* cond);
	bool	FindValOfTaskCond(enCondKey key,bool bExcluded,uint32_t cond);
public:
	TaskDesc(void)
		:mpTaskDesc(NULL),mbNeedRelease(true)
	{
		mpTaskDesc	= new stTaskDesc();
	};
	TaskDesc(stTaskDesc*	desc)
		:mpTaskDesc(desc),mbNeedRelease(false)
	{};

	virtual ~TaskDesc(void)
	{
		if(NULL != mpTaskDesc && true == mbNeedRelease)
		{
			delete mpTaskDesc;
			mpTaskDesc = NULL;
		}
	}

	void		Dump(void);
	void		Reset(void)	{memset(mpTaskDesc,0,sizeof(stTaskDesc));};
	stTaskDesc*	GetDataPoint(void) {return mpTaskDesc;};

	void		SetTaskIndex(uint16_t index)
	{
		assert(NULL != mpTaskDesc);
		mpTaskDesc->index = index;
	};
	void		SetTaskName(const char* name)
	{
		assert(NULL != mpTaskDesc);
		strncpy(mpTaskDesc->name,name,strlen(name)+1);
	};
	void		SetTaskCmdline(const char* cmdLine)
	{
		assert(NULL != mpTaskDesc);
		strncpy(mpTaskDesc->cmdline,cmdLine,strlen(cmdLine)+1);
	}
	void		SetTimeout(uint32_t timeout)
	{
		assert(NULL != mpTaskDesc);
		mpTaskDesc->execTimeout	= timeout;
	}
	void		SetLoopCnt(uint32_t loopCnt)
	{
		assert(NULL != mpTaskDesc);
		mpTaskDesc->loopCount	= loopCnt;
	}
	void		SetRsvVal(uint64_t val)
	{
		assert(NULL != mpTaskDesc);
		mpTaskDesc->resv1		= val;
	}

	uint16_t	GetTaskIndex(void)
	{
		assert(NULL != mpTaskDesc);
		return mpTaskDesc->index;
	}
	const char*	GetTaskName(void)
	{
		assert(NULL != mpTaskDesc);
		return (const char*)(mpTaskDesc->name);
	};
	const char*	GetCmdLine(void)
	{
		assert(NULL != mpTaskDesc);
		return (const char*)(mpTaskDesc->cmdline);
	};
	uint32_t	GetTimeOut(void)
	{
		assert(NULL != mpTaskDesc);
		return mpTaskDesc->execTimeout;
	};
	uint32_t	GetLoopCount(void)
	{
		assert(NULL != mpTaskDesc);
		return mpTaskDesc->loopCount;
	};
	uint64_t	GetRsvVal(void)
	{
		assert(NULL != mpTaskDesc);
		return mpTaskDesc->resv1;
	}

	bool		IsMatchedDut(DutInfo* dut);
	bool		AddCondition(enCondKey key,const char* val,bool bExcluded=false);
	bool		AddCondition(enCondKey key,uint32_t val,bool bExcluded=false);
	bool		GetCondOfTaskCond(enCondKey key,bool bExcluded,uint8_t index,char*& cond);
	bool		GetCondOfTaskCond(enCondKey key,bool bExcluded,uint8_t index,uint32_t*& cond);
};

#endif
