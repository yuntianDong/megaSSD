#ifndef _COMMAND_RESPONSE_H
#define _COMMAND_RESPONSE_H

#include "Response.h"
#include "buffer/Buffers.h"

#define DUMP_LINE_BYTES			8

class CmdResp : public Response
{
protected:
	Buffers*	mCmdDataBuf;

	CmdResp(void)
		:mCmdDataBuf(NULL)
	{};
public:
	virtual void* GetRespAddr(void)=0;
	virtual uint32_t GetRespSize(void)=0;
	virtual void* GetCmdAddr(void)=0;
	virtual uint32_t GetCmdSize(void)=0;

	void SetDataBuffer(Buffers* buf) {mCmdDataBuf = buf;};
	Buffers* GetDataBuffer(void) {return mCmdDataBuf;};

	void DumpHex(void)
	{
		uint8_t*	req		= (uint8_t*)(this->GetCmdAddr());
		uint8_t*	resp	= (uint8_t*)(this->GetRespAddr());
		uint32_t	reqSize	= this->GetCmdSize();
		uint32_t	respSize= this->GetRespSize();

		uint32_t maxSize = (reqSize > respSize)?reqSize:respSize;

		for(uint32_t idx=0;idx<maxSize;)
		{
			printf("\n[%04x] ",idx);

			for(uint32_t tmp=0;tmp<DUMP_LINE_BYTES;tmp++)
			{
				if((idx+tmp) < reqSize)
				{
					printf("%02x ",req[(idx+tmp)]);
				}
			}

			printf("| ");

			for(uint32_t tmp=0;tmp<DUMP_LINE_BYTES;tmp++)
			{
				if((idx+tmp) < respSize)
				{
					printf("%02x ",resp[(idx+tmp)]);
				}
			}

			idx += DUMP_LINE_BYTES;
		}
		printf("\n");
	}
};

#endif
