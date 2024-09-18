/*
 * file : BufferMngr.cpp
 */
#include <stdlib.h>
#include <string.h>

#include "buffer/BufferMngr.h"

IoBufPool::IoBufPool(uint32_t bufItemSize, uint32_t bufItemMax, uint32_t sectorSize)
	: RingBufPool(bufItemSize, bufItemMax, BUF_T_IO, true), muSectorSize(sectorSize)
{
}

Buffers *IoBufPool::CreateBufItem(uint64_t bufSize, const char *mode)
{
	bufSize = GetBufItemSize();

	switch (menBufType)
	{
	case BUF_T_IO:
		return (Buffers *)new SectorBuf(bufSize, muSectorSize, mode);
	case BUF_T_PATIO:
		return (Buffers *)new PatternBuf(bufSize, muSectorSize, mode);
	case BUF_T_UNK:
	default:
		LOGERROR("Invalid bufType : %d", menBufType);
		return NULL;
	}

	return NULL;
}

bool IoBufPool::NeedReallocated(void *pArgs)
{
	if (NULL == pArgs)
	{
		return false;
	}
	uint32_t expSectorSize = *((uint32_t *)pArgs);

	return expSectorSize != muSectorSize;
}

SysBufPool::SysBufPool(uint32_t maxBufPoolSize)
	: LimSizeBufPool(maxBufPoolSize, BUF_T_UNK, true)
{
}

FrmBufPool::FrmBufPool(const char *bufTag, uint32_t maxBufPoolSize)
	: MaxSizeBufPool(maxBufPoolSize, BUF_T_PAT_REC, true)
{
	strncpy(mBufTag, bufTag, FRMWK_POOL_NAME_LEN - 1);
	DisableCreateNewBufIfLargerSizeFlag();
}

Buffers *FrmBufPool::CreateBufItem(uint32_t bufSize, const char *mode)
{
	switch (menBufType)
	{
	case BUF_T_PAT_REC:
		return (Buffers *)new SwapBuf(mBufTag, bufSize, mode);
	case BUF_T_UNK:
	default:
		LOGERROR("Invalid bufType : %d", menBufType);
		return NULL;
	}

	return NULL;
}

BufferMngr *BufferMngr::gInstance = NULL;

BufferMngr *BufferMngr::GetInstance(void)
{
	if (NULL == gInstance)
	{
		gInstance = new BufferMngr();
	}

	return gInstance;
}

BufferMngr::BufferMngr(void)
	: mpIoBufPool(NULL), mpSecuBufPool(NULL), mpSysBufPool(NULL), mpFrmBufPool(NULL)
{
}

BufferMngr::~BufferMngr()
{
	if (NULL != mpIoBufPool)
	{
		delete mpIoBufPool;
		mpIoBufPool = NULL;
	}

	if (NULL != mpSecuBufPool)
	{
		delete mpSecuBufPool;
		mpSecuBufPool = NULL;
	}

	if (NULL != mpSysBufPool)
	{
		delete mpSysBufPool;
		mpSysBufPool = NULL;
	}

	if (NULL != mpFrmBufPool)
	{
		delete mpFrmBufPool;
		mpFrmBufPool = NULL;
	}
}

void BufferMngr::CfgIoBufPool(uint32_t bufItemSize, uint32_t bufItemMax, uint32_t sectorSize)
{
	if (NULL != mpIoBufPool)
	{
		delete mpIoBufPool;
		mpIoBufPool = NULL;
	}
	mpIoBufPool = (BaseBufPool *)new IoBufPool(bufItemSize, bufItemMax, sectorSize);
}

void BufferMngr::CfgSecuBufPool(uint32_t bufItemSize, uint32_t bufItemMax, uint32_t sectorSize)
{
	if (NULL != mpSecuBufPool)
	{
		delete mpSecuBufPool;
		mpSecuBufPool = NULL;
	}
	mpSecuBufPool = (BaseBufPool *)new IoBufPool(bufItemSize, bufItemMax, sectorSize);
}

void BufferMngr::CfgSysBufPool(uint32_t maxBufPoolSize)
{
	if (NULL != mpSysBufPool)
	{
		delete mpSysBufPool;
		mpSysBufPool = NULL;
	}
	mpSysBufPool = (BaseBufPool *)new SysBufPool(maxBufPoolSize);
}

void BufferMngr::CfgFrmBufPool(const char *bufTag, uint32_t maxBufPoolSize)
{
	if (NULL != mpFrmBufPool)
	{
		delete mpFrmBufPool;
		mpFrmBufPool = NULL;
	}
	mpFrmBufPool = (BaseBufPool *)new FrmBufPool(bufTag, maxBufPoolSize);
}
