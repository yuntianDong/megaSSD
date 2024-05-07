/*
 * file : ut_NVMeMIPCIeCmd.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "../../../src/inc/protocol/nvme-mi/NVMeMIPCIeCmd.h"
#include "Logging.h"

class ut_NVMeMIPCIeCmd : public testing::Test
{
};

TEST_F(ut_NVMeMIPCIeCmd,CheckMIPCIeCmdReq)
{
	MIPCIeCmdReq	req;

	EXPECT_TRUE(req.SetOPC(0x88));
	EXPECT_TRUE(req.SetCTLID(0x99AA));
	EXPECT_TRUE(req.SetNMD0(0x88888888));
	EXPECT_TRUE(req.SetNMD1(0xAAAAAAAA));
	EXPECT_TRUE(req.SetNMD2(0xEEEEEEEE));

	EXPECT_EQ(NMCP_NMIMT_PCIECMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());

	EXPECT_EQ(0x88,req.GetOPC());
	EXPECT_EQ(0x99AA,req.GetCTLID());
	EXPECT_EQ(0x88888888,req.GetNMD0());
	EXPECT_EQ(0xAAAAAAAA,req.GetNMD1());
	EXPECT_EQ(0xEEEEEEEE,req.GetNMD2());
}

TEST_F(ut_NVMeMIPCIeCmd,CheckMIPCIeCmdResp)
{
	MIPCIeCmdResp resp;

	EXPECT_TRUE(resp.SetStatus(0x88));

	EXPECT_EQ(NMCP_NMIMT_PCIECMD,resp.GetNVMeMIMsgType());
	EXPECT_TRUE(resp.GetROR());
	EXPECT_EQ(0x88,resp.GetStatus());
}

TEST_F(ut_NVMeMIPCIeCmd,CheckMIPCIeCfgCmd)
{
	MIPCIeCfgCmd req;

	EXPECT_TRUE(req.SetLength(0x890A));
	EXPECT_TRUE(req.SetOffset(0xFFF));

	EXPECT_EQ(NMCP_NMIMT_PCIECMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());

	EXPECT_EQ(0x890A,req.GetLength());
	EXPECT_EQ(0xFFF,req.GetOffset());
}

TEST_F(ut_NVMeMIPCIeCmd,CheckMIPCIeCfgRead)
{
	MIPCIeCfgRead req;

	EXPECT_EQ(NMCP_NMIMT_PCIECMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(MIPCIE_OPC_CFGREAD,req.GetOPC());
}

TEST_F(ut_NVMeMIPCIeCmd,CheckMIPCIeCfgWrite)
{
	MIPCIeCfgWrite req;

	EXPECT_EQ(NMCP_NMIMT_PCIECMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(MIPCIE_OPC_CFGWRITE,req.GetOPC());
}

TEST_F(ut_NVMeMIPCIeCmd,CheckMIPCIeIOCmd)
{
	MIPCIeIOCmd req;

	EXPECT_TRUE(req.SetBAR(0x7));
	EXPECT_TRUE(req.SetLength(0x890A));
	EXPECT_TRUE(req.SetOffset(0xFFFFFFFF));

	EXPECT_EQ(NMCP_NMIMT_PCIECMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());

	EXPECT_EQ(0x7,req.GetBAR());
	EXPECT_EQ(0x890A,req.GetLength());
	EXPECT_EQ(0xFFFFFFFF,req.GetOffset());
}

TEST_F(ut_NVMeMIPCIeCmd,CheckMIPCIeIORead)
{
	MIPCIeIORead req;

	EXPECT_EQ(NMCP_NMIMT_PCIECMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(MIPCIE_OPC_IOREAD,req.GetOPC());
}

TEST_F(ut_NVMeMIPCIeCmd,CheckMIPCIeIOWrite)
{
	MIPCIeIOWrite req;

	EXPECT_EQ(NMCP_NMIMT_PCIECMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(MIPCIE_OPC_IOWRITE,req.GetOPC());
}

TEST_F(ut_NVMeMIPCIeCmd,CheckMIPCIeMemCmd)
{
	MIPCIeMemCmd req;

	EXPECT_TRUE(req.SetBAR(0x7));
	EXPECT_TRUE(req.SetLength(0x890A));
	EXPECT_TRUE(req.SetOffsetLow(0xEEEEEEEE));
	EXPECT_TRUE(req.SetOffsetHigh(0xCCCCCCCC));

	EXPECT_EQ(NMCP_NMIMT_PCIECMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());

	EXPECT_EQ(0x7,req.GetBAR());
	EXPECT_EQ(0x890A,req.GetLength());
	EXPECT_EQ(0xEEEEEEEE,req.GetOffsetLow());
	EXPECT_EQ(0xCCCCCCCC,req.GetOffsetHigh());
}

TEST_F(ut_NVMeMIPCIeCmd,CheckMIPCIeMemRead)
{
	MIPCIeMemRead req;

	EXPECT_EQ(NMCP_NMIMT_PCIECMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(MIPCIE_OPC_MEMREAD,req.GetOPC());
}

TEST_F(ut_NVMeMIPCIeCmd,CheckMIPCIeMemWrite)
{
	MIPCIeMemWrite req;

	EXPECT_EQ(NMCP_NMIMT_PCIECMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(MIPCIE_OPC_MEMWRITE,req.GetOPC());
}
