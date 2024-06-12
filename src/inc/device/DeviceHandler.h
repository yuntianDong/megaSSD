#ifndef _DEVICE_HANDLER_H
#define _DEVICE_HANDLER_H

#include "device/Devices.h"
#include "device/NVMeScan.h"
#define PCI_BAR_PATH	"/sys/bus/pci/devices/%04x:%02x:%02x.%x/resource0"
#define MAP_SIZE		4096
#define SPDK_ENV_OPT_NAME	"asha"

class DeviceHandler
{
protected:
	Device *mdev;
	Operator *devoperator;

public:
	DeviceHandler(){};

	virtual ~DeviceHandler()
	{
		del_obj(mdev);
	};

	virtual void GenOperator(const char *bdf,int nsid = -1)=0;
	virtual void ReleaseOperator(void)=0;
	Device* GetDevice(void)
	{ 
		if(mdev)
		{
			return mdev;
		}else{
			LOGERROR("Invalid ctrlrID\n");
		}
		return nullptr;
	};
};

class SPDKHandler:public DeviceHandler
{
public:
	SPDKHandler(const char* bdf,int idx);
	~SPDKHandler()
	{
		if(devoperator->GetSpdkCtrlr())
		{
			ReleaseOperator();
		}
		del_obj(devoperator);
	};

	void GenOperator(const char *bdf,int nsid = -1);
	void ReleaseOperator(void);
};

class KernelHandler:public DeviceHandler
{
public:
	KernelHandler(const char* bdf,int idx,int nsid = -1);
	~KernelHandler()
	{
		if(devoperator->GetNVMeFd() != INVALID_DEVHDLR)
		{
			ReleaseOperator();
		}
		del_obj(devoperator);
	};
	void GenOperator(const char *bdf,int nsid = -1);
	void ReleaseOperator(void);
};

class PcieHandler:public DeviceHandler
{
private:
	struct pci_access *mPacc;
public:
	PcieHandler(const char* bdf,int idx);
	~PcieHandler()
	{
		if(devoperator->GetPcieDev())
		{
			ReleaseOperator();
		}
		del_obj(devoperator);
		pci_cleanup(mPacc);
	};

	void GenOperator(const char *bdf,int nsid = -1);
	void ReleaseOperator(void);
};
#endif
