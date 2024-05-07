#ifndef _ASHA_FILE_H
#define _ASHA_FILE_H

#include <stdint.h>
#include <boost/filesystem.hpp>
#include "Logging.h"

class BoostFile
{
private:
	boost::filesystem::path		mFilePath;
public:
	BoostFile(const boost::filesystem::path&	path)
		:mFilePath(path)
	{
		mFilePath = boost::filesystem::system_complete(mFilePath);
	};

	bool	IsExists(void)
	{
		return boost::filesystem::exists(mFilePath);
	};
	bool	IsRegularFile(void)
	{
		if(false == IsExists())
		{
			return false;
		}
		return boost::filesystem::is_regular_file(mFilePath);
	};
	bool	Delete(void)
	{
		if(false == IsExists())
		{
			return false;
		}

		return boost::filesystem::remove(mFilePath);
	};
	bool	RenameAs(const boost::filesystem::path	toPath)
	{
		if(false == IsExists())
		{
			return false;
		}

		try{
			boost::filesystem::rename(mFilePath,toPath);
		}
		catch(...)
		{
			return false;
		}

		mFilePath = toPath;
		return true;
	};
	bool	CopyTo(const boost::filesystem::path	toPath)
	{
		if(false == IsExists())
		{
			return false;
		}

		try{
			boost::filesystem::copy_file(mFilePath,toPath);
		}
		catch(...)
		{
			return false;
		}

		mFilePath	= toPath;
		return true;
	};

	uint64_t	GetFileSize(void)
	{
		if(false == IsExists())
		{
			return 0;
		}
		return boost::filesystem::file_size(mFilePath);
	};
	bool	CreateAndFillFile(uint64_t filesize,char fill_char)
	{
		try
		{
			boost::filesystem::filebuf	fs;

			fs.open(mFilePath,std::ios_base::out |
					std::ios_base::trunc |
					std::ios_base::binary);

			fs.pubseekoff(filesize-1, std::ios_base::beg);
			fs.sputc(fill_char);
			fs.close();
		}
		catch(...)
		{
			LOGERROR("Exception Raised when CreateAndFillFile()");
			return false;
		}

		return true;
	};
};

#endif
