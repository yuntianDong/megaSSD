#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "nvme-mi/NVMeMICRC.h"
#include "utility/CRC.h"
#include "Logging.h"

class utNVMeCRC : public testing::Test
{
};

TEST_F(utNVMeCRC,CheckCRC8ATM)
{
	uint8_t data[]	= {0xf,0x11,0x1,0x0,0x0,0xc0,0x84,0x0,0x0,0x0,0x3,0x0,0x1,0x0,0xff,0xff,0xff};
	uint8_t dLen	= sizeof(data)/sizeof(uint8_t);

	uint8_t crc8	= smbus_crc8(0,data,dLen);
	EXPECT_EQ(crc8,0x78);

	uint8_t crc8_2	= AshaCRCMngr()(CRC_8_ATM,data,dLen);
	EXPECT_EQ(crc8_2,0x78);

	for(int idx=0;idx<100;idx++)
	{
		uint8_t val	= smbus_crc8(0,data,dLen);
		EXPECT_EQ(val,0x78);
	}
}

TEST_F(utNVMeCRC,CheckCRC8ATM1)
{
	uint8_t data1[]	= {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	uint8_t data2[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	uint8_t data3[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	uint8_t data4[] = {0xFF, 0x0,0xFF, 0x0,0xFF, 0x0,0xFF, 0x0};

	uint8_t crc8_1	= smbus_crc8(0,data1,sizeof(data1)/sizeof(uint8_t));
	EXPECT_EQ(crc8_1,0xD7);
	uint8_t crc8_2	= smbus_crc8(0,data2,sizeof(data2)/sizeof(uint8_t));
	EXPECT_EQ(crc8_2,0x0);
	uint8_t crc8_3	= smbus_crc8(0,data3,sizeof(data3)/sizeof(uint8_t));
	EXPECT_EQ(crc8_3,0x0C);
	uint8_t crc8_4	= smbus_crc8(0,data4,sizeof(data4)/sizeof(uint8_t));
	EXPECT_EQ(crc8_4,0x72);
}

TEST_F(utNVMeCRC,CheckCRC32C)
{
	uint8_t data[]	= {0x84,0x80,0x00,0x00,0xEE,0xDD,0xCC,0xBB};
	uint8_t dLen	= sizeof(data)/sizeof(uint8_t);

	uint32_t crc32_2	= AshaCRCMngr()(CRC_32C,data,dLen);
	EXPECT_EQ(crc32_2,0xB3809E6A);
	printf("crc32(Boost) = %x\n",crc32_2);
}

