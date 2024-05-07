#ifndef _BOOST_CRC_H
#define _BOOST_CRC_H

#include <stdint.h>
#include <boost/crc.hpp>

using namespace boost;

#define BAD_CRC_RETURN		0x8BADBAD8

#define	ALL_WELLKNOWN_CRC_TBL	\
	ZZ(	CRC_1		,	1	,	0x1		,	0	,	0	,	false	,	false	)	\
	ZZ(	CRC_5_CCITT	,	5	,	0xB		,	0	,	0	,	false	,	false	)	\
	ZZ(	CRC_5_USB	,	5	,	0x5		,	0	,	0	,	false	,	false	)	\
	ZZ(	CRC_7		,	7	,	0x9		,	0	,	0	,	false	,	false	)	\
	ZZ(	CRC_8_ATM	,	8	,	0x7		,	0	,	0	,	false	,	false	)	\
	ZZ(	CRC_8_CCITT	,	8	,	0x8D	,	0	,	0	,	false	,	false	)	\
	ZZ(	CRC_8_Dallas,	8	,	0x31	,	0	,	0	,	false	,	false	)	\
	ZZ(	CRC_8		,	8	,	0xD5	,	0	,	0	,	false	,	false	)	\
	ZZ( CRC_10		,	10	,	0x233	,	0	,	0	,	false	,	false	)	\
	ZZ( CRC_12		,	12	,	0x80F	,	0	,	0	,	false	,	false	)	\
	ZZ( CRC_16_CCITT,	16	,	0x1021	,	0	,	0	,	false	,	false	)	\
	ZZ( CRC_16_IBM	,	16	,	0x8005	,	0	,	0	,	false	,	false	)	\
	ZZ( CRC_16_BBS	,	16	,	0x8408	,	0	,	0	,	false	,	false	)	\
	ZZ( CRC_16_NVM16BCRC	,	16	,	0x8BB7	,	0	,	0	,	false	,	false	)	\
	ZZ( CRC_32_IEEE ,	32	,	0x04C11DB7	,	0xFFFFFFFF	,	0xFFFFFFFF	,	true	,	true	)	\
	ZZ( CRC_32C 	,	32	,	0x1EDC6F41	,	0xFFFFFFFF	,	0xFFFFFFFF	,	true	,	true	)	\
	ZZ( CRC_64_ISO 	,	64	,	0x000000000000001B	,	0xFFFFFFFFFFFFFFFF	,	0xFFFFFFFFFFFFFFFF	,	true	,	true	)	\
	ZZ( CRC_64_ECMA_182 	,	64	,	0x42F0E1EBA9EA3693	,	0xFFFFFFFFFFFFFFFF	,	0xFFFFFFFFFFFFFFFF	,	true	,	true	)	\
	ZZ( CRC_64_NVM64BCRC 	,	64	,	0xAD93D23594C93659	,	0xFFFFFFFFFFFFFFFF	,	0xFFFFFFFFFFFFFFFF	,	true	,	true	)	\

#define ZZ(a,b,c,d,e,f,h)	a,
typedef enum _enCRCType
{
	ALL_WELLKNOWN_CRC_TBL
}enCRCType;
#undef ZZ

typedef struct _stCRCParam
{
	enCRCType	type;
	uint8_t		bits;
	uint64_t	truncPoly;
	uint64_t	initRem;
	uint64_t	finalXor;
	bool		reflectIn;
	bool		reflectRem;
}stCRCParam;

class AshaCRCMngr
{
private:
	bool	FindCRCParam(enCRCType type,uint8_t& bits,uint64_t& truncPoly,uint64_t& initRem,\
			uint64_t& finalXor,bool& reflectIn,bool& reflectRem)
	{
		if(type >= mWKNCRCTblCnt)
		{
			return false;
		}

		stCRCParam& param = mWellKnownCRCTbl[type];
		bits		= param.bits;
		truncPoly	= param.truncPoly;
		initRem		= param.initRem;
		finalXor	= param.finalXor;
		reflectIn	= param.reflectIn;
		reflectRem	= param.reflectRem;

		return true;
	};
public:
	AshaCRCMngr(){};

	static	stCRCParam	mWellKnownCRCTbl[];
	static	int		mWKNCRCTblCnt;

	uint64_t operator()(enCRCType type,uint8_t* bytes,uint32_t length)
	{
		uint8_t		bits		= 0;
		uint64_t	truncPoly	= 0;
		uint64_t	initRem		= 0;
		uint64_t	finalXor	= 0;
		bool		reflectIn	= false;
		bool		reflectRem	= false;

		if(false == FindCRCParam(type,bits,truncPoly,initRem,finalXor,reflectIn,reflectRem))
		{
			return BAD_CRC_RETURN;
		}

		switch(bits)
		{
			case	1:
			{
				crc_basic<1> crc(truncPoly,initRem,finalXor,reflectIn,reflectRem);
				crc.process_bytes(bytes,length);
				return crc.checksum();
			}
			case	5:
			{
				crc_basic<5> crc(truncPoly,initRem,finalXor,reflectIn,reflectRem);
				crc.process_bytes(bytes,length);
				return crc.checksum();
			}
			case	7:
			{
				crc_basic<7> crc(truncPoly,initRem,finalXor,reflectIn,reflectRem);
				crc.process_bytes(bytes,length);
				return crc.checksum();
			}
			case	8:
			{
				crc_basic<8> crc(truncPoly,initRem,finalXor,reflectIn,reflectRem);
				crc.process_bytes(bytes,length);
				return crc.checksum();
			}
			case	10:
			{
				crc_basic<10> crc(truncPoly,initRem,finalXor,reflectIn,reflectRem);
				crc.process_bytes(bytes,length);
				return crc.checksum();
			}
			case	12:
			{
				crc_basic<12> crc(truncPoly,initRem,finalXor,reflectIn,reflectRem);
				crc.process_bytes(bytes,length);
				return crc.checksum();
			}
			case	16:
			{
				crc_basic<16> crc(truncPoly,initRem,finalXor,reflectIn,reflectRem);
				crc.process_bytes(bytes,length);
				return crc.checksum();
			}
			case	32:
			{
				crc_basic<32> crc(truncPoly,initRem,finalXor,reflectIn,reflectRem);
				crc.process_bytes(bytes,length);
				return crc.checksum();
			}
			case	64:
			{
				crc_basic<64> crc(truncPoly,initRem,finalXor,reflectIn,reflectRem);
				crc.process_bytes(bytes,length);
				return crc.checksum();
			}
			default:
				return BAD_CRC_RETURN;
		}
	}
};

uint8_t	 CalCRC8(uint8_t* bytes,uint32_t length);
uint16_t CalCRC16(uint8_t* bytes,uint32_t length);
uint32_t CalCRC32(uint8_t* bytes,uint32_t length);

uint16_t CalCRC16NVM16BCRC(uint8_t* bytes,uint32_t length);
uint32_t CalCRC32NVM32BCRC(uint8_t* bytes,uint32_t length);
uint64_t CalCRC64NVM64BCRC(uint8_t* bytes,uint32_t length);

#endif
