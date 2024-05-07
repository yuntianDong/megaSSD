/*
 * file : PerfStat.c
 */

#include <string.h>
#include "perf/PerfStat.h"
#include "perf/PerfUtility.h"

#define	MB_BYTES				(1024*1024)
#define MiB_BYTES				(1000*1000)

void InitPerfStat(PerfStat* stat)
{
	memset(stat,0,sizeof(PerfStat));
}

void IncCntIOCompleted(PerfStat* stat,uint16_t incCnt)
{
	stat->uIOCompleted += incCnt;
}

void IncCntIOSubmitted(PerfStat* stat,uint16_t incCnt)
{
	stat->uIOSubmitted += incCnt;
}

void IncCntIOError(PerfStat* stat,uint16_t incCnt)
{
	stat->uIOErrors	+= incCnt;
}

void IncCntIODMCError(PerfStat* stat,uint16_t incCnt)
{
	stat->uIODMCError += incCnt;
}

uint32_t NumOfPendingIO(PerfStat* stat)
{
	return (uint32_t)(stat->uIOSubmitted - stat->uIOCompleted);
}

void UptEclapsedTime(PerfStat* stat,uint64_t eclapsed)
{
	stat->uEclapsedTime	= eclapsed;
}

double CalcMBps(PerfStat* stat,uint32_t xferSize)
{
	return (double)( (stat->uIOCompleted * xferSize / MiB_BYTES) / (double)(stat->uEclapsedTime / NSEC_PER_MSEC) ) * MSEC_PER_SEC;
}

double CalcIOps(PerfStat* stat)
{
	return (double)( stat->uIOCompleted / (double)(stat->uEclapsedTime / NSEC_PER_MSEC) ) * MSEC_PER_SEC;
}

uint32_t GetIOCmdErrCnt(PerfStat* stat)
{
	return stat->uIOErrors;
}

uint32_t GetIODMCErrCnt(PerfStat* stat)
{
	return stat->uIODMCError;
}


