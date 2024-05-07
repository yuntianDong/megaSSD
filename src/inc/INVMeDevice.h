#ifndef _I_NVME_DEVICE_H
#define _I_NVME_DEVICE_H

#include "device/DeviceHandler.h"
#include "device/nvme/NVMeCommand.h"
#include "IBasicLock.h"


class INVMeDevice : IBasicLock
{
public:
	NVMeAdminCmd* getAdmin;
	NVMeNVMCmd* getIO;
	INVMeDevice(void)
	{getAdmin = nullptr; getIO = nullptr;};

	NVMeAdminCmd* GetCtrlr(Device* device);
	NVMeNVMCmd* GetNS(Device* device,uint32_t nsID);
	~INVMeDevice()
	{};
};

#endif
