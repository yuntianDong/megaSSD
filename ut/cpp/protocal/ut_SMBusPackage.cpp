/*
 * file : ut_SMBusPackage.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "../../../src/inc/protocol/smbus/SMBusPackage.h"
#include "Logging.h"

class ut_SMBusPackage : public testing::Test
{
};

TEST_F(ut_SMBusPackage,CheckSMBusPackage)
{
	SMBusPackage	req(100);

	EXPECT_TRUE(req.SetDestSlaveAddr(0x88));
	EXPECT_TRUE(req.SetCommandCode(0x99));
	EXPECT_TRUE(req.SetByteCount(0xAA));
	EXPECT_TRUE(req.SetSrcSlaveAddr(0xBB));
	EXPECT_TRUE(req.SetPEC(0xCC));

	EXPECT_EQ(0x88,req.GetDestSlaveAddr());
	EXPECT_EQ(0x99,req.GetCommandCode());
	EXPECT_EQ(0xAA,req.GetByteCount());
	EXPECT_EQ(0xBB,req.GetSrcSlaveAddr());
	EXPECT_EQ(0xCC,req.GetPEC());
}
