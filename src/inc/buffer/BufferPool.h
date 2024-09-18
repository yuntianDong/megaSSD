#ifndef _BUFFER_POOL_H
#define _BUFFER_POOL_H

#include "buffer/HostBuffer.h"

#define BUFITEM_DESCNAME_LEN		33
#define BUFPOOL_ITEM_MAX			1024

#ifndef UNUSED
#define UNUSED(expr) do { (void)(expr); } while (0)
#endif

#pragma pack(push)
#pragma pack(1)
typedef struct _stBufItemDesc
{
	char		cBufName[BUFITEM_DESCNAME_LEN];
	Buffers* 	pBuf;
	uint64_t	uBufSize;
}stBufItemDesc;
#pragma pack(pop)

typedef enum _enBufType
{
	BUF_T_UNK,
	BUF_T_SYS,
	BUF_T_IO,
	BUF_T_PATIO,
	BUF_T_PAT_REC,
}enBufType;

class BaseBufPool
{
private:
	stBufItemDesc*	mpBufPool[BUFPOOL_ITEM_MAX];
	uint64_t		muBufAllocSize;
	bool			mbCreateNewBufIfNotExist;
	bool			mbCreateNewBufIfLargerSize;

	stBufItemDesc* GetBufItemDesc(const char *bufName);
	stBufItemDesc* GetBufItemDesc(uint32_t index=-1);
	void SetBufItemDesc(const char *bufName,Buffers* pBuf,uint64_t bufSize);
	void SetBufItemDesc(stBufItemDesc* desc,const char *bufName,Buffers* pBuf,uint64_t bufSize);
	void ClrBufItemDesc(uint32_t index=-1);
	void ClrBufItemDesc(stBufItemDesc* desc);
	void UptBufItemDesc(const char *bufName,uint64_t bufSize);

protected:
	enBufType		menBufType;
	uint32_t		muBufHeader;
	uint32_t		muBufTailer;

	virtual Buffers* CreateBufItem(uint64_t bufSize,const char* mode);

	virtual bool Advance(uint32_t &Header);		// muBufHeader go ahead
	virtual bool Follow(uint32_t &Tailer);		// muBufTailer go ahead
	virtual bool NeedRelease(void);
	virtual uint32_t GetAllocBufSize(void){return 0;};
	void Release(uint32_t index=(uint32_t)-1);
	void DisableCreateNewBufIfLargerSizeFlag(void) {mbCreateNewBufIfLargerSize = false;}

public:
	BaseBufPool(enBufType bufType=BUF_T_UNK,bool bNewBufIfNotExist=true);
	virtual ~BaseBufPool();

	Buffers* Alloc(const char *bufName,uint64_t bufSize=0,const char* mode="nvme");
	Buffers* Get(const char* bufName,uint64_t bufSize=0,const char* mode="nvme");
	uint32_t GetAllocBytes(void){return muBufAllocSize;};
	void SetBufType(enBufType bufType=BUF_T_UNK) {menBufType = bufType;};

	virtual bool NeedReallocated(void* args=NULL) {UNUSED(args);return false;};

	void Dump(void);
};

/*
 *  RingBufPool
 *  	The size of buffer item is fixed,and the max buffer item count is limited.
 *  	If user request new buffer when all available buffer slot has been used, the
 *  	oldest buffer slot will be released and the buffer of this slot will be reused.
 */
class RingBufPool : public BaseBufPool
{
private:
	uint32_t	muBufItemSize;
	uint32_t	muBufItemMax;

protected:
	virtual bool Advance(uint32_t &Header);

	virtual Buffers* CreateBufItem(uint64_t bufSize,const char* mode);
	uint32_t GetBufItemSize(void){return muBufItemSize;};
	uint32_t GetBufItemMax(void){return muBufItemMax;};

	uint32_t GetAllocBufSize(void){return GetBufItemSize();};

public:
	RingBufPool(uint32_t bufItemSize,uint32_t bufItemMax,
			enBufType bufType=BUF_T_UNK,bool bNewBufIfNotExist=true);
};

/*
 * 	LimSizeBufPool
 * 		Buffer pool max size is limited. If the size of the buffer user request is larger
 *		than the free buffer size, it will free oldest buffer until free buffer size is
 *		enough to allocate the new buffer.
 */
class LimSizeBufPool : public BaseBufPool
{
private:
	uint32_t	muMaxBufPoolSize;

protected:
	virtual bool Advance(uint32_t &Header);
	virtual bool Follow(uint32_t &Tailer);
	virtual bool NeedRelease(void);

public:
	LimSizeBufPool(uint32_t maxBufPoolSize,enBufType bufType=BUF_T_UNK,bool bNewBufIfNotExist=true);
};

/*
 *	MaxSizeBufPool
 *		Buffer pool max size is limited. If the size of the buffer user request is larger
 *		than the free buffer size, the operation will be failed.
 *		No buffer free operation for it.
 */
class MaxSizeBufPool : public BaseBufPool
{
private:
	uint32_t	muMaxBufPoolSize;

protected:
	virtual bool Advance(uint32_t &Header);

public:
	MaxSizeBufPool(uint32_t maxBufPoolSize,enBufType bufType=BUF_T_UNK,bool bNewBufIfNotExist=true);
};

#endif
