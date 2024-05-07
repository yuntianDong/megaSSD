/*
 * file : NanoJsonRPC.cpp
 */

#include "net/NanoJsonRPC.h"
#include "net/RPCJson.h"

bool NanoJsonRPCServer::PreHandleCheck(void* req,int reqSize,void* resp,int respSize)
{
	RPCJsonReq*	pJReq 	= NULL;
	char *		method 	= NULL;
	int			reqID	= 0;
	char *		version	= NULL;

	try
	{
		pJReq	= new RPCJsonReq((const char*)req);
	}
	catch(...)
	{
		RPCJsonErrParseError error(0);
		CopyJsonRespToBuf<RPCJsonErrParseError>(error,resp,respSize);
		LOGERROR("Invalid Json Format");
		return false;
	}

	try
	{
		reqID	= pJReq->GetID();
		method	= (char*)(pJReq->GetMethod());
		version	= (char*)(pJReq->GetVersion());
	}
	catch(...)
	{
		RPCJsonErrInvalidReq error(0);
		CopyJsonRespToBuf<RPCJsonErrInvalidReq>(error,resp,respSize);
		LOGERROR("Invalid JSONRPC Request");
		return false;
	}

	if(0 != strcmp(version,RPCJSON_VERSION))
	{
		RPCJsonErrInvalidReq error(0);
		CopyJsonRespToBuf<RPCJsonErrInvalidReq>(error,resp,respSize);
		LOGERROR("JSONRPC Version Mismatch");
		return false;
	}

	return true;
}

int NanoJsonRPCServer::HandleRoutine(void* req,int reqSize,void* resp,int respSize)
{
	if(false == PreHandleCheck(req,reqSize,resp,respSize))
	{
		return NANORPC_SRV_RERR_SUCCESS;
	}

	RPCJsonReq	jReq((const char*)req);
	const char* method	= jReq.GetMethod();
	int	reqId			= jReq.GetID();

	if(0 == strcmp(method,NJR_REQ_CLOSE_SERVICE))
	{
		RPCJsonRespWithSuccess msg(reqId);
		CopyJsonRespToBuf<RPCJsonRespWithSuccess>(msg,resp,respSize);
		return NANORPC_SRV_RERR_CANCELLED;
	}
	else if(0 == strcmp(method,NJR_REQ_GET_VERSION))
	{
		char Version[NANORPC_VERSION_STRLEN];
		snprintf(Version,NANORPC_VERSION_STRLEN,NANORPC_VERSION_FORMAT,\
						NANORPC_VERSION_MAJOR,NANORPC_VERSION_MINOR,NANORPC_VERSION_BUILD);

		RPCJsonRespWithSuccess msg(reqId);
		msg.AddResult<std::string>("Version",Version);
		CopyJsonRespToBuf<RPCJsonRespWithSuccess>(msg,resp,respSize);
	}
	else
	{
		RPCJsonErrMethodNotFound error(reqId);
		CopyJsonRespToBuf<RPCJsonErrMethodNotFound>(error,resp,respSize);
	}

	return NANORPC_SRV_RERR_SUCCESS;
}

const char* NanoJsonRPCClient::GetVersion(void)
{
	char req[NANORPC_MAXLEN_REQ];
	char resp[NANORPC_MAXLEN_RESP];

	memset(req,0,sizeof(char)*NANORPC_MAXLEN_REQ);
	memset(resp,0,sizeof(char)*NANORPC_MAXLEN_RESP);

	RPCJsonReq jr(GetJsonReqID(),NJR_REQ_GET_VERSION);
	jr(req,NANORPC_MAXLEN_REQ);

	if(false == SendReqAndWaitForResp((void*)req,NANORPC_MAXLEN_REQ,resp,NANORPC_MAXLEN_RESP))
	{
		return NanoRPCClient::GetVersion();
	}

	try
	{
		RPCJsonRespWithSuccess result(resp);
		return result.GetResult<std::string>("Version").c_str();
	}
	catch(...)
	{
		RPCJsonRespWithFailure error(resp);
		LOGERROR("Error Code = %d : %s",error.GetErrorCode(),error.GetErrorMessage());
	}

	return "";
}

bool NanoJsonRPCClient::CloseService(void)
{
	char req[NANORPC_MAXLEN_REQ];
	char resp[NANORPC_MAXLEN_RESP];

	memset(req,0,sizeof(char)*NANORPC_MAXLEN_REQ);
	memset(resp,0,sizeof(char)*NANORPC_MAXLEN_RESP);

	RPCJsonReq jr(GetJsonReqID(),NJR_REQ_CLOSE_SERVICE);
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
