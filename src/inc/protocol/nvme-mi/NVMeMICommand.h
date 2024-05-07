/*
 * NVMeMICommand.h
 *
 */

#ifndef _PROTOCAL_NVME_MI_NVMEMICOMMAND_H_
#define _PROTOCAL_NVME_MI_NVMEMICOMMAND_H_

#include "protocol/mctp/MCTPRegMap.h"
#include "Logging.h"

#define NVMIC_OPC_RDNVMEMI			0x0
#define NVMIC_OPC_NVMSHSP			0x1
#define NVMIC_OPC_CTLRHSP			0x2
#define NVMIC_OPC_CFGSET			0x3
#define NVMIC_OPC_CFGGET			0x4
#define NVMIC_OPC_VPDREAD			0x5
#define NVMIC_OPC_VPDWRITE			0x6
#define NVMIC_OPC_RESET				0x7

#define NVMIC_RESP_S_SUCCEEDED		0

class NVMeMICmdReq : public MCTPNVMeMIMsgRegMap
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("OPC",4 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("ReservedB567",5 , 0 , 24 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NMD0",8 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NMD1",12 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("RequestData",16 , 0 , (uint16_t)0 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CRC",16 , 0 ,32 , REG_OFF_F2B);
	};
	virtual void InitRegVal(void)
	{
		this->SetROR(false);
		this->SetNVMeMIMsgType(NMCP_NMIMT_NVMeMICMD);
	};
public:
	NVMeMICmdReq(void)
	{
		InitRegTbl();
		InitRegVal();
	}

	virtual uint16_t	GetLength(void)
	{
		uint16_t val = (GetRegTbl()->GetByteOffset("RequestData")) + \
				int((GetRegTbl()->GetLength("RequestData")) / BITS_OF_BYTE) + \
				int((GetRegTbl()->GetLength("CRC")) / BITS_OF_BYTE);

		LOGDEBUG("NVMeMICmdReq::GetLength() = %d\n",val);

		return val;
	};

	bool SetOPC(uint8_t val){return this->SetRegVal("OPC",(uint64_t)val);};
	uint8_t GetOPC(void){return (uint8_t)(this->GetRegVal("OPC"));};

	bool SetNMD0(uint32_t val){return this->SetRegVal("NMD0",(uint64_t)val);};
	uint32_t GetNMD0(void){return (uint32_t)(this->GetRegVal("NMD0"));};

	bool SetNMD1(uint32_t val){return this->SetRegVal("NMD1",(uint64_t)val);};
	uint32_t GetNMD1(void){return (uint32_t)(this->GetRegVal("NMD1"));};

	bool SetRequestData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		GetRegTbl()->AddRegDesc("RequestData",16 , 0 , length * BITS_OF_BYTE , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CRC", (length+16) , 0 ,32 , REG_OFF_F2B);

		return this->SetRegByBuf("RequestData",buf,offset,length);
	};
	bool GetRequestData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		return this->GetRegByBuf("RequestData",buf,offset,length);
	};
};

class NVMeMICmdResp : public MCTPNVMeMIMsgRegMap
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("Status",4 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NVMeMR",5 , 0 , 24 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("ResponseData",8 , 0 , REG_LEN_UNDEF , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NVMeMRB1",5 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NVMeMRB2",6 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NVMeMRB3",7 , 0 , 8 , REG_OFF_F2B);
	};
	virtual void InitRegVal(void)
	{
		this->SetROR(true);
		this->SetNVMeMIMsgType(NMCP_NMIMT_NVMeMICMD);
	};
public:
	NVMeMICmdResp(void)
	{
		InitRegTbl();
		InitRegVal();
	};
	NVMeMICmdResp(Buffers* regMapBuf)
		:MCTPNVMeMIMsgRegMap(regMapBuf)
	{
		InitRegTbl();
		InitRegVal();
	};

	void UpdateResponseDataLen(void)
	{
		uint16_t	msgDatumLen	= (GetRegTbl()->GetLength("MessageData")) / 8;
		uint16_t	dataBytes	=  msgDatumLen - MCTP_HEADER_SIZE;

		GetRegTbl()->AddRegDesc("ResponseData",8 , 0 , dataBytes * BITS_OF_BYTE , REG_OFF_F2B);
	};

	virtual void Debug(void)
	{
		printf("\tStatus=%d,MRB1=%d,MRB2=%d,MRB3=%d\n",
				GetStatus(),
				GetNVMeMRB1(),
				GetNVMeMRB2(),
				GetNVMeMRB3());

		printf("\tResponseData Data:\n");
		Buffers buf(MCTP_MESSAGE_MAX_SIZE,8);
		buf.FillZero(0,MCTP_MESSAGE_MAX_SIZE);

		GetResponseData(&buf,0,128);
		buf.Dump(0,128);
	}

	bool Succeeded(void) {return NVMIC_RESP_S_SUCCEEDED == GetStatus();};

	bool SetStatus(uint8_t val){return this->SetRegVal("Status",(uint64_t)val);};
	uint8_t GetStatus(void){return (uint8_t)(this->GetRegVal("Status"));};

	bool SetNVMeMR(uint32_t val){return this->SetRegVal("NVMeMR",(uint64_t)val);};
	uint32_t GetNVMeMR(void){return (uint32_t)(this->GetRegVal("NVMeMR"));};

	bool SetNVMeMRB1(uint8_t val){return this->SetRegVal("NVMeMRB1",(uint64_t)val);};
	uint8_t GetNVMeMRB1(void){return (uint8_t)(this->GetRegVal("NVMeMRB1"));};

	bool SetNVMeMRB2(uint8_t val){return this->SetRegVal("NVMeMRB2",(uint64_t)val);};
	uint8_t GetNVMeMRB2(void){return (uint8_t)(this->GetRegVal("NVMeMRB2"));};

	bool SetNVMeMRB3(uint8_t val){return this->SetRegVal("NVMeMRB3",(uint64_t)val);};
	uint8_t GetNVMeMRB3(void){return (uint8_t)(this->GetRegVal("NVMeMRB3"));};

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
 * NVMe-MI Commands
 */

class NVMICConfigCmd : public NVMeMICmdReq
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("CFGID",8 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NMD0B2",9 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NMD0B3",10 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NMD0B4",11 , 0 , 8 , REG_OFF_F2B);
	};
public:
	NVMICConfigCmd(){InitRegTbl();};

	bool SetCFGID(uint8_t val){return this->SetRegVal("CFGID",(uint64_t)val);};
	uint8_t GetCFGID(void){return (uint8_t)(this->GetRegVal("CFGID"));};

	bool SetNMD0B2(uint8_t val){return this->SetRegVal("NMD0B2",(uint64_t)val);};
	uint8_t GetNMD0B2(void){return (uint8_t)(this->GetRegVal("NMD0B2"));};

	bool SetNMD0B3(uint8_t val){return this->SetRegVal("NMD0B3",(uint64_t)val);};
	uint8_t GetNMD0B3(void){return (uint8_t)(this->GetRegVal("NMD0B3"));};

	bool SetNMD0B4(uint8_t val){return this->SetRegVal("NMD0B4",(uint64_t)val);};
	uint8_t GetNMD0B4(void){return (uint8_t)(this->GetRegVal("NMD0B4"));};
};

class NVMICConfigGet : public NVMICConfigCmd
{
protected:
	virtual void InitRegVal(void)
	{
		SetOPC(NVMIC_OPC_CFGGET);
	}
public:
	NVMICConfigGet(void){InitRegVal();};
};

class NVMICConfigSet : public NVMICConfigCmd
{
protected:
	virtual void InitRegVal(void)
	{
		SetOPC(NVMIC_OPC_CFGSET);
	}
public:
	NVMICConfigSet(void){InitRegVal();};
};

class NVMICCtrlHSP : public NVMeMICmdReq
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("CTLID",8 , 0 , 16 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("MAXRENT",10 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("INCF",11 , 0 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("INCPF",11 , 1 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("INCVF",11 , 2 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("RALL",11 , 7 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CSTS",12 , 0 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CTEMP",12 , 1 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("PDLU",12 , 2 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SPARE",12 , 3 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CWARN",12 , 4 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CCF",15 , 7 , 1 , REG_OFF_F2B);
	};
	virtual void InitRegVal(void)
	{
		SetOPC(NVMIC_OPC_CTLRHSP);
	};
public:
	NVMICCtrlHSP(void){InitRegTbl();InitRegVal();};

	bool SetCTLID(uint16_t val){return this->SetRegVal("CTLID",(uint64_t)val);};
	uint16_t GetCTLID(void){return (uint16_t)(this->GetRegVal("CTLID"));};

	bool SetMAXRENT(uint8_t val){return this->SetRegVal("MAXRENT",(uint64_t)val);};
	uint8_t GetMAXRENT(void){return (uint8_t)(this->GetRegVal("MAXRENT"));};

	bool SetINCF(bool val){return this->SetRegVal("INCF",(uint64_t)val);};
	bool GetINCF(void){return (bool)(this->GetRegVal("INCF"));};

	bool SetINCPF(bool val){return this->SetRegVal("INCPF",(uint64_t)val);};
	bool GetINCPF(void){return (bool)(this->GetRegVal("INCPF"));};

	bool SetINCVF(bool val){return this->SetRegVal("INCVF",(uint64_t)val);};
	bool GetINCVF(void){return (bool)(this->GetRegVal("INCVF"));};

	bool SetRALL(bool val){return this->SetRegVal("RALL",(uint64_t)val);};
	bool GetRALL(void){return (bool)(this->GetRegVal("RALL"));};

	bool SetCSTS(bool val){return this->SetRegVal("CSTS",(uint64_t)val);};
	bool GetCSTS(void){return (bool)(this->GetRegVal("CSTS"));};

	bool SetCTEMP(bool val){return this->SetRegVal("CTEMP",(uint64_t)val);};
	bool GetCTEMP(void){return (bool)(this->GetRegVal("CTEMP"));};

	bool SetPDLU(bool val){return this->SetRegVal("PDLU",(uint64_t)val);};
	bool GetPDLU(void){return (bool)(this->GetRegVal("PDLU"));};

	bool SetSPARE(bool val){return this->SetRegVal("SPARE",(uint64_t)val);};
	bool GetSPARE(void){return (bool)(this->GetRegVal("SPARE"));};

	bool SetCWARN(bool val){return this->SetRegVal("CWARN",(uint64_t)val);};
	bool GetCWARN(void){return (bool)(this->GetRegVal("CWARN"));};

	bool SetCCF(bool val){return this->SetRegVal("CCF",(uint64_t)val);};
	bool GetCCF(void){return (bool)(this->GetRegVal("CCF"));};
};

class NVMICNVMSHSP : public NVMeMICmdReq
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("CS",15 , 7 , 1 , REG_OFF_F2B);
	};
	virtual void InitRegVal(void)
	{
		SetOPC(NVMIC_OPC_NVMSHSP);
	};
public:
	NVMICNVMSHSP(void)
	{
		InitRegTbl();
		InitRegVal();
	}

	bool SetCS(bool val){return this->SetRegVal("CS",(uint64_t)val);};
	bool GetCS(void){return (bool)(this->GetRegVal("CS"));};
};

class NVMICReadData : public NVMeMICmdReq
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("CTRLID",8 , 0 , 16 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("PORTID",10 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("DTYP",11 , 0 , 8 , REG_OFF_F2B);
	};
	virtual void InitRegVal(void)
	{
		SetOPC(NVMIC_OPC_RDNVMEMI);
	};
public:
	NVMICReadData(void)
	{
		InitRegTbl();
		InitRegVal();
	}

	bool SetCTRLID(uint16_t val){return this->SetRegVal("CTRLID",(uint64_t)val);};
	uint16_t GetCTRLID(void){return (uint16_t)(this->GetRegVal("CTRLID"));};

	bool SetPORTID(uint8_t val){return this->SetRegVal("PORTID",(uint64_t)val);};
	uint8_t GetPORTID(void){return (uint8_t)(this->GetRegVal("PORTID"));};

	bool SetDTYP(uint8_t val){return this->SetRegVal("DTYP",(uint64_t)val);};
	uint8_t GetDTYP(void){return (uint8_t)(this->GetRegVal("DTYP"));};
};

class NVMICReset : public NVMeMICmdReq
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("TYPE",11 , 0 , 8 , REG_OFF_F2B);
	};
	virtual void InitRegVal(void)
	{
		SetOPC(NVMIC_OPC_RESET);
	};
public:
	NVMICReset(void)
	{
		InitRegTbl();
		InitRegVal();
	}

	bool SetTYPE(uint8_t val){return this->SetRegVal("TYPE",(uint64_t)val);};
	uint8_t GetTYPE(void){return (uint8_t)(this->GetRegVal("TYPE"));};
};

class NVMICVPDCmd : public NVMeMICmdReq
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("DOFST",8 , 0 , 16 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("DLEN",12 , 0 , 16 , REG_OFF_F2B);
	};
public:
	NVMICVPDCmd(void){InitRegTbl();}

	bool SetDOFST(uint16_t val){return this->SetRegVal("DOFST",(uint64_t)val);};
	uint16_t GetDOFST(void){return (uint16_t)(this->GetRegVal("DOFST"));};

	bool SetDLEN(uint16_t val){return this->SetRegVal("DLEN",(uint64_t)val);};
	uint16_t GetDLEN(void){return (uint16_t)(this->GetRegVal("DLEN"));};
};

class NVMICVPDRead : public NVMICVPDCmd
{
protected:
	virtual void InitRegVal(void)
	{
		SetOPC(NVMIC_OPC_VPDREAD);
	};
public:
	NVMICVPDRead(void){InitRegVal();};
};

class NVMICVPDWrite : public NVMICVPDCmd
{
protected:
	virtual void InitRegVal(void)
	{
		SetOPC(NVMIC_OPC_VPDWRITE);
	};
public:
	NVMICVPDWrite(void){InitRegVal();};
};

#endif /* _PROTOCAL_NVME_MI_NVMEMICOMMAND_H_ */
