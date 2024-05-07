#include <stdio.h>
#include <stdlib.h>

#include "ScsiResponse.h"
#include "Logging.h"

#define ILLEGAL_REQUEST		0x5

#define ZZ(a,b,c,d,e,f)		{a,b,c,d,e,f},
stSenseDataFmt mFixedSenseFormat[]	=
{
	SCSI_FIXED_SENSE_FORMAT
};

stSenseDataFmt mDescSenseFormat[]	=
{
	SCSI_DESC_SENSE_FORMAT
};
#undef ZZ

ScsiResponse::ScsiResponse(void)
	:muDuration(0),muAuxInfo(0),muDXferLen(0),mpDataBuf(NULL)
{
	mpSenseResp = (stSenseResp*)malloc(sizeof(stSenseResp));
	memset(mpSenseResp,0,sizeof(stSenseResp));
	memset(&msCmdExResp,0,sizeof(stCmdExRespInfo));

	msCmdExResp.scsi_status = SCSI_STATUS_UNDEF;
}

ScsiResponse::~ScsiResponse()
{
	//LOGINFO("Response Type : %d",(int)GetRespType());
	if(RESP_DESC == GetRespType())
	{
		for(int idx=0;idx<SENSE_MAX_DESC_NUMBER;idx++)
		{
			stDescData * descData	= mpSenseResp->s.desSense.sDescLst[idx];

			if(NULL != descData)
			{
				if(NULL != descData->spData)
				{
					free(descData->spData);
					descData->spData = NULL;
				}

				free(descData);
				descData = NULL;
			}
		}
	}else if(RESP_FIXED == GetRespType())
	{
		uint8_t * pAddData	= mpSenseResp->s.fixSense.pAddDatum;
		uint8_t addLen		= mpSenseResp->s.fixSense.sAddLen;
		if((addLen > SENSE_FIXED_HEADER_LEN) && (NULL != pAddData))
		{
			free(pAddData);
			pAddData	= NULL;
		}
	}

	if(NULL != mpSenseResp)
	{
		free(mpSenseResp);
		mpSenseResp = NULL;
	}
}

void ScsiResponse::ParseFixedSense(uint8_t* sense,uint8_t slen)
{
	if(RESP_FIXED != GetRespType())
	{
		LOGERROR("Response Code is not correct");
		return;
	}

	if(SENSE_FIXED_LEN_MIN > slen)
	{
		LOGERROR("The length of Sense Data is too shorter");
		return;
	}

	stSenseRecFix * s = &(mpSenseResp->s.fixSense);
	memcpy((void *)s,(void *)sense,SENSE_FIXED_LEN_MIN);

	uint8_t leftBytes	= s->sAddLen-SENSE_FIXED_HEADER_LEN;
	if(slen < (leftBytes + SENSE_FIXED_HEADER_LEN))
		leftBytes = slen - SENSE_FIXED_HEADER_LEN;

	if(0 < leftBytes)
	{
		s->pAddDatum = (uint8_t *)malloc(leftBytes);
		memcpy(s->pAddDatum,&sense[18],leftBytes);
	}
}

void ScsiResponse::ParseDescSense(uint8_t* sense,uint8_t slen)
{
	if(RESP_FIXED != GetRespType())
	{
		LOGERROR("Response Code is not correct");
		return;
	}

	if(SENSE_DESC_LEN_MIN > slen)
	{
		LOGERROR("The length of Sense Data is too shorter");
		return;
	}

	stSenseResDesc* s = &(mpSenseResp->s.desSense);
	memcpy((void *)s,(void *)sense,SENSE_DESC_LEN_MIN);

	uint8_t addLen		= s->sDescLength;
	uint8_t *addBytes	= &(sense[SENSE_DESC_LEN_MIN]);
	addLen = ((addLen+SENSE_DESC_LEN_MIN) > slen)?slen:addLen;

	while(0 < addLen)
	{
		stDescData* pDesc	= (stDescData*)malloc(sizeof(stDescData));
		pDesc->sType		= *(addBytes++);
		pDesc->sLength		= *(addBytes++);
		pDesc->spData		= (uint8_t *)malloc(pDesc->sLength);
		memcpy(pDesc->spData,addBytes,pDesc->sLength);

		addLen -= pDesc->sLength;
	}
}

void ScsiResponse::ParseSense(uint8_t* sense,uint8_t slen)
{
	if(NULL == sense)
	{
		LOGERROR("Invalid Sense Buffer!!");
		return;
	}

	uint8_t byte0 = sense[0];
	mpSenseResp->respCode	= (byte0 & SENSE_RESP_CODE_BITMASK);

	//LOGINFO("Response Code : 0x%x",mpSenseResp->respCode);
	//LOGINFO("Response Type : %d",(int)GetRespType());

	if(RESP_FIXED == GetRespType())
		ParseFixedSense(sense,slen);
	else if(RESP_DESC == GetRespType())
		ParseDescSense(sense,slen);
	else
		LOGERROR("Invalid Response Type, respCode: 0x%x",mpSenseResp->respCode);
}

uint8_t ScsiResponse::GetSenseKey(void)
{
	if(RESP_FIXED == GetRespType())
	{
		return mpSenseResp->s.fixSense.b2.key;
	}
	else if(RESP_DESC == GetRespType())
	{
		return mpSenseResp->s.desSense.b1.key;
	}

	return (uint8_t)(-1);
}

bool ScsiResponse::IsFileMarkSet(void)
{
	if(RESP_FIXED == GetRespType())
	{
		return mpSenseResp->s.fixSense.b2.fileMark;
	}

	return false;
}

bool ScsiResponse::IsEOMFlagSet(void)
{
	if(RESP_FIXED == GetRespType())
	{
		return mpSenseResp->s.fixSense.b2.eom;
	}

	return false;
}

bool ScsiResponse::IsILIFlagSet(void)
{
	if(RESP_FIXED == GetRespType())
	{
		return mpSenseResp->s.fixSense.b2.ili;
	}

	return false;
}

uint32_t ScsiResponse::GetSenseInfo(void)
{
	if(RESP_FIXED == GetRespType())
	{
		uint8_t* pVal 	= mpSenseResp->s.fixSense.sInfo;
		uint32_t tmpVal = 0;

		tmpVal	+= (pVal[0]) << 24;
		tmpVal	+= (pVal[1]) << 16;
		tmpVal	+= (pVal[2]) << 8;
		tmpVal	+= (pVal[3]) << 0;

		return tmpVal;
	}

	return (uint32_t)-1;
}

uint32_t ScsiResponse::GetCmdSpecInfo(void)
{
	if(RESP_FIXED == GetRespType())
	{
		uint8_t* pVal 	= mpSenseResp->s.fixSense.sCmdSpec;
		uint32_t tmpVal = 0;

		tmpVal	+= (pVal[0]) << 24;
		tmpVal	+= (pVal[1]) << 16;
		tmpVal	+= (pVal[2]) << 8;
		tmpVal	+= (pVal[3]) << 0;

		return tmpVal;
	}

	return (uint32_t)-1;
}

uint8_t ScsiResponse::GetAddCode(void)
{
	if(RESP_FIXED == GetRespType())
	{
		return mpSenseResp->s.fixSense.sASC;
	}
	else if(RESP_DESC == GetRespType())
	{
		return mpSenseResp->s.desSense.sASC;
	}

	return (uint8_t)-1;
}

uint8_t ScsiResponse::GetAddCodeQual(void)
{
	if(RESP_FIXED == GetRespType())
	{
		return mpSenseResp->s.fixSense.sASCQ;
	}
	else if(RESP_DESC == GetRespType())
	{
		return mpSenseResp->s.desSense.sASCQ;
	}

	return (uint8_t)-1;
}

uint8_t ScsiResponse::GetFRUC(void)
{
	if(RESP_FIXED == GetRespType())
	{
		return mpSenseResp->s.fixSense.sFRUC;
	}

	return (uint8_t)-1;
}

bool ScsiResponse::IsSKSVFlagSet(void)
{
	if(RESP_FIXED == GetRespType())
	{
		return mpSenseResp->s.fixSense.b15.sksv;
	}

	return false;
}

uint32_t ScsiResponse::GetSenseKeySpec(void)
{
	if(RESP_FIXED == GetRespType())
	{
		uint8_t* pVal 	= mpSenseResp->s.fixSense.sKeySpecEx;
		uint32_t tmpVal	= 0;

		tmpVal	+= mpSenseResp->s.fixSense.b15.keySpec << 16;
		tmpVal  += pVal[0] << 8;
		tmpVal	+= pVal[1];

		return tmpVal;
	}

	return (uint32_t)-1;
}

uint8_t* ScsiResponse::GetAddBytes(void)
{
	if(RESP_FIXED == GetRespType())
	{
		return mpSenseResp->s.fixSense.pAddDatum;
	}

	return NULL;
}
