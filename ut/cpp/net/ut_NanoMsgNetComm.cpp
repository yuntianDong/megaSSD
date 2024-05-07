/*
 * ut_NanoMsgNetComm.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "net/NanoMsgNetComm.h"

#define TCP_RPC		"tcp://127.0.0.1:5555"

class ut_NanoMsgNetComm : public testing::Test
{

};

TEST_F(ut_NanoMsgNetComm,NanoMsgRPC)
{
	NanoMsgRPCServer*	s = new NanoMsgRPCServer(TCP_RPC);
	NanoMsgRPCClient*   c = new NanoMsgRPCClient(TCP_RPC);

	s->SetOptSendTimeOut(8000);
	s->SetOptRecvTimeOut(8000);
	c->SetOptSendTimeOut(8000);
	c->SetOptRecvTimeOut(8000);

	char RequMsg[]	= "Request";
	char RespMsg[]	= "Response";

	char RecvMsg[10]= "";

	EXPECT_EQ(strlen(RequMsg)+1,c->SendDatum((void*)RequMsg,strlen(RequMsg)+1));
	EXPECT_EQ(strlen(RequMsg)+1,s->RecvDatum((void*)RecvMsg,strlen(RequMsg)+1));
	EXPECT_STREQ(RequMsg,RecvMsg);

	EXPECT_EQ(strlen(RespMsg)+1,s->SendDatum((void*)RespMsg,strlen(RespMsg)+1));
	EXPECT_EQ(strlen(RespMsg)+1,c->RecvDatum((void*)RecvMsg,strlen(RespMsg)+1));
	EXPECT_STREQ(RespMsg,RecvMsg);

	delete s;
	delete c;
}

TEST_F(ut_NanoMsgNetComm,NanoMsgRPCMultiClient)
{
	NanoMsgRPCServer*	s = new NanoMsgRPCServer(TCP_RPC);
	s->SetOptSendTimeOut(8000);
	s->SetOptRecvTimeOut(8000);

	char RequMsg[]	= "Request";
	char RespMsg[]	= "Response";

	char RecvMsg[10]= "";

	NanoMsgRPCClient* cls[3];
	for(int idx=0;idx<3;idx++)
	{
		cls[idx]	= new NanoMsgRPCClient(TCP_RPC);
		cls[idx]->SetOptSendTimeOut(8000);
		cls[idx]->SetOptRecvTimeOut(8000);

		EXPECT_EQ(strlen(RequMsg)+1,cls[idx]->SendDatum((void*)RequMsg,strlen(RequMsg)+1));
		EXPECT_EQ(strlen(RequMsg)+1,s->RecvDatum((void*)RecvMsg,strlen(RequMsg)+1));
		EXPECT_STREQ(RequMsg,RecvMsg);

		EXPECT_EQ(strlen(RespMsg)+1,s->SendDatum((void*)RespMsg,strlen(RespMsg)+1));
		EXPECT_EQ(strlen(RespMsg)+1,cls[idx]->RecvDatum((void*)RecvMsg,strlen(RespMsg)+1));
		EXPECT_STREQ(RespMsg,RecvMsg);

		delete cls[idx];
	}

	delete s;
}
