/*
 * file : IBlkDevMngr.cpp
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "IBlkDevMngr.h"

#define INVALID_DEVHDLR					int(-1)

int OpenDevice(const char* name)
{
	int fd	= INVALID_DEVHDLR;

	if( (fd = open(name,O_RDWR)) < 0 )
	{
		printf("ERROR: Cannot open device: %s\n",name);
		return INVALID_DEVHDLR;
	}

	return fd;
}

IBlkDevMngr::IBlkDevMngr(const char* devName)
	:mpBufDevLock(NULL)
{
	int fd = OpenDevice(devName);
	if(INVALID_DEVHDLR != fd)
	{
		BlkDevMngr* pBlkDevMngr = new BlkDevMngr(fd);
		mpBufDevLock = new externally_locked<BlkDevMngr,lockable_type>(\
				*(this->mLock),*pBlkDevMngr);
	}
}

IBlkDevMngr::~IBlkDevMngr(void)
{
	if(NULL != mpBufDevLock)
	{
		delete mpBufDevLock;
		mpBufDevLock = NULL;
	}
}

bool IBlkDevMngr::BlkDiscard(uint64_t offset,uint64_t length,uint32_t timeout)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	uint32_t execTime = 0;

	bool bRtn = mpBufDevLock->get(guard).BlkDiscard(offset,length,execTime);
	if(0 != timeout && execTime > timeout)
	{
		return false;
	}

	return bRtn;
}

bool IBlkDevMngr::BlkDiscardWithSecu(uint64_t offset,uint64_t length,uint32_t timeout)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	uint32_t execTime = 0;

	bool bRtn = mpBufDevLock->get(guard).BlkDiscardWithSecu(offset,length,execTime);
	if(0 != timeout && execTime > timeout)
	{
		return false;
	}

	return bRtn;
}

bool IBlkDevMngr::BlkZeroOut(uint64_t offset,uint64_t length,uint32_t timeout)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	uint32_t execTime = 0;

	bool bRtn = mpBufDevLock->get(guard).BlkZeroOut(offset,length,execTime);
	if(0 != timeout && execTime > timeout)
	{
		return false;
	}

	return bRtn;
}

bool IBlkDevMngr::BlkFlushBuf(uint32_t timeout)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	uint32_t execTime = 0;

	bool bRtn = mpBufDevLock->get(guard).BlkFlushBuf(execTime);
	if(0 != timeout && execTime > timeout)
	{
		return false;
	}

	return bRtn;
}

uint64_t IBlkDevMngr::BlkGetTotalSize(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufDevLock->get(guard).BlkGetTotalSize();
}

uint32_t IBlkDevMngr::BlkGetSoftBlockSize(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufDevLock->get(guard).BlkGetSoftBlockSize();
}

uint32_t IBlkDevMngr::BlkGetLogBlockSize(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufDevLock->get(guard).BlkGetLogBlockSize();
}

uint32_t IBlkDevMngr::BlkGetPhyBlockSize(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufDevLock->get(guard).BlkGetPhyBlockSize();
}

uint32_t IBlkDevMngr::BlkGetIOMin(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufDevLock->get(guard).BlkGetIOMin();
}

uint32_t IBlkDevMngr::BlkGetIOOpt(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufDevLock->get(guard).BlkGetIOOpt();
}

uint32_t IBlkDevMngr::BlkGetAlignOffset(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufDevLock->get(guard).BlkGetAlignOffset();
}

uint32_t IBlkDevMngr::BlkGetMaxSectors(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufDevLock->get(guard).BlkGetMaxSectors();
}

bool IBlkDevMngr::BlkSetLogBlockSize(uint32_t logBlkSize)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufDevLock->get(guard).BlkSetLogBlockSize(logBlkSize);
}

bool IBlkDevMngr::BlkGetDiskROFlag(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufDevLock->get(guard).BlkGetDiskROFlag();
}

