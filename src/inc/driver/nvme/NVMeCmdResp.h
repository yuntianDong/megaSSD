#ifndef _NVME_CMD_RESP_H
#define _NVME_CMD_RESP_H

#include <string.h>

#include "../CmdResp.h"
#include "ClassMember.h"
#include "Logging.h"

#define NVME_DW2_SQID_OFFSET				16
#define NVME_DW2_SQID_BITS					16
#define NVME_DW2_SQHP_OFFSET				0
#define NVME_DW2_SQHP_BITS					16
#define NVME_DW3_SF_OFFSET					17
#define NVME_DW3_SF_BITS					15
#define NVME_DW3_PTAG_OFFSET				16
#define NVME_DW3_PTAG_BITS					1
#define NVME_DW3_CID_OFFSET					0
#define NVME_DW3_CID_BITS					16

#define NVME_SF_SC_OFFSET					17
#define NVME_SF_SC_BITS						8
#define NVME_SF_SCT_OFFSET					25
#define NVME_SF_SCT_BITS					3
#define NVME_SF_M_OFFSET					30
#define NVME_SF_M_BITS						1
#define NVME_SF_DNR_OFFSET					31
#define NVME_SF_DNR_BITS					1

#define CQ_ENTRY_DWORDS						4
#define SQ_ENTRY_DWORDS						16
#define NVME_CMD_SUCCESSFUL					0x0
#define NVME_CMD_IOCTLERR					0xFF

class NVMeCmdResp : public CmdResp
{
private:
	uint32_t	mCqEntry[CQ_ENTRY_DWORDS];
	uint32_t	mSqEntry[SQ_ENTRY_DWORDS];

	Buffers*	mCmdMetaBuf;
protected:
	ClassMemberBitMask<uint32_t,uint16_t> SQID(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(&(mCqEntry[2]),\
				NVME_DW2_SQID_OFFSET,NVME_DW2_SQID_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> SQHP(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(&(mCqEntry[2]),\
				NVME_DW2_SQHP_OFFSET,NVME_DW2_SQHP_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> SF(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(&(mCqEntry[3]),\
				NVME_DW3_SF_OFFSET,NVME_DW3_SF_BITS);
	}
	ClassMemberBitMask<uint32_t,uint16_t> CID(void)
	{
		return ClassMemberBitMask<uint32_t,uint16_t>(&(mCqEntry[3]),\
				NVME_DW3_CID_OFFSET,NVME_DW3_CID_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> PTAG(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(&(mCqEntry[3]),\
				NVME_DW3_PTAG_OFFSET,NVME_DW3_PTAG_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> SC(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(&(mCqEntry[3]),\
				NVME_SF_SC_OFFSET,NVME_SF_SC_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> SCT(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(&(mCqEntry[3]),\
				NVME_SF_SCT_OFFSET,NVME_SF_SCT_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> M(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(&(mCqEntry[3]),\
				NVME_SF_M_OFFSET,NVME_SF_M_BITS);
	}
	ClassMemberBitMask<uint32_t,uint8_t> DNR(void)
	{
		return ClassMemberBitMask<uint32_t,uint8_t>(&(mCqEntry[3]),\
				NVME_SF_DNR_OFFSET,NVME_SF_DNR_BITS);
	}
public:
	NVMeCmdResp(void)
	{
		memset(mCqEntry,0,sizeof(uint32_t)*CQ_ENTRY_DWORDS);
		memset(mSqEntry,0,sizeof(uint32_t)*SQ_ENTRY_DWORDS);

		mCmdMetaBuf = NULL;

		SetSC(NVME_CMD_IOCTLERR);
	}

	void Dump(void)
	{
		LOGINFO("CDW0\t=0x%02x",GetRespVal());
		LOGINFO("CDW1\t=0x%02x",GetDW1());
		LOGINFO("SQID\t=0x%02x",GetSQID());
		LOGINFO("SQHP\t=0x%02x",GetSQHP());
		LOGINFO("SF\t=0x%02x",GetStatus());
		LOGINFO("CID\t=0x%02x",GetCID());
		LOGINFO("PTAG\t=0x%02x",GetPhaseTag());
		LOGINFO("SC\t=0x%02x",GetSC());
		LOGINFO("SCT\t=0x%02x",GetSCT());
		LOGINFO("M\t=0x%02x",GetMFlag());
		LOGINFO("DNR\t=0x%02x",GetDNRFlag());
		LOGINFO("DataBuffer\t=0x%lx",(uint64_t)GetDataBuffer());
		LOGINFO("MetaBuffer\t=0x%lx",(uint64_t)GetMetaBuffer());
		LOGINFO("ExecTime\t=0x%x",GetExecTime());
	}

	bool Succeeded(void) {return NVME_CMD_SUCCESSFUL == GetSC();};

	void SetMetaBuffer(Buffers* buf) {mCmdMetaBuf = buf;};
	Buffers* GetMetaBuffer(void) {return mCmdMetaBuf;};

	void* GetRespAddr(void) {return (void*)mCqEntry;};
	uint32_t GetRespSize(void) {return sizeof(uint32_t)*CQ_ENTRY_DWORDS;};

	void* GetCmdAddr(void) {return (void*)mSqEntry;};
	uint32_t GetCmdSize(void) {return sizeof(uint32_t)*SQ_ENTRY_DWORDS;};

	void SetMFlag(bool val) {M().Set((uint8_t)val);};
	bool GetMFlag(void) {return (bool)(M().Get());};

	void SetDNRFlag(bool val) {DNR().Set((uint8_t)val);};
	bool GetDNRFlag(void) {return (bool)(DNR().Get());};

	void SetSC(uint8_t val) {SC().Set(val);};
	uint8_t GetSC(void) {return SC().Get();};

	void SetSCT(uint8_t val) {SCT().Set(val);};
	uint8_t GetSCT(void) {return SCT().Get();};

	void SetRespVal(uint32_t val) {mCqEntry[0] = val;};
	uint32_t GetRespVal(void) {return mCqEntry[0];};

	void SetDW1(uint32_t val) {mCqEntry[1] = val;};
	uint32_t GetDW1(void) {return mCqEntry[1];};

	void SetDW3(uint32_t val) {mCqEntry[3] = val;};
	uint32_t GetDW3(void) {return mCqEntry[3];};

	void SetSQID(uint16_t val) {SQID().Set(val);};
	uint16_t GetSQID(void) {return SQID().Get();};

	void SetSQHP(uint16_t val) {SQHP().Set(val);};
	uint16_t GetSQHP(void) {return SQHP().Get();};

	void SetStatus(uint16_t val) {SF().Set(val);};
	uint16_t GetStatus(void) {return SF().Get();};

	void SetCID(uint16_t val) {CID().Set(val);};
	uint16_t GetCID(void) {return CID().Get();};

	void SetPhaseTag(bool val) {PTAG().Set((uint8_t)val);};
	bool GetPhaseTag(void) {return (bool)(PTAG().Get());};
};

#endif
