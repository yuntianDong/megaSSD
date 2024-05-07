/*
 * file : ut_MCTPRegMap.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "../../../src/inc/protocol/mctp/MCTPRegMap.h"
#include "Logging.h"

class ut_MCTPRegMap : public testing::Test
{
};

TEST_F(ut_MCTPRegMap,CheckMCTPPakcageRegMap)
{
	MCTPPackageRegMap mpRegMap;
	mpRegMap.SetHeaderVersion(0x1);
	EXPECT_EQ(0x1,mpRegMap.GetHeaderVersion());

	mpRegMap.SetDstEPID(0x2);
	EXPECT_EQ(0x2,mpRegMap.GetDstEPID());

	mpRegMap.SetSrcEPID(0x3);
	EXPECT_EQ(0x3,mpRegMap.GetSrcEPID());

	mpRegMap.SetMsgTag(0x7);
	EXPECT_EQ(0x7,mpRegMap.GetMsgTag());

	mpRegMap.SetTO(true);
	EXPECT_TRUE(mpRegMap.GetTO());

	mpRegMap.SetPktSeq(0x2);
	EXPECT_EQ(0x2,mpRegMap.GetPktSeq());

	mpRegMap.SetSOM(true);
	EXPECT_TRUE(mpRegMap.GetSOM());
	EXPECT_FALSE(mpRegMap.GetEOM());

	printf("DWORD[0] = %x\n",mpRegMap.GetBuffer()->GetDWord(0));
	mpRegMap.GetBuffer()->Dump(0,32);
}

TEST_F(ut_MCTPRegMap,CheckMCTPNVMeMIMsgRegMap)
{
	MCTPNVMeMIMsgRegMap mpRegMap;
	EXPECT_EQ(0x4,mpRegMap.GetMessageType());
	EXPECT_TRUE(mpRegMap.GetIC());

	mpRegMap.SetCSI(0x1);
	EXPECT_EQ(0x1,mpRegMap.GetCSI());

	mpRegMap.SetReserved(0x3);
	EXPECT_EQ(0x3,mpRegMap.GetReserved());

	mpRegMap.SetNVMeMIMsgType(0xA);
	EXPECT_EQ(0xA,mpRegMap.GetNVMeMIMsgType());

	mpRegMap.SetROR(true);
	EXPECT_TRUE(mpRegMap.GetROR());

	mpRegMap.SetReserved2(0x1234);
	EXPECT_EQ(0x1234,mpRegMap.GetReserved2());

	mpRegMap.SetCRC(0xFEDCBA09);
	EXPECT_EQ(0xFEDCBA09,mpRegMap.GetCRC());

	mpRegMap.GetBuffer()->Dump(0,64);
	mpRegMap.Debug();
}
