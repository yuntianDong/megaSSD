#ifndef _MCU_PWR_RESP_H
#define _MCU_PWR_RESP_H

#include <assert.h>
#include "utility/CRC.h"
#include "MCUPwrDef.h"
#include "Logging.h"

#define MCP_RESP_OFF_HEAD		0
#define MCP_RESP_OFF_PLEN		1
#define MCP_RESP_OFF_OPCODE		2
#define MCP_RESP_OFF_DLEN		3
#define MCP_RESP_OFF_DATUM		4

#define MCP_RESP_NON_DATA_LEN			7
#define MCP_RESP_DLEN_GETPWRID			32
#define MCP_RESP_DLEN_SETPWRID			1
#define MCP_RESP_DLEN_POWERON			1
#define MCP_RESP_DLEN_POWEROFF			1
#define MCP_RESP_DLEN_DETECTDEVICE		1
#define MCP_RESP_DLEN_GETFWREV			8
#define MCP_RESP_DLEN_GETVOL			2
#define MCP_RESP_DLEN_GETTEMP			1
#define MCP_RESP_DLEN_WREEPROM			1

class MPCtrlResp
{
private:
	uint8_t*	mRespDatum;
	uint8_t		mDatumLen;

public:
	MPCtrlResp(uint8_t* vals,uint8_t len)
		:mRespDatum(NULL),mDatumLen(len)
	{
		mRespDatum	= (uint8_t*)malloc(len);
		memcpy(mRespDatum,vals,len);
	};

	virtual ~MPCtrlResp(void)
	{
		if(NULL != mRespDatum)
		{
			free(mRespDatum);
			mRespDatum = NULL;
		}
	}

	void SetByte(uint8_t offset,uint8_t val) {mRespDatum[offset] = val;};
	uint8_t GetByte(uint8_t offset) {return mRespDatum[offset];};
	void SetWord(uint8_t offset,uint16_t val)
	{
		*(mRespDatum+offset) 	= (uint8_t)( (val&0xFF00) >> 8);
		*(mRespDatum+offset+1)	= (uint8_t)( (val&0x00FF) );
	};
	uint16_t GetWord(uint8_t offset)
	{
		return ((uint16_t)*(mRespDatum+offset) << 8) |
				*(mRespDatum+offset+1);
	};

	uint8_t GetHead(void) {return GetByte(MCP_RESP_OFF_HEAD);};
	uint8_t GetPLen(void) {return GetByte(MCP_RESP_OFF_PLEN);};
	uint8_t GetOPCode(void) {return GetByte(MCP_RESP_OFF_OPCODE);};
	uint8_t GetDLen(void) {return GetByte(MCP_RESP_OFF_DLEN);};
	uint8_t GetDataByOffset(uint8_t offset)
	{
		assert(offset < GetDLen());
		return GetByte(MCP_RESP_OFF_DATUM + offset);
	};
	void GetDatum(uint8_t* vals,uint8_t len)
	{
		assert(len + MCP_RESP_OFF_DATUM >= GetDLen());
		memcpy(vals,mRespDatum+MCP_RESP_OFF_DATUM,GetDLen());
	};
	uint8_t GetTail(void)
	{
		uint8_t offset = MCP_RESP_OFF_DATUM + GetDLen();
		return GetByte(offset);
	};
	uint16_t GetCRC(void)
	{
		uint8_t offset = MCP_RESP_OFF_DATUM + GetDLen() + 1;
		return GetWord(offset);
	};
	bool CheckValid(void)
	{
		uint8_t  offset = MCP_RESP_OFF_DATUM + GetDLen() + 1;
		uint16_t expCRC = CalCRC16(mRespDatum,offset);

		if(expCRC != GetCRC())
		{
			LOGERROR("CRC Check Failed. Act: %04X, Exp: %04X",expCRC,GetCRC());
		}

		return (expCRC == GetCRC()) &&
				(MPC_REQ_VAL_TAIL == GetTail()) &&
				(MPC_REQ_VAL_HEAD == GetHead()) &&
				((offset+2) == GetPLen());
	};
	void Dump(void)
	{
		printf("0:Head          = %02X\n",GetHead());
		printf("1:PackageLen    = %02X\n",GetPLen());
		printf("2:CmdCode       = %02X\n",GetOPCode());
		printf("3:DataLen       = %02X\n",GetDLen());
		printf("4:Datum         = ");
		for(uint8_t idx=0;idx<GetDLen();idx++)
		{
			printf("%02X",GetDataByOffset(idx));
		}
		printf("\n");
		printf("0:Tail          = %02X\n",GetTail());
		printf("0:CRC           = %04X\n",GetCRC());
	};
	void DumpHex(void)
	{
		printf("\n");

		for(uint8_t idx=0;idx<mDatumLen;idx++)
		{
			printf("%02X ",mRespDatum[idx]);
		}

		printf("\n");
	};
};

#endif
