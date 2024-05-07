#ifndef _JSONRPC_X_TASKMNGR_H
#define _JSONRPC_X_TASKMNGR_H

#include "test/TaskMngr.h"
#include "RPCJson.h"

void FillJsonReqWithTaskDesc(RPCJsonReq& req,TaskDesc& task);
void FillJsonReqWithTaskRslt(RPCJsonReq& req,TaskResult& rslt);
void GetTaskDescFromJsonReq(TaskDesc& task,RPCJsonReq& req);
void GetTaskRsltFromJsonReq(TaskResult& rslt,RPCJsonReq& req);
void FillJsonRespWithTaskDesc(RPCJsonRespWithSuccess& resp,TaskDesc& task);
void FillJsonRespWithTaskRslt(RPCJsonRespWithSuccess& resp,TaskResult& rslt);
void GetTaskDescFromJsonResp(TaskDesc& task,RPCJsonRespWithSuccess& resp);
void GetTaskRsltFromJsonResp(TaskResult& rslt,RPCJsonRespWithSuccess& resp);

#endif
