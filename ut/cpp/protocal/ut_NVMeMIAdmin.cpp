/*
 * file : ut_NVMeMIAdmin.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "../../../src/inc/protocol/nvme-mi/NVMeMIAdmin.h"
#include "Logging.h"

class ut_NVMeMIAdmin : public testing::Test
{
};

TEST_F(ut_NVMeMIAdmin,CheckNVMAdminReq)
{
	NVMAdminReq  req;

	EXPECT_TRUE(req.SetOPC(0x88));
	EXPECT_TRUE(req.SetCFLGS(0x55));
	EXPECT_TRUE(req.SetCTLID(0x7788));
	EXPECT_TRUE(req.SetSQEDW1(0x11111111));
	EXPECT_TRUE(req.SetSQEDW2(0x22222222));
	EXPECT_TRUE(req.SetSQEDW3(0x33333333));
	EXPECT_TRUE(req.SetSQEDW4(0x44444444));
	EXPECT_TRUE(req.SetSQEDW5(0x55555555));
	EXPECT_TRUE(req.SetDOFST(0x66666666));
	EXPECT_TRUE(req.SetDLEN(0x77777777));
	EXPECT_TRUE(req.SetSQEDW8(0x88888888));
	EXPECT_TRUE(req.SetSQEDW9(0x99999999));
	EXPECT_TRUE(req.SetSQEDW10(0xAAAAAAAA));
	EXPECT_TRUE(req.SetSQEDW11(0xBBBBBBBB));
	EXPECT_TRUE(req.SetSQEDW12(0xCCCCCCCC));
	EXPECT_TRUE(req.SetSQEDW13(0xDDDDDDDD));
	EXPECT_TRUE(req.SetSQEDW14(0xEEEEEEEE));
	EXPECT_TRUE(req.SetSQEDW15(0xFFFFFFFF));

	EXPECT_EQ(NMCP_NMIMT_NVMeADMIN,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());

	EXPECT_EQ(0x88,req.GetOPC());
	EXPECT_EQ(0x55,req.GetCFLGS());
	EXPECT_EQ(0x7788,req.GetCTLID());
	EXPECT_EQ(0x11111111,req.GetSQEDW1());
	EXPECT_EQ(0x22222222,req.GetSQEDW2());
	EXPECT_EQ(0x33333333,req.GetSQEDW3());
	EXPECT_EQ(0x44444444,req.GetSQEDW4());
	EXPECT_EQ(0x55555555,req.GetSQEDW5());
	EXPECT_EQ(0x66666666,req.GetDOFST());
	EXPECT_EQ(0x77777777,req.GetDLEN());
	EXPECT_EQ(0x88888888,req.GetSQEDW8());
	EXPECT_EQ(0x99999999,req.GetSQEDW9());
	EXPECT_EQ(0xAAAAAAAA,req.GetSQEDW10());
	EXPECT_EQ(0xBBBBBBBB,req.GetSQEDW11());
	EXPECT_EQ(0xCCCCCCCC,req.GetSQEDW12());
	EXPECT_EQ(0xDDDDDDDD,req.GetSQEDW13());
	EXPECT_EQ(0xEEEEEEEE,req.GetSQEDW14());
	EXPECT_EQ(0xFFFFFFFF,req.GetSQEDW15());
}

TEST_F(ut_NVMeMIAdmin,CheckNVMAdminResp)
{
	NVMAdminResp resp;

	EXPECT_TRUE(resp.SetStatus(0xFF));
	EXPECT_TRUE(resp.SetCQEDW0(0x88888888));
	EXPECT_TRUE(resp.SetCQEDW1(0xAAAAAAAA));
	EXPECT_TRUE(resp.SetCQEDW3(0xCCCCCCCC));

	EXPECT_EQ(NMCP_NMIMT_NVMeADMIN,resp.GetNVMeMIMsgType());
	EXPECT_TRUE(resp.GetROR());

	EXPECT_EQ(0xFF,resp.GetStatus());
	EXPECT_EQ(0x88888888,resp.GetCQEDW0());
	EXPECT_EQ(0xAAAAAAAA,resp.GetCQEDW1());
	EXPECT_EQ(0xCCCCCCCC,resp.GetCQEDW3());
}