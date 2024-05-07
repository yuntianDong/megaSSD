/*
 * file: TaskReport.cpp
 */

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <boost/format.hpp>
#include "test/TaskReport.h"
#include "utility/CRC.h"
#include "Logging.h"

#define SEPERATE_LINE1			"####################################################################\n"
#define REPORT_HEADER			"                 YMTC Test Report(V0.1)\n"
#define	SEPERATE_LINE2			"--------------------------------------------------------------------\n"
#define RSLT_STAT_HEAD			"                 Test Result Statistic\n"
#define RSLT_STAT_FORMAT		"                  %10s : %-03d\n"
#define RSLT_STAT_SEPERATE		"                 ---------------------\n"
#define RSLT_STAT_SUMMARY		"             Finish Rate : %.1f%%  Pass Rate : %.1f%%\n"
#define RSLT_ERRCODE_FORMAT		"E%04x"
#define FAIL_TASK_HEAD			"                 Failed Testcase List\n"
#define FAIL_TASK_FORMAT		"  %03d - %-36s E%04x %-12s(%02d)\n"
#define REPORT_FOOTER			"                                                      %4X XWJ@ASHA\n"

uint16_t TaskReport::CalcCRC16(string output)
{
	return CalCRC16((uint8_t*)output.c_str(),output.length());
}

void TaskReport::DumpRsltStat(string& output)
{
	output += SEPERATE_LINE2;
	output += RSLT_STAT_HEAD;

	int	passCnt,failCnt,runCnt;
	passCnt = failCnt = runCnt = 0;

	for(RSLT_STAT::iterator it=mRsltStat.begin();it!=mRsltStat.end();it++)
	{
		int rsltCode	= it->first;
		int ttlCount	= it->second;

		boost::format lineFmt(RSLT_STAT_FORMAT);

		if(0 == rsltCode)
		{
			lineFmt % "Passed" % ttlCount;
			passCnt = ttlCount;
		}
		else if(RSLT_E_NOTRUN == rsltCode)
		{
			lineFmt % "Running" % ttlCount;
			runCnt = ttlCount;
		}
		else
		{
			boost::format errCode(RSLT_ERRCODE_FORMAT);
			errCode % rsltCode;
			lineFmt % errCode.str() % ttlCount;
			failCnt += ttlCount;
		}

		output += lineFmt.str();
	}

	output += RSLT_STAT_SEPERATE;

	double	finiRate	= 0.0;
	double	passRate	= 0.0;
	if(0 != passCnt+failCnt+runCnt)
	{
		finiRate	= (double)(passCnt+failCnt)*100 / (double)(passCnt+failCnt+runCnt);
		passRate	= (double)(passCnt)*100 / (double)(passCnt+failCnt+runCnt);
	}

	boost::format summFmt(RSLT_STAT_SUMMARY);

	summFmt % finiRate % passRate;
	output += summFmt.str();

	output += SEPERATE_LINE2;
}

void TaskReport::DumpFailTask(string& output)
{
	output += SEPERATE_LINE2;
	output += FAIL_TASK_HEAD;

	for(FAIL_DESC::iterator it=mFailList.begin();it!=mFailList.end();it++)
	{
		string 		name = it->first;
		TaskResult* rslt = it->second;

		boost::format lineFmt(FAIL_TASK_FORMAT);

		lineFmt % rslt->GetRsltIndex() % name % rslt->GetTestResult() %\
				rslt->GetTargetPhyAddr() % rslt->GetSlotID();

		output += lineFmt.str();
	}

	output += SEPERATE_LINE2;
}

void TaskReport::Report(string& output)
{
	output += SEPERATE_LINE1;
	output += REPORT_HEADER;

	DumpRsltStat(output);
	DumpFailTask(output);

	boost::format footer(REPORT_FOOTER);
	footer % CalcCRC16(output);
	output += footer.str();

	output += SEPERATE_LINE1;
}

void TaskReport::Report(void)
{
	string	output = "";
	Report(output);

	printf(output.c_str());
}

bool TaskReport::Report(const char* filename)
{
	ofstream ofile;
	ofile.open(filename, ios::out);

	if(!ofile)
	{
		LOGERROR("Fail to Open %s",filename);
		return false;
	}

	string	output = "";
	Report(output);
	ofile << output;

	ofile.close();
	return true;
}
