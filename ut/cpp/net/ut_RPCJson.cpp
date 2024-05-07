/*
 * ut_RPCJson.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "net/RPCJson.h"

class ut_RPCJson : public testing::Test
{
};

TEST_F(ut_RPCJson,RPCJsonReq)
{
	RPCJsonReq j = RPCJsonReq(1,"test");
	EXPECT_EQ(j.GetID(),1);
	EXPECT_STREQ(j.GetMethod(),"test");
	EXPECT_STREQ(j.GetVersion(),RPCJSON_VERSION);
	j.AddParam<int>("param1",1);
	j.AddParam<std::string>("param2","val");
	const char* output = j();
	printf("output = %s\n",output);
	EXPECT_EQ(j.GetParam<int>("param1"),1);
	EXPECT_STREQ(j.GetParam<std::string>("param2").c_str(),"val");

	RPCJsonReq r = RPCJsonReq(output);
	EXPECT_EQ(r.GetID(),1);
	EXPECT_STREQ(r.GetMethod(),"test");
	EXPECT_STREQ(r.GetVersion(),RPCJSON_VERSION);
	EXPECT_EQ(r.GetParam<int>("param1"),1);
	EXPECT_STREQ(r.GetParam<std::string>("param2").c_str(),"val");
}

TEST_F(ut_RPCJson,RPCJsonRespWithSuccess)
{
	RPCJsonRespWithSuccess r = RPCJsonRespWithSuccess(1);
	EXPECT_EQ(r.GetID(),1);
	EXPECT_STREQ(r.GetVersion(),RPCJSON_VERSION);
	r.AddResult<int>("key1",1);
	r.AddResult<std::string>("key2","val");
	EXPECT_EQ(r.GetResult<int>("key1"),1);
	EXPECT_STREQ(r.GetResult<std::string>("key2").c_str(),"val");

	const char* output = r();
	printf("output = %s\n",output);

	RPCJsonRespWithSuccess j = RPCJsonRespWithSuccess(output);
	EXPECT_EQ(j.GetID(),1);
	EXPECT_STREQ(j.GetVersion(),RPCJSON_VERSION);
	EXPECT_EQ(j.GetResult<int>("key1"),1);
	EXPECT_STREQ(j.GetResult<std::string>("key2").c_str(),"val");
}

TEST_F(ut_RPCJson,RPCJsonRespWithFailure)
{
	RPCJsonRespWithFailure f = RPCJsonRespWithFailure(1,-32690,"Just for Test");
	EXPECT_EQ(f.GetID(),1);
	EXPECT_STREQ(f.GetVersion(),RPCJSON_VERSION);
	EXPECT_EQ(f.GetErrorCode(),-32690);
	EXPECT_STREQ(f.GetErrorMessage(),"Just for Test");
	f.AddErrData("key1",1);
	f.AddErrData("key2","val");
	EXPECT_EQ(f.GetErrData<int>("key1"),1);
	EXPECT_STREQ(f.GetErrData<std::string>("key2").c_str(),"val");

	const char* output = f();
	printf("output = %s\n",output);

	RPCJsonRespWithFailure j = RPCJsonRespWithFailure(output);
	EXPECT_EQ(j.GetID(),1);
	EXPECT_STREQ(j.GetVersion(),RPCJSON_VERSION);
	EXPECT_EQ(j.GetErrorCode(),-32690);
	EXPECT_STREQ(j.GetErrorMessage(),"Just for Test");
	EXPECT_EQ(j.GetErrData<int>("key1"),1);
	EXPECT_STREQ(j.GetErrData<std::string>("key2").c_str(),"val");
}

TEST_F(ut_RPCJson,RPCJsonErrParseError)
{
	RPCJsonErrParseError e = RPCJsonErrParseError(1);
	EXPECT_EQ(e.GetID(),1);
	EXPECT_STREQ(e.GetVersion(),RPCJSON_VERSION);
	EXPECT_EQ(e.GetErrorCode(),RPCJSON_EC_PARSE_ERROR);
	EXPECT_STREQ(e.GetErrorMessage(),RPCJSON_EM_PARSE_ERROR);
}

TEST_F(ut_RPCJson,RPCJsonErrInvalidReq)
{
	RPCJsonErrInvalidReq e = RPCJsonErrInvalidReq(1);
	EXPECT_EQ(e.GetID(),1);
	EXPECT_STREQ(e.GetVersion(),RPCJSON_VERSION);
	EXPECT_EQ(e.GetErrorCode(),RPCJSON_EC_INVALID_REQUEST);
	EXPECT_STREQ(e.GetErrorMessage(),RPCJSON_EM_INVALID_REQUEST);
}

TEST_F(ut_RPCJson,RPCJsonErrMethodNotFound)
{
	RPCJsonErrMethodNotFound e = RPCJsonErrMethodNotFound(1);
	EXPECT_EQ(e.GetID(),1);
	EXPECT_STREQ(e.GetVersion(),RPCJSON_VERSION);
	EXPECT_EQ(e.GetErrorCode(),RPCJSON_EC_METHOD_NOT_FOUND);
	EXPECT_STREQ(e.GetErrorMessage(),RPCJSON_EM_METHOD_NOT_FOUND);
}

TEST_F(ut_RPCJson,RPCJsonErrInvalidParams)
{
	RPCJsonErrInvalidParams e = RPCJsonErrInvalidParams(1);
	EXPECT_EQ(e.GetID(),1);
	EXPECT_STREQ(e.GetVersion(),RPCJSON_VERSION);
	EXPECT_EQ(e.GetErrorCode(),RPCJSON_EC_INVALID_PARAMS);
	EXPECT_STREQ(e.GetErrorMessage(),RPCJSON_EM_INVALID_PARAMS);
}

TEST_F(ut_RPCJson,RPCJsonErrInternalError)
{
	RPCJsonErrInternalError e = RPCJsonErrInternalError(1);
	EXPECT_EQ(e.GetID(),1);
	EXPECT_STREQ(e.GetVersion(),RPCJSON_VERSION);
	EXPECT_EQ(e.GetErrorCode(),RPCJSON_EC_INTERNAL_ERROR);
	EXPECT_STREQ(e.GetErrorMessage(),RPCJSON_EM_INTERNAL_ERROR);
}

TEST_F(ut_RPCJson,RPCJsonWriteJson)
{
	RPCJsonReq j = RPCJsonReq(1,"CloseService");
	char expOutput[4096];
	strcpy(expOutput,j());

	printf(expOutput);

	for(int idx=0;idx<100;idx++)
	{
		RPCJsonReq r = RPCJsonReq(1,"CloseService");
		EXPECT_STREQ(expOutput,r());
	}
}
