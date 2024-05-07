#ifndef _PERF_CONTEXT_H
#define _PERF_CONTEXT_H

#include <stdint.h>

#include "PerfTask.h"
#include "PerfConfig.h"

class BasicPerfContext
{
public:
	uint64_t			mDevHdlr;
	uint32_t			mWorkerID;
	BasicPerfTask*		mpPerfTask;
	BasicPerfConfig*	mpPerfCfg;

	uint64_t			mIOCompleted;
	uint64_t			mIOSubmitted;
	uint32_t			mIOErrors;

	BasicPerfContext(void)
		:mDevHdlr(0),mWorkerID(0),mpPerfTask(NULL),mpPerfCfg(NULL),
		 mIOCompleted(0),mIOSubmitted(0),mIOErrors(0)
	{
	};

	virtual ~BasicPerfContext(void)
	{
	};
};

#endif
