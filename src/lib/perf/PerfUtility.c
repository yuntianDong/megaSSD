/*
 * file : PerfUtility.c
 */

#include "perf/PerfUtility.h"

void	GetCurrentTimestamp(struct timespec* timestamp)
{
	clock_gettime(CLOCK_MONOTONIC,timestamp);
}

uint64_t GetTimeEclapsed(struct timespec* end,struct timespec* start)
{
	uint64_t eclapsed = 0;

	if( (end->tv_nsec - start->tv_nsec) < 0 )
	{
		eclapsed = (uint64_t)(end->tv_sec - start->tv_sec - 1) * NSEC_PER_SEC +
				(uint64_t)(NSEC_PER_SEC+end->tv_nsec-start->tv_nsec);
	}
	else
	{
		eclapsed = (uint64_t)(end->tv_sec - start->tv_sec) * NSEC_PER_SEC +
				(uint64_t)(end->tv_nsec-start->tv_nsec);
	}

	return eclapsed;
}
