/**
 * file: utContext.cpp
*/
#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "context/Context.h"

class utContext : public testing::Test
{    
};

TEST_F(utContext, test_context)
{
    ParentContext *ctrlr_context = new ParentContext();

    //Test ParenetContext
    ctrlr_context->AddLbaMap(0,512,0);
    ctrlr_context->AddLbaMap(1,4096,0);
    ctrlr_context->AddLbaMap(2,4096,8);
    ctrlr_context->AddLbaMap(3,4096,64);

    ctrlr_context->DumpLbaMap();

    ctrlr_context->AddNsMap(1,0,0x100,0);
    ctrlr_context->AddNsMap(2,0x100,0x200,1);
    ctrlr_context->AddNsMap(3,0x300,0x300,2);
    ctrlr_context->AddNsMap(4,0x600,0x400,3);
    
    ctrlr_context->DumpNsMap();

    ctrlr_context->SetTotalNvmSize(0x600 + 0x400);

    EXPECT_EQ(ctrlr_context->GetTotalNvmSize(), 0x600 + 0x400);
    EXPECT_EQ(ctrlr_context->GetSectorSize(0),512);
    EXPECT_EQ(ctrlr_context->GetSectorSize(1),4096);
    EXPECT_EQ(ctrlr_context->GetSectorSize(2),4096);
    EXPECT_EQ(ctrlr_context->GetSectorSize(3),4096);
    EXPECT_EQ(ctrlr_context->GetSectorSize(4),0);
    EXPECT_EQ(ctrlr_context->GetMetaSize(0),0);
    EXPECT_EQ(ctrlr_context->GetMetaSize(1),0);
    EXPECT_EQ(ctrlr_context->GetMetaSize(2),8);
    EXPECT_EQ(ctrlr_context->GetMetaSize(3),64);
    EXPECT_EQ(ctrlr_context->GetMetaSize(4),0);
    EXPECT_NE(ctrlr_context->GetParentParameter(),nullptr);

    ChildContext* nsid_1_context = ctrlr_context->GetChildContext(1);
    nsid_1_context->SetCmdTO(8000);
    nsid_1_context->SetNsPIGuardSize(8);
    nsid_1_context->SetNsPISTagSize(8);
    EXPECT_EQ(nsid_1_context->GetNsID(),1);
    EXPECT_EQ(nsid_1_context->GetNsDataSize(),512);
    EXPECT_EQ(nsid_1_context->GetNsMetaSize(),0);
    EXPECT_EQ(nsid_1_context->GetNsCap(),0x100);
    EXPECT_EQ(nsid_1_context->GetCmdTO(),8000);
    EXPECT_EQ(nsid_1_context->GetNsPIGuardSize(),8);
    EXPECT_EQ(nsid_1_context->GetNsPISTagSize(),8);
    EXPECT_NE(nsid_1_context->GetParent(),nullptr);
    EXPECT_NE(nsid_1_context->GetChildParameter(),nullptr);

    ChildContext* nsid_2_context = ctrlr_context->GetChildContext(2);
    nsid_2_context->SetCmdTO(8000);
    nsid_2_context->SetNsPIGuardSize(8);
    nsid_2_context->SetNsPISTagSize(8);
    EXPECT_EQ(nsid_2_context->GetNsID(),2);
    EXPECT_EQ(nsid_2_context->GetNsDataSize(),4096);
    EXPECT_EQ(nsid_2_context->GetNsMetaSize(),0);
    EXPECT_EQ(nsid_2_context->GetNsCap(),0x200);
    EXPECT_EQ(nsid_2_context->GetCmdTO(),8000);
    EXPECT_EQ(nsid_2_context->GetNsPIGuardSize(),8);
    EXPECT_EQ(nsid_2_context->GetNsPISTagSize(),8);
    EXPECT_NE(nsid_2_context->GetParent(),nullptr);
    EXPECT_NE(nsid_2_context->GetChildParameter(),nullptr);

    ChildContext* nsid_3_context = ctrlr_context->GetChildContext(3);
    nsid_3_context->SetCmdTO(8000);
    nsid_3_context->SetNsPIGuardSize(8);
    nsid_3_context->SetNsPISTagSize(8);
    EXPECT_EQ(nsid_3_context->GetNsID(),3);
    EXPECT_EQ(nsid_3_context->GetNsDataSize(),4096);
    EXPECT_EQ(nsid_3_context->GetNsMetaSize(),8);
    EXPECT_EQ(nsid_3_context->GetNsCap(),0x300);
    EXPECT_EQ(nsid_3_context->GetCmdTO(),8000);
    EXPECT_EQ(nsid_3_context->GetNsPIGuardSize(),8);
    EXPECT_EQ(nsid_3_context->GetNsPISTagSize(),8);
    EXPECT_NE(nsid_3_context->GetParent(),nullptr);
    EXPECT_NE(nsid_3_context->GetChildParameter(),nullptr);

    ChildContext* nsid_4_context = ctrlr_context->GetChildContext(4);
    nsid_4_context->SetCmdTO(8000);
    nsid_4_context->SetNsPIGuardSize(8);
    nsid_4_context->SetNsPISTagSize(8);
    EXPECT_EQ(nsid_4_context->GetNsID(),4);
    EXPECT_EQ(nsid_4_context->GetNsDataSize(),4096);
    EXPECT_EQ(nsid_4_context->GetNsMetaSize(),64);
    EXPECT_EQ(nsid_4_context->GetNsCap(),0x400);
    EXPECT_EQ(nsid_4_context->GetCmdTO(),8000);
    EXPECT_EQ(nsid_4_context->GetNsPIGuardSize(),8);
    EXPECT_EQ(nsid_4_context->GetNsPISTagSize(),8);
    EXPECT_NE(nsid_4_context->GetParent(),nullptr);
    EXPECT_NE(nsid_4_context->GetChildParameter(),nullptr);

    EXPECT_TRUE(ctrlr_context->RemoveChildContext(1) == true);
    EXPECT_NE(ctrlr_context->GetChildren(),nullptr);
    ctrlr_context->ReleaseAllChildContext();

    ChildContext* invalid_test = new ChildContext(nullptr);
    EXPECT_EQ(invalid_test->GetParent(),nullptr);

    del_obj(ctrlr_context)
}