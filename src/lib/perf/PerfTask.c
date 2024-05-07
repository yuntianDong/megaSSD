/*
 * file : PerfTask.c
 */

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "perf/PerfTask.h"

#define	DEF_START_POS			0
#define DEF_END_POS				1024*1024*1024
#define DEF_XFER_SIZE			128*1024
#define DEF_DATA_PATTERN		0x57CAAC75
#define DEF_TASK_MODE			PERF_T_SEQ_WR

void InitRandmizer(PerfTask* task,char bNeedRand)
{
	uint16_t idx=0;
	uint16_t* randmizer 	= task->run.uRandomizer;

	for(idx=0;idx<RANDOMIZE_SEQ_ITEMS;idx++)
	{
		randmizer[idx] = idx;
	}

	if(1 == bNeedRand)
	{
		srand((unsigned)time(NULL));

		uint16_t lastIdx = 0;
		uint16_t lastVal = 0;
		uint16_t loopCnt = RANDOMIZE_SEQ_ITEMS / 2;
		while(loopCnt--)
		{
			uint16_t curIdx 	= rand() % RANDOMIZE_SEQ_ITEMS;
			uint16_t tmpVal		= randmizer[curIdx];

			randmizer[curIdx]	= lastVal;
			randmizer[lastIdx]	= tmpVal;

			lastIdx	= curIdx;
			lastVal = tmpVal;
		}
	}

	uint64_t	tmpVal		= (uint64_t)( (task->llEndPos - task->llStartPos) / (task->lXferMax) );
	task->run.uMaxRangeCnt	= (uint32_t)(tmpVal / RANDOMIZE_SEQ_ITEMS);
	task->run.uMaxRangeCnt	+= (tmpVal % RANDOMIZE_SEQ_ITEMS)?1:0;

	task->run.uMaxRandIdx	= (uint32_t)tmpVal;
	if(task->run.uMaxRandIdx > RANDOMIZE_SEQ_ITEMS)
	{
		task->run.uMaxRandIdx = RANDOMIZE_SEQ_ITEMS;
	}
}

char IsRandMode(enTaskMode mode)
{
	if(mode > PERF_T_SEQ_TR)
	{
		return 1;
	}

	return 0;
}

void InitPerfTask(PerfTask* task)
{
	memset(task,0,sizeof(PerfTask));

	task->llStartPos	= DEF_START_POS;
	task->llEndPos		= DEF_END_POS;
	task->lXferMax = task->lXferMin	= DEF_XFER_SIZE;
	task->lDataPat		= DEF_DATA_PATTERN;
	task->eTaskMode		= DEF_TASK_MODE;

	task->opt.bAddrOverlay	= 0;
	task->opt.bRandXfer		= 0;
}

void GetOpOffset(PerfTask* task,uint8_t userID,uint64_t* offset)
{
	uint16_t	randIdx		= task->run.user[userID].uRandIndex;
	uint32_t	rangeIdx	= task->run.user[userID].uRangeIndex;
	uint16_t	maxRandIdx	= task->run.uMaxRandIdx;
	uint32_t	maxRangeIdx	= task->run.uMaxRangeCnt;
	uint32_t	xferSize	= task->lXferMax;

	uint16_t*	randomizer	= task->run.uRandomizer;

	*offset	= (randomizer[randIdx] + rangeIdx*RANDOMIZE_SEQ_ITEMS) * xferSize;

	if(++randIdx >= maxRandIdx)
	{
		randIdx = 0;
		if(++rangeIdx >= maxRangeIdx)
		{
			rangeIdx = 0;
		}
		else if(rangeIdx == maxRangeIdx -1)
		{
			uint64_t tmpVal = task->llEndPos - task->llStartPos - (rangeIdx * RANDOMIZE_SEQ_ITEMS * xferSize);
			task->run.uMaxRandIdx = (tmpVal % xferSize)?(tmpVal/xferSize +1):(tmpVal/xferSize);
		}

		task->run.user[userID].uRangeIndex	= rangeIdx;
	}

	task->run.user[userID].uRandIndex	= randIdx;
}

void GetOpXferSize(PerfTask* task,uint8_t userID,uint32_t* xferSize)
{
	if(0 == task->opt.bRandXfer)
	{
		*xferSize = task->lXferMax;
	}
	else
	{
		uint32_t	xferMin	= task->lXferMin;
		uint32_t	xferMax	= task->lXferMax;
		uint16_t	randIdx	= task->run.user[userID].uRandIndex;
		uint16_t	randVal	= task->run.uRandomizer[randIdx];

		*xferSize =  xferMin + (xferMax - xferMin) * randVal / RANDOMIZE_SEQ_ITEMS;
		*xferSize =  (*xferSize) & ( (uint32_t)(~XFER_SIZE_ALIGN) ^ (uint32_t)(XFER_SIZE_ALIGN-1));
	}
}

void FillOpDataBuf(PerfTask* task,uint8_t userID,uint8_t* bufAddr,uint32_t bufLen)
{
	uint16_t idx=0;
	for(idx=0; idx<(bufLen/sizeof(uint32_t)); idx++)
	{
		*((uint32_t*)bufAddr + idx)	= task->lDataPat;
	}
}

void UptOpDataBuf(PerfTask* task,uint8_t* bufAddr,uint32_t bufSize,uint64_t offset,uint32_t sectorSize)
{
	if(0 == task->opt.bAddrOverlay)
	{
		return;
	}

	uint16_t sectorCnt	= bufSize / sectorSize;
	uint64_t logicAddr	= offset / sectorSize;
	uint8_t*	dataBuf	= bufAddr;

	uint16_t idx=0;
	for(idx=0;idx<sectorCnt;idx++)
	{
		*((uint64_t*)dataBuf)	= logicAddr+idx;
		dataBuf += sectorSize;
	}

	return;
}

void FillTaskWSeqWrite(PerfTask* task,uint64_t startPos,uint64_t length,uint32_t xferSize,uint32_t pattern)
{
	task->llStartPos	= startPos;
	task->llEndPos		= startPos + length;
	task->lXferMax = task->lXferMin	= xferSize;
	task->lDataPat		= pattern;
	task->eTaskMode		= PERF_T_SEQ_WR;

	InitRandmizer(task,IsRandMode(task->eTaskMode));
}

void FillTaskWSeqRead(PerfTask* task,uint64_t startPos,uint64_t length,uint32_t xferSize,uint32_t pattern)
{
	task->llStartPos	= startPos;
	task->llEndPos		= startPos + length;
	task->lXferMax = task->lXferMin	= xferSize;
	task->lDataPat		= pattern;
	task->eTaskMode		= PERF_T_SEQ_RD;

	InitRandmizer(task,IsRandMode(task->eTaskMode));
}

void FillTaskWRandWrite(PerfTask* task,uint64_t startPos,uint64_t length,uint32_t xferSize,uint32_t pattern)
{
	task->llStartPos	= startPos;
	task->llEndPos		= startPos + length;
	task->lXferMax = task->lXferMin	= xferSize;
	task->lDataPat		= pattern;
	task->eTaskMode		= PERF_T_RND_WR;

	InitRandmizer(task,IsRandMode(task->eTaskMode));
}

void FillTaskWRandRead(PerfTask* task,uint64_t startPos,uint64_t length,uint32_t xferSize,uint32_t pattern)
{
	task->llStartPos	= startPos;
	task->llEndPos		= startPos + length;
	task->lXferMax = task->lXferMin	= xferSize;
	task->lDataPat		= pattern;
	task->eTaskMode		= PERF_T_RND_RD;

	InitRandmizer(task,IsRandMode(task->eTaskMode));
}

void SetAddrOverlay(PerfTask* task,char bAddrOL)
{
	task->opt.bAddrOverlay	= bAddrOL;
}

void SetRandXferSize(PerfTask* task,uint32_t xferMax,uint32_t xferMin)
{
	task->opt.bRandXfer		= 1;
	task->lXferMax			= xferMax;
	task->lXferMin			= xferMin;
}

