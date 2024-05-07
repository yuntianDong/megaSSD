#ifndef _DNVME_METABUF_H
#define _DNVME_METABUF_H

#include "DNVMeIOCtrl.h"
#include "buffer/Buffers.h"

#include <map>
using namespace std;

#define METABUF_ID_MAX					1024
#define DEF_META_BUFSIZE				16
#define DEF_META_BUFALIGN				1
#define INVALID_META_BUFSIZE			(uint32_t)-1

class DNVMeMetaBuf
{
private:
	int				mDevHdlr;
	uint32_t		mMetaBufSize;
	uint32_t		mCurAllocMetaID;
	DNVMeIOCtrl*	mpDNVMeIOCtrl;

	map<uint32_t,Buffers*>	mAllocMetaBufLst;

	void ReleaseAllMetaBuf(void);
protected:
	DNVMeMetaBuf(int devHdlr);

	int GetDevHdlr(void) {return mDevHdlr;};
public:
	virtual ~DNVMeMetaBuf(void);

	static DNVMeMetaBuf*	gInstance;
	static DNVMeMetaBuf*	GetInstance(int devHdlr);

	bool SetMetaBufSize(uint32_t metaSize);
	uint32_t GetMetaBufSize(void) {return mMetaBufSize;};

	Buffers*	GetMetaBuf(uint32_t&	metaId);
};

#endif
