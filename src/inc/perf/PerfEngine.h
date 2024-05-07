#ifndef _PERF_ENGINE_H
#define _PERF_ENGINE_H

#include "PerfContext.h"
#include "PerfQueue.h"
#include "PerfParam.h"
#include "PerfStat.h"
#include "PerfTask.h"

typedef struct _stPerfEngineOp
{
	int (*setup)(PerfContext* context,PerfParam* param);
	void (*cleanup)(PerfContext* context);
	int (*prepare_io)(PerfContext* context,PerfQueue* queue,PerfTask* task);
	int (*submit_io)(PerfContext* context,PerfQueue* queue,PerfStat* stat);
	int (*check_io)(PerfContext* context,PerfQueue* queue,PerfStat* stat);
	int (*verify_io)(PerfContext* context,PerfQueue* queue,PerfStat* stat);
}PerfEngineOp;

#ifdef __cplusplus
extern "C" {
#endif

int perf_main_loop(uint32_t wID,int fd,PerfParam* param,PerfEngineOp* ops,PerfStat* stat);

#ifdef __cplusplus
}
#endif
#endif
