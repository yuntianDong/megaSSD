/*
 * file : perfTask.cpp
 */
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "perf/PerfTask.h"

BasicPerfTask::BasicPerfTask(uint64_t start,uint64_t length,uint32_t xferSize,uint32_t pattern,enTaskMode mode)
	:mStartPos(start),mEndPos(start+length),mXferSize(xferSize),mDataPat(pattern),mTaskMode(mode)
{
	mBuf	= (uint8_t*)malloc(xferSize*sizeof(uint8_t));
	mCurrentPos	= mStartPos;
	mRandIndex	= 0;

	memset(mRandomize,0,sizeof(uint16_t)*RANDOMIZE_SEQ_ITEMS);

	Randomization();
}

BasicPerfTask::~BasicPerfTask(void)
{
	if(NULL != mBuf)
	{
		free(mBuf);
		mBuf	= NULL;
	}
}

bool BasicPerfTask::NeedRandomize(void)
{
	return (mTaskMode >= PERF_T_RND_RD && mTaskMode <= PERF_T_RND_TR);
}

void BasicPerfTask::Randomization(void)
{
	for(int idx=0;idx<RANDOMIZE_SEQ_ITEMS;idx++)
	{
		mRandomize[idx]	= idx;
	}

	uint16_t lastIdx = 0;
	uint16_t lastVal = 0;
	uint16_t loopCnt = RANDOMIZE_SEQ_ITEMS / 2;

	while(true == NeedRandomize() && 0 < --loopCnt)
	{
		if(0 == lastIdx && 0 == lastVal)
		{
			srand((unsigned)time(NULL));
		}

		uint16_t curIdx 	= rand() % RANDOMIZE_SEQ_ITEMS;
		uint16_t tmpVal		= mRandomize[curIdx];

		mRandomize[curIdx]	= lastVal;
		mRandomize[lastIdx]	= tmpVal;

		lastIdx	= curIdx;
		lastVal = tmpVal;
	}
}

bool BasicPerfTask::GetNextOper(PerfOper& op)
{
	if(NULL == mBuf)
	{
		return false;
	}

	op.opType	= (enOpType)((uint8_t)mTaskMode % (uint8_t)OP_FENCE);
	op.offset	= mCurrentPos + mRandomize[mRandIndex++] * mXferSize;
	op.length	= mXferSize;
	op.buf		= mBuf;
	op.nbufs	= mXferSize;

	if(mRandIndex >= RANDOMIZE_SEQ_ITEMS)
	{
		mRandIndex = 0;
		mCurrentPos += mXferSize * RANDOMIZE_SEQ_ITEMS;
	}

	if(mCurrentPos >= mEndPos)
	{
		mCurrentPos = mStartPos;
	}

	return true;
}

void BasicPerfTask::Reset(void)
{
	mCurrentPos	= mStartPos;
	mRandIndex	= 0;
}
