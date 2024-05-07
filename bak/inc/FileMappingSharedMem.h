#ifndef _FILE_MAPPING_SHARED_MEM_H
#define _FILE_MAPPING_SHARED_MEM_H

#include "buffer/Buffers.h"

#define	FMSM_FILEPATH_LEN			240
#define	DEF_MAPPED_FILENAME			"Dump.bin"
#define DEF_MAPPED_FILESIZE			4096

typedef enum _enAccessMode
{
	M_READWRITE,
	M_READONLY
}enAccessMode;

class FileMapSharedMem
{
protected:
	int						mFileHandlr;
	void*					mpMappedMem;
	uint32_t				mMappedSize;
	enAccessMode			mAccessMode;

	char					mFilePath[FMSM_FILEPATH_LEN];
	uint64_t				mFileSize;

	bool		IsWritable(void) {return M_READWRITE == mAccessMode;};
	uint64_t	GetFileSize(void);
	bool		CheckAndPrepareFile(void);
	void		OpenMapRegion(uint32_t offset=0,uint32_t length=0);

	uint32_t	GetSharedMemLength(void) {return mMappedSize;};
	void*		GetSharedMemAddress(void) {return mpMappedMem;};

	uint32_t Read(uint32_t offset,void* data,uint32_t length);
	uint32_t Write(uint32_t offset,void* data,uint32_t length);
public:
	FileMapSharedMem(enAccessMode mode);
	FileMapSharedMem(const char* filePath,enAccessMode mode,
			uint64_t fileSize=0,uint32_t mrOffset=0,uint32_t mrLength=0);
	virtual ~FileMapSharedMem(void);

	bool Flush(uint32_t mrOffset=0,uint32_t mrLength=0, bool async = false);

	uint32_t Read(Buffers* buf,uint32_t offset,uint32_t length);
	uint32_t Write(Buffers* buf,uint32_t offset,uint32_t length);
};

#endif
