#ifndef _TASK_MNGR_H
#define _TASK_MNGR_H

#include <string>
#include <string.h>

#include "test/TaskDesc.h"
#include "test/TaskResult.h"
#include "test/TaskReport.h"

#define	TASK_MNGR_ITEMS_MAX			128

typedef enum _enTaskStatus
{
	TS_UNDEF = 0,
	TS_DONE	= 1,
	TS_TODO,
	TS_ASIGN
}TaskStatus;

typedef struct _stTaskMngr
{
	uint16_t		idxAdd;
	uint16_t		idxFetch;

	struct{
		TaskStatus	status;
		stTaskDesc	desc;
		stRsltDesc	rslt;
	}items[TASK_MNGR_ITEMS_MAX];
}stTaskMngr;

class TaskMngr
{
protected:
	stTaskMngr*	mpTaskMngr;
	bool		mbNeedRework;
	uint16_t	mIdxRework;

	bool	IsValidItem(uint16_t index)
	{
		assert(NULL != mpTaskMngr);
		if(index >= TASK_MNGR_ITEMS_MAX)
		{
			return false;
		}

		TaskStatus	status= mpTaskMngr->items[index].status;
		return (TS_UNDEF != status);
	};
	uint16_t GetAddIndex(void)
	{
		return (NULL == mpTaskMngr)?0:(mpTaskMngr->idxAdd);
	};
	uint16_t GetFetchIndex(void)
	{
		return (NULL == mpTaskMngr)?0:(mpTaskMngr->idxFetch);
	};
	void GoForward(uint16_t &index)
	{
		index = (index + 1) % TASK_MNGR_ITEMS_MAX;
	};
	void GoBack(uint16_t &index)
	{
		index = (index - 1 + TASK_MNGR_ITEMS_MAX) % TASK_MNGR_ITEMS_MAX;
	}
	void UptTaskStatus(uint16_t index,TaskStatus status)
	{
		if(NULL == mpTaskMngr || index >= TASK_MNGR_ITEMS_MAX)
		{
			return;
		}

		mpTaskMngr->items[index].status	= status;
	};
	TaskDesc*	GetTaskDesc(uint16_t index)
	{
		if(NULL == mpTaskMngr || index >= TASK_MNGR_ITEMS_MAX)
		{
			return NULL;
		}

		return new TaskDesc(&(mpTaskMngr->items[index].desc));
	}
	TaskResult*	GetTaskResult(uint16_t index)
	{
		if(NULL == mpTaskMngr || index >= TASK_MNGR_ITEMS_MAX)
		{
			return NULL;
		}

		return new TaskResult(&(mpTaskMngr->items[index].rslt));
	};
	TaskStatus	GetTaskStatus(uint16_t index)
	{
		if(NULL == mpTaskMngr || index >= TASK_MNGR_ITEMS_MAX)
		{
			return TS_UNDEF;
		}

		return mpTaskMngr->items[index].status;
	};
	TaskDesc*	FindNextUndoneTaskDesc(void);
	uint16_t	GetStatusCount(TaskStatus status)
	{
		uint16_t tmpIdx	= GetAddIndex();
		uint16_t ttlCnt	= 0;
		GoBack(tmpIdx);
		while( tmpIdx != GetAddIndex() && true == IsValidItem(tmpIdx))
		{
			if(TS_UNDEF == status || status == GetTaskStatus(tmpIdx))
			{
				ttlCnt++;
			}
			GoBack(tmpIdx);
		}

		return ttlCnt;
	}

public:
	TaskMngr(void)
		:mpTaskMngr(NULL),mbNeedRework(false),mIdxRework(0)
	{};
	TaskMngr(stTaskMngr* pTMngr)
		:mpTaskMngr(pTMngr)
	{
		mbNeedRework	= (0 != GetAddIndex()) || (0 != GetFetchIndex());
		mIdxRework		= GetFetchIndex();
		GoBack(mIdxRework);
	};

	void Dump(void);
	void DumpMngrItem(uint16_t idx);
	void Reset(void)
	{
		assert(NULL != mpTaskMngr);
		//mpTaskMngr->idxAdd = mpTaskMngr->idxFetch	= 0;
		memset(mpTaskMngr,0,sizeof(stTaskMngr));
	}
	uint16_t GetTotalCount(void)
	{
		return GetStatusCount(TS_UNDEF);
	}

	uint16_t GetFinishCnt(void)
	{
		return GetStatusCount(TS_DONE);
	}

	uint16_t GetRunningCnt(void)
	{
		return GetStatusCount(TS_ASIGN);
	}

	uint16_t GetPendingCnt(void)
	{
		return (GetAddIndex() - GetFetchIndex() + TASK_MNGR_ITEMS_MAX) % TASK_MNGR_ITEMS_MAX;
	};
	bool IsEmpty(void)
	{
		assert(NULL != mpTaskMngr);
		return mpTaskMngr->idxAdd == mpTaskMngr->idxFetch;
	};
	bool IsFull(void)
	{
		assert(NULL != mpTaskMngr);
		return ((mpTaskMngr->idxAdd + 1) % TASK_MNGR_ITEMS_MAX) == mpTaskMngr->idxFetch;
	};
	bool FindTaskDescByIndex(uint16_t index,TaskDesc*& pDesc)
	{
		pDesc = GetTaskDesc(index);

		return NULL != pDesc;
	};
	bool FindTaskResultByIndex(uint16_t index,TaskResult*& pRslt)
	{
		pRslt = GetTaskResult(index);

		if(NULL!= pRslt)
		{
			pRslt->SetRsltIndex(index);
			UptTaskStatus(index,TS_DONE);
		}
		return NULL != pRslt;
	};
	bool GetTaskDescForExe(TaskDesc*& desc);
	bool GetTaskDescForCfg(TaskDesc*& desc);

	bool MakeTestReport(TaskReport& report);
	bool SaveResultAs(string& output);
	bool SaveResultAs(const char* filename);
	bool PrintTestResult(void);
};

#endif
