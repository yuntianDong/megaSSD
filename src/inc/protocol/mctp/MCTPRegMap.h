/*
 * MCTPRegMap.h
 *
 */

#ifndef _PROTOCAL_MCTP_MCTPREGMAP_H_
#define _PROTOCAL_MCTP_MCTPREGMAP_H_

#include "protocol/RegisterMap.h"

#ifndef BYTE_OF_DWORD
#define BYTE_OF_DWORD					4
#endif
#ifndef BITS_OF_BYTE
#define BITS_OF_BYTE					8
#endif

#define MCTP_PACKAGE_MAX_SIZE			256
#define	MCTP_PACKAGE_LITTLE_ENDIAN		false

#define MCTP_MESSAGE_MAX_SIZE			4224
#define MCTP_MESSAGE_LITTLE_ENDIAN		true	//Debug Purpose false

#define MCTP_NVME_MI_MSG_TYPE			0x4
#define MCTP_NVME_MI_IC_FLAG			true

#define NMCP_NMIMT_PRIMITIVE			0x0
#define NMCP_NMIMT_NVMeMICMD			0x1
#define NMCP_NMIMT_NVMeADMIN			0x2
#define NMCP_NMIMT_PCIECMD				0x4

#define	MCTP_HEADER_SIZE				4
#define MCTP_CRC_BYTES					4

class MCTPPackageRegMap : public RegisterMap
{
protected:
	virtual void InitRegTbl(void)
	{
		LOGDEBUG("MCTPPackageRegMap::InitRegTbl(void)");

		GetRegTbl()->AddRegDesc("HeaderVersion",0 , 0 , 4 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("Reserved",0 , 4 , 4 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("DstEPID",1 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SrcEPID",2 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("MsgTag",3 , 0 , 3 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("TO",3 , 3 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("PktSeq",3 , 4 , 2 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("EOM",3 , 6 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SOM",3 , 7 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("PacketPayload",4 , 0 , (uint16_t)0 , REG_OFF_F2B);
	}
public:
	MCTPPackageRegMap(void)
		:RegisterMap(MCTP_PACKAGE_MAX_SIZE,MCTP_PACKAGE_LITTLE_ENDIAN)
	{
		LOGDEBUG("MCTPPackageRegMap::MCTPPackageRegMap(void)");
		InitRegTbl();
	};
	virtual ~MCTPPackageRegMap(){};

	virtual void Debug(void)
	{
		printf("\tHV=%x,DstEPID=%x,SrcEPID=%x,MsgTag=%x,TO=%x,PktSeq=%x,EOM=%x,SOM=%x\n",
				GetHeaderVersion(),
				GetDstEPID(),
				GetSrcEPID(),
				GetMsgTag(),
				GetTO(),
				GetPktSeq(),
				GetEOM(),
				GetSOM());

		Buffers buf(MCTP_MESSAGE_MAX_SIZE,8);
		GetPacketPayload(&buf,0,MCTP_MESSAGE_MAX_SIZE);
		buf.Dump(0,128);
	}

	virtual uint16_t GetLength(void)
	{
		return ( ( GetRegTbl()->GetLength("PacketPayload") ) / BITS_OF_BYTE ) + MCTP_HEADER_SIZE;
	}

	void UpdatePacketPayloadLen(uint16_t dataBytes)
	{
		GetRegTbl()->AddRegDesc("PacketPayload",4 , 0 , dataBytes * BITS_OF_BYTE , REG_OFF_F2B);
	};

	bool SetHeaderVersion(uint8_t val){return this->SetRegVal("HeaderVersion",(uint64_t)val);};
	uint8_t GetHeaderVersion(void){return (uint8_t)(this->GetRegVal("HeaderVersion"));};

	bool SetReserved(uint8_t val){return this->SetRegVal("Reserved",(uint64_t)val);};
	uint8_t GetReserved(void){return (uint8_t)(this->GetRegVal("Reserved"));};

	bool SetDstEPID(uint8_t val){return this->SetRegVal("DstEPID",(uint64_t)val);};
	uint8_t GetDstEPID(void){return (uint8_t)(this->GetRegVal("DstEPID"));};

	bool SetSrcEPID(uint8_t val){return this->SetRegVal("SrcEPID",(uint64_t)val);};
	uint8_t GetSrcEPID(void){return (uint8_t)(this->GetRegVal("SrcEPID"));};

	bool SetMsgTag(uint8_t val){return this->SetRegVal("MsgTag",(uint64_t)val);};
	uint8_t GetMsgTag(void){return (uint8_t)(this->GetRegVal("MsgTag"));};

	bool SetTO(bool val){return this->SetRegVal("TO",(uint64_t)val);};
	bool GetTO(void){return (bool)(this->GetRegVal("TO"));};

	bool SetPktSeq(uint8_t val){return this->SetRegVal("PktSeq",(uint64_t)val);};
	uint8_t GetPktSeq(void){return (uint8_t)(this->GetRegVal("PktSeq"));};

	bool SetEOM(bool val){return this->SetRegVal("EOM",(uint64_t)val);};
	bool GetEOM(void){return (bool)(this->GetRegVal("EOM"));};

	bool SetSOM(bool val){return this->SetRegVal("SOM",(uint64_t)val);};
	bool GetSOM(void){return (bool)(this->GetRegVal("SOM"));};

	bool SetPacketPayload(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		GetRegTbl()->AddRegDesc("PacketPayload",4 , 0 , length * BITS_OF_BYTE , REG_OFF_F2B);

		return this->SetRegByBuf("PacketPayload",buf,offset,length);
	};
	bool GetPacketPayload(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		return this->GetRegByBuf("PacketPayload",buf,offset,length);
	};
};

class MCTPNVMeMIMsgRegMap : public RegisterMap
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("MessageType",0 , 0 , 7 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("IC",0 , 7 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CSI",1 , 0 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("ReservedB2",1 , 1 , 2 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("NVMeMIMsgType",1 , 3 , 4 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("ROR",1 , 7 , 1 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("ReservedB34",2 , 0 ,16 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("MessageData",4 , 0 , REG_LEN_UNDEF , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CRC",0 , 0 ,MCTP_CRC_BYTES*BITS_OF_BYTE , REG_OFF_APP);
	}
	virtual void InitRegVal(void)
	{
		SetMessageType(MCTP_NVME_MI_MSG_TYPE);
		SetIC(MCTP_NVME_MI_IC_FLAG);
	}
public:
	MCTPNVMeMIMsgRegMap(void)
		:RegisterMap(MCTP_MESSAGE_MAX_SIZE,MCTP_MESSAGE_LITTLE_ENDIAN)
	{
		InitRegTbl();
		InitRegVal();
	};
	MCTPNVMeMIMsgRegMap(Buffers* regMapBuf)
		:RegisterMap(regMapBuf)
	{
		InitRegTbl();
		InitRegVal();
	};

	bool SetLength(uint16_t val)
	{
		if( val <= (MCTP_HEADER_SIZE + MCTP_CRC_BYTES) ||
			 val >= (MCTP_MESSAGE_MAX_SIZE - MCTP_HEADER_SIZE - MCTP_CRC_BYTES) )
		{
			return false;
		}

		uint16_t dataBytes = (val - MCTP_HEADER_SIZE - MCTP_CRC_BYTES);
		GetRegTbl()->AddRegDesc("MessageData",4 , 0 , dataBytes*BITS_OF_BYTE , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CRC",dataBytes + 4 , 0 ,MCTP_CRC_BYTES*BITS_OF_BYTE , REG_OFF_F2B);

		return true;
	}

	virtual uint16_t	GetLength(void)
	{
		uint16_t offsetCRC	= GetRegTbl()->GetByteOffset("CRC");
		uint16_t nBitsCRC	= GetRegTbl()->GetLength("CRC");
		enOffType offTypeCRC= GetRegTbl()->GetRegOffType("CRC");

		if(REG_OFF_F2B == offTypeCRC && 0 != offsetCRC )
		{
			return offsetCRC + (nBitsCRC/BITS_OF_BYTE);
		}

		return (uint16_t)-1;
	};

	virtual void Debug(void)
	{
		printf("\tMT=%d,IC=%d,CSI=%d,MSG=%d,ROR=%d",
				GetMessageType(),
				GetIC(),
				GetCSI(),
				GetNVMeMIMsgType(),
				GetROR());

		printf("\tMessageData Data:\n");
		Buffers buf(MCTP_MESSAGE_MAX_SIZE,8);
		buf.FillZero(0,MCTP_MESSAGE_MAX_SIZE);

		GetMessageData(&buf,0,128);
		buf.Dump(0,128);
		printf("\tCRC	: 0x%x\n",GetCRC());
	}

	bool SetMessageType(uint8_t val){return this->SetRegVal("MessageType",(uint64_t)val);};
	uint8_t GetMessageType(void){return (uint8_t)(this->GetRegVal("MessageType"));};

	bool SetIC(bool val){return this->SetRegVal("IC",(uint64_t)val);};
	bool GetIC(void){return (bool)(this->GetRegVal("IC"));};

	bool SetCSI(uint8_t val){return this->SetRegVal("CSI",(uint64_t)val);};
	uint8_t GetCSI(void){return (uint8_t)(this->GetRegVal("CSI"));};

	bool SetReserved(uint8_t val){return this->SetRegVal("ReservedB2",(uint64_t)val);};
	uint8_t GetReserved(void){return (uint8_t)(this->GetRegVal("ReservedB2"));};

	bool SetNVMeMIMsgType(uint8_t val){return this->SetRegVal("NVMeMIMsgType",(uint64_t)val);};
	uint8_t GetNVMeMIMsgType(void){return (uint8_t)(this->GetRegVal("NVMeMIMsgType"));};

	bool SetROR(bool val){return this->SetRegVal("ROR",(uint64_t)val);};
	bool GetROR(void){return (bool)(this->GetRegVal("ROR"));};

	bool SetReserved2(uint16_t val){return this->SetRegVal("ReservedB34",(uint64_t)val);};
	uint16_t GetReserved2(void){return (uint16_t)(this->GetRegVal("ReservedB34"));};

	bool SetMessageData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		return this->SetRegByBuf("MessageData",buf,offset,length);
	};
	bool GetMessageData(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		return this->GetRegByBuf("MessageData",buf,offset,length);
	};

	bool SetCRC(uint32_t val){return this->SetRegVal("CRC",(uint64_t)val);};
	uint32_t GetCRC(void){return (uint32_t)(this->GetRegVal("CRC"));};
};

#endif /* _PROTOCAL_MCTP_MCTPREGMAP_H_ */
