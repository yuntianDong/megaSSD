/*
 * file: utTaskReport.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "test/TaskReport.h"
#include "Logging.h"

class utTaskReport : public testing::Test
{
public:
	stRsltDesc*		mpRsltDesc;

	virtual void SetUp()
	{
		mpRsltDesc	= (stRsltDesc*)malloc(sizeof(stRsltDesc));
		memset(mpRsltDesc,0,sizeof(stRsltDesc));
	}

	virtual void TearDown()
	{
		if(NULL != mpRsltDesc)
		{
			free(mpRsltDesc);
			mpRsltDesc = NULL;
		}
	}
};

TEST_F(utTaskReport,CheckDump)
{
	TaskResult rslt1(mpRsltDesc);
	TaskResult rslt2(mpRsltDesc);
	TaskResult rslt3(mpRsltDesc);

	rslt1.SetTargetPhyAddr("0000:00:0e.0");
	rslt1.SetRsltIndex(88);

	TaskReport	tr;

	tr.Record("Case1",&rslt1);
	rslt2.SetTestResult(3);
	tr.Record("Case2",&rslt2);
	rslt3.SetTestResult(5);
	tr.Record("Case3",&rslt3);

	tr.Report();
	EXPECT_TRUE(tr.Report("Report.log"));
}
