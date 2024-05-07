/*
 *  * file : IPCPwrMngr.cpp
 *   */

#include <unistd.h>

#include "IPCPwrMngr.h"
#include "Logging.h"

PwrMngrServer::PwrMngrServer(void)
	:MsgProcCommServer(PM_PROC_NAME,PM_MAX_REQS,PM_MAX_SLAVES),muPwrCtrlMaxIdx(0)
{
	memset(mpPwrCtlr,0,sizeof(PowerCtrlr*)*PM_MAX_CTRLR);
	memset(mpCHThread,0,sizeof(boost::thread*)*PM_MAX_SLAVES);
}

PwrMngrServer::PwrMngrServer(enPwrCtrlrType ty,const char* port)
	:MsgProcCommServer(PM_PROC_NAME,PM_MAX_REQS,PM_MAX_SLAVES),muPwrCtrlMaxIdx(0)
{
	memset(mpPwrCtlr,0,sizeof(PowerCtrlr*)*PM_MAX_CTRLR);
	memset(mpCHThread,0,sizeof(boost::thread*)*PM_MAX_SLAVES);

	AddPwrCtrlr(ty,port);
}

PwrMngrServer::~PwrMngrServer()
{
	for(int idx=0;idx<PM_MAX_SLAVES;idx++)
	{
		if(NULL != mpCHThread[idx])
		{
			mpCHThread[idx]->join();
			delete mpCHThread[idx];
			mpCHThread[idx] = NULL;
		}
	}

	for(int idx=0;idx<PM_MAX_CTRLR;idx++)
	{
		if(NULL != mpPwrCtlr[idx])
		{
			delete mpPwrCtlr[idx];
			mpPwrCtlr[idx] = NULL;
		}
	}
}

bool PwrMngrServer::AddPwrCtrlr(enPwrCtrlrType ty,const char* port)
{
	PowerCtrlr*	 pc	= new PowerCtrlr(ty,port);
	const char* ver = pc->GetVersion();
	if("ERROR" == ver)
	{
		pc = NULL;
		return false;
	}

	mpPwrCtlr[muPwrCtrlMaxIdx++]	= pc;
	LOGDEBUG("PowerMngr Version %s",ver);
	return true;
}

void PwrMngrServer::_TrdDelayPower(uint16_t	slaveID,uint32_t delayTime,bool bPowOn)
{
	bool rslt = true;
	char* mem	= static_cast<char*>(mpMapRegion->get_address());
	stMsgResp*	resp = (stMsgResp*)(mem+mRespSize*slaveID);

	usleep(delayTime*1000);

	if(true == bPowOn)
	{
		rslt = GetPowerCtrlr(slaveID)->PowerOn();
	}
	else
	{
		rslt = GetPowerCtrlr(slaveID)->PowerOff();
	}

	WaitReqReceived(slaveID,REQ_T_GET_RETURN_DATA);

	resp->errorCode		= (true == rslt)?RESP_E_SUCCESS:RESP_E_FAIL_HANDLE;
	resp->version		= GetVersion();
	resp->slaveID		= slaveID;
	resp->reqType		= REQ_T_ASYNC_FINISH;
}

void PwrMngrServer::_TrdB2BPowCycle(uint16_t slaveID,uint16_t nCycles,uint32_t delayPOff,uint32_t delayPOn)
{
	bool rslt = true;
	char* mem	= static_cast<char*>(mpMapRegion->get_address());
	stMsgResp*	resp = (stMsgResp*)(mem+mRespSize*slaveID);

	if(CH_POWER_ON == GetPowerCtrlr(slaveID)->GetPowerStatus())
	{
		rslt = GetPowerCtrlr(slaveID)->PowerOff();

		if(true == rslt)
		{
			usleep(delayPOff*1000);
		}
	}

	for(uint16_t idx=0;idx<nCycles && true == rslt;idx++)
	{
		rslt = GetPowerCtrlr(slaveID)->PowerOn();

		if(true == rslt)
		{
			usleep(delayPOn*1000);

			rslt = GetPowerCtrlr(slaveID)->PowerOff();

			if(true == rslt)
			{
				usleep(delayPOff*1000);
			}
		}

		*(uint16_t*)&(resp->respDatum[0])	= idx;
	}

	WaitReqReceived(slaveID,REQ_T_GET_RETURN_DATA);

	resp->errorCode		= (true == rslt)?RESP_E_SUCCESS:RESP_E_FAIL_HANDLE;
	resp->version		= GetVersion();
	resp->slaveID		= slaveID;
	resp->reqType		= REQ_T_ASYNC_FINISH;
}

bool PwrMngrServer::cb_ReceiveAndResponse(stMsgReq& req)
{
	uint16_t	slaveID	= req.slaveID;
	uint16_t	reqType = req.reqType;

	LOGDEBUG("%04d - %x",slaveID,reqType);

	if(slaveID >= mMaxSlaves)
	{
		LOGERROR("Invalid SlaveID");
		return false;
	}

	char* mem	= static_cast<char*>(mpMapRegion->get_address());
	stMsgResp*	resp = (stMsgResp*)(mem+mRespSize*slaveID);
	memset(resp,0,sizeof(stMsgResp));

	if(NULL == GetPowerCtrlr(slaveID))
	{
		resp->errorCode	= RESP_E_FAIL_OPEN;
		resp->dataSize	= 0;

		return false;
	}

	switch(reqType)
	{
		case REQ_T_POWER_ON:
		{
			bool rslt = GetPowerCtrlr(slaveID)->PowerOn();

			resp->errorCode	= (true == rslt)?RESP_E_SUCCESS:RESP_E_FAIL_HANDLE;
			resp->dataSize	= 0;
			break;
		}
		case REQ_T_POWER_OFF:
		{
			bool rslt = GetPowerCtrlr(slaveID)->PowerOff();

			resp->errorCode	= (true == rslt)?RESP_E_SUCCESS:RESP_E_FAIL_HANDLE;
			resp->dataSize	= 0;
			break;
		}
		case REQ_T_ASYNC_POW_ON:
		{
			uint32_t delayTime	= *(uint32_t*)(&(req.reqDatum[0]));

			resp->errorCode		= RESP_E_ASYNC_REQ;
			boost::thread* pThrd = mpCHThread[slaveID];
			if(NULL != pThrd)
			{
				pThrd->join();
				delete pThrd;
				pThrd = NULL;
			}

			mpCHThread[slaveID] = (boost::thread*)new boost::thread(boost::bind(&PwrMngrServer::_TrdDelayPower,
					this,slaveID,delayTime,true));

			break;
		}
		case REQ_T_ASYNC_POW_OFF:
		{
			uint32_t delayTime	= *(uint32_t*)(&(req.reqDatum[0]));

			resp->errorCode		= RESP_E_ASYNC_REQ;
			boost::thread* pThrd = mpCHThread[slaveID];
			if(NULL != pThrd)
			{
				pThrd->join();
				delete pThrd;
				pThrd = NULL;
			}

			mpCHThread[slaveID] = (boost::thread*)new boost::thread(boost::bind(&PwrMngrServer::_TrdDelayPower,
					this,slaveID,delayTime,false));

			break;
		}
		case REQ_T_B2B_POW_CYCLE:
		{
			uint16_t nCycles	= *(uint16_t*)(&(req.reqDatum[0]));
			uint32_t delayPOff	= *(uint32_t*)(&(req.reqDatum[2]));
			uint32_t delayPOn	= *(uint32_t*)(&(req.reqDatum[6]));

			resp->errorCode		= RESP_E_ASYNC_REQ;
			resp->dataSize		= 1;
			resp->respDatum[0]	= 0;

			boost::thread* pThrd = mpCHThread[slaveID];
			if(NULL != pThrd)
			{
				pThrd->join();
				delete pThrd;
				pThrd = NULL;
			}

			mpCHThread[slaveID] = (boost::thread*)new boost::thread(boost::bind(\
					&PwrMngrServer::_TrdB2BPowCycle,this,slaveID,nCycles,delayPOff,delayPOn));

			break;
		}
		case REQ_T_GET_STATUS:
		{
			uint8_t rslt = GetPowerCtrlr(slaveID)->GetPowerStatus();

			resp->dataSize	= 1;
			resp->respDatum[0]	= rslt;

			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_GET_FWREV:
		{
			const char* fwRev = GetPowerCtrlr(slaveID)->GetVersion();
			resp->dataSize	= strlen(fwRev) + 1;
			memcpy(resp->respDatum,fwRev,resp->dataSize);

			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_GET_VOL:
		{
			uint32_t val = GetPowerCtrlr(slaveID)->GetVoltage();

			memcpy(resp->respDatum,(uint8_t*)&val,sizeof(uint32_t));
			resp->dataSize	= sizeof(uint32_t);

			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_GET_CUR:
		{
			uint32_t val = GetPowerCtrlr(slaveID)->GetCurrent();

			memcpy(resp->respDatum,(uint8_t*)&val,sizeof(uint32_t));
			resp->dataSize	= sizeof(uint32_t);

			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_GET_POW:
		{
			uint32_t val = GetPowerCtrlr(slaveID)->GetPower();

			memcpy(resp->respDatum,(uint8_t*)&val,sizeof(uint32_t));
			resp->dataSize	= sizeof(uint32_t);

			resp->errorCode	= RESP_E_SUCCESS;
			break;
		}
		default:
		{
			return MsgProcCommServer::cb_ReceiveAndResponse(req);
		}
	}

	resp->version	= GetVersion();
	resp->slaveID	= slaveID;
	resp->reqType	= reqType;

	return true;
}

bool PwrMngrClient::PowerOn(void)
{
	return SendSyncRequest(REQ_T_POWER_ON,NULL,NULL,0,0,PM_TIMEOUT);
}

bool PwrMngrClient::PowerOff(void)
{
	return SendSyncRequest(REQ_T_POWER_OFF,NULL,NULL,0,0,PM_TIMEOUT);
}

bool PwrMngrClient::AsyncPowerOn(uint32_t delayTime)
{
	return SendAsyncRequest(REQ_T_ASYNC_POW_ON,(uint8_t*)&delayTime,sizeof(uint32_t),PM_TIMEOUT);
}

bool PwrMngrClient::AsyncPowerOff(uint32_t delayTime)
{
	return SendAsyncRequest(REQ_T_ASYNC_POW_OFF,(uint8_t*)&delayTime,sizeof(uint32_t),PM_TIMEOUT);
}

bool PwrMngrClient::AsyncB2BPowerCycle(uint16_t nCycles,uint32_t delayPowOff,uint32_t delayPowOn)
{
	uint8_t datum[10];
	*(uint16_t*)&(datum[0])	= nCycles;
	*(uint32_t*)&(datum[2])	= delayPowOff;
	*(uint32_t*)&(datum[6])	= delayPowOn;

	return SendAsyncRequest(REQ_T_B2B_POW_CYCLE,datum,10,PM_TIMEOUT);
}

bool PwrMngrClient::WaitForDone(uint32_t timeOut)
{
	return WaitAsyncDone(NULL,0,timeOut);
}

bool PwrMngrClient::IsPoweredOn(void)
{
	uint8_t result = 0xFF;
	if(false == SendSyncRequest(REQ_T_GET_STATUS,NULL,&result,0,1,PM_TIMEOUT))
	{
		return false;
	}

	return result == CH_POWER_ON;
}

uint32_t PwrMngrClient::GetVoltage(void)
{
	uint32_t val = (uint32_t)-1;
	if(false == SendSyncRequest(REQ_T_GET_VOL,NULL,&val,0,sizeof(uint32_t),PM_TIMEOUT))
	{
		LOGERROR("Fail to SendSyncRequest(REQ_T_GET_VOL)");
	}

	return val;
}

uint32_t PwrMngrClient::GetCurrent(void)
{
	uint32_t val = (uint32_t)-1;
	if(false == SendSyncRequest(REQ_T_GET_CUR,NULL,&val,0,sizeof(uint32_t),PM_TIMEOUT))
	{
		LOGERROR("Fail to SendSyncRequest(REQ_T_GET_CUR)");
	}

	return val;
}

uint32_t PwrMngrClient::GetPower(void)
{
	uint32_t val = (uint32_t)-1;
	if(false == SendSyncRequest(REQ_T_GET_POW,NULL,&val,0,sizeof(uint32_t),PM_TIMEOUT))
	{
		LOGERROR("Fail to SendSyncRequest(REQ_T_GET_POW)");
	}

	return val;
}

const char* PwrMngrClient::GetVersion(void)
{
	memset(mPwrFWRev,0,PM_FWREV_LEN);

	if(false == SendSyncRequest(REQ_T_GET_FWREV,NULL,mPwrFWRev,0,PM_FWREV_LEN,PM_TIMEOUT))
	{
		LOGERROR("Fail to SendSyncRequest(REQ_T_GET_FWREV)");
	}

	return (const char*)mPwrFWRev;
}

bool MCUPwrMngrServer::cb_ReceiveAndResponse(stMsgReq& req)
{
	uint16_t	slaveID	= req.slaveID;
	uint16_t	reqType = req.reqType;
	uint8_t		portID	= slaveID;

	LOGDEBUG("%04d - %x",slaveID,reqType);

	if(slaveID >= mMaxSlaves)
	{
		LOGERROR("Invalid SlaveID");
		return false;
	}

	char* mem	= static_cast<char*>(mpMapRegion->get_address());
	stMsgResp*	resp = (stMsgResp*)(mem+mRespSize*slaveID);
	memset(resp,0,sizeof(stMsgResp));

	if(NULL == GetPowerCtrlr(slaveID))
	{
		resp->errorCode	= RESP_E_FAIL_OPEN;
		resp->dataSize	= 0;

		return false;
	}

	MCUPwrCtrlr* pMCUPwrCtrlr	= GetPowerCtrlr(slaveID)->GetMCUPwrCtrlr();

	switch(reqType)
	{
		case REQ_T_MCU_GET_POWID:
		{
			bool bRtn = pMCUPwrCtrlr->GetPowerID(resp->respDatum,MPC_REQ_DLEN_POWERID);
			resp->dataSize	= MPC_REQ_DLEN_POWERID;
			resp->errorCode	= (true == bRtn)?RESP_E_SUCCESS:RESP_E_FAIL_POWID;
			break;
		}
		case REQ_T_MCU_SET_POWID:
		{
			bool bRtn = pMCUPwrCtrlr->SetPowerID(req.reqDatum,req.cmdSize);
			resp->errorCode	= (true == bRtn)?RESP_E_SUCCESS:RESP_E_FAIL_POWID;
			break;
		}
		case REQ_T_MCU_GET_TEMP:
		{
			uint32_t tempVal	= pMCUPwrCtrlr->GetTemp(portID);
			resp->dataSize		= sizeof(uint32_t);
			*(uint32_t*)resp->respDatum		= tempVal;
			resp->errorCode		= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_MCU_DEV_DETECT:
		{
			bool bDetected		= pMCUPwrCtrlr->DetectDevice(portID);
			resp->dataSize		= 1;
			resp->respDatum[0]	= (true == bDetected)?1:0;
			resp->errorCode		= RESP_E_SUCCESS;
			break;
		}
		case REQ_T_MCU_WRITE_EEPROM:
		{
			uint8_t regAddr	= *(uint8_t*)req.reqDatum;
			uint8_t dataLen	= *(uint8_t*)(req.reqDatum+1);
			uint16_t offset = *(uint16_t*)(req.reqDatum+2);
			uint8_t* pData		= req.reqDatum + 4;

			bool bRtn		= pMCUPwrCtrlr->WrEEPROM(portID,regAddr,offset,pData,(uint8_t)dataLen);
			resp->errorCode	= (true == bRtn)?RESP_E_SUCCESS:RESP_E_FAIL_EEPROM;
			break;
		}
		case REQ_T_MCU_READ_EEPROM:
		{
			uint8_t regAddr	= *(uint8_t*)req.reqDatum;
			uint8_t dataLen	= *(uint8_t*)(req.reqDatum+1);
			uint16_t offset = *(uint16_t*)(req.reqDatum+2);
			uint8_t* pData		= resp->respDatum;

			resp->dataSize	= dataLen;
			bool bRtn		= pMCUPwrCtrlr->RdEEPROM(portID,regAddr,offset,pData,dataLen);
			resp->errorCode	= (true == bRtn)?RESP_E_SUCCESS:RESP_E_FAIL_EEPROM;
			break;
		}
		case REQ_T_MCU_NVMEMI_START:
		{
			uint8_t xferBytes	= *(uint8_t*)req.reqDatum;
			bool bRtn			= pMCUPwrCtrlr->NVMeMIStart(portID,xferBytes);
			resp->errorCode		= (true == bRtn)?RESP_E_SUCCESS:RESP_E_FAIL_NVMEMI;
			break;
		}
		case REQ_T_MCU_NVMEMI_DXFER:
		{
			uint8_t dLen	 	= *(uint8_t*)req.reqDatum;
			uint8_t offset		= *(uint8_t*)(req.reqDatum +1);
			uint8_t* pData		= req.reqDatum + 2;
			bool bRtn			= pMCUPwrCtrlr->NVMeMIDXfer(portID,pData,dLen,offset);
			resp->errorCode		= (true == bRtn)?RESP_E_SUCCESS:RESP_E_FAIL_NVMEMI;
			break;
		}
		case REQ_T_MCU_NVMEMI_READY:
		{
			uint16_t recvBytes	= 0;
			bool bRtn			= pMCUPwrCtrlr->NVMeMIReady(portID,recvBytes);
			resp->dataSize		= 2;
			resp->respDatum[0]	= recvBytes;
			resp->errorCode		= (true == bRtn)?RESP_E_SUCCESS:RESP_E_FAIL_NVMEMI;
			break;
		}
		case REQ_T_MCU_NVMEMI_DRCVR:
		{
			uint8_t dLen	 	= *(uint8_t*)req.reqDatum;
			uint8_t offset		= *(uint8_t*)(req.reqDatum +1);
			uint8_t* pData		= resp->respDatum;
			bool bRtn			= pMCUPwrCtrlr->NVMeMIDRcvr(portID,pData,dLen,offset);
			resp->errorCode		= (true == bRtn)?RESP_E_SUCCESS:RESP_E_FAIL_NVMEMI;
			break;
		}
		case REQ_T_MCU_UPDATEFW:
		{
			uint32_t fwSize	 	= *(uint32_t*)req.reqDatum;
			bool bRtn			= pMCUPwrCtrlr->UpdateFW(fwSize);
			resp->errorCode		= (true == bRtn)?RESP_E_SUCCESS:RESP_E_FAIL_UPDATEFW;
			break;
		}
		case REQ_T_MCU_DOWNLOADFW:
		{
			uint8_t dLen	 	= *(uint8_t*)req.reqDatum;
			uint8_t* pData		= req.reqDatum + 1;
			bool bRtn			= pMCUPwrCtrlr->DownloadFW(pData,dLen);
			resp->errorCode		= (true == bRtn)?RESP_E_SUCCESS:RESP_E_FAIL_DWLDFW;
			break;
		}
		case REQ_T_MCU_RESET:
		{
			bool bRtn			= pMCUPwrCtrlr->Reset();
			resp->errorCode		= (true == bRtn)?RESP_E_SUCCESS:RESP_E_FAIL_RESET;
			break;
		}
		default:
		{
			return PwrMngrServer::cb_ReceiveAndResponse(req);
		}
	}

	resp->version	= GetVersion();
	resp->slaveID	= slaveID;
	resp->reqType	= reqType;

	return true;
}

bool MCUPwrMngrClient::GetPowerID(uint8_t* pwrID,uint8_t len)
{
	return SendSyncRequest(REQ_T_MCU_GET_POWID,NULL,pwrID,0,len,PM_TIMEOUT);
}

bool MCUPwrMngrClient::SetPowerID(uint8_t* pwrID,uint8_t len)
{
	return SendSyncRequest(REQ_T_MCU_SET_POWID,pwrID,NULL,len,0,PM_TIMEOUT);
}

uint32_t MCUPwrMngrClient::GetTemp(void)
{
	uint32_t tempVal = 0;
	SendSyncRequest(REQ_T_MCU_GET_TEMP,NULL,&tempVal,0,sizeof(tempVal),PM_TIMEOUT);

	return tempVal;
}

bool MCUPwrMngrClient::DetectDevice(void)
{
	uint8_t result = 0xFF;
	SendSyncRequest(REQ_T_MCU_DEV_DETECT,NULL,&result,0,sizeof(uint8_t),PM_TIMEOUT);

	return 0 == result;
}

bool MCUPwrMngrClient::WrEEPROM(uint8_t regAddr,uint16_t offset,uint8_t* data,uint8_t len)
{
	if(PM_MCU_DATA_MAXLEN < len)
	{
		return false;
	}

	uint8_t input[PM_MCU_DATA_MAXLEN+4];
	memset(input,0,PM_MCU_DATA_MAXLEN+4);
	*(uint8_t*)input		= regAddr;
	*(uint8_t*)(input+1)	= len;
	*(uint16_t*)(input+2)	= offset;
	memcpy(input+4,data,len);

	return SendSyncRequest(REQ_T_MCU_WRITE_EEPROM,input,NULL,64,0,PM_TIMEOUT);
}

bool MCUPwrMngrClient::RdEEPROM(uint8_t regAddr,uint16_t offset,uint8_t* data,uint8_t len)
{
	if(PM_MCU_DATA_MAXLEN < len)
	{
		return false;
	}

	uint8_t input[4];
	memset(input,0,4);
	*(uint8_t*)input		= regAddr;
	*(uint8_t*)(input+1)	= len;
	*(uint16_t*)(input+2)	= offset;

	return SendSyncRequest(REQ_T_MCU_READ_EEPROM,input,data,4,len,PM_TIMEOUT);
}

bool MCUPwrMngrClient::NVMeMIStart(uint8_t xferBytes)
{
	return SendSyncRequest(REQ_T_MCU_NVMEMI_START,&xferBytes,NULL,1,0,PM_TIMEOUT);
}

bool MCUPwrMngrClient::NVMeMIDXfer(uint8_t* data,uint8_t len,uint8_t offset)
{
	if(PM_MCU_DATA_MAXLEN < len)
	{
		return false;
	}

	uint8_t input[PM_MCU_DATA_MAXLEN+1];
	memset(input,0,PM_MCU_DATA_MAXLEN+1);
	*(uint8_t*)input		= len;
	*(uint8_t*)(input+1)	= offset;
	memcpy(input+2,data,len);

	return SendSyncRequest(REQ_T_MCU_NVMEMI_DXFER,input,NULL,PM_MCU_DATA_MAXLEN+2,0,PM_TIMEOUT);
}

bool MCUPwrMngrClient::NVMeMIReady(uint16_t& recvBytes)
{
	return SendSyncRequest(REQ_T_MCU_NVMEMI_READY,NULL,&recvBytes,0,sizeof(uint16_t),PM_TIMEOUT);
}

bool MCUPwrMngrClient::NVMeMIDRcvr(uint8_t* data,uint8_t len,uint8_t offset)
{
	if(PM_MCU_DATA_MAXLEN < len)
	{
		return false;
	}
	
	uint8_t input[2];
	memset(input,0,2);
	
	*(uint8_t*)input		= len;
	*(uint8_t*)(input+1)	= offset;

	return SendSyncRequest(REQ_T_MCU_NVMEMI_DRCVR,input,data,2,len,PM_TIMEOUT);
}

bool MCUPwrMngrClient::UpdateFW(uint32_t fwSize)
{
	return SendSyncRequest(REQ_T_MCU_UPDATEFW,&fwSize,NULL,sizeof(uint32_t),0,PM_TIMEOUT);
}

bool MCUPwrMngrClient::DownloadFW(uint8_t* data,uint8_t len)
{
	if(PM_MCU_DATA_MAXLEN < len)
	{
		return false;
	}

	uint8_t input[PM_MCU_DATA_MAXLEN+1];
	memset(input,0,PM_MCU_DATA_MAXLEN+1);
	*(uint8_t*)input	= len;
	memcpy(input+1,data,len);

	return SendSyncRequest(REQ_T_MCU_NVMEMI_DRCVR,input,NULL,PM_MCU_DATA_MAXLEN+1,0,PM_TIMEOUT);
}

bool MCUPwrMngrClient::Reset(void)
{
	return SendRequest(REQ_T_MCU_RESET,NULL,0);
}

