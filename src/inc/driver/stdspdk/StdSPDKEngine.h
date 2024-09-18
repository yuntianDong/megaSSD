#ifndef _STD_SPDK_ENGINE_H
#define _STD_SPDK_ENGINE_H

#include "driver/nvme/NVMeEngine.h"

static int outstanding_cmds;
class StdSPDKEngine : public NVMeEngine
{
public:
	StdSPDKEngine(spdk_nvme_ctrlr *ctrlr)
		: NVMeEngine(ctrlr) {outstanding_cmds = 0;};

	NVMeCmdResp *NVMePassthru(NVMeCmdReq &req, void *dBuf, uint32_t dBufLen, void *mBuf,
							  uint32_t mBufLen, uint32_t timeout);
	bool NVMeReset(void);
	bool NVMeSubSysReset(void);
	void DumpCmd(void *ctx);
	static void CmdCompletion(void *ctx, const struct spdk_nvme_cpl *cpl);
};
#endif