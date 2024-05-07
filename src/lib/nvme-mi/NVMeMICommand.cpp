/*
 *  * file NVMeMICommand.cpp
 *   */

#include <unistd.h>
#include <assert.h>
#include "nvme-mi/NVMeMICommand.h"
#include "Logging.h"

#define MCTP_PACKAGE_HEADER_VERSION			0x1
#define SMBUS_COMMANDCODE_MCTP				0xF

#define CRC32_BYTES_COUNT					4

#define DEF_CMD_BUF_SIZE					4096

#ifndef UNUSED_PARAM
#define UNUSED_PARAM(x)						(void)(v)
#endif

uint16_t NVMeMiCommand::GetMCTPPackageCount(MCTPNVMeMIMsgRegMap* msg)
{
	uint16_t	mctpUnit	= GetMCTPUnitSize();
	uint16_t	reqSize		= msg->GetLength();

	return (uint16_t)((reqSize + mctpUnit -1)/mctpUnit);
}

void NVMeMiCommand::InjectErrorToPackage(uint16_t noPack,SMBusPackage& smbus)
{
	Buffers* buf 	= smbus.GetBuffer();
	uint16_t dLen 	= smbus.GetLength();
	uint16_t index 	= 0;

	while(true)
	{
		uint16_t	packIdx	= 0;
		uint16_t	offset	= 0;
		uint32_t	bitMask	= 0;
		uint32_t	dwVal	= 0;
		if(false == mMIErrInject.Fetch(index++,packIdx,offset,bitMask,dwVal))
		{
			break;
		}

		if(packIdx == noPack)
		{
			if( (offset + sizeof(uint32_t)) >= dLen )
			{
				continue;
			}

			uint32_t orgVal = buf->GetDWord(offset);
			buf->SetDWord(offset, ( (orgVal&(~bitMask)) | (dwVal&bitMask) ) );
		}
	}
}

bool NVMeMiCommand::GetMCTPPackageByIndex(MCTPPackageRegMap& package,uint16_t index,MCTPNVMeMIMsgRegMap* req)
{
	LOGDEBUG("NVMeMiCommand::GetMCTPPackageByIndex\n");

	uint16_t mctpCount	= GetMCTPPackageCount(req);
	if(index >= mctpCount)
	{
		return false;
	}

	package.SetHeaderVersion(MCTP_PACKAGE_HEADER_VERSION);
	package.SetPktSeq(index);
	package.SetSOM(index == 0);
	package.SetSrcEPID(GetSrcEndPointID());
	package.SetDstEPID(GetDstEndPointID());

	uint16_t	mctpUnit	= GetMCTPUnitSize();
	if( index == (mctpCount - 1) )
	{
		package.SetEOM(true);
		uint16_t xferSize	= req->GetLength() - index*mctpUnit;
		package.SetPacketPayload(req->GetBuffer(),mctpUnit*index,xferSize);
	}
	else
	{
		package.SetEOM(false);
		package.SetPacketPayload(req->GetBuffer(),mctpUnit*index,mctpUnit);
	}

	return true;
}

bool NVMeMiCommand::GetSMBusPackage(SMBusPackage& smbus,MCTPPackageRegMap& mctp)
{
	LOGDEBUG("NVMeMiCommand::GetSMBusPackage\n");

	smbus.SetCommandCode(SMBUS_COMMANDCODE_MCTP);
	uint8_t bytes	= mctp.GetLength() + 1;
	smbus.SetByteCount(bytes);
	Buffers* buf = smbus.GetBuffer();
	smbus.SetDataBytes(mctp.GetBuffer(),0,mctp.GetLength());
	smbus.SetPEC(CalcSMBusPEC(buf,smbus.GetLength()-SMBUS_PEC_LEN));

	return true;
}

bool NVMeMiCommand::GetNVMeMIMsgFromSMBus(MCTPNVMeMIMsgRegMap* resp,uint16_t index,SMBusPackage& smbus,bool &isLast)
{
	LOGDEBUG("Enter NVMeMiCommand::GetNVMeMIMsgFromSMBus\n");

	uint8_t*	respDatum	= resp->GetBuffer()->GetBufferPoint();
	uint32_t	respSize	= resp->GetBuffer()->GetBufSize();
	uint16_t	bufAlign	= resp->GetBuffer()->GetBufAlignment();

	MCTPPackageRegMap	mctp;
	Buffers* mctpBuf	= mctp.GetBuffer();
	uint16_t mctpUnit	= GetMCTPUnitSize();

	Buffers 	msgBuf(respDatum+index*mctpUnit,respSize - index*mctpUnit,bufAlign);

	LOGDEBUG("index = %d,respSize - index*mctpUnit = %d",index,(respSize - index*mctpUnit));

	uint16_t dLength	= smbus.GetByteCount() - SMBUS_PEC_LEN - SMBUS_HEADER_SIZE;
	smbus.GetDataBytes(mctpBuf,0,dLength);

	mctp.UpdatePacketPayloadLen(dLength - MCTP_HEADER_SIZE);
	mctp.GetPacketPayload(&msgBuf,0,dLength - MCTP_HEADER_SIZE);

	isLast	= mctp.GetEOM();

	LOGDEBUG("Exit NVMeMiCommand::GetNVMeMIMsgFromSMBus\n");
	return true;
}

bool NVMeMiCommand::XferSMBusPackage(SMBusPackage& smbus)
{
	LOGDEBUG("NVMeMiCommand::XferSMBusPackage\n");

	Buffers* buf = smbus.GetBuffer();
	uint16_t len = smbus.GetLength();

	if( len > buf->GetBufSize() )
	{
		LOGERROR("len(%d) > buf->GetBufSize()(%d)",len,buf->GetBufSize());
		return false;
	}

	if( NULL == mpBPDriver || false == mpBPDriver->ReadyForWrite(len))
	{
		LOGERROR("Fail to call mpBPDriver->ReadyForWrite(len)");
		return false;
	}

	uint16_t unitSize	= mpBPDriver->GetUnitLength();
	uint16_t pieces		= (uint16_t)((len + unitSize -1) / unitSize);

	for(uint16_t idx=0;idx<pieces;idx++)
	{
		uint16_t offset		= idx*unitSize;
		uint16_t bytes		= (idx < (pieces -1))?unitSize:len - offset;

		LOGDEBUG("len = %d,offset = %d,bytes = %d\n",len,offset,bytes);

		if( NULL == mpBPDriver || false == mpBPDriver->Write( (buf->GetBufferPoint()) + offset,bytes,offset ))
		{
			LOGERROR("Fail to call mpBPDriver->Write( (buf->GetBufferPoint()) + %d,%d,%d )",offset,bytes,offset);
			return false;
		}
	}

	return true;
}

bool NVMeMiCommand::GetRcvrBytes(uint16_t& totalRcvr)
{
	totalRcvr	= 0;
	if( NULL == mpBPDriver || false == mpBPDriver->ReadyForRead(totalRcvr) )
	{
		return false;
	}

	LOGDEBUG("totalRcvr = %d\n",totalRcvr);
	return true;
}

bool NVMeMiCommand::TryGetRcvrBytes(uint16_t& totalRcvr,uint32_t timeoutms)
{
	totalRcvr	= 0;
	uint8_t	 maxTries	= 100;
	while(0 == totalRcvr && 0 < maxTries)
	{
		if(false == GetRcvrBytes(totalRcvr))
		{
			return false;
		}

		maxTries--;
		usleep((timeoutms*1000)/maxTries);
	}

	if( 0 == totalRcvr )
	{
		LOGERROR("ReadyForRead() == 0");
		return false;
	}

	return true;
}

bool NVMeMiCommand::RcvrSMBusResponse(SMBusPackage& smbus,uint16_t maxBytes,uint16_t index)
{
	LOGDEBUG("NVMeMiCommand::RcvrSMBusResponse\n");

	Buffers* buf = smbus.GetBuffer();

	uint16_t unitSize	= mpBPDriver->GetUnitLength();
	uint16_t pieces	= (uint16_t)((maxBytes + unitSize -1) / unitSize);
	for(uint16_t idx=0;idx<pieces;idx++)
	{
		uint16_t bufOff		= idx*unitSize;
		uint16_t rdOff		= idx*unitSize + index*maxBytes;
		uint16_t bytes		= (idx < (pieces -1))?unitSize:(maxBytes - unitSize*idx);

		LOGDEBUG("idx=%d,offset = %d,bytes = %d\n",idx,offset,bytes);

		if(NULL == mpBPDriver || false == mpBPDriver->Read(buf->GetBufferPoint() + bufOff,bytes,rdOff))
		{
			LOGERROR("Fail to call mpBPDriver->Read(buf->GetBufferPoint() + %d,%d,%d)",bufOff,bytes,rdOff);
			return false;
		}
	}

	return true;
}

bool NVMeMiCommand::SendNVMeMIRequest(MCTPNVMeMIMsgRegMap* req)
{
	LOGDEBUG("NVMeMiCommand::SendNVMeMIRequest\n");

	for(uint16_t idx=0;idx<GetMCTPPackageCount(req);idx++)
	{
		MCTPPackageRegMap	package;
		if( false == GetMCTPPackageByIndex(package,idx,req) )
		{
			LOGERROR("Fail to call GetMCTPPackageByIndex(package,%d,req)",idx);
			return false;
		}

		uint16_t bufSize	= GetSMBusPackageSize();
		SMBusPackage	smbus(bufSize);
		smbus.SetSrcSlaveAddr(GetSrcI2CAddr());
		smbus.SetDestSlaveAddr(GetDstI2CAddr());

		if( false == GetSMBusPackage(smbus,package) )
		{
			LOGERROR("Fail to call GetSMBusPackage(smbus,package)");
			return false;
		}

		smbus.Debug();

		if(false == mMIErrInject.IsEmpty())
		{
			InjectErrorToPackage(idx,smbus);
		}

		if( false == XferSMBusPackage(smbus) )
		{
			LOGERROR("Fail to call XferSMBusPackage(smbus)");
			return false;
		}
	}

	if(true == GetAutoClear())
	{
		ClearInjectErr();
	}

	return true;
}

bool NVMeMiCommand::GetNVMeMIResponse(MCTPNVMeMIMsgRegMap* resp)
{
	LOGDEBUG("Enter NVMeMiCommand::GetNVMeMIResponse\n");

	uint16_t	totlRcvr	= 0;
	uint16_t	respBytes	= 0;
	if( false == TryGetRcvrBytes(totlRcvr,1000) )
	{
		LOGERROR("Fail to call TryGetRcvrBytes(totlRcvr,1000)");
		return false;
	}

	uint16_t	unitSize	= GetSMBusPackageSize();
	uint16_t	idx			= 0;
	bool		isLast		= false;

	while(false == isLast)
	{
		uint16_t		packLen 	= unitSize;
		SMBusPackage	package(unitSize);
		package.UpdateDataBytes(GetMCTPUnitSize());

		LOGDEBUG("idx=%d,packLen=%d",idx,packLen);

		if( false == RcvrSMBusResponse(package,packLen,idx) )
		{
			LOGERROR("Fail to call RcvrSMBusResponse(package,%d)",packLen);
			return false;
		}

		package.Debug();

		if( false == GetNVMeMIMsgFromSMBus(resp,idx,package,isLast) )
		{
			LOGERROR("Fail to call GetNVMeMIMsgFromSMBus(resp,%d,package)",idx);
			return false;
		}

		respBytes += (packLen - SMBUS_EXTRA_DATA_SIZE);
		idx++;

		usleep(1000000);	// Workaround, wait 1s
		if(true)
		{
			uint16_t recvBytes = 0;
			GetRcvrBytes(recvBytes);
			LOGINFO("GetRcvrBytes(%d)",recvBytes);
		}
	}

	LOGDEBUG("respBytes = %d",respBytes);

	LOGDEBUG("Exit NVMeMiCommand::GetNVMeMIResponse\n");
	return resp->SetLength(respBytes);
}

bool NVMeMiCommand::SendReqAndRecvResp(MCTPNVMeMIMsgRegMap* req,MCTPNVMeMIMsgRegMap* resp)
{
	req->Debug();

	if( false == SendNVMeMIRequest(req) )
	{
		LOGERROR("Fail to call SendNVMeMIRequest(req)");
		return false;
	}

	if( false == GetNVMeMIResponse(resp) )
	{
		LOGERROR("Fail to call GetNVMeMIResponse(resp)");
		return false;
	}

	resp->Debug();

	return true;
}

NVMeMIPrimResp* NVMeMiCommand::MIPrimGetStatus(bool cesf)
{
	NMCPCmdGetState		req;
	req.SetCESF(cesf);
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMIPrimResp*	resp	= new NVMeMIPrimResp();
	SendReqAndRecvResp(&req,resp);
	return resp;
}

NVMeMIPrimResp* NVMeMiCommand::MIPrimPause(void)
{
	NMCPCmdPause		req;
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMIPrimResp*	resp	= new NVMeMIPrimResp();
	SendReqAndRecvResp(&req,resp);
	return resp;
}

NVMeMIPrimResp* NVMeMiCommand::MIPrimResume(void)
{
	NMCPCmdResume		req;
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMIPrimResp*	resp	= new NVMeMIPrimResp();
	SendReqAndRecvResp(&req,resp);
	return resp;
}

NVMeMIPrimResp* NVMeMiCommand::MIPrimAbort(void)
{
	NMCPCmdAbort		req;
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMIPrimResp*	resp	= new NVMeMIPrimResp();
	SendReqAndRecvResp(&req,resp);
	return resp;
}

NVMeMIPrimResp* NVMeMiCommand::MIPrimReplay(uint8_t rro)
{
	NMCPCmdReplay		req;
	req.SetRRO(rro);
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMIPrimResp*	resp	= new NVMeMIPrimResp();
	SendReqAndRecvResp(&req,resp);
	return resp;
}

NVMeMICmdResp*	NVMeMiCommand::MICmdConfigGet(uint8_t cfgID,uint8_t pID)
{
	NVMICConfigGet		req;
	req.SetCFGID(cfgID);
	req.SetNMD0B4(pID);
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMICmdResp*	resp	= new NVMeMICmdResp();
	SendReqAndRecvResp(&req,resp);
	resp->UpdateResponseDataLen();
	return resp;
}

NVMeMICmdResp*	NVMeMiCommand::MICmdConfigSet(uint8_t cfgID,uint8_t pID,uint16_t wdVal,uint32_t dwVal)
{
	NVMICConfigSet		req;
	req.SetCFGID(cfgID);
	req.SetNMD0B4(pID);
	req.SetNMD0B2((uint8_t)( wdVal & 0xFF ));
	req.SetNMD0B3((uint8_t)( (wdVal >> 8) & 0xFF ));
	req.SetNMD1(dwVal);
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMICmdResp*	resp	= new NVMeMICmdResp();
	SendReqAndRecvResp(&req,resp);
	resp->UpdateResponseDataLen();
	return resp;
}

NVMeMICmdResp*  NVMeMiCommand::MICmdCtrlHSP(uint16_t ctlID,uint8_t maxRent,bool incf,bool incpf,bool incvf,\
		bool all,bool ccf,bool cwarn,bool spare,bool pdlu,bool ctemp,bool csts)
{
	NVMICCtrlHSP		req;
	req.SetCTLID(ctlID);
	req.SetMAXRENT(maxRent);
	req.SetINCF(incf);
	req.SetINCPF(incpf);
	req.SetINCVF(incvf);
	req.SetRALL(all);
	req.SetCCF(ccf);
	req.SetCWARN(cwarn);
	req.SetSPARE(spare);
	req.SetPDLU(pdlu);
	req.SetCTEMP(ctemp);
	req.SetCSTS(csts);
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMICmdResp*	resp	= new NVMeMICmdResp();
	SendReqAndRecvResp(&req,resp);
	resp->UpdateResponseDataLen();
	return resp;
}

NVMeMICmdResp*	NVMeMiCommand::MICmdNVMSHSP(bool cs)
{
	NVMICNVMSHSP		req;
	req.SetCS(cs);
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMICmdResp*	resp	= new NVMeMICmdResp();
	SendReqAndRecvResp(&req,resp);
	resp->UpdateResponseDataLen();
	return resp;
}

NVMeMICmdResp*	NVMeMiCommand::MICmdReadData(uint16_t ctlID,uint8_t pID,uint8_t dtyp)
{
	NVMICReadData		req;
	req.SetCTRLID(ctlID);
	req.SetPORTID(pID);
	req.SetDTYP(dtyp);
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMICmdResp*	resp	= new NVMeMICmdResp();
	SendReqAndRecvResp(&req,resp);
	resp->UpdateResponseDataLen();
	return resp;
}

NVMeMICmdResp*	NVMeMiCommand::MICmdReset(uint8_t type)
{
	NVMICReset		req;
	req.SetTYPE(type);
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMICmdResp*	resp	= new NVMeMICmdResp();
	SendReqAndRecvResp(&req,resp);
	resp->UpdateResponseDataLen();
	return resp;
}

NVMeMICmdResp*	NVMeMiCommand::MICmdVPDRead(uint16_t dofst,uint16_t dlen)
{
	NVMICVPDRead		req;
	req.SetDOFST(dofst);
	req.SetDLEN(dlen);
	req.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMICmdResp*	resp	= new NVMeMICmdResp();
	SendReqAndRecvResp(&req,resp);
	resp->UpdateResponseDataLen();
	return resp;
}

NVMeMICmdResp*	NVMeMiCommand::MICmdVPDWrite(uint16_t dofst,uint16_t dlen,Buffers* buf,uint16_t offset)
{
	NVMICVPDWrite			req;
	req.SetDOFST(dofst);
	req.SetDLEN(dlen);
	req.SetCSI(GetCmdSlotID());
	req.SetRequestData(buf,offset,dlen);

	Buffers* reqBuf = req.GetBuffer();
	uint16_t reqLen	= req.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	req.SetCRC(crc);

	NVMeMICmdResp*	resp	= new NVMeMICmdResp();
	SendReqAndRecvResp(&req,resp);
	resp->UpdateResponseDataLen();
	return resp;
}

NVMeCmdResp* NVMeMiCommand::DeviceSelfTest(uint32_t nsid,uint8_t stc,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSDeviceSelfTest cmd	= PSDeviceSelfTest();
	cmd.SetSTC(stc);
	cmd.SetNSID(nsid);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,NULL,0,0);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,NULL,0,0);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::FirmwareCommit(uint8_t slotID,uint8_t cAction,bool bBPID,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSFirmwareCommit cmd		= PSFirmwareCommit();
	cmd.SetFS(slotID);
	cmd.SetCA(cAction);
	cmd.SetBPIDFlag(bBPID);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,NULL,0,0);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,NULL,0,0);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::FirmwareDownload(uint32_t offset,uint32_t length,Buffers* buf,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSFirmwareDownload cmd		= PSFirmwareDownload();
	cmd.SetNUMD(length);
	cmd.SetOFST(offset);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,buf,offset,length+1);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,NULL,0,0);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::GetFeatures(uint32_t nsid,uint8_t fid,uint8_t sel,uint32_t cdw11,Buffers* buf,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSGetFeature cmd		= PSGetFeature();
	cmd.SetFID(fid);
	cmd.SetSEL(sel);
	cmd.SetCDW11(cdw11);
	cmd.SetNSID(nsid);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,NULL,0,DEF_CMD_BUF_SIZE);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,buf,0,DEF_CMD_BUF_SIZE);
	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::GetLogPage(uint32_t nsid,uint8_t lid,uint64_t offset,uint32_t length,uint8_t lsp,bool bRAE,Buffers* buf,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSGetLogPage cmd		= PSGetLogPage();
	cmd.SetLID(lid);
	cmd.SetLSP(lsp);
	cmd.SetRAEFlag(bRAE);
	cmd.SetNumOfDW(length);
	cmd.SetLogPageOffset(offset);
	cmd.SetNSID(nsid);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,NULL,offset,length+1);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,buf,offset,length+1);
	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::Identify(uint8_t cns,uint16_t cntid,uint32_t nsid,Buffers* buf,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSIdentify	cmd			= PSIdentify();
	cmd.SetCNS(cns);
	cmd.SetCNTID(cntid);
	cmd.SetNSID(nsid);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,NULL,0,DEF_CMD_BUF_SIZE);
	miReq.SetCTLID(cntid);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,buf,0,DEF_CMD_BUF_SIZE);
	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::NSAttach(uint32_t nsid,uint8_t sel,Buffers* buf,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSNamespaceAttach cmd	= PSNamespaceAttach();
	cmd.SetSEL(sel);
	cmd.SetNSID(nsid);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,buf,0,DEF_CMD_BUF_SIZE);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,NULL,0,0);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::NSMngmnt(uint32_t nsid,uint8_t sel,Buffers* buf,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSNamespaceMngr cmd		= PSNamespaceMngr();
	cmd.SetSEL(sel);
	cmd.SetNSID(nsid);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,buf,0,DEF_CMD_BUF_SIZE);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,NULL,0,0);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::SetFeatures(uint32_t nsid,uint8_t fid,bool bSV,uint32_t cdw11,Buffers* buf,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSSetFeature cmd		= PSSetFeature();
	cmd.SetFID(fid);
	cmd.SetSVFlag(bSV);
	cmd.SetCDW11(cdw11);
	cmd.SetNSID(nsid);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,buf,0,DEF_CMD_BUF_SIZE);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,NULL,0,0);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::FormatNVM(uint32_t nsid,uint8_t lbaf,uint8_t pi,uint8_t ses,bool bMSET,bool bPIL,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSFormatNVM cmd			= PSFormatNVM();
	cmd.SetNSID(nsid);
	cmd.SetLBAF(lbaf);
	cmd.SetMSETFlag(bMSET);
	cmd.SetPI(pi);
	cmd.SetPILFlag(bPIL);
	cmd.SetSES(ses);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,NULL,0,0);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,NULL,0,0);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::Sanitize(uint8_t act,uint8_t passCnt,uint32_t overPat,bool bAUSE,bool bOIPBP,bool bNDAS,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSSanitize cmd			= PSSanitize();
	cmd.SetSANACT(act);
	cmd.SetAUSEFlag(bAUSE);
	cmd.SetOWPASS(passCnt);
	cmd.SetOIPBPFlag(bOIPBP);
	cmd.SetNDASFlag(bNDAS);
	cmd.SetOVRPAT(overPat);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,NULL,0,0);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,NULL,0,0);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::SecurityRecv(uint8_t nssf,uint8_t spsp0,uint8_t spsp1,uint8_t secp,uint32_t length,Buffers* buf,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSSecurityRecv cmd		= PSSecurityRecv();
	cmd.SetNSSF(nssf);
	cmd.SetSPSP0(spsp0);
	cmd.SetSPSP1(spsp1);
	cmd.SetSECP(secp);
	cmd.SetAL(length);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,NULL,0,length);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,buf,0,length);
	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeMiCommand::SecuritySend(uint8_t nssf,uint8_t spsp0,uint8_t spsp1,uint8_t secp,uint32_t length,Buffers* buf,uint32_t timeout)
{
	UNUSED_PARAM(timeout);

	PSSecuritySend cmd		= PSSecuritySend();
	cmd.SetNSSF(nssf);
	cmd.SetSPSP0(spsp0);
	cmd.SetSPSP1(spsp1);
	cmd.SetSECP(secp);
	cmd.SetTL(length);

	NVMAdminReq		miReq;
	miReq.Translate(&cmd,buf,0,length);
	miReq.SetCSI(GetCmdSlotID());

	Buffers* reqBuf = miReq.GetBuffer();
	uint16_t reqLen	= miReq.GetLength();

	uint32_t crc	= CalcCRC32(reqBuf,reqLen - CRC32_BYTES_COUNT);
	miReq.SetCRC(crc);

	NVMAdminResp	miResp;
	SendReqAndRecvResp(&miReq,&miResp);

	NVMeCmdResp* resp	= new NVMeCmdResp();
	miResp.Translate(resp,NULL,0,0);

	return resp;
}

