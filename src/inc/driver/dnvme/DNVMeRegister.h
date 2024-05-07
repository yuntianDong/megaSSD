#ifndef _DNVME_REGISTER_H
#define _DNVME_REGISTER_H

#include <stdint.h>
#include "NVMeRegDef.h"

class DNVMeRegister
{
private:
	enNVMeSpecRev mSpecRev;
	int mDevHdlr;

	bool ValidSpecRev(const vector<enNVMeSpecRev> revLst) {
		return std::find(revLst.begin(),revLst.end(),mSpecRev) != revLst.end();
	};

protected:
	DNVMeRegister(int devHdlr,enNVMeSpecRev specRev);

	static stNVMePciSpc mPciSpcTbl[];
	static stNVMeCtlSpc mCtlSpcTbl[];

public:
	bool Read(enNVMePciSpc reg,uint64_t &value,uint8_t bits=0,uint8_t offset=0);
	bool Read(enNVMeCtlSpc reg,uint64_t &value,uint8_t bits=0,uint8_t offset=0);

	bool Write(enNVMePciSpc reg,uint64_t value,uint8_t bits=0,uint8_t offset=0);
	bool Write(enNVMeCtlSpc reg,uint64_t value,uint8_t bits=0,uint8_t offset=0);
};


/////////////////////////////////////////////////////////////////////////////////
//				NVMe PCI Register
/////////////////////////////////////////////////////////////////////////////////
class DNVMePCIReg:public DNVMeRegister
{
private:
	static bool mInstanceFlag;
	static DNVMePCIReg* mInstanceObj;

	void ScanPCICapRegOffset(void);
	DNVMePCIReg(int devHdlr,enNVMeSpecRev specRev);

protected:
	static stNVMePciReg mPciRegTbl[];

public:
	static DNVMePCIReg* GetInstance(int devHdlr,enNVMeSpecRev specRev);

	bool GetRegVal(enNVMePCIReg reg,uint32_t &val);
	bool SetRegVal(enNVMePCIReg reg,uint32_t val);
	bool ClrRegVal(enNVMePCIReg reg);
};

#define GetDeviceID(v)				GetRegVal(PCIHI_DID,v)
#define GetVendorID(v)				GetRegVal(PCIHI_VID,v)

/////////////////////////////////////////////////////////////////////////////////
//				NVMe Controller Register
/////////////////////////////////////////////////////////////////////////////////

#endif
