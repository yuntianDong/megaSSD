#ifndef _I_NVME_DEVICE_H
#define _I_NVME_DEVICE_H

#include "device/DeviceHandler.h"
#include "device/nvme/NVMeCommand.h"
#include "IBasicLock.h"

#define NS_MAX_NUM 1024

class INVMeDevice : IBasicLock
{
public:
	NVMeAdminCmd *getAdmin;
	NVMeNVMCmd *getIO;
	DeviceHandler *nvmeNsHandlers[NS_MAX_NUM];
	INVMeDevice(void) : nvmeNsHandlers{nullptr}
	{
		getAdmin = nullptr;
		getIO = nullptr;
	};

	NVMeAdminCmd *GetCtrlr(Device *device);
	NVMeNVMCmd *GetNS(Device *device, uint32_t nsID);
	void ReleaseNS(uint32_t nsID);
	~INVMeDevice()
	{
		for (int idx = 0; idx < NS_MAX_NUM; idx++)
		{
			del_obj(nvmeNsHandlers[idx]);
		}
	};
};

#endif
