/*
 * MCUPwrBPDriver.h
 *
 *  Created on: 2020Äê12ÔÂ23ÈÕ
 *      Author: Lenovo
 */

#ifndef _NVME_MI_MCUPWRBPDRIVER_H_
#define _NVME_MI_MCUPWRBPDRIVER_H_

#include "NVMeMIBpDriver.h"
#include "power/MCUPwrCtrlr.h"
#include "Logging.h"

#define MCU_POWRTCTRL_PACKAGE_UNIT			40

class MCUPwrBPDriver : public NVMeMIBpDriver,public MCUPwrCtrlr
{
private:
	uint8_t		mSerialChannel;
public:
	MCUPwrBPDriver()
		:MCUPwrCtrlr(),mSerialChannel(0)
	{};

	MCUPwrBPDriver(const char* port,uint8_t ch)
		:MCUPwrCtrlr(),mSerialChannel(ch)
	{
		ConfigSerial(port);
	};

	virtual uint16_t	GetUnitLength(void)	{return MCU_POWRTCTRL_PACKAGE_UNIT;};

	virtual bool ReadyForWrite(uint16_t xferBytes)
	{
		bool rtn = NVMeMIStart(mSerialChannel,xferBytes);
		LOGINFO("%s = ReadyForWrite(%d)",(true == rtn)?"OK":"ERR",xferBytes);
		return rtn;
	};
	virtual bool Write(uint8_t* data,uint16_t len,uint16_t offset)
	{
		bool rtn = NVMeMIDXfer(mSerialChannel,data,len,offset);
		if(16 < len)
		{
			uint32_t* temp = (uint32_t*)data;
			LOGINFO("%s=Write(%d,%d : %08x %08x %08x %08x)",\
					(true == rtn)?"OK":"ERR",offset,len,\
					*(temp++),*(temp++),*(temp++),*(temp++));
		}
		else
		{
			LOGINFO("%s=Write(%d,%d : %2x)",\
					(true == rtn)?"OK":"ERR",offset,len,*data);
		}

		return rtn;
	};
	virtual bool ReadyForRead(uint16_t& recvBytes)
	{
		bool rtn = NVMeMIReady(mSerialChannel,recvBytes);
		LOGINFO("%s = NVMeMIReady(%d)",(true == rtn)?"OK":"ERR",recvBytes);
		return rtn;
	};
	virtual bool Read(uint8_t* data,uint16_t len,uint16_t offset)
	{
		bool rtn = NVMeMIDRcvr(mSerialChannel,data,len,offset);
		if( true == rtn )
		{
			if(16 < len)
			{
				uint32_t* temp = (uint32_t*)data;
				LOGINFO("%s=Read(%d,%d : %08x %08x %08x %08x)",\
						(true == rtn)?"OK":"ERR",offset,len,\
						*(temp++),*(temp++),*(temp++),*(temp++));
			}
			else
			{
				LOGINFO("%s=Read(%d,%d : %2x)",\
						(true == rtn)?"OK":"ERR",offset,len,*data);
			}
		}
		else
		{
			LOGDEBUG("%s=Read()",(true == rtn)?"OK":"ERR");
		}

		return rtn;
	};
};

#endif /* _NVME_MI_MCUPWRBPDRIVER_H_ */
