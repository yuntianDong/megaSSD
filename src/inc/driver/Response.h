#ifndef _RESPONSE_H
#define _RESPONSE_H

#include <stdint.h>

class Response
{
protected:
	uint32_t	mExecTime;
public:
	Response(void)
		:mExecTime(0)
	{};

	virtual ~Response(void)
	{};

	virtual bool Succeeded(void)=0;

	void SetExecTime(uint32_t val) {mExecTime	= val;};
	uint32_t GetExecTime(void) {return mExecTime;}
};

#endif
