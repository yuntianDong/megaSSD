/*
 * file : BlockEngine.cpp
 */
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/blkpg.h>

#include "driver/block/BlockEngine.h"
#include "utility/ClkTimer.h"
#include "Logging.h"

BlkOpResp* BlockEngine::BlkDiscard(uint64_t offset,uint64_t length)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	uint64_t range[2];
	range[0]	= offset;
	range[1]	= length;

	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKDISCARD,range);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);

	return resp;
}

BlkOpResp* BlockEngine::BlkDiscardWithSecu(uint64_t offset,uint64_t length)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	uint64_t range[2];
	range[0]	= offset;
	range[1]	= length;

	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKSECDISCARD,range);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);

	return resp;
}

BlkOpResp* BlockEngine::BlkZeroOut(uint64_t offset,uint64_t length)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	uint64_t range[2];
	range[0]	= offset;
	range[1]	= length;

	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKZEROOUT,range);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);

	return resp;
}

BlkOpResp* BlockEngine::BlkFlushBuf(void)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKFLSBUF,NULL);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);

	return resp;
}

BlkOpResp* BlockEngine::BlkGetTotalSize(void)
{
	uint64_t exeTime	= 0;
	int rc				= 0;
	uint64_t blkTotalBytes = (uint64_t)-1;

	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKGETSIZE64,&blkTotalBytes);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);
	resp->SetRespData((uint64_t)blkTotalBytes);

	return resp;
}

BlkOpResp* BlockEngine::BlkGetSoftBlockSize(void)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	int softBlkSize	= (int)-1;
	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKBSZGET,&softBlkSize);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);
	resp->SetRespData((uint64_t)softBlkSize);

	return resp;
}

BlkOpResp* BlockEngine::BlkGetLogBlockSize(void)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	int logicBlkSize	= (int)-1;
	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKSSZGET,&logicBlkSize);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);
	resp->SetRespData((uint64_t)logicBlkSize);

	return resp;
}

BlkOpResp* BlockEngine::BlkGetPhyBlockSize(void)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	uint32_t phyBlkSize	= (uint32_t)-1;
	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKPBSZGET,&phyBlkSize);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);
	resp->SetRespData((uint64_t)phyBlkSize);

	return resp;
}

BlkOpResp* BlockEngine::BlkGetIOMin(void)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	uint32_t ioMin	= (uint32_t)-1;
	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKIOMIN,&ioMin);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);
	resp->SetRespData((uint64_t)ioMin);

	return resp;
}

BlkOpResp* BlockEngine::BlkGetIOOpt(void)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	uint32_t ioOpt	= (uint32_t)-1;
	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKIOOPT,&ioOpt);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);
	resp->SetRespData((uint64_t)ioOpt);

	return resp;
}

BlkOpResp* BlockEngine::BlkGetAlignOffset(void)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	int alignOff	= (int)-1;
	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKALIGNOFF,&alignOff);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);
	resp->SetRespData((uint64_t)alignOff);

	return resp;
}

BlkOpResp* BlockEngine::BlkGetMaxSectors(void)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	uint16_t maxSectors	= (uint16_t)-1;
	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKSECTGET,&maxSectors);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);
	resp->SetRespData((uint64_t)maxSectors);

	return resp;
}

BlkOpResp* BlockEngine::BlkSetLogBlockSize(uint32_t logBlkSize)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKBSZSET,&logBlkSize);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);

	return resp;
}

BlkOpResp* BlockEngine::BlkGetDiskROFlag(void)
{
	uint64_t exeTime	= 0;
	int rc				= 0;

	int bROFlag			= (int)-1;
	if(true)
	{
		SysClkTimer timer(exeTime,TU_MS);
		rc = ioctl(mFD,BLKROGET,&bROFlag);
	}

	BlkOpResp* resp		= new BlkOpResp();
	resp->SetStatus(rc);
	resp->SetExecTime((uint32_t)exeTime);
	resp->SetRespData((uint64_t)bROFlag);

	return resp;
}

