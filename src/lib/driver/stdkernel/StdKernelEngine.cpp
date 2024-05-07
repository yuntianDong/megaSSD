/*
 * file StdKernelEngine.cpp
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "driver/stdkernel/StdKernelEngine.h"
#include "Logging.h"

NVMeCmdResp* StdKernelEngine::NVMePassthru(NVMeCmdReq &req,void* dBuf,uint32_t dBufLen,void *mBuf,
			uint32_t mBufLen,uint32_t timeout)
{
	int ret = 0;
	struct timeval start;
	struct timeval end;

	NVMeCmdResp*	resp	= new NVMeCmdResp();
	
	nvme_passthru_cmd cmd = {};
	memcpy(&cmd,req.GetCmdAddr(),req.GetCmdSize());

	cmd.metadata	= (uint64_t)mBuf;
	cmd.metadata_len= mBufLen;
	cmd.addr		= (uint64_t)dBuf;
	cmd.data_len	= dBufLen;
	cmd.timeout_ms	= timeout;


	// //debug print
	// if(cmd.opcode == NVME_NVM_OP_WRITE)
	// {
	// 	this->DumpCmd(&cmd);
	// }
	
	gettimeofday(&start,NULL);
	if(true == req.isAdminCmd())
	{
		ret = ioctl(mFd,NVME_IOCTL_ADMIN_CMD,&cmd);
	}
	else
	{
		ret = ioctl(mFd,NVME_IOCTL_IO_CMD,&cmd);
	}
	gettimeofday(&end,NULL);

	if(0 > ret)
	{
		LOGERROR("ioctl Failed, ret = %d, error :%s\n",ret,strerror(errno));
		this->DumpCmd(&cmd);
		return resp;
	}

	resp->SetStatus(ret);
	resp->SetRespVal(cmd.result);
	resp->SetExecTime((end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	return resp;
}

bool StdKernelEngine::NVMeReset(void)
{
	int ret = ioctl(mFd,NVME_IOCTL_RESET,NULL);

	if(ret)
	{
		LOGERROR("ioctl Failed, ret = %d, error :%s\n",ret,strerror(errno));
	}

	return ret >= 0;
}


bool StdKernelEngine::NVMeSubSysReset(void)
{
	int ret = ioctl(mFd,NVME_IOCTL_SUBSYS_RESET,NULL);

	return ret >= 0;
}

void StdKernelEngine::DumpCmd(void* ctx)
{
	nvme_passthru_cmd* cmd = (nvme_passthru_cmd*)ctx;

    LOGINFO("opc\t=0x%x",cmd->opcode);
    LOGINFO("nsid\t=0x%x",cmd->nsid);
    LOGINFO("cdw10\t=0x%x",cmd->cdw10);
    LOGINFO("cdw11\t=0x%x",cmd->cdw11);
    LOGINFO("cdw12\t=0x%x",cmd->cdw12);
    LOGINFO("cdw13\t=0x%x",cmd->cdw13);
    LOGINFO("cdw14\t=0x%x",cmd->cdw14);
    LOGINFO("cdw15\t=0x%x",cmd->cdw15);
	LOGINFO("data buf addr\t=0x%llx",cmd->addr);
	LOGINFO("databuf len\t=0x%x",cmd->data_len);
	LOGINFO("metadata buf addr\t=0x%llx",cmd->metadata);
	LOGINFO("metadata buf len\t=0x%x\n",cmd->metadata_len);

}
