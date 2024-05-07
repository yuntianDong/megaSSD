#ifndef _STD_KERNEL_ENGINE_H
#define _STD_KERNEL_ENGINE_H

#include "driver/nvme/NVMeEngine.h"

class StdKernelEngine : public NVMeEngine
{
public:
	StdKernelEngine(int fd)
		:NVMeEngine(fd)
	{};

	NVMeCmdResp* NVMePassthru(NVMeCmdReq &req,void* dBuf,uint32_t dBufLen,void *mBuf,
			uint32_t mBufLen,uint32_t timeout);
	bool NVMeReset(void);
	bool NVMeSubSysReset(void);
	void DumpCmd(void* ctx);
};

#endif
