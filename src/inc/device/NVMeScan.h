#ifndef _NVME_SCAN_H
#define _NVME_SCAN_H

#include "device/PCIAddr.h"
#include "udev/udevScan.h"

#ifdef VBOX_DEBUG
#define NVME_DEV_SUBSYSTEM		"misc"
#else
#define NVME_DEV_SUBSYSTEM		"nvme"
#define UIO_DEV_SUBSYSTEM		"uio"
#endif
#define INVALID_DEV_NAME		""
#define NVME_DEV_NAME_PAT		"/dev/nvme%d"
#define NVME_NODE_NAME_PATH		"nvme%d"
#define NVME_NODE_NAME_MAX		8

class NVMeScan
{
private:
	uDevScan *	mDevScan;
public:
	NVMeScan(void)
	{
		mDevScan = new uDevScan();
	};

	virtual ~NVMeScan(void)
	{
		if(NULL != mDevScan)
		{
			delete mDevScan;
			mDevScan = NULL;
		}
	}

	const char* GetDevNameByPciAddr(PCIAddr* pciAddr);
	const char* GetLinkNameByPciAddr(PCIAddr* pciAddr);
	PCIAddr* GetPciAddrByDevName(const char* devName);
};

#endif
