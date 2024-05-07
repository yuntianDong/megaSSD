#ifndef _SCPI_POWER_CONTROLER_H
#define _SCPI_POWER_CONTROLER_H

#include <boost/thread/mutex.hpp>

#include "BasicPwrCtrlr.h"
#include "serial/BoostSerial.h"

//		COMMAND					ARGS	ARG_CH		ARG_VAL		INSTRUCT
#define SCPI_CMD_TABLE	\
	ZZ(RUN_POWER_UP , 			0	, false,	false,		"RUN:POWer:UP")	\
	ZZ(RUN_POWER_DOWN , 		0	, false,	false,		"RUN:POWer:DOWN")	\
	ZZ(RUN_POWER_REQ , 			0	, false,	false,		"RUN:POWer:?")	\
	ZZ(RUN_POWER_CHANNEL_SET , 	1	, false,	true,		"RUN:POWer:Channels:%d")	\
	ZZ(RUN_POWER_CHANNEL_REQ , 	0	, false,	false,		"RUN:POWer:Channels:?")	\
	ZZ(RUN_SAVE_CHANNEL , 		1	, false,	true,		"RUN:Save:Channels:%d")	\
	ZZ(SIGNAL_CH_VOLTAGE_SET , 	2	, true,		true,		"SIGnal:CH%d:VOLTage:%d")	\
	ZZ(SIGNAL_CH_VOLTAGE_REQ , 	1	, true,		false,		"SIGnal:CH%d:VOLTage:?")	\
	ZZ(MESURE_VOLTAGE_CH , 		1	, true,		false,		"MEASure:VOLTage:CH%d:?")	\
	ZZ(MESURE_CURRENT_CH , 		1	, true,		false,		"MEASure:CURrent:CH%d:?")	\
	ZZ(MESURE_POWER_CH , 		1	, true,		false,		"MEASure:POWer:CH%d:?")	\
	ZZ(MESURE_OUTPUT , 			0	, false,	false,		"MEASure:OUTputs:?")	\
	ZZ(VERSION_CMD , 			0	, false,	false,		"VERSion")	\
	ZZ(SET_OPEN_CHANNEL , 		1	, true,		false,		"Channel:Set:Open:CH%d")	\
	ZZ(SET_CLOSE_CHANNEL, 		1	, true,		false,		"Channel:Set:Close:CH%d")	\
	ZZ(GET_CHANNEL , 			1	, true,		false,		"Channel:Get:CH%d")

#define ZZ(a,b,c,d,e)		a,
typedef enum _enSCPICMD
{
	SCPI_CMD_TABLE
}SCPICMD;
#undef ZZ

#define	ENCODE_STRLEN		10
#define UART_DELIMS_STRLEN	10
#define RESP_STR_MAXLEN		80

#define	DEF_ENCODE_FMT		"UTF-8"
#define UART_READ_DELIMS	"#"

#define VOL_RESP_FORMAT		"%d"
#define CUR_RESP_FORMAT		"%d"
#define POW_RESP_FORMAT		"%d"

#define DEF_CH_BITMASK		0xFFFFFFFF

#define CTRLR_POWER_ON		"UP"
#define CTRLR_POWER_OFF		"DOWN"
#define CTRLR_CH_OPEN		"OPEN"
#define CTRLR_CH_CLOSE		"CLOSE"
#define SCPI_OP_SUCCESS		"SUCCESS"
#define SCPI_OP_FAILURE		"FAILURE"
#define SCPI_OP_TODO		"NOTIMPLEMENTED"

typedef struct _stSCPICmdDesc
{
	SCPICMD		cmdID;
	uint8_t		nargs;
	bool		hasArgCH;
	bool		hasArgVal;
	const char	*cmdFmt;
}stSCPICmdDesc;

class _FuncLock
{
private:
	boost::mutex* 	mpLock;
public:
	_FuncLock(boost::mutex& lock)
	{
		mpLock = &lock;
		mpLock->lock();
	};

	virtual ~_FuncLock(void)
	{
		mpLock->unlock();
	};
};

class SCPIPwrCtrlr : public BasicPwrCtrlr
{
private:
	char			mEncoding[ENCODE_STRLEN];
	char			mSerDelims[UART_DELIMS_STRLEN];
	char			mResponse[RESP_STR_MAXLEN];
	uint32_t		mCHBitMask;

	BoostSerial* 	mpSerial;
	boost::mutex 	mtx_;
protected:
	static	stSCPICmdDesc mSCPICmdTbl[];

	bool SendCommand(SCPICMD cmd,uint32_t ch=0,uint32_t val=0);
	bool ReadResponse(void);
	uint8_t GetCtrlrPowerStatus(void);
	bool CtrlrPowerOn(void);
	bool CtrlrPowerOff(void);
	uint32_t GetCHBitMask(void);

	bool IsChannelOn(uint8_t ch) {return 0 != (mCHBitMask & (1 << (ch-1)));};
	uint32_t GetCHMask(uint8_t ch,bool reversed=false)
	{
		uint32_t val = 0;

		if(0 != ch)
		{
			val = (uint32_t)(1 << (ch-1));
		}

		if(true == reversed)
		{
			val = ~val;
		}

		return val;
	};
public:
	SCPIPwrCtrlr(const char* port);
	virtual ~SCPIPwrCtrlr(void);

	void SetCmdEncoding(const char* encoding) {strncpy(mEncoding,encoding,ENCODE_STRLEN);};
	void SetUartDelims(const char* delims) {strncpy(mSerDelims,delims,UART_DELIMS_STRLEN);};

	const char* GetVersion(void);

	uint8_t GetPowerStatus(uint8_t ch);
	bool PowerOn(uint8_t ch = ALL_CHANNELS);
	bool PowerOff(uint8_t ch = ALL_CHANNELS);
	bool SetVoltage(uint8_t ch,uint32_t vol) {return false;};
	uint32_t GetVoltage(uint8_t ch);
	uint32_t GetCurrent(uint8_t ch);
	uint32_t GetPower(uint8_t ch);
};

#endif
