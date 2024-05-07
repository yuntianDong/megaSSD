/*
 * NVMeMIPrimitives.h
 *
 */

#ifndef _PROTOCAL_NVME_MI_NVMEMIPRIMITIVES_H_
#define _PROTOCAL_NVME_MI_NVMEMIPRIMITIVES_H_

#include "protocol/mctp/MCTPRegMap.h"

#define NMCP_CPO_PAUSE				0x0
#define NMCP_CPO_RESUME				0x1
#define NMCP_CPO_ABORT				0x2
#define NMCP_CPO_GETSTATE			0x3
#define NMCP_CPO_REPLAY				0x4

#define NVMeMI_PRIM_REQ_LEN			12
#define NVMeMI_PRIM_RESP_LEN		12

#define NVMeMI_RESP_INVALID			0xFF
#define NVMeMI_RESP_S_SUCCEEDED		0

class NVMeMIPrimReq : public MCTPNVMeMIMsgRegMap
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("CPO",4 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("TAG",5 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CPSP",6 , 0 , 16 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CRC",8 , 0 ,32 , REG_OFF_F2B);
	}
	virtual void InitRegVal(void)
	{
		this->SetROR(false);
		this->SetNVMeMIMsgType(NMCP_NMIMT_PRIMITIVE);
	}
public:
	NVMeMIPrimReq(void)
	{
		InitRegTbl();
		InitRegVal();
	};

	virtual uint16_t	GetLength(void)	{return NVMeMI_PRIM_REQ_LEN;};

	bool SetCPO(uint8_t val){return this->SetRegVal("CPO",(uint64_t)val);};
	uint8_t GetCPO(void){return (uint8_t)(this->GetRegVal("CPO"));};

	bool SetTag(uint8_t val){return this->SetRegVal("TAG",(uint64_t)val);};
	uint8_t GetTag(void){return (uint8_t)(this->GetRegVal("TAG"));};

	bool SetCPSP(uint16_t val){return this->SetRegVal("CPSP",(uint64_t)val);};
	uint16_t GetCPSP(void){return (uint16_t)(this->GetRegVal("CPSP"));};
};

class NVMeMIPrimResp : public MCTPNVMeMIMsgRegMap
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("Status",4 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("TAG",5 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CPSR",6 , 0 , 16 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CRC",8 , 0 ,32 , REG_OFF_F2B);
	}
	virtual void InitRegVal(void)
	{
		this->SetROR(true);
		this->SetNVMeMIMsgType(NMCP_NMIMT_PRIMITIVE);
	}
public:
	NVMeMIPrimResp(void)
	{
		InitRegTbl();
		InitRegVal();

		SetStatus(NVMeMI_RESP_INVALID);
	};
	NVMeMIPrimResp(Buffers* regMapBuf)
		:MCTPNVMeMIMsgRegMap(regMapBuf)
	{
		InitRegTbl();
		InitRegVal();

		SetStatus(NVMeMI_RESP_INVALID);
	};

	uint16_t	GetLength(void)	{return NVMeMI_PRIM_RESP_LEN;};

	bool Succeeded(void) {return NVMeMI_RESP_S_SUCCEEDED == GetStatus();};

	bool SetStatus(uint8_t val){return this->SetRegVal("Status",(uint64_t)val);};
	uint8_t GetStatus(void){return (uint8_t)(this->GetRegVal("Status"));};

	bool SetTag(uint8_t val){return this->SetRegVal("TAG",(uint64_t)val);};
	uint8_t GetTag(void){return (uint8_t)(this->GetRegVal("TAG"));};

	bool SetCPSR(uint16_t val){return this->SetRegVal("CPSR",(uint64_t)val);};
	uint16_t GetCPSR(void){return (uint16_t)(this->GetRegVal("CPSR"));};
};

/*
 * NVMe-MI Control Primitive Commands
 */

class NMCPCmdPause : public NVMeMIPrimReq
{
protected:
	virtual void InitRegVal(void)
	{
		this->SetCPO(NMCP_CPO_PAUSE);
	}
public:
	NMCPCmdPause(void)
	{
		InitRegVal();
	}
};

class NMCPCmdResume : public NVMeMIPrimReq
{
protected:
	virtual void InitRegVal(void)
	{
		this->SetCPO(NMCP_CPO_RESUME);
	}
public:
	NMCPCmdResume(void)
	{
		InitRegVal();
	}
};

class NMCPCmdAbort : public NVMeMIPrimReq
{
protected:
	virtual void InitRegVal(void)
	{
		this->SetCPO(NMCP_CPO_ABORT);
	}
public:
	NMCPCmdAbort(void)
	{
		InitRegVal();
	}
};

class NMCPCmdGetState : public NVMeMIPrimReq
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("CESF",6 , 0 , 1 , REG_OFF_F2B);
	}
	virtual void InitRegVal(void)
	{
		this->SetCPO(NMCP_CPO_GETSTATE);
	}
public:
	NMCPCmdGetState(void)
	{
		InitRegTbl();
		InitRegVal();
	}

	bool SetCESF(bool val){return this->SetRegVal("CESF",(uint64_t)val);};
	bool GetCESF(void){return (bool)(this->GetRegVal("CESF"));};
};

class NMCPCmdReplay : public NVMeMIPrimReq
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("RRO",6 , 0 , 8 , REG_OFF_F2B);
	}
	virtual void InitRegVal(void)
	{
		this->SetCPO(NMCP_CPO_REPLAY);
	}
public:
	NMCPCmdReplay(void)
	{
		InitRegTbl();
		InitRegVal();
	}

	bool SetRRO(uint8_t val){return this->SetRegVal("RRO",(uint64_t)val);};
	uint8_t GetRRO(void){return (uint8_t)(this->GetRegVal("RRO"));};
};

#endif /* _PROTOCAL_NVME_MI_NVMEMIPRIMITIVES_H_ */
