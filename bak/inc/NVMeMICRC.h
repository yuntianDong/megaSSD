/*
 * NVMeMICRC.h
 *
 */

#ifndef _NVME_MI_NVMEMICRC_H_
#define _NVME_MI_NVMEMICRC_H_

#include <stdint.h>

uint8_t smbus_crc8(uint8_t crcInitial, uint8_t* data, uint32_t len);

uint8_t* Crc32_Init(uint32_t poly);
uint32_t CalculateCRC32(uint8_t* crc32Tbl,uint8_t* data, uint32_t len);

#endif /* _NVME_MI_NVMEMICRC_H_ */
