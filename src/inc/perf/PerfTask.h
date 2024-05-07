#ifndef _PERF_TASK_H
#define _PERF_TASK_H

#include <stdint.h>

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

#define RANDOMIZE_SEQ_ITEMS		1024
#define MAX_USERS				32
#define XFER_SIZE_ALIGN			512

typedef struct _PerfTask
{
	uint64_t	llStartPos;
	uint64_t	llEndPos;
	uint32_t	lXferMax;
	uint32_t	lXferMin;
	uint32_t	lDataPat;

	enTaskMode	eTaskMode;

	struct {
		char	bAddrOverlay	: 1;
		char	bRandXfer		: 1;
	}opt;

	struct
	{
		struct
		{
			uint16_t	uRandIndex;
			uint32_t	uRangeIndex;
		}user[MAX_USERS];

		uint16_t	uRandomizer[RANDOMIZE_SEQ_ITEMS];
		uint16_t	uMaxRandIdx;
		uint32_t	uMaxRangeCnt;
	}run;
}PerfTask;

#ifdef __cplusplus
extern "C" {
#endif

void InitPerfTask(PerfTask* task);
void GetOpOffset(PerfTask* task,uint8_t userID,uint64_t* offset);
void GetOpXferSize(PerfTask* task,uint8_t userID,uint32_t* xferSize);
void FillOpDataBuf(PerfTask* task,uint8_t userID,uint8_t* bufAddr,uint32_t bufSize);
void UptOpDataBuf(PerfTask* task,uint8_t* bufAddr,uint32_t bufSize,uint64_t offset,uint32_t sectorSize);
void FillTaskWSeqWrite(PerfTask* task,uint64_t startPos,uint64_t length,uint32_t xferSize,uint32_t pattern);
void FillTaskWSeqRead(PerfTask* task,uint64_t startPos,uint64_t length,uint32_t xferSize,uint32_t pattern);
void FillTaskWRandWrite(PerfTask* task,uint64_t startPos,uint64_t length,uint32_t xferSize,uint32_t pattern);
void FillTaskWRandRead(PerfTask* task,uint64_t startPos,uint64_t length,uint32_t xferSize,uint32_t pattern);
void SetAddrOverlay(PerfTask* task,char bAddrOL);
void SetRandXferSize(PerfTask* task,uint32_t xferMax,uint32_t xferMin);

#ifdef __cplusplus
}
#endif

#endif
