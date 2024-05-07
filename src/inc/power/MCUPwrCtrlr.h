#ifndef _MCU_PWR_CTRLR_H
#define _MCU_PWR_CTRLR_H

#include <assert.h>
#include <boost/thread/mutex.hpp>

#include "BasicPwrCtrlr.h"
#include "serial/BoostSerial.h"

#include "MPCtrlReq.h"
#include "MPCtrlResp.h"

#ifndef UNUSED
#define UNUSED(expr) do { (void)(expr); } while (0)
#endif

#define	MCU_PWRCTRLR_CH_MAX		4
#define MCU_RESP_RTN_SIZE_MIN	MCP_RESP_NON_DATA_LEN
#define MCU_RESP_RTN_SIZE_MAX	64

#define MCP_PM_FWREV_LEN		10

class MCUPwrCtrlr : public BasicPwrCtrlr
{
private:
	BoostSerial* 	mpSerial;
	boost::mutex 	mtx_;
	uint8_t			mCHPwrBitMask;
	char			mFWRev[MCP_PM_FWREV_LEN];

	void	SetCHBit(uint8_t ch) {mCHPwrBitMask |= (1 << ch);};
	void	ClrCHBit(uint8_t ch) {mCHPwrBitMask &= (~(1 << ch));};
	bool	ChkCHBit(uint8_t ch) {return 0 < (mCHPwrBitMask &= (1 << ch));};

	MPCtrlResp*		HandleMCPReq(MPCtrlReq& req);
	bool			HandleReqAndCheckResp(MPCtrlReq& req,uint8_t* data,uint8_t dlen);
	bool			CheckAndGetRespData(MPCtrlResp* resp,uint8_t* data,uint8_t dlen);
	bool			SendMCPReq(MPCtrlReq& req);

	bool	DoSingleCHPower(uint8_t ch,bool bPowerOn);
public:
	MCUPwrCtrlr()
		:BasicPwrCtrlr(),mCHPwrBitMask(0),mpSerial(NULL)
	{};
	MCUPwrCtrlr(const char* port);
	virtual ~MCUPwrCtrlr(void);

	void ConfigSerial(const char* port);

	uint8_t GetPowerStatus(uint8_t ch)
	{
		assert(ch < MCU_PWRCTRLR_CH_MAX);
		return (true == ChkCHBit(ch))?CH_POWER_ON:CH_POWER_OFF;
	};

	const char* GetVersion(void)
	{
		if(0 == strlen(mFWRev))
		{
			GetFWRev((uint8_t*)mFWRev,MCP_PM_FWREV_LEN);
		}

		mFWRev[MCP_PM_FWREV_LEN-1] = '\0';
		return (const char*)mFWRev;
	};

	bool PowerOn(uint8_t ch = ALL_CHANNELS);
	bool PowerOff(uint8_t ch = ALL_CHANNELS);
	bool SetVoltage(uint8_t ch,uint32_t vol){UNUSED(ch);UNUSED(vol);return false;};
	uint32_t GetVoltage(uint8_t ch);
	uint32_t GetCurrent(uint8_t ch);
	uint32_t GetPower(uint8_t ch);
	bool GetPowerID(uint8_t* pwrID,uint8_t len);
	bool SetPowerID(uint8_t* pwrID,uint8_t len);
	uint32_t GetTemp(uint8_t ch);
	bool DetectDevice(uint8_t ch);
	bool GetFWRev(uint8_t* fwRev,uint8_t len);
	bool WrEEPROM(uint8_t ch,uint8_t regAddr,uint16_t offset,uint8_t* data,uint8_t len);
	bool RdEEPROM(uint8_t ch,uint8_t regAddr,uint16_t offset,uint8_t* data,uint8_t len);
	bool NVMeMIStart(uint8_t ch,uint8_t xferBytes);
	bool NVMeMIDXfer(uint8_t ch,uint8_t* data,uint8_t len,uint8_t offset);
	bool NVMeMIReady(uint8_t ch,uint16_t& recvBytes);
	bool NVMeMIDRcvr(uint8_t ch,uint8_t* data,uint8_t len,uint8_t offset);
	bool UpdateFW(uint32_t fwSize);
	bool DownloadFW(uint8_t* data,uint8_t len);
	bool Reset(void);
};

#endif
