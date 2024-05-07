/*
 * file : utMCUPwrCtrlr.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "power/MCUPwrCtrlr.h"
#include "Logging.h"

#define  PWR_USB_DEV_NAME		"/dev/ttyACM0"

class utMCUPwrCtrlr : public testing::Test
{
public:
	MCUPwrCtrlr*		mpPwrCtlr;

	virtual void SetUp()
	{
		mpPwrCtlr = new MCUPwrCtrlr(PWR_USB_DEV_NAME);
	}

	virtual void TearDown()
	{
		if(NULL != mpPwrCtlr)
		{
			delete mpPwrCtlr;
			mpPwrCtlr = NULL;
		}
	}
};

TEST_F(utMCUPwrCtrlr,CheckBasic)
{
	EXPECT_TRUE(mpPwrCtlr->PowerOff(0));
	EXPECT_FALSE(mpPwrCtlr->DetectDevice(0));
	EXPECT_TRUE(mpPwrCtlr->PowerOn(0));
	EXPECT_TRUE(mpPwrCtlr->DetectDevice(0));
}

TEST_F(utMCUPwrCtrlr,CheckMeasure)
{
	EXPECT_TRUE(mpPwrCtlr->PowerOn(0));

	EXPECT_GT(mpPwrCtlr->GetCurrent(0),0);
	EXPECT_GT(mpPwrCtlr->GetVoltage(0),0);
	EXPECT_GT(mpPwrCtlr->GetPower(0),0);
	EXPECT_GT(mpPwrCtlr->GetTemp(0),0);

	EXPECT_TRUE(mpPwrCtlr->PowerOff(0));
	EXPECT_EQ(mpPwrCtlr->GetCurrent(0),0);
	EXPECT_GT(mpPwrCtlr->GetVoltage(0),0);
	EXPECT_EQ(mpPwrCtlr->GetPower(0),0);
	EXPECT_EQ(mpPwrCtlr->GetTemp(0),0);
}

TEST_F(utMCUPwrCtrlr,CheckPowerID)
{
	EXPECT_TRUE(mpPwrCtlr->PowerOff(0));

	uint8_t expPowID[MPC_REQ_DLEN_POWERID];
	uint8_t actPowID[MPC_REQ_DLEN_POWERID];
	memset(expPowID,0xCC,MPC_REQ_DLEN_POWERID);
	memset(actPowID,0,MPC_REQ_DLEN_POWERID);
	EXPECT_TRUE(mpPwrCtlr->SetPowerID(expPowID,MPC_REQ_DLEN_POWERID));
	EXPECT_TRUE(mpPwrCtlr->GetPowerID(actPowID,1));
	EXPECT_EQ(actPowID[0],0xCC);
	EXPECT_EQ(actPowID[1],0);
	EXPECT_TRUE(mpPwrCtlr->GetPowerID(actPowID,6));
	EXPECT_EQ(actPowID[5],0xCC);
	EXPECT_EQ(actPowID[6],0);
	EXPECT_TRUE(mpPwrCtlr->GetPowerID(actPowID,32));
	EXPECT_EQ(actPowID[31],0xCC);

	EXPECT_TRUE(mpPwrCtlr->PowerOn(0));
	memset(expPowID,0xAA,MPC_REQ_DLEN_POWERID);
	EXPECT_TRUE(mpPwrCtlr->SetPowerID(expPowID,MPC_REQ_DLEN_POWERID));
	EXPECT_TRUE(mpPwrCtlr->GetPowerID(actPowID,32));
	EXPECT_EQ(actPowID[0],0xAA);
	EXPECT_EQ(actPowID[31],0xAA);
}

TEST_F(utMCUPwrCtrlr,CheckEEPROM)
{
	EXPECT_TRUE(mpPwrCtlr->PowerOn(0));

	uint8_t expSN[20];
	uint8_t actSN[20];
	memset(expSN,0,20);
	memset(actSN,0,20);
	EXPECT_TRUE(mpPwrCtlr->RdEEPROM(0,0,expSN,20));
	if(0 == expSN[0] && 0 == expSN[1])
	{
		memset(expSN,0xBB,20);
	}
	EXPECT_TRUE(mpPwrCtlr->WrEEPROM(0,0,expSN,20));

	EXPECT_TRUE(mpPwrCtlr->RdEEPROM(0,0,actSN,20));
	EXPECT_EQ(expSN[0],actSN[0]);
	EXPECT_EQ(expSN[19],actSN[19]);
}

TEST_F(utMCUPwrCtrlr,CheckFWRev)
{
	uint8_t expFWRev[MCP_RESP_DLEN_GETFWREV];
	uint8_t actFWRev[MCP_RESP_DLEN_GETFWREV];

	memset(expFWRev,0,MCP_RESP_DLEN_GETFWREV);
	memset(actFWRev,0,MCP_RESP_DLEN_GETFWREV);

	EXPECT_TRUE(mpPwrCtlr->PowerOff(0));
	EXPECT_TRUE(mpPwrCtlr->GetFWRev(expFWRev,MCP_RESP_DLEN_GETFWREV));
	EXPECT_FALSE(0 == memcmp(expFWRev,actFWRev,MCP_RESP_DLEN_GETFWREV));
	EXPECT_TRUE(mpPwrCtlr->PowerOn(0));
	EXPECT_TRUE(mpPwrCtlr->GetFWRev(actFWRev,MCP_RESP_DLEN_GETFWREV));
	EXPECT_TRUE(0 == memcmp(expFWRev,actFWRev,MCP_RESP_DLEN_GETFWREV));
}
