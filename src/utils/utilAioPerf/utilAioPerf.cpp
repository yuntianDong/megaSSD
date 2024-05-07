/*
 * file : utilAioPerf.c
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <boost/program_options.hpp>
#include <string>
#include "perf/aio/AioPerfEngine.h"
#include "Logging.h"

using namespace std;

#define PROGRAM_NAME		"utilDevScan"
#define PROGRAM_VER			"V1.0"
#define PROGRAM_AUTHOR		"XIAOWJ"

#define INVALID_DEVHDLR			-1


#define DEF_ARG_DEVNAME		"/dev/nvme0n1"
#define DEF_ARG_RWMOD		false
#define DEF_ARG_SRMOD		true
#define DEF_ARG_OFFSET		0
#define DEF_ARG_SIZE		1*1024*1024*1024
#define DEF_ARG_XFERSIZE	128*1024
#define DEF_ARG_PATTERN		0xCA5775AC
#define DEF_ARG_NUMOFQ		4
#define DEF_ARG_QDEPTH		32
#define DEF_ARG_EXECTIME	60

typedef struct _stCmdArgs
{
	string devname;
	bool rwmod;
	bool srmod;
	uint64_t offset;
	uint64_t size;
	uint32_t xfersize;
	uint32_t pattern;
	uint16_t numOfQ;
	uint16_t qDepth;
	uint32_t execTime;
}stCmdArgs;

int OpenDevice(const char* name)
{
	int fd	= INVALID_DEVHDLR;

	if( (fd = open(name,O_RDWR | O_DIRECT)) < 0 )
	{
		LOGERROR("ERROR: Cannot open device: %s\n",name);
		return INVALID_DEVHDLR;
	}

	return fd;
}

int ParseCommandLine(int argc, char *argv[],stCmdArgs& cmdArgs)
{
	namespace po = boost::program_options;
    po::options_description desc(PROGRAM_NAME " "
    		PROGRAM_VER " " PROGRAM_AUTHOR "\n"
			"options");

    desc.add_options()
		("help,h", "Print utilDevScan Tool Usage")
		("devname,n", po::value<string>( &(cmdArgs.devname) )->default_value(DEF_ARG_DEVNAME), "Dev Name , eg: /dev/nvme0n1")
		("rwmod,m", po::value<bool>( &(cmdArgs.rwmod) )->default_value(DEF_ARG_RWMOD), "Read/Write Mode , True: Read, False: Write")
		("srmod,s", po::value<bool>( &(cmdArgs.srmod) )->default_value(DEF_ARG_SRMOD), "Seq/Rand Mode, True: Seq, False: Random")
		("offset,o", po::value<uint64_t>( &(cmdArgs.offset) )->default_value(DEF_ARG_OFFSET), "Start Offset(byte)")
		("size,l", po::value<uint64_t>( &(cmdArgs.size) )->default_value(DEF_ARG_SIZE), "Test Size(byte)")
		("xfersize,x", po::value<uint32_t>( &(cmdArgs.xfersize) )->default_value(DEF_ARG_XFERSIZE), "Test xferSize(byte)")
		("pattern,p", po::value<uint32_t>( &(cmdArgs.pattern) )->default_value(DEF_ARG_PATTERN), "Data Pattern ,4 Bytes")
		("numq,q", po::value<uint16_t>( &(cmdArgs.numOfQ) )->default_value(DEF_ARG_NUMOFQ), "Num Of Queue")
		("qdepth,d", po::value<uint16_t>( &(cmdArgs.qDepth) )->default_value(DEF_ARG_QDEPTH), "Queue Depth")
		("exectime,t", po::value<uint32_t>( &(cmdArgs.execTime) )->default_value(DEF_ARG_EXECTIME), "Execution Time(sec)")
		;

    po::variables_map vm;
    try{
    	po::store(po::parse_command_line(argc, argv, desc), vm);
    }
    catch(const char* msg){
    	printf("Error: %d!\n",msg);
    	return -1;
    }
    po::notify(vm);

    if( vm.count("help") ){
        std::cout << desc << std::endl;
        return 0;
    }

    return 1;
}

int main(int argc, char *argv[])
{
	stCmdArgs cmdArgs;

	int rtn = ParseCommandLine(argc,argv,cmdArgs);
	if(0 >= rtn)
	{
		return 0;
	}

	const char* devName	= cmdArgs.devname.c_str();
	bool rwmod			= cmdArgs.rwmod;
	bool srmod			= cmdArgs.srmod;
	uint64_t offset		= cmdArgs.offset;
	uint64_t size		= cmdArgs.size;
	uint32_t xfersize	= cmdArgs.xfersize;
	uint32_t pattern	= cmdArgs.pattern;
	uint16_t numOfQ		= cmdArgs.numOfQ;
	uint16_t qDepth		= cmdArgs.qDepth;
	uint32_t execTime	= cmdArgs.execTime;

	int fd = OpenDevice(devName);
	if(INVALID_DEVHDLR == fd)
	{
		return -1;
	}

	PerfParam	param;
	InitPerfParam(&param,numOfQ,qDepth,execTime);
	SetErrAborted(&param,1);

	InitPerfTask(&(param.sPerfTask));
	if(true == rwmod)
	{
		if(true == srmod)
		{
			FillTaskWSeqRead(&(param.sPerfTask),offset,size,xfersize,pattern);
		}
		else
		{
			FillTaskWRandRead(&(param.sPerfTask),offset,size,xfersize,pattern);
		}
	}
	else
	{
		if(true == srmod)
		{
			FillTaskWSeqWrite(&(param.sPerfTask),offset,size,xfersize,pattern);
		}
		else
		{
			FillTaskWRandWrite(&(param.sPerfTask),offset,size,xfersize,pattern);
		}
	}

	PerfStat	stat;
	InitPerfStat(&stat);

	PerfEngineOp	ioengine = {
			.setup		= aio_setup,
			.cleanup	= aio_cleanup,
			.prepare_io = aio_prepare_io,
			.submit_io	= aio_submit_io,
			.check_io	= aio_check_io,
			.verify_io	= aio_verify_io,
	};

	rtn = perf_main_loop(0,fd,&param,&ioengine,&stat);

	LOGINFO("%d - %s",fd,devName);
	LOGINFO("IO Completed = %d,IO CMD Errors = %d, IO DMC Errors = %d, Duration= %d",
			stat.uIOCompleted,stat.uIOErrors,stat.uIODMCError,stat.uEclapsedTime/NSEC_PER_SEC);
	LOGINFO("MBps = %.2f,IOPS = %.2f",CalcMBps(&stat,xfersize),CalcIOps(&stat));

	return rtn;
}
