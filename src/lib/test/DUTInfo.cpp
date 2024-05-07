/*
 * file: DUTInfo.cpp
 */

#include "test/DUTInfo.h"

#include <math.h>
#include <assert.h>

#include "device/nvme/NVMeCommand.h"
#include "utility/ByteHdlr.h"

#define IDFY_BUF_SIZE					4096
#define BUF_ALIGN_SIZE					128
#define NVME_CMD_TIMEOUT				8000

#define IDFY_CNS_IDFY_CTRLR				1
#define IDFY_CNS_IDFY_NSID				0

#define IDFY_CTRLR_SN_OFFSET			4
#define IDFY_CTRLR_SN_LENGTH			20
#define IDFY_CTRLR_MN_OFFSET			24
#define IDFY_CTRLR_MN_LENGTH			40
#define IDFY_CTRLR_FR_OFFSET			64
#define IDFY_CTRLR_FR_LENGTH			8

#define IDFY_NS_NSZE_OFFSET				0
#define IDFY_NS_FLBAS_OFFSET			26
#define IDFY_NS_NLBAF_BITMASK			0x7
#define IDFY_NS_LBAF0_OFFSET			128
#define IDFY_NS_LBAF_LENGTH				4
#define IDFY_NS_LBAF_LBADS_OFFSET		2

int	NVMeDutInfo::GetNSIDFromDevName(const char* devName)
{
	int cntid	= 0;
	int nsid	= 0;

	if( 2 !=sscanf(devName,"nvme%dn%d",&cntid,&nsid) )
	{
		return 1;
	}

	return nsid;
}

NVMeDutInfo::NVMeDutInfo(const char* devName)
{
	mpPciAddr	= NVMeScan().GetPciAddrByDevName(devName);
	mDevNSID	= GetNSIDFromDevName(devName);

	memcpy(mDevName,devName,DUTINFO_DN_LEN);

	assert(mpPciAddr != NULL);

	LOGDEBUG("PciAddr = %s",mpPciAddr->GetString());
	LOGDEBUG("nsid = %d",mDevNSID);
}

NVMeDutInfo::~NVMeDutInfo(void)
{
	if(NULL != mpPciAddr)
	{
		delete mpPciAddr;
		mpPciAddr = NULL;
	}
}

DeviceHandler* NVMeDutInfo::GetDevHdlr(void)
{
	assert(NULL != mpPciAddr);

	NVMeScan s;

	const char* devName = s.GetDevNameByPciAddr(mpPciAddr);
	if(0 >= strlen(devName))
	{
		return NULL;
	}

	return new DeviceHandler(devName);
}

bool NVMeDutInfo::GetInfoFromIdfyCtrlCmd(char dutSN[],char dutMN[],char dutFR[])
{
	bool bRtn	= true;

	DeviceHandler* devHdlr = GetDevHdlr();
	if(NULL == devHdlr)
	{
		LOGERROR("Fail to GetDevHdlr()");
		return false;
	}
	int	fd	= devHdlr->GetDevHdlr();
	if(INVALID_DEVHDLR == fd)
	{
		LOGERROR("Fail to Dev Handler");
		bRtn = false;
	}
	else
	{
		NVMeAdminCmd*	pAdmCmd	= new NVMeAdminCmd(fd);
		Buffers*		buf		= new Buffers(IDFY_BUF_SIZE,BUF_ALIGN_SIZE);

		NVMeCmdResp*	res = pAdmCmd->Identify(IDFY_CNS_IDFY_CTRLR,0,0,buf,NVME_CMD_TIMEOUT);
		if(false == res->Succeeded())
		{
			LOGERROR("Fail to Issue Identify(IDFY_CNS_IDFY_CTRLR) Command");
			bRtn = false;
		}
		else
		{
			uint8_t*	bufAddr	= buf->GetBufferPoint();

			ByteToString(bufAddr+IDFY_CTRLR_SN_OFFSET,dutSN,IDFY_CTRLR_SN_LENGTH);
			ByteToString(bufAddr+IDFY_CTRLR_MN_OFFSET,dutMN,IDFY_CTRLR_MN_LENGTH);
			ByteToString(bufAddr+IDFY_CTRLR_FR_OFFSET,dutFR,IDFY_CTRLR_FR_LENGTH);
		}

		delete buf;
		delete pAdmCmd;
		delete res;
	}

	delete devHdlr;
	return bRtn;
}

bool NVMeDutInfo::GetInfoFromIdfyNmspCmd(uint32_t& dutCap,uint32_t& dutSector)
{
	bool bRtn	= true;

	DeviceHandler* devHdlr = GetDevHdlr();
	if(NULL == devHdlr)
	{
		LOGERROR("Fail to GetDevHdlr()");
		return false;
	}
	int	fd	= devHdlr->GetDevHdlr();
	if(INVALID_DEVHDLR == fd)
	{
		LOGERROR("Fail to Dev Handler");
		bRtn = false;
	}
	else
	{
		NVMeAdminCmd*	pAdmCmd	= new NVMeAdminCmd(fd);
		Buffers*		buf		= new Buffers(IDFY_BUF_SIZE,BUF_ALIGN_SIZE);
		buf->SetEndianMode(BUFV_LITTLE_ENDIAN);

		NVMeCmdResp*	res = pAdmCmd->Identify(IDFY_CNS_IDFY_NSID,0,mDevNSID,buf,NVME_CMD_TIMEOUT);
		if(false == res->Succeeded())
		{
			LOGERROR("Fail to Issue Identify(IDFY_CNS_IDFY_NSID) Command");
			bRtn = false;
		}
		else
		{
			uint64_t nsSize	= buf->GetQWord(IDFY_NS_NSZE_OFFSET);
			uint8_t nLBAF	= (buf->GetByte(IDFY_NS_FLBAS_OFFSET)) & IDFY_NS_NLBAF_BITMASK;
			uint32_t offset = IDFY_NS_LBAF0_OFFSET + nLBAF * IDFY_NS_LBAF_LENGTH + IDFY_NS_LBAF_LBADS_OFFSET;
			uint32_t sectorSize	= pow(2,buf->GetByte(offset));
			uint32_t dutGBSize	= (uint32_t)(((uint64_t)nsSize * sectorSize) / (1000*1000*1000));

			dutSector	= sectorSize;
			dutCap		= dutGBSize;
		}

		delete buf;
		delete pAdmCmd;
		delete res;
	}

	delete devHdlr;

	return bRtn;
}

bool NVMeDutInfo::UpdateDutInfo(void)
{
	return GetDutDevName(mDutInfo.dutDevName) && GetDutPhyAddr(mDutInfo.dutPhyAddr) &&
			GetInfoFromIdfyCtrlCmd(mDutInfo.dutSN,mDutInfo.dutMN,mDutInfo.dutFWRev) &&
			GetInfoFromIdfyNmspCmd(mDutInfo.dutCapGB,mDutInfo.dutSector);
}

bool NVMeDutInfo::GetDutPhyAddr(char dutPhyAddr[])
{
	assert(NULL != mpPciAddr);
	memcpy(dutPhyAddr,mpPciAddr->GetString(),DUTINFO_PA_LEN);

	return true;
}

bool NVMeDutInfo::GetDutDevName(char dutDevName[])
{
	assert(NULL != mpPciAddr);
	memcpy(dutDevName,mDevName,DUTINFO_DN_LEN);

	return true;
}
