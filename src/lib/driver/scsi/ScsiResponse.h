#ifndef _SCSI_RESPONSE_H
#define _SCSI_RESPONSE_H

#include <stdint.h>
#include <string.h>

/*
 *	SCSI Response Sense
 *	Reference: T10/1731-D SCSI Primary Commands - 4 (SPC-4)  4.5 Sense data
 */

#define SENSE_MAX_DESC_NUMBER		32

#define SENSE_RESP_CODE_BITMASK		0x7F
#define SENSE_FIXED_LEN_MIN			18
#define SENSE_FIXED_HEADER_LEN		10
#define SENSE_DESC_LEN_MIN			8
#define SENSE_DESC_HEADER_LEN		8

/*
 *	Scsi Sense Response Code
 */
#define SENSE_CODE_FIX_INFO					0x70
#define SENSE_CODE_FIX_ERROR				0x71
#define SENSE_CODE_DESC_INFO				0x72
#define SENSE_CODE_DESC_ERROR				0x73

/*
 *  Scsi Status Code
 */
#define SCSI_STATUS_GOOD					0x0
#define SCSI_STATUS_UNDEF					0xFF

typedef enum{
	RESP_FIXED,
	RESP_DESC,
	RESP_FENSE,
}enRespType;

#pragma pack(1)
typedef struct
{
	uint8_t		sType;			// Descriptor Type
	uint8_t		sLength;		// Descriptor Length
	uint8_t*	spData;			// Descriptor Data
}stDescData;

typedef struct
{
	struct{
		uint8_t resCode	: 7;	// Response Code
		uint8_t	valid	: 1;	// Valid
	}b0;
	struct{
		uint8_t key		: 4;	// Key Code
		uint8_t	resv0	: 4;	// Reserved0
	}b1;
	uint8_t		sASC;			// Additional sense code
	uint8_t		sASCQ;			// Additional sense code qualifier
	uint8_t		sResv1[3];		// Reserved
	uint8_t		sDescLength;	// Descriptors Length
	stDescData*	sDescLst[SENSE_MAX_DESC_NUMBER];		// Descriptors List
}stSenseResDesc;

typedef struct
{
	struct{
		uint8_t resCode	: 7;	// Response Code
		uint8_t	valid	: 1;	// Valid
	}b0;
	uint8_t		sResv0;			// Reserved 0
	struct{
		uint8_t key		: 4;
		uint8_t resv	: 1;
		uint8_t ili		: 1;
		uint8_t eom		: 1;
		uint8_t fileMark: 1;
	}b2;
	uint8_t 	sInfo[4];
	uint8_t  	sAddLen;
	uint8_t 	sCmdSpec[4];
	uint8_t		sASC;
	uint8_t		sASCQ;
	uint8_t		sFRUC;
	struct{
		uint8_t	keySpec	: 7;
		uint8_t sksv	: 1;
	}b15;
	uint8_t		sKeySpecEx[2];
	uint8_t *	pAddDatum;
}stSenseRecFix;

typedef struct{
	uint8_t respCode;
	union{
		stSenseResDesc 	desSense;
		stSenseRecFix	fixSense;
	}s;
}stSenseResp;
#pragma pack()

#define _VARLEN				0
/*
 * Scsi Sense Data Fixed Format, 18bytes
 * Reference T10/1731-D SCSI Primary Commands - 4 (SPC-4)  4.5.3 Fixed format sense data
 *		index			O/M		byteOffset	bitOffset	bitLength	defVal
 */
#define SCSI_FIXED_SENSE_FORMAT			\
	ZZ(FIXED_VALID,		false,		0,			7,			1,			0)	\
	ZZ(FIXED_RESPCODE,	true,		0,			0,			7,			0)	\
	ZZ(FIXED_OBSOLETE,	true,		1,			0,			8,			0)	\
	ZZ(FIXED_FILEMARK,	true,		2,			7,			1,			0)	\
	ZZ(FIXED_EOM,		true,		2,			6,			1,			0)	\
	ZZ(FIXED_ILI,		true,		2,			5,			1,			0)	\
	ZZ(FIXED_RESV0,		true,		2,			4,			1,			0)	\
	ZZ(FIXED_KEYCODE,	true,		2,			0,			4,			0)	\
	ZZ(FIXED_INFO,		false,		3,			0,			32,			0)	\
	ZZ(FIXED_ADDLEN,	true,		7,			0,			8,			0)	\
	ZZ(FIXED_CMDSPEC,	true,		8,			0,			32,			0)	\
	ZZ(FIXED_ASC,		true,		12,			0,			8,			0)	\
	ZZ(FIXED_ASCQ,		true,		13,			0,			8,			0)	\
	ZZ(FIXED_FRUC,		true,		14,			0,			8,			0)	\
	ZZ(FIXED_SKSV,		true,		15,			7,			1,			0)	\
	ZZ(FIXED_KEYSPEC0,	true,		15,			0,			7,			0)	\
	ZZ(FIXED_KEYSPEC1,	true,		16,			0,			16,			0)	\
	ZZ(FIXED_ADDDATA,	true,		18,			0,			_VARLEN,	0)	\

/*
 * Scsi Sense Data Fixed Format, 18bytes
 * Reference T10/1731-D SCSI Primary Commands - 4 (SPC-4)  4.5.3 Fixed format sense data
 *		index			O/M		byteOffset	bitOffset	bitLength	defVal
 */
#define SCSI_DESC_SENSE_FORMAT			\
	ZZ(DESC_RESV0,		true,		0,			7,			1,			0)	\
	ZZ(DESC_RESPCODE,	true,		0,			0,			7,			0)	\
	ZZ(DESC_RESV1,		true,		1,			4,			4,			0)	\
	ZZ(DESC_KEYCODE,	true,		1,			0,			4,			0)	\
	ZZ(DESC_ASC,		true,		2,			0,			8,			0)	\
	ZZ(DESC_ASCQ,		true,		3,			0,			8,			0)	\
	ZZ(DESC_RESV2,		true,		4,			0,			24,			0)	\
	ZZ(DESC_ADDLEN,		true,		7,			0,			8,			0)	\
	ZZ(DESC_DESCTYPE,	true,		0,			0,			8,			0)	\
	ZZ(DESC_DESCLEN,	true,		1,			0,			8,			0)	\
	ZZ(DESC_DESCDATA,	true,		2,			0,			_VARLEN,	0)	\

#define	ZZ(a,b,c,d,e,f)		a,
typedef enum _enFixSenseField
{
	SCSI_FIXED_SENSE_FORMAT
}enFixSenseField;
#undef ZZ

#define ZZ(a,b,c,d,e,f)		a,
typedef enum _enDescSenseField
{
	SCSI_DESC_SENSE_FORMAT
}enDescSenseField;
#undef ZZ

typedef struct _stSenseDataFmt
{
	int 			iField;
	bool 			bMandatory;
	uint8_t			uByteOffset;
	uint8_t			uBitOffset;
	uint8_t			uBitLength;
	uint32_t		uDefVal;
}stSenseDataFmt;

typedef struct _stCmdExRespInfo
{
	uint8_t		scsi_status;
	uint8_t		masked_status;
	uint8_t		msg_status;
	uint16_t	host_status;
	uint16_t	driver_status;
}stCmdExRespInfo;

//class ScsiResponse : public CmdResponse
class ScsiResponse
{
private:
	void ParseSense(uint8_t* sense,uint8_t slen);
	void ParseFixedSense(uint8_t* sense,uint8_t slen);
	void ParseDescSense(uint8_t* sense,uint8_t slen);

protected:
	stSenseResp*  		mpSenseResp;		// Command returned Sense Data
	stCmdExRespInfo		msCmdExResp;		// Command returned status
	uint32_t			muDuration;			// Time taken by cmd (unit: millisec)
	uint32_t			muAuxInfo;			// Auxiliary Information
	uint32_t			muDXferLen;			// Actual Transfered
	uint8_t*			mpDataBuf;			// Host Data Buffer

	static stSenseDataFmt mFixedSenseFormat[];
	static stSenseDataFmt mDescSenseFormat[];

public:
	ScsiResponse(void);
	virtual ~ScsiResponse();

	void SetSenseData(uint8_t* sense,uint8_t slen){ParseSense(sense,slen);};
	void SetDuration(uint32_t duration){muDuration=duration;};
	void SetAuxInfo(uint32_t auxInfo){muAuxInfo=auxInfo;};
	void SetDataBuf(uint8_t* pBuf,uint32_t dxferLen){mpDataBuf=pBuf;muDXferLen=dxferLen;};
	void SetCmdStatus(stCmdExRespInfo exRespInfo){memcpy(&msCmdExResp,&exRespInfo,sizeof(stCmdExRespInfo));};

	/*
	 * Other Functions
	 */
	uint32_t GetCmdDuration(void){return muDuration;};
	uint32_t GetAuxInfo(void){return muAuxInfo;};
	uint32_t GetDataXferLen(void){return muDXferLen;};
	uint8_t* GetDataBuffer(void){return mpDataBuf;};

	/*
	 * Cmd Status Functions
	 */
	uint8_t	GetScsiStatus(void){return msCmdExResp.scsi_status;};
	uint8_t GetMaskedStatus(void){return msCmdExResp.masked_status;};
	uint8_t GetMsgStatus(void){return msCmdExResp.msg_status;};
	uint16_t GetHostStatus(void){return msCmdExResp.host_status;};
	uint16_t GetDriverStatus(void){return msCmdExResp.driver_status;};

	bool IsErrorResp(void)
	{
		return (SCSI_STATUS_GOOD != GetScsiStatus()) ||
				(mpSenseResp->respCode == SENSE_CODE_FIX_ERROR) ||
				(mpSenseResp->respCode == SENSE_CODE_DESC_ERROR);
	}

	enRespType GetRespType(void)
	{
		if(mpSenseResp->respCode == 0 || mpSenseResp->respCode == SENSE_CODE_FIX_INFO || mpSenseResp->respCode == SENSE_CODE_FIX_ERROR)
			return RESP_FIXED;
		else if(mpSenseResp->respCode == SENSE_CODE_DESC_INFO || mpSenseResp->respCode == SENSE_CODE_DESC_ERROR)
			return RESP_DESC;
		else
			return RESP_FENSE;
	}

	/*
	 * Sense Data Functions
	 */
	uint8_t GetSenseKey(void);
	bool IsFileMarkSet(void);
	bool IsEOMFlagSet(void);
	bool IsILIFlagSet(void);
	uint32_t GetSenseInfo(void);
	uint32_t GetCmdSpecInfo(void);
	uint8_t GetAddCode(void);
	uint8_t GetAddCodeQual(void);
	uint8_t GetFRUC(void);
	bool IsSKSVFlagSet(void);
	uint32_t GetSenseKeySpec(void);
	uint8_t* GetAddBytes(void);
};

#endif
