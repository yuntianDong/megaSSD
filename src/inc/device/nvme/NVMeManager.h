#ifndef _NVME_MANAGE_H
#define _NVME_MANAGE_H

#include "device/DeviceHandler.h"

class NVMeManager
{
protected:
	NVMeEngine* mpEngine;
public:
	NVMeManager(Device* dev)
		:mpEngine(NULL)
	{
		if(nullptr != dev->GetEngine())
		{
			mpEngine = dev->GetEngine();
		}
	}

	virtual ~NVMeManager(void)
	{};

	bool NVMeReset(void)
	{ return mpEngine->NVMeReset();};
	bool NVMeSubSysReset(void)
	{ return mpEngine->NVMeSubSysReset();};
};

#endif
