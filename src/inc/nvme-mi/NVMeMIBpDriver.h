/*
 * NVMeMIBpDriver.h
 *
 *  Created on: 2020Äê12ÔÂ23ÈÕ
 *      Author: Lenovo
 */

#ifndef _NVME_MI_NVMEMIBPDRIVER_H_
#define _NVME_MI_NVMEMIBPDRIVER_H_

#include <stdint.h>

#ifndef   UNUSED_PARAM
#define   UNUSED_PARAM(v)   (void)(v)
#endif

class NVMeMIBpDriver
{
public:
	NVMeMIBpDriver(){};
	virtual ~NVMeMIBpDriver(){};

	virtual uint16_t	GetUnitLength(void)	{return (uint16_t)-1;};

	virtual bool ReadyForWrite(uint16_t xferBytes) {UNUSED_PARAM(xferBytes);return false;};
	virtual bool Write(uint8_t* data,uint16_t len,uint16_t offset=0) {UNUSED_PARAM(data);UNUSED_PARAM(len);UNUSED_PARAM(offset);return false;};
	virtual bool ReadyForRead(uint16_t& recvBytes) {recvBytes = 0; return false;};
	virtual bool Read(uint8_t* data,uint16_t len,uint16_t offset=0) {UNUSED_PARAM(data);UNUSED_PARAM(len);UNUSED_PARAM(offset);return false;};
};

#endif /* _NVME_MI_NVMEMIBPDRIVER_H_ */
