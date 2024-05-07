/*
 * file : utIHostDevice.cpp
 */

#define TEST_BDF_NAME       "0000:c1:00.0"
#define BDF_DRIVER_NVME     "nvme"
#define BDF_DRIVER_PCIE     "pcie"
#define BDF_DRIVER_UIO      "uio_pci_generic"
#define INVALID_DEV_IDX      0xFFFF
#define DUAL_PORT_SUPPORT   false

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "IHostDevice.h"

class utIHostDevice : public testing::Test
{
};

TEST_F(utIHostDevice, test_IHostDevice)
{
    IHostDevice *hostDevice = new IHostDevice(TEST_BDF_NAME);

    Device *nvme_dev = hostDevice->GetNVMeDevice(0);
    EXPECT_NE(nvme_dev , nullptr);

    EXPECT_STREQ(nvme_dev->GetBdf(),TEST_BDF_NAME);
    EXPECT_STREQ(nvme_dev->GetDriver(),BDF_DRIVER_NVME);
    EXPECT_EQ(nvme_dev->GetBdfID(),0);
    EXPECT_NE(nvme_dev->GetEngine() , nullptr);

    Operator* nvme_operator = nvme_dev->GetOperator();
    EXPECT_NE(nvme_operator , nullptr);
    EXPECT_NE(nvme_operator->GetNVMeFd() , -1);

    Device* pci_dev = hostDevice->GetPcieDevice(0);
    EXPECT_NE(pci_dev , nullptr);

    EXPECT_STREQ(pci_dev->GetBdf(),TEST_BDF_NAME);
    EXPECT_STREQ(pci_dev->GetDriver(),BDF_DRIVER_PCIE);
    EXPECT_EQ(pci_dev->GetBdfID(),0);
    // EXPECT_EQ(pci_dev->GetEngine() , nullptr);

    Operator* pci_operator = pci_dev->GetOperator();
    EXPECT_NE(pci_operator , nullptr);
    EXPECT_NE(pci_operator->GetPcieDev() , nullptr);
    EXPECT_NE(pci_operator->GetBarMemBase() , nullptr);

    Device* invalid_nvme_idx = hostDevice->GetNVMeDevice(INVALID_DEV_IDX);
    EXPECT_EQ(invalid_nvme_idx , nullptr);
    Device* invalid_pci_idx = hostDevice->GetPcieDevice(INVALID_DEV_IDX);
    EXPECT_EQ(invalid_pci_idx , nullptr);

    del_obj(hostDevice)
}