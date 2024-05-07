#ifndef _BLOCK_ENGINE_H
#define _BLOCK_ENGINE_H

#include "BlkOpResp.h"

class BlockEngine
{
private:
	int		mFD;
protected:

public:
	BlockEngine(int fd)
		:mFD(fd)
	{};

	BlkOpResp* BlkDiscard(uint64_t offset,uint64_t length);
	BlkOpResp* BlkDiscardWithSecu(uint64_t offset,uint64_t length);
	BlkOpResp* BlkZeroOut(uint64_t offset,uint64_t length);
	BlkOpResp* BlkFlushBuf(void);
	BlkOpResp* BlkGetTotalSize(void);
	BlkOpResp* BlkGetSoftBlockSize(void);
	BlkOpResp* BlkGetLogBlockSize(void);
	BlkOpResp* BlkGetPhyBlockSize(void);
	BlkOpResp* BlkGetIOMin(void);
	BlkOpResp* BlkGetIOOpt(void);
	BlkOpResp* BlkGetAlignOffset(void);
	BlkOpResp* BlkGetMaxSectors(void);
	BlkOpResp* BlkSetLogBlockSize(uint32_t logBlkSize);
	BlkOpResp* BlkGetDiskROFlag(void);
};

#endif
