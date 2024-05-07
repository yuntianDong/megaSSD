/*
 * file : JsonRPC2TaskMngr.h
 */

#include "net/JsonRPC2TaskMngr.h"

#define STRING_SEPERATE_CHAR		","

const char* ConcatenateString(char* str1,char* str2,char* str3,const char* sep)
{
	std::string s1 = str1;
	std::string s2 = str2;
	std::string s3 = str3;
	std::string ch = sep;

	s1 = s1 + ch + s2;
	if(0 < strlen(str3))
	{
		s1 = s1 + ch + s3;
	}

	return s1.c_str();
}
void SplitString(const char* str,char* str1,char* str2,char* str3,const char* sep)
{
	char *p	= strtok((char*)str,sep);
	int idx = 0;

	while(p && idx < 3)
	{
		if(0 == idx)
		{
			strncpy(str1,p,strlen(p));
			str1[strlen(p)] = '\0';
		}
		else if(1 == idx)
		{
			strncpy(str2,p,strlen(p));
			str2[strlen(p)] = '\0';
		}
		else if(2 == idx)
		{
			strncpy(str3,p,strlen(p));
			str3[strlen(p)] = '\0';
		}

		p=strtok(NULL,sep);
		idx++;
	}
}
const char* ConcatenateNumber(uint32_t val1,uint32_t val2,uint32_t val3,const char* sep)
{
	char cVal1[10];memset(cVal1,0,sizeof(char)*10);
	char cVal2[10];memset(cVal2,0,sizeof(char)*10);
	char cVal3[10];memset(cVal3,0,sizeof(char)*10);

	snprintf(cVal1,10,"%d",val1);
	snprintf(cVal2,10,"%d",val2);
	snprintf(cVal3,10,"%d",val3);

	return ConcatenateString(cVal1,cVal2,cVal3,sep);
}
void SplitNumber(const char* str,uint32_t& val1,uint32_t& val2,uint32_t& val3,const char* sep)
{
	char cVal1[10];memset(cVal1,0,sizeof(char)*10);
	char cVal2[10];memset(cVal2,0,sizeof(char)*10);
	char cVal3[10];memset(cVal3,0,sizeof(char)*10);

	SplitString(str,cVal1,cVal2,cVal3,sep);

	sscanf(cVal1,"%d",&val1);
	sscanf(cVal2,"%d",&val2);
	sscanf(cVal3,"%d",&val3);
}

void FillJsonReqWithTaskDesc(RPCJsonReq& req,TaskDesc& task)
{
	stTaskDesc* taskDesc	= task.GetDataPoint();

	req.AddParam<uint16_t>("index",taskDesc->index);
	req.AddParam<std::string>("name",taskDesc->name);
	req.AddParam<std::string>("cmdline",taskDesc->cmdline);
	req.AddParam<uint32_t>("execTimeout",taskDesc->execTimeout);
	req.AddParam<uint32_t>("loopCount",taskDesc->loopCount);

	req.AddParam<std::string>("reqSN",ConcatenateString(taskDesc->taskCond.reqSN[0],taskDesc->taskCond.reqSN[1],taskDesc->taskCond.reqSN[2],STRING_SEPERATE_CHAR));
	req.AddParam<std::string>("excSN",ConcatenateString(taskDesc->taskCond.excSN[0],taskDesc->taskCond.excSN[1],taskDesc->taskCond.excSN[2],STRING_SEPERATE_CHAR));
	req.AddParam<std::string>("reqMN",ConcatenateString(taskDesc->taskCond.reqMN[0],taskDesc->taskCond.reqMN[1],taskDesc->taskCond.reqMN[2],STRING_SEPERATE_CHAR));
	req.AddParam<std::string>("excMN",ConcatenateString(taskDesc->taskCond.excMN[0],taskDesc->taskCond.excMN[1],taskDesc->taskCond.excMN[2],STRING_SEPERATE_CHAR));
	req.AddParam<std::string>("reqFR",ConcatenateString(taskDesc->taskCond.reqFR[0],taskDesc->taskCond.reqFR[1],taskDesc->taskCond.reqFR[2],STRING_SEPERATE_CHAR));
	req.AddParam<std::string>("excFR",ConcatenateString(taskDesc->taskCond.excFR[0],taskDesc->taskCond.excFR[1],taskDesc->taskCond.excFR[2],STRING_SEPERATE_CHAR));
	req.AddParam<std::string>("reqGR",ConcatenateString(taskDesc->taskCond.reqGR[0],taskDesc->taskCond.reqGR[1],taskDesc->taskCond.reqGR[2],STRING_SEPERATE_CHAR));
	req.AddParam<std::string>("excGR",ConcatenateString(taskDesc->taskCond.excGR[0],taskDesc->taskCond.excGR[1],taskDesc->taskCond.excGR[2],STRING_SEPERATE_CHAR));
	req.AddParam<std::string>("reqGB",ConcatenateNumber(taskDesc->taskCond.reqGB[0],taskDesc->taskCond.reqGB[1],taskDesc->taskCond.reqGB[2],STRING_SEPERATE_CHAR));
	req.AddParam<std::string>("excGB",ConcatenateNumber(taskDesc->taskCond.excGB[0],taskDesc->taskCond.excGB[1],taskDesc->taskCond.excGB[2],STRING_SEPERATE_CHAR));
	req.AddParam<std::string>("reqSS",ConcatenateNumber(taskDesc->taskCond.reqSS[0],taskDesc->taskCond.reqSS[1],taskDesc->taskCond.reqSS[2],STRING_SEPERATE_CHAR));
	req.AddParam<std::string>("excSS",ConcatenateNumber(taskDesc->taskCond.excSS[0],taskDesc->taskCond.excSS[1],taskDesc->taskCond.excSS[2],STRING_SEPERATE_CHAR));

}
void FillJsonReqWithTaskRslt(RPCJsonReq& req,TaskResult& rslt)
{
	stRsltDesc* rsltDesc	= rslt.GetDataPoint();

	req.AddParam<uint16_t>("index",rsltDesc->index);
	req.AddParam<int>("result",rsltDesc->result);
	req.AddParam<uint32_t>("execTime",rsltDesc->execTime);
	req.AddParam<uint8_t>("slotID",rsltDesc->slotID);
	req.AddParam<bool>("valid",rsltDesc->valid);
	req.AddParam<std::string>("tagtAddr",rsltDesc->tagtAddr);
}
void GetTaskDescFromJsonReq(TaskDesc& task,RPCJsonReq& req)
{
	stTaskDesc* taskDesc	= task.GetDataPoint();

	taskDesc->index			= req.GetParam<uint16_t>("index");
	strncpy(taskDesc->name,req.GetParam<std::string>("name").c_str(),TASK_NAME_LEN);
	strncpy(taskDesc->cmdline,req.GetParam<std::string>("cmdline").c_str(),TASK_CMDLINE_LEN);
	taskDesc->execTimeout	= req.GetParam<uint32_t>("execTimeout");
	taskDesc->loopCount		= req.GetParam<uint32_t>("loopCount");

	SplitString(req.GetParam<std::string>("reqSN").c_str(),taskDesc->taskCond.reqSN[0],taskDesc->taskCond.reqSN[1],taskDesc->taskCond.reqSN[2],STRING_SEPERATE_CHAR);
	SplitString(req.GetParam<std::string>("excSN").c_str(),taskDesc->taskCond.excSN[0],taskDesc->taskCond.excSN[1],taskDesc->taskCond.excSN[2],STRING_SEPERATE_CHAR);
	SplitString(req.GetParam<std::string>("reqMN").c_str(),taskDesc->taskCond.reqMN[0],taskDesc->taskCond.reqMN[1],taskDesc->taskCond.reqMN[2],STRING_SEPERATE_CHAR);
	SplitString(req.GetParam<std::string>("excMN").c_str(),taskDesc->taskCond.excMN[0],taskDesc->taskCond.excMN[1],taskDesc->taskCond.excMN[2],STRING_SEPERATE_CHAR);
	SplitString(req.GetParam<std::string>("reqFR").c_str(),taskDesc->taskCond.reqFR[0],taskDesc->taskCond.reqFR[1],taskDesc->taskCond.reqFR[2],STRING_SEPERATE_CHAR);
	SplitString(req.GetParam<std::string>("excFR").c_str(),taskDesc->taskCond.excFR[0],taskDesc->taskCond.excFR[1],taskDesc->taskCond.excFR[2],STRING_SEPERATE_CHAR);
	SplitString(req.GetParam<std::string>("reqGR").c_str(),taskDesc->taskCond.reqGR[0],taskDesc->taskCond.reqGR[1],taskDesc->taskCond.reqGR[2],STRING_SEPERATE_CHAR);
	SplitString(req.GetParam<std::string>("excGR").c_str(),taskDesc->taskCond.excGR[0],taskDesc->taskCond.excGR[1],taskDesc->taskCond.excGR[2],STRING_SEPERATE_CHAR);
	SplitNumber(req.GetParam<std::string>("reqGB").c_str(),taskDesc->taskCond.reqGB[0],taskDesc->taskCond.reqGB[1],taskDesc->taskCond.reqGB[2],STRING_SEPERATE_CHAR);
	SplitNumber(req.GetParam<std::string>("excGB").c_str(),taskDesc->taskCond.excGB[0],taskDesc->taskCond.excGB[1],taskDesc->taskCond.excGB[2],STRING_SEPERATE_CHAR);
	SplitNumber(req.GetParam<std::string>("reqSS").c_str(),taskDesc->taskCond.reqSS[0],taskDesc->taskCond.reqSS[1],taskDesc->taskCond.reqSS[2],STRING_SEPERATE_CHAR);
	SplitNumber(req.GetParam<std::string>("excSS").c_str(),taskDesc->taskCond.excSS[0],taskDesc->taskCond.excSS[1],taskDesc->taskCond.excSS[2],STRING_SEPERATE_CHAR);

}
void GetTaskRsltFromJsonReq(TaskResult& rslt,RPCJsonReq& req)
{
	stRsltDesc* rsltDesc	= rslt.GetDataPoint();

	rsltDesc->index		= req.GetParam<uint16_t>("index");
	rsltDesc->result	= req.GetParam<int>("result");
	rsltDesc->execTime	= req.GetParam<uint32_t>("execTime");
	rsltDesc->slotID	= req.GetParam<uint8_t>("slotID");
	rsltDesc->valid		= req.GetParam<bool>("valid");
	strncpy(rsltDesc->tagtAddr,req.GetParam<std::string>("tagtAddr").c_str(),TAGT_ADDR_MAX_LEN);
}
void FillJsonRespWithTaskDesc(RPCJsonRespWithSuccess& resp,TaskDesc& task)
{
	stTaskDesc* taskDesc	= task.GetDataPoint();

	resp.AddResult<int>("index",taskDesc->index);
	resp.AddResult<std::string>("name",taskDesc->name);
	resp.AddResult<std::string>("cmdline",taskDesc->cmdline);
	resp.AddResult<int>("execTimeout",taskDesc->execTimeout);
	resp.AddResult<int>("loopCount",taskDesc->loopCount);

	resp.AddResult<std::string>("reqSN",ConcatenateString(taskDesc->taskCond.reqSN[0],taskDesc->taskCond.reqSN[1],taskDesc->taskCond.reqSN[2],STRING_SEPERATE_CHAR));
	resp.AddResult<std::string>("excSN",ConcatenateString(taskDesc->taskCond.excSN[0],taskDesc->taskCond.excSN[1],taskDesc->taskCond.excSN[2],STRING_SEPERATE_CHAR));
	resp.AddResult<std::string>("reqMN",ConcatenateString(taskDesc->taskCond.reqMN[0],taskDesc->taskCond.reqMN[1],taskDesc->taskCond.reqMN[2],STRING_SEPERATE_CHAR));
	resp.AddResult<std::string>("excMN",ConcatenateString(taskDesc->taskCond.excMN[0],taskDesc->taskCond.excMN[1],taskDesc->taskCond.excMN[2],STRING_SEPERATE_CHAR));
	resp.AddResult<std::string>("reqFR",ConcatenateString(taskDesc->taskCond.reqFR[0],taskDesc->taskCond.reqFR[1],taskDesc->taskCond.reqFR[2],STRING_SEPERATE_CHAR));
	resp.AddResult<std::string>("excFR",ConcatenateString(taskDesc->taskCond.excFR[0],taskDesc->taskCond.excFR[1],taskDesc->taskCond.excFR[2],STRING_SEPERATE_CHAR));
	resp.AddResult<std::string>("reqGR",ConcatenateString(taskDesc->taskCond.reqGR[0],taskDesc->taskCond.reqGR[1],taskDesc->taskCond.reqGR[2],STRING_SEPERATE_CHAR));
	resp.AddResult<std::string>("excGR",ConcatenateString(taskDesc->taskCond.excGR[0],taskDesc->taskCond.excGR[1],taskDesc->taskCond.excGR[2],STRING_SEPERATE_CHAR));
	resp.AddResult<std::string>("reqGB",ConcatenateNumber(taskDesc->taskCond.reqGB[0],taskDesc->taskCond.reqGB[1],taskDesc->taskCond.reqGB[2],STRING_SEPERATE_CHAR));
	resp.AddResult<std::string>("excGB",ConcatenateNumber(taskDesc->taskCond.excGB[0],taskDesc->taskCond.excGB[1],taskDesc->taskCond.excGB[2],STRING_SEPERATE_CHAR));
	resp.AddResult<std::string>("reqSS",ConcatenateNumber(taskDesc->taskCond.reqSS[0],taskDesc->taskCond.reqSS[1],taskDesc->taskCond.reqSS[2],STRING_SEPERATE_CHAR));
	resp.AddResult<std::string>("excSS",ConcatenateNumber(taskDesc->taskCond.excSS[0],taskDesc->taskCond.excSS[1],taskDesc->taskCond.excSS[2],STRING_SEPERATE_CHAR));

}
void FillJsonRespWithTaskRslt(RPCJsonRespWithSuccess& resp,TaskResult& rslt)
{
	stRsltDesc* rsltDesc	= rslt.GetDataPoint();

	resp.AddResult<int>("index",rsltDesc->index);
	resp.AddResult<int>("result",rsltDesc->result);
	resp.AddResult<int>("execTime",rsltDesc->execTime);
	resp.AddResult<int>("slotID",rsltDesc->slotID);
	resp.AddResult<bool>("valid",rsltDesc->valid);
	resp.AddResult<std::string>("tagtAddr",rsltDesc->tagtAddr);
}
void GetTaskDescFromJsonResp(TaskDesc& task,RPCJsonRespWithSuccess& resp)
{
	stTaskDesc* taskDesc	= task.GetDataPoint();

	taskDesc->index			= resp.GetResult<uint16_t>("index");
	strncpy(taskDesc->name,resp.GetResult<std::string>("name").c_str(),TASK_NAME_LEN);
	strncpy(taskDesc->cmdline,resp.GetResult<std::string>("cmdline").c_str(),TASK_CMDLINE_LEN);
	taskDesc->execTimeout	= resp.GetResult<uint32_t>("execTimeout");
	taskDesc->loopCount		= resp.GetResult<uint32_t>("loopCount");

	SplitString(resp.GetResult<std::string>("reqSN").c_str(),taskDesc->taskCond.reqSN[0],taskDesc->taskCond.reqSN[1],taskDesc->taskCond.reqSN[2],STRING_SEPERATE_CHAR);
	SplitString(resp.GetResult<std::string>("excSN").c_str(),taskDesc->taskCond.excSN[0],taskDesc->taskCond.excSN[1],taskDesc->taskCond.excSN[2],STRING_SEPERATE_CHAR);
	SplitString(resp.GetResult<std::string>("reqMN").c_str(),taskDesc->taskCond.reqMN[0],taskDesc->taskCond.reqMN[1],taskDesc->taskCond.reqMN[2],STRING_SEPERATE_CHAR);
	SplitString(resp.GetResult<std::string>("excMN").c_str(),taskDesc->taskCond.excMN[0],taskDesc->taskCond.excMN[1],taskDesc->taskCond.excMN[2],STRING_SEPERATE_CHAR);
	SplitString(resp.GetResult<std::string>("reqFR").c_str(),taskDesc->taskCond.reqFR[0],taskDesc->taskCond.reqFR[1],taskDesc->taskCond.reqFR[2],STRING_SEPERATE_CHAR);
	SplitString(resp.GetResult<std::string>("excFR").c_str(),taskDesc->taskCond.excFR[0],taskDesc->taskCond.excFR[1],taskDesc->taskCond.excFR[2],STRING_SEPERATE_CHAR);
	SplitString(resp.GetResult<std::string>("reqGR").c_str(),taskDesc->taskCond.reqGR[0],taskDesc->taskCond.reqGR[1],taskDesc->taskCond.reqGR[2],STRING_SEPERATE_CHAR);
	SplitString(resp.GetResult<std::string>("excGR").c_str(),taskDesc->taskCond.excGR[0],taskDesc->taskCond.excGR[1],taskDesc->taskCond.excGR[2],STRING_SEPERATE_CHAR);

	SplitNumber(resp.GetResult<std::string>("reqGB").c_str(),taskDesc->taskCond.reqGB[0],taskDesc->taskCond.reqGB[1],taskDesc->taskCond.reqGB[2],STRING_SEPERATE_CHAR);
	SplitNumber(resp.GetResult<std::string>("excGB").c_str(),taskDesc->taskCond.excGB[0],taskDesc->taskCond.excGB[1],taskDesc->taskCond.excGB[2],STRING_SEPERATE_CHAR);
	SplitNumber(resp.GetResult<std::string>("reqSS").c_str(),taskDesc->taskCond.reqSS[0],taskDesc->taskCond.reqSS[1],taskDesc->taskCond.reqSS[2],STRING_SEPERATE_CHAR);
	SplitNumber(resp.GetResult<std::string>("excSS").c_str(),taskDesc->taskCond.excSS[0],taskDesc->taskCond.excSS[1],taskDesc->taskCond.excSS[2],STRING_SEPERATE_CHAR);
}
void GetTaskRsltFromJsonResp(TaskResult& rslt,RPCJsonRespWithSuccess& resp)
{
	stRsltDesc* rsltDesc	= rslt.GetDataPoint();

	rsltDesc->index		= resp.GetResult<int>("index");
	rsltDesc->result	= resp.GetResult<int>("result");
	rsltDesc->execTime	= resp.GetResult<int>("execTime");
	rsltDesc->slotID	= resp.GetResult<int>("slotID");
	rsltDesc->valid		= resp.GetResult<bool>("valid");
	strncpy(rsltDesc->tagtAddr,resp.GetResult<std::string>("tagtAddr").c_str(),TAGT_ADDR_MAX_LEN);
}
