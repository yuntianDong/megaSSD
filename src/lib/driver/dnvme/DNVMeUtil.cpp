/*
 * file DNVMeUtil.cpp
 */
#include <unistd.h>
#include <sys/mman.h>
#include "driver/dnvme/DNVMeUtil.h"

#define METADATA_UNIQUE_ID_BITS			18

uint8_t *
DNVMeUtil::mmap(int fd, size_t bufLength, uint16_t bufID, MmapRegion region)
{
    int prot = PROT_READ;

    if (region >= MMAPREGION_FENCE)
        return NULL;

    if (region == MMR_META)
        prot |= PROT_WRITE;

    off_t encodeOffset = bufID;
    encodeOffset |= ((off_t)region << METADATA_UNIQUE_ID_BITS);
    encodeOffset *= sysconf(_SC_PAGESIZE);
    return (uint8_t *)::mmap(0, bufLength, prot, MAP_SHARED, fd,
        encodeOffset);
}

void
DNVMeUtil::munmap(uint8_t *memPtr, size_t bufLength)
{
    ::munmap(memPtr, bufLength);
}
