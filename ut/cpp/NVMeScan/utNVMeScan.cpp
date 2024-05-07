/*
 * file : utNVMeScan.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "device/NVMeScan.h"
#include "Logging.h"

class utNVMeScan : public testing::Test
{
};

TEST_F(utNVMeScan,GetPciAddrByDevName)
{
	NVMeScan d;

	PCIAddr* addr	= d.GetPciAddrByDevName("/dev/nvme0n1");
	EXPECT_TRUE(addr!=NULL);

	const char* devName = d.GetDevNameByPciAddr(addr);
 	EXPECT_STREQ(devName,"/dev/nvme0");

	delete addr;
}

