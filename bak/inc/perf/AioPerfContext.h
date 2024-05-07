#ifndef _AIO_PERF_CONTEXT_H
#define _AIO_PERF_CONTEXT_H

#include <libaio.h>

#include "PerfContext.h"

class AioPerfContext : public BasicPerfContext
{
public:
	struct io_event		*mpAIOEvents;
	io_context_t		mAIOCtx;

	AioPerfContext(void)
		:mpAIOEvents(NULL)
	{
	}

	virtual ~AioPerfContext(void)
	{
	};
};

#endif
