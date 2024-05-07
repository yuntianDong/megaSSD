#ifndef _PERF_TASK_H
#define _PERF_TASK_H

#include <stdint.h>

#define RANDOMIZE_SEQ_ITEMS		1024

typedef enum _enTaskMode
{
	PERF_T_SEQ_RD	= 0,
	PERF_T_SEQ_WR	= 1,
	PERF_T_SEQ_TR	= 2,
	PERF_T_RND_RD	= 6,
	PERF_T_RND_WR	= 7,
	PERF_T_RND_TR	= 8,
	PERF_T_FENCE	= PERF_T_RND_TR+1,
}enTaskMode;

typedef enum _enOpType
{
	OP_READ		= 0,
	OP_WRITE	= 1,
	OP_TRIM		= 2,
	OP_FENCE	= OP_TRIM+1,
}enOpType;

typedef struct _stPerfOper
{
	enOpType	opType;
	uint64_t	offset;
	uint32_t	length;
	void*		buf;
	uint32_t	nbufs;
}PerfOper;

class BasicPerfTask
{
private:
	uint64_t	mStartPos;
	uint64_t	mEndPos;
	uint32_t	mXferSize;
	uint32_t	mDataPat;
	enTaskMode	mTaskMode;

	uint8_t*	mBuf;
	uint64_t	mCurrentPos;
	uint16_t	mRandomize[RANDOMIZE_SEQ_ITEMS];
	uint16_t	mRandIndex;
protected:
	bool NeedRandomize(void);
	void Randomization(void);
public:
	BasicPerfTask(uint64_t start,uint64_t length,uint32_t xferSize,uint32_t pattern,enTaskMode mode);
	virtual ~BasicPerfTask(void);

	virtual bool GetNextOper(PerfOper& op);
	virtual void Reset(void);

	uint32_t GetXferSize(void) {return mXferSize;};
	uint32_t GetDataPat(void) {return mDataPat;};
};

#endif
