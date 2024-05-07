#ifndef _PERF_UTILITY_H
#define _PERF_UTILITY_H

#include <stdint.h>
#include <time.h>

#define MSEC_PER_SEC		1000
#define USEC_PER_SEC		1000000
#define NSEC_PER_MSEC		1000000
#define NSEC_PER_SEC		1000000000

#ifdef __cplusplus
extern "C" {
#endif

uint64_t GetTimeEclapsed(struct timespec* end,struct timespec *start);
void	GetCurrentTimestamp(struct timespec* timestamp);

#ifdef __cplusplus
}
#endif
#endif
