#ifndef _PERF_PARAM_H
#define _PERF_PARAM_H

#include <stdint.h>
#include "PerfTask.h"


typedef struct _stPerfParam
{
	uint32_t	uNumOfQueue;
	uint32_t	uQueueDepth;
	uint32_t	uSectorSize;
	uint32_t	uExecTime;

	struct {
		char	bErrAborted	: 1;
		char	bDataVerify	: 1;
	}opt;

	PerfTask	sPerfTask;
}PerfParam;

#ifdef __cplusplus
extern "C" {
#endif

void InitPerfParam(PerfParam* param,uint32_t numOfQ,uint32_t qDepth,uint32_t duration);
void SetErrAborted(PerfParam* param,char bErrAborted);
void SetDataVerify(PerfParam* param,char bDataVerify);

#ifdef __cplusplus
}
#endif

#endif
