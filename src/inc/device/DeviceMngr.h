#include "device/DeviceHandler.h"

class DeviceMngr
{
private:
    DeviceHandler *mpPcieDeviceHandler[MAX_DEVICE_NUM];
    DeviceHandler *mpNVMeDeviceHandler[MAX_DEVICE_NUM];
    const char *mbdf[MAX_DEVICE_NUM];
    bool ifrestore;
public:
    DeviceMngr(const char *bdf,bool ifrestore=false);
    ~DeviceMngr();

    int GetDevAllBdf(const char *bdf,const char *mbdf[]);
    uint64_t GetDSN(const char *bdf);
    Device *GenPcieDevice(int ctrlrID);
    Device *GenNVMeDevice(int ctrlrID);
    int GetNodeCnt(void)
    {
        int i = 0;
        for (i; i < MAX_DEVICE_NUM; i++)
        {
            if(mbdf[i] == nullptr)
            {
                break;
            }
        }
        return i;
    };
};