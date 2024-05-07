/*
 * file : ut_NVMeMICommand.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "../../../src/inc/protocol/nvme-mi/NVMeMICommand.h"
#include "Logging.h"

class ut_NVMeMICommand : public testing::Test
{
};

TEST_F(ut_NVMeMICommand,CheckNVMeMICmdReq)
{
	NVMeMICmdReq req;

	EXPECT_TRUE(req.SetOPC(0xFF));
	EXPECT_TRUE(req.SetNMD0(0x12345678));
	EXPECT_TRUE(req.SetNMD1(0xFEDCBA09));
	EXPECT_TRUE(req.SetCRC(0x90123456));

	EXPECT_EQ(NMCP_NMIMT_NVMeMICMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(0xFF,req.GetOPC());
	EXPECT_EQ(0x12345678,req.GetNMD0());
	EXPECT_EQ(0xFEDCBA09,req.GetNMD1());
	EXPECT_EQ(0x90123456,req.GetCRC());
}

TEST_F(ut_NVMeMICommand,CheckNVMeMICmdResp)
{
	NVMeMICmdResp resp;

	EXPECT_TRUE(resp.SetStatus(0xFF));
	EXPECT_TRUE(resp.SetNVMeMR(0xFEDCBA));
	EXPECT_TRUE(resp.SetCRC(0x90123456));

	EXPECT_EQ(NMCP_NMIMT_NVMeMICMD,resp.GetNVMeMIMsgType());
	EXPECT_TRUE(resp.GetROR());

	EXPECT_EQ(0xFF,resp.GetStatus());
	EXPECT_EQ(0xFEDCBA,resp.GetNVMeMR());
	EXPECT_EQ(0x90123456,resp.GetCRC());
}

TEST_F(ut_NVMeMICommand,CheckNVMICConfigCmd)
{
	NVMICConfigCmd req;

	EXPECT_TRUE(req.SetCFGID(0xFF));
	EXPECT_TRUE(req.SetNMD0B2(0xEE));
	EXPECT_TRUE(req.SetNMD0B3(0xDD));
	EXPECT_TRUE(req.SetNMD0B4(0xCC));

	EXPECT_EQ(0xFF,req.GetCFGID());
	EXPECT_EQ(0xEE,req.GetNMD0B2());
	EXPECT_EQ(0xDD,req.GetNMD0B3());
	EXPECT_EQ(0xCC,req.GetNMD0B4());
}

TEST_F(ut_NVMeMICommand,CheckNVMICConfigGet)
{
	NVMICConfigGet	req;

	EXPECT_EQ(NMCP_NMIMT_NVMeMICMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(NVMIC_OPC_CFGGET,req.GetOPC());
}

TEST_F(ut_NVMeMICommand,CheckNVMICConfigSet)
{
	NVMICConfigSet	req;

	EXPECT_EQ(NMCP_NMIMT_NVMeMICMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(NVMIC_OPC_CFGSET,req.GetOPC());
}

TEST_F(ut_NVMeMICommand,CheckNVMICCtrlHSP)
{
	NVMICCtrlHSP req;

	EXPECT_TRUE(req.SetCTLID(0x890A));
	EXPECT_TRUE(req.SetMAXRENT(0x88));
	EXPECT_TRUE(req.SetINCF(false));
	EXPECT_TRUE(req.SetINCPF(true));
	EXPECT_TRUE(req.SetINCVF(false));
	EXPECT_TRUE(req.SetRALL(true));
	EXPECT_TRUE(req.SetCSTS(false));
	EXPECT_TRUE(req.SetCTEMP(true));
	EXPECT_TRUE(req.SetPDLU(false));
	EXPECT_TRUE(req.SetSPARE(true));
	EXPECT_TRUE(req.SetCWARN(false));
	EXPECT_TRUE(req.SetCCF(true));

	EXPECT_EQ(NMCP_NMIMT_NVMeMICMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(NVMIC_OPC_CTLRHSP,req.GetOPC());

	EXPECT_EQ(0x890A,req.GetCTLID());
	EXPECT_EQ(0x88,req.GetMAXRENT());

	EXPECT_FALSE(req.GetINCF());
	EXPECT_TRUE(req.GetINCPF());
	EXPECT_FALSE(req.GetINCVF());
	EXPECT_TRUE(req.GetRALL());
	EXPECT_FALSE(req.GetCSTS());
	EXPECT_TRUE(req.GetCTEMP());
	EXPECT_FALSE(req.GetPDLU());
	EXPECT_TRUE(req.GetSPARE());
	EXPECT_FALSE(req.GetCWARN());
	EXPECT_TRUE(req.GetCCF());
}

TEST_F(ut_NVMeMICommand,CheckNVMICNVMSHSP)
{
	NVMICNVMSHSP req;

	EXPECT_TRUE(req.SetCS(true));

	EXPECT_EQ(NMCP_NMIMT_NVMeMICMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(NVMIC_OPC_NVMSHSP,req.GetOPC());

	EXPECT_TRUE(req.GetCS());
}

TEST_F(ut_NVMeMICommand,CheckNVMICReadData)
{
	NVMICReadData req;

	EXPECT_TRUE(req.SetCTRLID(0x890A));
	EXPECT_TRUE(req.SetPORTID(0xBC));
	EXPECT_TRUE(req.SetDTYP(0xDE));

	EXPECT_EQ(NMCP_NMIMT_NVMeMICMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(NVMIC_OPC_RDNVMEMI,req.GetOPC());

	EXPECT_EQ(0x890A,req.GetCTRLID());
	EXPECT_EQ(0xBC,req.GetPORTID());
	EXPECT_EQ(0xDE,req.GetDTYP());
}

TEST_F(ut_NVMeMICommand,CheckNVMICReset)
{
	NVMICReset req;

	EXPECT_TRUE(req.SetTYPE(0x8F));

	EXPECT_EQ(NMCP_NMIMT_NVMeMICMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(NVMIC_OPC_RESET,req.GetOPC());

	EXPECT_EQ(0x8F,req.GetTYPE());
}

TEST_F(ut_NVMeMICommand,CheckNVMICVPDCmd)
{
	NVMICVPDCmd req;

	EXPECT_TRUE(req.SetDOFST(0x890A));
	EXPECT_TRUE(req.SetDLEN(0xBCDE));

	EXPECT_EQ(NMCP_NMIMT_NVMeMICMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());

	EXPECT_EQ(0x890A,req.GetDOFST());
	EXPECT_EQ(0xBCDE,req.GetDLEN());
}

TEST_F(ut_NVMeMICommand,CheckNVMICVPDRead)
{
	NVMICVPDRead req;

	EXPECT_EQ(NMCP_NMIMT_NVMeMICMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(NVMIC_OPC_VPDREAD,req.GetOPC());
}

TEST_F(ut_NVMeMICommand,CheckNVMICVPDWrite)
{
	NVMICVPDWrite req;

	EXPECT_EQ(NMCP_NMIMT_NVMeMICMD,req.GetNVMeMIMsgType());
	EXPECT_FALSE(req.GetROR());
	EXPECT_EQ(NVMIC_OPC_VPDWRITE,req.GetOPC());
}
