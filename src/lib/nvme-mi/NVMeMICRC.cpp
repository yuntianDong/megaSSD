/*
 * file NVMeMICRC.cpp
 */

#include <stdlib.h>
#include "nvme-mi/NVMeMICRC.h"

#define CRC_TABLE_SIZE		256

uint8_t smbus_crc8(uint8_t crcInitial, uint8_t* data, uint32_t len)
{
    uint8_t i;
    uint8_t crc = crcInitial;
    while (len--)
    {
        crc ^= *data++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ 0x07;
            }
            else
                crc <<= 1;
        }
    }
    return crc;
}

uint32_t DwordBitRevert(uint32_t val)
{
	uint32_t ret;
	uint8_t index;

	for(index = 0; index < 32; index++)
	{
		ret |= val&0x1;
		if(index == 31)
			break;

		ret <<= 1;
		val >>= 1;
	}

	return ret;
}

uint8_t* Crc32_Init(uint32_t poly)
{
	uint32_t revPoly;
	uint16_t index;
	uint32_t u32Gen;
	uint8_t byteOffset;

	revPoly = DwordBitRevert(poly);

	uint8_t* Crc32Table	= (uint8_t*)malloc(CRC_TABLE_SIZE*sizeof(uint8_t));

	for(index = 0; index < 256; index++)
	{
		u32Gen = index;

		for(byteOffset = 0; byteOffset < 8; byteOffset++)
		{
			if(u32Gen&0x1)
				u32Gen = revPoly^(u32Gen>>1);
			else
				u32Gen >>= 1;
		}
		Crc32Table[index] = u32Gen;
	}

	return Crc32Table;
}

uint32_t CalculateCRC32(uint8_t* crc32Tbl,uint8_t* data, uint32_t len)
{
	uint32_t index;
	uint8_t position;
	uint32_t initVal = 0xffffffff;

	if(data == NULL || crc32Tbl == NULL)
		return 0;

	for(index = 0; index < len; index++)
	{
		position = (uint8_t)(initVal^data[index]);
		initVal = (initVal>>8) ^ (crc32Tbl[position]);
	}

	return (initVal^0xffffffff);
}
