#ifndef _DNVME_IOCTRL_H
#define _DNVME_IOCTRL_H

#include <stdint.h>
#include <string.h>
#include <vector>

using namespace std;

#define	IOCTL_CALL_SUCCESS			0

typedef enum
{
	NVME_REG_PCI,
	NVME_REG_BAR0,
	NVME_REG_FENCE,
}emNVMeRegType;

typedef enum
{
	NVME_Q_SQ,
	NVME_Q_CQ,
	NVME_Q_FENCE,
}emQType;

typedef enum
{
	NVME_DEV_ENABLE,
	NVME_DEV_DISABLE,
	NVME_DEV_DISABLE_COMPLETELY,
	NVME_DEV_STATE_FENCE,
}emDevState;

typedef enum
{
	NVME_INT_T_MSI,
	NVME_INT_T_MSI_MULTI,
	NVME_INT_T_MSIX,
	NVME_INT_T_NONE,
	NMVE_INT_T_FENCE,
}emIntType;

typedef enum
{
	NVME_DATA_NONE,
	NVME_DATA_OUT,
	NVME_DATA_IN,
	NVME_DATA_INOUT,
	NVME_DATA_FENCE,
}emDataDir;

typedef enum
{
	NVME_MASK_PRP1_PAGE		= 1,
	NVME_MASK_PRP1_LIST		= 2,
	NVME_MASK_PRP2_PAGE		= 4,
	NVME_MASK_PRP2_LIST		= 8,
	NVME_MASK_MPTR			= 16,
}emDataPointerMask;

class DNVMeIOCtrl
{
private:
	int mDevHdlr;

	static bool mInstanceFlag;
	static DNVMeIOCtrl* mInstanceObj;

	DNVMeIOCtrl(int devHdlr);
protected:

public:
	static DNVMeIOCtrl* GetInstance(int devHdlr);

	// IOCTL : NVME_IOCTL_ERR_CHK
	int IOCtlErrCheck(int &result);
	// IOCTL : NVME_IOCTL_DEVICE_STATE
	int IOCtlDeviceState(emDevState state);

	// IOCTL : NVME_IOCTL_GET_Q_METRICS
	int IOCtlGetQMetrics(uint16_t qId,emQType qType,uint8_t * buffer,uint32_t length);
	int IOCtlGetCQMetrics(uint16_t qId,uint16_t& tail_ptr,uint16_t& head_ptr,uint32_t& elements,\
			uint8_t& irq_enabled,uint16_t& irq_no,uint8_t& pbit_new_entry);
	int IOCtlGetSQMetrics(uint16_t qId,uint16_t& cqId,uint16_t& tail_ptr,uint16_t& head_ptr,\
			uint32_t& elements,uint16_t& tail_ptr_virt);
	// IOCTL : NVME_IOCTL_GET_DRIVER_METRICS
	int IOCtlGetDriverMetrics(uint32_t &drvRev,uint32_t &apiRev);
	// IOCTL : NVME_IOCTL_GET_DEVICE_METRICS
	int IOCtlGetDeviceMetrics(emIntType &intType,uint16_t &numIrqs);
	// IOCTL : NVME_IOCTL_DUMP_METRICS
	int IOCtlDumpMetrics(const char* filename,uint16_t flen);
	// IOCTL : NVME_IOCTL_SET_IRQ
	int IOCtlSetIRQ(emIntType intType,uint16_t numIrqs);
	// IOCTL : NVME_IOCTL_MARK_SYSLOG
	int IOCtlMarkSysLog(const char* logMsg,uint16_t length);

	// IOCTL : NVME_IOCTL_READ_GENERIC
	int IOCtlReadGeneric(emNVMeRegType regType,uint32_t offset,uint32_t length,
			uint8_t nBytesPerUnit,uint8_t * buffer);
	// IOCTL : NVME_IOCTL_WRITE_GENERIC
	int IOCtlWriteGeneric(emNVMeRegType regType,uint32_t offset,uint32_t length,
			uint8_t nBytesPerUnit,uint8_t * buffer);

	// IOCTL : NVME_IOCTL_PREPARE_SQ_CREATION
	int IOCtlPrepareSQCreation(uint16_t sqId,uint16_t cqId,uint32_t elements,bool contigBuf);
	// IOCTL : NVME_IOCTL_PREPARE_CQ_CREATION
	int IOCtlPrepareCQCreation(uint16_t cqId,uint32_t elements,bool contigBuf);
	// IOCTL : NVME_IOCTL_CREATE_ADMN_Q
	int IOCtlCreateAdminQ(emQType qType,uint32_t elements);

	// IOCTL : NVME_IOCTL_SEND_64B_CMD
	int IOCtlSend64BCmd(uint16_t& cmdId,uint16_t sqId,uint8_t *p64BCmd,emDataDir dir,
			uint32_t dataSize,uint8_t *pDataBuf,emDataPointerMask dataMask,uint32_t metaBufId);
	// IOCTL : NVME_IOCTL_TOXIC_64B_DWORD
	int IOCtlToxic64BCmd(uint16_t sqId,uint16_t seIdx,uint8_t dwIdx,uint32_t value,uint32_t mask);
	// IOCTL : NVME_IOCTL_RING_SQ_DOORBELL
	int IOCtlRingSQDoorbell(uint16_t sqId);
	// IOCTL : NVME_IOCTL_REAP_INQUIRY
	int IOCtlReapInquiry(uint16_t cqId,uint32_t &numCmds,uint32_t &isrCount);
	// IOCTL : NVME_IOCTL_REAP
	int IOCtlReap(uint16_t cqId,uint32_t count,uint8_t *pData,uint32_t length,uint32_t &numReaped,
			uint32_t &numLeft,uint32_t &isrCount);

	// IOCTL : NVME_IOCTL_METABUF_CREATE
	int IOCtlMetaBufCreate(uint32_t size);
	// IOCTL : NVME_IOCTL_METABUF_ALLOC
	int IOCtlMetaBufAlloc(uint32_t mId);
	// IOCTL : NVME_IOCTL_METABUF_DELETE
	int IOCtlMetaBufDelete(uint32_t mId);
};

#endif
