/*
 * file: utTaskMngr.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "test/TaskMngr.h"
#include "Logging.h"

class utTaskMngr : public testing::Test
{
public:
	stTaskMngr*		mpTaskMngr;
	TaskMngr*		mpMngr;

	virtual void SetUp()
	{
		mpTaskMngr	= (stTaskMngr*)malloc(sizeof(stTaskMngr));
		memset(mpTaskMngr,0,sizeof(stTaskMngr));
		mpMngr		= new TaskMngr(mpTaskMngr);
	}

	virtual void TearDown()
	{
		if(NULL != mpTaskMngr)
		{
			free(mpTaskMngr);
			mpTaskMngr = NULL;
		}

		if(NULL != mpMngr)
		{
			delete mpMngr;
			mpMngr = NULL;
		}
	}
};

TEST_F(utTaskMngr,CheckBasic)
{
	EXPECT_TRUE(mpMngr->IsEmpty());
	EXPECT_FALSE(mpMngr->IsFull());
}

TEST_F(utTaskMngr,CheckGetTaskDesc)
{
	TaskDesc*	pdesc1	= NULL;
	TaskDesc*	pdesc2	= NULL;
	TaskDesc*	pdesc3	= NULL;
	TaskDesc*	pdesc4	= NULL;

	EXPECT_TRUE(mpMngr->GetTaskDescForCfg(pdesc1));
	EXPECT_EQ(pdesc1->GetTaskIndex(),0);
	pdesc1->SetTaskName("Task1");
	pdesc1->SetTaskCmdline("CommandLine1");

	EXPECT_FALSE(mpMngr->IsEmpty());

	EXPECT_TRUE(mpMngr->GetTaskDescForCfg(pdesc2));
	EXPECT_EQ(pdesc2->GetTaskIndex(),1);
	pdesc2->SetTaskName("Task2");
	pdesc2->SetTaskCmdline("CommandLine2");

	EXPECT_TRUE(mpMngr->GetTaskDescForExe(pdesc3));
	EXPECT_EQ(pdesc3->GetTaskIndex(),0);
	EXPECT_STREQ(pdesc3->GetTaskName(),pdesc1->GetTaskName());
	EXPECT_STREQ(pdesc3->GetCmdLine(),pdesc1->GetCmdLine());

	EXPECT_TRUE(mpMngr->GetTaskDescForExe(pdesc4));
	EXPECT_EQ(pdesc4->GetTaskIndex(),1);
	EXPECT_STREQ(pdesc4->GetTaskName(),pdesc2->GetTaskName());
	EXPECT_STREQ(pdesc4->GetCmdLine(),pdesc2->GetCmdLine());

	EXPECT_TRUE(mpMngr->IsEmpty());

	delete pdesc1;pdesc1 = NULL;
	delete pdesc2;pdesc2 = NULL;
	delete pdesc3;pdesc3 = NULL;
	delete pdesc4;pdesc4 = NULL;
}

TEST_F(utTaskMngr,CheckTestResult)
{
	TaskDesc*	pdesc1	= NULL;
	TaskDesc*	pdesc2	= NULL;
	TaskDesc*	pdesc3	= NULL;
	TaskDesc*	pdesc4	= NULL;

	TaskResult*	prslt1	= NULL;
	TaskResult*	prslt2	= NULL;
	TaskReport	report;

	EXPECT_TRUE(mpMngr->GetTaskDescForCfg(pdesc1));
	EXPECT_EQ(pdesc1->GetTaskIndex(),0);
	pdesc1->SetTaskName("Task1");
	EXPECT_STREQ(pdesc1->GetTaskName(),"Task1");
	pdesc1->SetTaskCmdline("CommandLine1");
	EXPECT_STREQ(pdesc1->GetCmdLine(),"CommandLine1");

	EXPECT_FALSE(mpMngr->IsEmpty());

	EXPECT_TRUE(mpMngr->GetTaskDescForCfg(pdesc2));
	EXPECT_EQ(pdesc2->GetTaskIndex(),1);
	pdesc2->SetTaskName("Task2");
	pdesc2->SetTaskCmdline("CommandLine2");

	EXPECT_TRUE(mpMngr->GetTaskDescForExe(pdesc3));
	EXPECT_EQ(pdesc3->GetTaskIndex(),0);
	EXPECT_STREQ(pdesc3->GetTaskName(),pdesc1->GetTaskName());
	EXPECT_STREQ(pdesc3->GetCmdLine(),pdesc1->GetCmdLine());

	EXPECT_TRUE(mpMngr->GetTaskDescForExe(pdesc4));
	EXPECT_EQ(pdesc4->GetTaskIndex(),1);
	EXPECT_STREQ(pdesc4->GetTaskName(),pdesc2->GetTaskName());
	EXPECT_STREQ(pdesc4->GetCmdLine(),pdesc2->GetCmdLine());

	EXPECT_TRUE(mpMngr->IsEmpty());

	EXPECT_TRUE(mpMngr->FindTaskResultByIndex(0,prslt1));
	EXPECT_EQ(prslt1->GetRsltIndex(),0);
	prslt1->SetTestResult(3);
	prslt1->SetTargetPhyAddr("Target1");

	EXPECT_TRUE(mpMngr->FindTaskResultByIndex(1,prslt2));
	EXPECT_EQ(prslt2->GetRsltIndex(),1);
	prslt2->SetTestResult(5);
	prslt2->SetTargetPhyAddr("Target2");

	EXPECT_TRUE(mpMngr->MakeTestReport(report));
	report.Report();

	delete pdesc1;pdesc1 = NULL;
	delete pdesc2;pdesc2 = NULL;
	delete pdesc3;pdesc3 = NULL;
	delete pdesc4;pdesc4 = NULL;

	delete prslt1;prslt1 = NULL;
	delete prslt2;prslt2 = NULL;
}

TEST_F(utTaskMngr,CheckRecoverFlow)
{
	TaskDesc*	pdesc1	= NULL;
	TaskDesc*	pdesc2	= NULL;
	TaskDesc*	pdesc3	= NULL;
	TaskDesc*	pdesc4	= NULL;

	EXPECT_TRUE(mpMngr->GetTaskDescForCfg(pdesc1));
	EXPECT_EQ(pdesc1->GetTaskIndex(),0);
	pdesc1->SetTaskName("Task1");
	EXPECT_STREQ(pdesc1->GetTaskName(),"Task1");
	pdesc1->SetTaskCmdline("CommandLine1");
	EXPECT_STREQ(pdesc1->GetCmdLine(),"CommandLine1");

	EXPECT_FALSE(mpMngr->IsEmpty());

	EXPECT_TRUE(mpMngr->GetTaskDescForCfg(pdesc2));
	EXPECT_EQ(pdesc2->GetTaskIndex(),1);
	pdesc2->SetTaskName("Task2");
	pdesc2->SetTaskCmdline("CommandLine2");

	EXPECT_TRUE(mpMngr->GetTaskDescForExe(pdesc3));
	EXPECT_EQ(pdesc3->GetTaskIndex(),0);
	EXPECT_STREQ(pdesc3->GetTaskName(),pdesc1->GetTaskName());
	EXPECT_STREQ(pdesc3->GetCmdLine(),pdesc1->GetCmdLine());

	EXPECT_TRUE(mpMngr->GetTaskDescForExe(pdesc4));
	EXPECT_EQ(pdesc4->GetTaskIndex(),1);
	EXPECT_STREQ(pdesc4->GetTaskName(),pdesc2->GetTaskName());
	EXPECT_STREQ(pdesc4->GetCmdLine(),pdesc2->GetCmdLine());

	EXPECT_TRUE(mpMngr->IsEmpty());

	TaskMngr	mngr2(mpTaskMngr);
	TaskDesc*	pdesc5	= NULL;
	TaskDesc*	pdesc6	= NULL;
	TaskDesc*	pdesc7	= NULL;

	mngr2.Dump();

	EXPECT_TRUE(mngr2.GetTaskDescForExe(pdesc5));
	EXPECT_EQ(pdesc5->GetTaskIndex(),1);
	EXPECT_STREQ(pdesc5->GetTaskName(),pdesc2->GetTaskName());
	EXPECT_STREQ(pdesc5->GetCmdLine(),pdesc2->GetCmdLine());

	EXPECT_TRUE(mngr2.GetTaskDescForExe(pdesc6));
	EXPECT_EQ(pdesc6->GetTaskIndex(),0);
	EXPECT_STREQ(pdesc6->GetTaskName(),pdesc1->GetTaskName());
	EXPECT_STREQ(pdesc6->GetCmdLine(),pdesc1->GetCmdLine());

	EXPECT_FALSE(mngr2.GetTaskDescForExe(pdesc7));

	delete pdesc1;pdesc1 = NULL;
	delete pdesc2;pdesc2 = NULL;
	delete pdesc3;pdesc3 = NULL;
	delete pdesc4;pdesc4 = NULL;
	delete pdesc5;pdesc5 = NULL;
	delete pdesc6;pdesc6 = NULL;
}
