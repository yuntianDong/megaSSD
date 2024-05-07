/*
 * file: TaskMngr.cpp
 */

#include <fstream>
#include <iostream>
#include <boost/format.hpp>

#include "test/TaskMngr.h"
#include "Logging.h"

#define RESULT_SAVE_LINE_FORMAT		"%04d,%40s,%2d,%16s\n"

TaskDesc* TaskMngr::FindNextUndoneTaskDesc(void)
{
	TaskDesc* desc	= NULL;
	do{
		LOGDEBUG("mIdxRework = %d",mIdxRework);

		if(TS_ASIGN == GetTaskStatus(mIdxRework))
		{
			desc = GetTaskDesc(mIdxRework);
		}
		GoBack(mIdxRework);
	}while((NULL == desc) && (mIdxRework != GetAddIndex()) && IsValidItem(mIdxRework));

	if(NULL == desc)
	{
		mbNeedRework = false;
	}

	return desc;
}

bool TaskMngr::GetTaskDescForExe(TaskDesc*& pDesc)
{
	if(true == mbNeedRework)
	{
		pDesc	= FindNextUndoneTaskDesc();
		if(NULL != pDesc)
		{
			return true;
		}
	}

	if(true == IsEmpty())
	{
		return false;
	}

	pDesc	= GetTaskDesc(mpTaskMngr->idxFetch);
	if(NULL != pDesc)
	{
		assert(mpTaskMngr->idxFetch == pDesc->GetTaskIndex());

		UptTaskStatus(mpTaskMngr->idxFetch,TS_ASIGN);
		GoForward(mpTaskMngr->idxFetch);
		return true;
	}

	return false;
}

bool TaskMngr::GetTaskDescForCfg(TaskDesc*& pDesc)
{
	if(true == IsFull())
	{
		LOGWARN("The Item is over MAX");
		return false;
	}

	pDesc	= GetTaskDesc(mpTaskMngr->idxAdd);
	if(NULL != pDesc)
	{
		pDesc->Reset();
		pDesc->SetTaskIndex(mpTaskMngr->idxAdd);
		UptTaskStatus(mpTaskMngr->idxAdd,TS_TODO);
		GoForward(mpTaskMngr->idxAdd);
		return true;
	}

	LOGERROR("Fail to GetTaskDesc(%d)",mpTaskMngr->idxAdd);
	return false;
}

bool TaskMngr::MakeTestReport(TaskReport& report)
{
	assert(NULL != mpTaskMngr);

	uint16_t idx = GetFetchIndex();
	GoBack(idx);
	for(;idx != GetAddIndex();)
	{
		if(false == IsValidItem(idx))
		{
			break;
		}

		TaskResult* rslt	= GetTaskResult(idx);
		const char*	name	= GetTaskDesc(idx)->GetTaskName();

		report.Record(name,rslt);
		GoBack(idx);
	}

	return true;
}

void TaskMngr::Dump(void)
{
	printf("idxAdd             = %d\n",mpTaskMngr->idxAdd);
	printf("idxFetch           = %d\n",mpTaskMngr->idxFetch);
	for(int idx=0;idx<TASK_MNGR_ITEMS_MAX;idx++)
	{
		DumpMngrItem(idx);
	}
}

void TaskMngr::DumpMngrItem(uint16_t idx)
{
	TaskDesc* desc		= GetTaskDesc(idx);
	TaskResult* rslt	= GetTaskResult(idx);
	TaskStatus status	= GetTaskStatus(idx);

	printf("[%03d:%d] - [%03d]%10s([%04d]%10s) = [%03d]%d(%02d)\n",
			idx,status,
			desc->GetTaskIndex(),
			desc->GetTaskName(),
			desc->GetLoopCount(),
			desc->GetCmdLine(),
			rslt->GetRsltIndex(),
			rslt->GetTestResult(),
			rslt->GetSlotID());

	delete desc; desc = NULL;
	delete rslt; rslt = NULL;
}

bool TaskMngr::SaveResultAs(string& output)
{
	uint16_t index = GetAddIndex();
	GoForward(index);
	while(index != GetFetchIndex())
	{
		if(true == IsValidItem(index))
		{
			TaskDesc* desc 	= GetTaskDesc(index);
			TaskResult* rslt= GetTaskResult(index);

			const char* name= desc->GetTaskName();
			int result		= rslt->GetTestResult();
			int slotID		= rslt->GetSlotID();
			const char* dut	= rslt->GetTargetPhyAddr();

			boost::format lineFmt(RESULT_SAVE_LINE_FORMAT);

			lineFmt % name % result % slotID % dut;

			output += lineFmt.str();
		}

		GoForward(index);
	}

	return true;
}

bool TaskMngr::SaveResultAs(const char* filename)
{
	ofstream ofile;
	ofile.open(filename, ios::out);

	if(!ofile)
	{
		LOGERROR("Fail to Open %s",filename);
		return false;
	}

	string	output = "";
	SaveResultAs(output);
	ofile << output;

	ofile.close();
	return true;
}

bool TaskMngr::PrintTestResult(void)
{
	string output = "";
	SaveResultAs(output);

	printf("\n%s\n",output.c_str());
	return true;
}
