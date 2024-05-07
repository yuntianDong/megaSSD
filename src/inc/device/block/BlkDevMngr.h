#ifndef _BLK_DEV_MNGR_H
#define _BLK_DEV_MNGR_H

#include "driver/block/BlockEngine.h"

class BlkDevMngr
{
protected:
	BlockEngine*	mpBlkEngr;
public:
	BlkDevMngr(int fd);
	virtual ~BlkDevMngr(void);

	bool BlkDiscard(uint64_t offset,uint64_t length,uint32_t &execTime);
	bool BlkDiscardWithSecu(uint64_t offset,uint64_t length,uint32_t &execTime);
	bool BlkZeroOut(uint64_t offset,uint64_t length,uint32_t &execTime);
	bool BlkFlushBuf(uint32_t &execTime);
	uint64_t BlkGetTotalSize(void);
	uint32_t BlkGetSoftBlockSize(void);
	uint32_t BlkGetLogBlockSize(void);
	uint32_t BlkGetPhyBlockSize(void);
	uint32_t BlkGetIOMin(void);
	uint32_t BlkGetIOOpt(void);
	uint32_t BlkGetAlignOffset(void);
	uint32_t BlkGetMaxSectors(void);
	bool BlkSetLogBlockSize(uint32_t logBlkSize);
	bool BlkGetDiskROFlag(void);
};

#endif
