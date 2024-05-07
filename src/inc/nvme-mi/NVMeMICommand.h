/*
 * NVMeMICommand.h
 *
 */

#ifndef _NVME_MI_NVMEMICOMMAND_H_
#define _NVME_MI_NVMEMICOMMAND_H_

#include "protocol/nvme-mi/NVMeMIAdmin.h"
#include "protocol/nvme-mi/NVMeMICommand.h"
#include "protocol/nvme-mi/NVMeMIPrimitives.h"
#include "protocol/smbus/SMBusPackage.h"
#include "MCUPwrBPDriver.h"
#include "DummyBPDriver.h"
#include "NVMeMIErrInjection.h"
//#include "NVMeMICRC.h"
#include "utility/CRC.h"

#define NVMeMI_CRC32_POLY				0x1EDC6F41
#define SMBUS_PEC_CRCINITIAL			0
#define MCTP_XFER_UNIT_SIZE_DEF			64
#define CMD_SLOT_ID_MAX					1

#define SMBUS_EXTRA_DATA_SIZE			(SMBUS_HEADER_SIZE + MCTP_HEADER_SIZE + SMBUS_PEC_LEN)

#ifndef   UNUSED_PARAM
#define   UNUSED_PARAM(v)   (void)(v)
#endif

class NVMeMiCommand
{
private:
	NVMeMIBpDriver*	mpBPDriver;
	//uint8_t*	mpCRC32Table;
	uint16_t	mMCTPUnitSize;

	NVMeMIErrInject		mMIErrInject;
	bool				mbAutoClear;

	uint8_t		mSrcEPID;
	uint8_t		mDstEPID;
	uint8_t		mSrcAddr;
	uint8_t		mDstAddr;
	uint8_t		mCmdSlotID;

protected:
	uint32_t	CalcCRC32(Buffers* buf,uint16_t len)
	{
		//return CalculateCRC32(mpCRC32Table,buf->GetBufferPoint(),(uint32_t)len);
		return (uint32_t)AshaCRCMngr()(CRC_32C,buf->GetBufferPoint(),(uint32_t)len);
	};
	uint8_t		CalcSMBusPEC(Buffers* buf,uint8_t len)
	{
		//return smbus_crc8(SMBUS_PEC_CRCINITIAL,buf->GetBufferPoint(),(uint32_t)len);
		return (uint8_t)AshaCRCMngr()(CRC_8_ATM,buf->GetBufferPoint(),(uint32_t)len);
	};

	void		InjectErrorToPackage(uint16_t noPack,SMBusPackage& smbus);
	bool		GetMCTPPackageByIndex(MCTPPackageRegMap& package,uint16_t index,MCTPNVMeMIMsgRegMap* req);
	bool		GetSMBusPackage(SMBusPackage& smbus,MCTPPackageRegMap& mctp);
	bool		GetNVMeMIMsgFromSMBus(MCTPNVMeMIMsgRegMap* resp,uint16_t index,SMBusPackage& smbus,bool &isLast);
	uint16_t	GetSMBusPackageSize(void)
	{
		return GetMCTPUnitSize() + SMBUS_EXTRA_DATA_SIZE;
	};
	bool		XferSMBusPackage(SMBusPackage& smbus);
	bool		GetRcvrBytes(uint16_t& totalRcvr);
	bool		TryGetRcvrBytes(uint16_t& totalRcvr,uint32_t timeoutms);
	bool		RcvrSMBusResponse(SMBusPackage& smbus,uint16_t maxBytes,uint16_t index);
	bool		SendNVMeMIRequest(MCTPNVMeMIMsgRegMap* req);
	bool		GetNVMeMIResponse(MCTPNVMeMIMsgRegMap* resp);

	bool		SendReqAndRecvResp(MCTPNVMeMIMsgRegMap* req,MCTPNVMeMIMsgRegMap* resp);

public:
	NVMeMiCommand()
		:mMCTPUnitSize(MCTP_XFER_UNIT_SIZE_DEF),mpBPDriver(NULL),mbAutoClear(true)
	{
		mSrcEPID		= mDstEPID		= 0;
		mSrcAddr		= mDstAddr		= 0;
		mCmdSlotID		= 0;
		//mpCRC32Table	= Crc32_Init(NVMeMI_CRC32_POLY);
	};
	virtual ~NVMeMiCommand()
	{
/*		if(NULL != mpCRC32Table)
		{
			free(mpCRC32Table);
			mpCRC32Table = NULL;
		}*/
	}

	bool InjectError(uint16_t noPack,uint16_t offset,uint32_t bitMask,uint32_t dwVal)
	{
		return mMIErrInject.Push(noPack,offset,bitMask,dwVal);
	};
	void SetAutoClear(bool enable){mbAutoClear = enable;};
	bool GetAutoClear(void){return mbAutoClear;};
	void ClearInjectErr(void) {mMIErrInject.Clear();};

	void SetBPDriver(NVMeMIBpDriver* bp){mpBPDriver=bp;};

	void SetSrcI2CAddr(uint8_t val) {mSrcAddr = val;};
	uint8_t GetSrcI2CAddr(void) {return mSrcAddr;};
	void SetDstI2CAddr(uint8_t val) {mDstAddr = val;};
	uint8_t GetDstI2CAddr(void) {return mDstAddr;};

	void SetCmdSlotID(uint8_t csi)
	{
		mCmdSlotID	= ( csi >= CMD_SLOT_ID_MAX )?CMD_SLOT_ID_MAX:csi;
	}
	uint8_t GetCmdSlotID(void) {return mCmdSlotID;};

	void SetSrcEndPointID(uint8_t val) {mSrcEPID = val;};
	uint8_t GetSrcEndPointID(void) {return mSrcEPID;};
	void SetDstEndPointID(uint8_t val) {mDstEPID = val;};
	uint8_t GetDstEndPointID(void) {return mDstEPID;}

	void SetMCTPUnitSize(uint16_t val) {mMCTPUnitSize = val;};
	uint16_t GetMCTPUnitSize(void) {return mMCTPUnitSize;};

	uint16_t	GetMCTPPackageCount(MCTPNVMeMIMsgRegMap* msg);

	/*
	 * NVMe-MI Primitives
	 */
	NVMeMIPrimResp* MIPrimGetStatus(bool cesf);
	NVMeMIPrimResp* MIPrimPause(void);
	NVMeMIPrimResp* MIPrimResume(void);
	NVMeMIPrimResp* MIPrimAbort(void);
	NVMeMIPrimResp* MIPrimReplay(uint8_t rro);

	/*
	 * NVMe-MI Commands
	 */
	NVMeMICmdResp*	MICmdConfigGet(uint8_t cfgID,uint8_t pID);
	NVMeMICmdResp*	MICmdConfigSet(uint8_t cfgID,uint8_t pID,uint16_t wdVal,uint32_t dwVal);
	NVMeMICmdResp*  MICmdCtrlHSP(uint16_t ctlID,uint8_t maxRent,bool incf,bool incpf,bool incvf,\
			bool all,bool ccf,bool cwarn,bool spare,bool pdlu,bool ctemp,bool csts);
	NVMeMICmdResp*	MICmdNVMSHSP(bool cs);
	NVMeMICmdResp*	MICmdReadData(uint16_t ctlID,uint8_t pID,uint8_t dtyp);
	NVMeMICmdResp*	MICmdReset(uint8_t type);
	NVMeMICmdResp*	MICmdVPDRead(uint16_t dofst,uint16_t dlen);
	NVMeMICmdResp*	MICmdVPDWrite(uint16_t dofst,uint16_t dlen,Buffers* buf,uint16_t offset);

	/*
	 * Admin Commands
	 */
	NVMeCmdResp* DeviceSelfTest(uint32_t nsid,uint8_t stc,uint32_t timeout);
	NVMeCmdResp* FirmwareCommit(uint8_t slotID,uint8_t cAction,bool bBPID,uint32_t timeout);
	NVMeCmdResp* FirmwareDownload(uint32_t offset,uint32_t length,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* GetFeatures(uint32_t nsid,uint8_t fid,uint8_t sel,uint32_t cdw11,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* GetLogPage(uint32_t nsid,uint8_t lid,uint64_t offset,uint32_t length,uint8_t lsp,bool bRAE,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* Identify(uint8_t cns,uint16_t cntid,uint32_t nsid,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* NSAttach(uint32_t nsid,uint8_t sel,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* NSMngmnt(uint32_t nsid,uint8_t sel,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* SetFeatures(uint32_t nsid,uint8_t fid,bool bSV,uint32_t cdw11,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* FormatNVM(uint32_t nsid,uint8_t lbaf,uint8_t pi,uint8_t ses,bool bMSET,bool bPIL,uint32_t timeout);
	NVMeCmdResp* Sanitize(uint8_t act,uint8_t passCnt,uint32_t overPat,bool bAUSE,bool bOIPBP,bool bNDAS,uint32_t timeout);
	NVMeCmdResp* SecurityRecv(uint8_t nssf,uint8_t spsp0,uint8_t spsp1,uint8_t secp,uint32_t length,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* SecuritySend(uint8_t nssf,uint8_t spsp0,uint8_t spsp1,uint8_t secp,uint32_t length,Buffers* buf,uint32_t timeout);
};

#endif /* _NVME_MI_NVMEMICOMMAND_H_ */
