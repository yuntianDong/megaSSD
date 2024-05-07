#ifndef _FILE_MAPPING_SHARED_MEM_H
#define _FILE_MAPPING_SHARED_MEM_H

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "buffer/Buffers.h"

typedef boost::shared_ptr<boost::interprocess::mapped_region> MapRegionPtrType;
typedef boost::shared_ptr<boost::interprocess::file_mapping> FileMappingPtrType;

#define	FMSM_FILEPATH_LEN			240
#define	DEF_MAPPED_FILENAME			"Dump.bin"
#define DEF_MAPPED_FILESIZE			4096

class FileMapSharedMemServer
{
protected:
	FileMappingPtrType		mpFileMap;
	MapRegionPtrType		mpMapRegion;

	char					mFilePath[FMSM_FILEPATH_LEN];
	uint64_t				mFileSize;

	uint64_t	GetFileSize(void);
	bool		CheckAndPrepareFile(void);
	void		OpenFileMap(void);
	void		OpenMapRegion(uint32_t offset=0,uint32_t length=0);

	uint32_t	GetSharedMemLength(void) {return mpMapRegion->get_size();};
	void*		GetSharedMemAddress(void) {return mpMapRegion->get_address();};

	uint32_t Read(uint32_t offset,void* data,uint32_t length);
	uint32_t Write(uint32_t offset,void* data,uint32_t length);
public:
	FileMapSharedMemServer(void);
	FileMapSharedMemServer(const char* filePath,uint64_t fileSize=0,uint32_t mrOffset=0,uint32_t mrLength=0);

	void Flush(void) {mpMapRegion->flush();};

	uint32_t Read(Buffers* buf,uint32_t offset,uint32_t length);
	uint32_t Write(Buffers* buf,uint32_t offset,uint32_t length);
};

class FileMapSharedMemClient
{
protected:
	FileMappingPtrType		mpFileMap;
	MapRegionPtrType		mpMapRegion;

	char					mFilePath[FMSM_FILEPATH_LEN];
	uint64_t				mFileSize;

	uint64_t	GetFileSize(void);
	void		OpenFileMap(void);
	void		OpenMapRegion(uint32_t offset=0,uint32_t length=0);

	uint32_t	GetSharedMemLength(void) {return mpMapRegion->get_size();};
	void*		GetSharedMemAddress(void) {return mpMapRegion->get_address();};

	uint32_t Read(uint32_t offset,void* data,uint32_t length);
public:
	FileMapSharedMemClient(void);
	FileMapSharedMemClient(const char* filePath,uint32_t mrOffset=0,uint32_t mrLength=0);

	uint32_t Read(Buffers* buf,uint32_t offset,uint32_t length);
};

#endif
