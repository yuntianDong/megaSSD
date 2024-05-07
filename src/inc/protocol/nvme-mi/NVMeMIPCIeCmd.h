/*
 * NVMeMIPCIeCmd.h
 *
 */

#ifndef _PROTOCAL_NVME_MI_NVMEMIPCIECMD_H_
#define _PROTOCAL_NVME_MI_NVMEMIPCIECMD_H_

#include "protocol/mctp/MCTPRegMap.h"

#define MIPCIE_OPC_CFGREAD			0x0
#define MIPCIE_OPC_CFGWRITE			0x1
#define MIPCIE_OPC_MEMREAD			0x2
#define MIPCIE_OPC_MEMWRITE			0x3
#define MIPCIE_OPC_IOREAD			0x4
#define MIPCIE_OPC_IOWRITE			0x5

#define MIPCI_RESP_S_SUCCEEDED		0

class MIPCIeCmdReq : public MCTPNVMeMIMsgRegMap
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("OPC",4 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("ReservedB5",5 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CTLID",6 , 0 , 16 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NMD0",8 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NMD1",12 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NMD2",16 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("RequestData",20 , 0 , REG_LEN_UNDEF , REG_OFF_F2B);
	};
	virtual void InitRegVal(void)
	{
		this->SetROR(false);
		this->SetNVMeMIMsgType(NMCP_NMIMT_PCIECMD);
	};
public:
	MIPCIeCmdReq(void)
	{
		InitRegTbl();
		InitRegVal();
	}

	bool SetOPC(uint8_t val){return this->SetRegVal("OPC",(uint64_t)val);};
	uint8_t GetOPC(void){return (uint8_t)(this->GetRegVal("OPC"));};

	bool SetCTLID(uint16_t val){return this->SetRegVal("CTLID",(uint64_t)val);};
	uint16_t GetCTLID(void){return (uint16_t)(this->GetRegVal("CTLID"));};

	bool SetNMD0(uint32_t val){return this->SetRegVal("NMD0",(uint64_t)val);};
	uint32_t GetNMD0(void){return (uint32_t)(this->GetRegVal("NMD0"));};

	bool SetNMD1(uint32_t val){return this->SetRegVal("NMD1",(uint64_t)val);};
	uint32_t GetNMD1(void){return (uint32_t)(this->GetRegVal("NMD1"));};

	bool SetNMD2(uint32_t val){return this->SetRegVal("NMD2",(uint64_t)val);};
	uint32_t GetNMD2(void){return (uint32_t)(this->GetRegVal("NMD2"));};

	bool SetRequestData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		return this->SetRegByBuf("RequestData",buf,offset,length);
	};
	bool GetRequestData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		return this->GetRegByBuf("RequestData",buf,offset,length);
	};
};

class MIPCIeCmdResp : public MCTPNVMeMIMsgRegMap
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("Status",4 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("ReservedB567",5 , 0 , 24 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("ResponseData",8 , 0 , REG_LEN_UNDEF , REG_OFF_F2B);
	};
	virtual void InitRegVal(void)
	{
		this->SetROR(true);
		this->SetNVMeMIMsgType(NMCP_NMIMT_PCIECMD);
	};
public:
	MIPCIeCmdResp(void)
	{
		InitRegTbl();
		InitRegVal();
	}
	MIPCIeCmdResp(Buffers* regMapBuf)
		:MCTPNVMeMIMsgRegMap(regMapBuf)
	{
		InitRegTbl();
		InitRegVal();
	};

	bool Succeeded(void) {return MIPCI_RESP_S_SUCCEEDED == GetStatus();};

	bool SetStatus(uint8_t val){return this->SetRegVal("Status",(uint64_t)val);};
	uint8_t GetStatus(void){return (uint8_t)(this->GetRegVal("Status"));};

	bool SetResponseData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		return this->SetRegByBuf("ResponseData",buf,offset,length);
	};
	bool GetResponseData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		return this->GetRegByBuf("ResponseData",buf,offset,length);
	};
};

/*
 * PCIe Commands
 */
class MIPCIeCfgCmd : public MIPCIeCmdReq
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("Length",8 , 0 , 16 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("Offset",12 , 0 , 12 , REG_OFF_F2B);
	};
public:
	MIPCIeCfgCmd(void){InitRegTbl();};

	bool SetLength(uint16_t val){return this->SetRegVal("Length",(uint64_t)val);};
	uint16_t GetLength(void){return (uint16_t)(this->GetRegVal("Length"));};

	bool SetOffset(uint16_t val){return this->SetRegVal("Offset",(uint64_t)val);};
	uint16_t GetOffset(void){return (uint16_t)(this->GetRegVal("Offset"));};
};

class MIPCIeCfgRead : public MIPCIeCfgCmd
{
protected:
	virtual void InitRegVal(void)
	{
		SetOPC(MIPCIE_OPC_CFGREAD);
	};
public:
	MIPCIeCfgRead(void) {InitRegVal();};
};

class MIPCIeCfgWrite : public MIPCIeCfgCmd
{
protected:
	virtual void InitRegVal(void)
	{
		SetOPC(MIPCIE_OPC_CFGWRITE);
	};
public:
	MIPCIeCfgWrite(void) {InitRegVal();};
};

class MIPCIeIOCmd : public MIPCIeCmdReq
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("Length",8 , 0 , 16 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("BAR",10 , 0 , 3 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("Offset",12 , 0 , 32 , REG_OFF_F2B);
	};
public:
	MIPCIeIOCmd(void){InitRegTbl();};

	bool SetBAR(uint8_t val){return this->SetRegVal("BAR",(uint64_t)val);};
	uint8_t GetBAR(void){return (uint8_t)(this->GetRegVal("BAR"));};

	bool SetLength(uint16_t val){return this->SetRegVal("Length",(uint64_t)val);};
	uint16_t GetLength(void){return (uint16_t)(this->GetRegVal("Length"));};

	bool SetOffset(uint32_t val){return this->SetRegVal("Offset",(uint64_t)val);};
	uint32_t GetOffset(void){return (uint32_t)(this->GetRegVal("Offset"));};
};

class MIPCIeIORead : public MIPCIeIOCmd
{
protected:
	virtual void InitRegVal(void)
	{
		SetOPC(MIPCIE_OPC_IOREAD);
	};
public:
	MIPCIeIORead(void) {InitRegVal();};
};

class MIPCIeIOWrite : public MIPCIeIOCmd
{
protected:
	virtual void InitRegVal(void)
	{
		SetOPC(MIPCIE_OPC_IOWRITE);
	};
public:
	MIPCIeIOWrite(void) {InitRegVal();};
};

class MIPCIeMemCmd : public MIPCIeCmdReq
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("Length",8 , 0 , 16 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("BAR",10 , 0 , 3 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("OffsetLow",12 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("OffsetHigh",16 , 0 , 32 , REG_OFF_F2B);
	};
public:
	MIPCIeMemCmd(void){InitRegTbl();};

	bool SetBAR(uint8_t val){return this->SetRegVal("BAR",(uint64_t)val);};
	uint8_t GetBAR(void){return (uint8_t)(this->GetRegVal("BAR"));};

	bool SetLength(uint16_t val){return this->SetRegVal("Length",(uint64_t)val);};
	uint16_t GetLength(void){return (uint16_t)(this->GetRegVal("Length"));};

	bool SetOffsetLow(uint32_t val){return this->SetRegVal("OffsetLow",(uint64_t)val);};
	uint32_t GetOffsetLow(void){return (uint32_t)(this->GetRegVal("OffsetLow"));};

	bool SetOffsetHigh(uint32_t val){return this->SetRegVal("OffsetHigh",(uint64_t)val);};
	uint32_t GetOffsetHigh(void){return (uint32_t)(this->GetRegVal("OffsetHigh"));};
};

class MIPCIeMemRead : public MIPCIeMemCmd
{
protected:
	virtual void InitRegVal(void)
	{
		SetOPC(MIPCIE_OPC_MEMREAD);
	};
public:
	MIPCIeMemRead(void) {InitRegVal();};
};

class MIPCIeMemWrite : public MIPCIeMemCmd
{
protected:
	virtual void InitRegVal(void)
	{
		SetOPC(MIPCIE_OPC_MEMWRITE);
	};
public:
	MIPCIeMemWrite(void) {InitRegVal();};
};

#endif /* _PROTOCAL_NVME_MI_NVMEMIPCIECMD_H_ */
