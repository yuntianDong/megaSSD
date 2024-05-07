/*
 * file : DNVMeIOCtrl.cpp
 */

#include <sys/ioctl.h>

#include "driver/dnvme/DNVMeIOCtrl.h"
#include "driver/dnvme/dnvme_ioctls.h"
#include "driver/dnvme/dnvme_interface.h"

bool DNVMeIOCtrl::mInstanceFlag	= false;
DNVMeIOCtrl* DNVMeIOCtrl::mInstanceObj	= NULL;

DNVMeIOCtrl* DNVMeIOCtrl::GetInstance(int devHdlr)
{
	if(false == mInstanceFlag)
	{
		mInstanceObj = new DNVMeIOCtrl(devHdlr);
		mInstanceFlag = true;
	}

	return mInstanceObj;
}


DNVMeIOCtrl::DNVMeIOCtrl(int devHdlr)
	:mDevHdlr(devHdlr)
{
}

int DNVMeIOCtrl::IOCtlErrCheck(int &result)
{
	return ioctl(mDevHdlr,NVME_IOCTL_ERR_CHK,result);
}

int DNVMeIOCtrl::IOCtlDeviceState(emDevState state)
{
	return ioctl(mDevHdlr,NVME_IOCTL_DEVICE_STATE,state);
}

int DNVMeIOCtrl::IOCtlGetQMetrics(uint16_t qId,emQType qType,uint8_t * buffer,uint32_t length)
{
	struct nvme_get_q_metrics qMetrics = {0};

	qMetrics.q_id	= qId;
	qMetrics.type	= (qType == NVME_Q_SQ)?METRICS_SQ:METRICS_CQ;
	qMetrics.buffer	= buffer;
	qMetrics.nBytes	= length;

	return ioctl(mDevHdlr,NVME_IOCTL_GET_Q_METRICS,&qMetrics);
}

int DNVMeIOCtrl::IOCtlGetCQMetrics(uint16_t qId,uint16_t& tail_ptr,uint16_t& head_ptr,uint32_t& elements,\
		uint8_t& irq_enabled,uint16_t& irq_no,uint8_t& pbit_new_entry)
{
	tail_ptr = head_ptr = elements = irq_no = pbit_new_entry = -1;
	irq_enabled = 0;

	struct nvme_gen_cq qMetrics	= {0};
	int rtn = IOCtlGetQMetrics(qId,NVME_Q_CQ,(uint8_t*)&qMetrics,sizeof(qMetrics));
	if(0 == rtn)
	{
		tail_ptr	= qMetrics.tail_ptr;
		head_ptr	= qMetrics.head_ptr;
		elements	= qMetrics.elements;
		irq_enabled	= qMetrics.irq_enabled;
		irq_no		= qMetrics.irq_no;
		pbit_new_entry	= qMetrics.pbit_new_entry;
	}

	return rtn;
}

int DNVMeIOCtrl::IOCtlGetSQMetrics(uint16_t qId,uint16_t& cqId,uint16_t& tail_ptr,uint16_t& head_ptr,\
		uint32_t& elements,uint16_t& tail_ptr_virt)
{
	cqId = tail_ptr = head_ptr = elements = tail_ptr_virt = -1;

	struct nvme_gen_sq qMetrics	= {0};
	int rtn = IOCtlGetQMetrics(qId,NVME_Q_SQ,(uint8_t*)&qMetrics,sizeof(qMetrics));
	if(0 == rtn)
	{
		cqId 		= qMetrics.cq_id;
		tail_ptr	= qMetrics.tail_ptr;
		head_ptr	= qMetrics.head_ptr;
		elements	= qMetrics.elements;
		tail_ptr_virt	= qMetrics.tail_ptr_virt;
	}

	return rtn;
}

int DNVMeIOCtrl::IOCtlGetDriverMetrics(uint32_t &drvRev,uint32_t &apiRev)
{
	struct metrics_driver dMetrics = {0};

	int rtn = ioctl(mDevHdlr,NVME_IOCTL_GET_DRIVER_METRICS,&dMetrics);
	if(0 == rtn)
	{
		drvRev	= dMetrics.driver_version;
		apiRev	= dMetrics.api_version;
	}

	return rtn;
}

int DNVMeIOCtrl::IOCtlGetDeviceMetrics(emIntType &intType,uint16_t &numIrqs)
{
	struct public_metrics_dev dMetrics = {0};

	int rtn = ioctl(mDevHdlr,NVME_IOCTL_GET_DEVICE_METRICS,&dMetrics);
	if(0 == rtn)
	{
		intType = (emIntType)dMetrics.irq_active.irq_type;
		numIrqs = dMetrics.irq_active.num_irqs;
	}

	return rtn;
}

int DNVMeIOCtrl::IOCtlDumpMetrics(const char* filename,uint16_t flen)
{
	struct nvme_file nvmeFile = {0};

	nvmeFile.file_name	= filename;
	nvmeFile.flen		= flen;

	return ioctl(mDevHdlr,NVME_IOCTL_DUMP_METRICS,&nvmeFile);
}

int DNVMeIOCtrl::IOCtlMarkSysLog(const char* logMsg,uint16_t length)
{
	struct nvme_logstr logStr = {0};

	logStr.slen		= length;
	logStr.log_str	= logMsg;

	return ioctl(mDevHdlr,NVME_IOCTL_MARK_SYSLOG,&logStr);
}

int DNVMeIOCtrl::IOCtlSetIRQ(emIntType intType,uint16_t numIrqs)
{
	struct interrupts intParam	= {0};

	intParam.irq_type	= (enum nvme_irq_type)intType;
	intParam.num_irqs	= numIrqs;

	return ioctl(mDevHdlr,NVME_IOCTL_SET_IRQ,&intParam);
}

int DNVMeIOCtrl::IOCtlReadGeneric(emNVMeRegType regType,uint32_t offset,uint32_t length,
			uint8_t nBytesPerUnit,uint8_t * buffer)
{
	struct rw_generic rwGen;
	rwGen.type		= (regType == NVME_REG_PCI)?NVMEIO_PCI_HDR:NVMEIO_BAR01;
	rwGen.acc_type	= (enum nvme_acc_type)(nBytesPerUnit - 1);
	rwGen.buffer	= buffer;
	rwGen.nBytes	= length;
	rwGen.offset	= offset;

	return ioctl(mDevHdlr,NVME_IOCTL_READ_GENERIC,&rwGen);
}

int DNVMeIOCtrl::IOCtlWriteGeneric(emNVMeRegType regType,uint32_t offset,uint32_t length,
			uint8_t nBytesPerUnit,uint8_t * buffer)
{
	struct rw_generic rwGen;
	rwGen.type		= (regType == NVME_REG_PCI)?NVMEIO_PCI_HDR:NVMEIO_BAR01;
	rwGen.acc_type	= (enum nvme_acc_type)(nBytesPerUnit - 1);
	rwGen.buffer	= buffer;
	rwGen.nBytes	= length;
	rwGen.offset	= offset;

	return ioctl(mDevHdlr,NVME_IOCTL_WRITE_GENERIC,&rwGen);
}

int DNVMeIOCtrl::IOCtlPrepareSQCreation(uint16_t sqId,uint16_t cqId,uint32_t elements,bool contigBuf)
{
	struct nvme_prep_sq prepSQ	= {0};
	prepSQ.sq_id	= sqId;
	prepSQ.cq_id	= cqId;
	prepSQ.elements	= elements;
	prepSQ.contig	= (uint8_t)contigBuf;

	return ioctl(mDevHdlr,NVME_IOCTL_PREPARE_SQ_CREATION,&prepSQ);
}

int DNVMeIOCtrl::IOCtlPrepareCQCreation(uint16_t cqId,uint32_t elements,bool contigBuf)
{
	struct nvme_prep_cq prepCQ	= {0};

	prepCQ.cq_id	= cqId;
	prepCQ.elements	= elements;
	prepCQ.contig	= contigBuf;

	return ioctl(mDevHdlr,NVME_IOCTL_PREPARE_CQ_CREATION,&prepCQ);
}

int DNVMeIOCtrl::IOCtlCreateAdminQ(emQType qType,uint32_t elements)
{
	struct nvme_create_admn_q adminQParam;

	adminQParam.type		= (qType == NVME_Q_SQ)?ADMIN_SQ:ADMIN_CQ;
	adminQParam.elements	= elements;

	return ioctl(mDevHdlr,NVME_IOCTL_CREATE_ADMN_Q,&adminQParam);
}

int DNVMeIOCtrl::IOCtlSend64BCmd(uint16_t &cmdId,uint16_t sqId,uint8_t *p64BCmd,emDataDir dir,
			uint32_t dataSize,uint8_t *pDataBuf,emDataPointerMask dataMask,uint32_t metaBufId)
{
	struct nvme_64b_send cmdDesc;

	cmdDesc.q_id			= sqId;
	cmdDesc.cmd_buf_ptr		= p64BCmd;
	cmdDesc.data_dir		= (uint8_t)dir;
	cmdDesc.bit_mask		= (enum send_64b_bitmask)dataMask;
	cmdDesc.data_buf_size	= dataSize;
	cmdDesc.data_buf_ptr	= pDataBuf;
	cmdDesc.meta_buf_id		= metaBufId;

	int rtn = ioctl(mDevHdlr,NVME_IOCTL_SEND_64B_CMD,&cmdDesc);
	cmdId	= cmdDesc.unique_id;

	return rtn;
}

int DNVMeIOCtrl::IOCtlToxic64BCmd(uint16_t sqId,uint16_t seIdx,uint8_t dwIdx,uint32_t value,uint32_t mask)
{
	struct backdoor_inject	toxicParam	= {0};
	toxicParam.q_id		= sqId;
	toxicParam.cmd_ptr	= seIdx;
	toxicParam.dword	= dwIdx;
	toxicParam.value	= value;
	toxicParam.value_mask	= mask;

	return ioctl(mDevHdlr,NVME_IOCTL_TOXIC_64B_DWORD,&toxicParam);
}

int DNVMeIOCtrl::IOCtlRingSQDoorbell(uint16_t sqId)
{
	return ioctl(mDevHdlr,NVME_IOCTL_RING_SQ_DOORBELL,sqId);
}

int DNVMeIOCtrl::IOCtlReapInquiry(uint16_t cqId,uint32_t &numCmds,uint32_t &isrCount)
{
	struct nvme_reap_inquiry inquiry	= {0};

	inquiry.q_id	= cqId;

	int rc = ioctl(mDevHdlr,NVME_IOCTL_REAP_INQUIRY,&inquiry);
	if(0 == rc)
	{
		numCmds		= inquiry.num_remaining;
		isrCount	= inquiry.isr_count;
	}

	return rc;
}

int DNVMeIOCtrl::IOCtlReap(uint16_t cqId,uint32_t count,uint8_t *pData,uint32_t length,uint32_t &numReaped,
			uint32_t &numLeft,uint32_t &isrCount)
{
	struct nvme_reap   reapRslt	= {0};

	reapRslt.q_id		= cqId;
	reapRslt.elements	= count;
	reapRslt.buffer		= pData;
	reapRslt.size		= length;

	int rc = ioctl(mDevHdlr,NVME_IOCTL_REAP,&reapRslt);
	if(0 == rc)
	{
		numReaped	= reapRslt.num_reaped;
		numLeft		= reapRslt.num_remaining;
		isrCount	= reapRslt.isr_count;
	}

	return rc;
}

int DNVMeIOCtrl::IOCtlMetaBufCreate(uint32_t size)
{
	return ioctl(mDevHdlr,NVME_IOCTL_METABUF_CREATE,size);
}

int DNVMeIOCtrl::IOCtlMetaBufAlloc(uint32_t mId)
{
	return ioctl(mDevHdlr,NVME_IOCTL_METABUF_ALLOC,mId);
}

int DNVMeIOCtrl::IOCtlMetaBufDelete(uint32_t mId)
{
	return ioctl(mDevHdlr,NVME_IOCTL_METABUF_DELETE,mId);
}
