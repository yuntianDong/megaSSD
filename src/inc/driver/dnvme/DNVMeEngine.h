#ifndef _DNVME_ENGINE_H
#define _DNVME_ENGINE_H

#include "driver/nvme/NVMeEngine.h"
#include "driver/dnvme/DNVMeQMngr.h"

#define DEF_ADMIN_QDEPTH_CQ		16
#define DEF_ADMIN_QDEPTH_SQ		16
#define DEF_IOQ_NUMBER			4
#define DEF_IOQ_QDEPTH_CQ		16
#define DEF_IOQ_QDEPTH_SQ		16

#define NVME_Q_ENTRY_SIZE_CQ	16
#define NVME_Q_ENTRY_SIZE_SQ	64

class DNVMeEngine:public NVMeEngine
{
private:
	uint16_t	mAdminCQDepth;
	uint16_t	mAdminSQDepth;
	uint16_t 	mIOCQDepth;
	uint16_t 	mIOSQDepth;
	uint16_t 	mIOQNum;
	uint16_t	mCurNSID;

	bool		mbIEnabled;
	uint16_t	muIntVector;

	IOQPair 	GetIOQPair(void);
	DNVMeCQ*	GetAdminCQ(void);
	DNVMeSQ*	GetAdminSQ(void);

	NVMeCmdResp* CreateIOCQ(uint16_t qid,uint16_t qSize,uint16_t iv,\
			bool bIEN,bool bPC,Buffers* buf,uint32_t timeout);
	NVMeCmdResp* CreateIOSQ(uint16_t qid,uint16_t qSize,uint16_t cqid,\
			uint8_t qPrio,bool bPC,Buffers* buf,uint32_t timeout);
	bool CreateIOQueues(void);
public:
	DNVMeEngine(int fd);

	bool CfgAdminQDepth(uint16_t cqDepth,uint16_t sqDepth);
	bool CfgIOQueue(uint16_t qNum,uint16_t cqDepth,uint16_t sqDepth);
	bool SetCQInt(bool ien,uint16_t iv);

	NVMeCmdResp* NVMePassthru(NVMePassthruCmd &req,uint32_t timeout);
	bool NVMeReset(void);
	bool NVMeRescan(void);
	bool NVMeSubSysReset(void);
	int GetCurNSID(void) {return -1;};
};

#endif
