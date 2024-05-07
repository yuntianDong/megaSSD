/*
 * file : ut_JsonRPC2TaskMngr.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "net/JsonRPC2TaskMngr.h"

class ut_JsonRPC2TaskMngr : public testing::Test
{
};

TEST_F(ut_JsonRPC2TaskMngr,FillJsonReqWithTaskDesc)
{
	RPCJsonReq req(1,"Test");
	TaskDesc   task;
	task.SetTaskIndex(88);
	task.SetTaskName("TestTask1");
	task.SetTaskCmdline("Python3 TestScript.py");
	task.SetLoopCnt(100);
	task.SetTimeout(8000);
	task.AddCondition(CK_SN,"123456",false);

	FillJsonReqWithTaskDesc(req,task);

	EXPECT_EQ(req.GetParam<uint16_t>("index"),88);
	EXPECT_STREQ(req.GetParam<std::string>("name").c_str(),"TestTask1");
	EXPECT_STREQ(req.GetParam<std::string>("cmdline").c_str(),"Python3 TestScript.py");
	EXPECT_EQ(req.GetParam<uint32_t>("loopCount"),100);
	EXPECT_EQ(req.GetParam<uint32_t>("execTimeout"),8000);
	EXPECT_STREQ(req.GetParam<std::string>("reqSN").c_str(),"123456,");
}

TEST_F(ut_JsonRPC2TaskMngr,FillJsonReqWithTaskRslt)
{
	RPCJsonReq req(1,"Test");
	TaskResult rslt;
	rslt.SetRsltIndex(99);
	rslt.SetSlotID(8);
	rslt.SetTargetPhyAddr("0000:11:22:33");
	rslt.SetTestExecTime(8000);
	rslt.SetTestResult(-1);

	FillJsonReqWithTaskRslt(req,rslt);

	EXPECT_EQ(req.GetParam<uint16_t>("index"),99);
	EXPECT_EQ(req.GetParam<int>("result"),-1);
	EXPECT_STREQ(req.GetParam<std::string>("tagtAddr").c_str(),"0000:11:22:33");
	EXPECT_EQ(req.GetParam<uint8_t>("slotID"),8);
	EXPECT_EQ(req.GetParam<uint32_t>("execTime"),8000);
}

TEST_F(ut_JsonRPC2TaskMngr,GetTaskDescFromJsonReq)
{
	RPCJsonReq req(1,"Test");
	req.AddParam<uint16_t>("index",88);
	req.AddParam<std::string>("name","TestTask1");
	req.AddParam<std::string>("cmdline","Python3 TestScript.py");
	req.AddParam<uint32_t>("loopCount",100);
	req.AddParam<uint32_t>("execTimeout",8000);
	req.AddParam<std::string>("reqSN","123456,");
	req.AddParam<std::string>("excSN","");
	req.AddParam<std::string>("reqMN","");
	req.AddParam<std::string>("excMN","");
	req.AddParam<std::string>("reqFR","");
	req.AddParam<std::string>("excFR","");
	req.AddParam<std::string>("reqGR","");
	req.AddParam<std::string>("excGR","");
	req.AddParam<std::string>("reqGB","");
	req.AddParam<std::string>("excGB","");
	req.AddParam<std::string>("reqSS","");
	req.AddParam<std::string>("excSS","");

	TaskDesc   task;
	char * reqSN = NULL;
	GetTaskDescFromJsonReq(task,req);
	task.GetCondOfTaskCond(CK_SN,false,0,reqSN);

	EXPECT_EQ(task.GetTaskIndex(),88);
	EXPECT_STREQ(task.GetTaskName(),"TestTask1");
	EXPECT_STREQ(task.GetCmdLine(),"Python3 TestScript.py");
	EXPECT_EQ(task.GetLoopCount(),100);
	EXPECT_EQ(task.GetTimeOut(),8000);
	EXPECT_STREQ(reqSN,"123456");
}

TEST_F(ut_JsonRPC2TaskMngr,GetTaskRsltFromJsonReq)
{
	RPCJsonReq req(1,"Test");

	req.AddParam<uint16_t>("index",88);
	req.AddParam<int>("result",-1);
	req.AddParam<std::string>("tagtAddr","0000:11:22:33");
	req.AddParam<uint8_t>("slotID",8);
	req.AddParam<uint32_t>("execTime",8000);
	req.AddParam<bool>("valid",true);

	TaskResult rslt;
	GetTaskRsltFromJsonReq(rslt,req);

	EXPECT_EQ(rslt.GetRsltIndex(),88);
	EXPECT_EQ(rslt.GetTestResult(),-1);
	EXPECT_STREQ(rslt.GetTargetPhyAddr(),"0000:11:22:33");
	EXPECT_EQ(rslt.GetSlotID(),8);
	EXPECT_EQ(rslt.GetTestExecTime(),8000);
}

TEST_F(ut_JsonRPC2TaskMngr,FillJsonRespWithTaskDesc)
{
	RPCJsonRespWithSuccess resp(1);
	TaskDesc   task;
	task.SetTaskIndex(88);
	task.SetTaskName("TestTask1");
	task.SetTaskCmdline("Python3 TestScript.py");
	task.SetLoopCnt(100);
	task.SetTimeout(8000);
	task.AddCondition(CK_SN,"123456",false);

	FillJsonRespWithTaskDesc(resp,task);

	EXPECT_EQ(resp.GetResult<uint16_t>("index"),88);
	EXPECT_STREQ(resp.GetResult<std::string>("name").c_str(),"TestTask1");
	EXPECT_STREQ(resp.GetResult<std::string>("cmdline").c_str(),"Python3 TestScript.py");
	EXPECT_EQ(resp.GetResult<uint32_t>("loopCount"),100);
	EXPECT_EQ(resp.GetResult<uint32_t>("execTimeout"),8000);
	EXPECT_STREQ(resp.GetResult<std::string>("reqSN").c_str(),"123456,");
}

TEST_F(ut_JsonRPC2TaskMngr,FillJsonRespWithTaskRslt)
{
	RPCJsonRespWithSuccess resp(1);
	TaskResult rslt;
	rslt.SetRsltIndex(99);
	rslt.SetSlotID(8);
	rslt.SetTargetPhyAddr("0000:11:22:33");
	rslt.SetTestExecTime(8000);
	rslt.SetTestResult(-1);

	FillJsonRespWithTaskRslt(resp,rslt);

	EXPECT_EQ(resp.GetResult<uint16_t>("index"),99);
	EXPECT_EQ(resp.GetResult<int>("result"),-1);
	EXPECT_STREQ(resp.GetResult<std::string>("tagtAddr").c_str(),"0000:11:22:33");
	EXPECT_EQ(resp.GetResult<uint8_t>("slotID"),8);
	EXPECT_EQ(resp.GetResult<uint32_t>("execTime"),8000);
}

TEST_F(ut_JsonRPC2TaskMngr,GetTaskDescFromJsonResp)
{
	RPCJsonRespWithSuccess resp(1);
	resp.AddResult<uint16_t>("index",88);
	resp.AddResult<std::string>("name","TestTask1");
	resp.AddResult<std::string>("cmdline","Python3 TestScript.py");
	resp.AddResult<uint32_t>("loopCount",100);
	resp.AddResult<uint32_t>("execTimeout",8000);
	resp.AddResult<std::string>("reqSN","123456,");
	resp.AddResult<std::string>("excSN","");
	resp.AddResult<std::string>("reqMN","");
	resp.AddResult<std::string>("excMN","");
	resp.AddResult<std::string>("reqFR","");
	resp.AddResult<std::string>("excFR","");
	resp.AddResult<std::string>("reqGR","");
	resp.AddResult<std::string>("excGR","");
	resp.AddResult<std::string>("reqGB","");
	resp.AddResult<std::string>("excGB","");
	resp.AddResult<std::string>("reqSS","");
	resp.AddResult<std::string>("excSS","");

	TaskDesc   task;
	char * reqSN = NULL;
	GetTaskDescFromJsonResp(task,resp);
	task.GetCondOfTaskCond(CK_SN,false,0,reqSN);

	EXPECT_EQ(task.GetTaskIndex(),88);
	EXPECT_STREQ(task.GetTaskName(),"TestTask1");
	EXPECT_STREQ(task.GetCmdLine(),"Python3 TestScript.py");
	EXPECT_EQ(task.GetLoopCount(),100);
	EXPECT_EQ(task.GetTimeOut(),8000);
	EXPECT_STREQ(reqSN,"123456");
}

TEST_F(ut_JsonRPC2TaskMngr,GetTaskRsltFromJsonResp)
{
	RPCJsonRespWithSuccess resp(1);

	resp.AddResult<uint16_t>("index",88);
	resp.AddResult<int>("result",-1);
	resp.AddResult<std::string>("tagtAddr","0000:11:22:33");
	resp.AddResult<uint8_t>("slotID",8);
	resp.AddResult<uint32_t>("execTime",8000);
	resp.AddResult<bool>("valid",true);

	TaskResult rslt;
	GetTaskRsltFromJsonResp(rslt,resp);

	EXPECT_EQ(rslt.GetRsltIndex(),88);
	EXPECT_EQ(rslt.GetTestResult(),-1);
	EXPECT_STREQ(rslt.GetTargetPhyAddr(),"0000:11:22:33");
	EXPECT_EQ(rslt.GetSlotID(),8);
	EXPECT_EQ(rslt.GetTestExecTime(),8000);
}
