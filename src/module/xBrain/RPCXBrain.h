#ifndef _RPC_XBRAIN_H
#define _RPC_XBRAIN_H

#include "IPCTaskMngr.h"
#include "RPCTaskMngr.h"

#define DEF_HOSTNAME		"localhost"

#define HOST_ADDR_LEN		32

class RPCXBrainServer : public RPCTaskMngrServer
{
public:
	RPCXBrainServer(const char* addr)
		:RPCTaskMngrServer(addr)
	{};
};

class RPCXBrainClient : public IPCTaskMngrServer,public RPCTaskMngrClient
{
private:
	char	mHostAddr[HOST_ADDR_LEN];
protected:
	virtual bool cb_RunCallbackRoutine(void);
	virtual bool UpdateTaskResult(uint16_t taskID,uint8_t slotID);
	uint16_t	GetGlobalTaskID(uint16_t taskID);
public:
	RPCXBrainClient(const char* addr,uint8_t index)
		:IPCTaskMngrServer(index),RPCTaskMngrClient(addr)
	{
		memcpy(mHostAddr,DEF_HOSTNAME,HOST_ADDR_LEN);
	};

	void SetHostID(const char* name) {strncpy(mHostAddr,name,HOST_ADDR_LEN);};
	bool AddSlot(uint8_t slotID) {return ReqHdlr_AddSlot(slotID);};
};

#endif
