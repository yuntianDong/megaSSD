/*
 * NVMeMIAdmin.h
 *
 */

#ifndef _PROTOCAL_NVME_MI_NVMEMIADMIN_H_
#define _PROTOCAL_NVME_MI_NVMEMIADMIN_H_

#include "protocol/mctp/MCTPRegMap.h"
#include "driver/nvme/NVMeCmdReq.h"
#include "driver/nvme/NVMeCmdResp.h"

#define	NVMADM_RESP_S_SUCCEEDED			0
#define NVMADM_RESP_BUF_MAXLEN			4096
#define NVMADM_RESP_HEADER_SIZE			20

class NVMAdminReq : public MCTPNVMeMIMsgRegMap
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("OPC",4 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CFLGS",5 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CTLID",6 , 0 , 16 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW1",8 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW2",12 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW3",16 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW4",20 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW5",24 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("DOFST",28 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("DLEN",32 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW8",36 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW9",40 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW10",44 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW11",48 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW12",52 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW13",56 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW14",60 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SQEDW15",64 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("RequestData",68 , 0 , (uint16_t)0 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CRC", 68 , 0 ,32 , REG_OFF_F2B);
	};
	virtual void InitRegVal(void)
	{
		this->SetROR(false);
		this->SetNVMeMIMsgType(NMCP_NMIMT_NVMeADMIN);
	};
public:
	NVMAdminReq(void)
	{
		InitRegTbl();
		InitRegVal();
	}

	bool Translate(NVMePassthruCmd *cmd,Buffers* buf,uint16_t offset,uint16_t dataLen)
	{
		this->SetOPC(cmd->GetOpCode());

		if( 0 < dataLen )
		{
			this->SetCFLGS(0x3);
			this->SetDOFST(offset);
			this->SetDLEN(dataLen);
		}

		this->SetSQEDW1(cmd->GetNSID());
		this->SetSQEDW2(0);
		this->SetSQEDW3(0);

		if( 0 < cmd->GetMetaBufSize() )
		{
			uint64_t metaAddr	= cmd->GetMetaBuf();
			this->SetSQEDW4((uint32_t)(metaAddr & 0xFFFFFFFF));
			this->SetSQEDW5((uint32_t)( (metaAddr << 8) & 0xFFFFFFFF));
		}

		this->SetSQEDW10(cmd->GetCDW10());
		this->SetSQEDW11(cmd->GetCDW11());
		this->SetSQEDW12(cmd->GetCDW12());
		this->SetSQEDW13(cmd->GetCDW13());
		this->SetSQEDW14(cmd->GetCDW14());
		this->SetSQEDW15(cmd->GetCDW15());

		if( 0 < dataLen && NULL != buf )
		{
			return this->SetRequestData(buf,offset,dataLen);
		}

		return true;
	};

	virtual void Debug(void)
	{
		printf("\t################################\n");
		printf("\tDW0	: OPC=%d,CFLGS=%d,CTLID=%d\n",GetOPC(),GetCFLGS(),GetCTLID());
		printf("\tDW1	: 0x%x\n",GetSQEDW1());
		printf("\tDW2	: 0x%x\n",GetSQEDW2());
		printf("\tDW3	: 0x%x\n",GetSQEDW3());
		printf("\tDW4	: 0x%x\n",GetSQEDW4());
		printf("\tDW5	: 0x%x\n",GetSQEDW5());
		printf("\tDOFST	: 0x%x\n",GetDOFST());
		printf("\tDLEN	: 0x%x\n",GetDLEN());
		printf("\tDW8	: 0x%x\n",GetSQEDW8());
		printf("\tDW9	: 0x%x\n",GetSQEDW9());
		printf("\tDW10	: 0x%x\n",GetSQEDW10());
		printf("\tDW11	: 0x%x\n",GetSQEDW11());
		printf("\tDW12	: 0x%x\n",GetSQEDW12());
		printf("\tDW13	: 0x%x\n",GetSQEDW13());
		printf("\tDW14	: 0x%x\n",GetSQEDW14());
		printf("\tDW15	: 0x%x\n",GetSQEDW15());
		printf("\tRequest Data:\n");
		Buffers buf(NVMADM_RESP_BUF_MAXLEN,DEF_BUF_ALIGNMENT);
		GetRequestData(&buf,0,GetDLEN());
		buf.Dump(0,128);
		printf("\tCRC	: 0x%x\n",GetCRC());
	}

	bool SetOPC(uint8_t val){return this->SetRegVal("OPC",(uint64_t)val);};
	uint8_t GetOPC(void){return (uint8_t)(this->GetRegVal("OPC"));};

	bool SetCFLGS(uint8_t val){return this->SetRegVal("CFLGS",(uint64_t)val);};
	uint8_t GetCFLGS(void){return (uint8_t)(this->GetRegVal("CFLGS"));};

	bool SetCTLID(uint16_t val){return this->SetRegVal("CTLID",(uint64_t)val);};
	uint16_t GetCTLID(void){return (uint16_t)(this->GetRegVal("CTLID"));};

	bool SetSQEDW1(uint32_t val){return this->SetRegVal("SQEDW1",(uint64_t)val);};
	uint32_t GetSQEDW1(void){return (uint32_t)(this->GetRegVal("SQEDW1"));};

	bool SetSQEDW2(uint32_t val){return this->SetRegVal("SQEDW2",(uint64_t)val);};
	uint32_t GetSQEDW2(void){return (uint32_t)(this->GetRegVal("SQEDW2"));};

	bool SetSQEDW3(uint32_t val){return this->SetRegVal("SQEDW3",(uint64_t)val);};
	uint32_t GetSQEDW3(void){return (uint32_t)(this->GetRegVal("SQEDW3"));};

	bool SetSQEDW4(uint32_t val){return this->SetRegVal("SQEDW4",(uint64_t)val);};
	uint32_t GetSQEDW4(void){return (uint32_t)(this->GetRegVal("SQEDW4"));};

	bool SetSQEDW5(uint32_t val){return this->SetRegVal("SQEDW5",(uint64_t)val);};
	uint32_t GetSQEDW5(void){return (uint32_t)(this->GetRegVal("SQEDW5"));};

	bool SetDOFST(uint32_t val){return this->SetRegVal("DOFST",(uint64_t)val);};
	uint32_t GetDOFST(void){return (uint32_t)(this->GetRegVal("DOFST"));};

	bool SetDLEN(uint32_t val){return this->SetRegVal("DLEN",(uint64_t)val);};
	uint32_t GetDLEN(void){return (uint32_t)(this->GetRegVal("DLEN"));};

	bool SetSQEDW8(uint32_t val){return this->SetRegVal("SQEDW8",(uint64_t)val);};
	uint32_t GetSQEDW8(void){return (uint32_t)(this->GetRegVal("SQEDW8"));};

	bool SetSQEDW9(uint32_t val){return this->SetRegVal("SQEDW9",(uint64_t)val);};
	uint32_t GetSQEDW9(void){return (uint32_t)(this->GetRegVal("SQEDW9"));};

	bool SetSQEDW10(uint32_t val){return this->SetRegVal("SQEDW10",(uint64_t)val);};
	uint32_t GetSQEDW10(void){return (uint32_t)(this->GetRegVal("SQEDW10"));};

	bool SetSQEDW11(uint32_t val){return this->SetRegVal("SQEDW11",(uint64_t)val);};
	uint32_t GetSQEDW11(void){return (uint32_t)(this->GetRegVal("SQEDW11"));};

	bool SetSQEDW12(uint32_t val){return this->SetRegVal("SQEDW12",(uint64_t)val);};
	uint32_t GetSQEDW12(void){return (uint32_t)(this->GetRegVal("SQEDW12"));};

	bool SetSQEDW13(uint32_t val){return this->SetRegVal("SQEDW13",(uint64_t)val);};
	uint32_t GetSQEDW13(void){return (uint32_t)(this->GetRegVal("SQEDW13"));};

	bool SetSQEDW14(uint32_t val){return this->SetRegVal("SQEDW14",(uint64_t)val);};
	uint32_t GetSQEDW14(void){return (uint32_t)(this->GetRegVal("SQEDW14"));};

	bool SetSQEDW15(uint32_t val){return this->SetRegVal("SQEDW15",(uint64_t)val);};
	uint32_t GetSQEDW15(void){return (uint32_t)(this->GetRegVal("SQEDW15"));};

	bool SetRequestData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		GetRegTbl()->AddRegDesc("RequestData",68 , 0 , length * BITS_OF_BYTE , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CRC", (length+68) , 0 ,32 , REG_OFF_F2B);

		return this->SetRegByBuf("RequestData",buf,offset,length);
	};
	bool GetRequestData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		return this->GetRegByBuf("RequestData",buf,offset,length);
	};
};

class NVMAdminResp : public MCTPNVMeMIMsgRegMap
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("Status",4 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("ReservedB567",5 , 0 , 24 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CQEDW0",8 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CQEDW1",12 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CQEDW3",16 , 0 , 32 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("ResponseData",20 , 0 , (uint16_t)0 , REG_OFF_F2B);
		//GetRegTbl()->AddRegDesc("CRC", 20 , 0 ,32 , REG_OFF_F2B);
	};
	virtual void InitRegVal(void)
	{
		this->SetROR(true);
		this->SetNVMeMIMsgType(NMCP_NMIMT_NVMeADMIN);
	};
public:
	NVMAdminResp(void)
	{
		InitRegTbl();
		InitRegVal();
	}
	NVMAdminResp(Buffers* regMapBuf)
		:MCTPNVMeMIMsgRegMap(regMapBuf)
	{
		InitRegTbl();
		InitRegVal();
	};

	bool Translate(NVMeCmdResp* resp,Buffers* buf,uint16_t offset,uint16_t dataLen)
	{
		UpdateResponseDataLen();

		resp->SetRespVal(this->GetCQEDW0());
		resp->SetDW1(this->GetCQEDW1());
		resp->SetDW3(this->GetCQEDW3());
		resp->SetStatus(this->GetStatus());

		if( 0 < dataLen && NULL != buf )
		{
			if(false == this->GetResponseData(buf,offset,dataLen))
			{
				Debug();
				LOGERROR("Fail to call this->GetResponseData(buf,%d,%d)",offset,dataLen);
				return false;
			}

			resp->SetDataBuffer(buf);
		}

		return true;
	};

	virtual void Debug(void)
	{
		UpdateResponseDataLen();

		printf("\t################################\n");
		printf("\tStatus: 0x%x\n",GetStatus());
		printf("\tDW0	: 0x%x\n",GetCQEDW0());
		printf("\tDW1	: 0x%x\n",GetCQEDW1());
		printf("\tDW3	: 0x%x\n",GetCQEDW3());

		printf("\tResponse Data:\n");
		Buffers buf(NVMADM_RESP_BUF_MAXLEN,DEF_BUF_ALIGNMENT);
		buf.FillZero(0,NVMADM_RESP_BUF_MAXLEN);

		GetResponseData(&buf,0,128);
		buf.Dump(0,128);
		printf("\tCRC	: 0x%x\n",GetCRC());
	}

	void UpdateResponseDataLen(void)
	{
		uint16_t	msgDatumLen	= GetRegTbl()->GetLength("MessageData") / 8;
		uint16_t	dataBytes	= NVMADM_RESP_HEADER_SIZE + msgDatumLen;

		GetRegTbl()->AddRegDesc("ResponseData",20 , 0 , dataBytes * BITS_OF_BYTE , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CRC", (20 + dataBytes)  , 0 ,32 , REG_OFF_F2B);
	};

	bool Succeeded(void) {return NVMADM_RESP_S_SUCCEEDED == GetStatus();};

	bool SetStatus(uint8_t val){return this->SetRegVal("Status",(uint64_t)val);};
	uint8_t GetStatus(void){return (uint8_t)(this->GetRegVal("Status"));};

	bool SetCQEDW0(uint32_t val){return this->SetRegVal("CQEDW0",(uint64_t)val);};
	uint32_t GetCQEDW0(void){return (uint32_t)(this->GetRegVal("CQEDW0"));};

	bool SetCQEDW1(uint32_t val){return this->SetRegVal("CQEDW1",(uint64_t)val);};
	uint32_t GetCQEDW1(void){return (uint32_t)(this->GetRegVal("CQEDW1"));};

	bool SetCQEDW3(uint32_t val){return this->SetRegVal("CQEDW3",(uint64_t)val);};
	uint32_t GetCQEDW3(void){return (uint32_t)(this->GetRegVal("CQEDW3"));};

	bool SetResponseData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		GetRegTbl()->AddRegDesc("ResponseData",20 , 0 , length * BITS_OF_BYTE , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CRC", (length+20) , 0 ,32 , REG_OFF_F2B);

		return this->SetRegByBuf("ResponseData",buf,offset,length);
	};
	bool GetResponseData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		return this->GetRegByBuf("ResponseData",buf,offset,length);
	};
};

#endif /* _PROTOCAL_NVME_MI_NVMEMIADMIN_H_ */
