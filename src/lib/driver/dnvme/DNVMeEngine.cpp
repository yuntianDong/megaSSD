/*
 * file : DNVMeEngine.cpp
 */

#include "driver/dnvme/DNVMeIOCtrl.h"
#include "driver/dnvme/DNVMeEngine.h"

#define ADMIN_CMD_OPCODE_CREATEIOSQ			0x01
#define ADMIN_CMD_OPCODE_CREATEIOCQ			0x05

#define IOQ_BUFFER_SIZE						4096
#define IOQ_BUFFER_ALIGN					4
#define IOQ_CREATE_CMD_TIMEOUT				8000
#define IOQ_SQ_QPRIO_DEF					2	// Medium

DNVMeEngine::DNVMeEngine(int fd)
		:NVMeEngine(fd)
{
	mAdminCQDepth	= DEF_ADMIN_QDEPTH_CQ;
	mAdminSQDepth	= DEF_ADMIN_QDEPTH_SQ;
	mIOCQDepth		= DEF_IOQ_QDEPTH_CQ;
	mIOSQDepth		= DEF_IOQ_QDEPTH_SQ;
	mIOQNum			= DEF_IOQ_NUMBER;

	DNVMeControl::GetInstance(mFd)->SetCtrlrState(NVME_DEV_ENABLE);
}

NVMeCmdResp* DNVMeEngine::CreateIOCQ(uint16_t qid,uint16_t qSize,uint16_t iv,\
		bool bIEN,bool bPC,Buffers* buf,uint32_t timeout)
{
	PSCreateIOCQ cmd	= PSCreateIOCQ();
	cmd.SetQID(qid);
	cmd.SetQSize(qSize);
	cmd.SetPCFlag(bPC);
	cmd.SetIENFlag(bIEN);
	cmd.SetIV(iv);
	cmd.SetDXferMode(DataXfer_PRP);

	uint32_t bufSize= NVME_Q_ENTRY_SIZE_CQ*(qSize+1);
	CONFIG_CMD_DATABUF(buf->GetBufferAddr(),bufSize)

	NVMeCmdResp* resp	= NVMePassthru(cmd,timeout);

	return resp;
}

NVMeCmdResp* DNVMeEngine::CreateIOSQ(uint16_t qid,uint16_t qSize,uint16_t cqid,\
		uint8_t qPrio,bool bPC,Buffers* buf,uint32_t timeout)
{
	PSCreateIOSQ cmd	= PSCreateIOSQ();
	cmd.SetQID(qid);
	cmd.SetQSize(qSize);
	cmd.SetPCFlag(bPC);
	cmd.SetQPrio(qPrio);
	cmd.SetCQID(cqid);
	cmd.SetDXferMode(DataXfer_PRP);

	uint32_t bufSize= NVME_Q_ENTRY_SIZE_SQ*(qSize+1);
	CONFIG_CMD_DATABUF(buf->GetBufferAddr(),bufSize)

	NVMeCmdResp* resp	= NVMePassthru(cmd,timeout);

	return resp;
}

bool DNVMeEngine::CreateIOQueues(void)
{
	for(uint16_t idx=0;idx<mIOQNum;idx++)
	{
		uint16_t cqId	= idx+1;
		uint16_t sqId	= cqId;

		Buffers* cqBuf	= (Buffers*)new Buffers(IOQ_BUFFER_SIZE,IOQ_BUFFER_ALIGN);
		Buffers* sqBuf	= (Buffers*)new Buffers(IOQ_BUFFER_SIZE,IOQ_BUFFER_ALIGN);

		NVMeCmdResp* resp = CreateIOCQ(cqId,mIOCQDepth-1,muIntVector,mbIEnabled,true,\
				cqBuf,IOQ_CREATE_CMD_TIMEOUT);
		if(false == resp->Succeeded())
		{
			return false;
		}

		NVMeCmdResp* resp = CreateIOSQ(sqId,mIOSQDepth-1,cqId,IOQ_SQ_QPRIO_DEF,true,\
				sqBuf,IOQ_CREATE_CMD_TIMEOUT);
		if(false == resp->Succeeded())
		{
			return false;
		}



	}
}

IOQPair DNVMeEngine::GetIOQPair(void)
{
	IOQPair ioQ	= DNVMeQMngr::GetInstance(mFd)->GetNextIOQPair();
	if(NULL == ioQ.first)
	{
		if(true == CreateIOQueues())
		{
			ioQ	= DNVMeQMngr::GetInstance(mFd)->GetNextIOQPair();
		}
	}

	return ioQ;
}

DNVMeCQ* DNVMeEngine::GetAdminCQ(void)
{
	DNVMeCQ* cq = DNVMeQMngr::GetInstance(mFd)->GetAdminCQ();
	if(NULL == cq)
	{
		if(true == DNVMeControl::GetInstance(mFd)->CreateAdminCQ(mAdminCQDepth))
		{
			cq = DNVMeQMngr::GetInstance(mFd)->GetAdminCQ();
		}
	}

	return cq;
}

DNVMeSQ* DNVMeEngine::GetAdminSQ(void)
{
	DNVMeSQ* sq = DNVMeQMngr::GetInstance(mFd)->GetAdminSQ();
	if(NULL == sq)
	{
		if(true == DNVMeControl::GetInstance(mFd)->CreateAdminSQ(mAdminSQDepth))
		{
			sq = DNVMeQMngr::GetInstance(mFd)->GetAdminSQ();
		}
	}

	return sq;
}

bool DNVMeEngine::CfgAdminQDepth(uint16_t cqDepth,uint16_t sqDepth)
{
	mAdminCQDepth	= cqDepth;
	mAdminSQDepth	= sqDepth;

	return true;
}

bool DNVMeEngine::CfgIOQueue(uint16_t qNum,uint16_t cqDepth,uint16_t sqDepth)
{
	mIOCQDepth	= cqDepth;
	mIOSQDepth	= sqDepth;
	mIOQNum		= qNum;

	return true;
}

bool DNVMeEngine::SetCQInt(bool ien,uint16_t iv)
{
	mbIEnabled	= ien;
	muIntVector	= iv;

	return true;
}

NVMeCmdResp* DNVMeEngine::NVMePassthru(NVMePassthruCmd &req,uint32_t timeout)
{
	int ret = 0;
	struct timeval start;
	struct timeval end;

	DNVMeCQ*	pCQ = NULL;
	DNVMeSQ*	pSQ = NULL;

	if(true == req.isAdminCmd())
	{
		pCQ	= GetAdminCQ();
		pSQ = GetAdminSQ();
	}
	else
	{
		IOQPair ioQPair = GetIOQPair();
		pCQ = ioQPair.first;
		pSQ = ioQPair.second;
	}

	if(NULL == pCQ || NULL == pSQ)
	{
		return new NVMeCmdResp();
	}

	if(true == req.isAdminCmd())
	{
		if(ADMIN_CMD_OPCODE_CREATEIOSQ == req.GetOpCode())
		{
			if(false == DNVMeControl::GetInstance(mFd)->PrepareSQCreation(req))
			{
				return new NVMeCmdResp();
			}
		}
		else if(ADMIN_CMD_OPCODE_CREATEIOCQ == req.GetOpCode())
		{
			if(false == DNVMeControl::GetInstance(mFd)->PrepareCQCreation(req))
			{
				return new NVMeCmdResp();
			}
		}
	}

	pCQ->SetTimeOutMS(timeout);

	gettimeofday(&start,NULL);
	uint16_t cmdID	= pSQ->SendCmd(req);
	if(false == pSQ->Ring())
	{
		return new NVMeCmdResp();
	}

	NVMeCmdResp* resp = pCQ->GetCQEntry();

	gettimeofday(&end,NULL);

	if(NULL != resp && cmdID == resp->GetCID())
	{
		resp->SetExecTime((end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));
	}

	return resp;
}

bool DNVMeEngine::NVMeReset(void)
{
	return false;
}

bool DNVMeEngine::NVMeRescan(void)
{
	return false;
}

bool DNVMeEngine::NVMeSubSysReset(void)
{
	return false;
}

