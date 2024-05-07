/*
 * file : ut_RegisterTbl.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "../../../src/inc/protocol/RegisterTbl.h"
#include "Logging.h"

class ut_RegisterTbl : public testing::Test
{
};

TEST_F(ut_RegisterTbl,CheckBasic)
{
	RegisterTbl tbl;

	EXPECT_TRUE(tbl.AddRegDesc("HeaderVersion",0 , 0 , 4 , REG_OFF_F2B));

	EXPECT_EQ(REG_OFF_F2B,tbl.GetRegOffType("HeaderVersion"));
	EXPECT_EQ(REG_LEN_VAL,tbl.GetRegLenType("HeaderVersion"));

	stRegOffset bytOff,bitOff;
	EXPECT_TRUE(tbl.GetRegByteOff("HeaderVersion",bytOff));
	EXPECT_TRUE(tbl.GetRegBitOff("HeaderVersion",bitOff));
	EXPECT_EQ(bytOff.offVal,0);
	EXPECT_EQ(bitOff.offVal,0);

	stRegLength nBits;
	EXPECT_TRUE(tbl.GetRegLength("HeaderVersion",nBits));
	EXPECT_EQ(nBits.lenVal,4);
}
