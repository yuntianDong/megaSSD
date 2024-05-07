#ifndef _NVME_COMMAND_H
#define _NVME_COMMAND_H

#include "device/DeviceHandler.h"
#include "context/Context.h"
#include "IPatternMngr.h"
#include "IHostBufMngr.h"
typedef struct _E2EDPParam
{
	uint32_t	initRefTag;
	uint16_t	appTagMask;
	uint16_t	appTagVal;
}E2EDPParam;

class NVMeCommand
{
protected:
	NVMeEngine* mpEngine;
	emDataXferMode mXferMode;
	IPatternMngr* mpatMngr;
	bool patSwitch;
	IHostBufMngr* mbufMngr;
	Device* mpdev;
public:
	NVMeCommand(Device* dev)
		:mpEngine(nullptr),mXferMode(DataXfer_PRP),mpatMngr(nullptr),mbufMngr(nullptr),mpdev(dev),patSwitch(false)
	{
		if(nullptr != dev->GetEngine())
		{
			mpEngine = dev->GetEngine();
		}
		else
		{
			LOGERROR("Driver engine is nullptr!");
		}
	}

	virtual ~NVMeCommand(void)
	{};

	NVMeEngine* GetEngine(void){return mpEngine;};
	void SetDataXferMode(emDataXferMode mode){mXferMode = mode;};
	emDataXferMode GetDataXferMode(void){return mXferMode;};

	bool LoadPatternMngr(IPatternMngr* patMngr)
	{
		if(patMngr == nullptr)
		{
			return false;
		}
		else
		{
			mpatMngr = patMngr;
			patSwitch = true;
			return true;
		}
		return false;
	};
	void PatternMngrSwitch(bool enable)
	{
		patSwitch = enable;
	};
	bool IsPatternMngrLoaded(void)
	{
		if(mpatMngr != nullptr && patSwitch == true)
		{
			return true;
		}
		return false;
	};
	bool LoadBufferMngr(IHostBufMngr* bufMngr)
	{
		if(bufMngr == nullptr)
		{
			LOGERROR("error parameter,bufmngr is nullptr!");
			return false;
		}
		else
		{
			mbufMngr = bufMngr;
			return true;
		}
		return false;
	};
	bool IsBufferMngrLoaded(void)
	{
		return (mbufMngr == nullptr) ? false : true;
	};
};

class NVMeAdminCmd : public NVMeCommand
{
private:
	ParentContext* parentContext;
public:
	NVMeAdminCmd(Device* dev)
		:NVMeCommand(dev),parentContext(nullptr)
	{};

	/* Other API */
	bool IsNSActive(uint32_t nsID);
	bool LoadParentContext(ParentContext* parent);
	bool IsParentContextLoaded(void);

	NVMeCmdResp* AdminPassthrough(uint32_t cdw0,uint32_t cdw1,uint32_t cdw2,uint32_t cdw3,\
			uint32_t cdw10,uint32_t cdw11,uint32_t cdw12, uint32_t cdw13,uint32_t cdw14,\
			uint32_t cdw15,uint32_t dBufLen,uint32_t mBufLen,Buffers* dBuf,Buffers* mBuf,uint32_t timeout);

	NVMeCmdResp* Abort(uint16_t cid,uint16_t sqid,uint32_t timeout);
	NVMeCmdResp* AsyncEventReq(uint32_t timeout);
	NVMeCmdResp* CreateIOCQ(uint16_t qid,uint16_t qSize,uint16_t iv,bool bIEN,bool bPC,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* CreateIOSQ(uint16_t qid,uint16_t qSize,uint16_t cqid,uint8_t qPrio,bool bPC,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* DeleteIOCQ(uint16_t qid,uint32_t timeout);
	NVMeCmdResp* DeleteIOSQ(uint16_t qid,uint32_t timeout);
	NVMeCmdResp* DoorbellBufCfg(Buffers* buf,uint32_t timeout);
	NVMeCmdResp* DeviceSelfTest(uint32_t nsid,uint8_t stc,uint32_t timeout);
	NVMeCmdResp* DirectiveRecv(uint8_t dOper,uint8_t dType,uint16_t dSpec,uint32_t numDW,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* DirectiveSend(uint8_t dOper,uint8_t dType,uint16_t dSpec,uint32_t numDW,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* FirmwareCommit(uint8_t slotID,uint8_t cAction,bool bBPID,uint32_t timeout);
	NVMeCmdResp* FirmwareDownload(uint32_t offset,uint32_t length,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* GetFeatures(uint32_t nsid,uint8_t fid,uint8_t sel,uint32_t cdw11,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* GetLogPage(uint32_t nsid,uint8_t lid,uint64_t offset,uint32_t length,uint8_t lsp,bool bRAE,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* Identify(uint8_t cns,uint16_t cntid,uint32_t nsid,uint8_t csi, uint16_t cnssi,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* KeepAlive(uint32_t timeout);
	NVMeCmdResp* NVMeMIRecv(uint8_t nmsp0,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* NVMeMISend(uint8_t nmsp0,uint32_t length,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* NSAttach(uint32_t nsid,uint8_t sel,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* NSMngmnt(uint32_t nsid,uint8_t sel,uint8_t csi,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* SetFeatures(uint32_t nsid,uint8_t fid,bool bSV,uint32_t cdw11,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* VirtualMngr(uint16_t cntid,uint8_t rt,uint8_t act,uint16_t numOfRes,uint32_t timeout);
	NVMeCmdResp* FormatNVM(uint32_t nsid,uint8_t lbaf,uint8_t pi,uint8_t ses,bool bMSET,bool bPIL,uint32_t timeout);
	NVMeCmdResp* Sanitize(uint8_t act,uint8_t passCnt,uint32_t overPat,bool bAUSE,bool bOIPBP,bool bNDAS,uint32_t timeout);
	NVMeCmdResp* SecurityRecv(uint8_t nssf,uint8_t spsp0,uint8_t spsp1,uint8_t secp,uint32_t length,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* SecuritySend(uint8_t nssf,uint8_t spsp0,uint8_t spsp1,uint8_t secp,uint32_t length,Buffers* buf,uint32_t timeout);
};

class NVMeNVMCmd : public NVMeCommand
{
protected:
	emFusedMode mFusedMode;
	bool mbFusedCmd;
	E2EDPParam	mE2EDPParam;
	uint32_t mnsid;
	ChildContext* childContext;
public:
	NVMeNVMCmd(Device* dev,uint32_t nsid)
		:NVMeCommand(dev),mFusedMode(Fused_None),mbFusedCmd(false),mnsid(nsid)
	{
		memset(&mE2EDPParam,0,sizeof(E2EDPParam));
	};

	void SetFusedFlag(void){mbFusedCmd = true;mFusedMode = Fused_None;};
	void ClrFusedFlag(void){mbFusedCmd = false;mFusedMode = Fused_None;};

	void SetE2EDPParam(uint32_t initRefTag,uint16_t appTagMask,uint16_t appTagVal)
	{
		mE2EDPParam.initRefTag	= initRefTag;
		mE2EDPParam.appTagMask	= appTagMask;
		mE2EDPParam.appTagVal	= appTagVal;
	};

	uint32_t GetInitRefTag(void) {return mE2EDPParam.initRefTag;};
	uint16_t GetAppTagMask(void) {return mE2EDPParam.appTagMask;};
	uint16_t GetAppTagVal(void) {return mE2EDPParam.appTagVal;};

	emFusedMode GetFusedMode(void){UpdateFusedMode();return mFusedMode;};
	void UpdateFusedMode(void)
	{
		if(true == mbFusedCmd)
		{
			if(Fused_None == mFusedMode)
			{
				mFusedMode = Fused_1stCmd;
			}
			else if(Fused_1stCmd == mFusedMode)
			{
				mFusedMode = Fused_2ndCmd;
			}
			else{
				mFusedMode = Fused_None;
				mbFusedCmd = false;
			}
		}
	};

	bool LoadChildContext(ChildContext* child);
	bool IsChildContextLoaded(void);

	NVMeCmdResp* IoPassthrough(uint32_t cdw0,uint32_t cdw1,uint32_t cdw2,uint32_t cdw3,\
			uint32_t cdw10,uint32_t cdw11,uint32_t cdw12, uint32_t cdw13,uint32_t cdw14,uint32_t cdw15,\
			uint32_t dBufLen,uint32_t mBufLen,Buffers* dBuf,Buffers* mBuf,uint32_t timeout);

	NVMeCmdResp* Compare(uint64_t slba,uint16_t length,uint8_t prinfo,\
			bool bFUA,bool bLR,bool bSTC,uint16_t elbat,uint16_t elbatm,uint64_t elbst,uint64_t elbrf,Buffers* dBuf,Buffers* mBuf,uint32_t timeout);
	NVMeCmdResp* DataSetMngr(bool bIDR,bool bIDW,bool bAD,uint8_t numOfRng,\
			Buffers* buf,uint32_t timeout);
	NVMeCmdResp* Flush(uint32_t timeout);
	NVMeCmdResp* Read(uint64_t slba,uint16_t length,uint8_t prinfo,\
			bool bFUA,bool bLR,bool bSTC,uint8_t dsm,uint16_t elbat,uint16_t elbatm,uint64_t elbst,uint64_t elbrf,Buffers* dBuf,\
			Buffers* mBuf,uint32_t timeout);
	NVMeCmdResp* ResvAcquire(uint8_t action,uint8_t type,bool bIEKEY,\
			Buffers* buf,uint32_t timeout);
	NVMeCmdResp* ResvRegister(uint8_t action,uint8_t cptpl,bool bIEKEY,\
			Buffers* buf,uint32_t timeout);
	NVMeCmdResp* ResvRelease(uint8_t action,uint8_t type,bool bIEKEY,\
			Buffers* buf,uint32_t timeout);
	NVMeCmdResp* ResvReport(uint32_t length,bool bEDS,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* Write(uint64_t slba,uint16_t length,uint8_t prinfo,bool bFUA,\
			bool bLR,bool bSTC,uint8_t dtype,uint16_t dspec,uint8_t dsm,uint16_t lbat,uint16_t lbatm,uint64_t lbst,uint64_t ilbrf,\
			Buffers* dBuf,Buffers* mBuf,uint32_t timeout);
	NVMeCmdResp* WriteUNC(uint64_t slba,uint16_t length,uint32_t timeout);
	NVMeCmdResp* WriteZeroes(uint64_t slba,uint16_t length,uint8_t prinfo,bool bFUA,\
			bool bLR,bool bDEAC,uint32_t timeout);
};

#endif
