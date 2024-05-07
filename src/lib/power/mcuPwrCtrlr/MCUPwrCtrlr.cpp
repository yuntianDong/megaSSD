/*
 *  * file : MCUPwrCtrlr.cpp
 *   */

#include "power/MCUPwrCtrlr.h"
#include "Logging.h"

#define RESP_DATA_MAXLEN		64
#define INVALID_VALUE			0xFFFF

MCUPwrCtrlr::MCUPwrCtrlr(const char* port)
	:BasicPwrCtrlr(),mCHPwrBitMask(0),mpSerial(NULL)
{
	ConfigSerial(port);

	mCHPwrBitMask	= 0;
	memset(mFWRev,0,sizeof(char)*MCP_PM_FWREV_LEN);
	mFWRev[0] = '\0';
}

MCUPwrCtrlr::~MCUPwrCtrlr(void)
{
	if(NULL != mpSerial)
	{
		delete mpSerial;
		mpSerial = NULL;
	}
}

void MCUPwrCtrlr::ConfigSerial(const char* port)
{
	if(NULL != mpSerial)
	{
		delete mpSerial;
	}

	mpSerial = new BoostSerial(port);
	mpSerial->Configure();
}

bool MCUPwrCtrlr::SendMCPReq(MPCtrlReq& req)
{
	mtx_.lock();

	uint8_t  reqLen		= req.GetMPCReqLen();
	uint8_t* reqData	= req.GetMPCReqData();

	bool rtn = mpSerial->Write(reqData,reqLen);
	mtx_.unlock();

	return rtn;
}

MPCtrlResp*	MCUPwrCtrlr::HandleMCPReq(MPCtrlReq& req)
{
	mtx_.lock();
	auto start = std::chrono::high_resolution_clock::now();
	uint8_t ch = req.GetPort();
	LOGDEBUG("%d port has been locked",ch);

	uint8_t  reqLen		= req.GetMPCReqLen();
	uint8_t* reqData	= req.GetMPCReqData();

	if(false == mpSerial->Write(reqData,reqLen))
	{
		LOGERROR("Fail to call mpSerial->Write()");
		mtx_.unlock();
		return NULL;
	}

	uint8_t respData[RESP_DATA_MAXLEN];
	memset(respData,0,RESP_DATA_MAXLEN*sizeof(uint8_t));

	if(false == mpSerial->Read(respData,RESP_DATA_MAXLEN))
	{
		LOGERROR("Fail to call mpSerial->Read()");
		mtx_.unlock();
		return NULL;
	}

	mtx_.unlock();
	LOGDEBUG("%d port is unlocked , HandleMCPReq take %lld milliseconds",ch,std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start));
	return new MPCtrlResp(respData,RESP_DATA_MAXLEN);
}

bool MCUPwrCtrlr::DoSingleCHPower(uint8_t ch,bool bPowerOn)
{
	bool bRtn = true;

	assert(ch < MCU_PWRCTRLR_CH_MAX);

	MPCtrlReq* req	= (true == bPowerOn)?(MPCtrlReq*)new ReqPowerOn():(MPCtrlReq*)new ReqPowerOff();
	req->SetPort(ch);

	uint8_t result = 0xFF;
	if(false == HandleReqAndCheckResp(*req,&result,1)
			|| 0 != result)
	{
		LOGERROR("Fail to HandleReqAndCheckResp(&result), result=%d",result);
		bRtn = false;
	}

	if(true == bPowerOn)
	{
		SetCHBit(ch);
	}
	else
	{
		ClrCHBit(ch);
	}

	if(NULL != req)
	{
		delete req;
		req = NULL;
	}

	return bRtn;
}

bool MCUPwrCtrlr::HandleReqAndCheckResp(MPCtrlReq& req,uint8_t* data,uint8_t dlen)
{
	bool bRtn = true;

	MPCtrlResp* resp = HandleMCPReq(req);
	if(NULL == resp)
	{
		LOGERROR("Fail to call HandleMCPReq(req)");
		bRtn = false;
	}

	if(true == bRtn && false == CheckAndGetRespData(resp,data,dlen))
	{
		LOGERROR("Fail to CheckAndGetRespData()");
		bRtn = false;
	}

	if(NULL != resp)
	{
		delete resp;
		resp = NULL;
	}

	return bRtn;
}

bool MCUPwrCtrlr::CheckAndGetRespData(MPCtrlResp* resp,uint8_t* data,uint8_t dlen)
{
	assert(NULL != resp);

	if(false == resp->CheckValid())
	{
		LOGERROR("Invalid MPCtrlResp");
		resp->Dump();
		return false;
	}

	if(NULL != data && 0 < dlen)
	{
		uint8_t respDLen	= resp->GetDLen();
		resp->GetDatum(data,(dlen > respDLen)?respDLen:dlen);
	}

	return true;
}

bool MCUPwrCtrlr::PowerOn(uint8_t ch)
{
	bool bRtn = true;

	if(ALL_CHANNELS == ch)
	{
		for(uint8_t idx=0;idx<MCU_PWRCTRLR_CH_MAX;idx++)
		{
			bRtn &= DoSingleCHPower(idx,true);
		}
	}
	else
	{
		bRtn = DoSingleCHPower(ch,true);
	}

	return bRtn;
}

bool MCUPwrCtrlr::PowerOff(uint8_t ch)
{
	bool bRtn = true;

	if(ALL_CHANNELS == ch)
	{
		for(uint8_t idx=0;idx<MCU_PWRCTRLR_CH_MAX;idx++)
		{
			bRtn &= DoSingleCHPower(idx,false);
		}
	}
	else
	{
		bRtn = DoSingleCHPower(ch,false);
	}

	return bRtn;
}

uint32_t MCUPwrCtrlr::GetVoltage(uint8_t ch)
{
	assert(ch < MCU_PWRCTRLR_CH_MAX);

	ReqGetVoltage req;
	req.SetPort(ch);

	uint16_t val = INVALID_VALUE;
	if(false == HandleReqAndCheckResp(req,(uint8_t*)&val,2))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return (uint32_t)val;
}

uint32_t MCUPwrCtrlr::GetCurrent(uint8_t ch)
{
	assert(ch < MCU_PWRCTRLR_CH_MAX);

	ReqGetCurrent req;
	req.SetPort(ch);

	uint16_t val = INVALID_VALUE;
	if(false == HandleReqAndCheckResp(req,(uint8_t*)&val,2))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return (uint32_t)val;
}

uint32_t MCUPwrCtrlr::GetPower(uint8_t ch)
{
	return GetVoltage(ch) * GetCurrent(ch);
}

bool MCUPwrCtrlr::GetPowerID(uint8_t* pwrID,uint8_t len)
{
	if( NULL == pwrID || 0 == len )
	{
		LOGERROR("Invalid Parameter");
		return false;
	}

	ReqGetPowerID	req;

	if(false == HandleReqAndCheckResp(req,pwrID,(len > MPC_REQ_DLEN_POWERID)?MPC_REQ_DLEN_POWERID:len))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return true;
}

bool MCUPwrCtrlr::SetPowerID(uint8_t* pwrID,uint8_t len)
{
	if(NULL == pwrID || 0 == len)
	{
		LOGERROR("Invalid Parameter");
		return false;
	}

	ReqSetPowerID	req;
	req.SetDatum(pwrID,(len > MPC_REQ_DLEN_POWERID)?MPC_REQ_DLEN_POWERID:len);

	uint8_t result = 0xFF;
	if(false == HandleReqAndCheckResp(req,&result,1)
			|| 0 != result)
	{
		LOGERROR("Fail to HandleReqAndCheckResp(&result), result=%d",result);
		return false;
	}

	return true;
}

uint32_t MCUPwrCtrlr::GetTemp(uint8_t ch)
{
	assert(ch < MCU_PWRCTRLR_CH_MAX);

	ReqGetTemp req;
	req.SetPort(ch);

	uint16_t val = INVALID_VALUE;
	if(false == HandleReqAndCheckResp(req,(uint8_t*)&val,2))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return (uint32_t)val;
}

bool MCUPwrCtrlr::DetectDevice(uint8_t ch)
{
	assert(ch < MCU_PWRCTRLR_CH_MAX);

	ReqDetectDevice req;
	req.SetPort(ch);

	uint8_t result = 0xFF;
	if(false == HandleReqAndCheckResp(req,&result,1)
			|| 0 != result)
	{
		LOGERROR("Fail to HandleReqAndCheckResp(&result), result=%d",result);
		return false;
	}

	return true;
}

bool MCUPwrCtrlr::GetFWRev(uint8_t* fwRev,uint8_t len)
{
	if(NULL == fwRev || len < MCP_RESP_DLEN_GETFWREV)
	{
		LOGERROR("Invalid Parameter");
		return false;
	}

	ReqGetFWRev	req;

	if(false == HandleReqAndCheckResp(req,fwRev,MCP_RESP_DLEN_GETFWREV))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return true;
}

bool MCUPwrCtrlr::WrEEPROM(uint8_t ch,uint8_t regAddr,uint16_t offset,uint8_t* data,uint8_t len)
{
	if(NULL == data || 0 == len)
	{
		LOGERROR("Invalid Parameter");
		return false;
	}

	ReqWrEEPPROM	req(regAddr);
	req.SetRegAddr(offset);
	req.SetDatum(data,len);
	req.SetPort(ch);

	uint8_t result = 0xFF;
	if(false == HandleReqAndCheckResp(req,&result,1)
			|| 0 != result)
	{
		LOGERROR("Fail to HandleReqAndCheckResp(&result), result=%d",result);
		return false;
	}

	return true;
}

bool MCUPwrCtrlr::RdEEPROM(uint8_t ch,uint8_t regAddr,uint16_t offset,uint8_t* data,uint8_t len)
{
	if(NULL == data || 0 == len)
	{
		LOGERROR("Invalid Parameter");
		return false;
	}

	ReqRdEEPPROM	req(regAddr);
	req.SetRegAddr(offset);
	req.SetDataLen(len);

	req.SetPort(ch);

	if(false == HandleReqAndCheckResp(req,data,len))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return true;
}

bool MCUPwrCtrlr::NVMeMIStart(uint8_t ch,uint8_t xferBytes)
{
	ReqNVMeMIStart	req;
	req.SetPort(ch);
	req.SetDatum(&xferBytes,1);

	uint8_t result = 0xFF;
	if(false == HandleReqAndCheckResp(req,&result,1))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return 0 == result;
}

bool MCUPwrCtrlr::NVMeMIDXfer(uint8_t ch,uint8_t* data,uint8_t len,uint8_t offset)
{
	if(NULL == data || 0 == len)
	{
		LOGERROR("Invalid Parameter");
		return false;
	}

	ReqNVMeMIDXfer	req;
	req.SetPort(ch);
	req.SetDatum(data,len);
	req.SetRegAddr(offset);

	uint8_t result = 0xFF;
	if(false == HandleReqAndCheckResp(req,&result,2))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return 0 == result;
}

bool MCUPwrCtrlr::NVMeMIReady(uint8_t ch,uint16_t& recvBytes)
{
	ReqNVMeMIReady	req;
	req.SetPort(ch);

	if(false == HandleReqAndCheckResp(req,(uint8_t*)&recvBytes,sizeof(uint16_t)))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return true;
}

bool MCUPwrCtrlr::NVMeMIDRcvr(uint8_t ch,uint8_t* data,uint8_t len,uint8_t offset)
{
	ReqNVMeMIDRcvr	req;
	req.SetPort(ch);
	req.SetDataLen(len);
	req.SetRegAddr(offset);

	if(false == HandleReqAndCheckResp(req,data,len))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return true;
}

bool MCUPwrCtrlr::UpdateFW(uint32_t fwSize)
{
	ReqUpdateFW		req;
	req.SetDatum((uint8_t*)&fwSize,sizeof(uint32_t));

	uint8_t result = 0xFF;
	if(false == HandleReqAndCheckResp(req,&result,sizeof(uint8_t)))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return true;
}

bool MCUPwrCtrlr::DownloadFW(uint8_t* data,uint8_t len)
{
	ReqDownloadFW	req;
	req.SetDatum(data,len);

	uint8_t result = 0xFF;
	if(false == HandleReqAndCheckResp(req,&result,sizeof(uint8_t)))
	{
		LOGERROR("Fail to HandleReqAndCheckResp()");
		return false;
	}

	return true;
}

bool MCUPwrCtrlr::Reset(void)
{
	ReqReset		req;

	uint8_t result = 0xFF;
	if(false == SendMCPReq(req))
	{
		LOGERROR("Fail to SendMCPReq()");
		return false;
	}

	return true;
}

