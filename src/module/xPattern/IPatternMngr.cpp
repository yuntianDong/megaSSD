/*
 * file : IPatternMngr.cpp
 */

#include "IPatternMngr.h"
#include "utility/CRC.h"


IPatternMngr::IPatternMngr(stPatRecType type,bool bAddrOL,IHostBufMngr* hostBufMngr,ParentContext* parentContext):patType(type),bAddrOL(bAddrOL)
{
	mHostBufMngrLock	= new externally_locked<IHostBufMngr,lockable_type>(*(this->mLock),*(hostBufMngr));
	mParentContextLock	= new externally_locked<ParentContext,lockable_type>(*(this->mLock),*(parentContext));
	bitMapBufName[0]	= '\0';
	snprintf(bitMapBufName,BUFITEM_DESCNAME_LEN,"%s%08x","PATREC",NS_ID_DEFAULT);

	if(type == PATREC_BITMAP)
	{
		bitMapBufSize = this->CalBitMapBufSize(parentContext);
		hostBufMngr->UpdateFrmBufPool(bitMapBufName,bitMapBufSize,NS_ID_DEFAULT);
		Buffers* bitMapBuf = hostBufMngr->GetFrmwkBuf(bitMapBufName,bitMapBufSize,NS_ID_DEFAULT);
		mpPatMngr = (PatternMngr*)new PatternMngr(type,bitMapBuf,bAddrOL);
	}
	else if(type == PATREC_CRC8)
	{
		//TODO
		LOGWARN("PATREC_CRC8 Not Support!");
	}
	else
	{
		bitMapBufSize = this->CalBitMapBufSize(parentContext);
		hostBufMngr->UpdateFrmBufPool(bitMapBufName,bitMapBufSize,NS_ID_DEFAULT);
		Buffers* bitMapBuf = hostBufMngr->GetFrmwkBuf(bitMapBufName,bitMapBufSize,NS_ID_DEFAULT);
		mpPatMngr = (PatternMngr*)new PatternMngr(type,bitMapBuf,bAddrOL);
	}
}

uint64_t IPatternMngr::CalBitMapBufSize(ParentContext* parentContext)
{
	uint64_t totalCap = 0;

	ParentParameter* parentParam = parentContext->GetParentParameter();
	
	// for(const auto& pair : parentParam->NsMap)
	// {
	// 	totalCap += pair.second.nsCap;
	// }

	totalCap = parentParam->totalNvmSize / 512;
	totalCap = ((int(totalCap / 2) + 31) >> 5) << 5;
	return totalCap;
}

IPatternMngr::~IPatternMngr(void)
{
	del_obj(mpPatMngr);
// 	del_obj(mHostBufMngrLock);
// 	del_obj(mParentContextLock);
}

bool IPatternMngr::UptPatRecFromIOWrBuf(Buffers* ioWrBuf,uint32_t bufOffset,uint32_t nsid,uint64_t bitMapOffset,uint16_t length)
{
	if(MAX_NS_NUM <= nsid || NULL == ioWrBuf || bufOffset >= ioWrBuf->GetBufSize())
	{
		LOGERROR("wrong Parameter!");
		return false;
	}

	if(NULL == mpPatMngr)
	{
		return false;
	}

	return mpPatMngr->RecordFromWrBuf(ioWrBuf,bufOffset,bitMapOffset,length);
}

bool IPatternMngr::CompareWithIORdBuf(Buffers* ioWrBuf,uint32_t bufOffset,\
		uint32_t nsid,uint64_t bitMapOffset,uint16_t length,std::string bufmode)
{
	if(MAX_NS_NUM <= nsid || NULL == ioWrBuf || bufOffset >= ioWrBuf->GetBufSize())
	{
		LOGERROR("wrong Parameter!");
		return false;
	}

	if(NULL == mpPatMngr)
	{
		return false;
	}
	// LOGDEBUG("Dump bitMap of data");
	// this->DumpBitmapBuf(nsid,bitMapOffset,length);

	return mpPatMngr->CompareWithRdBuf(ioWrBuf,bufOffset,bitMapOffset,length,bufmode);
}

bool IPatternMngr::FillIOWrBufFromPatRec(Buffers* cmpBuf,uint32_t nsid,uint32_t bufOffset,uint64_t slba,uint16_t length)
{
	if(MAX_NS_NUM <= nsid || NULL == cmpBuf || bufOffset >= cmpBuf->GetBufSize())
	{
		LOGERROR("wrong Parameter!");
		return false;
	}

	if(NULL == mpPatMngr)
	{
		return false;
	}

	strict_lock<lockable_type> guard(*(this->mLock));

	ParentContext *parentContext = &mParentContextLock->get(guard);
	std::map<uint32_t, NsMapInfo> *nsmap = &parentContext->GetParentParameter()->NsMap;

	return mpPatMngr->FillPattern(cmpBuf,bufOffset,nsmap->at(nsid).offset+slba,length);
}

void IPatternMngr::DumpBitmapBuf(uint32_t nsid,uint32_t offset,uint32_t length)
{
	if(MAX_NS_NUM <= nsid)
	{
		LOGERROR("nsid is out of boundary!");
		return;
	}

	if(NULL == mpPatMngr)
	{
		LOGERROR("PatMngr is nullptr, please check!");
		return;
	}

	strict_lock<lockable_type> guard(*(this->mLock));

	ParentContext *parentContext = &mParentContextLock->get(guard);
	std::map<uint32_t, NsMapInfo> *nsmap = &parentContext->GetParentParameter()->NsMap;
	//calculate bitmap offset postion
	if((offset % (NBITS_PER_BYTE/PAT_IDX_BITS)) != 0)
	{
		LOGINFO("Given offset is pointed to next 4 bit at beginning");
	}
	offset = int((offset + nsmap->at(nsid).offset)/ (NBITS_PER_BYTE/PAT_IDX_BITS));
	length = int(length / (NBITS_PER_BYTE/PAT_IDX_BITS));
	
	mpPatMngr->DebugDump(offset,length);
}

bool IPatternMngr::ReleaseAllPattern(void)
{
	strict_lock<lockable_type> guard(*(this->mLock));
	snprintf(bitMapBufName,BUFITEM_DESCNAME_LEN,"%s%08x","PATREC",NS_ID_DEFAULT);

	del_obj(mpPatMngr)

	if(patType != PATREC_BITMAP)
	{
		LOGWARN("Invalid PATREC type,cur ver only support PATREC_BITMAP");
		return false;
	}
	ParentContext *parentContext = &mParentContextLock->get(guard);
	IHostBufMngr *hostBufMngr = &mHostBufMngrLock->get(guard);

	bitMapBufSize = this->CalBitMapBufSize(parentContext);
	hostBufMngr->UpdateFrmBufPool(bitMapBufName,bitMapBufSize,NS_ID_DEFAULT);
	Buffers* bitMapBuf = hostBufMngr->GetFrmwkBuf(bitMapBufName,bitMapBufSize,NS_ID_DEFAULT);
	if(NULL == bitMapBuf)
	{
		LOGERROR("GetFrmwkBuf failed!");
		return false;
	}
	mpPatMngr = (PatternMngr*)new PatternMngr(patType,bitMapBuf,bAddrOL);
	return true;

}

bool IPatternMngr::DeletePattern(uint32_t nsid,uint32_t slba,uint64_t length)
{
	strict_lock<lockable_type> guard(*(this->mLock));
	ParentContext *parentContext = &mParentContextLock->get(guard);
	uint64_t offset = parentContext->GetParentParameter()->NsMap.at(nsid).offset + slba;

	return mpPatMngr->DeletePattern(offset,length);
}

/**
 * CRC APIs
*/
uint16_t IPatternMngr::CalcNVMe16BCRC(Buffers* buf,uint32_t offset,uint32_t length)
{
	return CalCRC16NVM16BCRC(buf->GetBufferPoint() + offset,length);
}

uint32_t IPatternMngr::CalcNVMe32BCRC(Buffers* buf,uint32_t offset,uint32_t length)
{
	return CalCRC32NVM32BCRC(buf->GetBufferPoint() + offset,length);
}

uint64_t IPatternMngr::CalcNVMe64BCRC(Buffers* buf,uint32_t offset,uint32_t length)
{
	return CalCRC64NVM64BCRC(buf->GetBufferPoint() + offset,length);
}