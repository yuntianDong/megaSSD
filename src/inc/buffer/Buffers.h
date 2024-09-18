#ifndef _BUFFERS_H
#define _BUFFERS_H

#include <stdio.h>
#include <stdint.h>
#include <iostream>

#include "Commons.h"

#define UNUSED_PARAM(x) (void)x

#define DEF_BUF_ALIGNMENT			4096
#define BUF_CMP_NOERROR				(uint32_t)-1

typedef enum
{
	BUF_VLEN_1B,		// Byte		, 8
	BUF_VLEN_2B,		// Word		,16
	BUF_VLEN_4B,		// DWord	,32
	BUF_VLEN_8B,		// QWord	,64
}enBufValLen;

#define	NBYTE_PER_BYTE			1
#define NBYTE_PER_WORD			2
#define NBYTE_PER_DWORD			4
#define NBYTE_PER_QWORD			8
#define NBITS_PER_BYTE          8

typedef enum _enEndianMode
{
	BUFV_ENDIAN_UNK,
	BUFV_BIG_ENDIAN,
	BUFV_LITTLE_ENDIAN,
}enEndianMode;

typedef int (*PFPRINTFN)(const char *,...);

class Buffers
{
private:
	uint8_t* mBuf;

	uint64_t mBufSize;
	uint16_t mBufAligned;

	enEndianMode	mEndianMode;
	bool			mbNeedRelease;
	const char* mdriver;

	void AllocatBuf(uint64_t size,uint16_t align);

protected:
	void InitialBuf(uint8_t val);
	void ReleaseBuf(void);

	void UpdateBufSize(uint64_t val) {mBufSize = val;};

	uint64_t ReadnBytes(uint64_t offset,uint8_t nBytes);
	void WritenBytes(uint64_t offset,uint8_t nBytes,uint64_t val);

	bool GetBufVal(uint64_t offset,uint8_t& val);
	bool GetBufVal(uint64_t offset,uint16_t& val, enEndianMode emode=BUFV_ENDIAN_UNK);
	bool GetBufVal(uint64_t offset,uint32_t& val, enEndianMode emode=BUFV_ENDIAN_UNK);
	bool GetBufVal(uint64_t offset,uint64_t& val, enEndianMode emode=BUFV_ENDIAN_UNK);

	bool SetBufVal(uint64_t offset,uint8_t val);
	bool SetBufVal(uint64_t offset,uint16_t val,enEndianMode emode=BUFV_ENDIAN_UNK);
	bool SetBufVal(uint64_t offset,uint32_t val,enEndianMode emode=BUFV_ENDIAN_UNK);
	bool SetBufVal(uint64_t offset,uint64_t val,enEndianMode emode=BUFV_ENDIAN_UNK);

	bool FillBufVal(uint64_t offset,uint8_t pattern,uint32_t length);
	bool FillBufVal(uint64_t offset,uint16_t pattern,uint32_t length);
	bool FillBufVal(uint64_t offset,uint32_t pattern,uint32_t length);
	bool FillBufVal(uint64_t offset,uint64_t pattern,uint32_t length);

public:
	Buffers(uint64_t size,uint16_t align,const char* mode);
	Buffers(uint8_t * buffer,uint64_t size,uint16_t align);
	virtual ~Buffers(void);

	friend uint32_t _GetDWValByOffset(Buffers& buf,uint64_t offset);

	void DumpBuffer(uint64_t offset,uint32_t length,PFPRINTFN fn=NULL);

	virtual inline uint64_t GetBufSize(void) {return this->mBufSize;};
	inline uint16_t GetBufAlignment(void) {return this->mBufAligned;};

	inline uint64_t GetBufferAddr(void) {return (uint64_t)(this->mBuf);};
	inline uint8_t* GetBufferPoint(void) {return this->mBuf;};

	inline void SetEndianMode(enEndianMode eMode) {this->mEndianMode = eMode;};
	inline enEndianMode GetEndianMode(void) {return this->mEndianMode;};

	uint8_t& operator[](uint64_t offset) {return *(this->mBuf + offset);};
	const uint8_t& operator[](uint64_t offset)const {return *(this->mBuf + offset);};

	bool operator==(Buffers &other) {return this->GetBufferAddr() == other.GetBufferAddr();};

	virtual uint8_t 	GetByte(uint64_t offset);
	virtual uint16_t	GetWord(uint64_t offset);
	virtual uint32_t	GetDWord(uint64_t offset);
	virtual uint64_t	GetQWord(uint64_t offset);

	virtual void SetByte(uint64_t offset,uint8_t val) {SetBufVal(offset,val);};
	virtual void SetWord(uint64_t offset,uint16_t val) {SetBufVal(offset,val);};
	virtual void SetDWord(uint64_t offset,uint32_t val) {SetBufVal(offset,val);};
	virtual void SetQWord(uint64_t offset,uint64_t val) {SetBufVal(offset,val);};

	virtual void FillWithBytePat(uint64_t offset,uint32_t length,uint8_t pattern) {FillBufVal(offset,pattern,length);};
	virtual void FillWithWordPat(uint64_t offset,uint32_t length,uint16_t pattern) {FillBufVal(offset,pattern,length);};
	virtual void FillWithDWordPat(uint64_t offset,uint32_t length,uint32_t pattern) {FillBufVal(offset,pattern,length);};
	virtual void FillWithQWordPat(uint64_t offset,uint32_t length,uint64_t pattern) {FillBufVal(offset,pattern,length);};
	virtual void FillZero(uint64_t offset,uint32_t length) {FillBufVal(offset,(uint8_t)0x0,length);};
	virtual void FillHexF(uint64_t offset,uint32_t length) {FillBufVal(offset,(uint8_t)0xFF,length);};

	inline void Dump(uint64_t offset,uint32_t length) {DumpBuffer(offset,length);};

	virtual void MarkAddrOverlay(uint64_t sOffset,uint16_t numSector,uint64_t logicAddr);
	virtual void FillSector(uint64_t sOffset,uint16_t numSector,uint32_t pattern);
	virtual uint32_t GetSectorSize(void){return (uint32_t)-1;};

	bool IsSectorBuf(void){return (uint32_t)-1 != this->GetSectorSize();};

	/* bits operation */
	bool ReadRegVal(uint64_t startByte,uint8_t startBits,uint8_t bitsLen,uint64_t& value);
	bool WriteRegVal(uint64_t startByte,uint8_t startBits,uint8_t bitsLen,uint64_t value);

	/* buffer operation */
	bool CopyFromBuff(Buffers& fromBuff, uint64_t startByte, uint64_t fromByte, uint32_t length);
	bool CopyToBuff(Buffers& toBuff, uint64_t startByte, uint64_t fromByte, uint32_t length);
	bool Compare(Buffers& desBuff, uint64_t desOffset, uint64_t offset, uint32_t length,\
			uint32_t &rtnErrPos, uint32_t &rtnSrcVal, uint32_t &rtnDesVal);

	/* file operation */
	bool ToFile(const char* fileName, uint64_t offset, uint32_t length);
	bool FromFile(const char* fileName, uint64_t offset, uint32_t length);

	/* Calculate CRC */
	uint8_t  GenCRC8(uint64_t offset,uint32_t length);
	uint16_t GenCRC16(uint64_t offset,uint32_t length);
	uint32_t GenCRC32(uint64_t offset,uint32_t length);
};

#endif
