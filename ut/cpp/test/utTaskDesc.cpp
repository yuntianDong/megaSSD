/*
 * file : utTaskDesc.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "test/TaskDesc.h"
#include "Logging.h"

class utTaskDesc : public testing::Test
{
public:
	stTaskDesc*	mpDescData;
	TaskDesc*	mpDesc;

	virtual void SetUp()
	{
		mpDescData	= (stTaskDesc*)malloc(sizeof(stTaskDesc));
		memset(mpDescData,0,sizeof(stTaskDesc));
		mpDesc		= new TaskDesc(mpDescData);
	}

	virtual void TearDown()
	{
		if(NULL != mpDescData)
		{
			free(mpDescData);
			mpDescData = NULL;
		}

		if(NULL != mpDesc)
		{
			delete mpDesc;
			mpDesc = NULL;
		}
	}
};

TEST_F(utTaskDesc,CheckBasic)
{
	mpDesc->SetTaskIndex(0x1234);
	EXPECT_EQ(mpDesc->GetTaskIndex(),0x1234);

	mpDesc->SetTaskName("Test");
	EXPECT_STREQ(mpDesc->GetTaskName(),"Test");

	mpDesc->SetTaskCmdline("CommandLine");
	EXPECT_STREQ(mpDesc->GetCmdLine(),"CommandLine");

	mpDesc->SetTimeout(1000);
	EXPECT_EQ(mpDesc->GetTimeOut(),1000);

	mpDesc->SetLoopCnt(8);
	EXPECT_EQ(mpDesc->GetLoopCount(),8);
}

TEST_F(utTaskDesc,CheckAddCondition)
{
	EXPECT_TRUE(mpDesc->AddCondition(CK_SN,"SN123456"));
	EXPECT_TRUE(mpDesc->AddCondition(CK_SN,"SN654321"));
	EXPECT_TRUE(mpDesc->AddCondition(CK_SN,"SN135790",true));
	EXPECT_TRUE(mpDesc->AddCondition(CK_SN,"SN098765",true));

	EXPECT_TRUE(mpDesc->AddCondition(CK_MN,"MN123456"));
	EXPECT_TRUE(mpDesc->AddCondition(CK_MN,"MN654321"));
	EXPECT_TRUE(mpDesc->AddCondition(CK_MN,"MN135790",true));
	EXPECT_TRUE(mpDesc->AddCondition(CK_MN,"MN098765",true));

	EXPECT_TRUE(mpDesc->AddCondition(CK_FR,"FR123456"));
	EXPECT_TRUE(mpDesc->AddCondition(CK_FR,"FR654321"));
	EXPECT_TRUE(mpDesc->AddCondition(CK_FR,"FR135790",true));
	EXPECT_TRUE(mpDesc->AddCondition(CK_FR,"FR098765",true));

	EXPECT_TRUE(mpDesc->AddCondition(CK_GR,"GR123456"));
	EXPECT_TRUE(mpDesc->AddCondition(CK_GR,"GR654321"));
	EXPECT_TRUE(mpDesc->AddCondition(CK_GR,"GR135790",true));
	EXPECT_TRUE(mpDesc->AddCondition(CK_GR,"GR098765",true));

	EXPECT_TRUE(mpDesc->AddCondition(CK_GB,120));
	EXPECT_TRUE(mpDesc->AddCondition(CK_GB,240));
	EXPECT_TRUE(mpDesc->AddCondition(CK_GB,480,true));
	EXPECT_TRUE(mpDesc->AddCondition(CK_GB,960,true));

	EXPECT_TRUE(mpDesc->AddCondition(CK_SS,512));
	EXPECT_TRUE(mpDesc->AddCondition(CK_SS,4096));
	EXPECT_TRUE(mpDesc->AddCondition(CK_SS,512,true));
	EXPECT_TRUE(mpDesc->AddCondition(CK_SS,4096,true));

	mpDesc->Dump();
}

TEST_F(utTaskDesc,CheckIsMatchedDut)
{
	stDutInfo	info;
	strcpy(info.dutSN,"SN123456");
	strcpy(info.dutMN,"MN123456");
	strcpy(info.dutFWRev,"FR123456");
	strcpy(info.dutGITRev,"GR123456");
	info.dutCapGB	= 8;
	info.dutSector	= 512;

	DutInfo		di(&info);

	EXPECT_TRUE(mpDesc->IsMatchedDut(&di));

	mpDesc->Reset();
	mpDesc->AddCondition(CK_SN,"SN123457");
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));
	mpDesc->Reset();
	mpDesc->AddCondition(CK_SN,"SN123457",true);
	EXPECT_TRUE(mpDesc->IsMatchedDut(&di));
	mpDesc->AddCondition(CK_SN,"SN123456",true);
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));

	mpDesc->Reset();
	mpDesc->AddCondition(CK_MN,"MN123457");
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));
	mpDesc->Reset();
	mpDesc->AddCondition(CK_MN,"MN123457",true);
	EXPECT_TRUE(mpDesc->IsMatchedDut(&di));
	mpDesc->AddCondition(CK_MN,"MN123456",true);
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));

	mpDesc->Reset();
	mpDesc->AddCondition(CK_FR,"FR123457");
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));
	mpDesc->Reset();
	mpDesc->AddCondition(CK_FR,"FR123457",true);
	EXPECT_TRUE(mpDesc->IsMatchedDut(&di));
	mpDesc->AddCondition(CK_FR,"FR123456",true);
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));

	mpDesc->Reset();
	mpDesc->AddCondition(CK_GR,"GR123457");
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));
	mpDesc->Reset();
	mpDesc->AddCondition(CK_GR,"GR123457",true);
	EXPECT_TRUE(mpDesc->IsMatchedDut(&di));
	mpDesc->AddCondition(CK_GR,"GR123456",true);
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));

	mpDesc->Reset();
	mpDesc->AddCondition(CK_GB,10);
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));
	mpDesc->Reset();
	mpDesc->AddCondition(CK_GB,10,true);
	EXPECT_TRUE(mpDesc->IsMatchedDut(&di));
	mpDesc->AddCondition(CK_GB,8,true);
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));

	mpDesc->Reset();
	mpDesc->AddCondition(CK_SS,4096);
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));
	mpDesc->Reset();
	mpDesc->AddCondition(CK_SS,4096,true);
	EXPECT_TRUE(mpDesc->IsMatchedDut(&di));
	mpDesc->AddCondition(CK_SS,512,true);
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));

	mpDesc->Reset();
	mpDesc->AddCondition(CK_SN,"SN123456");
	mpDesc->AddCondition(CK_SS,4096);
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));

	mpDesc->Reset();
	mpDesc->AddCondition(CK_SN,"SN123456",true);
	mpDesc->AddCondition(CK_SS,512);
	EXPECT_FALSE(mpDesc->IsMatchedDut(&di));

	mpDesc->Reset();
	mpDesc->AddCondition(CK_SN,"SN123457",true);
	mpDesc->AddCondition(CK_SS,512);
	EXPECT_TRUE(mpDesc->IsMatchedDut(&di));

	mpDesc->Reset();
	mpDesc->AddCondition(CK_SN,"SN123456");
	mpDesc->AddCondition(CK_SS,512);
	EXPECT_TRUE(mpDesc->IsMatchedDut(&di));
}
