/*
 * NVMeScan.cpp
 */
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <assert.h>
#include "device/NVMeScan.h"
#include "Logging.h"

using namespace std;


//
//	devPath : /devices/pci0000:00/0000:00:0e.0/nvme/nvme0
//
const char* GetPCIAddrFromDevPath(string devPath)
{
	size_t pos = devPath.find(NVME_DEV_SUBSYSTEM);
	if(string::npos == pos)
	{
		return NULL;
	}
	devPath.erase(pos-1);

	pos = devPath.rfind("/");
	if(string::npos == pos)
	{
		return NULL;
	}
	devPath.erase(0,pos+1);

	return devPath.c_str();
}

const char* NVMeScan::GetDevNameByPciAddr(PCIAddr* pciAddr)
{
	assert(NULL != pciAddr);

	if(false == mDevScan->DoScan(ScanBy_SubSystem,NVME_DEV_SUBSYSTEM))
	{
		LOGERROR("Fail to call DoScan()");
		return INVALID_DEV_NAME;
	}

	for(int idx=0;idx<mDevScan->GetDevNodeCount();idx++)
	{
		uDevNode* node = mDevScan->GetDevNode(idx);
		string sysPath	= node->GetSysPath();
		// LOGDEBUG("sysPath = %s",sysPath.c_str());

		if(string::npos != sysPath.find(pciAddr->GetString(),0))
		{
			LOGDEBUG("node->GetDevNode() = %s",node->GetDevNode());
			return node->GetDevNode();
		}
	}

	return INVALID_DEV_NAME;
}

const char* NVMeScan::GetLinkNameByPciAddr(PCIAddr* pciAddr)
{
	assert(NULL != pciAddr);

	if(false == mDevScan->DoScan(ScanBy_SubSystem,NVME_DEV_SUBSYSTEM))
	{
		LOGERROR("Fail to call DoScan()");
		return INVALID_DEV_NAME;
	}

	for(int idx=0;idx<mDevScan->GetDevNodeCount();idx++)
	{
		uDevNode* node = mDevScan->GetDevNode(idx);
		string sysPath	= node->GetSysPath();

		if(string::npos != sysPath.find(pciAddr->GetString(),0))
		{
			LOGDEBUG("Dev Link Node is %s",node->GetDevProperty("DEVLINKS"));
			return node->GetDevProperty("DEVLINKS");
		}
	}

	return INVALID_DEV_NAME;
}

PCIAddr* NVMeScan::GetPciAddrByDevName(const char* devName)
{
	int devIndex	= 0;
	if(NULL == devName || 1 != sscanf(devName,NVME_DEV_NAME_PAT,&devIndex))
	{
		LOGERROR("devName is null or sscanf() Failed");
		return NULL;
	}

	char nodeName[NVME_NODE_NAME_MAX];
	snprintf(nodeName,NVME_NODE_NAME_MAX,NVME_NODE_NAME_PATH,devIndex);

	assert(NULL != mDevScan);
	LOGDEBUG("nodeName = %s",nodeName);

	if(false == mDevScan->DoScan(ScanBy_SysName,nodeName))
	{
		LOGERROR("DoScan() Failed");
		return NULL;
	}

	if(1 != mDevScan->GetDevNodeCount())
	{
		LOGERROR("GetDevNodeCount() should return 1");
		return NULL;
	}

	uDevNode* node 		= mDevScan->GetDevNode(0);
	assert(NULL != node);
	const char* devPath = node->GetDevPath();
	if(NULL == devPath)
	{
		LOGERROR("devPath shouldn't be NULL");
		return NULL;
	}

	LOGDEBUG("devPath = %s",devPath);
	const char* addr	= ::GetPCIAddrFromDevPath(devPath);
	if(NULL == addr)
	{
		LOGERROR("GetPCIAddrFromDevPath() return NULL");
		return NULL;
	}

	LOGDEBUG("addr = %s",addr);
	return new PCIAddr(addr);
}
