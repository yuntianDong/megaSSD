/*
 * file: CRC.cpp
 */

#include "utility/CRC.h"

#define	ZZ(a,b,c,d,e,f,h)	{a,b,c,d,e,f,h},
stCRCParam	AshaCRCMngr::mWellKnownCRCTbl[]	=
{
	ALL_WELLKNOWN_CRC_TBL
};
#undef	ZZ
int	AshaCRCMngr::mWKNCRCTblCnt	= sizeof(AshaCRCMngr::mWellKnownCRCTbl) / sizeof(AshaCRCMngr::mWellKnownCRCTbl[0]);


uint8_t	 CalCRC8(uint8_t* bytes,uint32_t length)
{
	return (uint8_t)AshaCRCMngr()(CRC_8_ATM,bytes,length);
}

uint16_t CalCRC16(uint8_t* bytes,uint32_t length)
{
	return (uint16_t)AshaCRCMngr()(CRC_16_CCITT,bytes,length);
}

uint32_t CalCRC32(uint8_t* bytes,uint32_t length)
{
	return (uint32_t)AshaCRCMngr()(CRC_32C,bytes,length);
}

uint16_t CalCRC16NVM16BCRC(uint8_t* bytes,uint32_t length)
{
	return (uint16_t)AshaCRCMngr()(CRC_16_NVM16BCRC,bytes,length);
}

uint32_t CalCRC32NVM32BCRC(uint8_t* bytes,uint32_t length)
{
	return (uint32_t)AshaCRCMngr()(CRC_32C,bytes,length);
}

uint64_t CalCRC64NVM64BCRC(uint8_t* bytes,uint32_t length)
{
	return (uint64_t)AshaCRCMngr()(CRC_64_NVM64BCRC,bytes,length);
}
