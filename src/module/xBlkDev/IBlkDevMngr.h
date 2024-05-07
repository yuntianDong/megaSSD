#ifndef _IBLK_DEV_MNGR_H
#define _IBLK_DEV_MNGR_H

#include "device/block/BlkDevMngr.h"
#include "IBasicLock.h"

class IBlkDevMngr : public IBasicLock
{
protected:
	externally_locked<BlkDevMngr,lockable_type>		*mpBufDevLock;

public:
	IBlkDevMngr(const char* devName);
	virtual ~IBlkDevMngr(void);

	bool BlkDiscard(uint64_t offset,uint64_t length,uint32_t timeout);
	bool BlkDiscardWithSecu(uint64_t offset,uint64_t length,uint32_t timeout);
	bool BlkZeroOut(uint64_t offset,uint64_t length,uint32_t timeout);
	bool BlkFlushBuf(uint32_t timeout);
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
