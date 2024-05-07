#ifndef _PERF_WORKER_H
#define _PERF_WORKER_H

#include <stdint.h>

#include "PerfEngine.h"

#define INVALID_DEV_HANDLER				0xFFFFFFFFFFFFFFFF

class BasicPerfWorker
{
private:
	uint64_t			mDevHdlr;
	uint32_t			mWkID;
	BasicPerfStat		mPerfStat;

	BasicPerfTask*		mpPerfTask;
	BasicPerfConfig*	mpPerfConfig;
	BasicPerfEngine*	mpPerfEngine;
protected:
	virtual bool tearUp(BasicPerfEngine &engine) {return true;};
	virtual void tearDown(BasicPerfEngine &engine) {return;};
public:
	BasicPerfWorker(uint64_t devHdlr);

	double GetStatIOPS(void) {return mPerfStat.GetStatIOPS();};
	double GetStatMBps(void) {return mPerfStat.GetStatMBps();};
	uint32_t GetIOErrCnt(void) {return mPerfStat.GetIOErrCnt();};

	void SetPerfTask(BasicPerfTask* task) {mpPerfTask = task;};
	void SetPerfConfig(BasicPerfConfig* cfg) {mpPerfConfig = cfg;};
	void SetPerfEngine(BasicPerfEngine* engine) {mpPerfEngine = engine;};
	void SetWorkID(uint32_t wId){mWkID = wId;};

	int Execute(void);
};

#endif
