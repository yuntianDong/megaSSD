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
		return nullptr;
	}
	else
	{
		getIO = new NVMeNVMCmd(device,nsID);
		return getIO;
	}
}