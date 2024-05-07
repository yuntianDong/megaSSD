#ifndef _I_HOST_DEVICE_H
#define _I_HOST_DEVICE_H

#include "device/DeviceMngr.h"

class IHostDevice
{
private:
    DeviceMngr* mpDevMngr;
    const char* mBDF;
public:
    IHostDevice(const char* bdf):mpDevMngr(nullptr),mBDF(bdf)
    { mpDevMngr = new DeviceMngr(mBDF);};
    ~IHostDevice()
    {
        del_obj(mpDevMngr);
    };

    Device* GetNVMeDevice(int idx)
    { 
        if(mpDevMngr->GenNVMeDevice(idx))
        {
            return mpDevMngr->GenNVMeDevice(idx);
        }else{
            LOGERROR("Invalid device index, maybe out of range, use GetDevCnt to check!");
            return nullptr;
        }
        
    };
    Device* GetPcieDevice(int idx)
    { 
        if(mpDevMngr->GenPcieDevice(idx))
        {
            return mpDevMngr->GenPcieDevice(idx);
        }else{
            LOGERROR("Invalid device index, maybe out of range, use GetDevCnt to check!");
            return nullptr;
        }
    };
    int GetDevCnt(void)
    {
        return mpDevMngr->GetNodeCnt();
    }

    void CloseDev(void)
    {
        del_obj(mpDevMngr);
    }

    Device* RestoreDev(uint32_t idx,const char* driver)
    {
        Device* dev = nullptr;
        mpDevMngr = new DeviceMngr(mBDF);
        if(strcmp(driver,PCIE_IN_USE) == 0)
        {
            dev = this->GetPcieDevice(idx);
        }else{
            dev = this->GetNVMeDevice(idx);
        }
        return dev;
    }
};
#endif