/*
 * file : DNVMeControl.cpp
 */

#include "driver/dnvme/DNVMeControl.h"

DNVMeControl*	DNVMeControl::gInstance	= NULL;

DNVMeControl*	DNVMeControl::GetInstance(int devHdlr)
{
	if(NULL == gInstance)
	{
		gInstance	= new DNVMeControl(devHdlr);
	}

	return gInstance;
}

bool DNVMeControl::SetIrq(emIntType ty,uint16_t numIrq)
{
	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlSetIRQ(ty,numIrq);

	return IOCTL_CALL_SUCCESS == rtn;
}

bool DNVMeControl::GetIrq(emIntType& ty,uint16_t& numIrq)
{
	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlGetDeviceMetrics(ty,numIrq);

	return IOCTL_CALL_SUCCESS == rtn;
}

bool DNVMeControl::SetCtrlrState(emDevState state)
{
	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlDeviceState(state);

	return IOCTL_CALL_SUCCESS == rtn;
}

bool DNVMeControl::GetDrvVer(uint32_t& drvRev)
{
	uint32_t apiRev = 0;
	drvRev = 0;

	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlGetDriverMetrics(drvRev,apiRev);

	return IOCTL_CALL_SUCCESS == rtn;
}

bool DNVMeControl::GetErrorCode(int& errCode)
{
	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlErrCheck(errCode);

	return IOCTL_CALL_SUCCESS == rtn;
}

bool DNVMeControl::PrepareSQCreation(PSCreateIOSQ& cmd)
{
	uint16_t sqId	= cmd.GetQID();
	uint16_t cqId	= cmd.GetCQID();
	uint32_t elements	= cmd.GetQSize();
	bool contigBuf	= cmd.GetPCFlag();

	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlPrepareSQCreation(sqId,cqId,elements,contigBuf);

	return IOCTL_CALL_SUCCESS == rtn;
}

bool DNVMeControl::PrepareCQCreation(PSCreateIOCQ& cmd)
{
	uint16_t cqId	= cmd.GetQID();
	uint32_t elements	= cmd.GetQSize();
	bool contigBuf	= cmd.GetPCFlag();

	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlPrepareCQCreation(cqId,elements,contigBuf);

	return IOCTL_CALL_SUCCESS == rtn;
}

bool DNVMeControl::CreateAdminSQ(uint16_t qDepth)
{
	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlCreateAdminQ(NVME_Q_SQ,qDepth);

	return IOCTL_CALL_SUCCESS == rtn;
}

bool DNVMeControl::CreateAdminCQ(uint16_t qDepth)
{
	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlCreateAdminQ(NVME_Q_CQ,qDepth);

	return IOCTL_CALL_SUCCESS == rtn;
}

bool DNVMeControl::DumpLog(const char* filename)
{
	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlDumpMetrics(filename,strlen(filename)+1);

	return IOCTL_CALL_SUCCESS == rtn;
}

bool DNVMeControl::MarkLog(const char* msgLog)
{
	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlMarkSysLog(msgLog,strlen(msgLog)+1);

	return IOCTL_CALL_SUCCESS == rtn;
}


