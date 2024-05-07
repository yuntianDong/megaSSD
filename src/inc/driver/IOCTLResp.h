#ifndef _IOCTL_RESP_H
#define _IOCTL_RESP_H

#include "Response.h"

#define ST_IOCTL_SUCCESS		0
#define ST_UNDEFINED			0x8BAD

class IOCtlResp : public Response
{
protected:
	int		mErrCode;
public:
	IOCtlResp(void)
		:mErrCode(ST_UNDEFINED)
	{};

	void SetStatus(int val) {mErrCode = val;};
	int GetStatus(void) {return mErrCode;};

	bool Succeeded(void) {return ST_IOCTL_SUCCESS == GetStatus();};
};

#endif
