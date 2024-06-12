/*
 * file: DeviceMngr.cpp
 */
#include "device/DeviceMngr.h"

DeviceMngr::DeviceMngr(const char *bdf):mpPcieDeviceHandler{nullptr},mpNVMeDeviceHandler{nullptr},mbdf{nullptr}
{
    if(this->GetDevAllBdf(bdf,mbdf))
    {
        LOGWARN("Something wrong when check whether this device is dual port!");
    }
    const char* driverInUse;
    struct pci_dev* mppcidev;

    int i = 0;
    while (i < MAX_DEVICE_NUM && mbdf[i] != nullptr)
    {
        mpPcieDeviceHandler[i] = (DeviceHandler*)new PcieHandler(mbdf[i],i);
        //Get which current driver in use
        assert(mpPcieDeviceHandler[i]->GetDevice());
        mppcidev =  mpPcieDeviceHandler[i]->GetDevice()->GetOperator()->GetPcieDev();
        pci_fill_info(mppcidev,PCI_FILL_DRIVER);
        if(driverInUse = pci_get_string_property(mppcidev,PCI_FILL_DRIVER))
        {
            if(strcmp(driverInUse,NVME_IN_USE) == 0)
            {
                mpNVMeDeviceHandler[i] = (DeviceHandler*)new KernelHandler(mbdf[i],i);
            }else if(strcmp(driverInUse,UIO_IN_USE) == 0){
                mpNVMeDeviceHandler[i] = (DeviceHandler*)new SPDKHandler(mbdf[i],i);
            }else{
                LOGERROR("Driver Invalid : %s , not a SSD driver",driverInUse);
                std::abort();
            }
        }else{
            LOGERROR("Cannot get current driver in use");
        }
        i++;
    }
    
    mppcidev = nullptr;
}

DeviceMngr::~DeviceMngr()
{
    for(int i = 0;i < MAX_DEVICE_NUM;i++)
    {
        if(nullptr == mpPcieDeviceHandler[i] && nullptr == mpNVMeDeviceHandler[i])
        {
            break;
        }
        del_obj(mpPcieDeviceHandler[i]);
        del_obj(mpNVMeDeviceHandler[i]);
    }
}

int DeviceMngr::GetDevAllBdf(const char *bdf,const char *mbdf[])
{
    int bdfIdx = 0,ret = 0;
    mbdf[bdfIdx] = bdf;
    uint64_t mdsn = this->GetDSN(bdf);
    uDevScan *mDevScan = new uDevScan();

    /* 
        Scan all device to check whether DSN is same with given bdf
        Dev node is like /dev/nvme0 or /dev/uio0
        Parent dev node is bdf : like 0000:05:00.0
    */

    //nvme node
    if(false == mDevScan->DoScan(ScanBy_SubSystem,NVME_DEV_SUBSYSTEM))
	{
		LOGWARN("Fail to call DoScan(), maybe no device under nvme subsystem");
		ret = -1;
	}

    for(int idx=0;idx < mDevScan->GetDevNodeCount();idx++)
    {
        uDevNode *ParentNode = nullptr;

        ParentNode = mDevScan->GetDevNode(idx)->GetParent();
        if(ParentNode == nullptr)
        {
            LOGWARN("cannot fetch parent node of node %s ",mDevScan->GetDevNode(idx)->GetSysName());
            continue;
        }
        const char *scanbdf =  ParentNode->GetSysName();
        LOGDEBUG("nvme in use bdf:%s",scanbdf);

        //if scanbdf is same with given bdf ,go to next scanbdf
        if(strcmp(bdf,scanbdf) == 0)
        {
            continue;
        }

        uint64_t dsn = this->GetDSN(scanbdf);
        if(mdsn != -1 && dsn != -1 && mdsn == dsn)
        {
            mbdf[++bdfIdx] = scanbdf;
        }
    }

    //uio node
    if(false == mDevScan->DoScan(ScanBy_SubSystem,UIO_DEV_SUBSYSTEM))
	{
		LOGWARN("Fail to call DoScan(),maybe no device under uio subsystem");
		ret = -1;
	}

    for(int idx=0;idx < mDevScan->GetDevNodeCount();idx++)
    {
        uDevNode *ParentNode = nullptr;
        ParentNode = mDevScan->GetDevNode(idx)->GetParent();
        if(ParentNode == nullptr)
        {
            LOGWARN("cannot fetch parent node of node %s ",mDevScan->GetDevNode(idx)->GetSysName());
            continue;
        }
        const char *scanbdf =  ParentNode->GetSysName();
        LOGDEBUG("uio in use bdf:%s",scanbdf);
        if(strcmp(bdf,scanbdf) == 0)
        {
            continue;
        }
        uint64_t dsn = this->GetDSN(scanbdf);
        if(mdsn != -1 && dsn != -1 && mdsn == dsn)
        {
            mbdf[++bdfIdx] = scanbdf;
        }
    }

    del_obj(mDevScan);

    return ret;
}

uint64_t DeviceMngr::GetDSN(const char *bdf)
{
    /* Get DSN Func used in lspci*/

    //where is offset of header
    int where = 0x100;
    pci_dev* mppcidev = nullptr;
    Device* mpdev = nullptr;
    DeviceHandler *mpPcieDeviceHandler = (DeviceHandler*)new PcieHandler(bdf,0);

    mpdev = mpPcieDeviceHandler->GetDevice();
    if(mpdev == nullptr)
    {
        LOGWARN("cannot get pcie dev of %s ",bdf);
        return -1;
    }
    mppcidev =  mpdev->GetOperator()->GetPcieDev();

    //read id from header ,check if is DSN then read DSN from reg
    do
    {
        uint32_t header;
        int id;
        header = pci_read_long(mppcidev,where);
        id = header & 0xffff;
        if(id == PCI_EXT_CAP_ID_DSN)
        {
            uint32_t t1,t2;

            t1 = pci_read_long(mppcidev,where + 4);
            t2 = pci_read_long(mppcidev,where + 8);

            LOGDEBUG("Device Serial Number %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x",
            t2 >> 24, (t2 >> 16) & 0xff, (t2 >> 8) & 0xff, t2 & 0xff,t1 >> 24, 
            (t1 >> 16) & 0xff, (t1 >> 8) & 0xff, t1 & 0xff);

            uint64_t dsn = ((uint64_t)t2 << 32) | t1;
            LOGDEBUG("Device Serial Number raw %lx\n",dsn);

            mppcidev = nullptr;
            mpdev = nullptr;
            del_obj(mpPcieDeviceHandler);

            return dsn;
        }
        where = (header >> 20) & ~3;

    } while (where);

    mppcidev = nullptr;
    mpdev = nullptr;
    del_obj(mpPcieDeviceHandler);

    return -1;
}

Device* DeviceMngr::GenPcieDevice(int idx)
{   
    if(idx > this->GetNodeCnt())
    {
        LOGERROR("Invalid device index!");
        return nullptr;
    }

    Device *pciedev = mpPcieDeviceHandler[idx]->GetDevice();

    if(pciedev == nullptr)
    {
        LOGERROR("PcieDevice is null!");
        return nullptr;
    }
    return pciedev;
}

Device* DeviceMngr::GenNVMeDevice(int idx)
{
    if(idx > this->GetNodeCnt())
    {
        LOGERROR("Invalid device index!");
        return nullptr;
    }

    Device *nvmedev = mpNVMeDeviceHandler[idx]->GetDevice();

    if(nvmedev == nullptr)
    {
        LOGERROR("NVMeDevice is null!");
        return nullptr;
    }
    return nvmedev;
}