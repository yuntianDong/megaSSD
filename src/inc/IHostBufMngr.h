#ifndef _I_HOST_BUFMNGR_H
#define _I_HOST_BUFMNGR_H

#include "buffer/BufferMngr.h"
#include "IBasicLock.h"
#include "utility/Misc.h"

#ifndef DEF_CMD_BUFIDX
#define DEF_CMD_BUFIDX		0
#endif

#ifndef DEF_CMD_NSID
#define DEF_CMD_NSID		0xFFFFFFFF
#endif

#ifndef DEF_SECTOR_SIZE
#define DEF_SECTOR_SIZE		4096
#endif

/*
 * Default Buffer Parameter
 * 	IO: 512MB	SYS: 256MB	FRM: 256MB
 * 	TOTAL: 1GB
 */
#define DEF_HOST_IOBUF_ITEMSIZE			1*1024*1024
#define DEF_HOST_IOBUF_ALLOCMAX			512*1024*1024
#define DEF_HOST_METABUF_ITEMSIZE		4*1024
#define DEF_HOST_SYSBUF_ALLOCMAX		256*1024*1024
#define DEF_HOST_FRMBUF_ALLOCMAX		256*1024*1024

#define DEF_HOST_IOBUF_TYPE				BUF_T_PATIO
#define DEF_HOST_METABUF_TYPE			BUF_T_SYS
#define DEF_HOST_SYSBUF_TYPE			BUF_T_SYS
#define DEF_HOST_FRMBUF_TYPE			BUF_T_PAT_REC

#define DEF_IO_SECTOR_SIZE				4*1024

class IHostBufMngr : public IBasicLock
{
private:
	uint32_t		muIOBufItemSize;
	uint32_t		muIOBufMaxSize;
	uint32_t		muMetaBufItemSize;
	uint32_t		muSysBufMaxSize;
	uint32_t		muFrmBufMaxSize;

	enBufType		menIOBufType;
	enBufType		menMetaBufType;
	enBufType		menSysBufType;
	enBufType		menFrmBufType;

	uint32_t		muIOSectorSize;
protected:
	externally_locked<BufferMngr,lockable_type>		*mpBufPoolLock;

	void GetIOCmdBufName(char* bufName,uint32_t bufLen,bool fRead,uint32_t nsid,uint32_t bufIdx);
	void GetMetaCmdBufName(char* bufName,uint32_t bufLen,bool fRead,uint32_t nsid,uint32_t bufIdx);
	void GetSysCmdBufName(char* bufName,uint32_t bufLen,const char* cmdName,uint32_t bufIdx);
	void GetFrmCmdBufName(char* bufName,uint32_t bufLen,const char* cmdName,uint32_t bufIdx);

	Buffers* GetIOBufFromPool(BufferMngr& bufPool,const char* bufName,const char* mode);
	Buffers* GetMetaBufFromPool(BufferMngr& bufPool,const char* bufName,const char* mode);
	Buffers* GetSysBufFromPool(BufferMngr& bufPool,const char* bufName,uint32_t bufSize,const char* mode);
	Buffers* GetFrmBufFromPool(BufferMngr& bufPool,const char* bufName,uint64_t bufSize,const char* mode);

	Buffers* GetIOBuffer(BufferMngr& bufPool,bool bRead,uint32_t nsid,uint32_t bufIdx,const char* mode);
	Buffers* GetMetaBuffer(BufferMngr& bufPool,bool bRead,uint32_t nsid,uint32_t bufIdx,const char* mode);

public:
	static IHostBufMngr* gInstance;
	static IHostBufMngr* GetInstance(void);

	IHostBufMngr(void);
	virtual ~IHostBufMngr(void);

	void SetSectorSize(uint32_t sectorSize) {muIOSectorSize=sectorSize;};
	uint32_t GetSectorSize(void) {return muIOSectorSize;};

	bool SetIOBufType(enBufType ty)
	{
		if(BUF_T_IO != ty && BUF_T_PATIO != ty)
		{
			return false;
		}

		menIOBufType = ty;
		return true;
	};
	bool SetMetaBufType(enBufType ty)
	{
		menMetaBufType = ty;
		return true;
	};
	bool SetSysBufType(enBufType ty)
	{
		menSysBufType = ty;
		return true;
	};
	bool SetFrmBufType(enBufType ty)
	{
		UNUSED(ty);
		return false;
	};

	enBufType GetIOBufType(void){return menIOBufType;};
	enBufType GetMetaBufType(void){return menMetaBufType;};
	enBufType GetSysBufType(void){return menSysBufType;};
	enBufType GetFrmBufType(void){return menFrmBufType;};

	void SetIOBufItemSize(uint32_t bufSize) {muIOBufItemSize = bufSize;};
	void SetIOBufMaxSize(uint32_t bufSize) {muIOBufMaxSize = bufSize;};
	void SetMetaBufItemSize(uint32_t bufSize) {muMetaBufItemSize = bufSize;};
	void SetSysBufMaxSize(uint32_t bufSize) {muSysBufMaxSize = bufSize;};
	void SetFrmBufMaxSize(uint32_t bufSize) {muFrmBufMaxSize = bufSize;};

	uint32_t GetIOBufItemSize(void) {return muIOBufItemSize;};
	uint32_t GetIOBufMaxSize(void) {return muIOBufMaxSize;};
	uint32_t GetMetaBufItemSize(void) {return muMetaBufItemSize;};
	uint32_t GetSysBufMaxSize(void) {return muSysBufMaxSize;};
	uint32_t GetFrmBufMaxSize(void) {return muFrmBufMaxSize;};

	void DumpIOBufMngr(void);
	void DumpSysBufMngr(void);
	void DumpFrmkBufMngr(void);

	bool UpdateFrmBufPool(const char* bufName,uint64_t bufSize,uint32_t bufIdx);

	uint32_t GetIOBufAllocBytes(void);
	uint32_t GetSysBufAllocBytes(void);
	uint32_t GetFrmBufAllocBytes(void);

	Buffers* GetIORdBuf(uint32_t nsid=DEF_CMD_NSID,uint32_t bufIdx=DEF_CMD_BUFIDX,const char* mode="nvme");
	Buffers* GetIOWrBuf(uint32_t nsid=DEF_CMD_NSID,uint32_t bufIdx=DEF_CMD_BUFIDX,const char* mode="nvme");
	Buffers* GetMetaRdBuf(uint32_t nsid=DEF_CMD_NSID,uint32_t bufIdx=DEF_CMD_BUFIDX,const char* mode="nvme");
	Buffers* GetMetaWrBuf(uint32_t nsid=DEF_CMD_NSID,uint32_t bufIdx=DEF_CMD_BUFIDX,const char* mode="nvme");
	Buffers* GetSysBuf(const char* bufName,uint32_t bufSize=0,uint32_t bufIdx=DEF_CMD_BUFIDX,const char* mode="nvme");
	Buffers* GetUsrBuf(const char* bufName,uint32_t bufSize=0,uint32_t bufIdx=DEF_CMD_BUFIDX,const char* mode="nvme");
	Buffers* GetFrmwkBuf(const char* bufName,uint32_t bufSize=0,uint32_t bufIdx=DEF_CMD_BUFIDX,const char* mode="nvme");
};

#endif
