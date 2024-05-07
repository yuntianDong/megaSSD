/*
 *  file : ScsiCommand.cpp
 */

#include <malloc.h>
#include <string.h>
#include "ScsiCommand.h"
#include "Logging.h"

#define	ZZ(a,b,c,d)		{a,b,c,d},
stCDBStruct	ScsiCommand::mCDBFormatTbl[]	=	\
{
	CDB_FORMAT_TABLE
};
#undef ZZ

ScsiCommand::ScsiCommand(uint8_t opCode,enCmdDirect cmdDir,enCDBType cdbType)
	:mOPCode(opCode),mpCDB(NULL),mCDBType(cdbType),mCmdDir(cmdDir)
{
	int cdbSize	= GetCDBSize();

	mpCDB = (uint8_t *)malloc(cdbSize*sizeof(uint8_t));
	memset(mpCDB,0,cdbSize);

	SetRegVal(CDB_FIELD_OPCODE,mOPCode);
}

ScsiCommand::~ScsiCommand()
{
	if(NULL != mpCDB)
	{
		free(mpCDB);
		mpCDB 	= NULL;
	}
}

void ScsiCommand::DumpCDBReg(PFPRINTFN fn)
{
	if(NULL == fn) fn=(PFPRINTFN)printf;

	for(int idx=0; idx < GetCDBSize(); idx++)
	{
		fn("CDB[%d] = 0x%02x\n",idx,mpCDB[idx]);
	}
}

bool ScsiCommand::GetRegPosition(enCDBFieldIdx reg,uint8_t &bitOffset,uint8_t &bitLength)
{
	enCDBType cdbType	= GetCDBType();
	uint8_t tblSize		= sizeof(mCDBFormatTbl)/sizeof(stCDBStruct);

	//LOGDEBUG("tblSize : %d",tblSize);

	bitOffset			= 0;
	bitLength			= 0;

	for(int idx=0;idx<tblSize;idx++)
	{
		stCDBStruct cdb	= mCDBFormatTbl[idx];

		if(cdb.index == reg && (cdb.cdbType == CDB_FENCE ||
				cdb.cdbType == cdbType))
		{
			bitOffset	= cdb.bitsOffset;
			bitLength	= cdb.bitslength;

			//LOGDEBUG("bitOffset : %d,bitLength : %d",bitOffset,bitLength);
			return true;
		}
	}

	return false;
}

void ScsiCommand::SetRegVal(enCDBFieldIdx reg,uint32_t val)
{
	uint8_t bitOffset,bitLength;

	if(true == GetRegPosition(reg,bitOffset,bitLength))
	{
		while(bitLength > 0)
		{
			uint8_t nthByte	= ( (bitOffset+bitLength)>>3 );
			nthByte			-=( ((bitOffset+bitLength) & 0x7) == 0)?1:0;
			uint8_t nthBits	= bitOffset & 0x7;

			uint8_t nBits	= ( 0 == (bitLength%8) )?8:bitLength%8;
			uint8_t bitMask	= ((1 << nBits) - 1) << nthBits;

			//LOGDEBUG("reg:%d",reg);
			//LOGDEBUG("nthByte:%d,nthBits:%d",nthByte,nthBits);
			//LOGDEBUG("nBits:%d,bitMask:%x",nBits,bitMask);

			if(0 == nthBits)
			{
				mpCDB[nthByte]	= ((val<< nthBits) & bitMask);
			}
			else
			{
				uint8_t tmpVal	= mpCDB[nthByte];
				if((nBits + nthBits) > sizeof(uint8_t)*8)
					nBits	= (((nBits + nthBits) >> 3) << 3) - nthBits;;

				mpCDB[nthByte]	= ( (val<< nthBits) & bitMask) | (tmpVal & (~bitMask));
			}

			val = val >> nBits;
			bitLength	-= nBits;
		}
	}
}

uint32_t ScsiCommand::GetRegVal(enCDBFieldIdx reg)
{
	uint32_t	val	= 0;
	uint8_t 	bitOffset,bitLength;

	if(true == GetRegPosition(reg,bitOffset,bitLength))
	{
		while(bitLength > 0)
		{
			//LOGDEBUG("bitLength:%d",bitLength);

			uint8_t nthByte	= (int)(bitOffset/8);
			uint8_t nthBits	= (int)(bitOffset%8);

			uint8_t nBits	= ( 0 == (bitLength%8) )?8:bitLength%8;
			if((nBits + nthBits) > sizeof(uint8_t)*8)
				nBits		= (((nBits + nthBits) >> 3) << 3) - nthBits;

			uint8_t bitMask	= ((1 << nBits) - 1);

			val = val << nBits;
			val	= (val & ~bitMask) | ( (mpCDB[nthByte] & (bitMask << nthBits) ) >> nthBits );

			bitLength	-= nBits;
			bitOffset	+= nBits;
		}
	}

	return val;
}
