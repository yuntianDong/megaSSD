#ifndef _RUNNING_PARAM_H
#define _RUNNING_PARAM_H

#include <stddef.h>
#include <stdint.h>
#include <map>
#include "Commons.h"

#define DEF_PARAM_CMD_TIME_OUT              8000
#define DEF_PARAM_NS_CAPACITY               0
#define DEF_PARAM_NS_DATA_SIZE              512
#define DEF_PARAM_NS_META_SIZE              0
#define DEF_PARAM_NS_PI_GUARD_SIZE          0
#define DEF_PARAM_NS_PI_STORAGE_TAG_SIZE    0

typedef struct _NsMapInfo
{
	uint64_t offset;
	uint64_t nsCap;

}NsMapInfo;

typedef struct _LbaMapInfo
{
	uint32_t sectorSize;
	uint32_t metaSize;

}LbaMapInfo;


typedef struct _GlobalParameter
{
    std::map<uint32_t, NsMapInfo>       NsMap;
    std::map<uint32_t, LbaMapInfo>      lbaMap;
    uint64_t                            totalNvmSize;   // total NVMe size (byte based)

}ParentParameter;

typedef struct _ThreadParameter
{
	uint32_t	cmdTO;          // cmd time out
	uint32_t	nsID;			// namespace ID
	uint64_t	nsCapacity;		// namespace cap (lba based)
	uint32_t	nsDataSize;		// namespace data size (byte based)
	uint32_t	nsMetaSize;		// namespace meta size (byte based)
	uint32_t	nsPIGuardSize;	// namespace PI Format Guard Bytes, 16,32,64 supported
	uint32_t	nsPISTagSize;	// namespace PI Storage Tag Size

}ChildParameter;

#endif