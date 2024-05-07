/*
 * file : DNVMeRegister.cpp
 */

#include "driver/dnvme/DNVMeIOCtrl.h"
#include "driver/dnvme/DNVMeRegister.h"
#include "Logging.h"

#define MAX_REG_VALSIZE		16

#define ZZ(a, b, c, d, e, f, g, h, i)          { b, c, d, e, f, g, h, i },
stNVMePciSpc DNVMeRegister::mPciSpcTbl[] =
{
    PCISPC_TABLE
};
#undef ZZ

#define ZZ(a, b, c, d, e, f, g, h)             { b, c, d, e, f, g, h },
stNVMeCtlSpc DNVMeRegister::mCtlSpcTbl[] =
{
	CTLSPC_TABLE
};
#undef ZZ

DNVMeRegister::DNVMeRegister(int devHdlr,enNVMeSpecRev specRev)
	:mDevHdlr(devHdlr),mSpecRev(specRev)
{
}

bool DNVMeRegister::Read(enNVMePciSpc reg,uint64_t &value,uint8_t bits,uint8_t offset)
{
	if(!ValidSpecRev(mPciSpcTbl[reg].specRev)){
		return false;
	}

	uint8_t tmpVal[MAX_REG_VALSIZE];

	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlReadGeneric(NVME_REG_PCI,
											mPciSpcTbl[reg].offset,
											mPciSpcTbl[reg].size,
											sizeof(uint8_t),
											(uint8_t*)&tmpVal);

	if( 0 == rtn )
	{
		if( 0 == offset and 0 == bits )
		{
			value = (uint64_t)tmpVal;
		}else
		{
			uint64_t bitMask	= (1 << bits -1) << offset;

			value = ((uint64_t)(tmpVal) & bitMask ) >> offset;
		}
	}

	return 0 == rtn;
}

bool DNVMeRegister::Read(enNVMeCtlSpc reg,uint64_t &value,uint8_t bits,uint8_t offset)
{
	if(!ValidSpecRev(mCtlSpcTbl[reg].specRev)){
		return false;
	}

	uint8_t tmpVal[MAX_REG_VALSIZE];

	int rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlReadGeneric(NVME_REG_BAR0,
											mCtlSpcTbl[reg].offset,
											mCtlSpcTbl[reg].size,
											sizeof(uint32_t),
											(uint8_t*)&tmpVal);

	if( 0 == rtn )
	{
		if( 0 == offset and 0 == bits )
		{
			value = (uint64_t)tmpVal;
		}else
		{
			uint64_t bitMask	= (1 << bits -1) << offset;

			value = ((uint64_t)(tmpVal) & bitMask ) >> offset;
		}
	}

	return 0 == rtn;
}

bool DNVMeRegister::Write(enNVMePciSpc reg,uint64_t value,uint8_t bits,uint8_t offset)
{
	if(!ValidSpecRev(mPciSpcTbl[reg].specRev)){
		return false;
	}

	int rtn = 0;

	if( 0 == bits and 0 == offset){
		rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlWriteGeneric(NVME_REG_PCI,
				mPciSpcTbl[reg].offset,
				mPciSpcTbl[reg].size,
				sizeof(uint8_t),
				(uint8_t*)&value);

	}else
	{
		uint8_t tmpVal[MAX_REG_VALSIZE];
		rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlReadGeneric(NVME_REG_PCI,
											mPciSpcTbl[reg].offset,
											mPciSpcTbl[reg].size,
											sizeof(uint8_t),
											(uint8_t*)&tmpVal);

		uint64_t bitMask	= (1 << bits -1) << offset;
		uint64_t newVal		= ( (uint64_t)(tmpVal) & (~bitMask)	) \
				| ( ( value & (1 << bits -1) ) << offset );


		rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlWriteGeneric(NVME_REG_PCI,
						mPciSpcTbl[reg].offset,
						mPciSpcTbl[reg].size,
						sizeof(uint8_t),
						(uint8_t*)&newVal);
	}

	return 0 == rtn;
}

bool DNVMeRegister::Write(enNVMeCtlSpc reg,uint64_t value,uint8_t bits,uint8_t offset)
{
	if(!ValidSpecRev(mCtlSpcTbl[reg].specRev)){
		return false;
	}

	int rtn = 0;

	if( 0 == bits and 0 == offset){
		rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlWriteGeneric(NVME_REG_PCI,
				mCtlSpcTbl[reg].offset,
				mCtlSpcTbl[reg].size,
				sizeof(uint32_t),
				(uint8_t*)&value);

	}else
	{
		uint8_t tmpVal[MAX_REG_VALSIZE];
		rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlReadGeneric(NVME_REG_PCI,
				mCtlSpcTbl[reg].offset,
				mCtlSpcTbl[reg].size,
				sizeof(uint32_t),
				(uint8_t*)&tmpVal);

		uint64_t bitMask	= (1 << bits -1) << offset;
		uint64_t newVal		= ( (uint64_t)(tmpVal) & (~bitMask)	) \
				| ( ( value & (1 << bits -1) ) << offset );


		rtn = DNVMeIOCtrl::GetInstance(mDevHdlr)->IOCtlWriteGeneric(NVME_REG_PCI,
				mCtlSpcTbl[reg].offset,
				mCtlSpcTbl[reg].size,
				sizeof(uint32_t),
				(uint8_t*)&newVal);
	}

	return 0 == rtn;
}

bool DNVMePCIReg::mInstanceFlag				= false;
DNVMePCIReg* DNVMePCIReg::mInstanceObj	= NULL;

#define ZZ(a, b, c, d, e, f, g, h)			{b, c, d, e, f, g, h},
stNVMePciReg DNVMePCIReg::mPciRegTbl[]		=
{
	PCIREG_TABLE
};
#undef ZZ

DNVMePCIReg* DNVMePCIReg::GetInstance(int devHdlr,enNVMeSpecRev specRev)
{
	if(false == mInstanceFlag)
	{
		mInstanceObj	= new DNVMePCIReg(devHdlr,specRev);
		mInstanceFlag	= true;
	}

	return mInstanceObj;
}

DNVMePCIReg::DNVMePCIReg(int devHdlr,enNVMeSpecRev specRev)
	: DNVMeRegister(devHdlr,specRev)
{
	ScanPCICapRegOffset();
}

bool DNVMePCIReg::GetRegVal(enNVMePCIReg reg,uint32_t &val)
{
	uint64_t tmpVal	= 0;

	bool rtn = Read(mPciRegTbl[reg].spcID,tmpVal,mPciRegTbl[reg].offset,mPciRegTbl[reg].bits);

	if(true == rtn)
	{
		val	= (uint32_t)tmpVal;
	}

	return rtn;
}

bool DNVMePCIReg::SetRegVal(enNVMePCIReg reg,uint32_t val)
{
	if(REG_RW != mPciRegTbl[reg].type) return false;

	uint64_t tmpVal	= val;

	return Write(mPciRegTbl[reg].spcID,tmpVal,mPciRegTbl[reg].offset,mPciRegTbl[reg].bits);
}

bool DNVMePCIReg::ClrRegVal(enNVMePCIReg reg)
{
	if(REG_RO != mPciRegTbl[reg].type) return false;

	if(REG_RWC == mPciRegTbl[reg].type)
		return SetRegVal(reg,1);
	else
		return SetRegVal(reg,0);
}

void DNVMePCIReg::ScanPCICapRegOffset(void)
{
	return;
}
