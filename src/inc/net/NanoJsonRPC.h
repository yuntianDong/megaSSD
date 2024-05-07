#ifndef _NANO_JSON_RPC_H
#define _NANO_JSON_RPC_H

#include <assert.h>
#include <stdint.h>
#include <ctime>
#include <cstdlib>
#include "NanoRPC.h"

#define NJR_REQ_GET_VERSION		"Version"
#define NJR_REQ_CLOSE_SERVICE	"CloseService"

#define JSON_ID_PREFIX_OFFSET		16
#define JSON_ID_POSTFIX_BITMASK		0xFFFF

class NanoJsonRPCServer : public NanoRPCServer
{
protected:
	template <typename T>
	void CopyJsonRespToBuf(T& resp,void* buf,int length)
	{
		resp((char*)buf,length);
	};
	virtual bool PreHandleCheck(void* req,int reqSize,void* resp,int respSize);
	virtual int HandleRoutine(void* req,int reqSize,void* resp,int respSize);
public:
	NanoJsonRPCServer(const char* addr)
		:NanoRPCServer(addr)
	{};
};

class NanoJsonRPCClient : public NanoRPCClient
{
private:
	uint16_t	mReqIndex;
	uint16_t	mID;
protected:
	uint32_t	GetJsonReqID(void)
	{
		uint32_t tmpVal = (mID << JSON_ID_PREFIX_OFFSET) | (mReqIndex & JSON_ID_POSTFIX_BITMASK);
		mReqIndex += 1;

		return tmpVal;
	};
	uint16_t	GetRandNum(void)
	{
		srand(time(0));
		return (uint16_t)rand();
	}
public:
	NanoJsonRPCClient(const char* addr)
		:NanoRPCClient(addr),mReqIndex(0)
	{
		mID = GetRandNum();
	};

	const char* GetVersion(void);
	bool CloseService(void);
};

#endif
