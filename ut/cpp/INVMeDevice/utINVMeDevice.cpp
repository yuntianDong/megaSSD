/**
 * file: utINVMeDevice.cpp
*/
#include<iostream>
using namespace std;
#include<gtest/gtest.h>
#include<fstream>
#include<cmath>

#include "INVMeDevice.h"
#include "IHostDevice.h"
#include "IHostBufMngr.h"

#define TEST_BDF_NAME       "0000:c1:00.0"
#define DEFAULT_NS_ID       1

class utINVMeDevice: public testing::Test
{
public:
    IHostDevice *dev_mngr;
    Device *dev;
    IHostBufMngr *buf_mngr;
    ParentContext *parent_ctx;

    virtual void SetUp()
    {
        dev_mngr = new IHostDevice(TEST_BDF_NAME);
        dev = dev_mngr->GetNVMeDevice(0);
        buf_mngr = new IHostBufMngr();
        parent_ctx = new ParentContext();
    }

    virtual void TearDown()
    {
        del_obj(parent_ctx)
        del_obj(buf_mngr)
        del_obj(dev_mngr)
    }
};

// TEST_F(utINVMeDevice, test_INVMeDevice)
// {
//     INVMeDevice *nvm_mngr = new INVMeDevice();
//     NVMeAdminCmd *dev_admin_cmd = nvm_mngr->GetCtrlr(dev);
//     NVMeNVMCmd *dev_nvm_cmd = nvm_mngr->GetNS(dev,DEFAULT_NS_ID);

//     EXPECT_TRUE(dev_admin_cmd != nullptr);
//     EXPECT_TRUE(dev_nvm_cmd != nullptr);

//     NVMeAdminCmd *invalid_admin_cmd = nvm_mngr->GetCtrlr(nullptr);
//     EXPECT_TRUE(invalid_admin_cmd == nullptr);
//     NVMeNVMCmd *invalid_nvm_cmd = nvm_mngr->GetNS(nullptr,DEFAULT_NS_ID);
//     EXPECT_TRUE(invalid_nvm_cmd == nullptr);

//     del_obj(nvm_mngr);
// }

TEST_F(utINVMeDevice, test_GetCtrlr)
{
    INVMeDevice *nvm_mngr = new INVMeDevice();
    NVMeAdminCmd *dev_admin_cmd = nvm_mngr->GetCtrlr(dev);

    EXPECT_TRUE(dev_admin_cmd->LoadParentContext(parent_ctx));
    EXPECT_TRUE(dev_admin_cmd->LoadBufferMngr(buf_mngr));

    EXPECT_TRUE(dev_admin_cmd->LoadPatternMngr(nullptr) == false);
    EXPECT_EQ(dev_admin_cmd->IsPatternMngrLoaded(),false);

    EXPECT_TRUE(dev_admin_cmd->IsNSActive(DEFAULT_NS_ID));
    EXPECT_EQ(dev_admin_cmd->IsNSActive(NS_ID_SPECIAL - 1),false);

    EXPECT_TRUE(dev_admin_cmd->IsBufferMngrLoaded());
    EXPECT_TRUE(dev_admin_cmd->IsParentContextLoaded());
    EXPECT_EQ(dev_admin_cmd->IsPatternMngrLoaded(),false);

    LOGINFO("Cfg context and test Identify API");
    Buffers *iden_buf = buf_mngr->GetUsrBuf("Identify",4096,0,dev->GetDriver());
    NVMeCmdResp *resp = dev_admin_cmd->Identify(0x01,0,0,0,0,iden_buf,8000);
    EXPECT_TRUE(resp->Succeeded());
    iden_buf->SetEndianMode(BUFV_LITTLE_ENDIAN);
    uint64_t total_nvm_size = iden_buf->GetQWord(280);
    parent_ctx->SetTotalNvmSize(total_nvm_size);
    iden_buf->SetEndianMode(BUFV_BIG_ENDIAN);
    iden_buf->FillZero(0,4096);

    resp = dev_admin_cmd->Identify(0x0,0,1,0,0,iden_buf,8000);
    iden_buf->SetEndianMode(BUFV_LITTLE_ENDIAN);
    uint16_t num_lbaf = iden_buf->GetByte(25) + 1;
    LOGINFO("number of lbaf is %u",num_lbaf);
    EXPECT_NE(num_lbaf,0);
    uint32_t lba_structure_pos = 128;
    for(uint16_t lbaf = 0; lbaf < num_lbaf; lbaf++)
    {
        uint32_t lba_structure = iden_buf->GetDWord(lba_structure_pos);

        uint32_t meta_size = lba_structure & 0xFFFF;
        uint32_t data_size = std::pow(2,(lba_structure & 0xFF0000) >> 16);

        parent_ctx->AddLbaMap(lbaf,data_size,meta_size);
        lba_structure_pos += 4;
    }

    parent_ctx->DumpLbaMap();

    iden_buf->FillZero(0,4096);
    iden_buf->SetEndianMode(BUFV_BIG_ENDIAN);
    resp = dev_admin_cmd->Identify(0x10,0,0,0,0,iden_buf,8000);
    EXPECT_TRUE(resp->Succeeded());

    Buffers *cfg_ns_buf = buf_mngr->GetUsrBuf("Identify",4096,1,dev->GetDriver());

    uint64_t ns_map_offset = 0;
    iden_buf->SetEndianMode(BUFV_LITTLE_ENDIAN);
    for(uint32_t idx = 0;idx < cfg_ns_buf->GetBufSize();idx += 4)
    {
        uint32_t nsid = iden_buf->GetDWord(idx);
        if(nsid == NS_ID_DEFAULT)
        {
            break;
        }
        else
        {
            resp = dev_admin_cmd->Identify(0x11,0,nsid,0,0,cfg_ns_buf,8000);
            EXPECT_TRUE(resp->Succeeded());

            cfg_ns_buf->SetEndianMode(BUFV_LITTLE_ENDIAN);
            uint16_t lbaf = (cfg_ns_buf->GetByte(26)) >> 5;
            parent_ctx->AddNsMap(nsid,ns_map_offset,cfg_ns_buf->GetQWord(8),lbaf);
            ns_map_offset += cfg_ns_buf->GetQWord(8);
            ChildContext *child_ctx = parent_ctx->GetChildContext(nsid);
            EXPECT_NE(child_ctx,nullptr);
            cfg_ns_buf->SetEndianMode(BUFV_BIG_ENDIAN);
            cfg_ns_buf->FillZero(0,4096);
        }
    }

    iden_buf->SetEndianMode(BUFV_BIG_ENDIAN);
    parent_ctx->DumpNsMap();

    NVMeNVMCmd *dev_nvm_cmd = nvm_mngr->GetNS(dev,DEFAULT_NS_ID);
    EXPECT_TRUE(dev_nvm_cmd->LoadChildContext(parent_ctx->GetChildContext(DEFAULT_NS_ID)));
    EXPECT_TRUE(dev_nvm_cmd->LoadBufferMngr(buf_mngr));

    LOGINFO("Test Write");
    buf_mngr->SetSectorSize(512);
    Buffers *write_buf = buf_mngr->GetIOWrBuf(DEFAULT_NS_ID,0,dev->GetDriver());
    write_buf->FillSector(0,4,0x57ACCA75);
    write_buf->Dump(0,128);
    resp = dev_nvm_cmd->Write(0,3,0,0,0,0,0,0,0,0,0,0,0,write_buf,nullptr,8000);
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }
    EXPECT_TRUE(resp->Succeeded());

    LOGINFO("Test Read");
    Buffers *read_buf = buf_mngr->GetIORdBuf(DEFAULT_NS_ID,0,dev->GetDriver());
    read_buf->FillZero(0,512*4);
    resp = dev_nvm_cmd->Read(0,3,0,0,0,0,0,0,0,0,0,read_buf,nullptr,8000);
    read_buf->Dump(0,128);
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }
    EXPECT_TRUE(resp->Succeeded());

    LOGINFO("Test Format NVM 512 + 8");
    resp = dev_admin_cmd->FormatNVM(NS_ID_SPECIAL,1,0,0,0,0,8000);
    EXPECT_TRUE(resp->Succeeded());
    ChildContext* nsid_1_ctx = parent_ctx->GetChildContext(DEFAULT_NS_ID);
    EXPECT_EQ(nsid_1_ctx->GetNsDataSize(),4096);
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }
    // EXPECT_TRUE(dev->GetEngine()->NVMeReset());
    // sleep(1);

    LOGINFO("Test Format NVM 4k");
    resp = dev_admin_cmd->FormatNVM(NS_ID_SPECIAL,2,1,0,0,0,8000);
    EXPECT_TRUE(resp->Succeeded());
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }
    // EXPECT_TRUE(dev->GetEngine()->NVMeReset());
    // sleep(1);

    LOGINFO("Test Format NVM 4k + 8");
    resp = dev_admin_cmd->FormatNVM(NS_ID_SPECIAL,3,2,0,0,0,8000);
    EXPECT_TRUE(resp->Succeeded());
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }
    // EXPECT_TRUE(dev->GetEngine()->NVMeReset());
    // sleep(1);

    LOGINFO("Test Format NVM 512");
    resp = dev_admin_cmd->FormatNVM(NS_ID_SPECIAL,0,0,0,0,0,8000);
    EXPECT_TRUE(resp->Succeeded());
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }
    EXPECT_EQ(nsid_1_ctx->GetNsDataSize(),512);
    // EXPECT_TRUE(dev->GetEngine()->NVMeReset());
    sleep(1);

    LOGINFO("Test GetFeatures");
    Buffers *get_feature_buf = buf_mngr->GetUsrBuf("GetFeatures",4096,0,dev->GetDriver());
    resp = dev_admin_cmd->GetFeatures(0,0x4,0,0,get_feature_buf,8000);
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }
    EXPECT_TRUE(resp->Succeeded());

    LOGINFO("Test SetFeatures");
    Buffers *set_feature_buf = buf_mngr->GetUsrBuf("SetFeatures",4096,0,dev->GetDriver());
    resp = dev_admin_cmd->SetFeatures(0,0x2,0,0,set_feature_buf,8000);
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }
    EXPECT_TRUE(resp->Succeeded());

    LOGINFO("Test GetLogPage");
    Buffers *get_log_page_buf = buf_mngr->GetUsrBuf("GetLogPage",4096,0,dev->GetDriver());
    resp = dev_admin_cmd->GetLogPage(NS_ID_SPECIAL,0x2,0,127,0,false,get_log_page_buf,8000);
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }
    EXPECT_TRUE(resp->Succeeded());

    LOGINFO("Test DeviceSelfTest");
    resp = dev_admin_cmd->DeviceSelfTest(1,1,8000);
    EXPECT_TRUE(resp->Succeeded());
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }

    LOGINFO("Test NSAttach");
    Buffers *ns_attach_buf = buf_mngr->GetUsrBuf("NSAttach",4096,0,dev->GetDriver());
    ns_attach_buf->SetEndianMode(BUFV_LITTLE_ENDIAN);
    ns_attach_buf->SetWord(0,1);
    ns_attach_buf->SetWord(2,0);
    ns_attach_buf->SetEndianMode(BUFV_BIG_ENDIAN);
    resp = dev_admin_cmd->NSAttach(1,1,ns_attach_buf,8000);
    EXPECT_TRUE(resp->Succeeded());
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }

    LOGINFO("Test NSManagement Delete NS");
    Buffers *ns_mgmt_buf = buf_mngr->GetUsrBuf("NSManagement",4096,0,dev->GetDriver());
    resp = dev_admin_cmd->NSMngmnt(1,1,0,ns_mgmt_buf,8000);
    EXPECT_TRUE(resp->Succeeded());
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }

    // LOGINFO("Test FwDownload");
    // std::ifstream file("/root/dyt/DUEY801H_FUNC_NONSEC_20231009_186354e6/drivecfg_ff_UD2_2MGC_1RG_64Die_7680G_TAS_V1.00.01.smg", std::ios::binary | std::ios::ate);
    // EXPECT_TRUE(file.is_open());
    // std::streampos fileSize = file.tellg();
    // file.seekg(0, std::ios::beg);
    // Buffers *fw_download_buf = buf_mngr->GetUsrBuf("FwDownload",fileSize,0,dev->GetDriver());
    // void *buf = (void*)fw_download_buf->GetBufferAddr();
    // file.read((char*)buf, fileSize);
    // resp = dev_admin_cmd->FirmwareDownload(0,ceil(fileSize/4),fw_download_buf,8000);
    // file.close();
    // EXPECT_TRUE(resp->Succeeded());
    // if(true != resp->Succeeded())
    // {
    //     resp->Dump();
    // }
    // sleep(1);

    // LOGINFO("Test FwCommit");
    // resp  = dev_admin_cmd->FirmwareCommit(0,1,false,8000);
    // EXPECT_TRUE(resp->Succeeded() == false);
    // EXPECT_TRUE(resp->GetSC() == 0x11);
    // if(true != resp->Succeeded())
    // {
    //     resp->Dump();
    // }
    LOGINFO("sleep 1s,reset ctrlr");
    sleep(1);
    EXPECT_TRUE(dev->GetEngine()->NVMeReset());

    LOGINFO("Test Sanitize");
    resp = dev_admin_cmd->Sanitize(2,0,0,false,false,false,8000);
    if(true != resp->Succeeded())
    {
        resp->Dump();
    }
    EXPECT_TRUE(resp->Succeeded());

    LOGINFO("sleep 70 secs to wait sanitize down");
    sleep(70);


    del_obj(nvm_mngr);
}