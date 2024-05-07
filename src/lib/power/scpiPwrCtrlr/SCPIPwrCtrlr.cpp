/*
 * file : SCPIPwrCtrlr.cpp
 */

#include "power/SCPIPwrCtrlr.h"
#include "Logging.h"

#define ZZ(a, b, c, d, e)	{a, b, c, d, e},
stSCPICmdDesc SCPIPwrCtrlr::mSCPICmdTbl[]	=
{
		SCPI_CMD_TABLE
};
#undef ZZ

#define	SCPI_CMD_MAXLEN			80

SCPIPwrCtrlr::SCPIPwrCtrlr(const char* port)
	:BasicPwrCtrlr()
{
	strncpy(mEncoding,DEF_ENCODE_FMT,ENCODE_STRLEN);
	strncpy(mSerDelims,UART_READ_DELIMS,UART_DELIMS_STRLEN);
	strncpy(mResponse,"",RESP_STR_MAXLEN);

	mpSerial = new BoostSerial(port);

	mpSerial->Configure();
	CtrlrPowerOn();
	mCHBitMask	= GetCHBitMask();

	LOGDEBUG("mCHBitMask = %x",mCHBitMask);
}


SCPIPwrCtrlr::~SCPIPwrCtrlr(void)
{
	if(NULL != mpSerial)
	{
		delete mpSerial;
		mpSerial = NULL;
	}
}


bool SCPIPwrCtrlr::SendCommand(SCPICMD cmd,uint32_t ch,uint32_t val)
{
	stSCPICmdDesc* cmdDesc	= &(mSCPICmdTbl[cmd]);
	if(0 == cmdDesc->nargs)
	{
		return mpSerial->Write(cmdDesc->cmdFmt,mEncoding);
	}
	else
	{
		char cmdStr[SCPI_CMD_MAXLEN];

		if(2 == cmdDesc->nargs)
		{
			snprintf(cmdStr,SCPI_CMD_MAXLEN,cmdDesc->cmdFmt,ch,val);
		}
		else if(true == cmdDesc->hasArgVal)
		{
			snprintf(cmdStr,SCPI_CMD_MAXLEN,cmdDesc->cmdFmt,val);
		}
		else
		{
			snprintf(cmdStr,SCPI_CMD_MAXLEN,cmdDesc->cmdFmt,ch);
		}

		return mpSerial->Write(cmdStr,mEncoding);
	}

	return false;
}


bool SCPIPwrCtrlr::ReadResponse(void)
{
	return mpSerial->ReadUtil(mResponse,RESP_STR_MAXLEN,mSerDelims,mEncoding);
}


uint8_t SCPIPwrCtrlr::GetCtrlrPowerStatus(void)
{
	LOGDEBUG("GetCtrlrPowerStatus");

	uint8_t rtnCode	= CH_POWER_ERROR;

	if(false == SendCommand(RUN_POWER_REQ))
	{
		rtnCode = CH_POWER_ERROR;
	}

	if(false == ReadResponse())
	{
		rtnCode = CH_POWER_ERROR;
	}

	if(0 == strncmp(mResponse,SCPI_OP_FAILURE,strlen(SCPI_OP_FAILURE)))
	{
		rtnCode = CH_POWER_ERROR;
	}
	else if(0 == strncmp(mResponse,CTRLR_POWER_ON,strlen(CTRLR_POWER_ON)))
	{
		rtnCode = CH_POWER_ON;
	}
	else if(0 == strncmp(mResponse,CTRLR_POWER_OFF,strlen(CTRLR_POWER_OFF)))
	{
		rtnCode = CH_POWER_OFF;
	}
	else{
		rtnCode = CH_POWER_ERROR;
	}

	return rtnCode;
}


bool SCPIPwrCtrlr::CtrlrPowerOn(void)
{
	LOGDEBUG("CtrlrPowerOn");

	if(CH_POWER_ON == GetCtrlrPowerStatus())
	{
		return true;
	}

	if(false == SendCommand(RUN_POWER_UP))
	{
		return false;
	}

	if(0 != strncmp(mResponse,SCPI_OP_SUCCESS,strlen(SCPI_OP_SUCCESS)))
	{
		return false;
	}

	return true;
}


bool SCPIPwrCtrlr::CtrlrPowerOff(void)
{
	LOGDEBUG("CtrlrPowerOff");

	if(CH_POWER_OFF == GetCtrlrPowerStatus())
	{
		return true;
	}

	if(false == SendCommand(RUN_POWER_DOWN))
	{
		return false;
	}

	if(0 != strncmp(mResponse,SCPI_OP_SUCCESS,strlen(SCPI_OP_SUCCESS)))
	{
		return false;
	}

	return true;
}


uint32_t SCPIPwrCtrlr::GetCHBitMask(void)
{
	LOGDEBUG("GetCHBitMask");

	if(CH_POWER_ON != GetCtrlrPowerStatus())
	{
		return 0;
	}

	if(false == SendCommand(RUN_POWER_CHANNEL_REQ))
	{
		return 0;
	}

	uint32_t chBitMask	= 0;
	if(false == ReadResponse() || 1 != sscanf(mResponse,"%d",&chBitMask))
	{
		return 0;
	}

	return chBitMask;
}


const char* SCPIPwrCtrlr::GetVersion(void)
{
	LOGDEBUG("GetVersion");

	_FuncLock lock(mtx_);

	if(false == SendCommand(VERSION_CMD))
	{
		return "ERROR";
	}

	if(false == ReadResponse())
	{
		return "ERROR";
	}

	return mResponse;
}


uint8_t SCPIPwrCtrlr::GetPowerStatus(uint8_t ch)
{
	LOGDEBUG("GetPowerStatus");

	uint32_t chBitMask	= mCHBitMask;

	if(0 == ( chBitMask & GetCHMask(ch) ) )
	{
		return CH_POWER_OFF;
	}

	return CH_POWER_ON;
}


bool SCPIPwrCtrlr::PowerOn(uint8_t ch)
{
	LOGDEBUG("%d PowerOn",ch);

	_FuncLock lock(mtx_);

	uint32_t chBitMask	= mCHBitMask;
	if(ALL_CHANNELS == ch || 0 == ch)
	{
		chBitMask	= DEF_CH_BITMASK;
	}
	else if(true == IsChannelOn(ch))
	{
		return true;
	}
	else
	{
		chBitMask	|= GetCHMask(ch);
	}

	LOGDEBUG("chBitMask = %x",chBitMask);
	if(false == SendCommand(RUN_POWER_CHANNEL_SET,0,chBitMask))
	{
		LOGERROR("SendCommand Failed");
		return false;
	}

	if(false == ReadResponse())
	{
		LOGERROR("ReadResponse Failed");
		return false;
	}

	if(0 != strncmp(mResponse,SCPI_OP_SUCCESS,strlen(SCPI_OP_SUCCESS)))
	{
		return false;
	}

	mCHBitMask = chBitMask;

	return true;
}


bool SCPIPwrCtrlr::PowerOff(uint8_t ch)
{
	LOGDEBUG("%d PowerOff",ch);

	_FuncLock lock(mtx_);

	uint32_t chBitMask	= mCHBitMask;
	if(ALL_CHANNELS == ch || 0 == ch)
	{
		chBitMask	= 0;
	}
	else if(false == IsChannelOn(ch))
	{
		return true;
	}
	else
	{
		chBitMask	&= GetCHMask(ch,true);
	}

	LOGDEBUG("chBitMask = %x",chBitMask);
	if(false == SendCommand(RUN_POWER_CHANNEL_SET,0,chBitMask))
	{
		LOGERROR("SendCommand Failed");
		return false;
	}

	if(false == ReadResponse())
	{
		LOGERROR("ReadResponse Failed");
		return false;
	}

	if(0 != strncmp(mResponse,SCPI_OP_SUCCESS,strlen(SCPI_OP_SUCCESS)))
	{
		return false;
	}

	mCHBitMask = chBitMask;

	return true;
}


uint32_t SCPIPwrCtrlr::GetVoltage(uint8_t ch)
{
	LOGDEBUG("GetVoltage");

	_FuncLock lock(mtx_);

	//if(CH_POWER_ON != GetPowerStatus(ch))
	//{
	//	LOGERROR("Channel:%d is OFF",ch);
	//	return -1;
	//}

	if(false == SendCommand(MESURE_VOLTAGE_CH,ch))
	{
		LOGERROR("SendCommand Failed");
		return -1;
	}

	if(false == ReadResponse())
	{
		LOGERROR("ReadResponse Failed");
		return -1;
	}

	uint32_t val= 0;
	if(1 != sscanf(mResponse,VOL_RESP_FORMAT,&val))
	{
		LOGERROR("Parse Response Failed");
		return -1;
	}

	return val;
}


uint32_t SCPIPwrCtrlr::GetCurrent(uint8_t ch)
{
	LOGDEBUG("GetCurrent");

	_FuncLock lock(mtx_);

	//if(CH_POWER_ON != GetPowerStatus(ch))
	//{
	//	LOGERROR("Channel:%d is OFF",ch);
	//	return -1;
	//}

	if(false == SendCommand(MESURE_CURRENT_CH,ch))
	{
		LOGERROR("SendCommand Failed");
		return -1;
	}

	if(false == ReadResponse())
	{
		LOGERROR("ReadResponse Failed");
		return -1;
	}

	uint32_t val = 0;
	if(1 != sscanf(mResponse,CUR_RESP_FORMAT,&val))
	{
		LOGERROR("Parse Response Failed");
		return -1;
	}

	return val;
}


uint32_t SCPIPwrCtrlr::GetPower(uint8_t ch)
{
	LOGDEBUG("GetPower");

	_FuncLock lock(mtx_);

	//if(CH_POWER_ON != GetPowerStatus(ch))
	//{
	//	LOGERROR("Channel:%d is OFF",ch);
	//	return -1;
	//}

	if(false == SendCommand(MESURE_POWER_CH,ch))
	{
		LOGERROR("SendCommand Failed");
		return -1;
	}

	if(false == ReadResponse())
	{
		LOGERROR("ReadResponse Failed");
		return -1;
	}

	uint32_t val = 0;
	if(1 != sscanf(mResponse,POW_RESP_FORMAT,&val))
	{
		LOGERROR("Parse Response Failed");
		return -1;
	}

	return val;
}

