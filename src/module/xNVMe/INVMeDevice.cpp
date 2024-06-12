/*
 * file : INVMeDevice.cpp
 */

#include "INVMeDevice.h"
#include "Logging.h"

NVMeAdminCmd* INVMeDevice::GetCtrlr(Device* device)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	if (device == nullptr)
	{
		LOGERROR("Invalid Parameter, object Device is NULL, please check!\n");
		return nullptr;
	}
	else
	{
		getAdmin = new NVMeAdminCmd(device);
		return getAdmin;
	}   
}

NVMeNVMCmd* INVMeDevice::GetNS(Device* device,uint32_t nsID)
{
	strict_lock<lockable_type> guard(*(this->mLock));

	if (device == nullptr)
	{
		LOGERROR("Invalid Parameter, object Device is NULL, please check!\n");
		getIO = nullptr;
	}
	else if (strcmp(device->GetDriver(),NVME_IN_USE) == 0)
	{
		if (nvmeNsHandlers[nsID-1] != nullptr)
		{
			getIO = new NVMeNVMCmd(nvmeNsHandlers[nsID-1]->GetDevice(),nsID);
		}
		else
		{
			nvmeNsHandlers[nsID-1] = (DeviceHandler*)new KernelHandler(device->GetBdf(),device->GetBdfID(),nsID);
			getIO = new NVMeNVMCmd(nvmeNsHandlers[nsID-1]->GetDevice(),nsID);
		}
	}
	else
	{
		getIO = new NVMeNVMCmd(device,nsID);
	}

	return getIO;
}