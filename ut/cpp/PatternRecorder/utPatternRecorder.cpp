/**
 * file: utPatternRecorder.cpp
*/
#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "IPatternMngr.h"
#define DEFAULT_NS_ID       1

class utPatternRecorder : public testing::Test
{
};

TEST_F(utPatternRecorder, test_IPatternMngr)
{
    IHostBufMngr* buf_mngr = new IHostBufMngr();
    ParentContext* ctrlr_context = new ParentContext();

    //config ctrlr_context
    ctrlr_context->AddLbaMap(0,512,0);
    ctrlr_context->AddLbaMap(1,4096,0);
    ctrlr_context->AddLbaMap(2,4096,8);
    ctrlr_context->AddLbaMap(3,4096,64);

    ctrlr_context->DumpLbaMap();

    ctrlr_context->AddNsMap(1,0,0x1000,0);
    
    ctrlr_context->DumpNsMap();

    ctrlr_context->SetTotalNvmSize(0x1000);
    
    //start test IPatternMngr
    IPatternMngr* p = new IPatternMngr(PATREC_BITMAP,false,buf_mngr,ctrlr_context);

    buf_mngr->SetSectorSize(512);
    Buffers* test_write_buf = buf_mngr->GetIOWrBuf(DEFAULT_NS_ID,0,"nvme");
    test_write_buf->FillSector(0,1,p->GetPatternByIndex(4));
    test_write_buf->FillSector(1,1,p->GetPatternByIndex(3));
    test_write_buf->FillSector(2,1,p->GetPatternByIndex(2));
    test_write_buf->FillSector(3,1,p->GetPatternByIndex(1)); 

    EXPECT_TRUE(p->UptPatRecFromIOWrBuf(test_write_buf,0,1,0,4));
    p->DumpBitmapBuf(1,0,4);
    EXPECT_TRUE(p->CompareWithIORdBuf(test_write_buf,0,1,0,4,"nvme"));
    EXPECT_TRUE(p->ReleaseAllPattern());
    p->DumpBitmapBuf(1,0,4);

    EXPECT_EQ(0xFFFFFFFF,p->GetPatternByIndex(0x1));
    EXPECT_EQ(0x0000FFFF,p->GetPatternByIndex(0x2));
    EXPECT_EQ(0xF0F00F0F,p->GetPatternByIndex(0x3));
    EXPECT_EQ(0xFF00FF00,p->GetPatternByIndex(0x4));
    EXPECT_EQ(0xFF0000FF,p->GetPatternByIndex(0x5));
    EXPECT_EQ(0x5AA5A55A,p->GetPatternByIndex(0x6));
    EXPECT_EQ(0xCAACACCA,p->GetPatternByIndex(0x7));
    EXPECT_EQ(0x57757557,p->GetPatternByIndex(0x8));
    EXPECT_EQ(0xC55C5CC5,p->GetPatternByIndex(0x9));
    EXPECT_EQ(0x7AA7A77A,p->GetPatternByIndex(0xA));
    EXPECT_EQ(0x7CC7C77C,p->GetPatternByIndex(0xB));
    EXPECT_EQ(0x57ACCA75,p->GetPatternByIndex(0xC));
    EXPECT_EQ(0x7AC55CA7,p->GetPatternByIndex(0xD));
    EXPECT_EQ(0xAC5775CA,p->GetPatternByIndex(0xE));
    EXPECT_EQ(0x00000000,p->GetPatternByIndex(0xF));


    EXPECT_EQ(p->GetIndexByPattern(0xFFFFFFFF),0x1);
    EXPECT_EQ(p->GetIndexByPattern(0x0000FFFF),0x2);
    EXPECT_EQ(p->GetIndexByPattern(0xF0F00F0F),0x3);
    EXPECT_EQ(p->GetIndexByPattern(0xFF00FF00),0x4);
    EXPECT_EQ(p->GetIndexByPattern(0xFF0000FF),0x5);
    EXPECT_EQ(p->GetIndexByPattern(0x5AA5A55A),0x6);
    EXPECT_EQ(p->GetIndexByPattern(0xCAACACCA),0x7);
    EXPECT_EQ(p->GetIndexByPattern(0x57757557),0x8);
    EXPECT_EQ(p->GetIndexByPattern(0xC55C5CC5),0x9);
    EXPECT_EQ(p->GetIndexByPattern(0x7AA7A77A),0xA);
    EXPECT_EQ(p->GetIndexByPattern(0x7CC7C77C),0xB);
    EXPECT_EQ(p->GetIndexByPattern(0x57ACCA75),0xC);
    EXPECT_EQ(p->GetIndexByPattern(0x7AC55CA7),0xD);
    EXPECT_EQ(p->GetIndexByPattern(0xAC5775CA),0xE);
    EXPECT_EQ(p->GetIndexByPattern(0x00000000),0xF);

    EXPECT_TRUE(p->IsValidIndex(0xF));
    EXPECT_EQ(p->IsValidIndex(16),false);

    LOGINFO("clean up");

    delete p;
    delete buf_mngr;
    delete ctrlr_context;
}