#ifndef _AIO_PERF_ENGINE_H
#define _AIO_PERF_ENGINE_H

#include "perf/PerfEngine.h"

#ifdef __cplusplus
extern "C" {
#endif

int aio_setup(PerfContext* context,PerfParam* param);
void aio_cleanup(PerfContext* context);
int aio_prepare_io(PerfContext* context,PerfQueue* queue,PerfTask* task);
int aio_submit_io(PerfContext* context,PerfQueue* queue,PerfStat* stat);
int aio_check_io(PerfContext* context,PerfQueue* queue,PerfStat* stat);
int aio_verify_io(PerfContext* context,PerfQueue* queue,PerfStat* stat);

#ifdef __cplusplus
}
#endif

#endif
