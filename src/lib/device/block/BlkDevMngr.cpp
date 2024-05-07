/*
 * BlkDevMngr.cpp
 */

#include "device/block/BlkDevMngr.h"
#include "Logging.h"

BlkDevMngr::BlkDevMngr(int fd)
	:mpBlkEngr(NULL)
{
	mpBlkEngr	= new BlockEngine(fd);
}

BlkDevMngr::~BlkDevMngr(void)
{
	if(NULL != mpBlkEngr)
	{
		delete mpBlkEngr;
		mpBlkEngr = NULL;
	}
}

bool BlkDevMngr::BlkDiscard(uint64_t offset,uint64_t length,uint32_t &execTime)
{
	BlkOpResp* resp = mpBlkEngr->BlkDiscard(offset,length);
	if(false == resp->Succeeded())
	{
		return false;
	}

	execTime = resp->GetExecTime();
	return true;
}

bool BlkDevMngr::BlkDiscardWithSecu(uint64_t offset,uint64_t length,uint32_t &execTime)
{
	BlkOpResp* resp = mpBlkEngr->BlkDiscardWithSecu(offset,length);
	if(false == resp->Succeeded())
	{
		return false;
	}

	execTime = resp->GetExecTime();
	return true;
}

bool BlkDevMngr::BlkZeroOut(uint64_t offset,uint64_t length,uint32_t &execTime)
{
	BlkOpResp* resp = mpBlkEngr->BlkZeroOut(offset,length);
	if(false == resp->Succeeded())
	{
		return false;
	}

	execTime = resp->GetExecTime();
	return true;
}

bool BlkDevMngr::BlkFlushBuf(uint32_t &execTime)
{
	BlkOpResp* resp = mpBlkEngr->BlkFlushBuf();
	if(false == resp->Succeeded())
	{
		return false;
	}

	execTime = resp->GetExecTime();
	return true;
}

uint64_t BlkDevMngr::BlkGetTotalSize(void)
{
	BlkOpResp* resp = mpBlkEngr->BlkGetTotalSize();
	if(false == resp->Succeeded())
	{
		return (uint64_t)-1;
	}

	return resp->GetRespData();
}

uint32_t BlkDevMngr::BlkGetSoftBlockSize(void)
{
	BlkOpResp* resp = mpBlkEngr->BlkGetSoftBlockSize();
	if(false == resp->Succeeded())
	{
		return (uint32_t)-1;
	}

	return resp->GetRespData();
}

uint32_t BlkDevMngr::BlkGetLogBlockSize(void)
{
	BlkOpResp* resp = mpBlkEngr->BlkGetLogBlockSize();
	if(false == resp->Succeeded())
	{
		return (uint32_t)-1;
	}

	return resp->GetRespData();
}

uint32_t BlkDevMngr::BlkGetPhyBlockSize(void)
{
	BlkOpResp* resp = mpBlkEngr->BlkGetPhyBlockSize();
	if(false == resp->Succeeded())
	{
		return (uint32_t)-1;
	}

	return resp->GetRespData();
}

uint32_t BlkDevMngr::BlkGetIOMin(void)
{
	BlkOpResp* resp = mpBlkEngr->BlkGetIOMin();
	if(false == resp->Succeeded())
	{
		return (uint32_t)-1;
	}

	return resp->GetRespData();
}

uint32_t BlkDevMngr::BlkGetIOOpt(void)
{
	BlkOpResp* resp = mpBlkEngr->BlkGetIOOpt();
	if(false == resp->Succeeded())
	{
		return (uint32_t)-1;
	}

	return resp->GetRespData();
}

uint32_t BlkDevMngr::BlkGetAlignOffset(void)
{
	BlkOpResp* resp = mpBlkEngr->BlkGetAlignOffset();
	if(false == resp->Succeeded())
	{
		return (uint32_t)-1;
	}

	return resp->GetRespData();
}

uint32_t BlkDevMngr::BlkGetMaxSectors(void)
{
	BlkOpResp* resp = mpBlkEngr->BlkGetMaxSectors();
	if(false == resp->Succeeded())
	{
		return (uint32_t)-1;
	}

	return resp->GetRespData();
}

bool BlkDevMngr::BlkSetLogBlockSize(uint32_t logBlkSize)
{
	BlkOpResp* resp = mpBlkEngr->BlkSetLogBlockSize(logBlkSize);

	return resp->Succeeded();
}

bool BlkDevMngr::BlkGetDiskROFlag(void)
{
	BlkOpResp* resp = mpBlkEngr->BlkGetDiskROFlag();

	return resp->Succeeded() && (0 < resp->GetRespData());
}

