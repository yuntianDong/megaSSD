#ifndef _DNVME_CONTROL_H
#define _DNVME_CONTROL_H

#include "DNVMeIOCtrl.h"
#include "driver/nvme/NVMeCmdReq.h"

class DNVMeControl
{
private:
	int		mDevHdlr;
protected:
	DNVMeControl(int devHdlr)
		:mDevHdlr(devHdlr)
	{
	};
public:
	static DNVMeControl*	gInstance;
	static DNVMeControl*	GetInstance(int devHdlr);

	bool SetIrq(emIntType ty,uint16_t numIrq);
	bool GetIrq(emIntType& ty,uint16_t& numIrq);

	bool SetCtrlrState(emDevState state);
	bool GetDrvVer(uint32_t& drvRev);
	bool GetErrorCode(int& errCode);

	bool PrepareSQCreation(PSCreateIOSQ& cmd);
	bool PrepareCQCreation(PSCreateIOCQ& cmd);
	bool CreateAdminSQ(uint16_t qDepth);
	bool CreateAdminCQ(uint16_t qDepth);

	bool DumpLog(const char* filename);
	bool MarkLog(const char* msgLog);
};

#endif
