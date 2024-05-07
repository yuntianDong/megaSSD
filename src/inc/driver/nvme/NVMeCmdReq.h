#ifndef _NVME_CMD_REQ_H
#define _NVME_CMD_REQ_H

#include <string.h>
#include "ClassMember.h"
#include "Logging.h"

#define NVME_ADMIN_OP_DELETEIOSQ		0x00
#define NVME_ADMIN_OP_CREATEIOSQ		0x01
#define NVME_ADMIN_OP_GETLOGPAGE		0x02
#define NVME_ADMIN_OP_DELETEIOCQ		0x04
#define NVME_ADMIN_OP_CREATEIOCQ		0x05
#define NVME_ADMIN_OP_IDENTIFY			0x06
#define NVME_ADMIN_OP_ABORT				0x08
#define NVME_ADMIN_OP_SETFEATURES		0x09
#define NVME_ADMIN_OP_GETFEATURES		0x0A
#define NVME_ADMIN_OP_ASYNCEVTREQ		0x0C
#define NVME_ADMIN_OP_NSMNGR			0x0D
#define NVME_ADMIN_OP_FWCOMMIT			0x10
#define NVME_ADMIN_OP_FWDNLD			0x11
#define NVME_ADMIN_OP_DEVSELFTEST		0x14
#define NVME_ADMIN_OP_NSATTACH			0x15
#define NVME_ADMIN_OP_KEEPALIVE			0x18
#define NVME_ADMIN_OP_DIRTVSEND			0x19
#define NVME_ADMIN_OP_DIRTVRECV			0x1A
#define NVME_ADMIN_OP_VMMNGR			0x1C
#define NVME_ADMIN_OP_NVMEMISEND		0x1D
#define NVME_ADMIN_OP_NVMEMIRECV		0x1E
#define NVME_ADMIN_OP_DBBUFCFG			0x7C
#define NVME_ADMIN_OP_FORMATNVM			0x80
#define NVME_ADMIN_OP_SECUSEND			0x81
#define NVME_ADMIN_OP_SECURECV			0x82
#define NVME_ADMIN_OP_SANITIZE			0x84

#define NVME_NVM_OP_FLUSH				0x00
#define NVME_NVM_OP_WRITE				0x01
#define NVME_NVM_OP_READ				0x02
#define NVME_NVM_OP_WRITEUNC			0x04
#define NVME_NVM_OP_COMPARE				0x05
#define NVME_NVM_OP_WRITEZERO			0x08
#define NVME_NVM_OP_DATASETMNG			0x09
#define NVME_NVM_OP_RSVTREG				0x0D
#define NVME_NVM_OP_RSVTRPT				0x0E
#define NVME_NVM_OP_RSVTACQ				0x11
#define NVME_NVM_OP_RSVTREL				0x15

#define NVME_CDW0_FLAG_FUSE_OFFSET	8
#define NVME_CDW0_FLAG_FUSE_BITS	2
#define NVME_CDW0_FLAG_FUSE_MASK	(1 << NVME_CDW0_FLAG_FUSE_BITS -1)
#define NVME_CDW0_FLAG_PSDT_OFFSET	14
#define NVME_CDW0_FLAG_PSDT_BITS	2
#define NVME_CDW0_FLAG_PSDT_MASK	(1 << NVME_CDW0_FLAG_PSDT_BITS -1)
#define NVME_CDW0_OP_XFER_OFFSET	0
#define NVME_CDW0_OP_XFER_BITS		2
#define NVME_CDW0_OP_XFER_MASK		(1 << NVME_CDW0_OP_XFER_BITS -1)

#define NVME_CDW10_QID_OFFSET		0
#define NVME_CDW10_QID_BITS			16
#define NVME_CDW10_QSIZE_OFFSET		16
#define NVME_CDW10_QSIZE_BITS		16
#define NVME_CDW10_LID_OFFSET		0
#define NVME_CDW10_LID_BITS			8
#define NVME_CDW10_LSP_OFFSET		8
#define NVME_CDW10_LSP_BITS			4
#define NVME_CDW10_RAE_OFFSET		15
#define NVME_CDW10_RAE_BITS			1
#define NVME_CDW10_NUMDL_OFFSET		16
#define NVME_CDW10_NUMDL_BITS		16
#define NVME_CDW10_CNS_OFFSET		0
#define NVME_CDW10_CNS_BITS			8
#define NVME_CDW10_CNTID_OFFSET		16
#define NVME_CDW10_CNTID_BITS		16
#define NVME_CDW10_CID_OFFSET		16
#define NVME_CDW10_CID_BITS			16
#define NVME_CDW10_FID_OFFSET		0
#define NVME_CDW10_FID_BITS			8
#define NVME_CDW10_SV_OFFSET		31
#define NVME_CDW10_SV_BITS			1
#define NVME_CDW10_SEL_OFFSET		8
#define NVME_CDW10_SEL_BITS			3
#define NVME_CDW10_SEL2_OFFSET		0
#define NVME_CDW10_SEL2_BITS		3
#define NVME_CDW10_FS_OFFSET		0
#define NVME_CDW10_FS_BITS			3
#define NVME_CDW10_CA_OFFSET		3
#define NVME_CDW10_CA_BITS			3
#define NVME_CDW10_BPID_OFFSET		31
#define NVME_CDW10_BPID_BITS		1
#define NVME_CDW10_STC_OFFSET		0
#define NVME_CDW10_STC_BITS			4
#define NVME_CDW10_ACT_OFFSET		0
#define NVME_CDW10_ACT_BITS			4
#define NVME_CDW10_RT_OFFSET		8
#define NVME_CDW10_RT_BITS			3
#define NVME_CDW10_CNTLID_OFFSET	16
#define NVME_CDW10_CNTLID_BITS		16
#define NVME_CDW10_LBAF_OFFSET		0
#define NVME_CDW10_LBAF_BITS		4
#define NVME_CDW10_MSET_OFFSET		4
#define NVME_CDW10_MSET_BITS		1
#define NVME_CDW10_PI_OFFSET		5
#define NVME_CDW10_PI_BITS			3
#define NVME_CDW10_PIL_OFFSET		8
#define NVME_CDW10_PIL_BITS			1
#define NVME_CDW10_SES_OFFSET		9
#define NVME_CDW10_SES_BITS			3
#define NVME_CDW10_NSSF_OFFSET		0
#define NVME_CDW10_NSSF_BITS		8
#define NVME_CDW10_SPSP0_OFFSET		8
#define NVME_CDW10_SPSP0_BITS		8
#define NVME_CDW10_SPSP1_OFFSET		16
#define NVME_CDW10_SPSP1_BITS		8
#define NVME_CDW10_SECP_OFFSET		24
#define NVME_CDW10_SECP_BITS		8
#define NVME_CDW10_SANACT_OFFSET	0
#define NVME_CDW10_SANACT_BITS		3
#define NVME_CDW10_AUSE_OFFSET		3
#define NVME_CDW10_AUSE_BITS		1
#define NVME_CDW10_OWPASS_OFFSET	4
#define NVME_CDW10_OWPASS_BITS		4
#define NVME_CDW10_OIPBP_OFFSET		8
#define NVME_CDW10_OIPBP_BITS		1
#define NVME_CDW10_NDAS_OFFSET		9
#define NVME_CDW10_NDAS_BITS		1
#define NVME_CDW10_NR_OFFSET		0
#define NVME_CDW10_NR_BITS			8
#define NVME_CDW10_RREGA_OFFSET		0
#define NVME_CDW10_RREGA_BITS		3
#define NVME_CDW10_IEKEY_OFFSET		3
#define NVME_CDW10_IEKEY_BITS		1
#define NVME_CDW10_CPTPL_OFFSET		30
#define NVME_CDW10_CPTPL_BITS		2
#define NVME_CDW10_RACQA_OFFSET		0
#define NVME_CDW10_RACQA_BITS		3
#define NVME_CDW10_RRELA_OFFSET		0
#define NVME_CDW10_RRELA_BITS		3
#define NVME_CDW10_RTYPE_OFFSET		8
#define NVME_CDW10_RTYPE_BITS		8
#define NVME_CDW10_NMSP0_OFFSET		0
#define NVME_CDW10_NMSP0_BITS		8

#define NVME_CDW11_PC_OFFSET		0
#define NVME_CDW11_PC_BITS			1
#define NVME_CDW11_QPRIO_OFFSET		1
#define NVME_CDW11_QPRIO_BITS		2
#define NVME_CDW11_CQID_OFFSET		16
#define NVME_CDW11_CQID_BITS		16
#define NVME_CDW11_NUMDU_OFFSET		0
#define NVME_CDW11_NUMDU_BITS		16
#define NVME_CDW11_IEN_OFFSET		1
#define NVME_CDW11_IEN_BITS			1
#define NVME_CDW11_IV_OFFSET		16
#define NVME_CDW11_IV_BITS			16
#define NVME_CDW11_DOPER_OFFSET		0
#define NVME_CDW11_DOPER_BITS		8
#define NVME_CDW11_DTYPE_OFFSET		8
#define NVME_CDW11_DTYPE_BITS		8
#define NVME_CDW11_DSPEC_OFFSET		16
#define NVME_CDW11_DSPEC_BITS		16
#define NVME_CDW11_NR_OFFSET		0
#define NVME_CDW11_NR_BITS			16
#define NVME_CDW11_IDR_OFFSET		0
#define NVME_CDW11_IDR_BITS			1
#define NVME_CDW11_IDW_OFFSET		1
#define NVME_CDW11_IDW_BITS			1
#define NVME_CDW11_AD_OFFSET		2
#define NVME_CDW11_AD_BITS			1
#define NVME_CDW11_EDS_OFFSET		0
#define NVME_CDW11_EDS_BITS			1
#define NVME_CDW11_CSI_OFFSET		24
#define NVME_CDW11_CSI_BITS			8
#define NVME_CDW11_CNSSI_OFFSET		0
#define NVME_CDW11_CNSSI_BITS	    16

#define NVME_CDW12_LPOL_OFFSET		0
#define NVME_CDW12_LPOL_BITS		32
#define NVME_CDW12_NLB_OFFSET		0
#define NVME_CDW12_NLB_BITS			16
#define NVME_CDW12_DTYPE_OFFSET		20
#define NVME_CDW12_DTYPE_BITS		4
#define NVME_CDW12_PRINFO_OFFSET	26
#define NVME_CDW12_PRINFO_BITS		4
#define NVME_CDW12_FUA_OFFSET		30
#define NVME_CDW12_FUA_BITS			1
#define NVME_CDW12_LR_OFFSET		31
#define NVME_CDW12_LR_BITS			1
#define NVME_CDW12_DEAC_OFFSET		25
#define NVME_CDW12_DEAC_BITS		1
#define NVME_CDW12_STC_OFFSET		24
#define NVME_CDW12_STC_BITS			1

#define NVME_CDW13_LPOU_OFFSET		0
#define NVME_CDW13_LPOU_BITS		32
#define NVME_CDW13_DSM_OFFSET		0
#define NVME_CDW13_DSM_BITS			8
#define NVME_CDW13_DSPEC_OFFSET		16
#define NVME_CDW13_DSPEC_BITS		16

#define NVME_CDW15_LBAT_OFFSET		0
#define NVME_CDW15_LBAT_BITS		16
#define NVME_CDW15_LBATM_OFFSET		16
#define NVME_CDW15_LBATM_BITS		16

#define NVME_CDW0_OPC_OFFSET		0
#define NVME_CDW0_OPC_BITS			8
#define NVME_CDW0_FLAGS_OFFSET		8
#define NVME_CDW0_FLAGS_BITS		8
#define NVME_CDW0_CID_OFFSET		16
#define NVME_CDW0_CID_BITS			16

typedef enum _emDataXferMode
{
	DataXfer_PRP,
	DataXfer_SGL_MD_SINGLE,
	DataXfer_SGL_MD_SEGMENT,
	DataXfer_FENCE,
}emDataXferMode;

typedef enum _emFusedMode
{
	Fused_None,
	Fused_1stCmd,
	Fused_2ndCmd,
	Fused_FENCE,
}emFusedMode;

typedef enum _emDataXferDir
{
	DataXferDir_None	=0b0,
	DataXferDir_OUT		=0b01,
	DataXferDir_IN		=0b10,
	DataXferDir_INOUT	=0b11,
	DataXferDir_FENCE,
}emDataXferDir;

#define NVME_CMD_DWORD				16
class NVMeCmdReq
{
private:
	uint32_t 	mNVMeCmdDesc[NVME_CMD_DWORD];
	bool 		mbAdminCmd;

protected:
	uint32_t* GetCDW10Addr(void) {return &(mNVMeCmdDesc[10]);};
	uint32_t* GetCDW11Addr(void) {return &(mNVMeCmdDesc[11]);};
	uint32_t* GetCDW12Addr(void) {return &(mNVMeCmdDesc[12]);};
	uint32_t* GetCDW13Addr(void) {return &(mNVMeCmdDesc[13]);};
	uint32_t* GetCDW14Addr(void) {return &(mNVMeCmdDesc[14]);};
	uint32_t* GetCDW15Addr(void) {return &(mNVMeCmdDesc[15]);};

	ClassMemberBitMask<uint32_t,uint8_t> dxferMode(void){return ClassMemberBitMask<uint32_t,uint8_t>(&(mNVMeCmdDesc[0]),NVME_CDW0_FLAG_PSDT_OFFSET,NVME_CDW0_FLAG_PSDT_BITS);};
	ClassMemberBitMask<uint32_t,uint8_t> fusedMode(void){return ClassMemberBitMask<uint32_t,uint8_t>(&(mNVMeCmdDesc[0]),NVME_CDW0_FLAG_FUSE_OFFSET,NVME_CDW0_FLAG_FUSE_BITS);};
	ClassMemberBitMask<uint32_t,uint8_t> dxferDir(void){return ClassMemberBitMask<uint32_t,uint8_t>(&(mNVMeCmdDesc[0]),NVME_CDW0_OP_XFER_OFFSET,NVME_CDW0_OP_XFER_BITS);};
	ClassMemberBitMask<uint32_t,uint8_t> opcode(void) {return ClassMemberBitMask<uint32_t,uint8_t>(&(mNVMeCmdDesc[0]),NVME_CDW0_OPC_OFFSET,NVME_CDW0_OPC_BITS);};
	ClassMemberBitMask<uint32_t,uint8_t> flags(void) {return ClassMemberBitMask<uint32_t,uint8_t>(&(mNVMeCmdDesc[0]),NVME_CDW0_FLAGS_OFFSET,NVME_CDW0_FLAGS_BITS);};
	ClassMemberBitMask<uint32_t,uint16_t> cid(void) {return ClassMemberBitMask<uint32_t,uint16_t>(&(mNVMeCmdDesc[0]),NVME_CDW0_CID_OFFSET,NVME_CDW0_CID_BITS);};
	ClassMember<uint32_t> nsid(void) {return ClassMember<uint32_t>(&(mNVMeCmdDesc[1]));};
	ClassMember<uint32_t> cdw2(void) {return ClassMember<uint32_t>(&(mNVMeCmdDesc[2]));};
	ClassMember<uint32_t> cdw3(void) {return ClassMember<uint32_t>(&(mNVMeCmdDesc[3]));};
	ClassMember<uint64_t> mptr(void) {return ClassMember<uint64_t>( (uint64_t *)&(mNVMeCmdDesc[4]) );};
	ClassMember<uint64_t> dptr1(void) {return ClassMember<uint64_t>( (uint64_t *)&(mNVMeCmdDesc[6]) );};
	ClassMember<uint64_t> dptr2(void) {return ClassMember<uint64_t>( (uint64_t *)&(mNVMeCmdDesc[8]) );};
	ClassMember<uint32_t> cdw10(void) {return ClassMember<uint32_t>(&(mNVMeCmdDesc[10]));};
	ClassMember<uint32_t> cdw11(void) {return ClassMember<uint32_t>(&(mNVMeCmdDesc[11]));};
	ClassMember<uint32_t> cdw12(void) {return ClassMember<uint32_t>(&(mNVMeCmdDesc[12]));};
	ClassMember<uint32_t> cdw13(void) {return ClassMember<uint32_t>(&(mNVMeCmdDesc[13]));};
	ClassMember<uint32_t> cdw14(void) {return ClassMember<uint32_t>(&(mNVMeCmdDesc[14]));};
	ClassMember<uint32_t> cdw15(void) {return ClassMember<uint32_t>(&(mNVMeCmdDesc[15]));};

public:
	NVMeCmdReq(uint8_t opCode,bool bAdminCmd,emDataXferMode xferMode,emFusedMode fusedMode)
	{
		memset(&mNVMeCmdDesc,0,NVME_CMD_DWORD*sizeof(uint32_t));

		SetOpCode(opCode);
		SetDXferMode(xferMode);
		SetFusedMode(fusedMode);

		mbAdminCmd	= bAdminCmd;
	}

	void Dump(void)
	{
		uint8_t 	bufSize	= GetCmdSize();
		uint8_t* 	cmdBuf	= (uint8_t*)GetCmdAddr();

		for(uint8_t idx=0;idx<bufSize;idx+=1)
		{
			if(0 == idx % 8)
			{
				printf("\n%04x :",idx);
			}

			printf(" %02x",*(cmdBuf+idx));
		}

		printf("\n");
	}

	void* GetCmdAddr(void) {return (void*)&mNVMeCmdDesc;};
	uint8_t GetCmdSize(void) {return NVME_CMD_DWORD*sizeof(uint32_t);};

	bool isAdminCmd(void) {return mbAdminCmd;};

	void SetDXferMode(emDataXferMode mode) {dxferMode().Set(uint8_t(mode));};
	emDataXferMode GetDXferMode(void) {return (emDataXferMode)dxferMode().Get();};
	void SetFusedMode(emFusedMode mode) {fusedMode().Set(uint8_t(mode));};
	emFusedMode GetFusedMode(void) {return (emFusedMode)fusedMode().Get();};

	emDataXferDir GetDXferDir(void) {return (emDataXferDir)dxferDir().Get();};

	void SetCID(uint16_t val) {cid().Set(val);};
	uint16_t GetCID(void) {return cid().Get();};

	void SetOpCode(uint8_t val) {opcode().Set(val);};
	uint8_t GetOpCode(void) {return opcode().Get();};
	void SetFlags(uint8_t val) {flags().Set(val);};
	uint8_t GetFlags(void) {return flags().Get();};
	void SetNSID(uint32_t val) {nsid().Set(val);};
	uint32_t GetNSID(void) {return nsid().Get();};
	void SetMPTR(uint64_t val) {mptr().Set(val);};
	uint64_t GetMPTR(void) {return mptr().Get();};
	void SetPRP1(uint64_t val) {dptr1().Set(val);};
	uint64_t GetPRP1(void) {return dptr1().Get();};
	void SetPRP2(uint64_t val) {dptr2().Set(val);};
	uint64_t GetPRP2(void) {return dptr2().Get();};

	uint32_t GetCDW2(void) {return cdw2().Get();};
	uint32_t GetCDW3(void) {return cdw3().Get();};
	uint32_t GetCDW10(void){return cdw10().Get();};
	uint32_t GetCDW11(void){return cdw11().Get();};
	uint32_t GetCDW12(void){return cdw12().Get();};
	uint32_t GetCDW13(void){return cdw13().Get();};
	uint32_t GetCDW14(void){return cdw14().Get();};
	uint32_t GetCDW15(void){return cdw15().Get();};

	void SetCDW2(uint32_t val) {return cdw2().Set(val);};
	void SetCDW3(uint32_t val) {return cdw3().Set(val);};
	void SetCDW10(uint32_t val) {return cdw10().Set(val);};
	void SetCDW11(uint32_t val) {return cdw11().Set(val);};
	void SetCDW12(uint32_t val) {return cdw12().Set(val);};
	void SetCDW13(uint32_t val) {return cdw13().Set(val);};
	void SetCDW14(uint32_t val) {return cdw14().Set(val);};
	void SetCDW15(uint32_t val) {return cdw15().Set(val);};
};

class PSDeleteIOSQ: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint16_t> QueueID(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW10Addr(),\
				NVME_CDW10_QID_OFFSET,NVME_CDW10_QID_BITS);
	}
public:
	PSDeleteIOSQ(void)
		:NVMeCmdReq(NVME_ADMIN_OP_DELETEIOSQ,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetQID(uint16_t qid) {QueueID().Set(qid);};
	uint16_t GetQID(void) {return QueueID().Get();};
};
class PSCreateIOSQ: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint16_t> QueueSize(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW10Addr(),\
				NVME_CDW10_QSIZE_OFFSET,NVME_CDW10_QSIZE_BITS);
	}

	ClassMemberBitMask<uint32_t,uint16_t> QueueID(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW10Addr(),\
				NVME_CDW10_QID_OFFSET,NVME_CDW10_QID_BITS);
	}

	ClassMemberBitMask<uint32_t,uint8_t> PC(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_PC_OFFSET,NVME_CDW11_PC_BITS);
	}

	ClassMemberBitMask<uint32_t,uint8_t> QPRIO(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_QPRIO_OFFSET,NVME_CDW11_QPRIO_BITS);
	}

	ClassMemberBitMask<uint32_t,uint16_t> CQID(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW11Addr(),\
				NVME_CDW11_CQID_OFFSET,NVME_CDW11_CQID_BITS);
	}
public:
	PSCreateIOSQ(void)
		:NVMeCmdReq(NVME_ADMIN_OP_CREATEIOSQ,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetQSize(uint16_t val) {QueueSize().Set(val);};
	uint16_t GetQSize(void) {return QueueSize().Get();};

	void SetQID(uint16_t val) {QueueID().Set(val);};
	uint16_t GetQID(void) {return QueueID().Get();};

	void SetPCFlag(bool bVal) {PC().Set((uint8_t)bVal);};
	bool GetPCFlag(void) {return (bool)(PC().Get());};

	void SetQPrio(uint8_t val) {QPRIO().Set(val);};
	uint8_t GetQPrio(void) {return QPRIO().Get();};

	void SetCQID(uint16_t val) {CQID().Set(val);};
	uint16_t GetCQID(void) {return CQID().Get();};
};
class PSGetLogPage: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> LID(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_LID_OFFSET,NVME_CDW10_LID_BITS);
	}

	ClassMemberBitMask<uint32_t,uint8_t> LSP(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_LSP_OFFSET,NVME_CDW10_LSP_BITS);
	}

	ClassMemberBitMask<uint32_t,uint8_t> RAE(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_RAE_OFFSET,NVME_CDW10_RAE_BITS);
	}

	ClassMemberBitMask<uint32_t,uint16_t> NUMDL(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW10Addr(),\
				NVME_CDW10_NUMDL_OFFSET,NVME_CDW10_NUMDL_BITS);
	}

	ClassMemberBitMask<uint32_t,uint16_t> NUMDU(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW11Addr(),\
				NVME_CDW11_NUMDU_OFFSET,NVME_CDW11_NUMDU_BITS);
	}

public:
	PSGetLogPage(void)
		:NVMeCmdReq(NVME_ADMIN_OP_GETLOGPAGE,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetLID(uint8_t val) {LID().Set(val);};
	uint8_t GetLID(void) {return LID().Get();};
	void SetLSP(uint8_t val) {LSP().Set(val);};
	uint8_t GetLSP(void) {return LSP().Get();};
	void SetRAEFlag(bool bVal) {RAE().Set((uint8_t)bVal);};
	bool GetRAEFlag(void) {return (bool)(RAE().Get());};
	void SetNumOfDW(uint32_t dwVal)
	{
		NUMDL().Set((uint16_t)(dwVal&0xFFFF));
		NUMDU().Set((uint16_t)((dwVal>>16)&0xFFFF));
	}
	uint32_t GetNumOfDW(void)
	{
		return ((uint32_t)(NUMDU().Get() & 0xFFFF) << 16) | ((uint32_t)NUMDL().Get() & 0xFFFF);
	}
	void SetLogPageOffset(uint64_t qwVal)
	{
		cdw12().Set((uint32_t)(qwVal & 0xFFFFFFFF));
		cdw13().Set((uint32_t)((qwVal>>32) & 0xFFFFFFFF));
	}
	uint64_t GetLogPageOffset(void)
	{
		return ( (uint64_t)(cdw13().Get() & 0xFFFFFFFF) << 32 |\
				(uint64_t)(cdw12().Get() & 0xFFFFFFFF) );
	}
};
class PSDeleteIOCQ: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint16_t> QueueID(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW10Addr(),\
				NVME_CDW10_QID_OFFSET,NVME_CDW10_QID_BITS);
	}
public:
	PSDeleteIOCQ(void)
		:NVMeCmdReq(NVME_ADMIN_OP_DELETEIOCQ,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetQID(uint16_t qid) {QueueID().Set(qid);};
	uint16_t GetQID(void) {return QueueID().Get();};
};
class PSCreateIOCQ: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint16_t> QueueSize(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW10Addr(),\
				NVME_CDW10_QSIZE_OFFSET,NVME_CDW10_QSIZE_BITS);
	}

	ClassMemberBitMask<uint32_t,uint16_t> QueueID(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW10Addr(),\
				NVME_CDW10_QID_OFFSET,NVME_CDW10_QID_BITS);
	}

	ClassMemberBitMask<uint32_t,uint8_t> PC(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_PC_OFFSET,NVME_CDW11_PC_BITS);
	}

	ClassMemberBitMask<uint32_t,uint8_t> IEN(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
						NVME_CDW11_IEN_OFFSET,NVME_CDW11_IEN_BITS);
	}

	ClassMemberBitMask<uint32_t,uint16_t> IV(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW11Addr(),\
						NVME_CDW11_IV_OFFSET,NVME_CDW11_IV_BITS);
	}
public:
	PSCreateIOCQ(void)
		:NVMeCmdReq(NVME_ADMIN_OP_CREATEIOCQ,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetQSize(uint16_t val) {QueueSize().Set(val);};
	uint16_t GetQSize(void) {return QueueSize().Get();};

	void SetQID(uint16_t val) {QueueID().Set(val);};
	uint16_t GetQID(void) {return QueueID().Get();};

	void SetPCFlag(bool bVal) {PC().Set((uint8_t)bVal);};
	bool GetPCFlag(void) {return (bool)PC().Get();};

	void SetIENFlag(bool bVal) {IEN().Set((uint8_t)bVal);};
	bool GetIENFlag(void) {return IEN().Get();};

	void SetIV(uint16_t val) {IV().Set(val);};
	uint16_t GetIV(void) {return IV().Get();};
};
class PSIdentify: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> CNS(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_CNS_OFFSET,NVME_CDW10_CNS_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> CNTID(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW10Addr(),\
				NVME_CDW10_CNTID_OFFSET,NVME_CDW10_CNTID_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> CSI(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_CSI_OFFSET,NVME_CDW11_CSI_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> CNSSI(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW11Addr(),\
				NVME_CDW11_CNSSI_OFFSET,NVME_CDW11_CNSSI_BITS);
	}
public:
	PSIdentify(void)
		:NVMeCmdReq(NVME_ADMIN_OP_IDENTIFY,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetCNS(uint8_t val) {CNS().Set(val);};
	uint8_t GetCNS(void) {return CNS().Get();};

	void SetCNTID(uint16_t val) {CNTID().Set(val);};
	uint16_t GetCNTID(void) {return CNTID().Get();};

	void SetCSI(uint8_t val) {CSI().Set(val);};
	uint16_t GetCSI(void) {return CSI().Get();};

	void SetCNSSI(uint16_t val) {CNSSI().Set(val);};
	uint16_t GetCNSSI(void) {return CNSSI().Get();};
};
class PSAbort: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint16_t> CID(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW10Addr(),\
				NVME_CDW10_CID_OFFSET,NVME_CDW10_CID_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> SQID(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW10Addr(),\
				NVME_CDW10_QID_OFFSET,NVME_CDW10_QID_BITS);
	}
public:
	PSAbort(void)
		:NVMeCmdReq(NVME_ADMIN_OP_ABORT,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetCID(uint16_t val) {CID().Set(val);};
	uint16_t GetCID(void) {return CID().Get();};

	void SetSQID(uint16_t val) {SQID().Set(val);};
	uint16_t GetSQID(void) {return SQID().Get();};
};
class PSSetFeature: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> FID(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_FID_OFFSET,NVME_CDW10_FID_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> SV(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SV_OFFSET,NVME_CDW10_SV_BITS);
	}
public:
	PSSetFeature(void)
		:NVMeCmdReq(NVME_ADMIN_OP_SETFEATURES,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetFID(uint8_t val) {FID().Set(val);};
	uint8_t GetFID(void) {return FID().Get();};

	void SetSVFlag(bool bVal) {SV().Set((uint8_t)bVal);};
	bool GetSVFlag(void) {return (bool)(SV().Get());};

	void SetCDW11(uint32_t val) {cdw11().Set(val);};
	uint32_t GetCDW11(void) {return cdw11().Get();};
};
class PSGetFeature: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> FID(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_FID_OFFSET,NVME_CDW10_FID_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> SEL(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SEL_OFFSET,NVME_CDW10_SEL_BITS);
	}
public:
	PSGetFeature(void)
		:NVMeCmdReq(NVME_ADMIN_OP_GETFEATURES,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetFID(uint8_t val) {FID().Set(val);};
	uint8_t GetFID(void) {return FID().Get();};

	void SetSEL(uint8_t val) {SEL().Set(val);};
	uint8_t GetSEL(void) {return SEL().Get();};

	void SetCDW11(uint32_t val) {cdw11().Set(val);};

};
class PSAsyncEventReq: public NVMeCmdReq
{
public:
	PSAsyncEventReq(void)
		:NVMeCmdReq(NVME_ADMIN_OP_ASYNCEVTREQ,true,DataXfer_PRP,Fused_None)
	{
	}
};
class PSNamespaceMngr: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> SEL(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SEL2_OFFSET,NVME_CDW10_SEL2_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> CSI(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_CSI_OFFSET,NVME_CDW11_CSI_BITS);
	}
public:
	PSNamespaceMngr(void)
		:NVMeCmdReq(NVME_ADMIN_OP_NSMNGR,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetSEL(uint8_t val) {SEL().Set(val);};
	uint8_t GetSEL(void) {return SEL().Get();};

	void SetCSI(uint8_t val) {CSI().Set(val);};
	uint8_t GetCSI(void) {return CSI().Get();};
};
class PSFirmwareCommit: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> FS(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_FS_OFFSET,NVME_CDW10_FS_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> CA(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_CA_OFFSET,NVME_CDW10_CA_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> BPID(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_BPID_OFFSET,NVME_CDW10_BPID_BITS);
	}
public:
	PSFirmwareCommit(void)
		:NVMeCmdReq(NVME_ADMIN_OP_FWCOMMIT,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetFS(uint8_t val) {FS().Set(val);};
	uint8_t GetFS(void) {return FS().Get();};

	void SetCA(uint8_t val) {CA().Set(val);};
	uint8_t GetCA(void) {return CA().Get();};

	void SetBPIDFlag(bool bVal) {BPID().Set((uint8_t)bVal);};
	bool GetBPIDFlag(void) {return (bool)(BPID().Get());};
};
class PSFirmwareDownload: public NVMeCmdReq
{
public:
	PSFirmwareDownload(void)
		:NVMeCmdReq(NVME_ADMIN_OP_FWDNLD,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetNUMD(uint32_t val) {cdw10().Set(val);};
	uint32_t GetNUMD(void) {return cdw10().Get();};

	void SetOFST(uint32_t val) {cdw11().Set(val);};
	uint32_t GetOFST(void) {return cdw11().Get();};
};
class PSDeviceSelfTest: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> STC(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_STC_OFFSET,NVME_CDW10_STC_BITS);
	}
public:
	PSDeviceSelfTest(void)
		:NVMeCmdReq(NVME_ADMIN_OP_DEVSELFTEST,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetSTC(uint8_t val) {STC().Set(val);};
	uint8_t GetSTC(void) {return STC().Get();};
};
class PSNamespaceAttach: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> SEL(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SEL2_OFFSET,NVME_CDW10_SEL2_BITS);
	}
public:
	PSNamespaceAttach(void)
		:NVMeCmdReq(NVME_ADMIN_OP_NSATTACH,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetSEL(uint8_t val) {SEL().Set(val);};
	uint8_t GetSEL(void) {return SEL().Get();};
};
class PSKeepAlive: public NVMeCmdReq
{
public:
	PSKeepAlive(void)
		:NVMeCmdReq(NVME_ADMIN_OP_KEEPALIVE,true,DataXfer_PRP,Fused_None)
	{
	}
};
class PSDirectiveSend: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> DOPER(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_DOPER_OFFSET,NVME_CDW11_DOPER_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> DTYPE(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_DTYPE_OFFSET,NVME_CDW11_DTYPE_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> DSPEC(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW11Addr(),\
				NVME_CDW11_DSPEC_OFFSET,NVME_CDW11_DSPEC_BITS);
	}
public:
	PSDirectiveSend(void)
		:NVMeCmdReq(NVME_ADMIN_OP_DIRTVSEND,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetNUMD(uint32_t val) {cdw10().Set(val);};
	uint32_t GetNUMD(void) {return cdw10().Get();};

	void SetDOPER(uint8_t val) {DOPER().Set(val);};
	uint8_t GetDOPER(void) {return DOPER().Get();};

	void SetDTYPE(uint8_t val) {DTYPE().Set(val);};
	uint8_t GetDTYPE(void) {return DTYPE().Get();};

	void SetDSPEC(uint16_t val) {DSPEC().Set(val);};
	uint16_t GetDSPEC(void) {return DSPEC().Get();};
};
class PSDirectiveRecv: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> DOPER(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_DOPER_OFFSET,NVME_CDW11_DOPER_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> DTYPE(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_DTYPE_OFFSET,NVME_CDW11_DTYPE_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> DSPEC(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW11Addr(),\
				NVME_CDW11_DSPEC_OFFSET,NVME_CDW11_DSPEC_BITS);
	}
public:
	PSDirectiveRecv(void)
		:NVMeCmdReq(NVME_ADMIN_OP_DIRTVRECV,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetNUMD(uint32_t val) {cdw10().Set(val);};
	uint32_t GetNUMD(void) {return cdw10().Get();};

	void SetDOPER(uint8_t val) {DOPER().Set(val);};
	uint8_t GetDOPER(void) {return DOPER().Get();};

	void SetDTYPE(uint8_t val) {DTYPE().Set(val);};
	uint8_t GetDTYPE(void) {return DTYPE().Get();};

	void SetDSPEC(uint16_t val) {DSPEC().Set(val);};
	uint16_t GetDSPEC(void) {return DSPEC().Get();};
};
class PSVirtualMngr: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> ACT(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_ACT_OFFSET,NVME_CDW10_ACT_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> RT(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_RT_OFFSET,NVME_CDW10_RT_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> CNTLID(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW10Addr(),\
				NVME_CDW10_CNTLID_OFFSET,NVME_CDW10_CNTLID_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> NR(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW11Addr(),\
				NVME_CDW11_NR_OFFSET,NVME_CDW11_NR_BITS);
	}
public:
	PSVirtualMngr(void)
		:NVMeCmdReq(NVME_ADMIN_OP_VMMNGR,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetACT(uint8_t val) {ACT().Set(val);};
	uint8_t GetACT(void) {return ACT().Get();};

	void SetRT(uint8_t val) {RT().Set(val);};
	uint8_t GetRT(void) {return RT().Get();};

	void SetCNTLID(uint16_t val) {CNTLID().Set(val);};
	uint16_t GetCNTLID(void) {return CNTLID().Get();};

	void SetNR(uint16_t val) {NR().Set(val);};
	uint16_t GetNR(void) {return NR().Get();};
};
class PSNVMeMISend: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> NMSP0(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_NMSP0_OFFSET,NVME_CDW10_NMSP0_BITS);
	}
public:
	PSNVMeMISend(void)
		:NVMeCmdReq(NVME_ADMIN_OP_NVMEMISEND,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetNMSP0(uint8_t val) {NMSP0().Set(val);};
	uint8_t GetNMSP0(void) {return NMSP0().Get();};

	void SetNUMD(uint32_t val) {cdw11().Set(val);};
	uint32_t GetNUMD(void) {return cdw11().Get();};
};
class PSNVMeMIRecv: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> NMSP0(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_NMSP0_OFFSET,NVME_CDW10_NMSP0_BITS);
	}
public:
	PSNVMeMIRecv(void)
		:NVMeCmdReq(NVME_ADMIN_OP_NVMEMIRECV,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetNMSP0(uint8_t val) {NMSP0().Set(val);};
	uint8_t GetNMSP0(void) {return NMSP0().Get();};
};
class PSDoorbellBufCfg: public NVMeCmdReq
{
public:
	PSDoorbellBufCfg(void)
		:NVMeCmdReq(NVME_ADMIN_OP_DBBUFCFG,true,DataXfer_PRP,Fused_None)
	{
	}
};
class PSFormatNVM: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> LBAF(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_LBAF_OFFSET,NVME_CDW10_LBAF_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> MSET(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_MSET_OFFSET,NVME_CDW10_MSET_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> PI(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_PI_OFFSET,NVME_CDW10_PI_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> PIL(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_PIL_OFFSET,NVME_CDW10_PIL_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> SES(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SES_OFFSET,NVME_CDW10_SES_BITS);
	}
public:
	PSFormatNVM(void)
		:NVMeCmdReq(NVME_ADMIN_OP_FORMATNVM,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetLBAF(uint8_t val) {LBAF().Set(val);};
	uint8_t GetLBAF(void) {return LBAF().Get();};

	void SetMSETFlag(bool val) {MSET().Set((uint8_t)val);};
	bool GetMSETFlag(void) {return (bool)(MSET().Get());};

	void SetPI(uint8_t val) {PI().Set(val);};
	uint8_t GetPI(void) {return PI().Get();};

	void SetPILFlag(bool val) {PIL().Set((uint8_t)val);};
	bool GetPILFlag(void) {return (bool)(PIL().Get());};

	void SetSES(uint8_t val) {SES().Set(val);};
	uint8_t GetSES(void) {return SES().Get();};
};
class PSSecuritySend: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> NSSF(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_NSSF_OFFSET,NVME_CDW10_NSSF_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> SPSP0(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SPSP0_OFFSET,NVME_CDW10_SPSP0_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> SPSP1(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SPSP1_OFFSET,NVME_CDW10_SPSP1_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> SECP(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SECP_OFFSET,NVME_CDW10_SECP_BITS);
	}
public:
	PSSecuritySend(void)
		:NVMeCmdReq(NVME_ADMIN_OP_SECUSEND,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetNSSF(uint8_t val) {NSSF().Set(val);};
	uint8_t GetNSSF(void) {return NSSF().Get();};

	void SetSPSP0(uint8_t val) {SPSP0().Set(val);};
	uint8_t GetSPSP0(void) {return SPSP0().Get();};

	void SetSPSP1(uint8_t val) {SPSP1().Set(val);};
	uint8_t GetSPSP1(void) {return SPSP1().Get();};

	void SetSECP(uint8_t val) {SECP().Set(val);};
	uint8_t GetSECP(void) {return SECP().Get();};

	void SetTL(uint32_t val) {cdw11().Set(val);};
	uint32_t GetTL(void) {return cdw11().Get();};
};

class PSSecurityRecv: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> NSSF(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_NSSF_OFFSET,NVME_CDW10_NSSF_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> SPSP0(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SPSP0_OFFSET,NVME_CDW10_SPSP0_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> SPSP1(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SPSP1_OFFSET,NVME_CDW10_SPSP1_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> SECP(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SECP_OFFSET,NVME_CDW10_SECP_BITS);
	}
public:
	PSSecurityRecv(void)
		:NVMeCmdReq(NVME_ADMIN_OP_SECURECV,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetNSSF(uint8_t val) {NSSF().Set(val);};
	uint8_t GetNSSF(void) {return NSSF().Get();};

	void SetSPSP0(uint8_t val) {SPSP0().Set(val);};
	uint8_t GetSPSP0(void) {return SPSP0().Get();};

	void SetSPSP1(uint8_t val) {SPSP1().Set(val);};
	uint8_t GetSPSP1(void) {return SPSP1().Get();};

	void SetSECP(uint8_t val) {SECP().Set(val);};
	uint8_t GetSECP(void) {return SECP().Get();};

	void SetAL(uint32_t val) {cdw11().Set(val);};
	uint32_t GetAL(void) {return cdw11().Get();};
};
class PSSanitize: public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> SANACT(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_SANACT_OFFSET,NVME_CDW10_SANACT_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> AUSE(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_AUSE_OFFSET,NVME_CDW10_AUSE_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> OWPASS(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_OWPASS_OFFSET,NVME_CDW10_OWPASS_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> OIPBP(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_OIPBP_OFFSET,NVME_CDW10_OIPBP_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> NDAS(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_NDAS_OFFSET,NVME_CDW10_NDAS_BITS);
	}
public:
	PSSanitize(void)
		:NVMeCmdReq(NVME_ADMIN_OP_SANITIZE,true,DataXfer_PRP,Fused_None)
	{
	}

	void SetSANACT(uint8_t val) {SANACT().Set(val);};
	uint8_t GetSANACT(void) {return SANACT().Get();};

	void SetAUSEFlag(bool val) {AUSE().Set((uint8_t)val);};
	bool GetAUSEFlag(void) {return (bool)(AUSE().Get());};

	void SetOWPASS(uint8_t val) {OWPASS().Set(val);};
	uint8_t GetOWPASS(void) {return OWPASS().Get();};

	void SetOIPBPFlag(bool val) {OIPBP().Set((uint8_t)val);};
	bool GetOIPBPFlag(void) {return (bool)(OIPBP().Get());};

	void SetNDASFlag(bool val) {NDAS().Set((uint8_t)val);};
	bool GetNDASFlag(void) {return (bool)(NDAS().Get());};

	void SetOVRPAT(uint32_t val) {cdw11().Set(val);};
	uint32_t GetOVRPAT(void) {return cdw11().Get();};
};

class PSFlush : public NVMeCmdReq
{
public:
	PSFlush(void)
		:NVMeCmdReq(NVME_NVM_OP_FLUSH,false,DataXfer_PRP,Fused_None)
	{
	}
};
class PSWrite : public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint16_t> NLB(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW12Addr(),\
				NVME_CDW12_NLB_OFFSET,NVME_CDW12_NLB_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> DTYPE(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_DTYPE_OFFSET,NVME_CDW12_DTYPE_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> STC(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_STC_OFFSET,NVME_CDW12_STC_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> PRINFO(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_PRINFO_OFFSET,NVME_CDW12_PRINFO_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> FUA(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_FUA_OFFSET,NVME_CDW12_FUA_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> LR(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_LR_OFFSET,NVME_CDW12_LR_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> DSM(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW13Addr(),\
				NVME_CDW13_DSM_OFFSET,NVME_CDW13_DSM_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> DSPEC(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW13Addr(),\
				NVME_CDW13_DSPEC_OFFSET,NVME_CDW13_DSPEC_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> LBAT(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW15Addr(),\
				NVME_CDW15_LBAT_OFFSET,NVME_CDW15_LBAT_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> LBATM(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW15Addr(),\
				NVME_CDW15_LBATM_OFFSET,NVME_CDW15_LBATM_BITS);
	}

public:
	PSWrite(void)
		:NVMeCmdReq(NVME_NVM_OP_WRITE,false,DataXfer_PRP,Fused_None)
	{
	}

	void SetSLBA(uint64_t qwVal)
	{
		cdw10().Set((uint32_t)(qwVal & 0xFFFFFFFF));
		cdw11().Set((uint32_t)((qwVal>>32) & 0xFFFFFFFF));
	}
	uint64_t GetSLBA(void)
	{
		return ( (uint64_t)(cdw11().Get() & 0xFFFFFFFF) << 32 |\
				(uint64_t)(cdw10().Get() & 0xFFFFFFFF) );
	}

	void SetNLB(uint16_t val) {NLB().Set(val);};
	uint16_t GetNLB(void) {return NLB().Get();};

	void SetDTYPE(uint8_t val) {DTYPE().Set(val);};
	uint8_t GetDTYPE(void) {return DTYPE().Get();};

	void SetPRINFO(uint8_t val) {PRINFO().Set(val);};
	uint8_t GetPRINFO(void) {return PRINFO().Get();};

	void SetFUAFlag(bool val) {FUA().Set((uint8_t)val);};
	bool GetFUAFlag(void) {return (bool)(FUA().Get());};

	void SetLRFlag(bool val) {LR().Set((uint8_t)val);};
	bool GetLRFlag(void) {return (bool)(LR().Get());};

	void SetSTCFlag(bool val) {STC().Set((uint8_t)val);};
	bool GetSTCFlag(void) {return (bool)(STC().Get());};

	void SetDSM(uint8_t val) {DSM().Set(val);};
	uint8_t GetDSM(void) {return DSM().Get();};

	void SetDSPEC(uint16_t val) {DSPEC().Set(val);};
	uint16_t GetDSPEC(void) {return DSPEC().Get();};

	void SetLBAT(uint16_t val) {LBAT().Set(val);};
	uint16_t GetLBAT(void) {return LBAT().Get();};

	void SetLBATM(uint16_t val) {LBATM().Set(val);};
	uint16_t GetLBATM(void) {return LBATM().Get();};

	void SetILBRT(uint32_t val) {cdw14().Set(val);};
	uint32_t GetILBRT(void) {return cdw14().Get();};
};
class PSRead : public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint16_t> NLB(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW12Addr(),\
				NVME_CDW12_NLB_OFFSET,NVME_CDW12_NLB_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> STC(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_STC_OFFSET,NVME_CDW12_STC_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> PRINFO(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_PRINFO_OFFSET,NVME_CDW12_PRINFO_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> FUA(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_FUA_OFFSET,NVME_CDW12_FUA_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> LR(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_LR_OFFSET,NVME_CDW12_LR_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> DSM(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW13Addr(),\
				NVME_CDW13_DSM_OFFSET,NVME_CDW13_DSM_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> LBAT(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW15Addr(),\
				NVME_CDW15_LBAT_OFFSET,NVME_CDW15_LBAT_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> LBATM(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW15Addr(),\
				NVME_CDW15_LBATM_OFFSET,NVME_CDW15_LBATM_BITS);
	}

public:
	PSRead(void)
		:NVMeCmdReq(NVME_NVM_OP_READ,false,DataXfer_PRP,Fused_None)
	{
	}

	void SetSLBA(uint64_t qwVal)
	{
		cdw10().Set((uint32_t)(qwVal & 0xFFFFFFFF));
		cdw11().Set((uint32_t)((qwVal>>32) & 0xFFFFFFFF));
	}
	uint64_t GetSLBA(void)
	{
		return ( (uint64_t)(cdw11().Get() & 0xFFFFFFFF) << 32 |\
				(uint64_t)(cdw10().Get() & 0xFFFFFFFF) );
	}

	void SetNLB(uint16_t val) {NLB().Set(val);};
	uint16_t GetNLB(void) {return NLB().Get();};

	void SetPRINFO(uint8_t val) {PRINFO().Set(val);};
	uint8_t GetPRINFO(void) {return PRINFO().Get();};

	void SetFUAFlag(bool val) {FUA().Set((uint8_t)val);};
	bool GetFUAFlag(void) {return (bool)(FUA().Get());};

	void SetLRFlag(bool val) {LR().Set((uint8_t)val);};
	bool GetLRFlag(void) {return (bool)(LR().Get());};

	void SetSTCFlag(bool val) {STC().Set((uint8_t)val);};
	bool GetSTCFlag(void) {return (bool)(STC().Get());};

	void SetDSM(uint8_t val) {DSM().Set(val);};
	uint8_t GetDSM(void) {return DSM().Get();};

	void SetELBAT(uint16_t val) {LBAT().Set(val);};
	uint16_t GetELBAT(void) {return LBAT().Get();};

	void SetELBATM(uint16_t val) {LBATM().Set(val);};
	uint16_t GetELBATM(void) {return LBATM().Get();};

	void SetEILBRT(uint32_t val) {cdw14().Set(val);};
	uint32_t GetEILBRT(void) {return cdw14().Get();};
};
class PSWriteUNC : public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint16_t> NLB(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW12Addr(),\
				NVME_CDW12_NLB_OFFSET,NVME_CDW12_NLB_BITS);
	}
public:
	PSWriteUNC(void)
		:NVMeCmdReq(NVME_NVM_OP_WRITEUNC,false,DataXfer_PRP,Fused_None)
	{
	}

	void SetSLBA(uint64_t qwVal)
	{
		cdw10().Set((uint32_t)(qwVal & 0xFFFFFFFF));
		cdw11().Set((uint32_t)((qwVal>>32) & 0xFFFFFFFF));
	}
	uint64_t GetSLBA(void)
	{
		return ( (uint64_t)(cdw11().Get() & 0xFFFFFFFF) << 32 |\
				(uint64_t)(cdw10().Get() & 0xFFFFFFFF) );
	}

	void SetNLB(uint16_t val) {NLB().Set(val);};
	uint16_t GetNLB(void) {return NLB().Get();};
};
class PSCompare : public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint16_t> NLB(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW12Addr(),\
				NVME_CDW12_NLB_OFFSET,NVME_CDW12_NLB_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> STC(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_STC_OFFSET,NVME_CDW12_STC_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> PRINFO(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_PRINFO_OFFSET,NVME_CDW12_PRINFO_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> FUA(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_FUA_OFFSET,NVME_CDW12_FUA_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> LR(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_LR_OFFSET,NVME_CDW12_LR_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> LBAT(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW15Addr(),\
				NVME_CDW15_LBAT_OFFSET,NVME_CDW15_LBAT_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> LBATM(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW15Addr(),\
				NVME_CDW15_LBATM_OFFSET,NVME_CDW15_LBATM_BITS);
	}

public:
	PSCompare(void)
		:NVMeCmdReq(NVME_NVM_OP_COMPARE,false,DataXfer_PRP,Fused_None)
	{
	}

	void SetSLBA(uint64_t qwVal)
	{
		cdw10().Set((uint32_t)(qwVal & 0xFFFFFFFF));
		cdw11().Set((uint32_t)((qwVal>>32) & 0xFFFFFFFF));
	}
	uint64_t GetSLBA(void)
	{
		return ( (uint64_t)(cdw11().Get() & 0xFFFFFFFF) << 32 |\
				(uint64_t)(cdw10().Get() & 0xFFFFFFFF) );
	}

	void SetNLB(uint16_t val) {NLB().Set(val);};
	uint16_t GetNLB(void) {return NLB().Get();};

	void SetPRINFO(uint8_t val) {PRINFO().Set(val);};
	uint8_t GetPRINFO(void) {return PRINFO().Get();};

	void SetFUAFlag(bool val) {FUA().Set((uint8_t)val);};
	bool GetFUAFlag(void) {return (bool)(FUA().Get());};

	void SetLRFlag(bool val) {LR().Set((uint8_t)val);};
	bool GetLRFlag(void) {return (bool)(LR().Get());};

	void SetSTCFlag(bool val) {STC().Set((uint8_t)val);};
	bool GetSTCFlag(void) {return (bool)(STC().Get());};

	void SetELBAT(uint16_t val) {LBAT().Set(val);};
	uint16_t GetELBAT(void) {return LBAT().Get();};

	void SetELBATM(uint16_t val) {LBATM().Set(val);};
	uint16_t GetELBATM(void) {return LBATM().Get();};

	void SetEILBRT(uint32_t val) {cdw14().Set(val);};
	uint32_t GetEILBRT(void) {return cdw14().Get();};
};
class PSWriteZeroes : public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint16_t> NLB(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW12Addr(),\
				NVME_CDW12_NLB_OFFSET,NVME_CDW12_NLB_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> DEAC(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_DEAC_OFFSET,NVME_CDW12_DEAC_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> PRINFO(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_PRINFO_OFFSET,NVME_CDW12_PRINFO_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> FUA(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_FUA_OFFSET,NVME_CDW12_FUA_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> LR(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW12Addr(),\
				NVME_CDW12_LR_OFFSET,NVME_CDW12_LR_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> LBAT(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW15Addr(),\
				NVME_CDW15_LBAT_OFFSET,NVME_CDW15_LBAT_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> LBATM(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(GetCDW15Addr(),\
				NVME_CDW15_LBATM_OFFSET,NVME_CDW15_LBATM_BITS);
	}

public:
	PSWriteZeroes(void)
		:NVMeCmdReq(NVME_NVM_OP_WRITEZERO,false,DataXfer_PRP,Fused_None)
	{
	}

	void SetSLBA(uint64_t qwVal)
	{
		cdw10().Set((uint32_t)(qwVal & 0xFFFFFFFF));
		cdw11().Set((uint32_t)((qwVal>>32) & 0xFFFFFFFF));
	}
	uint64_t GetSLBA(void)
	{
		return ( (uint64_t)(cdw11().Get() & 0xFFFFFFFF) << 32 |\
				(uint64_t)(cdw10().Get() & 0xFFFFFFFF) );
	}

	void SetNLB(uint16_t val) {NLB().Set(val);};
	uint16_t GetNLB(void) {return NLB().Get();};

	void SetDEACFlag(bool val) {DEAC().Set((uint8_t)val);};
	bool GetDEACFlag(void) {return (bool)(DEAC().Get());};

	void SetPRINFO(uint8_t val) {PRINFO().Set(val);};
	uint8_t GetPRINFO(void) {return PRINFO().Get();};

	void SetFUAFlag(bool val) {FUA().Set((uint8_t)val);};
	bool GetFUAFlag(void) {return (bool)(FUA().Get());};

	void SetLRFlag(bool val) {LR().Set((uint8_t)val);};
	bool GetLRFlag(void) {return (bool)(LR().Get());};

	void SetLBAT(uint16_t val) {LBAT().Set(val);};
	uint16_t GetLBAT(void) {return LBAT().Get();};

	void SetLBATM(uint16_t val) {LBATM().Set(val);};
	uint16_t GetLBATM(void) {return LBATM().Get();};

	void SetILBRT(uint32_t val) {cdw14().Set(val);};
	uint32_t GetILBRT(void) {return cdw14().Get();};
};
class PSDataSetMngr : public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> NR(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_NR_OFFSET,NVME_CDW10_NR_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> IDR(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_IDR_OFFSET,NVME_CDW11_IDR_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> IDW(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_IDW_OFFSET,NVME_CDW11_IDW_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> AD(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_AD_OFFSET,NVME_CDW11_AD_BITS);
	}
public:
	PSDataSetMngr(void)
		:NVMeCmdReq(NVME_NVM_OP_DATASETMNG,false,DataXfer_PRP,Fused_None)
	{
	}

	void SetNR(uint8_t val) {NR().Set(val);};
	uint8_t GetNR(void) {return NR().Get();};

	void SetIDRFlag(bool val) {IDR().Set((uint8_t)val);};
	bool GetIDRFlag(void) {return (bool)(IDR().Get());};

	void SetIDWFlag(bool val) {IDW().Set((uint8_t)val);};
	bool GetIDWFlag(void) {return (bool)(IDW().Get());};

	void SetADFlag(bool val) {AD().Set((uint8_t)val);};
	bool GetADFlag(void) {return (bool)(AD().Get());};
};
class PSResvRegister : public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> RREGA(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_RREGA_OFFSET,NVME_CDW10_RREGA_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> IEKEY(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_IEKEY_OFFSET,NVME_CDW10_IEKEY_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> CPTPL(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_CPTPL_OFFSET,NVME_CDW10_CPTPL_BITS);
	}
public:
	PSResvRegister(void)
		:NVMeCmdReq(NVME_NVM_OP_RSVTREG,false,DataXfer_PRP,Fused_None)
	{
	}

	void SetRREGA(uint8_t val) {RREGA().Set(val);};
	uint8_t GetRREGA(void) {return RREGA().Get();};

	void SetIEKEYFlag(bool val) {IEKEY().Set((uint8_t)val);};
	bool GetIEKEYFlag(void) {return (bool)(IEKEY().Get());};

	void SetCPTPL(uint8_t val) {CPTPL().Set(val);};
	uint8_t GetCPTPL(void) {return CPTPL().Get();};
};
class PSResvReport : public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> EDS(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW11Addr(),\
				NVME_CDW11_EDS_OFFSET,NVME_CDW11_EDS_BITS);
	}
public:
	PSResvReport(void)
		:NVMeCmdReq(NVME_NVM_OP_RSVTRPT,false,DataXfer_PRP,Fused_None)
	{
	}

	void SetNUMD(uint32_t val) {cdw10().Set(val);};
	uint32_t GetNUMD(void) {return cdw10().Get();};

	void SetEDSFlag(bool val) {EDS().Set((uint8_t)val);};
	bool GetEDSFlag(void) {return (bool)(EDS().Get());};
};
class PSResvAcquire : public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> RACQA(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_RACQA_OFFSET,NVME_CDW10_RACQA_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> IEKEY(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_IEKEY_OFFSET,NVME_CDW10_IEKEY_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> RTYPE(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_RTYPE_OFFSET,NVME_CDW10_RTYPE_BITS);
	}
public:
	PSResvAcquire(void)
		:NVMeCmdReq(NVME_NVM_OP_RSVTACQ,false,DataXfer_PRP,Fused_None)
	{
	}

	void SetRACQA(uint8_t val) {RACQA().Set(val);};
	uint8_t GetRACQA(void) {return RACQA().Get();};

	void SetRTYPE(uint8_t val) {RTYPE().Set(val);};
	uint8_t GetRTYPE(void) {return RTYPE().Get();};

	void SetIEKEYFlag(bool val) {IEKEY().Set((uint8_t)val);};
	bool GetIEKEYFlag(void) {return (bool)(IEKEY().Get());};
};
class PSResvRelease : public NVMeCmdReq
{
protected:
	ClassMemberBitMask<uint32_t,uint8_t> RRELA(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_RRELA_OFFSET,NVME_CDW10_RRELA_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> IEKEY(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_IEKEY_OFFSET,NVME_CDW10_IEKEY_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> RTYPE(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(GetCDW10Addr(),\
				NVME_CDW10_RTYPE_OFFSET,NVME_CDW10_RTYPE_BITS);
	}
public:
	PSResvRelease(void)
		:NVMeCmdReq(NVME_NVM_OP_RSVTREL,false,DataXfer_PRP,Fused_None)
	{
	}

	void SetRRELA(uint8_t val) {RRELA().Set(val);};
	uint8_t GetRRELA(void) {return RRELA().Get();};

	void SetRTYPE(uint8_t val) {RTYPE().Set(val);};
	uint8_t GetRTYPE(void) {return RTYPE().Get();};

	void SetIEKEYFlag(bool val) {IEKEY().Set((uint8_t)val);};
	bool GetIEKEYFlag(void) {return (bool)(IEKEY().Get());};
};

#endif
