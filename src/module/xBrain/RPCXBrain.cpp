/*
 * file : RPCXBrain.cpp
 */

#include "RPCXBrain.h"
#include "Logging.h"

uint16_t	RPCXBrainClient::GetGlobalTaskID(uint16_t taskID)
{
	TaskDesc* task = NULL;
	if(false == mpTaskMngr->FindTaskDescByIndex(taskID,task) || NULL == task)
	{
		return (uint16_t)-1;
	}

	uint16_t tID	= (uint16_t)(task->GetRsvVal());

	delete task;
	return tID;
}

bool RPCXBrainClient::cb_RunCallbackRoutine(void)
{
	if(0 != mpTaskMngr->GetTotalCount()
			&& (0 == GetLeftCount() || (uint16_t)-1 == GetLeftCount())
			&& mpTaskMngr->GetFinishCnt() == mpTaskMngr->GetTotalCount())
	{
		LOGINFO("Finish All Tasks.Stop!!!");
		return false;
	}

	if(0 < GetLeftCount() && 0 == mpTaskMngr->GetPendingCnt()
			&& GetAvailSlotCount() > mpTaskMngr->GetRunningCnt())
	{
		TaskDesc task;
		if(true == GetTaskDesc(&task))
		{
			task.SetRsvVal((uint16_t)(task.GetTaskIndex()));

			if(false == ReqHdlr_AddTask(task.GetDataPoint()))
			{
				LOGERROR("Fail to Add new Task,the Task will be lost!!!");
			}
		}
	}

	return IPCTaskMngrServer::cb_RunCallbackRoutine();
}

bool RPCXBrainClient::UpdateTaskResult(uint16_t taskID,uint8_t slotID)
{
	int rslt	= -1;
	if(false == GetSlotLastResult(slotID,rslt))
	{
		LOGERROR("Fail to call GetSlotLastResult(%d,rslt)",slotID);
		return false;
	}

	uint16_t gTID	= GetGlobalTaskID(taskID);

	TaskResult tr;
	tr.SetRsltIndex(gTID);
	tr.SetSlotID(slotID);
	tr.SetTestResult(rslt);
	tr.SetTargetPhyAddr(mHostAddr);

	UptTaskRslt(&tr);

	return IPCTaskMngrServer::UpdateTaskResult(taskID,slotID);
}
