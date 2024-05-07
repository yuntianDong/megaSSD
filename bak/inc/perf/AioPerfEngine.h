#ifndef _AIO_PERF_ENGINE_H
#define _AIO_PERF_ENGINE_H

#include <libaio.h>

#include "PerfEngine.h"
#include "AioPerfContext.h"

#define	MAX_IO_PER_CMD		16

class AioPerfEngine : public BasicPerfEngine
{
private:
	struct iocb*	mpIOCBs;
	uint8_t			mMaxIOsPerOnce;
	int				mEpollFd;

	int					mEventFd;
	struct epoll_event  *mpEPEvent;

	void InitEpEvent(void);
	void DeinitEpEvent(void);

	void GetIOEvent(uint32_t msTimeout);
protected:
	virtual BasicPerfContext*	GetPerfContext(void);
	virtual void io_completed(bool error,void* data);
public:
	AioPerfEngine(void);
	virtual ~AioPerfEngine(void);

	virtual bool init_engine(void);
	virtual void clean_engine(void);
	virtual void check_io_complete(void);
	virtual bool submit_io(void);
	virtual bool verify_io(void);
};

#endif
