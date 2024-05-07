#ifndef _BLOCK_OP_RESP_H
#define _BLOCK_OP_RESP_H

#include "../IOCtlResp.h"

class BlkOpResp : public IOCtlResp
{
private:
	uint64_t	mRtnData;
public:
	BlkOpResp(void)
		:IOCtlResp()
	{};

	void SetRespData(uint64_t val) {mRtnData = val;};
	uint64_t GetRespData(void) {return mRtnData;};
};

#endif
