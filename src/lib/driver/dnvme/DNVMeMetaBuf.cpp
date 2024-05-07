/*
 * file : DNVMeMetaBuf.cpp
 */

#include "driver/dnvme/DNVMeMetaBuf.h"
#include "driver/dnvme/DNVMeUtil.h"

DNVMeMetaBuf*	DNVMeMetaBuf::gInstance	= NULL;

DNVMeMetaBuf*	DNVMeMetaBuf::GetInstance(int devHdlr)
{
	if(NULL != gInstance)
	{
		return new DNVMeMetaBuf(devHdlr);
	}

	return gInstance;
}

DNVMeMetaBuf::DNVMeMetaBuf(int devHdlr)
	:mDevHdlr(devHdlr)
{
	mpDNVMeIOCtrl	= DNVMeIOCtrl::GetInstance(devHdlr);

	mMetaBufSize	= INVALID_META_BUFSIZE;
	mCurAllocMetaID = 0;
}

DNVMeMetaBuf::~DNVMeMetaBuf(void)
{
	ReleaseAllMetaBuf();
}

void DNVMeMetaBuf::ReleaseAllMetaBuf(void)
{
	for(map<uint32_t,Buffers*>::iterator iter = mAllocMetaBufLst.begin();iter!=mAllocMetaBufLst.end();iter++)
	{
		uint16_t id	 = iter->first;
		Buffers* buf = iter->second;

		// munmap buf
		DNVMeUtil::munmap(buf->GetBufferPoint(),buf->GetBufSize());

		mpDNVMeIOCtrl->IOCtlMetaBufDelete(id);
		delete buf;buf=NULL;
	}
}

bool DNVMeMetaBuf::SetMetaBufSize(uint32_t metaSize)
{
	if(0 != metaSize % sizeof(uint32_t))
	{
		return false;
	}

	if(0 < mAllocMetaBufLst.size())
	{
		ReleaseAllMetaBuf();
	}

	int rtn = mpDNVMeIOCtrl->IOCtlMetaBufCreate(metaSize);

	if(0 != rtn)
	{
		return false;
	}

	mMetaBufSize = metaSize;

	return true;
}

Buffers*	DNVMeMetaBuf::GetMetaBuf(uint32_t&	metaId)
{
	if(INVALID_META_BUFSIZE == mMetaBufSize)
	{
		SetMetaBufSize(DEF_META_BUFSIZE);
	}

	metaId	= mCurAllocMetaID++;
	if(METABUF_ID_MAX <= mCurAllocMetaID)
	{
		mCurAllocMetaID = 0;
	}

	map<uint32_t,Buffers*>::iterator iter = mAllocMetaBufLst.find(metaId);
	if(iter != mAllocMetaBufLst.end())
	{
		return iter->second;
	}

	int rtn = mpDNVMeIOCtrl->IOCtlMetaBufAlloc(metaId);
	if(0 != rtn)
	{
		return NULL;
	}

	//nmap
	uint8_t* mem = DNVMeUtil::mmap(GetDevHdlr(),GetMetaBufSize(),metaId,DNVMeUtil::MMR_META);
	if(NULL == mem)
	{
		mpDNVMeIOCtrl->IOCtlMetaBufDelete(metaId);
		return NULL;
	}
	Buffers* buf = new Buffers(mem,GetMetaBufSize(),DEF_META_BUFALIGN);
	mAllocMetaBufLst[metaId]	= buf;

	return buf;
}
