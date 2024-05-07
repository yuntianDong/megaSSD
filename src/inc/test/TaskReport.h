#ifndef _TASK_REPORT_H
#define _TASK_REPORT_H

#include <map>
#include <string>

#include "TaskResult.h"
using namespace std;

#define RSLT_E_NOTRUN			0x3FFFFFFF

class TaskReport
{
private:
	typedef map<int,int>				RSLT_STAT;
	typedef map<string,TaskResult*>		FAIL_DESC;

	RSLT_STAT				mRsltStat;
	FAIL_DESC				mFailList;
protected:
	uint16_t CalcCRC16(string output);
	void IncRsltCount(int rslt)
	{
		RSLT_STAT::iterator it = mRsltStat.find(rslt);
		if(it != mRsltStat.end())
		{
			mRsltStat[rslt] += 1;
		}
		else
		{
			mRsltStat[rslt]	= 1;
		}
	};
	void InsFailTask(string name,TaskResult* rslt)
	{
		mFailList.insert(FAIL_DESC::value_type(name,rslt));
	};
public:
	TaskReport(void)
	{
		mRsltStat.clear();
		mFailList.clear();
	};

	void Record(string name,TaskResult* rslt)
	{
		if(true == rslt->IsValid())
		{
			int r = rslt->GetTestResult();
			IncRsltCount(r);

			if(0 != r)
			{
				InsFailTask(name,rslt);
			}
		}
		else
		{
			IncRsltCount(RSLT_E_NOTRUN);
		}
	};

	void DumpRsltStat(string& output);
	void DumpFailTask(string& output);

	bool Report(const char* filename);
	void Report(string& output);
	void Report(void);
};

#endif
