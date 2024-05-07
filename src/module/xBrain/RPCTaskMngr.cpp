/*
 * file : RPCTaskMngr.cpp
 */

#include "RPCTaskMngr.h"
#include "net/JsonRPC2TaskMngr.h"
#include "Logging.h"

RPCTaskMngrServer::RPCTaskMngrServer(const char* addr)
	:NanoJsonRPCServer(addr),FileMapSharedMem(NJR_TASKMNGR_FILE_DUMP,M_READWRITE,NJR_TASKMNGR_FILE_SIZE)
{
	stTaskMngr*	taskMngr	= (stTaskMngr*)GetSharedMemAddress();
	mpTaskMngr				= new TaskMngr(taskMngr);
}

RPCTaskMngrServer::~RPCTaskMngrServer(void)
{
	if(NULL != mpTaskMngr)
	{
		delete mpTaskMngr;
		mpTaskMngr = NULL;
	}
}

bool	RPCTaskMngrServer::IsCycleDone(void)
{
	return 0 == mpTaskMngr->GetPendingCnt()
			&& (0 != mpTaskMngr->GetTotalCount())
			&& (mpTaskMngr->GetFinishCnt() == mpTaskMngr->GetTotalCount());
}

void	RPCTaskMngrServer::DumpTestResult(void)
{
	mpTaskMngr->SaveResultAs(NJR_TASKMNGR_TEST_DETAIL);
	TaskReport tr;
	mpTaskMngr->MakeTestReport(tr);

	tr.Report(NJR_TASKMNGR_TEST_REPORT);
}

int 	RPCTaskMngrServer::RunCallBackRoutine(void)
{
	if(true == IsCycleDone())
	{
		DumpTestResult();
		return NANORPC_SRV_RERR_CANCELLED;
	}

	return NanoRPCServer::RunCallBackRoutine();
}

int RPCTaskMngrServer::HandleRoutine(void* req,int reqSize,void* resp,int respSize)
{
	if(false == PreHandleCheck(req,reqSize,resp,respSize))
	{
		return NANORPC_SRV_RERR_SUCCESS;
	}

	RPCJsonReq	jReq((const char*)req);
	const char* method	= jReq.GetMethod();
	int	reqId			= jReq.GetID();

	if(0 == strcmp(method,NJR_REQ_GET_TOTALCNT))
	{
		uint16_t ttlCnt	= mpTaskMngr->GetTotalCount();
		RPCJsonRespWithSuccess msg(reqId);
		msg.AddResult<int>("Value",ttlCnt);
		CopyJsonRespToBuf<RPCJsonRespWithSuccess>(msg,resp,respSize);
	}
	else if(0 == strcmp(method,NJR_REQ_GET_LEFTCNT))
	{
		uint16_t leftCnt	= mpTaskMngr->GetPendingCnt();
		RPCJsonRespWithSuccess msg(reqId);
		msg.AddResult<int>("Value",leftCnt);
		CopyJsonRespToBuf<RPCJsonRespWithSuccess>(msg,resp,respSize);
	}
	else if(0 == strcmp(method,NJR_REQ_ADD_TASKDESC))
	{
		TaskDesc task;
		GetTaskDescFromJsonReq(task,jReq);
		if(false == AddTask(&task))
		{
			RPCJsonErrInternalError error(reqId);
			CopyJsonRespToBuf<RPCJsonErrInternalError>(error,resp,respSize);
		}
		else
		{
			RPCJsonRespWithSuccess msg(reqId);
			CopyJsonRespToBuf<RPCJsonRespWithSuccess>(msg,resp,respSize);
		}
	}
	else if(0 == strcmp(method,NJR_REQ_GET_TASKDESC))
	{
		TaskDesc* task = NULL;
		if(false == mpTaskMngr->GetTaskDescForExe(task) || NULL == task)
		{
			RPCJsonErrInternalError error(reqId);
			CopyJsonRespToBuf<RPCJsonErrInternalError>(error,resp,respSize);
		}
		else
		{
			RPCJsonRespWithSuccess msg(reqId);
			FillJsonRespWithTaskDesc(msg,*task);
			CopyJsonRespToBuf<RPCJsonRespWithSuccess>(msg,resp,respSize);

			delete task;
		}
	}
	else if(0 == strcmp(method,NJR_REQ_UPT_TASKRSLT))
	{
		TaskResult rslt;
		GetTaskRsltFromJsonReq(rslt,jReq);

		rslt.Dump();

		if(false == AddRslt(&rslt))
		{
			RPCJsonErrInternalError error(reqId);
			CopyJsonRespToBuf<RPCJsonErrInternalError>(error,resp,respSize);
		}
		else
		{
			RPCJsonRespWithSuccess msg(reqId);
			CopyJsonRespToBuf<RPCJsonRespWithSuccess>(msg,resp,respSize);
		}
	}
	else if(0 == strcmp(method,NJR_REQ_GET_TASKRSLT))
	{
		int index = jReq.GetParam<int>("index");

		TaskResult*	pRslt	= NULL;
		if(false == mpTaskMngr->FindTaskResultByIndex(index,pRslt) && NULL == pRslt)
		{
			RPCJsonErrInternalError error(reqId);
			CopyJsonRespToBuf<RPCJsonErrInternalError>(error,resp,respSize);
		}
		else
		{
			RPCJsonRespWithSuccess msg(reqId);
			FillJsonRespWithTaskRslt(msg,*pRslt);
			CopyJsonRespToBuf<RPCJsonRespWithSuccess>(msg,resp,respSize);

			delete pRslt;
		}
	}
	else if(0 == strcmp(method,NJR_REQ_DEBUG_DUMP))
	{
		mpTaskMngr->Dump();
		RPCJsonRespWithSuccess msg(reqId);
		CopyJsonRespToBuf<RPCJsonRespWithSuccess>(msg,resp,respSize);
	}
	else if(0 == strcmp(method,NJR_REQ_DUMP_RESULT))
	{
		DumpTestResult();
		RPCJsonRespWithSuccess msg(reqId);
		CopyJsonRespToBuf<RPCJsonRespWithSuccess>(msg,resp,respSize);
	}
	else
	{
		return NanoJsonRPCServer::HandleRoutine(req,reqSize,resp,respSize);
	}

	return NANORPC_SRV_RERR_SUCCESS;
}

bool RPCTaskMngrServer::AddTask(TaskDesc* task)
{
	TaskDesc*	desc;
	if(false == mpTaskMngr->GetTaskDescForCfg(desc) || NULL == desc)
	{
		return false;
	}

	uint16_t	descIdx	= desc->GetTaskIndex();
	memcpy(desc->GetDataPoint(),task->GetDataPoint(),sizeof(stTaskDesc));
	desc->SetTaskIndex(descIdx);

	delete desc;
	return Flush();
}

bool RPCTaskMngrServer::AddRslt(TaskResult* rslt)
{
	uint16_t	index	= rslt->GetRsltIndex();
	TaskResult*	pRslt	= NULL;
	if(false == mpTaskMngr->FindTaskResultByIndex(index,pRslt) && NULL == pRslt)
	{
		return false;
	}

	memcpy(pRslt->GetDataPoint(),rslt->GetDataPoint(),sizeof(stRsltDesc));

	delete pRslt;
	return Flush();
}

bool RPCTaskMngrServer::Reset(void)
{
	mpTaskMngr->Reset();
	return Flush();
}

uint16_t	RPCTaskMngrClient::GetTotalCount(void)
{
	char req[NANORPC_MAXLEN_REQ];
	char resp[NANORPC_MAXLEN_RESP];

	memset(req,0,sizeof(char)*NANORPC_MAXLEN_REQ);
	memset(resp,0,sizeof(char)*NANORPC_MAXLEN_RESP);

	RPCJsonReq jr(GetJsonReqID(),NJR_REQ_GET_TOTALCNT);
	jr(req,NANORPC_MAXLEN_REQ);

	if(false == SendReqAndWaitForResp((void*)req,NANORPC_MAXLEN_REQ,resp,NANORPC_MAXLEN_RESP))
	{
		return (uint16_t)-1;
	}

	try
	{
		RPCJsonRespWithSuccess result(resp);
		return result.GetResult<int>("Value");
	}
	catch(...)
	{
		RPCJsonRespWithFailure error(resp);
		LOGERROR("Error Code = %d : %s",error.GetErrorCode(),error.GetErrorMessage());
	}

	return (uint16_t)-1;
}

uint16_t	RPCTaskMngrClient::GetLeftCount(void)
{
	char req[NANORPC_MAXLEN_REQ];
	char resp[NANORPC_MAXLEN_RESP];

	memset(req,0,sizeof(char)*NANORPC_MAXLEN_REQ);
	memset(resp,0,sizeof(char)*NANORPC_MAXLEN_RESP);

	RPCJsonReq jr(GetJsonReqID(),NJR_REQ_GET_LEFTCNT);
	jr(req,NANORPC_MAXLEN_REQ);

	if(false == SendReqAndWaitForResp((void*)req,NANORPC_MAXLEN_REQ,resp,NANORPC_MAXLEN_RESP))
	{
		return (uint16_t)-1;
	}

	try
	{
		RPCJsonRespWithSuccess result(resp);
		return result.GetResult<int>("Value");
	}
	catch(...)
	{
		RPCJsonRespWithFailure error(resp);
		LOGERROR("Error Code = %d : %s",error.GetErrorCode(),error.GetErrorMessage());
	}

	return (uint16_t)-1;
}

bool		RPCTaskMngrClient::AddTaskDesc(TaskDesc* task)
{
	char req[NANORPC_MAXLEN_REQ];
	char resp[NANORPC_MAXLEN_RESP];

	memset(req,0,sizeof(char)*NANORPC_MAXLEN_REQ);
	memset(resp,0,sizeof(char)*NANORPC_MAXLEN_RESP);

	RPCJsonReq jr(GetJsonReqID(),NJR_REQ_ADD_TASKDESC);
	jr(req,NANORPC_MAXLEN_REQ);
	FillJsonReqWithTaskDesc(jr,*task);

	if(false == SendReqAndWaitForResp((void*)req,NANORPC_MAXLEN_REQ,resp,NANORPC_MAXLEN_RESP))
	{
		return false;
	}

	try
	{
		RPCJsonRespWithSuccess result(resp);
		return true;
	}
	catch(...)
	{
		RPCJsonRespWithFailure error(resp);
		LOGERROR("Error Code = %d : %s",error.GetErrorCode(),error.GetErrorMessage());
	}

	return false;
}

bool		RPCTaskMngrClient::GetTaskDesc(TaskDesc* task)
{
	char req[NANORPC_MAXLEN_REQ];
	char resp[NANORPC_MAXLEN_RESP];

	memset(req,0,sizeof(char)*NANORPC_MAXLEN_REQ);
	memset(resp,0,sizeof(char)*NANORPC_MAXLEN_RESP);

	RPCJsonReq jr(GetJsonReqID(),NJR_REQ_GET_TASKDESC);
	jr(req,NANORPC_MAXLEN_REQ);

	if(false == SendReqAndWaitForResp((void*)req,NANORPC_MAXLEN_REQ,resp,NANORPC_MAXLEN_RESP))
	{
		return false;
	}

	try
	{
		RPCJsonRespWithSuccess result(resp);
		GetTaskDescFromJsonResp(*task,result);
		return true;
	}
	catch(...)
	{
		RPCJsonRespWithFailure error(resp);
		LOGERROR("Error Code = %d : %s",error.GetErrorCode(),error.GetErrorMessage());
	}

	return false;
}

bool		RPCTaskMngrClient::UptTaskRslt(TaskResult* rslt)
{
	char req[NANORPC_MAXLEN_REQ];
	char resp[NANORPC_MAXLEN_RESP];

	memset(req,0,sizeof(char)*NANORPC_MAXLEN_REQ);
	memset(resp,0,sizeof(char)*NANORPC_MAXLEN_RESP);

	RPCJsonReq jr(GetJsonReqID(),NJR_REQ_UPT_TASKRSLT);
	FillJsonReqWithTaskRslt(jr,*rslt);
	jr(req,NANORPC_MAXLEN_REQ);

	if(false == SendReqAndWaitForResp((void*)req,NANORPC_MAXLEN_REQ,resp,NANORPC_MAXLEN_RESP))
	{
		return false;
	}

	try
	{
		RPCJsonRespWithSuccess result(resp);
		return true;
	}
	catch(...)
	{
		RPCJsonRespWithFailure error(resp);
		LOGERROR("Error Code = %d : %s",error.GetErrorCode(),error.GetErrorMessage());
	}

	return false;
}

bool		RPCTaskMngrClient::GetTaskRslt(TaskResult* rslt)
{
	char req[NANORPC_MAXLEN_REQ];
	char resp[NANORPC_MAXLEN_RESP];

	memset(req,0,sizeof(char)*NANORPC_MAXLEN_REQ);
	memset(resp,0,sizeof(char)*NANORPC_MAXLEN_RESP);

	RPCJsonReq jr(GetJsonReqID(),NJR_REQ_GET_TASKRSLT);
	jr(req,NANORPC_MAXLEN_REQ);

	if(false == SendReqAndWaitForResp((void*)req,NANORPC_MAXLEN_REQ,resp,NANORPC_MAXLEN_RESP))
	{
		return false;
	}

	try
	{
		RPCJsonRespWithSuccess result(resp);
		GetTaskRsltFromJsonResp(*rslt,result);
		return true;
	}
	catch(...)
	{
		RPCJsonRespWithFailure error(resp);
		LOGERROR("Error Code = %d : %s",error.GetErrorCode(),error.GetErrorMessage());
	}

	return false;
}

bool		RPCTaskMngrClient::DebugDump(void)
{
	char req[NANORPC_MAXLEN_REQ];
	char resp[NANORPC_MAXLEN_RESP];

	memset(req,0,sizeof(char)*NANORPC_MAXLEN_REQ);
	memset(resp,0,sizeof(char)*NANORPC_MAXLEN_RESP);

	RPCJsonReq jr(GetJsonReqID(),NJR_REQ_DEBUG_DUMP);
	jr(req,NANORPC_MAXLEN_REQ);

	if(false == SendReqAndWaitForResp((void*)req,NANORPC_MAXLEN_REQ,resp,NANORPC_MAXLEN_RESP))
	{
		return false;
	}

	try
	{
		RPCJsonRespWithSuccess result(resp);
		return true;
	}
	catch(...)
	{
		RPCJsonRespWithFailure error(resp);
		LOGERROR("Error Code = %d : %s",error.GetErrorCode(),error.GetErrorMessage());
	}

	return false;
}

bool		RPCTaskMngrClient::DumpTestResult(void)
{
	char req[NANORPC_MAXLEN_REQ];
	char resp[NANORPC_MAXLEN_RESP];

	memset(req,0,sizeof(char)*NANORPC_MAXLEN_REQ);
	memset(resp,0,sizeof(char)*NANORPC_MAXLEN_RESP);

	RPCJsonReq jr(GetJsonReqID(),NJR_REQ_DUMP_RESULT);
	jr(req,NANORPC_MAXLEN_REQ);

	if(false == SendReqAndWaitForResp((void*)req,NANORPC_MAXLEN_REQ,resp,NANORPC_MAXLEN_RESP))
	{
		return false;
	}

	try
	{
		RPCJsonRespWithSuccess result(resp);
		return true;
	}
	catch(...)
	{
		RPCJsonRespWithFailure error(resp);
		LOGERROR("Error Code = %d : %s",error.GetErrorCode(),error.GetErrorMessage());
	}

	return false;
}
