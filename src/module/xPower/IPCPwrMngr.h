#ifndef _IPC_PWR_MNGR_H
#define _IPC_PWR_MNGR_H

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <assert.h>

#include "power/PowerCtrlr.h"
#include "ipc/MsgProcessComm.h"

#define REQ_T_POWER_ON			0x6001
#define REQ_T_POWER_OFF			0x6002
#define REQ_T_GET_STATUS		0x6003
#define REQ_T_ASYNC_POW_ON		0x6004
#define REQ_T_ASYNC_POW_OFF 	0x6005
#define REQ_T_B2B_POW_CYCLE		0x6006
#define REQ_T_GET_VOL			0x6010
#define REQ_T_GET_CUR			0x6011
#define REQ_T_GET_POW			0x6012
#define REQ_T_GET_FWREV			0x6013
#define REQ_T_MCU_GET_POWID		0x6020
#define REQ_T_MCU_SET_POWID		0x6021
#define REQ_T_MCU_GET_TEMP		0x6022
#define REQ_T_MCU_DEV_DETECT	0x6023
#define REQ_T_MCU_WRITE_EEPROM	0x6024
#define REQ_T_MCU_READ_EEPROM	0x6025
#define REQ_T_MCU_NVMEMI_START	0x6026
#define REQ_T_MCU_NVMEMI_DXFER	0x6027
#define REQ_T_MCU_NVMEMI_READY	0x6028
#define REQ_T_MCU_NVMEMI_DRCVR	0x6029
#define REQ_T_MCU_UPDATEFW		0x602A
#define REQ_T_MCU_DOWNLOADFW	0x602B
#define REQ_T_MCU_RESET			0x602C

#define RESP_E_FAIL_OPEN		-1
#define RESP_E_FAIL_HANDLE		-2
#define RESP_E_FAIL_POWID		-201
#define RESP_E_FAIL_EEPROM		-202
#define RESP_E_FAIL_NVMEMI		-211
#define RESP_E_FAIL_UPDATEFW	-220
#define RESP_E_FAIL_DWLDFW		-221
#define RESP_E_FAIL_RESET		-222

#define PM_MCU_DATA_MAXLEN		40

#define PM_PROC_NAME			"PowerMngr"
#define PM_MAX_CTRLR			7
#define PM_MAX_SLOTS_PER_CTRL	4
#define PM_MAX_SLAVES			(PM_MAX_CTRLR * PM_MAX_SLOTS_PER_CTRL)
#define PM_MAX_REQS				(PM_MAX_SLAVES * 2)

#define PM_VERSION_MAJOR		0
#define PM_VERSION_MINOR		3
#define PM_VERSION_BUILD		0

#define PM_TIMEOUT				8
#define PM_FWREV_LEN			20

typedef enum _enAsyncFunc
{
	ASF_POW_ON,
	ASF_POW_OFF,
	ASF_B2B_POW_CYC,
}enAsyncFunc;

class PwrMngrServer : public MsgProcCommServer
{
private:
	uint8_t				muPwrCtrlMaxIdx;
protected:
	PowerCtrlr*			mpPwrCtlr[PM_MAX_CTRLR];
	boost::thread*		mpCHThread[PM_MAX_SLAVES];

	virtual bool cb_ReceiveAndResponse(stMsgReq& req);
	virtual uint32_t GetVersion(void)
	{
		return (uint32_t)PM_VERSION_MAJOR << 24  |
				(uint32_t)PM_VERSION_MINOR << 16 |
				(uint32_t)PM_VERSION_BUILD;
	};

	PowerCtrlr*			GetPowerCtrlr(uint16_t	slaveID)
	{
		uint8_t			pwrIdx	= slaveID / PM_MAX_SLOTS_PER_CTRL;
		uint8_t			portID	= slaveID % PM_MAX_SLOTS_PER_CTRL;

		assert( PM_MAX_CTRLR > pwrIdx );
		assert( pwrIdx < muPwrCtrlMaxIdx );
		assert( NULL != mpPwrCtlr[pwrIdx] );

		mpPwrCtlr[pwrIdx]->SetPortID(portID);
		return mpPwrCtlr[pwrIdx];
	};

	void _TrdDelayPower(uint16_t slaveID,uint32_t delayTime,bool bPowOn);
	void _TrdB2BPowCycle(uint16_t slaveID,uint16_t nCycles,uint32_t delayPowOff,uint32_t delayPowOn);

public:
	PwrMngrServer(void);
	PwrMngrServer(enPwrCtrlrType ty,const char* port);
	bool AddPwrCtrlr(enPwrCtrlrType ty,const char* port);
	virtual ~PwrMngrServer();
};

class PwrMngrClient : public MsgProcCommClient
{
private:
	char mPwrFWRev[PM_FWREV_LEN];
public:
	PwrMngrClient(uint16_t slaveID)
		:MsgProcCommClient(PM_PROC_NAME,slaveID)
	{
	};

	const char* GetVersion(void);
	bool PowerOn(void);
	bool PowerOff(void);
	bool AsyncPowerOn(uint32_t delayTime);
	bool AsyncPowerOff(uint32_t delayTime);
	bool AsyncB2BPowerCycle(uint16_t nCycles,uint32_t delayPowOff,uint32_t delayPowOn);
	bool WaitForDone(uint32_t timeOut=0);
	bool IsPoweredOn(void);
	uint32_t GetVoltage(void);
	uint32_t GetCurrent(void);
	uint32_t GetPower(void);
};

class SCPIPwrMngrServer : public PwrMngrServer
{
public:
	SCPIPwrMngrServer(void)
		:PwrMngrServer()
	{};
	SCPIPwrMngrServer(const char* port)
		:PwrMngrServer(PCT_SCPI,port)
	{};

	bool AddPwrCtrlr(const char* port){return PwrMngrServer::AddPwrCtrlr(PCT_SCPI,port);};
};

class SCPIPwrMngrClient : public PwrMngrClient
{
public:
	SCPIPwrMngrClient(uint16_t slaveID)
		:PwrMngrClient(slaveID)
	{};
};

class MCUPwrMngrServer : public PwrMngrServer
{
protected:
	virtual bool cb_ReceiveAndResponse(stMsgReq& req);
public:
	MCUPwrMngrServer(void)
		:PwrMngrServer()
	{};
	MCUPwrMngrServer(const char* port)
		:PwrMngrServer(PCT_MCU,port)
	{};

	bool AddPwrCtrlr(const char* port){return PwrMngrServer::AddPwrCtrlr(PCT_MCU,port);};
};

class MCUPwrMngrClient : public PwrMngrClient
{
public:
	MCUPwrMngrClient(uint16_t slaveID)
		:PwrMngrClient(slaveID)
	{};

	bool GetPowerID(uint8_t* pwrID,uint8_t len);
	bool SetPowerID(uint8_t* pwrID,uint8_t len);
	uint32_t GetTemp(void);
	bool DetectDevice(void);
	bool WrEEPROM(uint8_t regAddr,uint16_t offset,uint8_t* data,uint8_t len);
	bool RdEEPROM(uint8_t regAddr,uint16_t offset,uint8_t* data,uint8_t len);

	bool NVMeMIStart(uint8_t xferBytes);
	bool NVMeMIDXfer(uint8_t* data,uint8_t len,uint8_t offset);
	bool NVMeMIReady(uint16_t& recvBytes);
	bool NVMeMIDRcvr(uint8_t* data,uint8_t len,uint8_t offset);

	bool UpdateFW(uint32_t fwSize);
	bool DownloadFW(uint8_t* data,uint8_t len);
	bool Reset(void);
};

#endif
