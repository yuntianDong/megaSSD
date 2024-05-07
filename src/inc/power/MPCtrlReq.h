#ifndef _MP_CTRL_REQ_H
#define _MP_CTRL_REQ_H

#include <assert.h>
#include "utility/CRC.h"
#include "MCUPwrDef.h"
#include "Logging.h"

typedef enum _enRWFlags
{
	FLAG_NONE		= 0x0,
	FLAG_READ		= 0x55,
	FLAG_WRITE		= 0x33,
	FLAG_GETCUR		= 0x55,
	FLAG_GETVOL		= 0x33
}enRWFlags;

#define MAX_REQ_DESC_SIZE			64
#define MAX_REQ_DATA_SIZE			52

#define MPC_REQ_DLEN_INVALID		0xFF

#define	MPC_REQ_OFF_HEAD			0
#define	MPC_REQ_OFF_PLEN			1
#define	MPC_REQ_OFF_OPCODE			2
#define	MPC_REQ_OFF_PORT			3
#define	MPC_REQ_OFF_SLAVEADDR		4
#define	MPC_REQ_OFF_FLAG			5
#define	MPC_REQ_OFF_REGADDR			6
#define	MPC_REQ_OFF_DLEN			8
#define	MPC_REQ_OFF_DATA			9

#define MPC_REQ_DLEN_POWERID		32
#define MPC_REQ_DLEN_POWERON		1
#define MPC_REQ_DLEN_POWEROFF		1
#define MPC_REQ_DLEN_CURRENT		0
#define MPC_REQ_DLEN_VOLTAGE		0
#define MPC_REQ_DLEN_TEMP			0
#define MPC_REQ_DLEN_DETECT			0
#define MPC_REQ_DLEN_FWREV			0
#define MPC_REQ_DLEN_NVMEMISTART	1
#define MPC_REQ_DLEN_NVMEMIREADY	0
#define MPC_REQ_DLEN_UPDATEFW		4
#define MPC_REQ_DLEN_RESET			0


#define MPC_HEADER_LEN				MPC_REQ_OFF_DATA
#define MPC_TAILER_LEN				3

#define	MPC_DATA_POWERON			0x55
#define	MPC_DATA_POWEROFF			0x33

#define MPC_SLAVEADDR_TEMP			0x48
#define MPC_SLAVEADDR_EEPROM		0x53

class MPCtrlReq
{
private:
	uint8_t	mReqDatum[MAX_REQ_DESC_SIZE];
	enMPCOpCode mOpCode;
	enRWFlags	mFlag;
	uint8_t		mDatumSize;		// len(data)

	void SetByte(uint8_t offset,uint8_t val) {mReqDatum[offset] = val;};
	uint8_t GetByte(uint8_t offset) {return mReqDatum[offset];};
	void SetWord(uint8_t offset,uint16_t val)
	{
		*(mReqDatum+offset) 	= (uint8_t)( (val&0xFF00) >> 8);
		*(mReqDatum+offset+1)	= (uint8_t)( (val&0x00FF) );
	};
	uint16_t GetWord(uint8_t offset)
	{
		return ((uint16_t)*(mReqDatum+offset) << 8) |
				*(mReqDatum+offset+1);
	};

public:
	MPCtrlReq(enMPCOpCode opCode,enRWFlags flag,uint8_t datumSize)
		:mOpCode(opCode),mFlag(flag)
	{
		memset(mReqDatum,0,MAX_REQ_DESC_SIZE*sizeof(uint8_t));

		SetOPCode(opCode);
		SetFlag(flag);
		SetDataLen(datumSize);

		if( (FLAG_READ == flag) || (datumSize == 0))
		{
			mDatumSize = 1;
			SetDatum(0);
		}
		else
		{
			mDatumSize = datumSize;
		}
	};

	bool IsWriteReq(void)
	{
		if( MPC_OP_VOLTAGE == GetOPCode() )
		{
			return false;
		}

		return GetFlag() == FLAG_WRITE;
	};
	bool IsReadReq(void)
	{
		if( MPC_OP_VOLTAGE == GetOPCode() )
		{
			return true;
		}

		return GetFlag() == FLAG_READ;
	};

	uint8_t* GetMPCReqData(void)
	{
		SetHead(MPC_REQ_VAL_HEAD);
		SetTail(MPC_REQ_VAL_TAIL);
		SetPLen(MPC_HEADER_LEN+MPC_TAILER_LEN+mDatumSize);
		SetCRC();

		return mReqDatum;
	};
	uint8_t GetMPCReqLen(void)
	{
		return MPC_HEADER_LEN+MPC_TAILER_LEN+mDatumSize;
	};

	void SetHead(uint8_t val) {SetByte(MPC_REQ_OFF_HEAD,val);};
	uint8_t GetHead(void) {return GetByte(MPC_REQ_OFF_HEAD);};

	void SetPLen(uint8_t val) {SetByte(MPC_REQ_OFF_PLEN,val);};
	uint8_t GetPLen(void) {return GetByte(MPC_REQ_OFF_PLEN);};

	void SetOPCode(uint8_t val) {SetByte(MPC_REQ_OFF_OPCODE,val);};
	uint8_t GetOPCode(void) {return GetByte(MPC_REQ_OFF_OPCODE);};

	void SetPort(uint8_t val) {SetByte(MPC_REQ_OFF_PORT,val);};
	uint8_t GetPort(void) {return GetByte(MPC_REQ_OFF_PORT);};

	void SetSlaveAddr(uint8_t val) {SetByte(MPC_REQ_OFF_SLAVEADDR,val);};
	uint8_t GetSlaveAddr(void) {return GetByte(MPC_REQ_OFF_SLAVEADDR);};

	void SetFlag(uint8_t val) {SetByte(MPC_REQ_OFF_FLAG,val);};
	uint8_t GetFlag(void) {return GetByte(MPC_REQ_OFF_FLAG);};

	void SetRegAddr(uint16_t val) {SetWord(MPC_REQ_OFF_REGADDR,val);};
	uint16_t GetRegAddr(void) {return GetWord(MPC_REQ_OFF_REGADDR);};

	void SetDataLen(uint8_t val)
	{
		SetByte(MPC_REQ_OFF_DLEN,val);
	};
	uint8_t GetDataLen(void)
	{
		return GetByte(MPC_REQ_OFF_DLEN);
	};

	void SetDatum(uint8_t val) {SetByte(MPC_REQ_OFF_DATA,val);};
	uint8_t GetDatum(void) {return GetByte(MPC_REQ_OFF_DATA);};

	void SetDatum(uint8_t* vals,uint8_t len)
	{
		mDatumSize = len;
		SetDataLen(len);

		if(NULL != vals)
		{
			memcpy(mReqDatum+MPC_REQ_OFF_DATA,vals,mDatumSize);
		}
	};
	void GetDatum(uint8_t* vals,uint8_t len)
	{
		if( FLAG_READ == GetFlag() )
		{
			mDatumSize = 1;
		}
		else{
			mDatumSize	= GetDataLen();
		}

		assert(len >= mDatumSize);

		memcpy(vals,mReqDatum+MPC_REQ_OFF_DATA,mDatumSize);
	};

	void SetTail(uint8_t val)
	{
		uint8_t offset = MPC_REQ_OFF_DATA + mDatumSize;
		SetByte(offset,val);
	}
	uint8_t GetTail(void)
	{
		uint8_t offset = MPC_REQ_OFF_DATA + mDatumSize;
		return GetByte(offset);
	}

	void SetCRC(void)
	{
		uint8_t dataLen = MPC_HEADER_LEN + mDatumSize + 1;
		uint16_t crc = CalCRC16(mReqDatum,dataLen);
		SetWord(dataLen,crc);
	};
	uint16_t GetCRC(void)
	{
		uint8_t dataLen = MPC_HEADER_LEN + mDatumSize + 1;
		return GetWord(dataLen);
	};

	void Dump(void)
	{
		printf("0:Head          = %02X\n",GetHead());
		printf("1:PackageLen    = %02X\n",GetPLen());
		printf("2:CmdCode       = %02X\n",GetOPCode());
		printf("3:Port          = %02X\n",GetPort());
		printf("4:SlaveAddr     = %02X\n",GetSlaveAddr());
		printf("5:R/WFlag       = %02X\n",GetFlag());
		printf("6:RegAddr       = %04X\n",GetRegAddr());
		printf("8:DataLen       = %02X\n",GetDataLen());
		printf("9:Datum         = ");
		uint8_t datum[MAX_REQ_DATA_SIZE];
		GetDatum(datum,MAX_REQ_DATA_SIZE);
		for(uint8_t idx=0;idx<mDatumSize;idx++)
		{
			printf("%02X",datum[idx]);
		}
		printf("\n");
		printf("0:Tail          = %02X\n",GetTail());
		printf("0:CRC           = %04X\n",GetCRC());
		printf("DEBUG: mFlag	= %d\n",mFlag);
		printf("DEBUG: mDatumSize	= %d\n",mDatumSize);
	};

	void DumpHex(void)
	{
		for(uint8_t idx=0;idx<MAX_REQ_DESC_SIZE;idx++)
		{
			if(idx % 8 == 0)
			{
				printf("\n");
			}

			printf("%02X ",mReqDatum[idx]);
		}

		printf("\n");
	}
};

class ReqGetPowerID : public MPCtrlReq
{
public:
	ReqGetPowerID(void)
		:MPCtrlReq(MPC_OP_BOARDID,FLAG_READ,MPC_REQ_DLEN_POWERID)
	{};
};

class ReqSetPowerID : public MPCtrlReq
{
public:
	ReqSetPowerID(void)
		:MPCtrlReq(MPC_OP_BOARDID,FLAG_WRITE,MPC_REQ_DLEN_POWERID)
	{};
};

class ReqPowerOn : public MPCtrlReq
{
public:
	ReqPowerOn(void)
		:MPCtrlReq(MPC_OP_POWCTRL,FLAG_WRITE,MPC_REQ_DLEN_POWERON)
	{
		SetDatum(MPC_DATA_POWERON);
	};
};

class ReqPowerOff : public MPCtrlReq
{
public:
	ReqPowerOff(void)
		:MPCtrlReq(MPC_OP_POWCTRL,FLAG_WRITE,MPC_REQ_DLEN_POWEROFF)
	{
		SetDatum(MPC_DATA_POWEROFF);
	};
};

class ReqGetCurrent : public MPCtrlReq
{
public:
	ReqGetCurrent(void)
		:MPCtrlReq(MPC_OP_VOLTAGE,FLAG_GETCUR,MPC_REQ_DLEN_CURRENT)
	{};
};

class ReqGetVoltage : public MPCtrlReq
{
public:
	ReqGetVoltage(void)
		:MPCtrlReq(MPC_OP_VOLTAGE,FLAG_GETVOL,MPC_REQ_DLEN_VOLTAGE)
	{};
};

class ReqGetTemp : public MPCtrlReq
{
public:
	ReqGetTemp(void)
		:MPCtrlReq(MPC_OP_TEMP,FLAG_READ,MPC_REQ_DLEN_TEMP)
	{
		SetSlaveAddr(MPC_SLAVEADDR_TEMP);
	};
};

class ReqDetectDevice : public MPCtrlReq
{
public:
	ReqDetectDevice(void)
		:MPCtrlReq(MPC_OP_DETECT,FLAG_NONE,MPC_REQ_DLEN_DETECT)
	{
	};
};

class ReqGetFWRev : public MPCtrlReq
{
public:
	ReqGetFWRev(void)
		:MPCtrlReq(MPC_OP_FWREV,FLAG_READ,MPC_REQ_DLEN_FWREV)
	{
	};
};

class ReqWrEEPPROM : public MPCtrlReq
{
public:
	ReqWrEEPPROM(uint8_t addr)
		:MPCtrlReq(MPC_OP_EEPROM,FLAG_WRITE,MPC_REQ_DLEN_INVALID)
	{
		SetSlaveAddr(addr);
	};
};

class ReqRdEEPPROM : public MPCtrlReq
{
public:
	ReqRdEEPPROM(uint8_t addr)
		:MPCtrlReq(MPC_OP_EEPROM,FLAG_READ,MPC_REQ_DLEN_INVALID)
	{
		SetSlaveAddr(addr);
	};
};

class ReqNVMeMIStart : public MPCtrlReq
{
public:
	ReqNVMeMIStart(void)
		:MPCtrlReq(MPC_OP_NVMEMISTART,FLAG_WRITE,MPC_REQ_DLEN_NVMEMISTART)
	{};
};

class ReqNVMeMIDXfer : public MPCtrlReq
{
public:
	ReqNVMeMIDXfer(void)
		:MPCtrlReq(MPC_OP_NVMEMIDXFER,FLAG_WRITE,MPC_REQ_DLEN_INVALID)
	{};
};

class ReqNVMeMIReady : public MPCtrlReq
{
public:
	ReqNVMeMIReady(void)
		:MPCtrlReq(MPC_OP_NVMEMIREADY,FLAG_READ,MPC_REQ_DLEN_NVMEMIREADY)
	{};
};

class ReqNVMeMIDRcvr : public MPCtrlReq
{
public:
	ReqNVMeMIDRcvr(void)
		:MPCtrlReq(MPC_OP_NVMEMIDRCVR,FLAG_READ,MPC_REQ_DLEN_INVALID)
	{};
};

class ReqUpdateFW : public MPCtrlReq
{
public:
	ReqUpdateFW(void)
		:MPCtrlReq(MPC_OP_UPDATEFW,FLAG_WRITE,MPC_REQ_DLEN_UPDATEFW)
	{};
};

class ReqDownloadFW : public MPCtrlReq
{
public:
	ReqDownloadFW(void)
		:MPCtrlReq(MPC_OP_DOWNLOADFW,FLAG_WRITE,MPC_REQ_DLEN_INVALID)
	{};
};

class ReqReset : public MPCtrlReq
{
public:
	ReqReset(void)
		:MPCtrlReq(MPC_OP_RESET,FLAG_NONE,MPC_REQ_DLEN_RESET)
	{};
};

#endif
