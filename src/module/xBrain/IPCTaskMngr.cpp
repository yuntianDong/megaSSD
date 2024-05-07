/*
 * file: IPCTaskMngr.cpp
 */

#include <string.h>
#include "IPCTaskMngr.h"

#define UNUSED_TASKID			0xFFFF
#define UNUSED_LOOPCNT			0
#define DUT_PHYADDR_LEN			16

#define QUERY_TRY_MAXCOUNT		3

IPCTaskMngrServer*	IPCTaskMngrServer::gInstance[TASKMNGR_MAX_SERVERS] = {0};

IPCTaskMngrServer*	IPCTaskMngrServer::GetInstance(uint8_t index)
{
	if(index >= TASKMNGR_MAX_SERVERS)
	{
		LOGERROR("Invalid Index Parameter: %d",index);
	}

	if(NULL == gInstance[index])
	{
		gInstance[index]	= new IPCTaskMngrServer(index);
	}

	return gInstance[index];
}

IPCTaskMngrServer::IPCTaskMngrServer(uint8_t index)
	:MsgProcCommServer(),FileMapSharedMem(TASKMNGR_FILE_DUMP,M_READWRITE,TASKMNGR_FILE_SIZE)
{
	stTaskMngr*	taskMngr	= (stTaskMngr*)GetSharedMemAddress();
	mpTaskMngr				= new TaskMngr(taskMngr);

	//memset(mpDutClients,0,sizeof(IPCDUTMngrClient*)*MAX_SLOT_NUM);
	for(uint8_t idx=0;idx<MAX_SLOT_NUM;idx++)
	{
		mpDutClients[idx]	= NULL;
	}
	memset(mSlotTaskID,0xFF,sizeof(uint16_t)*MAX_SLOT_NUM);
	memset(mSlotLoopCnt,0,sizeof(uint32_t)*MAX_SLOT_NUM);

	char procName[PROC_NAME_MAXLEN];
	snprintf(procName,PROC_NAME_MAXLEN,TASKMNGR_PROC_FORMAT,TASKMNGR_PROC_NAME,index);
	mIndex			= index;
	mLastOpSlotID	= 0;

	Initialize(procName,TASKMNGR_MAX_REQS,TASKMNGR_MAX_SLAVES);
}

IPCTaskMngrServer::~IPCTaskMngrServer(void)
{
	if(NULL != mpTaskMngr)
	{
		delete mpTaskMngr;
		mpTaskMngr = NULL;
	}
}

bool IPCTaskMngrServer::cb_RunCallbackRoutine(void)
{
	uint8_t slotID	= GetNextIdleSlotID();
	if(slotID >= MAX_SLOT_NUM)
	{
		return MsgProcCommServer::cb_RunCallbackRoutine();
	}

	uint16_t taskID	= GetSlotTaskID(slotID);
	uint32_t loopCnt= GetSlotLeftLoops(slotID);

	if( (UNUSED_TASKID == taskID) && (true == mpTaskMngr->IsEmpty()))
	{
		return true;
	}

	if(UNUSED_TASKID != taskID && 0 == loopCnt)
	{
		if(false == UpdateTaskResult(taskID,slotID))
		{
			LOGERROR("Fail to call UpdateTaskResult(%d,%d)",taskID,slotID);
			return false;
		}
	}
	else
	{
		if(false == AsignTaskToSlot(taskID,slotID))
		{
			LOGERROR("Fail to call AsignTaskToSlot(desc,%d)",slotID);
			return false;
		}
	}

	return MsgProcCommServer::cb_RunCallbackRoutine();
}

bool IPCTaskMngrServer::AsignTaskToSlot(uint16_t taskID,uint8_t slotID)
{
	bool	bRtn	= true;
	TaskDesc* desc	= NULL;
	if(UNUSED_TASKID == taskID
			&& (false == mpTaskMngr->GetTaskDescForExe(desc) || NULL == desc))
	{
		LOGERROR("Fail to call GetTaskDescForExe(desc)");
		bRtn = false;
	}
	else if(UNUSED_TASKID != taskID
			&& (false == mpTaskMngr->FindTaskDescByIndex(taskID,desc) || NULL == desc) )
	{
		LOGERROR("Fail to call FindTaskDescByIndex(%d,desc)",taskID);
		bRtn = false;
	}

	if(true == bRtn)
	{
		if(UNUSED_TASKID == taskID)
		{
			SetSlotLeftLoops(slotID,desc->GetLoopCount());
			SetSlotTaskID(slotID,desc->GetTaskIndex());
		}

		if(0 < GetSlotLeftLoops(slotID))
		{
			SetSlotLeftLoops(slotID,GetSlotLeftLoops(slotID)-1);
		}

		const char* cmdline	= desc->GetCmdLine();
		if(false == ExecCmdLineOnSlot(slotID,cmdline))
		{
			LOGERROR("Fail to call ExecCmdLineOnSlot(%d,cmdline)",slotID);
			bRtn = false;
		}
	}

	if(NULL != desc)
	{
		delete desc;
		desc = NULL;
	}

	return bRtn;
}

bool IPCTaskMngrServer::UpdateTaskResult(uint16_t taskID,uint8_t slotID)
{
	int rslt	= -1;
	if(false == GetSlotLastResult(slotID,rslt))
	{
		LOGERROR("Fail to call GetSlotLastResult(%d,rslt)",slotID);
		return false;
	}

	TaskResult* tr	= NULL;
	if(false == mpTaskMngr->FindTaskResultByIndex(taskID,tr))
	{
		LOGERROR("Fail to call FindTaskResultByIndex(%d,tr)",taskID);
		return false;
	}
	char*	dutPhyAddr	= NULL;
	if(true == GetSlotPhyAddr(slotID,dutPhyAddr) && NULL != dutPhyAddr)
	{
		tr->SetTargetPhyAddr(dutPhyAddr);
		free(dutPhyAddr);
	}

	tr->SetSlotID(slotID);
	tr->SetTestResult(rslt);
	SetSlotTaskID(slotID,UNUSED_TASKID);

	if(NULL != tr)
	{
		delete tr;
		tr = NULL;
	}

	Flush();

	return true;
}

bool IPCTaskMngrServer::ExecCmdLineOnSlot(uint8_t slotID,const char* cmdline)
{
	IPCDUTMngrClient*	client	= mpDutClients[slotID];
	if(NULL == client)
	{
		LOGERROR("Slot Client is Empty");
		return false;
	}

	if(false == client->ExecTask(cmdline))
	{
		LOGERROR("Fail to call ExecTask(%s)",cmdline);
		return false;
	}

	return true;
}

bool IPCTaskMngrServer::IdentifySlot(uint8_t slotID)
{
	IPCDUTMngrClient*	client	= mpDutClients[slotID];
	if(NULL == client)
	{
		LOGERROR("Slot Client is Empty");
		return false;
	}

	return client->Identify();
}

bool IPCTaskMngrServer::GetSlotLastResult(uint8_t slotID,int& rslt)
{
	IPCDUTMngrClient*	client	= mpDutClients[slotID];
	if(NULL == client)
	{
		LOGERROR("Slot Client is Empty");
		return false;
	}

	rslt = client->GetLastResult();
	return true;
}

bool IPCTaskMngrServer::GetSlotPhyAddr(uint8_t slotID,char*& phyAddr)
{
	IPCDUTMngrClient*	client	= mpDutClients[slotID];
	if(NULL == client)
	{
		LOGERROR("Slot Client is Empty");
		return false;
	}

	DutInfo* dutinfo	= client->GetDutInfo();
	if(NULL == dutinfo)
	{
		LOGERROR("Fail to call GetDutInfo()");
		return false;
	}

	phyAddr = (char*)malloc(DUT_PHYADDR_LEN*sizeof(char));
	memset(phyAddr,0,DUT_PHYADDR_LEN);

	strncpy(phyAddr,dutinfo->GetPhyAddr(),DUT_PHYADDR_LEN);
	delete dutinfo;

	return true;
}

bool IPCTaskMngrServer::GetSlotStatus(uint8_t slotID,enDutStatus& status)
{
	IPCDUTMngrClient*	client	= mpDutClients[slotID];
	if(NULL == client)
	{
		LOGERROR("Slot Client is Empty");
		return false;
	}

	status = client->GetDutStatus();
	return true;
}

bool IPCTaskMngrServer::GetSlotDevInfo(uint8_t slotID,DutInfo*& info)
{
	IPCDUTMngrClient*	client	= mpDutClients[slotID];
	if(NULL == client)
	{
		LOGERROR("Slot Client is Empty");
		return false;
	}

	info = client->GetDutInfo();
	return true;
}

uint8_t	IPCTaskMngrServer::GetAvailSlotCount(void)
{
	uint8_t count = 0;

	for(uint8_t idx=0;idx<MAX_SLOT_NUM;idx++)
	{
		if(NULL != mpDutClients[idx])
		{
			count++;
		}
	}

	return count;
}

void	IPCTaskMngrServer::GetAllAvailSlotIDs(uint8_t*& psIDLst)
{
	uint8_t	count	= GetAvailSlotCount();
	psIDLst			= (uint8_t*)malloc(sizeof(uint8_t)*count);
	uint8_t	nIndex	= 0;

	for(uint8_t idx=0;idx<MAX_SLOT_NUM && nIndex<count;idx++)
	{
		if(NULL != mpDutClients[idx])
		{
			psIDLst[nIndex++]	= idx;
		}
	}
}

uint8_t	IPCTaskMngrServer::GetNextIdleSlotID(void)
{
	uint8_t idx= (mLastOpSlotID + 1) % MAX_SLOT_NUM;
	uint8_t maxChkCnt = QUERY_TRY_MAXCOUNT;
	uint8_t slotID	= MAX_SLOT_NUM;
	for(uint8_t tmp = 0;tmp < MAX_SLOT_NUM && 0 < maxChkCnt;tmp++)
	{
		if(NULL != mpDutClients[idx])
		{
			enDutStatus status = DUT_S_UNKNOWN;
			if(false == GetSlotStatus(idx,status))
			{
				slotID = MAX_SLOT_NUM;
				LOGWARN("Fail to GetSlotStatus(%d)",idx);
				break;
			}

			if(DUT_S_IDLE == status)
			{
				slotID = idx;
				break;
			}

			maxChkCnt--;
		}

		idx = (idx + 1) % MAX_SLOT_NUM;
	}

	mLastOpSlotID	= idx;
	return slotID;
}

bool IPCTaskMngrServer::cb_ReceiveAndResponse(stMsgReq& req)
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
		case REQ_T_DEBUGDUMP:
		{
			DebugDump();
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_RESET:
		{
			mpTaskMngr->Reset();
			Flush();
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_ADDTASK:
		{
			stTaskDesc*	task	= (stTaskDesc*)(req.reqDatum);
			resp->errorCode	= (true == ReqHdlr_AddTask(task))?RESP_E_SUCCESS:RESP_E_FAIL_ADDTASK;

			break;
		}
		case REQ_T_GETPENDINGTASKCNT:
		{
			resp->dataSize	= 2;
			*(uint16_t*)(resp->respDatum)	= mpTaskMngr->GetPendingCnt();
			resp->errorCode	= RESP_E_SUCCESS;

			break;
		}
		case REQ_T_PRTTESTREPORT:
		{
			ReqHdlr_PrintReport();
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_DUMPTESTREPORT:
		{
			char * filename = (char *)(req.reqDatum);

			ReqHdlr_DumpTestReport(filename);
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_PRTTESTRESULT:
		{
			ReqHdlr_PrintResult();
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_DUMPTESTRESULT:
		{
			char * filename = (char *)(req.reqDatum);

			ReqHdlr_DumpTestResult(filename);
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_ADDSLOT:
		{
			uint8_t	slotID	= req.reqDatum[0];
			bool rtn = ReqHdlr_AddSlot(slotID);
			resp->errorCode	= (true==rtn)?RESP_E_SUCCESS:RESP_E_FAIL_ADDSLOT;
			break;
		}
		case REQ_T_DELSLOT:
		{
			uint8_t	slotID	= req.reqDatum[0];
			ReqHdlr_DelSlot(slotID);
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_DELALLSLOT:
		{
			ReqHdlr_DelAllSlot();
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_GETDEVINFO:
		{
			uint8_t	slotID	= req.reqDatum[0];
			DutInfo* pInfo	= NULL;
			if(false == GetSlotDevInfo(slotID,pInfo))
			{
				resp->dataSize	= 0;
				resp->errorCode	= RESP_E_FAIL_GETDEVINFO;
			}
			else
			{
				resp->dataSize	= sizeof(stDutInfo);
				memcpy(resp->respDatum,(void*)(pInfo->GetDutInfo(false)),sizeof(stDutInfo));
				resp->errorCode	= RESP_E_SUCCESS;
			}
			break;
		}
		case REQ_T_GETDEVSTATUS:
		{
			uint8_t	slotID	= req.reqDatum[0];
			enDutStatus	status;
			if(false == GetSlotStatus(slotID,status))
			{
				resp->dataSize	= 0;
				resp->errorCode	= RESP_E_FAIL_GETDEVSTATUS;
			}
			else
			{
				resp->dataSize	= 1;
				resp->respDatum[0]	= status;
				resp->errorCode	= RESP_E_SUCCESS;
			}
			break;
		}
		case REQ_T_IDFYSLOT:
		{
			uint8_t	slotID	= req.reqDatum[0];
			resp->errorCode	= (true == ReqHdlr_IdfySlot(slotID))?RESP_E_SUCCESS:RESP_E_FAIL_IDFYSLOT;
			break;
		}
		case REQ_T_IDFYALLSLOT:
		{
			resp->errorCode	= (true == ReqHdlr_IdfyAllSlots())?RESP_E_SUCCESS:RESP_E_FAIL_IDFYSLOT;
			break;
		}
		case REQ_T_CHECKSLOTAVAIL:
		{
			uint8_t	slotID	= req.reqDatum[0];
			resp->errorCode	= (NULL !=mpDutClients[slotID])?RESP_E_SUCCESS:RESP_E_FAIL_CHECKSLOTAVAIL;
			break;
		}
		case REQ_T_GETAVAILSLOTS:
		{
			uint8_t	count = GetAvailSlotCount();
			uint8_t* pSlotLst	= NULL;
			GetAllAvailSlotIDs(pSlotLst);

			resp->respDatum[0]	= count;
			resp->dataSize	= count* sizeof(uint8_t) + 1;
			memcpy(&(resp->respDatum[1]),pSlotLst,count* sizeof(uint8_t));
			resp->errorCode	= RESP_E_SUCCESS;
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

bool IPCTaskMngrServer::ReqHdlr_AddTask(stTaskDesc*	task)
{
	TaskDesc* desc	= NULL;
	if(false == mpTaskMngr->GetTaskDescForCfg(desc) || NULL == desc)
	{
		return false;
	}

	uint16_t	descIdx	= desc->GetTaskIndex();
	memcpy(desc->GetDataPoint(),task,sizeof(stTaskDesc));
	desc->SetTaskIndex(descIdx);

	Flush();

	delete desc;
	return true;
}

bool IPCTaskMngrServer::ReqHdlr_PrintReport(void)
{
	TaskReport report;
	if(false == mpTaskMngr->MakeTestReport(report))
	{
		return false;
	}

	report.Report();
	return true;
}

bool IPCTaskMngrServer::ReqHdlr_DumpTestReport(const char* filename)
{
	TaskReport report;
	if(false == mpTaskMngr->MakeTestReport(report))
	{
		return false;
	}

	report.Report(filename);
	return true;
}

bool IPCTaskMngrServer::ReqHdlr_PrintResult(void)
{
	return mpTaskMngr->PrintTestResult();
}

bool IPCTaskMngrServer::ReqHdlr_DumpTestResult(const char* filename)
{
	return mpTaskMngr->SaveResultAs(filename);
}

bool IPCTaskMngrServer::ReqHdlr_AddSlot(uint8_t slotID)
{
	if(NULL != mpDutClients[slotID])
	{
		LOGERROR("Slot%d is not Empty",slotID);
		return false;
	}

	IPCDUTMngrClient* client	= new IPCDUTMngrClient(slotID,mIndex);
	if(false == client->IsGood())
	{
		return false;
	}

	mpDutClients[slotID]	= client;
	return true;
}

bool IPCTaskMngrServer::ReqHdlr_DelSlot(uint8_t slotID)
{
	LOGWARN("ReqHdlr_DelSlot : %d",slotID);

	if(NULL != mpDutClients[slotID])
	{
		delete mpDutClients[slotID];
		mpDutClients[slotID]	= NULL;
	}

	mSlotTaskID[slotID]	= UNUSED_TASKID;
	mSlotLoopCnt[slotID]= UNUSED_LOOPCNT;

	return true;
}

bool IPCTaskMngrServer::ReqHdlr_DelAllSlot(void)
{
	for(uint8_t slotID=0;slotID<MAX_SLOT_NUM;slotID++)
	{
		ReqHdlr_DelSlot(slotID);
	}

	return true;
}

bool IPCTaskMngrServer::ReqHdlr_IdfySlot(uint8_t slotID)
{
	if(false == IdentifySlot(slotID))
	{
		ReqHdlr_DelSlot(slotID);
		return false;
	}

	return true;
}

bool IPCTaskMngrServer::ReqHdlr_IdfyAllSlots(void)
{
	bool	rtn	= true;
	for(uint8_t idx=0;idx<MAX_SLOT_NUM;idx++)
	{
		IPCDUTMngrClient* client = mpDutClients[idx];
		if(NULL != client)
		{
			if(false == client->Identify())
			{
				ReqHdlr_DelSlot(idx);
				rtn	= false;
			}
		}
	}

	return rtn;
}

void IPCTaskMngrServer::DebugDump(void)
{
	printf("mIndex	= %d\n",mIndex);
	printf("mLastOpSlotID = %d\n",mLastOpSlotID);

	for(uint8_t idx=0;idx<MAX_SLOT_NUM;idx++)
	{
		printf("[%d] %s %2d %4d\n",idx,(NULL == mpDutClients[idx])?"NULL":"GOOD",
				mSlotTaskID[idx],mSlotLoopCnt[idx]);
	}

	printf("TaskMngr Dump:\n");
	mpTaskMngr->Dump();
}

IPCTaskMngrClient*	IPCTaskMngrClient::gInstance[TASKMNGR_MAX_SLAVES]	= {0};

IPCTaskMngrClient*	IPCTaskMngrClient::GetInstance(uint8_t svrIdx,uint8_t slaveID)
{
	if(slaveID > TASKMNGR_MAX_SLAVES)
	{
		LOGERROR("Invalid Parameter: slaveID(%d) >= TASKMNGR_MAX_SLAVES(4)",slaveID);
		return NULL;
	}

	if(NULL == gInstance[slaveID])
	{
		gInstance[slaveID]	= new IPCTaskMngrClient(svrIdx,slaveID);
	}

	return gInstance[slaveID];
}

IPCTaskMngrClient::IPCTaskMngrClient(uint8_t svrIdx,uint8_t slaveID)
	:MsgProcCommClient()
{
	char procName[PROC_NAME_MAXLEN];
	snprintf(procName,PROC_NAME_MAXLEN,TASKMNGR_PROC_FORMAT,TASKMNGR_PROC_NAME,svrIdx);

	Initialize(procName,slaveID);
}

bool IPCTaskMngrClient::Reset(void)
{
	return SendSyncRequest(REQ_T_RESET,NULL,NULL,0,0,TASKMNGR_RESP_TIMEOUT);
}

bool IPCTaskMngrClient::AddTask(TaskDesc* task)
{
	if(NULL == task)
	{
		return false;
	}

	return SendSyncRequest(REQ_T_ADDTASK,task->GetDataPoint(),NULL,sizeof(stTaskDesc),0,TASKMNGR_RESP_TIMEOUT);
}

uint16_t IPCTaskMngrClient::GetPeningTaskCnt(void)
{
	uint16_t count = 0;
	if(false == SendSyncRequest(REQ_T_GETPENDINGTASKCNT,NULL,&count,
			0,sizeof(uint16_t),TASKMNGR_RESP_TIMEOUT))
	{
		return (uint16_t)-1;
	}

	return count;
}

bool IPCTaskMngrClient::PrintTestReport(void)
{
	return SendSyncRequest(REQ_T_PRTTESTREPORT,NULL,NULL,0,0,TASKMNGR_RESP_TIMEOUT);
}

bool IPCTaskMngrClient::DumpTestReport(const char* filename)
{
	return SendSyncRequest(REQ_T_DUMPTESTREPORT,(void *)filename,NULL,strlen(filename)+1,0,TASKMNGR_RESP_TIMEOUT);
}

bool IPCTaskMngrClient::PrintTestResult(void)
{
	return SendSyncRequest(REQ_T_PRTTESTRESULT,NULL,NULL,0,0,TASKMNGR_RESP_TIMEOUT);
}

bool IPCTaskMngrClient::DumpTestResult(const char* filename)
{
	return SendSyncRequest(REQ_T_DUMPTESTRESULT,(void *)filename,NULL,strlen(filename)+1,0,TASKMNGR_RESP_TIMEOUT);
}

bool IPCTaskMngrClient::AddSlot(uint8_t slotID)
{
	return SendSyncRequest(REQ_T_ADDSLOT,&slotID,NULL,1,0,TASKMNGR_RESP_TIMEOUT);
}

bool IPCTaskMngrClient::DelSlot(uint8_t slotID)
{
	return SendSyncRequest(REQ_T_DELSLOT,&slotID,NULL,1,0,TASKMNGR_RESP_TIMEOUT);
}

DutInfo* IPCTaskMngrClient::GetSlotInfo(uint8_t slotID)
{
	stDutInfo	dutInfo;
	if(false == SendSyncRequest(REQ_T_GETDEVINFO,&slotID,&dutInfo,1,sizeof(stDutInfo),TASKMNGR_RESP_TIMEOUT))
	{
		return NULL;
	}

	return new DutInfo(&dutInfo);
}

enDutStatus	IPCTaskMngrClient::GetSlotStatus(uint8_t slotID)
{
	uint8_t tmpVal = 0;
	if(false == SendSyncRequest(REQ_T_GETDEVSTATUS,&slotID,&tmpVal,1,1,TASKMNGR_RESP_TIMEOUT))
	{
		return DUT_S_UNKNOWN;
	}

	return (enDutStatus)tmpVal;
}

bool IPCTaskMngrClient::IdentifySlot(uint8_t slotID)
{
	return SendSyncRequest(REQ_T_IDFYSLOT,&slotID,NULL,1,0,TASKMNGR_RESP_TIMEOUT);
}

bool IPCTaskMngrClient::IdfyAllSlots(void)
{
	return SendSyncRequest(REQ_T_IDFYALLSLOT,NULL,NULL,0,0,TASKMNGR_RESP_TIMEOUT);
}

bool IPCTaskMngrClient::CheckSlotAvail(uint8_t slotID)
{
	return SendSyncRequest(REQ_T_CHECKSLOTAVAIL,&slotID,NULL,1,0,TASKMNGR_RESP_TIMEOUT);
}

bool IPCTaskMngrClient::DebugDump(void)
{
	return SendSyncRequest(REQ_T_DEBUGDUMP,NULL,NULL,0,0,TASKMNGR_RESP_TIMEOUT);
}

uint8_t IPCTaskMngrClient::GetAvailSlotID(uint8_t* sIDLst,uint8_t maxCnt)
{
	if(NULL == sIDLst || 0 == maxCnt)
	{
		return (uint8_t)-1;
	}

	uint8_t	outDatum[MAX_SLOT_NUM+1];
	memset(outDatum,0,(MAX_SLOT_NUM+1)*sizeof(uint8_t));

	if(false == SendSyncRequest(REQ_T_GETAVAILSLOTS,NULL,outDatum,0,MAX_SLOT_NUM+1,TASKMNGR_RESP_TIMEOUT))
	{
		return (uint8_t)-1;
	}

	memcpy(sIDLst,&(outDatum[1]),(maxCnt > MAX_SLOT_NUM)?MAX_SLOT_NUM:maxCnt);
	return outDatum[0];
}

bool IPCTaskMngrClient::DelAllSlot(void)
{
	return SendSyncRequest(REQ_T_DELALLSLOT,NULL,NULL,0,0,TASKMNGR_RESP_TIMEOUT);
}
