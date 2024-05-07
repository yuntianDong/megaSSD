/*
 * file : PerfParam.c
 */

#include <string.h>
#include "perf/PerfParam.h"

void InitPerfParam(PerfParam* param,uint32_t numOfQ,uint32_t qDepth,uint32_t duration)
{
	memset(param,0,sizeof(PerfParam));

	param->uNumOfQueue		= numOfQ;
	param->uQueueDepth		= qDepth;
	param->uExecTime		= duration;
}

void SetErrAborted(PerfParam* param,char bErrAborted)
{
	param->opt.bErrAborted	= bErrAborted;
}

void SetDataVerify(PerfParam* param,char bDataVerify)
{
	param->opt.bDataVerify	= bDataVerify;
}
