#ifndef _PCI_ADDR_H
#define _PCI_ADDR_H

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "Logging.h"

#define PCI_ADDR_FMT_TYPE1			"%04x:%02x:%02x.%x"
#define PCI_ADDR_FMT_TYPE2			"%04x:%02x:%02x"
#define PCI_ADDR_FMT_TYPE3			"%02x:%02x.%x"
#define PCI_ADDR_FMT_TYPE4			"%02x:%02x"

#define PCI_ADDR_STRLEN_MAX			16

class PCIAddr
{
private:
	uint32_t	mDomain;
	uint8_t		mBus;
	uint8_t		mDev;
	uint8_t		mFunc;

	char	mPCIAddrStr[PCI_ADDR_STRLEN_MAX];
public:
	PCIAddr(uint32_t domain,uint8_t bus,uint8_t dev,uint8_t func)
		:mDomain(domain),mBus(bus),mDev(dev),mFunc(func)
	{
	};
	PCIAddr(const char* pciAddr)
	{
		assert(true == ParsePCIAddr(pciAddr));
	};

	uint32_t GetDomain(void) {return mDomain;};
	uint8_t GetBus(void) {return mBus;};
	uint8_t GetDev(void) {return mDev;};
	uint8_t GetFunc(void) {return mFunc;};
	void SetDomain(uint32_t v) {mDomain = v;};
	void SetBus(uint8_t v) {mBus = v;};
	void SetDev(uint8_t v) {mDev = v;};
	void SetFunc(uint8_t v) {mFunc = v;};

	bool ParsePCIAddr(const char* pciAddr)
	{
		uint32_t	domain,bus, dev, func;
		domain = bus = dev = func = 0;

		assert(pciAddr!=NULL);
		LOGDEBUG("pciAddr = %s",pciAddr);

		if ((sscanf(pciAddr, "%x:%x:%x.%x", &domain, &bus, &dev, &func) == 4) ||
		    (sscanf(pciAddr, "%x.%x.%x.%x", &domain, &bus, &dev, &func) == 4))
		{
			LOGDEBUG("PCI_ADDR_FMT_TYPE1");
		}
		else if (sscanf(pciAddr, "%x:%x:%x", &domain, &bus, &dev) == 3)
		{
			LOGDEBUG("PCI_ADDR_FMT_TYPE2");
		}
		else if ((sscanf(pciAddr, "%x:%x.%x", &bus, &dev, &func) == 3) ||
			   (sscanf(pciAddr, "%x.%x.%x", &bus, &dev, &func) == 3))
		{
			LOGDEBUG("PCI_ADDR_FMT_TYPE3");
		}
		else if ((sscanf(pciAddr, "%x:%x", &bus, &dev) == 2) ||
			   (sscanf(pciAddr, "%x.%x", &bus, &dev) == 2))
		{
			LOGDEBUG("PCI_ADDR_FMT_TYPE4");
		} else {
			return false;
		}

		if (dev > 0x1F || func > 7) {
			return false;
		}

		mDomain = domain;
		mBus 	= bus;
		mDev 	= dev;
		mFunc 	= func;

		LOGDEBUG("domain = %d,bus=%d,dev=%d,func=%d",domain,bus,dev,func);

		return true;
	};

	char* GetString(bool bDomain=true,bool bFunc=true)
	{
		memset(mPCIAddrStr,0,PCI_ADDR_STRLEN_MAX);

		if(true == bDomain && true == bFunc)
		{
			snprintf(mPCIAddrStr,PCI_ADDR_STRLEN_MAX,PCI_ADDR_FMT_TYPE1,\
					mDomain,mBus,mDev,mFunc);
		}
		else if(true == bDomain)
		{
			snprintf(mPCIAddrStr,PCI_ADDR_STRLEN_MAX,PCI_ADDR_FMT_TYPE2,\
					mDomain,mBus,mDev);
		}
		else if(true == bFunc)
		{
			snprintf(mPCIAddrStr,PCI_ADDR_STRLEN_MAX,PCI_ADDR_FMT_TYPE3,\
					mBus,mDev,mFunc);
		}
		else
		{
			snprintf(mPCIAddrStr,PCI_ADDR_STRLEN_MAX,PCI_ADDR_FMT_TYPE4,\
					mBus,mDev);
		}

		return mPCIAddrStr;
	};
};

#endif
