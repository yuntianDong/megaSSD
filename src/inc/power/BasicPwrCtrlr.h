#ifndef _BASIC_POWER_CTRLR_H
#define _BASIC_POWER_CTRLR_H

#include <stdint.h>

#define ALL_CHANNELS		0xFF
#define CH_POWER_ON			1
#define CH_POWER_OFF		0
#define CH_POWER_ERROR		0xFF

class BasicPwrCtrlr
{
public:
	BasicPwrCtrlr(void)
	{};
	virtual ~BasicPwrCtrlr(void)
	{};

	virtual const char* GetVersion(void) = 0;
	virtual uint8_t GetPowerStatus(uint8_t ch) = 0;
	virtual bool PowerOn(uint8_t ch = ALL_CHANNELS) = 0;
	virtual bool PowerOff(uint8_t ch = ALL_CHANNELS) = 0;
	virtual bool SetVoltage(uint8_t ch,uint32_t vol) = 0;
	virtual uint32_t GetVoltage(uint8_t ch) = 0;
	virtual uint32_t GetCurrent(uint8_t ch) = 0;
	virtual uint32_t GetPower(uint8_t ch) = 0;
};

#endif
