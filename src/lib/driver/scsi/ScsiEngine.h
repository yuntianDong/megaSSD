#ifndef _SCSI_ENGINE_H
#define _SCSI_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <scsi/sg.h>

#include "ScsiCommand.h"
#include "ScsiResponse.h"

#define SENSE_DATA_LEN					252
#define MAX_CDB_SIZE					16

typedef enum{
	SCSI_IOCTL_SGIO,
}enIOCTLMode;

typedef struct sg_io_hdr	stSGIOHdlr;

class ScsiEngine
{
private:
	int mDevHdlr;
	enIOCTLMode	mIOCTLMode;

	stSGIOHdlr	msSGIOHdlr;
protected:
	uint8_t		mSenseData[SENSE_DATA_LEN];
	uint8_t		mCmdCDB[MAX_CDB_SIZE];

	inline bool isSGIOMode(){return mIOCTLMode == SCSI_IOCTL_SGIO;};
	inline int GetDevHdlr(){return mDevHdlr;};
	inline stSGIOHdlr* GetSGIOHdlr(){return &msSGIOHdlr;};

	void InitSGIOHdlr(void);
	int IssueScsiCmdbySGIO(uint8_t* pCmd,uint8_t cmdSize,uint8_t* buffer,uint32_t size,enCmdDirect xferDirect,uint32_t timeout);

public:
	ScsiEngine(const char* devName,enIOCTLMode mode);

	void DumpLastCmdCDB(void);
	void DumpLastCmdSense(void);

	ScsiResponse* GetLastCmdResp(void);
	void DumpLastIOCtlParam(void);

	ScsiResponse* IssueCmdWithNonData(ScsiCommand & cmd,uint32_t timeout=8000);
	ScsiResponse* IssueCmdWithDataIn(ScsiCommand & cmd,uint8_t* buffer,uint32_t size,uint32_t timeout=8000);
	ScsiResponse* IssueCmdWithDataOut(ScsiCommand & cmd,uint8_t* buffer,uint32_t size,uint32_t timeout=8000);
};

#endif
