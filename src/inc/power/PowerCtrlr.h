#ifndef _POWER_CTRLR_H
#define _POWER_CTRLR_H

#include <assert.h>

#include "MCUPwrCtrlr.h"
#include "SCPIPwrCtrlr.h"
#include "Logging.h"

#define ALL_CHANNELS				0xFF
#define INVALID_CHANNEL				0xEE

#define	PWRCTR_PORT0_MCU			0
#define PWRCTR_PORT0_SCPI			1
#define PWRCTR_DEF_PORTID			0

typedef enum _enPwrCtrlrType
{
	PCT_MCU,
	PCT_SCPI
}enPwrCtrlrType;

class PowerCtrlr
{
private:
	uint8_t			mChannel;
	BasicPwrCtrlr*	mpPwrCtrlr;
	enPwrCtrlrType	mPwrCtrlrType;
public:
	PowerCtrlr(enPwrCtrlrType ty,const char* devName,uint8_t portID = PWRCTR_DEF_PORTID)
	{
		switch(ty)
		{
		case PCT_MCU:
			mpPwrCtrlr	= new MCUPwrCtrlr(devName);
			mChannel	= portID + PWRCTR_PORT0_MCU;
			break;
		case PCT_SCPI:
			mpPwrCtrlr	= new SCPIPwrCtrlr(devName);
			mChannel	= portID + PWRCTR_PORT0_SCPI;
			break;
		default:
			LOGERROR("Invalid PowerCtrlr Type");
			assert(0);
			break;
		}

		mPwrCtrlrType	= ty;
	};

	void SetPortID(uint8_t portID)
	{
		if(PCT_MCU == GetPwrType())
		{
			mChannel = portID + PWRCTR_PORT0_MCU;
		}
		else if(PCT_SCPI == GetPwrType())
		{
			mChannel = portID + PWRCTR_PORT0_SCPI;
		}
		else
		{
			LOGERROR("Invalid PowerCtrlr Type");
			assert(0);
		}
	};
	enPwrCtrlrType	GetPwrType(void) {return mPwrCtrlrType;};

	MCUPwrCtrlr*	GetMCUPwrCtrlr(void) {return (MCUPwrCtrlr*)mpPwrCtrlr;};
	SCPIPwrCtrlr*	GetSCPIPwrCtrlr(void) {return (SCPIPwrCtrlr*)mpPwrCtrlr;};

	const char* GetVersion(void) {return mpPwrCtrlr->GetVersion();};
	uint8_t GetPowerStatus(uint8_t ch = INVALID_CHANNEL)
	{
		ch = (INVALID_CHANNEL == ch)?mChannel:ch;
		return mpPwrCtrlr->GetPowerStatus(ch);
	};
	bool PowerOn(uint8_t ch = INVALID_CHANNEL)
	{
		ch = (INVALID_CHANNEL == ch)?mChannel:ch;
		return mpPwrCtrlr->PowerOn(ch);
	};
	bool PowerOff(uint8_t ch = INVALID_CHANNEL)
	{
		ch = (INVALID_CHANNEL == ch)?mChannel:ch;
		return mpPwrCtrlr->PowerOff(ch);
	};
	bool SetVoltage(uint8_t ch = INVALID_CHANNEL,uint32_t vol = 5000)
	{
		ch = (INVALID_CHANNEL == ch)?mChannel:ch;
		return mpPwrCtrlr->SetVoltage(ch,vol);
	};
	uint32_t GetVoltage(uint8_t ch = INVALID_CHANNEL)
	{
		ch = (INVALID_CHANNEL == ch)?mChannel:ch;
		return mpPwrCtrlr->GetVoltage(ch);
	};
	uint32_t GetCurrent(uint8_t ch = INVALID_CHANNEL)
	{
		ch = (INVALID_CHANNEL == ch)?mChannel:ch;
		return mpPwrCtrlr->GetCurrent(ch);
	};
	uint32_t GetPower(uint8_t ch = INVALID_CHANNEL)
	{
		ch = (INVALID_CHANNEL == ch)?mChannel:ch;
		return mpPwrCtrlr->GetPower(ch);
	};
};

#endif
