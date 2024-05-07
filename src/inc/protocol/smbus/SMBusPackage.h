/*
 * SMBusPackage.h
 *
 */

#ifndef _PROTOCAL_SMBUS_SMBUSPACKAGE_H_
#define _PROTOCAL_SMBUS_SMBUSPACKAGE_H_

#include "protocol/RegisterMap.h"

#ifndef BYTE_OF_DWORD
#define BYTE_OF_DWORD					4
#endif

#define	SMBUS_HEADER_SIZE				4
#define SMBUS_HEADER_BEFORE_BYTECOUNT	3
#define SMBUS_PEC_LEN					1

class SMBusPackage : public RegisterMap
{
protected:
	virtual void InitRegTbl(void)
	{
		GetRegTbl()->AddRegDesc("DestSlaveAddr",0 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("CommandCode",1 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("ByteCount",2 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("SrcSlaveAddr",3 , 0 , 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("DataBytes",4 , 0 , (uint16_t)0 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("PEC",0 , 0 ,32 , REG_OFF_APP);
	};

public:
	SMBusPackage(uint32_t regMapMaxSize)
		:RegisterMap(regMapMaxSize,true)
	{
		InitRegTbl();
	};
	SMBusPackage(Buffers* regMapBuf)
		:RegisterMap(regMapBuf)
	{};

	virtual void Debug(void)
	{
		printf("\tDAddr=%x,CmdCode=%x,ByteCnt=%d,SAddr=%x\n",
				GetDestSlaveAddr(),
				GetCommandCode(),
				GetByteCount(),
				GetSrcSlaveAddr());

		//GetRegTbl()->Debug();

		Buffers buf(64,8);
		GetDataBytes(&buf,0,64);
		buf.Dump(0,64);

		printf("\tPEC = %x\n",GetPEC());
	}

	uint16_t GetLength(void) {return GetByteCount() + SMBUS_HEADER_BEFORE_BYTECOUNT + SMBUS_PEC_LEN;};
	void UpdateDataBytes(uint16_t dataBytes)
	{
		GetRegTbl()->AddRegDesc("DataBytes",4 , 0 , dataBytes * 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("PEC",dataBytes+4 , 0 ,32 , REG_OFF_F2B);
	};

	bool SetDestSlaveAddr(uint8_t val){return this->SetRegVal("DestSlaveAddr",(uint64_t)val);};
	uint8_t GetDestSlaveAddr(void){return (uint8_t)(this->GetRegVal("DestSlaveAddr"));};

	bool SetCommandCode(uint8_t val){return this->SetRegVal("CommandCode",(uint64_t)val);};
	uint8_t GetCommandCode(void){return (uint8_t)(this->GetRegVal("CommandCode"));};

	bool SetByteCount(uint8_t val){return this->SetRegVal("ByteCount",(uint64_t)val);};
	uint8_t GetByteCount(void){return (uint8_t)(this->GetRegVal("ByteCount"));};

	bool SetSrcSlaveAddr(uint8_t val){return this->SetRegVal("SrcSlaveAddr",(uint64_t)val);};
	uint8_t GetSrcSlaveAddr(void){return (uint8_t)(this->GetRegVal("SrcSlaveAddr"));};

	bool SetPEC(uint8_t val){return this->SetRegVal("PEC",(uint64_t)val);};
	uint8_t GetPEC(void){return (uint8_t)(this->GetRegVal("PEC"));};

	bool SetDataBytes(Buffers* buf,uint16_t offset,uint16_t length)
	{
		if(0 != (length % BYTE_OF_DWORD)){return false;}

		GetRegTbl()->AddRegDesc("DataBytes",4 , 0 , length * 8 , REG_OFF_F2B);
		GetRegTbl()->AddRegDesc("PEC",length+4 , 0 ,32 , REG_OFF_F2B);

		return this->SetRegByBuf("DataBytes",buf,offset,length);
	};
	bool GetDataBytes(Buffers* buf,uint16_t offset,uint16_t length)
	{
		//if(0 != (length % BYTE_OF_DWORD)){return false;}

		return this->GetRegByBuf("DataBytes",buf,offset,length);
	};
};

#endif /* _PROTOCAL_SMBUS_SMBUSPACKAGE_H_ */
