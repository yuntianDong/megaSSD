/*
 * NVMeMIErrInjection.h
 *
 */

#ifndef _NVME_MI_NVMEMIERRINJECTION_H_
#define _NVME_MI_NVMEMIERRINJECTION_H_

#include <stdint.h>
#include <string.h>

typedef struct _stErrInjectDesc
{
	uint16_t	noPack;
	uint16_t	offset;
	uint32_t	bitMask;
	uint32_t	dwVal;
}stErrInjectDesc;

#define	MI_ERRINJ_DEBUG_PRINT_FORMAT	"ERR: [%d] O=%d M=%x V=%x"

#define	ERR_INJECT_MAX_COUNT	64

class NVMeMIErrInject
{
private:
	stErrInjectDesc		mErrInjectList[ERR_INJECT_MAX_COUNT];
	uint16_t			mCurIndex;

public:
	NVMeMIErrInject(void)
		:mCurIndex(0)
	{
		memset(mErrInjectList,0,sizeof(stErrInjectDesc)*ERR_INJECT_MAX_COUNT);
	};
	virtual ~NVMeMIErrInject(void){};

	void Debug(void)
	{
		printf("\n      noPack  (O)ffset Bit(M)ask DW(V)al\n");
		for(uint16_t idx=0;idx<mCurIndex;idx++)
		{
			stErrInjectDesc& desc = mErrInjectList[idx];
			printf(MI_ERRINJ_DEBUG_PRINT_FORMAT,desc.noPack,desc.offset,desc.bitMask,desc.dwVal);
		}
	}

	bool Push(uint16_t noPack,uint16_t offset,uint32_t bitMask,uint32_t dwVal)
	{
		if(mCurIndex >= ERR_INJECT_MAX_COUNT)
		{
			return false;
		}
		stErrInjectDesc*	desc = &(mErrInjectList[mCurIndex++]);
		desc->noPack	= noPack;
		desc->offset	= offset;
		desc->bitMask	= bitMask;
		desc->dwVal		= dwVal;

		return true;
	};

	bool Pull(uint16_t& noPack,uint16_t& offset,uint32_t& bitMask,uint32_t& dwVal)
	{
		if( (mCurIndex-1) >= ERR_INJECT_MAX_COUNT)
		{
			return false;
		}

		return Fetch(--mCurIndex,noPack,offset,bitMask,dwVal);
	};

	bool Fetch(uint16_t index,uint16_t& noPack,uint16_t& offset,uint32_t& bitMask,uint32_t& dwVal)
	{
		if(index >= mCurIndex)
		{
			return false;
		}

		stErrInjectDesc*	desc = &(mErrInjectList[index]);
		noPack	= desc->noPack;
		offset	= desc->offset;
		bitMask	= desc->bitMask;
		dwVal	= desc->dwVal;

		return true;
	}

	bool IsEmpty(void) {return 0 == mCurIndex;};

	void Clear(void)
	{
		mCurIndex	= 0;
	}
};

#endif /* _NVME_MI_NVMEMIERRINJECTION_H_ */
