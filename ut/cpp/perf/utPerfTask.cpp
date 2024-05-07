/*
 * file : utPerfTask.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "perf/PerfTask.h"

class utPerfTask:public testing::Test
{
public:
	virtual void SetUp()
	{
	}
};

TEST_F(utPerfTask,CheckGetOpOffset)
{
	PerfTask	task;
	InitPerfTask(&task);
	FillTaskWRandWrite(&task,0,1024*1024*1024,4096,0);

	uint64_t	offset=(uint64_t)-1;
	for(uint32_t idx=0;idx<100000;idx++)
	{
		GetOpOffset(&task,0,&offset);
		EXPECT_FALSE(offset==(uint64_t)-1);
	}
}
