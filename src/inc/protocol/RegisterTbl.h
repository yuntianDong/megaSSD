/*
 * RegisterTbl.h
 *
 */

#ifndef _PROTOCAL_REGISTERTBL_H_
#define _PROTOCAL_REGISTERTBL_H_

#include <stdio.h>
#include <string.h>

#include "protocol/RegisterCfg.h"
#include "Logging.h"

#define REG_DESC_LIST_MAX_CNT	256
#define REG_INVALID_VAL			(uint16_t)-1

class RegisterTbl
{
private:
	stRegDesc	mpRegDescLst[REG_DESC_LIST_MAX_CNT];
	uint16_t	mRegDescCnt;

	bool AddRegDesc(const char* reg,stRegOffset& byOffset,stRegOffset& btOffset,stRegLength& nBits,\
			enOffType offType,enLenType	lenType)
	{
		//LOGDEBUG("Enter RegisterTbl::AddRegDesc(#1,reg=%s)",reg);

		if(mRegDescCnt >= REG_DESC_LIST_MAX_CNT)
		{
			return false;
		}

		stRegDesc*	desc = NULL;
		if(false == FindRegDesc(reg,desc))
		{
			desc = &(mpRegDescLst[mRegDescCnt++]);
			strncpy(desc->name,reg,REG_NAME_STRMAX);
		}
		desc->bytOff		= byOffset;
		desc->bitOff		= btOffset;
		desc->nBits		= nBits;
		desc->offType	= offType;
		desc->lenType	= lenType;

		//LOGDEBUG("Exit RegisterTbl::AddRegDesc(#1,reg=%s)",reg);

		return true;
	};
	bool FindRegDesc(const char* name,stRegDesc*& desc)
	{
		//LOGDEBUG("RegisterTbl::FindRegDesc(%s)",name);

		for(uint16_t idx=0;idx<mRegDescCnt;idx++)
		{
			if( 0 == strcmp(mpRegDescLst[idx].name,name) )
			{
				desc = &(mpRegDescLst[idx]);
				return true;
			}
		}

		return false;
	};
public:
	RegisterTbl(void)
		:mRegDescCnt(0)
	{
		LOGDEBUG("RegisterTbl::RegisterTbl(void)");

		memset(mpRegDescLst,0,sizeof(stRegDesc) * REG_DESC_LIST_MAX_CNT);
	};

	void Debug(void)
	{
		printf("#######################################\n");

		printf("mpRegDescLst[%d]:\n",mRegDescCnt);
		for(uint16_t idx=0;idx<mRegDescCnt;idx++)
		{
			stRegDesc& desc = mpRegDescLst[idx];
			printf("%d - %s\n",idx,desc.name);
			if( REG_OFF_REG == desc.offType )
			{
				printf("\tbytOff = %s\n",desc.bytOff.offReg);
			}
			else
			{
				printf("\tbytOff = %d\n",desc.bytOff.offVal);
			}
			printf("\tbitOff = %d\n",desc.bitOff.offVal);
			if( REG_LEN_REG == desc.lenType )
			{
				printf("\tnBits = %s\n",desc.nBits.lenReg);
			}
			else
			{
				printf("\tnBits = %d\n",desc.nBits.lenVal);
			}
		}

		printf("#######################################\n");
	}

	bool AddRegDesc(const char* reg,uint16_t byOffset,uint16_t btOffset,uint16_t nBits,enOffType offType)
	{
		LOGDEBUG("RegisterTbl::AddRegDesc(#2,reg=%s)",reg);

		if(REG_OFF_F2B != offType && REG_OFF_B2F != offType && REG_OFF_APP != offType)
		{
			return false;
		}

		stRegOffset sByOff = {.offVal = byOffset};
		stRegOffset sBtOff = {.offVal = btOffset};
		stRegLength snBits = {.lenVal = nBits};

		return AddRegDesc(reg,sByOff,sBtOff,snBits,offType,REG_LEN_VAL);
	};
	bool AddRegDesc(const char* reg,uint16_t byOffset,uint16_t btOffset,const char* nBits,enOffType offType)
	{
		LOGDEBUG("RegisterTbl::AddRegDesc(#3,reg=%s)",reg);

		if(REG_OFF_F2B != offType && REG_OFF_B2F != offType && REG_OFF_APP != offType)
		{
			return false;
		}

		stRegOffset sByOff = {.offVal = byOffset};
		stRegOffset sBtOff = {.offVal = btOffset};
		stRegLength snBits;
		strncpy(snBits.lenReg,nBits,REG_NAME_STRMAX);

		return AddRegDesc(reg,sByOff,sBtOff,snBits,offType,REG_LEN_REG);
	};
	bool AddRegDesc(const char* reg,const char* byOffset,uint16_t offset,uint16_t nBits)
	{
		LOGDEBUG("RegisterTbl::AddRegDesc(#4,reg=%s)",reg);

		stRegOffset sByOff;
		strncpy(sByOff.offReg,byOffset,REG_NAME_STRMAX);
		stRegOffset sBtOff = {.offVal = offset};
		stRegLength snBits = {.lenVal = nBits};

		return AddRegDesc(reg,sByOff,sBtOff,snBits,REG_OFF_REG,REG_LEN_VAL);
	};
	bool AddRegDesc(const char* reg,const char* byOffset,uint16_t offset,const char* nBits)
	{
		LOGDEBUG("RegisterTbl::AddRegDesc(#5,reg=%s)",reg);

		stRegOffset sByOff;
		strncpy(sByOff.offReg,byOffset,REG_NAME_STRMAX);
		stRegOffset sBtOff = {.offVal = offset};
		stRegLength snBits;
		strncpy(snBits.lenReg,nBits,REG_NAME_STRMAX);

		return AddRegDesc(reg,sByOff,sBtOff,snBits,REG_OFF_REG,REG_LEN_REG);
	};

	uint16_t GetByteOffset(const char* name)
	{
		stRegOffset of;
		if(true == GetRegByteOff(name,of))
		{
			enOffType ty = GetRegOffType(name);
			if(REG_OFF_F2B == ty || REG_OFF_B2F == ty || REG_OFF_APP == ty)
			{
				return of.offVal;
			}
			else if(REG_OFF_REG == ty)
			{
				return GetByteOffset(of.offReg);
			}
		}

		return REG_INVALID_VAL;
	};
	uint16_t GetBitOffset(const char* name)
	{
		stRegOffset of;
		if(true == GetRegBitOff(name,of))
		{
			return of.offVal;
		}

		return REG_INVALID_VAL;
	};
	uint16_t GetLength(const char* name)
	{
		stRegLength nBits;
		if(true == GetRegLength(name,nBits))
		{
			enLenType ty = GetRegLenType(name);
			if(REG_LEN_VAL == ty)
			{
				return nBits.lenVal;
			}
			else if(REG_LEN_REG == ty)
			{
				return GetLength(nBits.lenReg);
			}
		}

		return REG_INVALID_VAL;
	};

	enOffType	GetRegOffType(const char* name)
	{
		stRegDesc* desc	= NULL;
		if(true == FindRegDesc(name,desc))
		{
			return desc->offType;
		}

		return REG_OFF_UNK;
	};
	enLenType	GetRegLenType(const char* name)
	{
		stRegDesc* desc	= NULL;
		if(true == FindRegDesc(name,desc))
		{
			return desc->lenType;
		}

		return REG_LEN_UNK;
	};
	bool GetRegByteOff(const char* name,stRegOffset& offset)
	{
		stRegDesc* desc	= NULL;
		if(true == FindRegDesc(name,desc))
		{
			offset = desc->bytOff;
			return true;
		}

		return false;
	};
	bool GetRegBitOff(const char* name,stRegOffset& offset)
	{
		stRegDesc* desc	= NULL;
		if(true == FindRegDesc(name,desc))
		{
			offset = desc->bitOff;
			return true;
		}

		return false;
	};
	bool GetRegLength(const char* name,stRegLength& length)
	{
		stRegDesc* desc	= NULL;
		if(true == FindRegDesc(name,desc))
		{
			length = desc->nBits;
			return true;
		}

		return false;
	};
};

#endif /* _PROTOCAL_REGISTERTBL_H_ */
