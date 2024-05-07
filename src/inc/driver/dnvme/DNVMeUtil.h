#ifndef _DNVME_UTIL_H
#define _DNVME_UTIL_H

#include <stdint.h>

class DNVMeUtil
{
public:
	typedef enum {
        MMR_CQ,             // Desire to map SQ memory
        MMR_SQ,             // Desire to map CQ memory
        MMR_META,           // Desire to map meta data memory
        MMAPREGION_FENCE    // always must be last element
	} MmapRegion;

	/**
	* For all mmap operations this method understand the unique encoding
	* which is needed to specify what memory to map into user space.
	* @note This method may throw
	* @param bufLength Pass the # of bytes consisting of the buffers total size
	* @param bufID Pass the buffer's ID corresponding to param region
	* @param region Pass what region of memory is being requested for mapping
	* @return A user space pointer to the mapped kernel memory, NULL indicates
	*      a failed mapping attempt.
	*/
	static uint8_t *mmap(int fd, size_t bufLength, uint16_t bufID, MmapRegion region);
	static void munmap(uint8_t *memPtr, size_t bufLength);
};

#endif
