/*
 * file: TaskDesc.cpp
 */

#include <string.h>

#include "test/TaskDesc.h"
#include "Logging.h"

uint16_t TaskDesc::GetCondItemLength(enCondKey key)
{
	switch(key)
	{
		case CK_SN:	return TASK_SN_LEN;
		case CK_MN: return TASK_MN_LEN;
		case CK_FR: return TASK_FR_LEN;
		case CK_GR: return TASK_GR_LEN;
		case CK_GB:
		case CK_SS:
			return sizeof(uint32_t);
		default:
			return 0;
	}

	return 0;
}

bool TaskDesc::IsCondStrType(enCondKey key)
{
	switch(key)
	{
		case CK_SN:
		case CK_MN:
		case CK_FR:
		case CK_GR:
			return true;
		case CK_GB:
		case CK_SS:
		default:
			return false;
	}

	return false;
}

void TaskDesc::AdvanceTaskCondIndex(enCondKey key,bool bExcluded)
{
	assert(NULL != mpTaskDesc);
	stTaskCond*	pCond	= &(mpTaskDesc->taskCond);

	uint8_t*	pIndex = NULL;
	switch(key)
	{
		case CK_SN:
		{
			pIndex	= (false == bExcluded)?&(pCond->idxReqSN):&(pCond->idxExcSN);
			break;
		}
		case CK_MN:
		{
			pIndex	= (false == bExcluded)?&(pCond->idxReqMN):&(pCond->idxExcMN);
			break;
		}
		case CK_FR:
		{
			pIndex	= (false == bExcluded)?&(pCond->idxReqFR):&(pCond->idxExcFR);
			break;
		}
		case CK_GR:
		{
			pIndex	= (false == bExcluded)?&(pCond->idxReqGR):&(pCond->idxExcGR);
			break;
		}
		case CK_GB:
		{
			pIndex	= (false == bExcluded)?&(pCond->idxReqGB):&(pCond->idxExcGB);
			break;
		}
		case CK_SS:
		{
			pIndex	= (false == bExcluded)?&(pCond->idxReqSS):&(pCond->idxExcSS);
			break;
		}
		default:
			assert(0);
	}

	*pIndex	= *pIndex + 1;
}

bool TaskDesc::GetIdxOfTaskCond(enCondKey key,bool bExcluded,uint8_t& index)
{
	assert(NULL != mpTaskDesc);

	stTaskCond*	pCond	= &(mpTaskDesc->taskCond);
	bool	bResult		= true;

	switch(key)
	{
		case CK_SN:
		{
			index	= (false == bExcluded)?pCond->idxReqSN:pCond->idxExcSN;
			bResult = ((false == bExcluded)?TASK_SN_REQ_SIZE:TASK_SN_EXC_SIZE) >= index ;
			break;
		}
		case CK_MN:
		{
			index	= (false == bExcluded)?pCond->idxReqMN:pCond->idxExcMN;
			bResult = ((false == bExcluded)?TASK_MN_REQ_SIZE:TASK_MN_EXC_SIZE) >= index ;
			break;
		}
		case CK_FR:
		{
			index	= (false == bExcluded)?pCond->idxReqFR:pCond->idxExcFR;
			bResult = ((false == bExcluded)?TASK_FR_REQ_SIZE:TASK_FR_EXC_SIZE) >= index ;
			break;
		}
		case CK_GR:
		{
			index	= (false == bExcluded)?pCond->idxReqGR:pCond->idxExcGR;
			bResult = ((false == bExcluded)?TASK_GR_REQ_SIZE:TASK_GR_EXC_SIZE) >= index ;
			break;
		}
		case CK_GB:
		{
			index	= (false == bExcluded)?pCond->idxReqGB:pCond->idxExcGB;
			bResult = ((false == bExcluded)?TASK_GB_REQ_SIZE:TASK_GB_EXC_SIZE) >= index ;
			break;
		}
		case CK_SS:
		{
			index	= (false == bExcluded)?pCond->idxReqSS:pCond->idxExcSS;
			bResult = ((false == bExcluded)?TASK_SS_REQ_SIZE:TASK_SS_EXC_SIZE) >= index ;
			break;
		}
		default:
			return false;
	}

	return bResult;
}

bool TaskDesc::GetCondOfTaskCond(enCondKey key,bool bExcluded,uint8_t index,char*& cond)
{
	assert(NULL != mpTaskDesc);

	stTaskCond*	pCond	= &(mpTaskDesc->taskCond);

	switch(key)
	{
		case CK_SN:
		{
			cond	= (false == bExcluded)?(pCond->reqSN[index]):(pCond->excSN[index]);
			break;
		}
		case CK_MN:
		{
			cond	= (false == bExcluded)?(pCond->reqMN[index]):(pCond->excMN[index]);
			break;
		}
		case CK_FR:
		{
			cond	= (false == bExcluded)?(pCond->reqFR[index]):(pCond->excFR[index]);
			break;
		}
		case CK_GR:
		{
			cond	= (false == bExcluded)?(pCond->reqGR[index]):(pCond->excGR[index]);
			break;
		}
		case CK_GB:
		case CK_SS:
		default:
			return false;
	}

	return true;
}

bool TaskDesc::GetCondOfTaskCond(enCondKey key,bool bExcluded,uint8_t index,uint32_t*& cond)
{
	assert(NULL != mpTaskDesc);

	stTaskCond*	pCond	= &(mpTaskDesc->taskCond);

	switch(key)
	{
	case CK_GB:
	{
		cond	= (false == bExcluded)?(&(pCond->reqGB[index])):(&(pCond->excGB[index]));
		break;
	}
	case CK_SS:
	{
		cond	= (false == bExcluded)?(&(pCond->reqSS[index])):(&(pCond->excSS[index]));
		break;
	}
	case CK_SN:
	case CK_MN:
	case CK_FR:
	case CK_GR:
	default:
		return false;
	}

	return true;
}

bool TaskDesc::HasValOfTaskCond(enCondKey key,bool bExcluded)
{
	uint8_t index = 0xFF;
	return GetIdxOfTaskCond(key,bExcluded,index) && 0 < index && 0xFF != index;
}

bool TaskDesc::FindValOfTaskCond(enCondKey key,bool bExcluded,const char* cond)
{
	uint8_t index = 0xFF;
	assert(GetIdxOfTaskCond(key,bExcluded,index));

	for(uint8_t tmp=0;tmp <= index; tmp++)
	{
		char*	condVal	= NULL;
		assert(GetCondOfTaskCond(key,bExcluded,tmp,condVal));
		if(0 == strncmp(condVal,cond,strlen(cond)))
		{
			return true;
		}
	}

	return false;
}

bool TaskDesc::FindValOfTaskCond(enCondKey key,bool bExcluded,uint32_t cond)
{
	uint8_t index = 0xFF;
	assert(GetIdxOfTaskCond(key,bExcluded,index));

	for(uint8_t tmp=0;tmp <= index; tmp++)
	{
		uint32_t *condVal	= NULL;
		assert(GetCondOfTaskCond(key,bExcluded,tmp,condVal));
		if(*condVal == cond)
		{
			return true;
		}
	}

	return false;
}

bool TaskDesc::AddCondition(enCondKey key,const char* val,bool bExcluded)
{
	if(false == IsCondStrType(key))
	{
		LOGERROR("Condition Type Error!");
		return false;
	}

	uint8_t index	= 0xFF;
	if(false == GetIdxOfTaskCond(key,bExcluded,index))
	{
		return false;
	}

	char*	cond = NULL;
	if(false == GetCondOfTaskCond(key,bExcluded,index,cond))
	{
		LOGERROR("Fail to call GetCondOfTaskCond()");
		return false;
	}

	uint16_t condStrLen = GetCondItemLength(key);
	strncpy(cond,val,condStrLen);
	AdvanceTaskCondIndex(key,bExcluded);

	return true;
}

bool TaskDesc::AddCondition(enCondKey key,uint32_t val,bool bExcluded)
{
	if(true == IsCondStrType(key))
	{
		LOGERROR("Condition Type Error!");
		return false;
	}

	uint8_t index	= 0xFF;
	if(false == GetIdxOfTaskCond(key,bExcluded,index))
	{
		return false;
	}

	uint32_t*	cond = NULL;
	if(false == GetCondOfTaskCond(key,bExcluded,index,cond))
	{
		LOGERROR("Fail to call GetCondOfTaskCond()");
		return false;
	}

	*cond	= val;
	AdvanceTaskCondIndex(key,bExcluded);
	return true;
}

bool TaskDesc::IsMatchedDut(DutInfo* dut)
{
	bool	bExcludedTbl[]	= {true,false};
	bool	bResult			= true;

	for(int idx=0;idx<=1;idx++)
	{
		bool bExcluded		= bExcludedTbl[idx];

		if(true == bResult && true == HasValOfTaskCond(CK_SN,bExcluded))
		{
			bool bFound	= FindValOfTaskCond(CK_SN,bExcluded,dut->GetDutSN());
			bResult	&= (bExcluded ^ bFound);
		}

		if(true == bResult && true == HasValOfTaskCond(CK_MN,bExcluded))
		{
			bool bFound	= FindValOfTaskCond(CK_MN,bExcluded,dut->GetDutMN());
			bResult	&= (bExcluded ^ bFound);
		}

		if(true == bResult && true == HasValOfTaskCond(CK_FR,bExcluded))
		{
			bool bFound	= FindValOfTaskCond(CK_FR,bExcluded,dut->GetDutFWRev());
			bResult	&= (bExcluded ^ bFound);
		}

		if(true == bResult && true == HasValOfTaskCond(CK_GR,bExcluded))
		{
			bool bFound	= FindValOfTaskCond(CK_GR,bExcluded,dut->GetDutGitRev());
			bResult	&= (bExcluded ^ bFound);
		}

		if(true == bResult && true == HasValOfTaskCond(CK_GB,bExcluded))
		{
			bool bFound	= FindValOfTaskCond(CK_GB,bExcluded,dut->GetDutCapGB());
			bResult	&= (bExcluded ^ bFound);
		}

		if(true == bResult && true == HasValOfTaskCond(CK_SS,bExcluded))
		{
			bool bFound	= FindValOfTaskCond(CK_SS,bExcluded,dut->GetDutSectorSize());
			bResult	&= (bExcluded ^ bFound);
		}
	}

	return bResult;
}

void DumpTaskCond(stTaskCond * cond)
{
	printf("idxReqSN        = %d\n",cond->idxReqSN);
	printf("ReqSN:");
	for(int i=0;i<cond->idxReqSN;i++)
	{
		printf("  %s",cond->reqSN[i]);
	}
	printf("\n");

	printf("idxExcSN        = %d\n",cond->idxExcSN);
	printf("excSN:");
	for(int i=0;i<cond->idxExcSN;i++)
	{
		printf("  %s",cond->excSN[i]);
	}
	printf("\n");

	printf("idxReqMN        = %d\n",cond->idxReqMN);
	printf("ReqMN:");
	for(int i=0;i<cond->idxReqMN;i++)
	{
		printf("  %s",cond->reqMN[i]);
	}
	printf("\n");

	printf("idxExcMN        = %d\n",cond->idxExcMN);
	printf("excMN:");
	for(int i=0;i<cond->idxExcMN;i++)
	{
		printf("  %s",cond->excMN[i]);
	}
	printf("\n");

	printf("idxReqFR        = %d\n",cond->idxReqFR);
	printf("ReqFR:");
	for(int i=0;i<cond->idxReqFR;i++)
	{
		printf("  %s",cond->reqFR[i]);
	}
	printf("\n");

	printf("idxExcFR        = %d\n",cond->idxExcFR);
	printf("excFR:");
	for(int i=0;i<cond->idxExcFR;i++)
	{
		printf("  %s",cond->excFR[i]);
	}
	printf("\n");

	printf("idxReqGR        = %d\n",cond->idxReqGR);
	printf("ReqGR:");
	for(int i=0;i<cond->idxReqGR;i++)
	{
		printf("  %s",cond->reqGR[i]);
	}
	printf("\n");

	printf("idxExcGR        = %d\n",cond->idxExcGR);
	printf("excGR:");
	for(int i=0;i<cond->idxExcGR;i++)
	{
		printf("  %s",cond->excGR[i]);
	}
	printf("\n");

	printf("idxReqGB        = %d\n",cond->idxReqGB);
	printf("ReqGB:");
	for(int i=0;i<cond->idxReqGB;i++)
	{
		printf("  %08d",cond->reqGB[i]);
	}
	printf("\n");

	printf("idxExcGB        = %d\n",cond->idxExcGB);
	printf("excGB:");
	for(int i=0;i<cond->idxExcGB;i++)
	{
		printf("  %08d",cond->excGB[i]);
	}
	printf("\n");

	printf("idxReqSS        = %d\n",cond->idxReqSS);
	printf("ReqSS:");
	for(int i=0;i<cond->idxReqSS;i++)
	{
		printf("  %08d",cond->reqSS[i]);
	}
	printf("\n");

	printf("idxExcSS        = %d\n",cond->idxExcSS);
	printf("excSS:");
	for(int i=0;i<cond->idxExcSS;i++)
	{
		printf("  %08d",cond->excSS[i]);
	}
	printf("\n");
}

void TaskDesc::Dump(void)
{
	printf("index           = %d\n",mpTaskDesc->index);
	printf("name            = %s\n",mpTaskDesc->name);
	printf("cmdline         = %s\n",mpTaskDesc->cmdline);
	printf("execTimeout     = %d\n",mpTaskDesc->execTimeout);
	printf("loopCount       = %d\n",mpTaskDesc->loopCount);
	DumpTaskCond(&(mpTaskDesc->taskCond));
}
