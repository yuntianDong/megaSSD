#ifndef _PATTERN_MNGR_H
#define _PATTERN_MNGR_H

#include "PatternRecorder.h"
#include "PatternBitmap.h"

/*
 *	PatternMngr
 *
 *		index 	: PatternRecoder
 */
typedef enum _stPatRecType
{
	PATREC_BITMAP,
	PATREC_CRC8,
	PATREC_UNK,
} stPatRecType;
class PatternMngr
{
private:
	stPatRecType mPatRecType;
	PatternRecoder *mPatRec;

public:
	PatternMngr(stPatRecType type, Buffers *bitMap, bool bAddrOL);
	~PatternMngr();

	bool RecordFromWrBuf(Buffers *wrBuf, uint32_t bufOffset, uint64_t bitMapOffset, uint32_t length);
	bool CompareWithRdBuf(Buffers *rdBuf, uint32_t bufOffset, uint64_t bitMapOffset, uint32_t length, std::string bufmode);
	bool FillPattern(Buffers *cmpBuf, uint32_t bufOffset, uint64_t bitMapOffset, uint32_t length);
	bool DeletePattern(uint64_t bitMapOffset, uint32_t length);

	void DebugDump(uint32_t offset, uint32_t length);

	PatternRecoder *GetPatRec(void);
};

#endif
