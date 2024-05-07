#ifndef _PERF_FUNC_TABLE_H
#define _PERF_FUNC_TABLE_H

#include <stdint.h>

#include "utility/ClkTimer.h"
#include "PerfContext.h"
#include "PerfStat.h"
#include "PerfErrorCode.h"
#include "Logging.h"

class BasicPerfEngine
{
protected:
	BasicPerfContext*	mpContext;

	bool				mbIsBlocked;
	bool				mbNeedVerify;
	bool				mbStopWhenError;
	bool				mbTimeBased;

	uint64_t			mDebugTimer1;
	uint64_t			mDebugTimer2;
	uint64_t			mDebugTimer3;

	bool	IsCmdQueueEmpty(void);
	bool	IsCmdQueueFull(void);

	uint32_t GetInQueueCnt(void);
	uint32_t GetQueueMaxIO(void);

	virtual void io_completed(bool error,void* data);
	virtual BasicPerfContext*	GetPerfContext(void);
public:
	BasicPerfEngine(void);
	virtual ~BasicPerfEngine(void);

	void SetStopWhenError(bool Enabled){mbStopWhenError = Enabled;};
	void SetTimeBased(bool Enabled){mbTimeBased = Enabled;};
	void SetNeedVerify(bool Enabled){mbNeedVerify = Enabled;};

	bool GetStopWhenError(void) {return mbStopWhenError;};
	bool GetTimeBased(void) {return mbTimeBased;};
	bool GetNeedVerify(void) {return mbNeedVerify;};

	virtual bool init_engine(void);
	virtual void clean_engine(void);
	virtual void check_io_complete(void);
	virtual bool submit_io(void);
	virtual bool verify_io(void);

	int Run(uint64_t devHdlr,uint32_t wId,
			BasicPerfTask* task,BasicPerfConfig* cfg,BasicPerfStat* stat);
};

#endif
