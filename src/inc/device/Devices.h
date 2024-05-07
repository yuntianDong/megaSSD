#ifndef _DEVICES_H
#define _DEVICES_H

#include "Commons.h"
#include "driver/stdkernel/StdKernelEngine.h"
#include "driver/stdspdk/StdSPDKEngine.h"

#define INVALID_DEVHDLR					int(-1)
#define DEV_NAME_MAXLEN					128
#define MAX_DEVICE_NUM                  32

union NVMeOperator
{
    spdk_nvme_ctrlr *mpctrlr;
    int mfd;
};
struct PcieOperator
{
    pci_dev *mppcidev;
    void *mpbarMemBase;

};
class Operator
{
private:
    NVMeOperator *mpnvmeOperator;
    PcieOperator *mppcieOperator;
public:
    Operator():mpnvmeOperator(nullptr),mppcieOperator(nullptr)
    {
        if(mpnvmeOperator == nullptr) mpnvmeOperator = new NVMeOperator;
        if(mppcieOperator == nullptr) mppcieOperator = new PcieOperator;
    };
    ~Operator()
    {
        del_obj(mpnvmeOperator);
        del_obj(mppcieOperator);
    };

    void SetSpdkCtrlr(spdk_nvme_ctrlr *ctrlr)
    { mpnvmeOperator->mpctrlr = ctrlr;};
    spdk_nvme_ctrlr *GetSpdkCtrlr(void)
    {return mpnvmeOperator->mpctrlr;};
    void SetNVMeFd(int fd)
    {mpnvmeOperator->mfd = fd;};
    int GetNVMeFd(void)
    {return mpnvmeOperator->mfd;};
    void SetPcieDev(pci_dev *pciedev)
    {mppcieOperator->mppcidev = pciedev;};
    pci_dev *GetPcieDev(void)
    {return mppcieOperator->mppcidev;};
    void SetBarMemBase(void *barMemBase)
    {mppcieOperator->mpbarMemBase = barMemBase;};
    void *GetBarMemBase(void)
    {return mppcieOperator->mpbarMemBase;};
};

class Device
{
private:
	string mbdf;
	int mBdfID;
	const char *mpdriver;

protected:
    Operator *mpoperator;
    NVMeEngine *mpengine;
public:
	Device(Operator *devOperator):mpoperator(devOperator)
	{};

    virtual ~Device(){};
    Operator* GetOperator(void)
    { assert(mpoperator); return mpoperator;};
    NVMeEngine* GetEngine(void)
    { assert(mpengine); return mpengine;};

	void SetBdf(const char *bdf)
	{ mbdf = bdf; };
	void SetBdfID(int idx)
	{ mBdfID = idx; };
	void SetDriver(const char *driver)
	{ mpdriver = driver; };
	const char *GetBdf(void)
	{ return mbdf.c_str(); };
	int GetBdfID(void)
	{ return mBdfID; };
	const char *GetDriver(void)
	{ return mpdriver; };
};

class SPDKDevice:public Device
{
public:
    SPDKDevice(Operator *devOperator):Device(devOperator)
    {mpengine = (NVMeEngine*)new StdSPDKEngine(mpoperator->GetSpdkCtrlr());};
    ~SPDKDevice()
    {
        del_obj(mpengine);
    };

};

class KernelDevice:public Device
{
public:
    KernelDevice(Operator *devOperator):Device(devOperator)
    {mpengine = (NVMeEngine*)new StdKernelEngine(mpoperator->GetNVMeFd());};
    ~KernelDevice()
    {
        del_obj(mpengine);
    };
};

class PcieDevice:public Device
{
public:
    PcieDevice(Operator *devOperator):Device(devOperator)
    {mpengine = nullptr;};
    ~PcieDevice(){};
};

#endif