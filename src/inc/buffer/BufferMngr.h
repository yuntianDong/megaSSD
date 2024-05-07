#ifndef _BUFFER_MNGR_H
#define _BUFFER_MNGR_H

#include "BufferPool.h"

#define DEFAULT_BUF_ITEMSIZE		512
#define FRMWK_POOL_NAME_LEN			10

class IoBufPool : public RingBufPool
{
private:
	uint32_t	muSectorSize;

protected:
	virtual Buffers* CreateBufItem(uint64_t bufSize,const char* mode);

public:
	IoBufPool(uint32_t bufItemSize,uint32_t bufItemMax,uint32_t sectorSize);
	virtual bool NeedReallocated(void* args=NULL);
};

class SysBufPool : public LimSizeBufPool
{
public:
	SysBufPool(uint32_t maxBufPoolSize);
};

class FrmBufPool : public MaxSizeBufPool
{
private:
	char mBufTag[FRMWK_POOL_NAME_LEN];

protected:
	virtual Buffers* CreateBufItem(uint32_t bufSize,const char* mode);

public:
	FrmBufPool(const char* bufTag,uint32_t maxBufPoolSize);
};

/*
 *  BufferMngr
 *  	IOBufPool	: RingBufPool
 *  	SysBufPool	: LimSizeBufPool
 *  	FrmBufPool	: MaxSizeBufPool
 */
class BufferMngr
{
private:
	BaseBufPool*	mpIoBufPool;
	BaseBufPool*	mpSecuBufPool;
	BaseBufPool*	mpSysBufPool;
	BaseBufPool*	mpFrmBufPool;

protected:
	BufferMngr(void);
public:
	static BufferMngr* gInstance;
	static BufferMngr* GetInstance(void);

	virtual ~BufferMngr();

	void CfgIoBufPool(uint32_t bufItemSize,uint32_t bufItemMax,uint32_t sectorSize);
	void CfgSecuBufPool(uint32_t bufItemSize,uint32_t bufItemMax,uint32_t sectorSize);
	void CfgSysBufPool(uint32_t maxBufPoolSize);
	void CfgFrmBufPool(const char* bufTag,uint32_t maxBufPoolSize);

	BaseBufPool* GetIoBufPool(void) {return mpIoBufPool;};
	BaseBufPool* GetSecuBufPool(void) {return mpSecuBufPool;};
	BaseBufPool* GetSysBufPool(void) {return mpSysBufPool;};
	BaseBufPool* GetFrmBufPool(void) {return mpFrmBufPool;};
};

#endif
