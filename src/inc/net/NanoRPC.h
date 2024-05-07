#ifndef _NANO_RPC_H
#define _NANO_RPC_H

#include "RPCJson.h"
#include "NanomsgNetComm.h"
#include "Logging.h"

#define NANORPC_SRV_RERR_SUCCESS			0
#define NANORPC_SRV_RERR_REQEST				-1
#define NANORPC_SRV_RERR_RESPONSE			-2
#define NANORPC_SRV_RERR_CANCELLED			-3
#define NANORPC_SRV_RERR_NOTSUPPORTED		-4

#define NANORPC_SEND_TIMEOUT				60000
#define NANORPC_RECV_TIMEOUT				60000
#define NANORPC_RESEND_TIMEOUT				8000
#define NANORPC_MAXLEN_REQ					4096
#define NANORPC_MAXLEN_RESP					4096
#define NANORPC_MINLEN_REQ					16

#define NANORPC_VERSION_MAJOR				0
#define NANORPC_VERSION_MINOR				1
#define NANORPC_VERSION_BUILD				0
#define NANORPC_VERSION_FORMAT				"%02x%02x%04x"
#define NANORPC_VERSION_STRLEN				12

class NanoRPCServer : public NanoMsgRPCServer
{
protected:
	virtual int SendResponse(void* resp,int length)
	{
		return SendDatum(resp,length);
	};

	virtual int GetRequest(void* req,int length)
	{
		return RecvDatum(req,length);
	};

	virtual int HandleRoutine(void* req,int reqSize,void* resp,int respSize)
	{
		return NANORPC_SRV_RERR_NOTSUPPORTED;
	};

	virtual int RunCallBackRoutine(void)
	{
		return NANORPC_SRV_RERR_SUCCESS;
	};

	int RecvReqAndSendResp(void)
	{
		uint8_t req[NANORPC_MAXLEN_REQ];
		uint8_t resp[NANORPC_MAXLEN_RESP];

		memset(req,0,sizeof(uint8_t)*NANORPC_MAXLEN_REQ);
		memset(resp,0,sizeof(uint8_t)*NANORPC_MAXLEN_RESP);

		if(0 >= GetRequest(req,NANORPC_MAXLEN_REQ))
		{
			LOGERROR("Fail to GetRequest()");
			return NANORPC_SRV_RERR_REQEST;
		}

		sleep(1);

		LOGDEBUG("GetRequest() = %s",(char *)req);

		int rtnCode = HandleRoutine(req,NANORPC_MAXLEN_REQ,resp,NANORPC_MAXLEN_RESP);

		LOGDEBUG("SendResponse() = %s",(char *)resp);

		if(0 >= SendResponse(resp,NANORPC_MAXLEN_RESP))
		{
			LOGERROR("Fail to SendResponse()");
			return NANORPC_SRV_RERR_RESPONSE;
		}

		return rtnCode;
	}
public:
	NanoRPCServer(const char* addr)
		:NanoMsgRPCServer(addr)
	{
	};

	bool Run(void)
	{
		while(true)
		{
			int rtnCode = RecvReqAndSendResp();
			if(NANORPC_SRV_RERR_SUCCESS != rtnCode)
			{
				LOGERROR("RecvReqAndSendResp() RtnCode = %d",rtnCode);
				return rtnCode == NANORPC_SRV_RERR_CANCELLED;
			}

			rtnCode = RunCallBackRoutine();
			if(NANORPC_SRV_RERR_SUCCESS != rtnCode)
			{
				LOGERROR("RunCallBackRoutine() RtnCode = %d",rtnCode);
				return rtnCode == NANORPC_SRV_RERR_CANCELLED;
			}
		}
	};
};

class NanoRPCClient : public NanoMsgRPCClient
{
protected:
	virtual bool SendRequest(void* req,int length)
	{
		return 0 < SendDatum(req,length);
	};

	virtual bool GetResponse(void* resp,int length)
	{
		return 0 < RecvDatum(resp,length);
	};

	bool SendReqAndWaitForResp(void* req,int reqSize,void* resp,int respSize)
	{
		LOGDEBUG("SendReqAndWaitForResp() req = %s",(char*)req);

		if(false == SendRequest(req,reqSize))
		{
			LOGERROR("Fail to SendRequest()");
			return false;
		}

		if(false == GetResponse(resp,respSize))
		{
			LOGERROR("Fail to GetResponse()");
			return false;
		}

		return true;
	};
public:
	NanoRPCClient(const char* addr)
		:NanoMsgRPCClient(addr)
	{
		SetRPCTimeout(NANORPC_SEND_TIMEOUT,NANORPC_RECV_TIMEOUT);
		SetRPCSendTO(NANORPC_RESEND_TIMEOUT);
	};

	void SetRPCTimeout(int sendTO,int recvTO)
	{
		SetOptSendTimeOut(sendTO);
		SetOptRecvTimeOut(recvTO);
	}

	void SetRPCSendTO(int timeout)
	{
		SetOptResendIVL(timeout);
	}

	virtual const char* GetVersion(void)
	{
		char Version[NANORPC_VERSION_STRLEN];
		snprintf(Version,NANORPC_VERSION_STRLEN,NANORPC_VERSION_FORMAT,\
				NANORPC_VERSION_MAJOR,NANORPC_VERSION_MINOR,NANORPC_VERSION_BUILD);

		return Version;
	};
};

#endif
