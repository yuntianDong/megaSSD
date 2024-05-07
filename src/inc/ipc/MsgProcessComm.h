#ifndef _MSG_PROCESS_COMM_H
#define _MSG_PROCESS_COMM_H

#include <stdint.h>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<boost::interprocess::message_queue> MsgQueuePtrType;
typedef boost::shared_ptr<boost::interprocess::mapped_region> MapRegionPtrType;
typedef boost::shared_ptr<boost::interprocess::shared_memory_object> SharedMemPtrType;

#define REQ_T_UNDEFINED				0x0
#define REQ_T_CLOSE					0xFFFF
#define	REQ_T_GET_RETURN_DATA		0xFFFE
#define REQ_T_ASYNC_FINISH			0xEEED
#define	REQ_T_SELF_TEST				0xFFE0

#define RESP_E_SUCCESS				0

#define RESP_E_INVALID_PARAM		-3
#define RESP_E_ASYNC_REQ			-5

#define RESP_D_SELFTEST				"SUCCEED"

#define VERSION_MAJOR				0
#define VERSION_MINOR				1
#define VERSION_BUILD				0
#define PROC_NAME_MAXLEN			40

#define	REQ_DATA_SIZE_MAX			1020
#define RESP_DATA_SIZE_MAX			1010

#define REQ_MSG_HEADER_LEN			8

typedef struct _stMsgReq
{
	uint16_t	reqType;
	uint16_t	slaveID;
	uint32_t	cmdSize;
	uint8_t		reqDatum[REQ_DATA_SIZE_MAX];
}stMsgReq;

typedef struct _stMsgResp
{
	uint32_t	version;
	int			errorCode;
	uint16_t	reqType;
	uint16_t	slaveID;
	uint16_t	dataSize;
	uint8_t		respDatum[RESP_DATA_SIZE_MAX];
}stMsgResp;

typedef bool (*CB_MSGPROCCOMM)(stMsgReq& req,stMsgResp& resp);

class MsgProcCommServer
{
private:
	SharedMemPtrType	mpSharedMem;
	MsgQueuePtrType		mpMsgQueue;

	char				mProcName[PROC_NAME_MAXLEN];

	void CreateMsgQueue(const char* proc,uint32_t reqSize,uint32_t maxReqs);
	void CreateSharedMem(const char* proc,uint32_t maxMemSize);
	void CreateMapRegion(void);
protected:
	uint32_t			mReqSize;
	uint32_t			mRespSize;
	uint16_t			mMaxSlaves;
	MapRegionPtrType	mpMapRegion;

	bool ReceiveRequest(void);
	virtual bool cb_ReceiveAndResponse(stMsgReq& req);
	virtual bool cb_RunCallbackRoutine(void);
	virtual uint32_t GetVersion(void)
	{
		return (uint32_t)VERSION_MAJOR << 24  |
				(uint32_t)VERSION_MINOR << 16 |
				(uint32_t)VERSION_BUILD;
	};

	void Initialize(const char* proc,uint32_t maxReqs,uint16_t numSlaves);
	void WaitReqReceived(uint16_t slaveID,uint16_t req);
public:
	MsgProcCommServer(void)
		:mReqSize(0),mRespSize(0),mMaxSlaves(0)
	{
	};
	MsgProcCommServer(const char* proc,uint32_t maxReqs,uint16_t numSlaves);
	virtual ~MsgProcCommServer(void);

	void Run(void);
};

class MsgProcCommClient
{
private:
	SharedMemPtrType	mpSharedMem;
	MapRegionPtrType	mpMapRegion;
	MsgQueuePtrType		mpMsgQueue;

	char				mProcName[PROC_NAME_MAXLEN];
	uint16_t			mSlaveID;
	uint32_t			mRespSize;

	bool				mbInitSuccess;
	bool				mbServcClosed;
protected:
	void Initialize(const char* proc,uint16_t slaveID);
	bool IsInitialized(void) {return mbInitSuccess;};
	bool IsServsClosed(void) {return mbServcClosed;};

	void OpenMsgQueue(const char* proc);
	void OpenSharedMem(const char* proc);
	void OpenMapRegion(uint16_t slaveID,uint32_t respSize);

	bool SendRequest(uint16_t reqType,void* pCmd,uint32_t cmdSize);
	bool TryGetResponse(uint16_t reqType,int& errCode,uint32_t& version,void* pRslt,uint32_t dataLen);
	bool WaitForResponse(uint16_t reqType,int& errCode,uint32_t& version,void* pRslt,uint32_t dataLen,uint32_t timeout);

	bool SendAsyncRequest(uint16_t reqType,void* input,uint32_t inLen,uint32_t timeout);
	bool WaitAsyncDone(void* output,uint32_t outLen,uint32_t timeout);
	bool SendSyncRequest(uint16_t reqType,void* input,void* output,uint32_t inLen,uint32_t outLen,uint32_t timeout);
public:
	MsgProcCommClient(void)
		:mSlaveID(0),mRespSize(0),mbInitSuccess(false),mbServcClosed(true)
	{
	};
	MsgProcCommClient(const char* proc,uint16_t slaveID);
	virtual ~MsgProcCommClient(void);

	bool IsGood(void) {return true == IsInitialized() && false == IsServsClosed();};

	bool DoSelfTest(void);
	bool CloseService(void);
};

#endif
