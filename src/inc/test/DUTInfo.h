#ifndef _DUT_INFO_H
#define _DUT_INFO_H

#include <string.h>

#include "device/NVMeScan.h"
#include "device/DeviceHandler.h"

#define DUTINFO_SN_LEN				21
#define DUTINFO_MN_LEN				41
#define DUTINFO_FR_LEN				9
#define DUTINFO_PA_LEN				16
#define DUTINFO_DN_LEN				20
#define DUTINFO_GITREV_LEN			9

#define UNDEF_PHY_ADDR				""

typedef struct _stDutInfo
{
	char		dutDevName[DUTINFO_DN_LEN];
	char		dutPhyAddr[DUTINFO_PA_LEN];
	char		dutSN[DUTINFO_SN_LEN];
	char		dutMN[DUTINFO_MN_LEN];
	char		dutFWRev[DUTINFO_FR_LEN];
	char		dutGITRev[DUTINFO_GITREV_LEN];
	uint32_t	dutCapGB;
	uint32_t	dutSector;
}stDutInfo;

class DutInfo
{
protected:
	stDutInfo			mDutInfo;
	virtual bool		UpdateDutInfo(void) {return false;};
public:
	DutInfo(void)
	{
		memset((void*)GetDutInfo(),0,GetDataSize());
	};
	DutInfo(stDutInfo*	pInfo)
	{
		if(NULL != pInfo)
		{
			memcpy((void*)GetDutInfo(),pInfo,GetDataSize());
		}
		else
		{
			memset((void*)GetDutInfo(),0,GetDataSize());
		}
	};
	virtual ~DutInfo() {};

	stDutInfo*	GetDutInfo(bool bRefresh=false)
	{
		if(bRefresh && false == UpdateDutInfo())
		{
			return NULL;
		};

		return &mDutInfo;
	};

	uint32_t	GetDataSize(void) {return sizeof(stDutInfo);};

	const char* GetDevName(void) {return mDutInfo.dutDevName;};
	const char*	GetPhyAddr(void) {return mDutInfo.dutPhyAddr;};
	const char*	GetDutSN(void) {return (const char*)mDutInfo.dutSN;};
	const char* GetDutMN(void) {return (const char*)mDutInfo.dutMN;};
	const char* GetDutFWRev(void) {return (const char*)mDutInfo.dutFWRev;};
	const char* GetDutGitRev(void) {return (const char*)mDutInfo.dutGITRev;};
	uint32_t	GetDutCapGB(void) {return mDutInfo.dutCapGB;};
	uint32_t	GetDutSectorSize(void) {return mDutInfo.dutSector;};
};

class NVMeDutInfo : public DutInfo
{
private:
	PCIAddr*	mpPciAddr;
	int			mDevNSID;
	char		mDevName[DUTINFO_DN_LEN];
protected:
	virtual bool	UpdateDutInfo(void);
	bool	GetDutDevName(char dutDevName[]);
	bool 	GetDutPhyAddr(char dutPhyAddr[]);
	bool	GetInfoFromIdfyCtrlCmd(char dutSN[],char dutMN[],char dutFR[]);
	bool	GetInfoFromIdfyNmspCmd(uint32_t& dutCap,uint32_t& dutSector);
	DeviceHandler*	GetDevHdlr(void);
	int		GetNSIDFromDevName(const char* devName);
public:
	NVMeDutInfo(const char* devName);
	virtual ~NVMeDutInfo(void);
};

#endif
