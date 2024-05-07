/*
 * file : MsgProcessComm.cpp
 */

#include <string.h>
#include <unistd.h>
#include "ipc/MsgProcessComm.h"
#include "utility/ClkTimer.h"
#include "Logging.h"

using namespace boost::interprocess;

#define INTERVAL_US					10000

#define NAME_POST_MSG_QUEUE			"MSG"
#define NAME_POST_SHARE_MEM			"MEM"

#define MSG_NAME_MAXLEN				(PROC_NAME_MAXLEN + 8)
#define MEM_NAME_MAXLEN				(PROC_NAME_MAXLEN + 8)

void MsgProcCommServer::Initialize(const char* proc,uint32_t maxReqs,uint16_t numSlaves)
{
	memcpy(mProcName,proc,PROC_NAME_MAXLEN);
	mMaxSlaves	= numSlaves;
	mReqSize	= sizeof(stMsgReq);
	mRespSize	= sizeof(stMsgResp);

	CreateMsgQueue(proc,mReqSize,maxReqs);
	CreateSharedMem(proc,mRespSize*numSlaves);
	CreateMapRegion();
}

void MsgProcCommServer::WaitReqReceived(uint16_t slaveID,uint16_t req)
{
	char* mem	= static_cast<char*>(mpMapRegion->get_address());
	stMsgResp*	resp = (stMsgResp*)(mem+mRespSize*slaveID);

	while(req != resp->reqType)
	{
		usleep(1000);
	}
}

MsgProcCommServer::MsgProcCommServer(const char* proc,uint32_t maxReqs,uint16_t numSlaves)
{
	Initialize(proc,maxReqs,numSlaves);
}

MsgProcCommServer::~MsgProcCommServer(void)
{
	char MsgName[MSG_NAME_MAXLEN];
	char MemName[MEM_NAME_MAXLEN];

	snprintf(MsgName,MSG_NAME_MAXLEN,"%s_%s",mProcName,NAME_POST_MSG_QUEUE);
	snprintf(MemName,MEM_NAME_MAXLEN,"%s_%s",mProcName,NAME_POST_SHARE_MEM);

	message_queue::remove(MsgName);
	shared_memory_object::remove(MemName);

	mpSharedMem.reset();
	mpMapRegion.reset();
	mpMsgQueue.reset();
}

void MsgProcCommServer::CreateMsgQueue(const char* proc,uint32_t reqSize,uint32_t maxReqs)
{
	char MsgName[MSG_NAME_MAXLEN];

	snprintf(MsgName,MSG_NAME_MAXLEN,"%s_%s",proc,NAME_POST_MSG_QUEUE);

	try{
		message_queue::remove(MsgName);

		mpMsgQueue	= MsgQueuePtrType(new message_queue(
				create_only,
				MsgName,
				maxReqs,
				reqSize
				));
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Create MsgQueue Failed.\n %s",ex.what());
	}
}

void MsgProcCommServer::CreateSharedMem(const char* proc,uint32_t maxMemSize)
{
	char MemName[MEM_NAME_MAXLEN];
	snprintf(MemName,MEM_NAME_MAXLEN,"%s_%s",proc,NAME_POST_SHARE_MEM);

	try
	{
		shared_memory_object::remove(MemName);

		mpSharedMem		= SharedMemPtrType(new shared_memory_object(
					create_only,
					MemName,
					read_write
					));

		mpSharedMem->truncate(maxMemSize);
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Create SharedMem Failed.\n %s",ex.what());
	}
}

void MsgProcCommServer::CreateMapRegion(void)
{
	try
	{
		mpMapRegion		= MapRegionPtrType(new mapped_region(
				*mpSharedMem,
				read_write
				));
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Create MapRegion Failed.\n %s",ex.what());
	}
}

bool MsgProcCommServer::ReceiveRequest(void)
{
	stMsgReq req;
	memset(&req,0,sizeof(stMsgReq));

    unsigned int priority;
    message_queue::size_type recvd_size;

    bool	rtn = false;
    try
    {
    	rtn = mpMsgQueue->try_receive(&req,sizeof(stMsgReq),recvd_size,priority);
    }
	catch(interprocess_exception &ex)
	{
		LOGERROR("Receive Request Failed.\n %s",ex.what());
		return false;
	}

	if(false == rtn)
	{
		return true;
	}

	if(recvd_size != (req.cmdSize + REQ_MSG_HEADER_LEN))
	{
		LOGERROR("Invalid Request Received!");
		return false;
	}

	return this->cb_ReceiveAndResponse(req);
}

bool MsgProcCommServer::cb_ReceiveAndResponse(stMsgReq& req)
{
	uint16_t	slaveID	= req.slaveID;
	uint16_t	reqType = req.reqType;

	LOGDEBUG("%04d - %d\n",slaveID,reqType);

	if(slaveID >= mMaxSlaves)
	{
		LOGERROR("Invalid SlaveID");
		return false;
	}

	char* mem	= static_cast<char*>(mpMapRegion->get_address());
	stMsgResp*	resp = (stMsgResp*)(mem+mRespSize*slaveID);
	memset(resp,0,sizeof(resp));

	switch(reqType)
	{
		case REQ_T_GET_RETURN_DATA:
		{
			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_SELF_TEST:
		{
			resp->errorCode	= RESP_E_SUCCESS;
			resp->dataSize	= strlen(RESP_D_SELFTEST)+1;

			memcpy(resp->respDatum,RESP_D_SELFTEST,resp->dataSize);
			break;
		}
		case REQ_T_CLOSE:
			LOGINFO("Stopped!");
			return false;
		default:
			LOGERROR("Invalid Request Type = %d",reqType);
			return false;
	}

	resp->version	= GetVersion();
	resp->slaveID	= slaveID;
	resp->reqType	= reqType;

	return true;
}

bool MsgProcCommServer::cb_RunCallbackRoutine(void)
{
	usleep(INTERVAL_US);
	return true;
}

void MsgProcCommServer::Run(void)
{
	while(true)
	{
		if(false == ReceiveRequest())
		{
			LOGERROR("Fail to Call ReceiveRequest()");
			break;
		}

		if(false == cb_RunCallbackRoutine())
		{
			LOGERROR("Fail to Call cb_RunCallbackRoutine()");
			break;
		}
	}
}

void MsgProcCommClient::Initialize(const char* proc,uint16_t slaveID)
{
	memcpy(mProcName,proc,PROC_NAME_MAXLEN);

	mSlaveID	= slaveID;
	mRespSize	= sizeof(stMsgResp);

	mbInitSuccess	= true;
	mbServcClosed	= false;

	OpenMsgQueue(proc);
	OpenSharedMem(proc);
	OpenMapRegion(slaveID,mRespSize);
}

MsgProcCommClient::MsgProcCommClient(const char* proc,uint16_t slaveID)
{
	Initialize(proc,slaveID);
}

MsgProcCommClient::~MsgProcCommClient(void)
{
	mpSharedMem.reset();
	mpMapRegion.reset();
	mpMsgQueue.reset();
}

void MsgProcCommClient::OpenMsgQueue(const char* proc)
{
	char MsgName[MSG_NAME_MAXLEN];

	snprintf(MsgName,MSG_NAME_MAXLEN,"%s_%s",proc,NAME_POST_MSG_QUEUE);

	try{
		mpMsgQueue	= MsgQueuePtrType(new message_queue(
				open_only,
				MsgName
				));
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Open MsgQueue Failed.\n %s",ex.what());
		mbInitSuccess	= false;
	}
}

void MsgProcCommClient::OpenSharedMem(const char* proc)
{
	char MemName[MEM_NAME_MAXLEN];
	snprintf(MemName,MEM_NAME_MAXLEN,"%s_%s",proc,NAME_POST_SHARE_MEM);

	try
	{
		mpSharedMem		= SharedMemPtrType(new shared_memory_object(
					open_only,
					MemName,
					read_only
					));
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Open SharedMem Failed.\n %s",ex.what());
		mbInitSuccess	= false;
	}
}

void MsgProcCommClient::OpenMapRegion(uint16_t slaveID,uint32_t respSize)
{
	if(false == mbInitSuccess)
	{
		return;
	}

	try
	{
		mpMapRegion		= MapRegionPtrType(new mapped_region(
				*mpSharedMem,
				read_only,
				slaveID*respSize,
				respSize
				));
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Create MapRegion Failed.\n %s",ex.what());
	}
}

bool MsgProcCommClient::SendRequest(uint16_t reqType,void* pCmd,uint32_t cmdSize)
{
	if(false == IsInitialized() || true == IsServsClosed())
	{
		return false;
	}

	stMsgReq req;

	memset(&req,0,sizeof(stMsgReq));

	req.reqType		= reqType;
	req.slaveID		= mSlaveID;
	uint32_t xferSzie	= REQ_MSG_HEADER_LEN;
	if(NULL != pCmd && 0 < cmdSize)
	{
		req.cmdSize		= cmdSize;
		memcpy(req.reqDatum,(char*)pCmd,cmdSize);
		xferSzie += cmdSize;
	}

	try
	{
		mpMsgQueue->send(&req,xferSzie,0);
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Send Request Failed.\n %s",ex.what());
		return false;
	}

	return true;
}

bool MsgProcCommClient::TryGetResponse(uint16_t reqType,int& errCode,uint32_t& version,void* pRslt,uint32_t dataLen)
{
	if(false == IsInitialized())
	{
		return false;
	}

	void * buf = mpMapRegion->get_address();
	stMsgResp* resp	= static_cast<stMsgResp*>(buf);

	LOGDEBUG("resp->version = %x,resp->reqType = %x",resp->version,resp->reqType);
	if(0 == resp->version || reqType != resp->reqType)
	{
		return false;
	}

	if(mSlaveID != resp->slaveID)
	{
		LOGERROR("SlaveID is not correct! Exp = %d,Act = %d",mSlaveID,resp->slaveID);
		return false;
	}

	if(NULL != pRslt && 0 < dataLen)
	{
		memcpy(pRslt,resp->respDatum,(dataLen > resp->dataSize)?resp->dataSize:dataLen);
	}

	errCode	= resp->errorCode;
	version	= resp->version;

	return true;
}

bool MsgProcCommClient::WaitForResponse(uint16_t reqType,int& errCode,uint32_t& version,void* pRslt,uint32_t dataLen,uint32_t timeout)
{
	if(false == IsInitialized())
	{
		return false;
	}

	uint64_t	eclapsed = 0;
	while( (0 == timeout) || (eclapsed < (uint64_t)timeout*USEC_PER_SEC))
	{
		SysClkTimer timer(eclapsed);

		if(true == TryGetResponse(reqType,errCode,version,pRslt,dataLen))
		{
			return true;
		}

		usleep(10000);
	}

	return false;
}

bool MsgProcCommClient::SendAsyncRequest(uint16_t reqType,void* input,uint32_t inLen,uint32_t timeout)
{
	if(false == SendRequest(reqType,input,inLen))
	{
		LOGERROR("Fail to SendRequest");
		return false;
	}

	int errCode = 0;
	uint32_t ver = 0;
	if(false == WaitForResponse(reqType,errCode,ver,NULL,0,timeout))
	{
		LOGERROR("Fail to WaitForResponse reqType=%x",reqType);
		return false;
	}

	if(RESP_E_ASYNC_REQ != errCode)
	{
		LOGERROR("Error Code = %d",errCode);
		return false;
	}

	if(false == SendRequest(REQ_T_GET_RETURN_DATA,NULL,0))
	{
		LOGERROR("Fail to Clean Return Data");
		return false;
	}

	if(false == WaitForResponse(REQ_T_GET_RETURN_DATA,errCode,ver,NULL,0,timeout))
	{
		LOGERROR("Fail to WaitForResponse reqType=REQ_T_GET_RETURN_DATA");
		return false;
	}

	if(RESP_E_SUCCESS != errCode)
	{
		LOGERROR("Error Code = %d",errCode);
		return false;
	}

	return true;
}

bool MsgProcCommClient::WaitAsyncDone(void* output,uint32_t outLen,uint32_t timeout)
{
	int errCode = 0;
	uint32_t ver = 0;

	if(false == WaitForResponse(REQ_T_ASYNC_FINISH,errCode,ver,output,outLen,timeout))
	{
		LOGERROR("Fail to WaitForResponse : reqType = REQ_T_ASYNC_FINISH");
		return false;
	}

	if(RESP_E_SUCCESS != errCode)
	{
		LOGERROR("Error Code = %d",errCode);
		return false;
	}

	if(false == SendRequest(REQ_T_GET_RETURN_DATA,NULL,0))
	{
		LOGERROR("Fail to Clean Return Data");
		return false;
	}

	if(false == WaitForResponse(REQ_T_GET_RETURN_DATA,errCode,ver,NULL,0,timeout))
	{
		LOGERROR("Fail to WaitForResponse : reqType = REQ_T_GET_RETURN_DATA");
		return false;
	}

	if(RESP_E_SUCCESS != errCode)
	{
		LOGERROR("Error Code = %d",errCode);
		return false;
	}

	return true;
}

bool MsgProcCommClient::SendSyncRequest(uint16_t reqType,void* input,void* output,uint32_t inLen,uint32_t outLen,uint32_t timeout)
{
	if(false == SendRequest(reqType,input,inLen))
	{
		LOGERROR("Fail to SendRequest");
		return false;
	}

	int errCode = 0;
	uint32_t ver = 0;
	if(false == WaitForResponse(reqType,errCode,ver,output,outLen,timeout))
	{
		LOGERROR("Fail to WaitForResponse : reqType = %x",reqType);
		return false;
	}

	if(RESP_E_SUCCESS != errCode)
	{
		LOGERROR("Error Code = %d",errCode);
		return false;
	}

	if(false == SendRequest(REQ_T_GET_RETURN_DATA,NULL,0))
	{
		LOGERROR("Fail to Clean Return Data");
		return false;
	}

	if(false == WaitForResponse(REQ_T_GET_RETURN_DATA,errCode,ver,NULL,0,timeout))
	{
		LOGERROR("Fail to WaitForResponse : reqType = REQ_T_GET_RETURN_DATA");
		return false;
	}

	if(RESP_E_SUCCESS != errCode)
	{
		LOGERROR("Error Code = %d",errCode);
		return false;
	}

	return true;
}

bool MsgProcCommClient::CloseService(void)
{
	if(false == SendRequest(REQ_T_CLOSE,NULL,0))
	{
		return false;
	}

	mbServcClosed	= true;
	return true;
}

bool MsgProcCommClient::DoSelfTest(void)
{
	char rtnDatum[10];

	if(false == SendSyncRequest(REQ_T_SELF_TEST,NULL,rtnDatum,0,10,8))
	{
		return false;
	}

	LOGINFO("rtnDatum : %s",rtnDatum);

	return true;
}
