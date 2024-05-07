#ifndef _IPC_TASK_MNGR_H
#define _IPC_TASK_MNGR_H

#include "test/TaskMngr.h"
#include "ipc/MsgProcessComm.h"
#include "ipc/FileMappingSharedMem.h"

#include "IPCDUTMngr.h"

#define REQ_T_DEBUGDUMP				0x81FF

#define REQ_T_RESET					0x8100
#define REQ_T_ADDTASK				0x8101
#define REQ_T_GETPENDINGTASKCNT		0x8102
#define REQ_T_PRTTESTREPORT			0x8105
#define REQ_T_DUMPTESTREPORT		0x8106
#define REQ_T_PRTTESTRESULT			0x8107
#define REQ_T_DUMPTESTRESULT		0x8108

#define REQ_T_ADDSLOT				0x8110
#define REQ_T_DELSLOT				0x8111
#define REQ_T_GETDEVINFO			0x8112
#define REQ_T_GETDEVSTATUS			0x8113
#define REQ_T_IDFYSLOT				0x8116
#define REQ_T_IDFYALLSLOT			0x8117
#define REQ_T_CHECKSLOTAVAIL		0x8118
#define REQ_T_GETAVAILSLOTS			0x8119
#define REQ_T_DELALLSLOT			0x811A

#define RESP_E_FAIL_ADDSLOT			-200
#define RESP_E_FAIL_GETDEVINFO		-201
#define RESP_E_FAIL_GETDEVSTATUS	-202
#define RESP_E_FAIL_CHECKSLOTAVAIL	-203
#define	RESP_E_FAIL_IDFYSLOT		-205
#define RESP_E_FAIL_ADDTASK			-206

#define TASKMNGR_PROC_NAME			"xBrainTaskMngr"
#define TASKMNGR_PROC_FORMAT		"%s_%d"
#define TASKMNGR_MAX_REQS			20
#define TASKMNGR_MAX_SLAVES			4
#define TASKMNGR_MAX_SLOTID			32
#define TASKMNGR_RESP_TIMEOUT		60
#define TASKMNGR_MAX_SERVERS		4

#define TASKMNGR_VERSION_MAJOR		0
#define TASKMNGR_VERSION_MINOR		1
#define TASKMNGR_VERSION_BUILD		0

#define	MAX_SLOT_NUM				32
#define TASKMNGR_FILE_DUMP			"/tmp/TaskMngr.bin"
#define TASKMNGR_FILE_SIZE			16*1024*1024

class IPCTaskMngrServer : public MsgProcCommServer,public FileMapSharedMem
{
protected:
	uint8_t				mIndex;
	TaskMngr*			mpTaskMngr;
	IPCDUTMngrClient*	mpDutClients[MAX_SLOT_NUM];
	uint16_t			mSlotTaskID[MAX_SLOT_NUM];
	uint32_t			mSlotLoopCnt[MAX_SLOT_NUM];
	uint8_t				mLastOpSlotID;

	void		DebugDump(void);

	bool		ReqHdlr_AddTask(stTaskDesc*	task);
	bool		ReqHdlr_PrintReport(void);
	bool		ReqHdlr_DumpTestReport(const char* filename);
	bool		ReqHdlr_PrintResult(void);
	bool		ReqHdlr_DumpTestResult(const char* filename);
	bool		ReqHdlr_AddSlot(uint8_t slotID);
	bool		ReqHdlr_DelSlot(uint8_t slotID);
	bool		ReqHdlr_DelAllSlot(void);
	bool		ReqHdlr_IdfySlot(uint8_t slotID);
	bool		ReqHdlr_IdfyAllSlots(void);

	uint8_t		GetAvailSlotCount(void);
	void		GetAllAvailSlotIDs(uint8_t*& psIDLst);
	uint8_t		GetNextIdleSlotID(void);
	bool		GetSlotStatus(uint8_t slotID,enDutStatus& status);
	bool		GetSlotDevInfo(uint8_t slotID,DutInfo*& info);
	bool		GetSlotLastResult(uint8_t slotID,int& rslt);
	bool		GetSlotPhyAddr(uint8_t slotID,char*& phyAddr);
	bool		ExecCmdLineOnSlot(uint8_t slotID,const char* cmdline);
	bool		IdentifySlot(uint8_t slotID);

	uint16_t	GetSlotTaskID(uint8_t slotID) {return mSlotTaskID[slotID];};
	uint32_t	GetSlotLeftLoops(uint8_t slotID) {return mSlotLoopCnt[slotID];};
	void		SetSlotTaskID(uint8_t slotID,uint16_t taskID) {mSlotTaskID[slotID]	= taskID;};
	void		SetSlotLeftLoops(uint8_t slotID,uint32_t loopCnt) {mSlotLoopCnt[slotID] = loopCnt;};

	virtual bool AsignTaskToSlot(uint16_t taskID,uint8_t slotID);
	virtual bool UpdateTaskResult(uint16_t taskID,uint8_t slotID);

	virtual bool cb_ReceiveAndResponse(stMsgReq& req);
	virtual bool cb_RunCallbackRoutine(void);
	virtual uint32_t GetVersion(void)
	{
		return (uint32_t)TASKMNGR_VERSION_MAJOR << 24  |
				(uint32_t)TASKMNGR_VERSION_MINOR << 16 |
				(uint32_t)TASKMNGR_VERSION_BUILD;
	};
public:
	static IPCTaskMngrServer*	gInstance[TASKMNGR_MAX_SERVERS];
	static IPCTaskMngrServer*	GetInstance(uint8_t index);

	IPCTaskMngrServer(uint8_t index);
	virtual ~IPCTaskMngrServer(void);

	bool Reset(void)
	{
		mpTaskMngr->Reset();
		return Flush();
	};
};

class IPCTaskMngrClient : public MsgProcCommClient
{
public:
	static IPCTaskMngrClient*	gInstance[TASKMNGR_MAX_SLAVES];
	static IPCTaskMngrClient*	GetInstance(uint8_t svrIdx,uint8_t slaveID);

	IPCTaskMngrClient(uint8_t svrIdx,uint8_t slaveID=0);

	bool		DebugDump(void);
	bool		Reset(void);
	bool		AddTask(TaskDesc* task);
	uint16_t	GetPeningTaskCnt(void);
	bool		PrintTestReport(void);
	bool		DumpTestReport(const char* filename);
	bool		PrintTestResult(void);
	bool		DumpTestResult(const char* filename);

	bool		AddSlot(uint8_t slotID);
	bool		DelSlot(uint8_t slotID);
	DutInfo*	GetSlotInfo(uint8_t slotID);
	enDutStatus	GetSlotStatus(uint8_t slotID);
	bool		IdentifySlot(uint8_t slotID);
	bool		IdfyAllSlots(void);
	bool		CheckSlotAvail(uint8_t slotID);
	uint8_t		GetAvailSlotID(uint8_t* sIDLst,uint8_t maxCnt);
	bool		DelAllSlot(void);
};

#endif
