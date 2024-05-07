#ifndef _RPC_TASKMNGR_H
#define _RPC_TASKMNGR_H

#include "test/TaskMngr.h"
#include "net/NanoJsonRPC.h"
#include "ipc/FileMappingSharedMem.h"

#define NJR_REQ_GET_TOTALCNT		"GetTotalCount"
#define NJR_REQ_GET_LEFTCNT			"GetLeftCount"
#define NJR_REQ_ADD_TASKDESC		"AddTaskDesc"
#define NJR_REQ_GET_TASKDESC		"GetTaskDesc"
#define NJR_REQ_UPT_TASKRSLT		"UptTaskRslt"
#define NJR_REQ_GET_TASKRSLT		"GetTaskRslt"
#define NJR_REQ_DEBUG_DUMP			"DebugDump"
#define NJR_REQ_DUMP_RESULT			"DumpDebugRslt"

#define NJR_TASKMNGR_FILE_DUMP			"/tmp/RPCTaskMngr.bin"
#define NJR_TASKMNGR_TEST_DETAIL		"TestResult.csv"
#define NJR_TASKMNGR_TEST_REPORT		"TestReport.log"
#define NJR_TASKMNGR_FILE_SIZE			16*1024*1024

class RPCTaskMngrServer : public NanoJsonRPCServer,public FileMapSharedMem
{
private:
	TaskMngr*			mpTaskMngr;
protected:
	virtual int HandleRoutine(void* req,int reqSize,void* resp,int respSize);
	virtual int RunCallBackRoutine(void);
	bool	IsCycleDone(void);
	void	DumpTestResult(void);
public:
	RPCTaskMngrServer(const char* addr);
	virtual ~RPCTaskMngrServer(void);

	bool	AddTask(TaskDesc* task);
	bool	AddRslt(TaskResult* rslt);
	bool	Reset(void);
};

class RPCTaskMngrClient : public NanoJsonRPCClient
{
public:
	RPCTaskMngrClient(const char* addr)
		:NanoJsonRPCClient(addr)
	{};

	uint16_t	GetTotalCount(void);
	uint16_t	GetLeftCount(void);

	bool		DumpTestResult(void);
	bool		DebugDump(void);
	bool		AddTaskDesc(TaskDesc* task);
	bool		GetTaskDesc(TaskDesc* task);
	bool		UptTaskRslt(TaskResult* rslt);
	bool		GetTaskRslt(TaskResult* rslt);
};

#endif
