/*
 * file: FileMappingSharedMem.cpp
 */

#include <string.h>
#include <assert.h>
#include "ipc/FileMappingSharedMem.h"
#include "utility/BoostFile.h"
#include "Logging.h"

using namespace boost::interprocess;

FileMapSharedMemServer::FileMapSharedMemServer(void)
{
	strncpy(mFilePath,DEF_MAPPED_FILENAME,FMSM_FILEPATH_LEN);
	mFileSize	= DEF_MAPPED_FILESIZE;

	assert(true == CheckAndPrepareFile());
	OpenFileMap();
	OpenMapRegion();
}

FileMapSharedMemServer::FileMapSharedMemServer(const char* filePath,
		uint64_t fileSize,uint32_t mrOffset,uint32_t mrLength)
{
	strncpy(mFilePath,filePath,FMSM_FILEPATH_LEN);
	mFileSize	== (0 == fileSize)?GetFileSize():fileSize;

	assert(true == CheckAndPrepareFile());
	OpenFileMap();
	OpenMapRegion(mrOffset,mrLength);
}

bool FileMapSharedMemServer::CheckAndPrepareFile(void)
{
	BoostFile	bf(mFilePath);
	if(false == bf.IsExists())
	{
		return bf.CreateAndFillFile(mFileSize,0);
	}

	return bf.IsRegularFile();
}

uint64_t FileMapSharedMemServer::GetFileSize(void)
{
	return BoostFile(mFilePath).GetFileSize();
}

void FileMapSharedMemServer::OpenFileMap(void)
{
	try{
        file_mapping::remove(mFilePath);

        mpFileMap = FileMappingPtrType(new file_mapping(
        		mFilePath,
				read_write));
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Create FileMapping Failed.\n %s",ex.what());
	}
}

void FileMapSharedMemServer::OpenMapRegion(uint32_t offset,uint32_t length)
{
	assert(offset < mFileSize);

	length	= (0 == length)?mFileSize:length;
	if( (offset + length) > mFileSize)
	{
		length	= mFileSize - offset;
	}

	try{
		mpMapRegion = MapRegionPtrType(new mapped_region(
				*mpFileMap,
				read_write,
				offset,
				length));
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Create MapRegion Failed.\n %s",ex.what());
	}
}

uint32_t FileMapSharedMemServer::Read(uint32_t offset,void* data,uint32_t length)
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

	memcpy(data,rgAddress,length);

	return length;
}

uint32_t FileMapSharedMemServer::Write(uint32_t offset,void* data,uint32_t length)
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

	memcpy(rgAddress,data,length);

	return length;
}

uint32_t FileMapSharedMemServer::Read(Buffers* buf,uint32_t offset,uint32_t length)
{
	if(NULL == buf)
	{
		return 0;
	}

	length	= (buf->GetBufSize() < length)?buf->GetBufSize():length;

	return Read(offset,buf->GetBufferPoint(),length);
}

uint32_t FileMapSharedMemServer::Write(Buffers* buf,uint32_t offset,uint32_t length)
{
	if(NULL == buf)
	{
		return 0;
	}

	length	= (buf->GetBufSize() < length)?buf->GetBufSize():length;

	return Write(offset,buf->GetBufferPoint(),length);
}

FileMapSharedMemClient::FileMapSharedMemClient(void)
{
	strncpy(mFilePath,DEF_MAPPED_FILENAME,FMSM_FILEPATH_LEN);
	mFileSize	= GetFileSize();
	assert(0 == mFileSize);

	OpenFileMap();
	OpenMapRegion();
}

FileMapSharedMemClient::FileMapSharedMemClient(const char* filePath,
		uint32_t mrOffset,uint32_t mrLength)
{
	strncpy(mFilePath,DEF_MAPPED_FILENAME,FMSM_FILEPATH_LEN);
	mFileSize	= GetFileSize();
	assert(0 == mFileSize);

	OpenFileMap();
	OpenMapRegion(mrOffset,mrLength);
}

uint64_t FileMapSharedMemClient::GetFileSize(void)
{
	return BoostFile(mFilePath).GetFileSize();
}

void FileMapSharedMemClient::OpenFileMap(void)
{
	try{
        mpFileMap = FileMappingPtrType(new file_mapping(
        		mFilePath,
				read_only));
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Create FileMapping Failed.\n %s",ex.what());
	}
}

void FileMapSharedMemClient::OpenMapRegion(uint32_t offset,uint32_t length)
{
	assert(offset < mFileSize);

	length	= (0 == length)?mFileSize:length;
	if( (offset + length) > mFileSize)
	{
		length	= mFileSize - offset;
	}

	try{
		mpMapRegion = MapRegionPtrType(new mapped_region(
				*mpFileMap,
				read_only,
				offset,
				length));
	}
	catch(interprocess_exception &ex)
	{
		LOGERROR("Create MapRegion Failed.\n %s",ex.what());
	}
}

uint32_t FileMapSharedMemClient::Read(uint32_t offset,void* data,uint32_t length)
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

	memcpy(data,rgAddress,length);

	return length;
}

uint32_t FileMapSharedMemClient::Read(Buffers* buf,uint32_t offset,uint32_t length)
{
	if(NULL == buf)
	{
		return 0;
	}

	length	= (buf->GetBufSize() < length)?buf->GetBufSize():length;

	return Read(offset,buf->GetBufferPoint(),length);
}
