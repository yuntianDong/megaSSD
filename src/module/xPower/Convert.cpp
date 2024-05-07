/*
 * file: Convert.cpp
 */

#include "Convert.h"

boost::python::list MCUPwrMngrClient_GetPowerID(MCUPwrMngrClient& client,uint8_t maxLen)
{
	boost::python::list vals;
	uint8_t pwrID[MPC_REQ_DLEN_POWERID];
	memset(pwrID,0,MPC_REQ_DLEN_POWERID);
	if(false == client.GetPowerID(pwrID,MPC_REQ_DLEN_POWERID))
	{
		return vals;
	}

	maxLen	= (maxLen > MPC_REQ_DLEN_POWERID)?MPC_REQ_DLEN_POWERID:maxLen;
	for(uint8_t idx=0;idx<maxLen;idx++)
	{
		vals.append(pwrID[idx]);
	}

	return vals;
}

bool MCUPwrMngrClient_SetPowerID(MCUPwrMngrClient& client,boost::python::list vals)
{
	uint8_t pwrID[MPC_REQ_DLEN_POWERID];
	memset(pwrID,0,MPC_REQ_DLEN_POWERID);

	uint8_t count = boost::python::len(vals);
	for(uint8_t idx=0;idx<count;idx++)
	{
		pwrID[idx]	= boost::python::extract<uint8_t>(vals[idx]);
	}

	return client.SetPowerID(pwrID,count);
}

bool MCUPwrMngrClient_WrEEPROM(MCUPwrMngrClient& client,uint8_t regAddr,uint16_t offset,Buffers& buf,uint8_t len)
{
	if(len > buf.GetBufSize() )
	{
		return false;
	}

	uint8_t* data = buf.GetBufferPoint();

	return client.WrEEPROM(regAddr,offset,data,len);
}

bool MCUPwrMngrClient_RdEEPROM(MCUPwrMngrClient& client,uint8_t regAddr,uint16_t offset,Buffers& buf,uint8_t len)
{
	if(len > buf.GetBufSize() )
	{
		return false;
	}

	uint8_t* data = buf.GetBufferPoint();

	return client.RdEEPROM(regAddr,offset,data,len);
}

bool MCUPwrCtrlr_WrEEPROM(MCUPwrCtrlr& pwrCtrlr,uint8_t port,uint8_t regAddr,uint16_t offset,Buffers& buf,uint8_t len)
{
	if(len > buf.GetBufSize() )
	{
		return false;
	}

	uint8_t* data = buf.GetBufferPoint();

	return pwrCtrlr.WrEEPROM(port,regAddr,offset,data,len);
}

bool MCUPwrCtrlr_RdEEPROM(MCUPwrCtrlr& pwrCtrlr,uint8_t port,uint8_t regAddr,uint16_t offset,Buffers& buf,uint8_t len)
{
	if(len > buf.GetBufSize() )
	{
		return false;
	}

	uint8_t* data = buf.GetBufferPoint();

	return pwrCtrlr.RdEEPROM(port,regAddr,offset,data,len);
}

boost::python::list MCUPwrCtrlr_GetPowerID(MCUPwrCtrlr& pwrCtrlr,uint8_t maxLen)
{
	boost::python::list vals;
	uint8_t pwrID[MPC_REQ_DLEN_POWERID];
	memset(pwrID,0,MPC_REQ_DLEN_POWERID);
	if(false == pwrCtrlr.GetPowerID(pwrID,MPC_REQ_DLEN_POWERID))
	{
		return vals;
	}

	maxLen	= (maxLen > MPC_REQ_DLEN_POWERID)?MPC_REQ_DLEN_POWERID:maxLen;
	for(uint8_t idx=0;idx<maxLen;idx++)
	{
		vals.append(pwrID[idx]);
	}

	return vals;
}

bool MCUPwrCtrlr_SetPowerID(MCUPwrCtrlr& pwrCtrlr,boost::python::list vals)
{
	uint8_t pwrID[MPC_REQ_DLEN_POWERID];
	memset(pwrID,0,MPC_REQ_DLEN_POWERID);

	uint8_t count = boost::python::len(vals);
	for(uint8_t idx=0;idx<count;idx++)
	{
		pwrID[idx]	= boost::python::extract<uint8_t>(vals[idx]);
	}

	return pwrCtrlr.SetPowerID(pwrID,count);
}


bool MCUPwrCtrlr_NVMeMIDXfer(MCUPwrCtrlr& pwrCtrlr,uint8_t port,Buffers& buf,uint16_t bufOff,uint8_t len,uint8_t offset)
{
	if( (bufOff + len) > buf.GetBufSize() )
	{
		return false;
	}

	uint8_t* data = buf.GetBufferPoint() + bufOff;

	return pwrCtrlr.NVMeMIDXfer(port,data,len,offset);
}

bool MCUPwrMngrClient_NVMeMIDXfer(MCUPwrMngrClient& client,Buffers& buf,uint16_t bufOff,uint8_t len,uint8_t offset)
{
	if( (len + bufOff) > buf.GetBufSize() )
	{
		return false;
	}

	uint8_t* data = buf.GetBufferPoint() + bufOff;

	return client.NVMeMIDXfer(data,len,offset);
}

boost::python::list MCUPwrCtrlr_NVMeMIReady(MCUPwrCtrlr& pwrCtrlr,uint8_t port)
{
	uint16_t	recvBytes	= 0;
	bool rtn = pwrCtrlr.NVMeMIReady(port,recvBytes);

	boost::python::list vals;
	vals.append(rtn);
	vals.append(recvBytes);

	return vals;
}

boost::python::list MCUPwrMngrClient_NVMeMIReady(MCUPwrMngrClient& client)
{
	uint16_t	recvBytes	= 0;
	bool rtn = client.NVMeMIReady(recvBytes);

	boost::python::list vals;
	vals.append(rtn);
	vals.append(recvBytes);

	return vals;
}

bool MCUPwrCtrlr_NVMeMIDRcvr(MCUPwrCtrlr& pwrCtrlr,uint8_t port,Buffers& buf,uint16_t bufOff,uint8_t len,uint8_t offset)
{
	if( (bufOff + len) > buf.GetBufSize() )
	{
		return false;
	}

	uint8_t* data = buf.GetBufferPoint() + bufOff;

	return pwrCtrlr.NVMeMIDRcvr(port,data,len,offset);
}

bool MCUPwrMngrClient_NVMeMIDRcvr(MCUPwrMngrClient& client,Buffers& buf,uint16_t bufOff,uint8_t len,uint8_t offset)
{
	if( (offset + len) > buf.GetBufSize() )
	{
		return false;
	}

	uint8_t* data = buf.GetBufferPoint() + bufOff;

	return client.NVMeMIDRcvr(data,len,offset);
}

bool MCUPwrCtrlr_DownloadFW(MCUPwrCtrlr& pwrCtrlr,Buffers& buf,uint16_t offset,uint8_t len)
{
	if( (offset + len) > buf.GetBufSize() )
	{
		return false;
	}

	uint8_t* data = buf.GetBufferPoint() + offset;

	return pwrCtrlr.DownloadFW(data,len);
}

bool MCUPwrMngrClient_DownloadFW(MCUPwrMngrClient& client,Buffers& buf,uint16_t offset,uint8_t len)
{
	if( (offset + len) > buf.GetBufSize() )
	{
		return false;
	}

	uint8_t* data = buf.GetBufferPoint() + offset;

	return client.DownloadFW(data,len);
}

