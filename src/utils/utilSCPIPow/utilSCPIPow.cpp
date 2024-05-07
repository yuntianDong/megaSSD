/*
 * file : utilSCPIPow.cpp
 */

#include <iostream>
#include <boost/program_options.hpp>
#include "power/SCPIPwrCtrlr.h"

using namespace std;

#define PROGRAM_NAME		"utilSCPIPow"
#define PROGRAM_VER			"V1.0"
#define PROGRAM_AUTHOR		"XIAOWJ"

#define SER_CMD_POWON		"POWERON"
#define SER_CMD_POWOFF		"POWEROFF"
#define SER_CMD_GETSTA		"GETSTATUS"
#define SER_CMD_GETVOL		"GETVOL"
#define SER_CMD_GETCUR		"GETCUR"
#define SER_CMD_GETPOW		"GETPOW"

typedef struct _stCmdArgs
{
	string	serPort;
	int		pwrCH;
	string	pwrOpCmd;
	int		pwrOpVal;
}stCmdArgs;

bool ParseCommandLine(int argc, char *argv[],stCmdArgs& cmdArgs)
{
	namespace po = boost::program_options;
    po::options_description desc(PROGRAM_NAME " "
    		PROGRAM_VER " " PROGRAM_AUTHOR "\n"
			"options");

    desc.add_options()
		("help,h", "Print utilDevScan Tool Usage")
		("port,p", po::value<string>(&cmdArgs.serPort)->default_value(""), "Serial Port")
		("channel,c", po::value<int>(&cmdArgs.pwrCH)->default_value(0), "Power Channel")
		("option,o", po::value<string>(&cmdArgs.pwrOpCmd)->default_value("GETSTATUS"), "Operation Name")
		("value,v", po::value<int>(&cmdArgs.pwrOpVal)->default_value(0), "Operation Value")
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

int HandlePowCommand(SCPIPwrCtrlr& pwrCtlr,string cmd,int ch,int val)
{
	//cmd.toupper();
	transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

	if(0 == strncmp(cmd.c_str(),SER_CMD_POWON,strlen(SER_CMD_POWON)) )
	{
		if(false == pwrCtlr.PowerOn(ch))
		{
			printf("ERROR: Fail to execute %s\n",SER_CMD_POWON);
			return -1;
		}
		else
		{
			printf("Succeed!");
			return 0;
		}
	}
	else if(0 == strncmp(cmd.c_str(),SER_CMD_POWOFF,strlen(SER_CMD_POWOFF)))
	{
		if(false == pwrCtlr.PowerOff(ch))
		{
			printf("ERROR: Fail to execute %s\n",SER_CMD_POWOFF);
			return -1;
		}
		else
		{
			printf("Succeed!");
			return 0;
		}
	}
	else if(0 == strncmp(cmd.c_str(),SER_CMD_GETSTA,strlen(SER_CMD_GETSTA)))
	{
		if(CH_POWER_ON == pwrCtlr.GetPowerStatus(ch))
		{
			printf("ON\n");
		}
		else
		{
			printf("OFF\n");
		}
	}
	else if(0 == strncmp(cmd.c_str(),SER_CMD_GETVOL,strlen(SER_CMD_GETVOL)))
	{
		uint32_t val	= pwrCtlr.GetVoltage(ch);
		if((uint32_t)-1 == val)
		{
			printf("ERROR: Fail to execute %s\n",SER_CMD_GETVOL);
			return -1;
		}

		printf("%s = %d\n",SER_CMD_GETVOL,val);
	}
	else if(0 == strncmp(cmd.c_str(),SER_CMD_GETCUR,strlen(SER_CMD_GETCUR)))
	{
		uint32_t val	= pwrCtlr.GetCurrent(ch);
		if((uint32_t)-1 == val)
		{
			printf("ERROR: Fail to execute %s\n",SER_CMD_GETCUR);
			return -1;
		}

		printf("%s = %d\n",SER_CMD_GETCUR,val);
	}
	else if(0 == strncmp(cmd.c_str(),SER_CMD_GETPOW,strlen(SER_CMD_GETPOW)))
	{
		uint32_t val	= pwrCtlr.GetPower(ch);
		if((uint32_t)-1 == val)
		{
			printf("ERROR: Fail to execute %s\n",SER_CMD_GETPOW);
			return -1;
		}

		printf("%s = %d\n",SER_CMD_GETPOW,val);
	}
	else
	{
		printf("ERROR : Invalid Power Command : %s",cmd.c_str());
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	stCmdArgs cmdArgs;
	if(false == ParseCommandLine(argc,argv,cmdArgs))
	{
		printf("Error: Invalud Argument!\n");
		return -1;
	}

	const char * SerPort	= cmdArgs.serPort.c_str();

	if(0 == strlen(SerPort))
	{
		printf("Serial Port is not specified!\n");
		return -1;
	}

	SCPIPwrCtrlr* pPwrCtrlr	=  new SCPIPwrCtrlr(SerPort);
	const char* version	= pPwrCtrlr->GetVersion();
	if(0 == strcmp(version,"ERROR"))
	{
		printf("Error: Fail to connect Port:%s\n",SerPort);
		return -1;
	}

	printf("Version : %s\n",version);

	return HandlePowCommand(*pPwrCtrlr,cmdArgs.pwrOpCmd,cmdArgs.pwrCH,cmdArgs.pwrOpVal);
}

