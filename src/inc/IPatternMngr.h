#ifndef _I_PATTERN_MNGR_H
#define _I_PATTERN_MNGR_H

#include "pattern/PatternMngr.h"
#include "pattern/PatternBitmap.h"
#include "IHostBufMngr.h"
#include "context/Context.h"
#include "IBasicLock.h"

#ifndef MAX_NS_NUM
#define MAX_NS_NUM			1024
#endif

#ifndef DEF_CMD_NSID
#define DEF_CMD_NSID		0xFFFFFFFF
#endif

class IPatternMngr : public IBasicLock
{
private:
	uint64_t bitMapBufSize;
	PatternMngr* mpPatMngr;
	char bitMapBufName[BUFITEM_DESCNAME_LEN];
	stPatRecType patType;
	bool bAddrOL;

	uint64_t CalBitMapBufSize(ParentContext* parentContext);
protected:
	externally_locked<IHostBufMngr,lockable_type>	*mHostBufMngrLock;
	externally_locked<ParentContext,lockable_type>	*mParentContextLock;

public:

	IPatternMngr(stPatRecType type,bool bAddrOL,IHostBufMngr* hostBufMngr,ParentContext* parentContext);
	virtual ~IPatternMngr();

	bool UptPatRecFromIOWrBuf(Buffers* ioWrBuf,uint32_t bufOffset,uint32_t nsid,uint64_t bitMapOffset,uint16_t length);
	bool CompareWithIORdBuf(Buffers* ioWrBuf,uint32_t bufOffset,uint32_t nsid,uint64_t bitMapOffset,uint16_t length,std::string bufmode);
	bool FillIOWrBufFromPatRec(Buffers* cmpBuf,uint32_t nsid,uint32_t bufOffset,uint64_t slba,uint16_t length);


	void DumpBitmapBuf(uint32_t nsid,uint32_t offset,uint32_t length);
	bool ReleaseAllPattern(void);
	bool DeletePattern(uint32_t nsid,uint32_t slba,uint64_t length);
	bool IsAddrOverlay(void){return bAddrOL;};

	/*
	 * PatternBitmap
	 */
	uint32_t GetPatternByIndex(uint8_t index){return PatternBitmap::GetPatternByIndex(index);};
	uint8_t GetIndexByPattern(uint32_t pattern){return PatternBitmap::GetIndexByPattern(pattern);};
	bool IsValidIndex(uint8_t index){return PatternBitmap::IsValidIndex(index);};
	bool SetIdxPattern(uint8_t index,uint32_t pattern){return PatternBitmap::SetIdxPattern(index,pattern);};

	/**
	 * CRC APIs
	*/
	uint16_t	CalcNVMe16BCRC(Buffers* buf,uint32_t offset,uint32_t length);
	uint32_t	CalcNVMe32BCRC(Buffers* buf,uint32_t offset,uint32_t length);
	uint64_t	CalcNVMe64BCRC(Buffers* buf,uint32_t offset,uint32_t length);
};

#endif
