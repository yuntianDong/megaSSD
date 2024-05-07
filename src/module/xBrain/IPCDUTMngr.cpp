/*
 * file : IPCDUTMngr.cpp
 */

#include <stdlib.h>

#include "IPCDUTMngr.h"
#include "Logging.h"

IPCDUTMngrServer*	GetDutMngrServer(const char* devname,uint8_t slotID)
{
	return IPCDUTMngrServer::GetInstance(devname,slotID);
}

IPCDUTMngrServer*	IPCDUTMngrServer::gInstances[]	= {0};

IPCDUTMngrServer*	IPCDUTMngrServer::GetInstance(const char* devname,uint8_t slotID)
{
	if(slotID > DUTMNGT_MAX_SLOTID)
	{
		LOGERROR("slotID should be <%d",DUTMNGT_MAX_SLOTID);
		return NULL;
	}

	if(NULL == gInstances[slotID])
	{
		gInstances[slotID]	= new IPCDUTMngrServer(devname,slotID);
	}

	return gInstances[slotID];
}

IPCDUTMngrServer::IPCDUTMngrServer(const char* devname,uint8_t slotID)
{
	char	procName[PROC_NAME_MAXLEN];
	snprintf(procName,PROC_NAME_MAXLEN,DUTMNGT_PROC_FORMAT,DUTMNGT_PROC_NAME,slotID);

	Initialize(procName,DUTMNGT_MAX_REQS,DUTMNGT_MAX_SLAVES);
	mDutStatus	= DUT_S_IDLE;
	mSlotID		= slotID;
	mpDutInfo	= NULL;
	mLastResult	= 0;

	strncpy(mDevName,devname,DUT_DEVNAME_MAXLEN);
	if(DUT_T_NVME == GetDutType())
	{
		mpDutInfo	= (DutInfo*)new NVMeDutInfo(devname);
	}
	else
	{
		LOGERROR("Unknown DUT Type : %s",devname);
		assert(0);
	}

	memset(mpSlotThread,0,sizeof(boost::thread*)*DUTMNGT_MAX_SLAVES);
}

IPCDUTMngrServer::~IPCDUTMngrServer(void)
{
	for(int idx=0;idx<DUTMNGT_MAX_SLAVES;idx++)
	{
		if(NULL != mpSlotThread[idx])
		{
			mpSlotThread[idx]->join();
			delete mpSlotThread[idx];
			mpSlotThread[idx] = NULL;
		}
	}

	if(NULL != mpDutInfo)
	{
		delete mpDutInfo;
		mpDutInfo = NULL;
	}
}

enDutType	IPCDUTMngrServer::GetDutType(void)
{
	if(NULL != strstr(mDevName,"nvme"))
	{
		return DUT_T_NVME;
	}

	return DUT_T_UNKNOWN;
}

stDutInfo*	IPCDUTMngrServer::GetDutInfo(void)
{
	assert(NULL != mpDutInfo);

	bool bRefresh	= (mDutStatus == DUT_S_IDLE)?true:false;
	stDutInfo*	pDutInfo	= mpDutInfo->GetDutInfo(bRefresh);
	if(NULL == pDutInfo)
	{
		mDutStatus	= DUT_S_ERROR;
	}

	return pDutInfo;
}

const char* IPCDUTMngrServer::GetStatusString(void)
{
	const char*	StatusStrLst[]	= {"IDLE","BUSY","ERROR","UNKNOWN"};

	if(DUT_S_UNKNOWN <= mDutStatus)
	{
		return "UNKNOWN";
	}

	return StatusStrLst[mDutStatus];
}

void IPCDUTMngrServer::PrintSlotInfo(void)
{
	stDutInfo*	info	= GetDutInfo();

	printf("###############################################\n");
	printf("Device Name     : %s\n",mDevName);
	printf("Device PhyAddr  : %s\n",info->dutPhyAddr);
	printf("Device SlotID   : %d\n",mSlotID);
	printf("Device Status   : %s\n",GetStatusString());
	printf("Serial Number   : %s\n",info->dutSN);
	printf("Model Number    : %s\n",info->dutMN);
	printf("Device Capacity : %dGB\n",info->dutCapGB);
	printf("LBA Format      : %d\n",info->dutSector);
	printf("FW Revision     : %s\n",info->dutFWRev);
	printf("Git CommitID    : %s\n",info->dutGITRev);
	printf("###############################################\n");
}

void IPCDUTMngrServer::_TrdExecShell(uint16_t slaveID,const char* cmdl)
{
	bool rslt = true;
	char* mem	= static_cast<char*>(mpMapRegion->get_address());
	stMsgResp*	resp = (stMsgResp*)(mem+mRespSize*slaveID);

	mDutStatus	= DUT_S_BUSY;

	LOGINFO("Execute Task : %s",cmdl);
	mLastResult = system(cmdl);

	WaitReqReceived(slaveID,REQ_T_GET_RETURN_DATA);

	resp->errorCode		= RESP_E_SUCCESS;
	resp->version		= GetVersion();
	resp->slaveID		= slaveID;
	resp->reqType		= REQ_T_ASYNC_FINISH;

	mDutStatus	= DUT_S_IDLE;
}

bool IPCDUTMngrServer::cb_ReceiveAndResponse(stMsgReq& req)
{
	uint16_t	slaveID	= req.slaveID;
	uint16_t	reqType = req.reqType;

	LOGDEBUG("%04d - %x",slaveID,reqType);

	if(slaveID >= mMaxSlaves)
	{
		LOGERROR("Invalid SlaveID");
		return false;
	}

	char* mem	= static_cast<char*>(mpMapRegion->get_address());
	stMsgResp*	resp = (stMsgResp*)(mem+mRespSize*slaveID);
	memset(resp,0,sizeof(stMsgResp));

	switch(reqType)
	{
		case REQ_T_IDENTIFY:
		{
			PrintSlotInfo();
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_GET_DUTINFO:
		{
			stDutInfo*	dutInfo	= GetDutInfo();
			if(NULL == dutInfo)
			{
				resp->dataSize	= 0;
				resp->errorCode	= RESP_E_GETINFO_ERROR;
			}
			else
			{
				resp->dataSize	= sizeof(stDutInfo);
				memcpy(resp->respDatum,(void*)dutInfo,sizeof(stDutInfo));
				resp->errorCode	= RESP_E_SUCCESS;
			}
			break;
		}
		case REQ_T_GET_DUTSTATUS:
		{
			resp->dataSize		= 1;
			resp->respDatum[0]	= (uint8_t)mDutStatus;
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_GET_LASTRESULT:
		{
			resp->dataSize		= 4;
			*(uint32_t*)(resp->respDatum)	= (uint32_t)mLastResult;
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_EXEC_TASK:
		{
			resp->dataSize		= 0;
			resp->errorCode		= RESP_E_ASYNC_REQ;
			boost::thread* pThrd = mpSlotThread[slaveID];
			if(NULL != pThrd)
			{
				pThrd->join();
				delete pThrd;
				pThrd = NULL;
			}

			uint16_t	strlen		= *(uint16_t*)(&(req.reqDatum[0]));
			if(strlen >= DUT_EXECTASK_MAXLEN)
			{
				resp->errorCode		= RESP_E_INVALID_PARAM;
			}
			else
			{
				char cmdLine[DUT_EXECTASK_MAXLEN];
				strncpy(cmdLine,(const char*)&(req.reqDatum[2]),DUT_EXECTASK_MAXLEN);

				mpSlotThread[slaveID] = (boost::thread*)new boost::thread(boost::bind(&IPCDUTMngrServer::_TrdExecShell,
									this,slaveID,cmdLine));

				usleep(100000);
			}

			break;
		}
		default:
		{
			return MsgProcCommServer::cb_ReceiveAndResponse(req);
		}
	}

	resp->version	= GetVersion();
	resp->slaveID	= slaveID;
	resp->reqType	= reqType;

	return true;
}

IPCDUTMngrClient::IPCDUTMngrClient(uint16_t slotID,uint8_t slaveID)
{
	char	procName[PROC_NAME_MAXLEN];
	snprintf(procName,PROC_NAME_MAXLEN,DUTMNGT_PROC_FORMAT,DUTMNGT_PROC_NAME,slotID);

	Initialize(procName,slaveID);
}

IPCDUTMngrClient::~IPCDUTMngrClient(void)
{

}

DutInfo*	IPCDUTMngrClient::GetDutInfo(void)
{
	stDutInfo	dutInfo;
	if(false == SendSyncRequest(REQ_T_GET_DUTINFO,NULL,&dutInfo,0,sizeof(stDutInfo),DUTMNGT_RESP_TIMEOUT))
	{
		return NULL;
	}

	return new DutInfo(&dutInfo);
}

enDutStatus	IPCDUTMngrClient::GetDutStatus(void)
{
	uint8_t tmpVal = 0;
	if(false == SendSyncRequest(REQ_T_GET_DUTSTATUS,NULL,&tmpVal,0,1,DUTMNGT_RESP_TIMEOUT))
	{
		return DUT_S_UNKNOWN;
	}

	return (enDutStatus)tmpVal;
}

bool IPCDUTMngrClient::Identify(void)
{
	return SendSyncRequest(REQ_T_IDENTIFY,NULL,NULL,0,0,DUTMNGT_RESP_TIMEOUT);
}

int	IPCDUTMngrClient::GetLastResult(void)
{
	int	result = -1;

	if(false == SendSyncRequest(REQ_T_GET_LASTRESULT,NULL,&result,0,4,DUTMNGT_RESP_TIMEOUT))
	{
		return -1;
	}

	return result;
}

bool IPCDUTMngrClient::ExecTask(const char* cmdline)
{
	int cmdlSize	= strlen(cmdline);
	if(cmdlSize > (DUT_EXECTASK_MAXLEN -3))
	{
		return false;
	}

	uint8_t	inputBuf[DUT_EXECTASK_MAXLEN];
	memset(inputBuf,0,sizeof(uint8_t)*DUT_EXECTASK_MAXLEN);

	*(uint16_t*)inputBuf	= cmdlSize;
	memcpy(inputBuf+2,cmdline,cmdlSize+1);

	return SendAsyncRequest(REQ_T_EXEC_TASK,inputBuf,DUT_EXECTASK_MAXLEN,DUTMNGT_RESP_TIMEOUT);
}

bool IPCDUTMngrClient::WaitTaskDone(uint32_t timeout)
{
	return WaitAsyncDone(NULL,0,timeout);
}
