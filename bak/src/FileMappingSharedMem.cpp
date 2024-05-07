/*
 * file: FileMappingSharedMem.cpp
 */

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "ipc/FileMappingSharedMem.h"
#include "Logging.h"

#define INVALID_HANDLER		-1

FileMapSharedMem::FileMapSharedMem(enAccessMode mode)
{
	strncpy(mFilePath,DEF_MAPPED_FILENAME,FMSM_FILEPATH_LEN);
	mFileHandlr	= INVALID_HANDLER;
	mFileSize	= DEF_MAPPED_FILESIZE;
	mAccessMode	= mode;
	mpMappedMem	= NULL;
	mMappedSize	= 0;

	OpenMapRegion();
}

FileMapSharedMem::FileMapSharedMem(const char* filePath,enAccessMode mode,
		uint64_t fileSize,uint32_t mrOffset,uint32_t mrLength)
{
	strncpy(mFilePath,filePath,FMSM_FILEPATH_LEN);
	mFileHandlr	= INVALID_HANDLER;
	mFileSize	= fileSize;
	mAccessMode	= mode;
	mpMappedMem	= NULL;
	mMappedSize	= 0;

	OpenMapRegion(mrOffset,mrLength);
}

FileMapSharedMem::~FileMapSharedMem(void)
{
	if(NULL != mpMappedMem)
	{
		munmap(mpMappedMem,mMappedSize);
		mpMappedMem = NULL;
		mMappedSize	= 0;
	}

	if(INVALID_HANDLER != mFileHandlr)
	{
		close(mFileHandlr);
		mFileHandlr	= INVALID_HANDLER;
	}
}

void FileMapSharedMem::OpenMapRegion(uint32_t offset,uint32_t length)
{
	int fd = open("/tmp/test.bin", O_CREAT | O_RDWR | O_TRUNC, 00777);
	assert(0 <= fd);
	ftruncate(fd, 0x10000);

	void*p	= mmap(NULL, 0x10000, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);
	LOGINFO("p = %llx,errno = %d",(uint64_t)p,errno);

	mpMappedMem	= p;
	mMappedSize	= 0x10000;
}

bool FileMapSharedMem::Flush(uint32_t offset,uint32_t length,bool async)
{
	assert(offset < mFileSize);

	length	= (0 == length)?mFileSize:length;
	if( (offset + length) > mFileSize)
	{
		length	= mFileSize - offset;
	}

	LOGINFO("Flush: offset = %d,length = %d",offset,length);
	return msync((uint8_t*)mpMappedMem+offset, length, async? MS_ASYNC : MS_SYNC) == 0;
}

uint32_t FileMapSharedMem::Read(uint32_t offset,void* data,uint32_t length)
{
	uint32_t	rgMaxSize	= GetSharedMemLength();
	void*		rgAddress	= GetSharedMemAddress();

	if( (offset >= rgMaxSize) || 0 == length)
	{
		return 0;
	}

	if( (offset + length) >= rgMaxSize)
	{
		length	= rgMaxSize - offset;
	}

	LOGINFO("Read: offset = %d,length = %d",offset,length);
	memcpy(data,(uint8_t*)rgAddress+offset,length);

	return length;
}

uint32_t FileMapSharedMem::Write(uint32_t offset,void* data,uint32_t length)
{
	assert(true == IsWritable());
	assert(NULL != data);

	uint32_t	rgMaxSize	= GetSharedMemLength();
	void*		rgAddress	= GetSharedMemAddress();

	if( (offset >= rgMaxSize) || 0 == length)
	{
		return 0;
	}

	if( (offset + length) > rgMaxSize)
	{
		length	= rgMaxSize - offset;
	}

	LOGINFO("Write: offset = %d,length = %d",offset,length);
	LOGINFO("rgAddress = %llx",(uint64_t)rgAddress);
	memcpy((uint8_t*)rgAddress+offset,data,length);
	LOGINFO("Write Success");

	return length;
}

uint32_t FileMapSharedMem::Read(Buffers* buf,uint32_t offset,uint32_t length)
{
	assert(NULL != buf);
	length	= (buf->GetBufSize() < length)?buf->GetBufSize():length;

	return Read(offset,buf->GetBufferPoint(),length);
}

uint32_t FileMapSharedMem::Write(Buffers* buf,uint32_t offset,uint32_t length)
{
	assert(NULL != buf);
	length	= (buf->GetBufSize() < length)?buf->GetBufSize():length;

	return Write(offset,buf->GetBufferPoint(),length);
}
