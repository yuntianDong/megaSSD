/*
 * file : DNVMeQueue.cpp
 */
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "driver/dnvme/DNVMeQueue.h"
#include "driver/dnvme/DNVMeIOCtrl.h"
#include "driver/dnvme/DNVMeUtil.h"
#include "driver/dnvme/DNVMeMetaBuf.h"

#define	OPCODE_DATADIR_BITMASK		0b00000011
#define	SMALL_DATACMD_PRPMASK		(NVME_MASK_PRP1_PAGE | NVME_MASK_PRP2_PAGE)
#define LARGE_DATACMD_PRPMASK		(NVME_MASK_PRP1_PAGE | NVME_MASK_PRP2_PAGE | NVME_MASK_PRP2_LIST)
#define DOUBLE_MEMORY_PAGE_SIZE		4096*2
#define INVALID_MERA_BUFID			(uint32_t)-1

bool CalTimeOut(uint32_t ms, struct timeval &initial, uint32_t &delta)
{
    struct timeval current;

    if (gettimeofday(&current, NULL) != 0)
        return true;

    time_t initial_us = (((time_t)1000000 * initial.tv_sec) + initial.tv_usec);
    time_t current_us = (((time_t)1000000 * current.tv_sec) + current.tv_usec);
    time_t timeout_us = ((time_t)ms * (time_t)1000);
    time_t delta_us = (current_us - initial_us);

    delta = (delta_us / 1000UL);

    return (delta_us >= timeout_us);
}

DNVMeQueue::~DNVMeQueue(void)
{
	if(true == mbContinBuf && NULL != mpQBuf)
	{
		DNVMeUtil::munmap(mpQBuf->GetBufferPoint(),mpQBuf->GetBufSize());
	}

	if(NULL != mpQBuf)
	{
		delete mpQBuf;
		mpQBuf = NULL;
	}
}

DNVMeCQ::DNVMeCQ(int devHdlr,uint16_t qId)
	:DNVMeQueue(devHdlr),mIrqEnabled(false),mIrqNo(0),mHeadPtr(-1),mTailPtr(-1),mPbitNewEntry(false)
{
	GetCQMetrics(qId);

	uint32_t	bufSize	= GetEntrySize()*GetNumEntries();
	mpReapBuf	= new Buffers(bufSize,QUEUE_BUF_ALIGN);
	mReapCount	= mLeftCount = 0;
	mTimeoutms	= DEF_REAP_INQUIRY_TIMEOUT;
}

DNVMeCQ::~DNVMeCQ(void)
{
	if(NULL != mpReapBuf)
	{
		delete mpReapBuf;
		mpReapBuf = NULL;
	}
}

bool DNVMeCQ::GetCQMetrics(uint16_t qId)
{
	uint8_t 	irqEnabled;
	uint16_t	irqNo;
	uint16_t	headPtr;
	uint16_t	tailPtr;
	uint8_t		pbitNewEntry;
	uint32_t 	numEntries;

	int rtn = DNVMeIOCtrl::GetInstance(GetDevHdlr())->IOCtlGetCQMetrics(qId,
			tailPtr,headPtr,numEntries,irqEnabled,irqNo,pbitNewEntry);

	if(IOCTL_CALL_SUCCESS != rtn)
	{
		return false;
	}

	Init(qId,CQ_ENTRY_SIZE,numEntries);
	mIrqEnabled	= irqEnabled;
	mIrqNo		= irqNo;
	mHeadPtr	= headPtr;
	mTailPtr	= tailPtr;
	mPbitNewEntry	= pbitNewEntry;

	SetValid();

	return true;
}

int DNVMeCQ::ReapInquiry(uint32_t &isrCount)
{
	uint32_t numCmds = 0;

	isrCount	= 0;
	int rtn = DNVMeIOCtrl::GetInstance(GetDevHdlr())->IOCtlReapInquiry(
			GetQueueID(),numCmds,isrCount);

	if(IOCTL_CALL_SUCCESS == rtn)
	{
		return numCmds;
	}

	return 0;
}

bool DNVMeCQ::ReapInquiryWaitAny(uint32_t ms, uint32_t &numCE,uint32_t &isrCount)
{
	uint32_t delta	= 0;
	numCE = 0;

    struct timeval initial;
    if (gettimeofday(&initial, NULL) != 0)
    {
    	return false;
    }

    while (::CalTimeOut(ms, initial, delta) == false) {
        if ((numCE = ReapInquiry(isrCount)) != 0) {
            return true;
        }
        usleep(10);
    }

    return false;
}

NVMeCmdResp* DNVMeCQ::GetCmdRespFromReapBuf(void)
{
	if(0 <= mReapCount || 0 <=mLeftCount)
	{
		return NULL;
	}

	int respIdx	= mReapCount - mLeftCount;

	NVMeCmdResp* resp = new NVMeCmdResp();
	uint8_t* respBuf  = (uint8_t*)(resp->GetRespAddr());

	for(uint8_t idx=0;idx<CQ_ENTRY_SIZE;idx++)
	{
		*(respBuf++)	= mpReapBuf->GetByte(respIdx*CQ_ENTRY_SIZE + idx);
	}

	return resp;
}

bool DNVMeCQ::DoReap(int numCmds,uint32_t &isrCount)
{
	uint8_t* pQBuf	= mpReapBuf->GetBufferPoint();
	uint32_t bufLen	= numCmds*CQ_ENTRY_SIZE;

	uint32_t numReaped,numLeft;
	numReaped = numLeft = isrCount = 0;

	int rtn = DNVMeIOCtrl::GetInstance(GetDevHdlr())->IOCtlReap(
			GetQueueID(),numCmds,pQBuf,bufLen,numReaped,numLeft,isrCount);

	if(IOCTL_CALL_SUCCESS == rtn)
	{
		mReapCount = mLeftCount = numReaped;
	}

	return IOCTL_CALL_SUCCESS == rtn;
}

bool DNVMeCQ::ReapNumCmds(uint32_t maxNums)
{
	uint32_t numCE 		= 0;
	uint32_t isrCount 	= 0;

	if(false == ReapInquiryWaitAny(mTimeoutms,numCE,isrCount))
	{
		return false;
	}

	if(0 < maxNums && maxNums < numCE)
	{
		numCE = maxNums;
	}

	if(false == DoReap(numCE,isrCount))
	{
		return false;
	}

	return true;
}

NVMeCmdResp* DNVMeCQ::GetCQEntry(void)
{
	if(false == ReapNumCmds(1))
	{
		return NULL;
	}

	return GetCmdRespFromReapBuf();
}

NVMeCmdResp* DNVMeCQ::GetNextCQEntry(void)
{
	NVMeCmdResp* resp = GetCmdRespFromReapBuf();
	if(NULL == resp && false == ReapNumCmds())
	{
		return NULL;
	}

	return GetCmdRespFromReapBuf();
}

Buffers* DNVMeCQ::GetQueueBuf(void)
{
	Buffers* buf = this->GetQueueBuf();
	if(NULL == buf && true == IsContinBuf())
	{
		uint32_t bufSize	= GetQueueSize();
		uint8_t * b = DNVMeUtil::mmap(GetDevHdlr(),bufSize,GetQueueID(),DNVMeUtil::MMR_CQ);
		buf = (Buffers*)new Buffers(b,bufSize,QUEUE_BUF_ALIGN);

		SetQueueBuf(buf,true);
	}

	return buf;
}

DNVMeSQ::DNVMeSQ(int devHdlr,uint16_t qId)
	:DNVMeQueue(devHdlr),mCQId(0),mHeadPtr(-1),mTailPtr(-1),mTailPtrVirt(0)
{
	GetSQMetrics(qId);
}

bool DNVMeSQ::GetSQMetrics(uint16_t qId)
{
	uint16_t	cqId;
	uint16_t	headPtr;
	uint16_t	tailPtr;
	uint16_t	tailPtrVirt;
	uint32_t 	numEntries;

	int rtn = DNVMeIOCtrl::GetInstance(GetDevHdlr())->IOCtlGetSQMetrics(qId,
			cqId,tailPtr,headPtr,numEntries,tailPtrVirt);

	if(IOCTL_CALL_SUCCESS != rtn)
	{
		return false;
	}

	Init(qId,SQ_ENTRY_SIZE,numEntries);

	mCQId		= cqId;
	mHeadPtr	= headPtr;
	mTailPtr	= tailPtr;
	mTailPtrVirt= tailPtrVirt;

	SetValid();

	return true;
}

void DNVMeSQ::NVMePassthruTo64BCmd(NVMePassthruCmd& cmd,uint8_t* p64BCmd)
{
	uint8_t* pCmdBuf	= (uint8_t*)(cmd.GetCmdAddr());

	memcpy(p64BCmd,pCmdBuf,SQ_ENTRY_SIZE);
	p64BCmd[4]	= 0;	// DW4~5:MPTR
	p64BCmd[5]	= 0;	// DW4~5:MPTR
	p64BCmd[6]	= 0;	// DW6~9:DPTR
	p64BCmd[7]	= 0;	// DW6~9:DPTR
	p64BCmd[8]	= 0;	// DW6~9:DPTR
	p64BCmd[9]	= 0;	// DW6~9:DPTR
}

uint32_t DNVMeSQ::GetMetaBufIDFromCmd(NVMePassthruCmd& cmd)
{
	uint32_t metaBufSize	= cmd.GetMetaBufSize();
	if(0 == metaBufSize)
	{
		return INVALID_MERA_BUFID;
	}

	uint8_t* pMetaBuf		= (uint8_t*)(cmd.GetMetaBuf());
	uint32_t nsID			= cmd.GetNSID();
	uint32_t metaID			= INVALID_MERA_BUFID;

	Buffers* buf	= DNVMeMetaBuf::GetInstance(GetDevHdlr())->GetMetaBuf(metaID);
	if(NULL == buf || (buf->GetBufSize() < metaBufSize))
	{
		return INVALID_MERA_BUFID;
	}

	memcpy(buf->GetBufferPoint(),pMetaBuf,metaBufSize);

	return metaID;
}

bool DNVMeSQ::Ring(void)
{
	int rtn = DNVMeIOCtrl::GetInstance(GetDevHdlr())->IOCtlRingSQDoorbell(GetQueueID());

	return IOCTL_CALL_SUCCESS == rtn;
}

uint16_t DNVMeSQ::SendCmd(NVMePassthruCmd& cmd)
{
	uint8_t CmdBuf[SQ_ENTRY_SIZE] = {0};
	uint16_t cmdID	= INVALID_CMDID;
	uint8_t OpCode	= cmd.GetOpCode();
	emDataDir dataDir	= (emDataDir)(OpCode & OPCODE_DATADIR_BITMASK);
	uint32_t dataSize	= cmd.GetDataBufSize();
	uint8_t *pDataBuf	= (uint8_t *)(cmd.GetDataBuf());
	uint32_t metaBufID	= GetMetaBufIDFromCmd(cmd);
	emDataPointerMask prpMask	= (emDataPointerMask)0;

	if(0 < dataSize && dataSize < DOUBLE_MEMORY_PAGE_SIZE)
	{
		prpMask	= (emDataPointerMask)SMALL_DATACMD_PRPMASK;
	}
	else if(dataSize > DOUBLE_MEMORY_PAGE_SIZE)
	{
		prpMask = (emDataPointerMask)LARGE_DATACMD_PRPMASK;
	}

	if(INVALID_MERA_BUFID != metaBufID)
	{
		prpMask = (emDataPointerMask)(prpMask |NVME_MASK_MPTR);
	}
	else
	{
		prpMask = (emDataPointerMask)(prpMask & ~NVME_MASK_MPTR);
		metaBufID = 0;
	}

	NVMePassthruTo64BCmd(cmd,CmdBuf);

	int rtn = DNVMeIOCtrl::GetInstance(GetDevHdlr())->IOCtlSend64BCmd(cmdID,
			GetQueueID(),CmdBuf,dataDir,dataSize,pDataBuf,prpMask,metaBufID);

	if(IOCTL_CALL_SUCCESS != rtn)
	{
		return INVALID_CMDID;
	}

	return cmdID;
}

Buffers* DNVMeSQ::GetQueueBuf(void)
{
	Buffers* buf = this->GetQueueBuf();
	if(NULL == buf && true == IsContinBuf())
	{
		uint32_t bufSize	= GetQueueSize();
		uint8_t * b = DNVMeUtil::mmap(GetDevHdlr(),bufSize,GetQueueID(),DNVMeUtil::MMR_SQ);
		buf = (Buffers*)new Buffers(b,bufSize,QUEUE_BUF_ALIGN);

		SetQueueBuf(buf,true);
	}

	return buf;
}
