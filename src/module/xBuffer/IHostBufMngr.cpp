/*
 * file : IHostBufMngr.cpp
 */

#include "IHostBufMngr.h"

IHostBufMngr* IHostBufMngr::gInstance	= NULL;

IHostBufMngr* IHostBufMngr::GetInstance(void)
{
	if(NULL == gInstance)
	{
		gInstance	= new IHostBufMngr();
	}

	return gInstance;
}

IHostBufMngr::IHostBufMngr(void)
{
	muIOBufMaxSize		= DEF_HOST_IOBUF_ALLOCMAX;
	muIOBufItemSize		= DEF_HOST_IOBUF_ITEMSIZE;
	muMetaBufItemSize	= DEF_HOST_METABUF_ITEMSIZE;
	muSysBufMaxSize		= DEF_HOST_SYSBUF_ALLOCMAX;
	muFrmBufMaxSize		= DEF_HOST_FRMBUF_ALLOCMAX;

	menIOBufType		= DEF_HOST_IOBUF_TYPE;
	menMetaBufType		= DEF_HOST_METABUF_TYPE;
	menSysBufType		= DEF_HOST_SYSBUF_TYPE;
	menFrmBufType		= DEF_HOST_FRMBUF_TYPE;

	muIOSectorSize		= DEF_IO_SECTOR_SIZE;

	mpBufPoolLock	= new externally_locked<BufferMngr,lockable_type>(\
			*(this->mLock),*(BufferMngr::GetInstance()));
}

IHostBufMngr::~IHostBufMngr(void)
{
	if(NULL != mpBufPoolLock)
	{
		delete mpBufPoolLock;
		mpBufPoolLock = NULL;
	}
}

void IHostBufMngr::DumpIOBufMngr(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));
	mpBufPoolLock->get(guard).GetIoBufPool()->Dump();
}

void IHostBufMngr::DumpSysBufMngr(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));
	mpBufPoolLock->get(guard).GetSysBufPool()->Dump();
}

void IHostBufMngr::DumpFrmkBufMngr(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));
	mpBufPoolLock->get(guard).GetFrmBufPool()->Dump();
}

uint32_t IHostBufMngr::GetIOBufAllocBytes(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));
	return mpBufPoolLock->get(guard).GetIoBufPool()->GetAllocBytes();
}

uint32_t IHostBufMngr::GetSysBufAllocBytes(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufPoolLock->get(guard).GetSysBufPool()->GetAllocBytes();
}

uint32_t IHostBufMngr::GetFrmBufAllocBytes(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return mpBufPoolLock->get(guard).GetFrmBufPool()->GetAllocBytes();
}

void IHostBufMngr::GetIOCmdBufName(char* bufName,uint32_t bufLen,bool fRead,uint32_t nsid,uint32_t bufIdx)
{
	if(true == fRead)
	{
		snprintf(bufName,bufLen,"IORD-L%04xB%08x",nsid,bufIdx);
	}else
	{
		snprintf(bufName,bufLen,"IOWR-L%04xB%08x",nsid,bufIdx);
	}
}

void IHostBufMngr::GetMetaCmdBufName(char* bufName,uint32_t bufLen,bool fRead,uint32_t nsid,uint32_t bufIdx)
{
	if(true == fRead)
	{
		snprintf(bufName,bufLen,"METARD-L%04xB%08x",nsid,bufIdx);
	}else
	{
		snprintf(bufName,bufLen,"METAWR-L%04xB%08x",nsid,bufIdx);
	}
}

void IHostBufMngr::GetSysCmdBufName(char* bufName,uint32_t bufLen,const char* cmdName,uint32_t bufIdx)
{
	snprintf(bufName,bufLen,"%s%08x",cmdName,bufIdx);
}

void IHostBufMngr::GetFrmCmdBufName(char* bufName,uint32_t bufLen,const char* cmdName,uint32_t bufIdx)
{
	snprintf(bufName,bufLen,"%s%08x",cmdName,bufIdx);
}

Buffers* IHostBufMngr::GetIOBufFromPool(BufferMngr& bufPool,const char* bufName,const char* mode)
{
	BaseBufPool* ioBufMngr = bufPool.GetIoBufPool();
	uint32_t sectorSize	= GetSectorSize();
	if(NULL == ioBufMngr || ioBufMngr->NeedReallocated((void*)&sectorSize))
	{
		bufPool.CfgIoBufPool(GetIOBufItemSize(),
				GetIOBufMaxSize(),
				sectorSize);
		ioBufMngr = bufPool.GetIoBufPool();
		ioBufMngr->SetBufType(menIOBufType);
	}

	return ioBufMngr->Get(bufName,0U,mode);
}

Buffers* IHostBufMngr::GetMetaBufFromPool(BufferMngr& bufPool,const char* bufName,const char* mode)
{
	BaseBufPool* bufMngr = bufPool.GetSysBufPool();
	if(NULL == bufMngr)
	{
		bufPool.CfgSysBufPool(GetSysBufMaxSize());
		bufMngr = bufPool.GetSysBufPool();
		bufMngr->SetBufType(menMetaBufType);
	}

	return bufMngr->Get(bufName,GetMetaBufItemSize(),mode);
}

Buffers* IHostBufMngr::GetSysBufFromPool(BufferMngr& bufPool,const char* bufName,uint32_t bufSize,const char* mode)
{
	BaseBufPool* bufMngr	= bufPool.GetSysBufPool();
	if(NULL == bufMngr)
	{
		bufPool.CfgSysBufPool(GetSysBufMaxSize());
		bufMngr	= bufPool.GetSysBufPool();
		bufMngr->SetBufType(menSysBufType);
	}

	return bufMngr->Get(bufName,bufSize,mode);
}

Buffers* IHostBufMngr::GetFrmBufFromPool(BufferMngr& bufPool,const char* bufName,uint64_t bufSize,const char* mode)
{
	BaseBufPool* bufMngr	= bufPool.GetFrmBufPool();
	if(NULL == bufMngr)
	{
		int pid = GetProcessID();
		char bufTag[FRMWK_POOL_NAME_LEN];
		snprintf(bufTag,FRMWK_POOL_NAME_LEN,"%04x%5s",pid,bufName+strlen(bufName)-5);
		bufPool.CfgFrmBufPool(bufTag,GetFrmBufMaxSize());
		bufMngr	= bufPool.GetFrmBufPool();
		bufMngr->SetBufType(menFrmBufType);
	}

	return bufMngr->Get(bufName,(uint32_t)bufSize,mode);
}

bool IHostBufMngr::UpdateFrmBufPool(const char* bufName,uint64_t bufSize,uint32_t bufIdx)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	BufferMngr& bufPool = mpBufPoolLock->get(guard);

	char bufExName[BUFITEM_DESCNAME_LEN] = "";
	GetFrmCmdBufName(bufExName,BUFITEM_DESCNAME_LEN,bufName,bufIdx);

	int pid = GetProcessID();
	char bufTag[FRMWK_POOL_NAME_LEN];
	snprintf(bufTag,FRMWK_POOL_NAME_LEN,"%04x%5s",pid,bufExName+strlen(bufExName)-5);
	bufPool.CfgFrmBufPool(bufTag,bufSize);

	BaseBufPool* bufMngr = bufPool.GetFrmBufPool();
	if(bufMngr != nullptr)
	{
		return true;
	}

	return false;
}

Buffers* IHostBufMngr::GetIOBuffer(BufferMngr& bufPool,bool bRead,uint32_t nsid,uint32_t bufIdx,const char* mode)
{
	char bufName[BUFITEM_DESCNAME_LEN] = "";
	GetIOCmdBufName(bufName,BUFITEM_DESCNAME_LEN,bRead,nsid,bufIdx);

	return GetIOBufFromPool(bufPool,bufName,mode);
}

Buffers* IHostBufMngr::GetMetaBuffer(BufferMngr& bufPool,bool bRead,uint32_t nsid,uint32_t bufIdx,const char* mode)
{
	char bufName[BUFITEM_DESCNAME_LEN] = "";
	GetMetaCmdBufName(bufName,BUFITEM_DESCNAME_LEN,bRead,nsid,bufIdx);

	return GetMetaBufFromPool(bufPool,bufName,mode);
}

Buffers* IHostBufMngr::GetIORdBuf(uint32_t nsid,uint32_t bufIdx,const char* mode)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return GetIOBuffer(mpBufPoolLock->get(guard),true,nsid,bufIdx,mode);
}

Buffers* IHostBufMngr::GetIOWrBuf(uint32_t nsid,uint32_t bufIdx,const char* mode)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return GetIOBuffer(mpBufPoolLock->get(guard),false,nsid,bufIdx,mode);
}

Buffers* IHostBufMngr::GetMetaRdBuf(uint32_t nsid,uint32_t bufIdx,const char* mode)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return GetMetaBuffer(mpBufPoolLock->get(guard),true,nsid,bufIdx,mode);
}

Buffers* IHostBufMngr::GetMetaWrBuf(uint32_t nsid,uint32_t bufIdx,const char* mode)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	return GetMetaBuffer(mpBufPoolLock->get(guard),false,nsid,bufIdx,mode);
}

Buffers* IHostBufMngr::GetSysBuf(const char* bufName,uint32_t bufSize,uint32_t bufIdx,const char* mode)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	char bufExName[BUFITEM_DESCNAME_LEN] = "";
	GetSysCmdBufName(bufExName,BUFITEM_DESCNAME_LEN,bufName,bufIdx);

	return GetSysBufFromPool(mpBufPoolLock->get(guard),bufExName,bufSize,mode);
}

Buffers* IHostBufMngr::GetUsrBuf(const char* bufName,uint32_t bufSize,uint32_t bufIdx,const char* mode)
{
	return GetSysBuf(bufName,bufSize,bufIdx,mode);
}

Buffers* IHostBufMngr::GetFrmwkBuf(const char* bufName,uint32_t bufSize,uint32_t bufIdx,const char* mode)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	char bufExName[BUFITEM_DESCNAME_LEN] = "";
	GetFrmCmdBufName(bufExName,BUFITEM_DESCNAME_LEN,bufName,bufIdx);

	return GetFrmBufFromPool(mpBufPoolLock->get(guard),bufExName,bufSize,mode);
}
