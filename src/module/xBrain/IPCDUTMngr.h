#ifndef _IPC_DUT_MNGR_H
#define _IPC_DUT_MNGR_H

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include "test/DUTInfo.h"
#include "ipc/MsgProcessComm.h"

#define	DUT_DEVNAME_MAXLEN			40
#define DUT_EXECTASK_MAXLEN			240

#define REQ_T_IDENTIFY				0x8000
#define REQ_T_GET_DUTINFO			0x8001
#define REQ_T_GET_DUTSTATUS			0x8002

#define REQ_T_EXEC_TASK				0x8010
#define REQ_T_GET_LASTRESULT		0x8011

#define RESP_E_GETINFO_ERROR		-100

#define DUTMNGT_PROC_NAME			"xBrainDutMngr"
#define DUTMNGT_PROC_FORMAT			"%s_%d"
#define DUTMNGT_MAX_REQS			20
#define DUTMNGT_MAX_SLAVES			4
#define DUTMNGT_MAX_SLOTID			32
#define DUTMNGT_RESP_TIMEOUT		60

#define DUTMNGT_VERSION_MAJOR		0
#define DUTMNGT_VERSION_MINOR		1
#define DUTMNGT_VERSION_BUILD		0

typedef enum _enDutStatus
{
	DUT_S_IDLE,
	DUT_S_BUSY,
	DUT_S_ERROR,
	DUT_S_UNKNOWN,
}enDutStatus;

typedef enum _enDutType
{
	DUT_T_NVME,
	DUT_T_UNKNOWN,
}enDutType;

class IPCDUTMngrServer : public MsgProcCommServer
{
private:
	DutInfo*	mpDutInfo;
	enDutStatus	mDutStatus;
	char		mDevName[DUT_DEVNAME_MAXLEN];
	uint8_t		mSlotID;
	int			mLastResult;

	enDutType	GetDutType(void);
	stDutInfo*	GetDutInfo(void);

	boost::thread*		mpSlotThread[DUTMNGT_MAX_SLAVES];
	void	_TrdExecShell(uint16_t slaveID,const char* cmdline);
protected:
	virtual bool cb_ReceiveAndResponse(stMsgReq& req);
	virtual uint32_t GetVersion(void)
	{
		return (uint32_t)DUTMNGT_VERSION_MAJOR << 24  |
				(uint32_t)DUTMNGT_VERSION_MINOR << 16 |
				(uint32_t)DUTMNGT_VERSION_BUILD;
	};
	void PrintSlotInfo(void);
	const char* GetStatusString(void);
public:
	static IPCDUTMngrServer*	gInstances[DUTMNGT_MAX_SLOTID];
	static IPCDUTMngrServer*	GetInstance(const char* devname,uint8_t slotID);

	IPCDUTMngrServer(const char* devname,uint8_t slotID);
	virtual ~IPCDUTMngrServer(void);
};

class IPCDUTMngrClient : public MsgProcCommClient
{
public:
	IPCDUTMngrClient(uint16_t slotID,uint8_t slaveID=0);
	virtual ~IPCDUTMngrClient(void);

	DutInfo*	GetDutInfo(void);
	enDutStatus	GetDutStatus(void);
	int			GetLastResult(void);

	bool		Identify(void);
	bool		ExecTask(const char* cmdline);
	bool		WaitTaskDone(uint32_t timeout=0);
};

IPCDUTMngrServer*	GetDutMngrServer(const char* devname,uint8_t slotID);

#endif
