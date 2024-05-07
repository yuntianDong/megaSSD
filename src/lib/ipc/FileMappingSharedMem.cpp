/*
 * file: FileMappingSharedMem.cpp
 */

#include <string.h>
#include <assert.h>
#include "ipc/FileMappingSharedMem.h"
#include "utility/BoostFile.h"
#include "Logging.h"

using namespace boost::interprocess;

FileMapSharedMem::FileMapSharedMem(enAccessMode mode)
{
	strncpy(mFilePath,DEF_MAPPED_FILENAME,FMSM_FILEPATH_LEN);
	mFileSize	= DEF_MAPPED_FILESIZE;
	mAccessMode	= mode;

	if(true == IsWritable())
	{
		assert(true == CheckAndPrepareFile());
	}
	OpenFileMap();
	OpenMapRegion();
}

FileMapSharedMem::FileMapSharedMem(const char* filePath,enAccessMode mode,
		uint64_t fileSize,uint32_t mrOffset,uint32_t mrLength)
{
	strncpy(mFilePath,filePath,FMSM_FILEPATH_LEN);
	mFileSize	= (0 == fileSize)?GetFileSize():fileSize;
	mAccessMode	= mode;

	if(true == IsWritable())
	{
		assert(true == CheckAndPrepareFile());
	}
	OpenFileMap();
	OpenMapRegion(mrOffset,mrLength);
}

bool FileMapSharedMem::CheckAndPrepareFile(void)
{
	BoostFile	bf(mFilePath);
	if(false == bf.IsExists())
	{
		return bf.CreateAndFillFile(mFileSize,0);
	}

	return bf.IsRegularFile();
}

uint64_t FileMapSharedMem::GetFileSize(void)
{
	return BoostFile(mFilePath).GetFileSize();
}

void FileMapSharedMem::OpenFileMap(void)
{
	try{
        mpFileMap = FileMappingPtrType(new file_mapping(
        		mFilePath,
				(true == IsWritable())?read_write:read_only));
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Create FileMapping Failed.\n %s",ex.what());
	}
}

void FileMapSharedMem::OpenMapRegion(uint32_t offset,uint32_t length)
{
	assert(offset < mFileSize);

	length	= (0 == length)?mFileSize:length;
	if( (offset + length) > mFileSize)
	{
		length	= mFileSize - offset;
	}

	try{
		mpFileMapRegion = MapRegionPtrType(new mapped_region(
				*mpFileMap,
				(true == IsWritable())?read_write:read_only,
				offset,
				length));
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Create MapRegion Failed.\n %s",ex.what());
	}
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

	memcpy(data,(uint8_t*)rgAddress+offset,length);
	return length;
}

uint32_t FileMapSharedMem::Write(uint32_t offset,void* data,uint32_t length)
{
	assert(true == IsWritable());

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

	memcpy((uint8_t*)rgAddress+offset,data,length);
	return length;
}

uint32_t FileMapSharedMem::Read(Buffers* buf,uint32_t offset,uint32_t length)
{
	if(NULL == buf)
	{
		return 0;
	}

	length	= (buf->GetBufSize() < length)?buf->GetBufSize():length;

	return Read(offset,buf->GetBufferPoint(),length);
}

uint32_t FileMapSharedMem::Write(Buffers* buf,uint32_t offset,uint32_t length)
{
	if(NULL == buf)
	{
		return 0;
	}

	length	= (buf->GetBufSize() < length)?buf->GetBufSize():length;

	return Write(offset,buf->GetBufferPoint(),length);
}
