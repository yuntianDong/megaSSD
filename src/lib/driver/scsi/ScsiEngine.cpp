/*
 *	file : SCSIEngine.cpp
 */

//#include <unistd.h>
//#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ScsiEngine.h"
#include "DeviceHandler.h"
#include "Logging.h"

ScsiEngine::ScsiEngine(const char* devName,enIOCTLMode mode)
	:mIOCTLMode(mode)
{
	mDevHdlr = DeviceHandler::GetUFSDevice(devName);
	InitSGIOHdlr();
}

void ScsiEngine::InitSGIOHdlr(void)
{
	memset(&msSGIOHdlr,0,sizeof(stSGIOHdlr));
	memset(mSenseData,0,sizeof(uint8_t)*SENSE_DATA_LEN);
	memset(mCmdCDB,0,sizeof(uint8_t)*MAX_CDB_SIZE);

	msSGIOHdlr.interface_id	= 'S';
	msSGIOHdlr.sbp			= mSenseData;
	msSGIOHdlr.mx_sb_len	= sizeof(uint8_t)*SENSE_DATA_LEN;
}

void ScsiEngine::DumpLastCmdCDB(void)
{
	uint8_t*	pCmd 	= mCmdCDB;
	uint8_t		cmdLen	= msSGIOHdlr.cmd_len;

	LOGINFO("Latest Command CDB Information:");
	for(int idx=0;idx<cmdLen;idx++)
	{
		LOGINFO("CMD[%d]=0x%02x",idx,pCmd[idx]);
	}
}

void ScsiEngine::DumpLastCmdSense(void)
{
	uint8_t* sense	= mSenseData;

	LOGINFO("Latest Command Sense Data:");
	for(int idx=0;idx<SENSE_DATA_LEN;idx++)
	{
		LOGINFO("Sense[%d]=0x%02x",idx,sense[idx]);
	}
}

void ScsiEngine::DumpLastIOCtlParam(void)
{
	LOGINFO("Latest Command SG IO Ioctl Input Parameter:");
	LOGINFO("[i]interface_id\t:%d\t",msSGIOHdlr.interface_id);
	LOGINFO("[i]dxfer_direction\t:%d\t",msSGIOHdlr.dxfer_direction);
	LOGINFO("[i]cmd_len\t:%d\t",msSGIOHdlr.cmd_len);
	LOGINFO("[i]mx_sb_len\t:%d\t",msSGIOHdlr.mx_sb_len);
	LOGINFO("[i]iovec_count\t:%d\t",msSGIOHdlr.iovec_count);
	LOGINFO("[i]dxfer_len\t:%d\t",msSGIOHdlr.dxfer_len);
	LOGINFO("[i]dxferp\t:%lx\t",(uint64_t)msSGIOHdlr.dxferp);
	LOGINFO("[i]cmdp\t:%lx\t",(uint64_t)msSGIOHdlr.cmdp);
	LOGINFO("[i]sbp\t:%lx\t",(uint64_t)msSGIOHdlr.sbp);
	LOGINFO("[i]timeout\t:%d\t",msSGIOHdlr.timeout);
	LOGINFO("[i]flags\t:%d\t",msSGIOHdlr.flags);
	LOGINFO("[i]pack_id\t:%d\t",msSGIOHdlr.pack_id);
	LOGINFO("[i]usr_ptr\t:%lx\t",(uint64_t)msSGIOHdlr.usr_ptr);
	LOGINFO("[o]status\t:%d\t",msSGIOHdlr.status);
	LOGINFO("[o]masked_status\t:%d\t",msSGIOHdlr.masked_status);
	LOGINFO("[o]msg_status\t:%d\t",msSGIOHdlr.msg_status);
	LOGINFO("[o]sb_len_wr\t:%d\t",msSGIOHdlr.sb_len_wr);
	LOGINFO("[o]host_status\t:%d\t",msSGIOHdlr.host_status);
	LOGINFO("[o]driver_status\t:%d\t",msSGIOHdlr.driver_status);
	LOGINFO("[o]resid\t:%d\t",msSGIOHdlr.resid);
	LOGINFO("[o]duration\t:%d\t",msSGIOHdlr.duration);
	LOGINFO("[o]info\t:%d\t",msSGIOHdlr.info);
}

ScsiResponse* ScsiEngine::GetLastCmdResp(void)
{
	stCmdExRespInfo exRespInfo;
	exRespInfo.scsi_status		= msSGIOHdlr.status;
	exRespInfo.masked_status	= msSGIOHdlr.masked_status;
	exRespInfo.msg_status		= msSGIOHdlr.msg_status;
	exRespInfo.host_status		= msSGIOHdlr.host_status;
	exRespInfo.driver_status	= msSGIOHdlr.driver_status;

	ScsiResponse * res = new ScsiResponse();
	res->SetSenseData(mSenseData,msSGIOHdlr.sb_len_wr);
	res->SetDuration(msSGIOHdlr.duration);
	res->SetAuxInfo(msSGIOHdlr.info);
	res->SetDataBuf((uint8_t *)msSGIOHdlr.dxferp,(uint32_t)msSGIOHdlr.dxfer_len);
	res->SetCmdStatus(exRespInfo);

	return res;
}

int ScsiEngine::IssueScsiCmdbySGIO(uint8_t* pCmd,uint8_t cmdSize,uint8_t* buffer,uint32_t size,enCmdDirect xferDirect,uint32_t timeout)
{
	InitSGIOHdlr();

	msSGIOHdlr.cmdp				= pCmd;
	msSGIOHdlr.cmd_len			= cmdSize;

	msSGIOHdlr.timeout			= timeout;
	msSGIOHdlr.dxfer_direction	= (int)xferDirect;

	msSGIOHdlr.dxferp			= (void *)buffer;
	msSGIOHdlr.dxfer_len		= size;
	memcpy(mCmdCDB,pCmd,cmdSize);
	return ioctl(GetDevHdlr(),SG_IO,&msSGIOHdlr);
}

ScsiResponse* ScsiEngine::IssueCmdWithNonData(ScsiCommand & cmd,uint32_t timeout)
{
	if(true == this->isSGIOMode())
	{
		int rc = IssueScsiCmdbySGIO(cmd.GetCDB(),cmd.GetCDBSize(),NULL,0,SGIO_DATA_NONE,timeout);
		if(0 > rc)
		{
			LOGERROR("ERROR: fail to call ioctl() , return code : %d\n",rc);
			return new ScsiResponse();
		}
		return GetLastCmdResp();
	}

	return NULL;
}

ScsiResponse* ScsiEngine::IssueCmdWithDataIn(ScsiCommand & cmd,uint8_t* buffer,uint32_t size,uint32_t timeout)
{
	if(true == this->isSGIOMode())
	{
		int rc = IssueScsiCmdbySGIO(cmd.GetCDB(),cmd.GetCDBSize(),buffer,size,SGIO_DATA_IN,timeout);
		if(0 > rc)
		{
			LOGERROR("ERROR: fail to call ioctl() , return code : %d\n",rc);
			return new ScsiResponse();
		}
		return GetLastCmdResp();
	}

	return NULL;
}

ScsiResponse* ScsiEngine::IssueCmdWithDataOut(ScsiCommand & cmd,uint8_t* buffer,uint32_t size,uint32_t timeout)
{
	if(true == this->isSGIOMode())
	{
		int rc = IssueScsiCmdbySGIO(cmd.GetCDB(),cmd.GetCDBSize(),buffer,size,SGIO_DATA_OUT,timeout);
		if(0 > rc)
		{
			LOGERROR("ERROR: fail to call ioctl() , return code : %d\n",rc);
			return new ScsiResponse();
		}
		return GetLastCmdResp();
	}

	return NULL;
}
