/*
 * file : NVMeCommand.cpp
 */

#include "device/nvme/NVMeCommand.h"
#include "Logging.h"

#define DEF_CMD_BUF_SIZE				4096
#define NVME_Q_ENTRY_SIZE_SQ			64
#define NVME_Q_ENTRY_SIZE_CQ			16
#define BYTES_PER_RANGE					16
#define NS_MANAGEMENT_BUF_NS_CAP_POS	8
#define NS_MANAGEMENT_BUF_LBAF_POS		26
#define NS_MANAGEMENT_BUF_DPS_POS		29

bool CustomCompare(const std::pair<uint32_t, NsMapInfo>& a, const std::pair<uint32_t, NsMapInfo>& b)
{
	return a.second.offset < b.second.offset;
}

bool SetCmdPIParam(NVMeCmdReq* pCmd,uint8_t stagSize,uint8_t crcSize,uint64_t sTag,uint64_t rTag)
{
	uint32_t cdw2	= 0;
	uint32_t cdw3	= 0;
	uint32_t cdw14	= 0;

	//printf("Debug : stagSize = %d,crcSize = %d,sTag = %llx,rTag = %llx",stagSize,crcSize,sTag,rTag);

	if( 0 == crcSize )
	{
		return true;
	}else if( 16 == crcSize )
	{
		if( stagSize > 32 )
		{
			return false;
		}
		uint32_t rTMask = ((uint64_t)1 << (32 - stagSize)) -1;
		uint32_t sTMask = ((uint64_t)1 << stagSize) -1;
		cdw14	= (rTag & rTMask) | ( (sTag & sTMask) << (32 - stagSize) );
	}else if( 32 == crcSize )
	{
		if( stagSize > 64 || stagSize < 16 )
		{
			return false;
		}

		uint32_t rTSizeCDW14	= ( (80 - stagSize) > 32 )? 32 : (80 - stagSize);
		uint32_t sTSizeCDW14	= 32 - rTSizeCDW14;
		uint32_t rTMaskCDW14	= ((uint64_t)1 << rTSizeCDW14) - 1;
		uint32_t sTMaskCDW14	= ((uint64_t)1 << sTSizeCDW14) - 1;
		cdw14	= ( rTag & rTMaskCDW14 ) | ( (sTag & sTMaskCDW14) << rTSizeCDW14 );
		uint32_t rTSizeCDW3		= ( (80 - stagSize) <= 32 )? 0 : (80 - stagSize - 32);
		uint32_t sTSizeCDW3		= 32 - rTSizeCDW3;
		uint32_t rTMaskCDW3		= ((uint64_t)1 << rTSizeCDW3) - 1;
		uint32_t sTMaskCDW3		= ((uint64_t)1 << sTSizeCDW3) - 1;
		cdw3	= ( (rTag >> 32) & rTMaskCDW3 ) | ( ((sTag >> sTSizeCDW14) & sTMaskCDW3) << rTSizeCDW3 );
		uint32_t rTSizeCDW2		= 16;
		uint32_t rTMaskCDW2		= ((uint64_t)1 << 16) - 1;
		cdw2	= ( sTag >> (stagSize - rTSizeCDW2) ) & rTMaskCDW2;
	}else if( 64 == crcSize )
	{
		if( stagSize > 48 )
		{
			return false;
		}
		uint32_t rTSizeCDW14	= ( (48 - stagSize) > 32 )? 32 : (48 - stagSize);
		uint32_t sTSizeCDW14	= 32 - rTSizeCDW14;
		uint32_t rTMaskCDW14	= ((uint64_t)1 << rTSizeCDW14) - 1;
		uint32_t sTMaskCDW14	= ((uint64_t)1 << sTSizeCDW14) - 1;
		cdw14	= ( rTag & rTMaskCDW14 ) | ( (sTag & sTMaskCDW14) << rTSizeCDW14 );
		uint32_t rTSizeCDW3		= ( (48 - stagSize) <= 32 )? 0 : (48 - stagSize - 32);
		uint32_t sTSizeCDW3		= 16 - rTSizeCDW3;
		uint32_t rTMaskCDW3		= ((uint64_t)1 << rTSizeCDW3) - 1;
		uint32_t sTMaskCDW3		= ((uint64_t)1 << sTSizeCDW3) - 1;
		cdw3	= ( (rTag >> 32) & rTMaskCDW3 ) | ( ((sTag >> sTSizeCDW14) & sTMaskCDW3) << rTSizeCDW3 );
	}else{
		return false;
	}

	pCmd->SetCDW14(cdw14);
	pCmd->SetCDW3(cdw3);
	uint32_t tmpVal = pCmd->GetCDW2();
	pCmd->SetCDW2( (tmpVal & 0xFFFF0000) | (cdw2 & 0x0000FFFF) );

	// LOGDEBUG("cdw2 = %08x,cdw3 = %08x,cdw14 = %08x",cdw2,cdw3,cdw14);

	return true;
}

/*
 * class NVMeAdminCmd
 */

/* Other API */
bool  NVMeAdminCmd::IsNSActive(uint32_t nsID)
{
    bool isNsActive = false;
    if(strcmp(mpdev->GetDriver(),NVME_IN_USE) == 0)
    {
		/* Check given nsID whether in Active nsID list got by Identify cmd with cns-0x2 */
		uint8_t cnsActiveNsIDList = 0x02;

		Buffers* activeNsBuf = mbufMngr->GetUsrBuf("activeNsBuf",4096,0,NVME_IN_USE);
		NVMeCmdResp* resp = this->Identify(cnsActiveNsIDList,0,0,0,0,activeNsBuf,8000);
		if(resp->Succeeded() != true)
		{
			LOGERROR("Failed to excute cns=0x02 Identify cmd, cannot estimate whether nsID is active!");
		}

		activeNsBuf->SetEndianMode(BUFV_LITTLE_ENDIAN);
		for(uint32_t idx = 0;idx <= 4096;idx += 4)
		{
			uint32_t activeNsID = activeNsBuf->GetDWord(idx);
			if(activeNsID == nsID)
			{
				isNsActive = true;
				break;
			}
			if(activeNsID == 0)
			{
				break;
			}
		}
		activeNsBuf->SetEndianMode(BUFV_BIG_ENDIAN);
		del_obj(resp);
    }else{
		/* Check given nsID whether active through spdk function */
        isNsActive = spdk_nvme_ctrlr_is_active_ns(mpdev->GetOperator()->GetSpdkCtrlr(),nsID);
    }
	return isNsActive;
}

bool NVMeAdminCmd::LoadParentContext(ParentContext* parent)
{
	if(parent == nullptr)
	{
		LOGWARN("Invalid argument parentContext!");
		return false;
	}

	if(parent->GetParentParameter() == nullptr)
	{
		LOGWARN("parentContext has no valid content!");
		return false;
	}
	parentContext = parent;
	return true;
}

bool NVMeAdminCmd::IsParentContextLoaded(void)
{
	return (parentContext == nullptr) ? false : true;
}

/* Admin Commands */
NVMeCmdResp* NVMeAdminCmd::AdminPassthrough(uint32_t cdw0,uint32_t cdw1,uint32_t cdw2,uint32_t cdw3,\
		uint32_t cdw10,uint32_t cdw11,uint32_t cdw12, uint32_t cdw13,uint32_t cdw14,uint32_t cdw15,\
		uint32_t dBufLen,uint32_t mBufLen,Buffers* dBuf,Buffers* mBuf,uint32_t timeout)
{
		NVMeCmdReq cmd	= NVMeCmdReq(0,true,DataXfer_PRP,Fused_None);

	uint32_t*	cmdBuf	= (uint32_t*)cmd.GetCmdAddr();
	cmdBuf[0]	= cdw0;
	cmdBuf[1]	= cdw1;
	cmdBuf[2]	= cdw2;
	cmdBuf[3]	= cdw3;
	cmdBuf[10]	= cdw10;
	cmdBuf[11]	= cdw11;
	cmdBuf[12]	= cdw12;
	cmdBuf[13]	= cdw13;
	cmdBuf[14]	= cdw14;
	cmdBuf[15]	= cdw15;

	void* dataBuf = nullptr;
	void* metaBuf = nullptr;

	if(0 < dBufLen && nullptr != dBuf)
	{
		dataBuf = (void*)dBuf->GetBufferAddr();
	}

	if(0 < mBufLen && nullptr != mBuf)
	{
		metaBuf = (void*)mBuf->GetBufferAddr();
	}

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,dBufLen,metaBuf,mBufLen,timeout);

	if(0 < dBufLen && nullptr != dBuf)
	{
		resp->SetDataBuffer(dBuf);
	}

	if(0 < mBufLen && nullptr != mBuf)
	{
		resp->SetMetaBuffer(mBuf);
	}

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::Abort(uint16_t cid,uint16_t sqid,uint32_t timeout)
{
	PSAbort cmd			= PSAbort();
	cmd.SetCID(cid);
	cmd.SetSQID(sqid);

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::AsyncEventReq(uint32_t timeout)
{
	PSAsyncEventReq cmd	= PSAsyncEventReq();

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::CreateIOCQ(uint16_t qid,uint16_t qSize,uint16_t iv,\
		bool bIEN,bool bPC,Buffers* buf,uint32_t timeout)
{
	PSCreateIOCQ cmd	= PSCreateIOCQ();
	cmd.SetQID(qid);
	cmd.SetQSize(qSize);
	cmd.SetPCFlag(bPC);
	cmd.SetIENFlag(bIEN);
	cmd.SetIV(iv);
	cmd.SetDXferMode(GetDataXferMode());

	uint32_t bufSize= NVME_Q_ENTRY_SIZE_CQ*(qSize+1);
	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,bufSize,nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::CreateIOSQ(uint16_t qid,uint16_t qSize,uint16_t cqid,uint8_t qPrio,\
		bool bPC,Buffers* buf,uint32_t timeout)
{
	PSCreateIOSQ cmd	= PSCreateIOSQ();
	cmd.SetQID(qid);
	cmd.SetQSize(qSize);
	cmd.SetPCFlag(bPC);
	cmd.SetQPrio(qPrio);
	cmd.SetCQID(cqid);
	cmd.SetDXferMode(GetDataXferMode());

	uint32_t bufSize= NVME_Q_ENTRY_SIZE_SQ*(qSize+1);
	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,bufSize,nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::DeleteIOCQ(uint16_t qid,uint32_t timeout)
{
	PSDeleteIOCQ cmd	= PSDeleteIOCQ();
	cmd.SetQID(qid);

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::DeleteIOSQ(uint16_t qid,uint32_t timeout)
{
	PSDeleteIOSQ cmd	= PSDeleteIOSQ();
	cmd.SetQID(qid);

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::DoorbellBufCfg(Buffers* buf,uint32_t timeout)
{
	PSDoorbellBufCfg cmd	= PSDoorbellBufCfg();
	cmd.SetDXferMode(GetDataXferMode());

	void* dataBuf = (void*)(buf->GetBufferAddr());
	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,buf->GetBufSize(),nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::DeviceSelfTest(uint32_t nsid,uint8_t stc,uint32_t timeout)
{
	PSDeviceSelfTest cmd	= PSDeviceSelfTest();
	cmd.SetSTC(stc);
	cmd.SetNSID(nsid);

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::DirectiveRecv(uint8_t dOper,uint8_t dType,uint16_t dSpec,\
		uint32_t numDW,Buffers* buf,uint32_t timeout)
{
	PSDirectiveRecv cmd	= PSDirectiveRecv();
	cmd.SetNUMD(numDW);
	cmd.SetDOPER(dOper);
	cmd.SetDTYPE(dType);
	cmd.SetDSPEC(dSpec);
	cmd.SetDXferMode(GetDataXferMode());

	uint32_t bufSize= numDW*sizeof(uint32_t);
	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,bufSize,nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::DirectiveSend(uint8_t dOper,uint8_t dType,uint16_t dSpec,\
		uint32_t numDW,Buffers* buf,uint32_t timeout)
{
	PSDirectiveSend cmd	= PSDirectiveSend();
	cmd.SetNUMD(numDW);
	cmd.SetDOPER(dOper);
	cmd.SetDTYPE(dType);
	cmd.SetDSPEC(dSpec);
	cmd.SetDXferMode(GetDataXferMode());

	uint32_t bufSize= numDW*sizeof(uint32_t);
	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,bufSize,nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::FirmwareCommit(uint8_t slotID,uint8_t cAction,bool bBPID,uint32_t timeout)
{
	PSFirmwareCommit cmd		= PSFirmwareCommit();
	cmd.SetFS(slotID);
	cmd.SetCA(cAction);
	cmd.SetBPIDFlag(bBPID);

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::FirmwareDownload(uint32_t offset,uint32_t length,Buffers* buf,uint32_t timeout)
{
	PSFirmwareDownload cmd		= PSFirmwareDownload();
	cmd.SetNUMD(length);
	cmd.SetOFST(offset);

	cmd.SetDXferMode(GetDataXferMode());

	uint32_t bufSize= (length+1)*sizeof(uint32_t);
	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,bufSize,nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::GetFeatures(uint32_t nsid,uint8_t fid,uint8_t sel,\
		uint32_t cdw11,Buffers* buf,uint32_t timeout)
{
	PSGetFeature cmd		= PSGetFeature();
	cmd.SetFID(fid);
	cmd.SetSEL(sel);
	cmd.SetCDW11(cdw11);
	cmd.SetNSID(nsid);
	cmd.SetDXferMode(GetDataXferMode());

	void* dataBuf = (void*)(buf->GetBufferAddr());
	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,buf->GetBufSize(),nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::GetLogPage(uint32_t nsid,uint8_t lid,uint64_t offset,\
		uint32_t length,uint8_t lsp,bool bRAE,Buffers* buf,uint32_t timeout)
{
	PSGetLogPage cmd		= PSGetLogPage();
	cmd.SetLID(lid);
	cmd.SetLSP(lsp);
	cmd.SetRAEFlag(bRAE);
	cmd.SetNumOfDW(length);
	cmd.SetLogPageOffset(offset);
	cmd.SetNSID(nsid);
	cmd.SetDXferMode(GetDataXferMode());

	uint32_t bufSize= (length+1)*sizeof(uint32_t);
	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,bufSize,nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::Identify(uint8_t cns,uint16_t cntid,uint32_t nsid,\
		uint8_t csi, uint16_t cnssi,Buffers* buf,uint32_t timeout)
{
	PSIdentify	cmd			= PSIdentify();
	cmd.SetCNS(cns);
	cmd.SetCNTID(cntid);
	cmd.SetNSID(nsid);
	cmd.SetCSI(csi);
	cmd.SetCNSSI(cnssi);
	cmd.SetDXferMode(GetDataXferMode());

	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,buf->GetBufSize(),nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::KeepAlive(uint32_t timeout)
{
	PSKeepAlive cmd			= PSKeepAlive();

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::NVMeMIRecv(uint8_t nmsp0,Buffers* buf,uint32_t timeout)
{
	PSNVMeMIRecv cmd		= PSNVMeMIRecv();
	cmd.SetNMSP0(nmsp0);
	cmd.SetDXferMode(GetDataXferMode());

	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,buf->GetBufSize(),nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::NVMeMISend(uint8_t nmsp0,uint32_t length,Buffers* buf,uint32_t timeout)
{
	PSNVMeMISend cmd		= PSNVMeMISend();
	cmd.SetNMSP0(nmsp0);
	cmd.SetNUMD(length);
	cmd.SetDXferMode(GetDataXferMode());

	uint32_t bufSize= (length+1)*sizeof(uint32_t);
	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,bufSize,nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::NSAttach(uint32_t nsid,uint8_t sel,Buffers* buf,uint32_t timeout)
{
	PSNamespaceAttach cmd	= PSNamespaceAttach();
	cmd.SetSEL(sel);
	cmd.SetNSID(nsid);
	cmd.SetDXferMode(GetDataXferMode());

	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,buf->GetBufSize(),nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::NSMngmnt(uint32_t nsid,uint8_t sel,uint8_t csi,Buffers* buf,\
		uint32_t timeout)
{
	PSNamespaceMngr cmd		= PSNamespaceMngr();
	cmd.SetSEL(sel);
	cmd.SetNSID(nsid);
	cmd.SetCSI(csi);
	cmd.SetDXferMode(GetDataXferMode());

	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,buf->GetBufSize(),nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	/* Update NsMap in ParentContext , if xContext is loaded*/
//start
	if(this->IsParentContextLoaded() && resp->Succeeded())
	{
		ParentParameter* parentParam = parentContext->GetParentParameter();

		if(sel == 0x1)//Delete NS
		{
			if(nsid == NS_ID_SPECIAL)
			{
				parentParam->NsMap.clear();

				//Delete all ChildContext
				parentContext->ReleaseAllChildContext();
				//if xPattern loaded, release all pattern
				if(this->IsPatternMngrLoaded())
				{
					if(mpatMngr->ReleaseAllPattern() == false)
					{
						LOGWARN("faild to release all pattern in xPattern!");
					}
				}
			}
			else //delete one of allocated ns
			{
				//Delete nsid ChildContext
				parentContext->RemoveChildContext(nsid);
				//Delete pattern of this nsid
				if(this->IsPatternMngrLoaded())
				{
					mpatMngr->DeletePattern(nsid,0,parentParam->NsMap.at(nsid).nsCap);
				}

				uint64_t tmpCap		= parentParam->NsMap[nsid].nsCap;
				uint64_t tmpOffset	= parentParam->NsMap[nsid].offset;
				parentParam->NsMap.erase(nsid);
				for(const auto& pair : parentParam->NsMap)
				{
					if(pair.second.offset > tmpOffset)
					{
						parentParam->NsMap[pair.first].offset -= tmpCap;
					}
				}
			}
		}else{//Creat NS
			buf->SetEndianMode(BUFV_LITTLE_ENDIAN);
			uint8_t lbaF = buf->GetByte(NS_MANAGEMENT_BUF_LBAF_POS);

			NsMapInfo nsMsg = {
				.offset = 0,
				.nsCap 	= buf->GetQWord(NS_MANAGEMENT_BUF_NS_CAP_POS),
			};
			if(!parentParam->NsMap.empty())
			{
				auto maxElement = std::max_element(parentParam->NsMap.begin(),parentParam->NsMap.end(),CustomCompare);
				nsMsg.offset = maxElement->second.offset + maxElement->second.nsCap;
			}
			uint32_t nsidx = resp->GetRespVal();
			parentParam->NsMap[nsidx] = nsMsg;

			//Generate ChildContext
			ChildContext* child = parentContext->GetChildContext(nsidx);
			child->SetNsDataSize(parentParam->lbaMap[lbaF].sectorSize);
			child->SetNsMetaSize(parentParam->lbaMap[lbaF].metaSize);
			child->SetNsCap(nsMsg.nsCap);

			buf->SetEndianMode(BUFV_BIG_ENDIAN);
		}
	}

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::SetFeatures(uint32_t nsid,uint8_t fid,bool bSV,uint32_t cdw11,\
		Buffers* buf,uint32_t timeout)
{
	PSSetFeature cmd		= PSSetFeature();
	cmd.SetFID(fid);
	cmd.SetSVFlag(bSV);
	cmd.SetCDW11(cdw11);
	cmd.SetNSID(nsid);
	cmd.SetDXferMode(GetDataXferMode());

	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,buf->GetBufSize(),nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::VirtualMngr(uint16_t cntid,uint8_t rt,uint8_t act,\
		uint16_t numOfRes,uint32_t timeout)
{
	PSVirtualMngr cmd		= PSVirtualMngr();
	cmd.SetACT(act);
	cmd.SetRT(rt);
	cmd.SetCNTLID(cntid);
	cmd.SetNR(numOfRes);

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::FormatNVM(uint32_t nsid,uint8_t lbaf,uint8_t pi,\
		uint8_t ses,bool bMSET,bool bPIL,uint32_t timeout)
{
	PSFormatNVM cmd			= PSFormatNVM();
	cmd.SetNSID(nsid);
	cmd.SetLBAF(lbaf);
	cmd.SetMSETFlag(bMSET);
	cmd.SetPI(pi);
	cmd.SetPILFlag(bPIL);
	cmd.SetSES(ses);

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	//Config ChildContext
	if(this->IsParentContextLoaded() && resp->Succeeded())
	{
		ParentParameter* parentParam = parentContext->GetParentParameter();
		
		if(nsid == NS_ID_SPECIAL)// Format all namspaces
		{
			parentParam->NsMap.clear();
			uint64_t capOffset = 0;
			std::vector<ChildContext*>* children = parentContext->GetChildren();
			if(children != nullptr)
			{
				for(ChildContext* child : *children)
				{
					if(child->GetNsDataSize() > parentParam->lbaMap[lbaf].sectorSize)
					{
						child->SetNsCap(child->GetNsCap() * (child->GetNsDataSize() / parentParam->lbaMap[lbaf].sectorSize));
						NsMapInfo nsInfo = {
							.offset = capOffset,
							.nsCap	= child->GetNsCap()
						};
						parentParam->NsMap[child->GetNsID()] = nsInfo;
					}
					if(child->GetNsDataSize() < parentParam->lbaMap[lbaf].sectorSize)
					{
						child->SetNsCap(child->GetNsCap() / (parentParam->lbaMap[lbaf].sectorSize / child->GetNsDataSize()));
						NsMapInfo nsInfo = {
							.offset = capOffset,
							.nsCap	= child->GetNsCap()
						};
						parentParam->NsMap[child->GetNsID()] = nsInfo;
					}
					if(child->GetNsDataSize() == parentParam->lbaMap[lbaf].sectorSize)
					{
						NsMapInfo nsInfo = {
							.offset = capOffset,
							.nsCap	= child->GetNsCap()
						};
						parentParam->NsMap[child->GetNsID()] = nsInfo;
					}
					child->SetNsDataSize(parentParam->lbaMap[lbaf].sectorSize);
					child->SetNsMetaSize(parentParam->lbaMap[lbaf].metaSize);

					capOffset += child->GetNsCap();
				}
			}
			else
			{
				LOGWARN("fail to get namespace contexts");
			}

			//Release all PatternRecoder
			if(this->IsPatternMngrLoaded())
			{
				if(mpatMngr->ReleaseAllPattern() == false)
				{
					LOGWARN("faild to release all pattern in xPattern!");
				}
			}
		}
		else if(nsid != NS_ID_DEFAULT)// Format one of active namespace
		{
			ChildContext* child = parentContext->GetChildContext(nsid);

			if(child->GetNsDataSize() > parentParam->lbaMap[lbaf].sectorSize)
			{
				child->SetNsCap(child->GetNsCap() * (child->GetNsDataSize() / parentParam->lbaMap[lbaf].sectorSize));
				uint64_t capDiff = child->GetNsCap() - parentParam->NsMap.at(nsid).nsCap;
				for(auto& pair : parentParam->NsMap)
				{
					if(pair.first == nsid)
					{
						pair.second.nsCap = child->GetNsCap();
					}
					if(pair.second.offset > parentParam->NsMap.at(nsid).offset)
					{
						pair.second.offset += capDiff;
					}
				}
			}
			if(child->GetNsDataSize() < parentParam->lbaMap[lbaf].sectorSize)
			{
				child->SetNsCap(child->GetNsCap() / (parentParam->lbaMap[lbaf].sectorSize / child->GetNsDataSize()));
				uint64_t capDiff = parentParam->NsMap.at(nsid).nsCap - child->GetNsCap();
				for(auto& pair : parentParam->NsMap)
				{
					if(pair.first == nsid)
					{
						pair.second.nsCap = child->GetNsCap();
					}
					if(pair.second.offset > parentParam->NsMap.at(nsid).offset)
					{
						pair.second.offset -= capDiff;
					}
				}
			}

			child->SetNsDataSize(parentParam->lbaMap[lbaf].sectorSize);
			child->SetNsMetaSize(parentParam->lbaMap[lbaf].metaSize);

			//release pattern recorder of this namespace
			if(this->IsPatternMngrLoaded())
			{
				mpatMngr->DeletePattern(nsid,0,parentParam->NsMap.at(nsid).nsCap);
			}
		}
	}

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::Sanitize(uint8_t act,uint8_t passCnt,uint32_t overPat,\
		bool bAUSE,bool bOIPBP,bool bNDAS,uint32_t timeout)
{
	PSSanitize cmd			= PSSanitize();
	cmd.SetSANACT(act);
	cmd.SetAUSEFlag(bAUSE);
	cmd.SetOWPASS(passCnt);
	cmd.SetOIPBPFlag(bOIPBP);
	cmd.SetNDASFlag(bNDAS);
	cmd.SetOVRPAT(overPat);

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	//Release all PatternRecoder
	if(resp->Succeeded() && this->IsPatternMngrLoaded())
	{
		if(mpatMngr->ReleaseAllPattern() == false)
		{
			LOGWARN("faild to release all pattern in xPattern!");
		}
	}

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::SecurityRecv(uint8_t nssf,uint8_t spsp0,uint8_t spsp1,\
		uint8_t secp,uint32_t length,Buffers* buf,uint32_t timeout)
{
	PSSecurityRecv cmd		= PSSecurityRecv();
	cmd.SetNSSF(nssf);
	cmd.SetSPSP0(spsp0);
	cmd.SetSPSP1(spsp1);
	cmd.SetSECP(secp);
	cmd.SetAL(length);

	cmd.SetDXferMode(GetDataXferMode());

	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,length,nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeAdminCmd::SecuritySend(uint8_t nssf,uint8_t spsp0,uint8_t spsp1,\
		uint8_t secp,uint32_t length,Buffers* buf,uint32_t timeout)
{
	PSSecuritySend cmd		= PSSecuritySend();
	cmd.SetNSSF(nssf);
	cmd.SetSPSP0(spsp0);
	cmd.SetSPSP1(spsp1);
	cmd.SetSECP(secp);
	cmd.SetTL(length);
	cmd.SetDXferMode(GetDataXferMode());

	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,length,nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}


/*
 * Class NVMeNVMCmd
 */

bool NVMeNVMCmd::LoadChildContext(ChildContext* child)
{
	if(child == nullptr)
	{
		LOGWARN("Invalid argument childContext!");
		return false;
	}

	if(child->GetChildParameter() == nullptr)
	{
		LOGWARN("childContext has no valid content!");
		return false;
	}

	if(child->GetParent() == nullptr)
	{
		LOGERROR("childContext has no parentChontext!");
		return false;
	}

	childContext = child;
	return true;
}

bool NVMeNVMCmd::IsChildContextLoaded(void)
{
	bool res;
	res = (childContext == nullptr) ? false : true;
	if (res == false)
	{
		LOGERROR("nsid : %d context not loaded!",mnsid);
	}
	return res;
}

NVMeCmdResp* NVMeNVMCmd::IoPassthrough(uint32_t cdw0,uint32_t cdw1,uint32_t cdw2,uint32_t cdw3,\
		uint32_t cdw10,uint32_t cdw11,uint32_t cdw12, uint32_t cdw13,uint32_t cdw14,uint32_t cdw15,\
		uint32_t dBufLen,uint32_t mBufLen,Buffers* dBuf,Buffers* mBuf,uint32_t timeout)
{
	NVMeCmdReq cmd	= NVMeCmdReq(0,false,DataXfer_PRP,Fused_None);

	uint32_t*	cmdBuf	= (uint32_t*)cmd.GetCmdAddr();
	cmdBuf[0]	= cdw0;
	cmdBuf[1]	= cdw1;
	cmdBuf[2]	= cdw2;
	cmdBuf[3]	= cdw3;
	cmdBuf[10]	= cdw10;
	cmdBuf[11]	= cdw11;
	cmdBuf[12]	= cdw12;
	cmdBuf[13]	= cdw13;
	cmdBuf[14]	= cdw14;
	cmdBuf[15]	= cdw15;

	void* dataBuf = nullptr;
	void* metaBuf = nullptr;

	if(0 < mBufLen && nullptr != mBuf)
	{
		metaBuf = (void*)(mBuf->GetBufferAddr());
	}

	if(0 < dBufLen && nullptr != dBuf)
	{
		dataBuf = (void*)(dBuf->GetBufferAddr());
	}

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,dBufLen,metaBuf,mBufLen,timeout);

	if(0 < dBufLen)
	{
		resp->SetDataBuffer(dBuf);
	}

	if(0 < mBufLen)
	{
		resp->SetMetaBuffer(mBuf);
	}

	return resp;
}

NVMeCmdResp* NVMeNVMCmd::Compare(uint64_t slba,uint16_t length,uint8_t prinfo,\
		bool bFUA,bool bLR,bool bSTC,uint16_t elbat,uint16_t elbatm,uint64_t elbst,uint64_t elbrf,Buffers* dBuf,Buffers* mBuf,uint32_t timeout)

{
	PSCompare cmd		= PSCompare();
	cmd.SetSLBA(slba);
	cmd.SetNLB(length);
	cmd.SetPRINFO(prinfo);
	cmd.SetFUAFlag(bFUA);
	cmd.SetLRFlag(bLR);
	cmd.SetELBAT(elbat);
	cmd.SetELBATM(elbatm);
	cmd.SetSTCFlag(bSTC);
	cmd.SetFusedMode(GetFusedMode());
	cmd.SetDXferMode(GetDataXferMode());
	cmd.SetNSID(mnsid);

	this->IsChildContextLoaded();
	uint32_t sectorSize = childContext->GetNsDataSize();
	uint32_t metaSize = childContext->GetNsMetaSize();
	uint32_t dBufLen = (length + 1) * sectorSize;
	uint32_t mBufLen = (length + 1) * metaSize;

	uint32_t piGuardSize = childContext->GetNsPIGuardSize();
	uint32_t pisTagSize = childContext->GetNsPISTagSize();

	if(false == SetCmdPIParam((NVMeCmdReq*)&cmd,pisTagSize,piGuardSize,elbst,elbrf))
	{
		NVMeCmdResp* resp	= new NVMeCmdResp();
		return resp;
	}

	void* dataBuf = nullptr;
	void* metaBuf = nullptr;

	if(0 < mBufLen && nullptr != mBuf)
	{
		metaBuf = (void*)(mBuf->GetBufferAddr());
	}

	if(0 < dBufLen && nullptr != dBuf)
	{
		dataBuf = (void*)(dBuf->GetBufferAddr());
	}

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,dBufLen,metaBuf,mBufLen,timeout);


	if(0 < dBufLen)
	{
		resp->SetDataBuffer(dBuf);
	}

	if(0 < mBufLen)
	{
		resp->SetMetaBuffer(mBuf);
	}

	return resp;
}

NVMeCmdResp* NVMeNVMCmd::DataSetMngr(bool bIDR,bool bIDW,bool bAD,uint8_t numOfRng,\
		Buffers* buf,uint32_t timeout)
{
	PSDataSetMngr cmd = 		PSDataSetMngr();
	cmd.SetNR(numOfRng);
	cmd.SetIDRFlag(bIDR);
	cmd.SetIDWFlag(bIDW);
	cmd.SetADFlag(bAD);
	cmd.SetNSID(mnsid);

	uint32_t bufSize	= (numOfRng+1) * BYTES_PER_RANGE;
	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,bufSize,nullptr,0,timeout);

	//Delete Pattern
	if(true == resp->Succeeded() && true == this->IsPatternMngrLoaded() && bAD == 1)
	{
		uint16_t offset = 8;

		for(uint16_t i = 0;i<numOfRng;i++)
		{
			uint64_t slba = buf->GetQWord(offset);
			uint32_t length = buf->GetDWord(offset-4);

			mpatMngr->DeletePattern(mnsid,slba,length);
			offset += BYTES_PER_RANGE;
		}
	}

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeNVMCmd::Flush(uint32_t timeout)
{
	PSFlush cmd =			PSFlush();
	cmd.SetNSID(mnsid);

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	return resp;
}

NVMeCmdResp* NVMeNVMCmd::Read(uint64_t slba,uint16_t length,uint8_t prinfo,\
		bool bFUA,bool bLR,bool bSTC,uint8_t dsm,uint16_t elbat,uint16_t elbatm,uint64_t elbst,uint64_t elbrf,Buffers* dBuf,\
		Buffers* mBuf,uint32_t timeout)
{
	PSRead cmd	=			PSRead();
	cmd.SetSLBA(slba);
	cmd.SetNLB(length);
	cmd.SetPRINFO(prinfo);
	cmd.SetFUAFlag(bFUA);
	cmd.SetLRFlag(bLR);
	cmd.SetDSM(dsm);
	cmd.SetELBAT(elbat);
	cmd.SetELBATM(elbatm);
	cmd.SetSTCFlag(bSTC);
	cmd.SetFusedMode(GetFusedMode());
	cmd.SetDXferMode(GetDataXferMode());
	cmd.SetNSID(mnsid);

	this->IsChildContextLoaded();
	uint32_t sectorSize = childContext->GetNsDataSize();
	uint32_t metaSize = childContext->GetNsMetaSize();
	uint32_t dBufLen = (length + 1) * sectorSize;
	uint32_t mBufLen = (length + 1) * metaSize;

	uint32_t piGuardSize = childContext->GetNsPIGuardSize();
	uint32_t pisTagSize = childContext->GetNsPISTagSize();

	if(false == SetCmdPIParam((NVMeCmdReq*)&cmd,pisTagSize,piGuardSize,elbst,elbrf))
	{
		NVMeCmdResp* resp	= new NVMeCmdResp();
		return resp;
	}

	void* dataBuf = nullptr;
	void* metaBuf = nullptr;

	if(0 < mBufLen && nullptr != mBuf)
	{
		metaBuf = (void*)(mBuf->GetBufferAddr());
	}

	if(0 < dBufLen && nullptr != dBuf)
	{
		dataBuf = (void*)(dBuf->GetBufferAddr());
	}

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,dBufLen,metaBuf,mBufLen,timeout);

	if(0 < dBufLen)
	{
		resp->SetDataBuffer(dBuf);
	}

	if(0 < mBufLen)
	{
		resp->SetMetaBuffer(mBuf);
	}

	return resp;
}

NVMeCmdResp* NVMeNVMCmd::ResvAcquire(uint8_t action,uint8_t type,bool bIEKEY,\
		Buffers* buf,uint32_t timeout)
{
	PSResvAcquire cmd			= PSResvAcquire();
	cmd.SetRACQA(action);
	cmd.SetRTYPE(type);
	cmd.SetIEKEYFlag(bIEKEY);

	cmd.SetDXferMode(GetDataXferMode());
	cmd.SetNSID(mnsid);

	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,buf->GetBufSize(),nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeNVMCmd::ResvRegister(uint8_t action,uint8_t cptpl,bool bIEKEY,\
		Buffers* buf,uint32_t timeout)
{
	PSResvRegister cmd			= PSResvRegister();
	cmd.SetRREGA(action);
	cmd.SetIEKEYFlag(bIEKEY);
	cmd.SetCPTPL(cptpl);

	cmd.SetDXferMode(GetDataXferMode());
	cmd.SetNSID(mnsid);

	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,buf->GetBufSize(),nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeNVMCmd::ResvRelease(uint8_t action,uint8_t type,bool bIEKEY,\
		Buffers* buf,uint32_t timeout)
{
	PSResvRelease cmd			= PSResvRelease();
	cmd.SetRRELA(action);
	cmd.SetRTYPE(type);
	cmd.SetIEKEYFlag(bIEKEY);

	cmd.SetDXferMode(GetDataXferMode());
	cmd.SetNSID(mnsid);

	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,buf->GetBufSize(),nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeNVMCmd::ResvReport(uint32_t length,bool bEDS,Buffers* buf,uint32_t timeout)
{
	PSResvReport cmd			= PSResvReport();
	cmd.SetEDSFlag(bEDS);
	cmd.SetNUMD(length);
	cmd.SetDXferMode(GetDataXferMode());
	cmd.SetNSID(mnsid);

	uint32_t bufSize= sizeof(uint32_t)*(length+1);
	void* dataBuf = (void*)(buf->GetBufferAddr());

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,bufSize,nullptr,0,timeout);

	resp->SetDataBuffer(buf);

	return resp;
}

NVMeCmdResp* NVMeNVMCmd::Write(uint64_t slba,uint16_t length,uint8_t prinfo,\
		bool bFUA,bool bLR,bool bSTC,uint8_t dtype,uint16_t dspec,uint8_t dsm,uint16_t lbat,uint16_t lbatm,uint64_t lbst,uint64_t ilbrf,\
		Buffers* dBuf,Buffers* mBuf,uint32_t timeout)
{
	PSWrite cmd			= PSWrite();
	cmd.SetSLBA(slba);
	cmd.SetNLB(length);
	cmd.SetPRINFO(prinfo);
	cmd.SetFUAFlag(bFUA);
	cmd.SetLRFlag(bLR);
	cmd.SetDSM(dsm);
	cmd.SetDSPEC(dspec);
	cmd.SetLBAT(lbat);
	cmd.SetLBATM(lbatm);
	cmd.SetSTCFlag(bSTC);
	cmd.SetDTYPE(dtype);
	cmd.SetFusedMode(GetFusedMode());
	cmd.SetDXferMode(GetDataXferMode());
	cmd.SetNSID(mnsid);

	this->IsChildContextLoaded();
	uint32_t sectorSize = childContext->GetNsDataSize();
	uint32_t metaSize = childContext->GetNsMetaSize();
	uint32_t dBufLen = (length + 1) * sectorSize;
	uint32_t mBufLen = (length + 1) * metaSize;

	uint32_t piGuardSize = childContext->GetNsPIGuardSize();
	uint32_t pisTagSize = childContext->GetNsPISTagSize();

	// LOGDEBUG("pisTagSize=%u,piGuardSize=%u,lbst=%u,ilbrf=%u",pisTagSize,piGuardSize,lbst,ilbrf);

	if(false == SetCmdPIParam((NVMeCmdReq*)&cmd,pisTagSize,piGuardSize,lbst,ilbrf))
	{
		NVMeCmdResp* resp	= new NVMeCmdResp();
		return resp;
	}

	void* dataBuf = nullptr;
	void* metaBuf = nullptr;

	if(0 < mBufLen && nullptr != mBuf)
	{
		metaBuf = (void*)(mBuf->GetBufferAddr());
	}

	if(true == this->IsPatternMngrLoaded() && true == mpatMngr->IsAddrOverlay())
	{
		dBuf->MarkAddrOverlay(0,length+1,slba);
	}

	if(0 < dBufLen && nullptr != dBuf)
	{
		dataBuf = (void*)(dBuf->GetBufferAddr());
	}

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,dataBuf,dBufLen,metaBuf,mBufLen,timeout);

	//Update Pattern
	if(true == resp->Succeeded() && true == this->IsPatternMngrLoaded())
	{
		mpatMngr->UptPatRecFromIOWrBuf(dBuf,0,mnsid,slba,length + 1);
	}

	if(0 < dBufLen)
	{
		resp->SetDataBuffer(dBuf);
	}

	if(0 < mBufLen)
	{
		resp->SetMetaBuffer(mBuf);
	}

	return resp;
}

NVMeCmdResp* NVMeNVMCmd::WriteUNC(uint64_t slba,uint16_t length,uint32_t timeout)
{
	PSWriteUNC cmd			= PSWriteUNC();
	cmd.SetSLBA(slba);
	cmd.SetNLB(length);
	cmd.SetNSID(mnsid);

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	return resp;
}

NVMeCmdResp* NVMeNVMCmd::WriteZeroes(uint64_t slba,uint16_t length,uint8_t prinfo,\
		bool bFUA,bool bLR,bool bDEAC,uint32_t timeout)
{
	PSWriteZeroes cmd			= PSWriteZeroes();
	cmd.SetSLBA(slba);
	cmd.SetNLB(length);
	cmd.SetPRINFO(prinfo);
	cmd.SetFUAFlag(bFUA);
	cmd.SetLRFlag(bLR);
	cmd.SetDEACFlag(bDEAC);
	cmd.SetLBAT(GetInitRefTag());
	cmd.SetLBATM(GetAppTagMask());
	cmd.SetILBRT(GetInitRefTag());
	cmd.SetFusedMode(GetFusedMode());
	cmd.SetDXferMode(GetDataXferMode());
	cmd.SetNSID(mnsid);

	NVMeCmdResp* resp	= GetEngine()->NVMePassthru(cmd,nullptr,0,nullptr,0,timeout);

	//TODO: Update Pattern

	return resp;
}
