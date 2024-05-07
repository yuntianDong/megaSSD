/*
 * file ut_MPCtrlResp.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "power/MPCtrlResp.h"
#include "Logging.h"

uint8_t EXP_RESP_GETPOWERID[]	= {0x55,0x27,0x60,0x20,0x45,0x31,0x54,0x31,0x30,0x39,0x30,0x30,0x34,0x30,0x30,0x34,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x9B,0xB1};

uint8_t TEST_GETPOWERID_DATUM[]	= {0x45,0x31,0x54,0x31,0x30,0x39,0x30,0x30,0x34,0x30,0x30,0x34,0x00,0x00,0x00,0x00,\
								   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

#define COMP2SEQ(a,b,c)	\
	for(uint8_t idx=0;idx<c;idx++)	\
	{	\
		EXPECT_EQ(a[idx],b[idx]);	\
	}	\

class ut_MPCtrlResp : public testing::Test
{
};

TEST_F(ut_MPCtrlResp,CheckGetPowerID)
{
	MPCtrlResp resp(EXP_RESP_GETPOWERID,sizeof(EXP_RESP_GETPOWERID));

	uint8_t	actDatum[0x20];
	resp.GetDatum(actDatum,0x20);

	COMP2SEQ(TEST_GETPOWERID_DATUM,actDatum,0x20);
	EXPECT_TRUE(resp.CheckValid());

	resp.Dump();
	resp.DumpHex();
}