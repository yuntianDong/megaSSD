/*
 * RegisterCfg.h
 *
 */

#ifndef _PROTOCAL_REGISTERCFG_H_
#define _PROTOCAL_REGISTERCFG_H_

#include <stdint.h>

#define REG_LEN_UNDEF		(uint16_t)-1
#define REG_NAME_STRMAX		80

typedef enum _enOffType
{
	REG_OFF_F2B,
	REG_OFF_B2F,
	REG_OFF_APP,
	REG_OFF_REG,
	REG_OFF_UNK,
}enOffType;

typedef enum _enLenType
{
	REG_LEN_VAL,
	REG_LEN_REG,
	REG_LEN_UNK,
}enLenType;

typedef union _stRegOffset
{
	uint16_t	offVal;
	char 	offReg[REG_NAME_STRMAX];
}stRegOffset;

typedef union _stRegLength
{
	uint16_t	lenVal;
	char 	lenReg[REG_NAME_STRMAX];
}stRegLength;

typedef struct _stRegDesc
{
	stRegOffset	bytOff;
	stRegOffset bitOff;
	stRegLength nBits;
	enOffType	offType;
	enLenType	lenType;
	char	name[REG_NAME_STRMAX];
}stRegDesc;

#endif /* _PROTOCAL_REGISTERCFG_H_ */
