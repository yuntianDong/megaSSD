#ifndef _DNVME_QUEUE_H
#define _DNVME_QUEUE_H

#include "buffer/Buffers.h"
#include "driver/nvme/NVMeCmdResp.h"
#include "driver/nvme/NVMeCmdReq.h"

#define	CQ_ENTRY_SIZE				16
#define SQ_ENTRY_SIZE				64
#define QUEUE_BUF_ALIGN				4
#define DEF_QUEUE_DEPTH_ACQ			16
#define DEF_QUEUE_DEPTH_ASQ			16
#define ADMIN_QID_CQ				0
#define ADMIN_QID_SQ				0

#define DEF_REAP_INQUIRY_TIMEOUT	8*1000
#define INVALID_CMDID				(uint16_t)-1

class DNVMeQueue
{
private:
	int			mDevHdlr;
	bool		mbValid;

    uint16_t 	mQId;
    uint16_t 	mEntrySize;
    uint32_t 	mNumEntries;

    bool		mbContinBuf;
    Buffers*	mpQBuf;
protected:
    virtual void Init(uint16_t qID,uint16_t entrySize,uint32_t numEntries)
    {
    	mQId		= qID;
    	mEntrySize	= entrySize;
    	mNumEntries	= numEntries;
    };
    int	GetDevHdlr(void){return mDevHdlr;};

    DNVMeQueue(int devHdlr)
    	:mDevHdlr(devHdlr),mbValid(false),mQId(0),mEntrySize(0),mNumEntries(0),mbContinBuf(true),mpQBuf(NULL)
    {};

    void SetValid(void){mbValid=true;};

    void		SetQueueBuf(Buffers* buf,bool bContin)
    {
    	mpQBuf		= buf;
    	mbContinBuf	= bContin;
    }
public:
    virtual ~DNVMeQueue(void);

    virtual bool IsValidQ(void){return mbValid;};

    void		DumpQBuf(uint32_t offset,uint32_t length)
    {
    	Buffers* buf = this->GetQueueBuf();
    	if(NULL != buf && offset < buf->GetBufSize())
    	{
    		if((offset + length) > buf->GetBufSize())
    		{
    			length = buf->GetBufSize() - offset;
    		}

    		buf->Dump(offset,length);
    	}
    }

    uint16_t	GetQueueID(void) {return mQId;};
    uint16_t	GetEntrySize(void) {return mEntrySize;};
    uint32_t	GetNumEntries(void) {return mNumEntries;};
    uint32_t	GetQueueSize(void) {return GetEntrySize()*GetNumEntries();};
    virtual Buffers* GetQueueBuf(void) {return mpQBuf;};
    bool		SetQueueBuf(Buffers* buf)
    {
    	if(NULL == buf && NULL != mpQBuf)
    	{
    		return false;
    	}

    	SetQueueBuf(buf,false);

    	return true;
    };

    virtual bool IsCQ(void) = 0;
    bool IsAdminQ(void) {return 0 == mQId;};
    bool IsContinBuf(void) {return mbContinBuf;};
};

class DNVMeCQ : public DNVMeQueue
{
private:
	bool 		mIrqEnabled;
	uint16_t	mIrqNo;
	uint16_t	mHeadPtr;
	uint16_t	mTailPtr;
	bool		mPbitNewEntry;

	Buffers*	mpReapBuf;
	int			mReapCount;
	int			mLeftCount;
	int			mTimeoutms;
protected:
	bool 	GetCQMetrics(uint16_t qId);

	int  ReapInquiry(uint32_t &isrCount);
	bool ReapInquiryWaitAny(uint32_t ms, uint32_t &numCE,uint32_t &isrCount);
	bool DoReap(int numCmds,uint32_t &isrCount);
	NVMeCmdResp* GetCmdRespFromReapBuf(void);
	bool ReapNumCmds(uint32_t maxNums=0);
public:
	DNVMeCQ(int devHdlr,uint16_t qId);
	virtual ~DNVMeCQ(void);

	virtual bool IsCQ(void) { return true;};
	void		SetTimeOutMS(uint32_t ms) {mTimeoutms = ms;};
	bool		GetIrqEnabled(void){return mIrqEnabled;};
	uint16_t	GetIrqNo(void){return mIrqNo;};
	uint16_t	GetHeadPtr(void)
	{
		if(false == GetCQMetrics(GetQueueID()))
		{
			return (uint16_t)-1;
		}

		return mHeadPtr;
	};
	uint16_t	GetTailPtr(void)
	{
		if(false == GetCQMetrics(GetQueueID()))
		{
			return (uint16_t)-1;
		}

		return mTailPtr;
	};
	bool		GetPbitNewEntry(void)
	{
		if(false == GetCQMetrics(GetQueueID()))
		{
			return (uint16_t)-1;
		}

		return mPbitNewEntry;
	};

	NVMeCmdResp* GetCQEntry(void);		// Get head cq entry
	NVMeCmdResp* GetNextCQEntry(void);	// Loop to get next one cq entry

	virtual Buffers* GetQueueBuf(void);
};

class DNVMeSQ : public DNVMeQueue
{
private:
	uint16_t	mCQId;
	uint16_t	mHeadPtr;
	uint16_t	mTailPtr;
	uint16_t	mTailPtrVirt;

protected:
	bool 	GetSQMetrics(uint16_t qId);

	void	NVMePassthruTo64BCmd(NVMePassthruCmd& cmd,uint8_t* p64BCmd);
	uint32_t	GetMetaBufIDFromCmd(NVMePassthruCmd& cmd);
public:
	DNVMeSQ(int devHdlr,uint16_t qId);

	virtual bool IsCQ(void) {return false;};
	uint16_t	GetCQId(void) {return mCQId;};
	uint16_t	GetHeadPtr(void)
	{
		if(false == GetSQMetrics(GetQueueID()))
		{
			return (uint16_t)-1;
		}

		return mHeadPtr;
	};
	uint16_t	GetTailPtr(void)
	{
		if(false == GetSQMetrics(GetQueueID()))
		{
			return (uint16_t)-1;
		}

		return mTailPtr;
	};
	uint16_t	GetTailPtrVirt(void)
	{
		if(false == GetSQMetrics(GetQueueID()))
		{
			return (uint16_t)-1;
		}

		return mTailPtrVirt;
	}

	bool	Ring(void);
	uint16_t	SendCmd(NVMePassthruCmd& cmd);

	virtual Buffers* GetQueueBuf(void);
};

#endif
