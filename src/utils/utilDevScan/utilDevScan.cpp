/*
 * file : utilDevScan.cpp
 */
#include <iostream>
#include <boost/program_options.hpp>
#include "udev/udevScan.h"

#define PROGRAM_NAME		"utilDevScan"
#define PROGRAM_VER			"V1.0"
#define PROGRAM_AUTHOR		"XIAOWJ"

//#define VBOX_DEBUG			0

#ifndef VBOX_DEBUG
#define NVME_DEV_SUBSYSTEM	"nvme"
#else
#define NVME_DEV_SUBSYSTEM	"misc"
#endif

#define PCI_SUBSYSTEM		"pci"
#define PCI_BRIDGE_CLASS	"Bridge"
#define PCI_AHCI_DRIVE		"ahci"

typedef struct _stCmdArgs
{
	bool bNVMeDev;
	bool bPCIBridge;
	bool bPCIAHCI;
}stCmdArgs;

//
//	devPath : /devices/pci0000:00/0000:00:0e.0/nvme/nvme0
//
const char* GetPCIAddrFromDevPath(string devPath)
{

	size_t pos = devPath.find(NVME_DEV_SUBSYSTEM);
	if(string::npos == pos)
	{
		return NULL;
	}
	devPath.erase(pos-1);

	pos = devPath.rfind("/");
	if(string::npos == pos)
	{
		return NULL;
	}
	devPath.erase(0,pos+1);

	return devPath.c_str();
}

bool PrintNVMeDevInfo(void)
{
	uDevScan	devScan;

	if(false == devScan.DoScan(ScanBy_SubSystem,NVME_DEV_SUBSYSTEM))
	{
		printf("ERROR: fail to DoScan\n");
		return false;
	}

	printf("\tDEVNODE\t\tSUBSYSTEM\tPCIEADDR\n");
	printf("\t------------------------------------------------------------------\n");

	for(int idx=0;idx<devScan.GetDevNodeCount();idx++)
	{
		uDevNode* node	= devScan.GetDevNode(idx);
		printf("\t%s",node->GetDevNode());
		printf("\t%s",node->GetSubSystem());
		printf("\t%s",GetPCIAddrFromDevPath(node->GetDevPath()));
		printf("\n");
	}

	return true;
}

bool PrintPCIBridgeInfo(void)
{
	uDevScan	devScan;

	if(false == devScan.DoScan(ScanBy_SubSystem,PCI_SUBSYSTEM))
	{
		printf("ERROR: fail to DoScan\n");
		return false;
	}

	printf("\tSUBSYSTEM\tDEVTYPE\tVENDOR\t\t\tMODEL\n");
	printf("\t------------------------------------------------------------------\n");

	for(int idx=0;idx<devScan.GetDevNodeCount();idx++)
	{
		uDevNode* node	= devScan.GetDevNode(idx);
		const char* devType = node->GetDevProperty("ID_PCI_CLASS_FROM_DATABASE");
		if(0 == strncmp(PCI_BRIDGE_CLASS,devType,strlen(PCI_BRIDGE_CLASS)))
		{
			printf("\t%s",node->GetSubSystem());
			printf("\t\t%s",devType);
			printf("\t%s",node->GetDevProperty("ID_VENDOR_FROM_DATABASE"));
			printf("\t%s",node->GetDevProperty("ID_MODEL_FROM_DATABASE"));
			printf("\n");
		}
	}

	return true;
}

bool PrintPCIAHCIInfo(void)
{
	uDevScan	devScan;

	if(false == devScan.DoScan(ScanBy_SubSystem,PCI_SUBSYSTEM))
	{
		printf("ERROR: fail to DoScan\n");
		return false;
	}

	printf("\tSUBSYSTEM\tDRIVER\tVENDOR\t\t\tINTERFACE\tSLOTNAME\n");
	printf("\t------------------------------------------------------------------\n");

	for(int idx=0;idx<devScan.GetDevNodeCount();idx++)
	{
		uDevNode* node	= devScan.GetDevNode(idx);
		const char* drvName = node->GetDevProperty("DRIVER");
		if(0 == strncmp(PCI_AHCI_DRIVE,drvName,strlen(PCI_AHCI_DRIVE)))
		{
			printf("\t%s",node->GetSubSystem());
			printf("\t\t%s",drvName);
			printf("\t%s",node->GetDevProperty("ID_VENDOR_FROM_DATABASE"));
			printf("\t%s",node->GetDevProperty("ID_PCI_INTERFACE_FROM_DATABASE"));
			printf("\t%s",node->GetDevProperty("PCI_SLOT_NAME"));
			printf("\n");
		}
	}

	return true;
}

bool ParseCommandLine(int argc, char *argv[],stCmdArgs& cmdArgs)
{
	namespace po = boost::program_options;
    po::options_description desc(PROGRAM_NAME " "
    		PROGRAM_VER " " PROGRAM_AUTHOR "\n"
			"options");

    desc.add_options()
		("help,h", "Print utilDevScan Tool Usage")
		("nvmeDev,n", po::bool_switch( &(cmdArgs.bNVMeDev) ), "Scan all nvme devices")
		("pciBridge,b", po::bool_switch( &(cmdArgs.bPCIBridge) ), "Scan all PCI bridge devices")
		("pciAHCI,a", po::bool_switch( &(cmdArgs.bPCIAHCI) ), "Scan all PCI AHCI Controller")
		;

    po::variables_map vm;
    try{
    	po::store(po::parse_command_line(argc, argv, desc), vm);
    }
    catch(...){
    	return false;
    }
    po::notify(vm);

    if( vm.count("help") ){
        std::cout << desc << std::endl;
    }

    return true;
}

int main(int argc, char *argv[])
{
	stCmdArgs cmdArgs;
	if(false == ParseCommandLine(argc,argv,cmdArgs))
	{
		printf("Error: Invalud Argument!\n");
		return -1;
	}

	if(true == cmdArgs.bNVMeDev && false == PrintNVMeDevInfo())
	{
		return -1;
	}

	if(true == cmdArgs.bPCIBridge && false == PrintPCIBridgeInfo())
	{
		return -1;
	}

	if(true == cmdArgs.bPCIAHCI && false == PrintPCIAHCIInfo())
	{
		return -1;
	}

	return 0;
}
