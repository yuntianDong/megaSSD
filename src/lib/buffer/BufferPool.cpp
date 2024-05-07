/*
 * file : BufferPool.cpp
 */

#include <stdlib.h>
#include <string.h>

#include "buffer/BufferPool.h"


BaseBufPool::BaseBufPool(enBufType bufType,bool bNewBufIfNotExist)
{
	muBufHeader	= muBufTailer	= 0;
	muBufAllocSize	= 0;
	menBufType		= bufType;
	mbCreateNewBufIfNotExist	= bNewBufIfNotExist;
	mbCreateNewBufIfLargerSize	= true;

	for(int idx=0;idx<BUFPOOL_ITEM_MAX;idx++)
	{
		mpBufPool[idx]	= NULL;
	}
}

BaseBufPool::~BaseBufPool()
{
	for(int idx=0;idx<BUFPOOL_ITEM_MAX;idx++)
	{
		if(NULL != mpBufPool[idx])
		{
			free(mpBufPool[idx]);
			mpBufPool[idx]	= NULL;
		}
	}
}

Buffers* BaseBufPool::Alloc(const char *bufName,uint64_t bufSize,const char* mode)
{
	Buffers* buf = NULL;
	if(0 == bufSize)
	{
		bufSize	= GetAllocBufSize();
	}

	stBufItemDesc* desc = GetBufItemDesc(bufName);
	if( true == mbCreateNewBufIfLargerSize &&
			NULL != desc && NULL != desc->pBuf &&
			(bufSize > (desc->uBufSize)) )
	{
		buf = CreateBufItem(bufSize,mode);
		if(NULL == buf)
		{
			LOGERROR("Fail to Alloc Buffer");
			return NULL;
		}

		buf->CopyFromBuff(*(desc->pBuf),0,0,desc->uBufSize);

		ClrBufItemDesc(desc);
		SetBufItemDesc(desc,bufName,buf,buf->GetBufSize());
	}
	else if(NULL == desc || NULL == desc->pBuf)
	{
		if(false == Advance(muBufHeader))
		{
			return NULL;
		}

		buf = CreateBufItem(bufSize,mode);
		if(NULL == buf)
		{
			LOGERROR("Fail to Alloc Buffer");
			return NULL;
		}

		SetBufItemDesc(bufName,buf,buf->GetBufSize());
	}
	else
	{
		buf	= desc->pBuf;
	}

	while(NeedRelease())
	{
		Release(muBufTailer);
		Follow(muBufTailer);
	}

	return buf;
}

Buffers* BaseBufPool::Get(const char* bufName,uint64_t bufSize,const char* mode)
{
	if(0 == bufSize)
	{
		bufSize	= GetAllocBufSize();
	}

	stBufItemDesc* desc = GetBufItemDesc(bufName);
	if(NULL == desc)
	{
		if(true == mbCreateNewBufIfNotExist and 0 != bufSize)
		{
			return Alloc(bufName,bufSize,mode);
		}

		return NULL;
	}else if(bufSize > (desc->uBufSize))
	{
		return Alloc(bufName,bufSize,mode);
	}

	return desc->pBuf;
}

void BaseBufPool::Dump(void)
{
	LOGINFO("muBufHeader = %d\n",muBufHeader);
	LOGINFO("muBufTailer = %d\n",muBufTailer);

	for(int idx=0;idx<BUFPOOL_ITEM_MAX;idx++)
	{
		stBufItemDesc* desc = mpBufPool[idx];

		if(NULL != desc)
		{
			LOGINFO("Index [%d]\n",idx);
			LOGINFO("\tName\t= %s\n",desc->cBufName);
			if(NULL != desc->pBuf)
			{
				LOGINFO("\tBufAddr\t= %lx\n",desc->pBuf->GetBufferAddr());
			}
			else
			{
				LOGINFO("\tBufAddr\t= 0\n");
			}
			LOGINFO("\tBufSize\t= %d\n",desc->uBufSize);
		}
	}
}

Buffers* BaseBufPool::CreateBufItem(uint64_t bufSize,const char* mode)
{
	if(0 == bufSize)
	{
		LOGERROR("Invalid Buffer Size");
		return NULL;
	}

	switch(menBufType)
	{
		case BUF_T_SYS:
			return (Buffers*)new CommandBuf(bufSize,mode);
		case BUF_T_UNK:
		default:
			return new HostBuffer(bufSize,mode);
	}

	return NULL;
}

stBufItemDesc* BaseBufPool::GetBufItemDesc(const char *bufName)
{
	uint32_t	index	= muBufTailer;
	do
	{
		stBufItemDesc* desc = mpBufPool[index];

		if(NULL != desc)
		{
			if(0 == strncmp(desc->cBufName,bufName,strlen(bufName)))
			{
				return desc;
			}
		}
	}while( Follow(index) && index != (muBufHeader+1) );

	return NULL;
}

stBufItemDesc* BaseBufPool::GetBufItemDesc(uint32_t index)
{
	if((uint32_t)-1 == index)
	{
		index	= muBufHeader;
	}

	if(index >= BUFPOOL_ITEM_MAX)
	{
		LOGERROR("Illegal Index = %d",index);
		return NULL;
	}

	return mpBufPool[index];
}

void BaseBufPool::SetBufItemDesc(stBufItemDesc* desc,const char *bufName,Buffers* pBuf,uint64_t bufSize)
{
	if(NULL == desc)
	{
		LOGERROR("Invalid desc Parameter!!!\n");
		return;
	}

	strncpy(desc->cBufName,bufName,strlen(bufName));

	if(NULL != desc->pBuf)
	{
		free(desc->pBuf);
		muBufAllocSize	-= (desc->uBufSize);
	}

	desc->pBuf		= pBuf;
	desc->uBufSize	= bufSize;
	muBufAllocSize	+= bufSize;

	return;
}

void BaseBufPool::SetBufItemDesc(const char *bufName,Buffers* pBuf,uint64_t bufSize)
{
	stBufItemDesc* desc	= mpBufPool[muBufHeader];
	if(NULL == desc)
	{
		desc = (stBufItemDesc*)malloc(sizeof(stBufItemDesc));
		memset(desc,0,sizeof(stBufItemDesc));
		mpBufPool[muBufHeader] = desc;
	}

	SetBufItemDesc(desc,bufName,pBuf,bufSize);
}

void BaseBufPool::ClrBufItemDesc(stBufItemDesc* desc)
{
	if(NULL != desc)
	{
		memset(desc->cBufName,0,BUFITEM_DESCNAME_LEN);
		if(NULL != desc->pBuf)
		{
			free(desc->pBuf);
			muBufAllocSize -= (desc->uBufSize);
			desc->pBuf		= NULL;
			desc->uBufSize	= 0;
		}
	}

	return;
}

void BaseBufPool::ClrBufItemDesc(uint32_t index)
{
	stBufItemDesc* desc = GetBufItemDesc(index);
	ClrBufItemDesc(desc);

	return;
}

void BaseBufPool::UptBufItemDesc(const char *bufName,uint64_t bufSize)
{
	stBufItemDesc* desc	= mpBufPool[muBufHeader];
	if(NULL != desc && NULL != desc->pBuf)
	{
		strncpy(desc->cBufName,bufName,strlen(bufName));
		desc->uBufSize	= bufSize;
	}

	return;
}

bool BaseBufPool::Advance(uint32_t &Header)
{
	if(Header+1 >= BUFPOOL_ITEM_MAX)
		return false;

	Header++;
	return true;
}

bool BaseBufPool::Follow(uint32_t &Tailer)
{
	if(Tailer+1 >= BUFPOOL_ITEM_MAX)
		return false;

	Tailer++;
	return true;
}

bool BaseBufPool::NeedRelease(void)
{
	return false;
}

void BaseBufPool::Release(uint32_t index)
{
	stBufItemDesc* desc = GetBufItemDesc(index);

	if(NULL == desc || NULL == desc->pBuf)
	{
		LOGWARN("Invalid Buffer Item Description,index=%d\n",index);
		return;
	}

	free(desc->pBuf);
	muBufAllocSize -= (desc->uBufSize);

	LOGINFO("[%d]-Buffer[%s] Size=%d Released\n",index,desc->cBufName,desc->uBufSize);

	memset(desc->cBufName,0,BUFITEM_DESCNAME_LEN);
	desc->pBuf		= NULL;
	desc->uBufSize	= 0;

	return;
}

RingBufPool::RingBufPool(uint32_t bufItemSize,uint32_t bufItemMax,enBufType bufType,bool bNewBufIfNotExist)
	:BaseBufPool(bufType,bNewBufIfNotExist),muBufItemSize(bufItemSize),muBufItemMax(bufItemMax)
{
}

bool RingBufPool::Advance(uint32_t &Header)
{
	uint32_t maxIndex	= int(muBufItemMax/muBufItemSize);
	if(++Header >= maxIndex)
	{
		Header	= 0;
	}

	return true;
}

Buffers* RingBufPool::CreateBufItem(uint64_t bufSize,const char* mode)
{
	bufSize	= muBufItemSize;
	return BaseBufPool::CreateBufItem(bufSize,mode);
}

LimSizeBufPool::LimSizeBufPool(uint32_t maxBufPoolSize,enBufType bufType,bool bNewBufIfNotExist)
	:BaseBufPool(bufType,bNewBufIfNotExist),muMaxBufPoolSize(maxBufPoolSize)
{
}

bool LimSizeBufPool::Advance(uint32_t &Header)
{
	if(++Header >= BUFPOOL_ITEM_MAX)
		Header = 0;

	return true;
}

bool LimSizeBufPool::Follow(uint32_t &Tailer)
{
	if(++Tailer >= BUFPOOL_ITEM_MAX)
		Tailer = 0;

	return true;
}

bool LimSizeBufPool::NeedRelease(void)
{
	return (muMaxBufPoolSize < GetAllocBytes());
}

MaxSizeBufPool::MaxSizeBufPool(uint32_t maxBufPoolSize,enBufType bufType,bool bNewBufIfNotExist)
	:BaseBufPool(bufType,bNewBufIfNotExist),muMaxBufPoolSize(maxBufPoolSize)
{
}

bool MaxSizeBufPool::Advance(uint32_t &Header)
{
	if(muMaxBufPoolSize <= GetAllocBytes())
	{
		return false;
	}

	if(Header+1 >= BUFPOOL_ITEM_MAX)
		return false;

	Header++;
	return true;
}
