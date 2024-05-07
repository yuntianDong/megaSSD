#ifndef _SCSI_COMMAND_H
#define _SCSI_COMMAND_H

#include <stdio.h>
#include <stdint.h>
#include <scsi/sg.h>

typedef int (*PFPRINTFN)(const char *,...);

typedef enum
{
	SGIO_DATA_NONE	= SG_DXFER_NONE,
	SGIO_DATA_IN	= SG_DXFER_FROM_DEV,
	SGIO_DATA_OUT	= SG_DXFER_TO_DEV,
}enCmdDirect;

typedef enum
{
	CDB_06B,
	CDB_10B,
	CDB_12B,
	CDB_16B,
	CDB_FENCE,
}enCDBType;

typedef enum{
	CDB_FIELD_OPCODE,
	CDB_FILED_MISC	,
	CDB_FILED_LBA	,
	CDB_FILED_LENGTH,
	CDB_FILED_CONTROL	,
	CDB_FILED_SERVICE	,
	CDB_FILED_ADDITIONAL,
	CDB_FILED_MISC2		,
}enCDBFieldIdx;

typedef struct
{
	enCDBFieldIdx		index;
	enCDBType			cdbType;
	uint8_t				bitsOffset;
	uint8_t				bitslength;
}stCDBStruct;

#define CDB_SIZE(a)		( (a) == CDB_06B)?6:( ( (a) == CDB_10B)?10:( ( (a)==CDB_12B)?12:( ( (a)==CDB_16B)?16:0 ) ) )

//		index				cdbType		bitsOffset	bitslength
// 	Note1: For CDB_06B, LBA[32:37] = CDB_FILED_SERVICE
#define CDB_FORMAT_TABLE							\
	ZZ(CDB_FIELD_OPCODE,	CDB_FENCE,		0,		8)		\
	ZZ(CDB_FILED_MISC,		CDB_FENCE,		13,		3)		\
	ZZ(CDB_FILED_SERVICE,	CDB_FENCE,		8,		5)		\
	ZZ(CDB_FILED_LBA,		CDB_06B,		16,		16)		\
	ZZ(CDB_FILED_LBA,		CDB_10B,		16,		32)		\
	ZZ(CDB_FILED_LBA,		CDB_12B,		16,		32)		\
	ZZ(CDB_FILED_LBA,		CDB_16B,		16,		32)		\
	ZZ(CDB_FILED_LENGTH,	CDB_06B,		32,		8)		\
	ZZ(CDB_FILED_LENGTH,	CDB_10B,		56,		16)		\
	ZZ(CDB_FILED_LENGTH,	CDB_12B,		48,		32)		\
	ZZ(CDB_FILED_LENGTH,	CDB_16B,		80,		32)		\
	ZZ(CDB_FILED_MISC2,		CDB_10B,		48,		8)		\
	ZZ(CDB_FILED_MISC2,		CDB_12B,		80,		8)		\
	ZZ(CDB_FILED_MISC2,		CDB_16B,		112,	8)		\
	ZZ(CDB_FILED_ADDITIONAL,CDB_16B,		48,		32)		\
	ZZ(CDB_FILED_CONTROL,	CDB_06B,		40,		8)		\
	ZZ(CDB_FILED_CONTROL,	CDB_10B,		72,		8)		\
	ZZ(CDB_FILED_CONTROL,	CDB_12B,		88,		8)		\
	ZZ(CDB_FILED_CONTROL,	CDB_16B,		120,	8)		\

class ScsiCommand
{
private:


protected:
	void SetCDBRegVal(uint8_t offset,uint8_t val);
	uint8_t GetCDBRegVal(uint8_t offset);

	uint8_t mOPCode;
	uint8_t * mpCDB;
	enCDBType mCDBType;
	enCmdDirect mCmdDir;

	static stCDBStruct mCDBFormatTbl[];
public:
	ScsiCommand(uint8_t opCode,enCmdDirect cmdDir,enCDBType cdbType=CDB_16B);
	virtual ~ScsiCommand();

	bool GetRegPosition(enCDBFieldIdx reg,uint8_t &bitOffset,uint8_t &bitLength);
	void SetRegVal(enCDBFieldIdx reg,uint32_t val);
	uint32_t GetRegVal(enCDBFieldIdx reg);

	inline uint8_t* GetCDB(void){return mpCDB;};
	inline enCDBType GetCDBType(void){return mCDBType;};
	inline uint8_t GetCDBSize(void){return CDB_SIZE(mCDBType);};
	inline enCmdDirect GetCmdDir(void){return mCmdDir;};

	void SetOpCode(uint8_t opCode) {SetRegVal(CDB_FIELD_OPCODE,(uint32_t)opCode);};
	uint8_t GetOpCode(void) {return (uint8_t)(GetRegVal(CDB_FIELD_OPCODE));};
	void SetMiscInfo(uint8_t miscInfo) {SetRegVal(CDB_FILED_MISC,(uint32_t)miscInfo);};
	uint8_t GetMiscInfo(void) {return (uint8_t)(GetRegVal(CDB_FILED_MISC));};
	void SetMisc2Info(uint8_t miscInfo) {SetRegVal(CDB_FILED_MISC2,(uint32_t)miscInfo);};
	uint8_t GetMisc2Info(void) {return (uint8_t)(GetRegVal(CDB_FILED_MISC2));};
	void SetServiceAction(uint8_t servAct) {SetRegVal(CDB_FILED_SERVICE,(uint32_t)servAct);};
	uint8_t GetServiceAction(void) {return (uint8_t)GetRegVal(CDB_FILED_SERVICE);};
	void SetAddress(uint32_t lba) {SetRegVal(CDB_FILED_LBA,(uint32_t)lba);};
	uint32_t GetAddress(void) {return (uint32_t)(GetRegVal(CDB_FILED_LBA));};
	void SetLength(uint32_t length) {SetRegVal(CDB_FILED_LENGTH,(uint32_t)length);};
	uint32_t GetLength(void) {return (uint32_t)(GetRegVal(CDB_FILED_LENGTH));};
	void SetAdditional(uint32_t addt) {SetRegVal(CDB_FILED_ADDITIONAL,(uint32_t)addt);};
	uint32_t GetAdditional(void) {return (uint32_t)(GetRegVal(CDB_FILED_ADDITIONAL));};
	void SetControl(uint8_t ctrl) {SetRegVal(CDB_FILED_CONTROL,(uint32_t)ctrl);};
	uint8_t GetControl(void) {return (uint8_t)(GetRegVal(CDB_FILED_CONTROL));};

	void DumpCDBReg(PFPRINTFN fn=NULL);
};

#endif
