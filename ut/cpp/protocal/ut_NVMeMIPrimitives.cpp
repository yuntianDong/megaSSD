/*
 * file : ut_NVMeMIPrimitives.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "../../../src/inc/protocol/nvme-mi/NVMeMIPrimitives.h"
#include "Logging.h"

class ut_NVMeMIPrimitives : public testing::Test
{
};

TEST_F(ut_NVMeMIPrimitives,CheckNVMeMIPrimReq)
{
	NVMeMIPrimReq req;

	EXPECT_TRUE(req.SetCSI(0));
	EXPECT_TRUE(req.SetCPO(0x55));
	EXPECT_TRUE(req.SetTag(0xAA));
	EXPECT_TRUE(req.SetCPSP(0xBBCC));
	EXPECT_TRUE(req.SetCRC(0x12345678));

	EXPECT_EQ(0x4,req.GetMessageType());
	EXPECT_TRUE(req.GetIC());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(0,req.GetCSI());
	EXPECT_EQ(0x0,req.GetNVMeMIMsgType());
	EXPECT_EQ(0x55,req.GetCPO());
	EXPECT_EQ(0xAA,req.GetTag());
	EXPECT_EQ(0xBBCC,req.GetCPSP());
	EXPECT_EQ(0x12345678,req.GetCRC());

	req.Debug();
}

TEST_F(ut_NVMeMIPrimitives,CheckNVMeMIPrimResp)
{
	NVMeMIPrimResp resp;

	EXPECT_TRUE(resp.SetCSI(0));
	EXPECT_TRUE(resp.SetStatus(0x55));
	EXPECT_TRUE(resp.SetTag(0xAA));
	EXPECT_TRUE(resp.SetCPSR(0xBBCC));
	EXPECT_TRUE(resp.SetCRC(0x12345678));

	EXPECT_EQ(0x4,resp.GetMessageType());
	EXPECT_TRUE(resp.GetIC());
	EXPECT_EQ(0,resp.GetCSI());
	EXPECT_TRUE(resp.GetROR());
	EXPECT_EQ(0x0,resp.GetNVMeMIMsgType());
	EXPECT_EQ(0x55,resp.GetStatus());
	EXPECT_EQ(0xAA,resp.GetTag());
	EXPECT_EQ(0xBBCC,resp.GetCPSR());
	EXPECT_EQ(0x12345678,resp.GetCRC());

	resp.Debug();
}

TEST_F(ut_NVMeMIPrimitives,CheckNMCPCmdPause)
{
	NMCPCmdPause req;

	EXPECT_EQ(0x4,req.GetMessageType());
	EXPECT_TRUE(req.GetIC());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(0,req.GetCSI());
	EXPECT_EQ(0x0,req.GetNVMeMIMsgType());
	EXPECT_EQ(NMCP_CPO_PAUSE,req.GetCPO());
}

TEST_F(ut_NVMeMIPrimitives,CheckNMCPCmdResume)
{
	NMCPCmdResume req;

	EXPECT_EQ(0x4,req.GetMessageType());
	EXPECT_TRUE(req.GetIC());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(0,req.GetCSI());
	EXPECT_EQ(0x0,req.GetNVMeMIMsgType());
	EXPECT_EQ(NMCP_CPO_RESUME,req.GetCPO());
}

TEST_F(ut_NVMeMIPrimitives,CheckNMCPCmdAbort)
{
	NMCPCmdAbort req;

	EXPECT_EQ(0x4,req.GetMessageType());
	EXPECT_TRUE(req.GetIC());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(0,req.GetCSI());
	EXPECT_EQ(0x0,req.GetNVMeMIMsgType());
	EXPECT_EQ(NMCP_CPO_ABORT,req.GetCPO());
}

TEST_F(ut_NVMeMIPrimitives,CheckNMCPCmdGetState)
{
	NMCPCmdGetState req;

	EXPECT_TRUE(req.SetCESF(true));

	EXPECT_EQ(0x4,req.GetMessageType());
	EXPECT_TRUE(req.GetIC());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(0,req.GetCSI());
	EXPECT_EQ(0x0,req.GetNVMeMIMsgType());
	EXPECT_EQ(NMCP_CPO_GETSTATE,req.GetCPO());
	EXPECT_TRUE(req.GetCESF());
}

TEST_F(ut_NVMeMIPrimitives,CheckNMCPCmdReplay)
{
	NMCPCmdReplay req;

	EXPECT_TRUE(req.SetRRO(0xCC));

	EXPECT_EQ(0x4,req.GetMessageType());
	EXPECT_TRUE(req.GetIC());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(0,req.GetCSI());
	EXPECT_EQ(0x0,req.GetNVMeMIMsgType());
	EXPECT_EQ(NMCP_CPO_REPLAY,req.GetCPO());
	EXPECT_EQ(0xCC,req.GetRRO());
}
