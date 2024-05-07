#ifndef _PERF_STAT_H
#define _PERF_STAT_H

#include <stdint.h>

typedef struct _stPerfStat
{
	uint64_t	uIOCompleted;
	uint64_t	uIOSubmitted;
	uint32_t	uIOErrors;
	uint32_t	uIODMCError;

	uint64_t	uEclapsedTime;
}PerfStat;

#ifdef __cplusplus
extern "C" {
#endif

void InitPerfStat(PerfStat* stat);
void IncCntIOCompleted(PerfStat* stat,uint16_t incCnt);
void IncCntIOSubmitted(PerfStat* stat,uint16_t incCnt);
void IncCntIOError(PerfStat* stat,uint16_t incCnt);
void IncCntIODMCError(PerfStat* stat,uint16_t incCnt);
uint32_t NumOfPendingIO(PerfStat* stat);
void UptEclapsedTime(PerfStat* stat,uint64_t eclapsed);

double CalcMBps(PerfStat* stat,uint32_t xferSize);
double CalcIOps(PerfStat* stat);
uint32_t GetIOCmdErrCnt(PerfStat* stat);
uint32_t GetIODMCErrCnt(PerfStat* stat);

#ifdef __cplusplus
}
#endif

#endif
