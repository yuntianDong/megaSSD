#ifndef _NVME_ENGINE_H
#define _NVME_ENGINE_H

#include "Commons.h"
#include "linux/nvme_ioctl.h"
#include "NVMeCmdReq.h"
#include "NVMeCmdResp.h"

class NVMeEngine
{
protected:
	int mFd;
	struct spdk_nvme_ctrlr *mCtrlr;
public:
	NVMeEngine(int fd)
		:mFd(fd),mCtrlr(nullptr)
	{};
	NVMeEngine(spdk_nvme_ctrlr *ctrlr):mCtrlr(ctrlr),mFd(-1)
	{};

	virtual NVMeCmdResp* NVMePassthru(NVMeCmdReq &req,void* dBuf,uint32_t dBufLen,void *mBuf,
			uint32_t mBufLen,uint32_t timeout)=0;
	virtual bool NVMeReset(void)=0;
	virtual bool NVMeSubSysReset(void)=0;
	virtual void DumpCmd(void* ctx)=0;
};

#endif
