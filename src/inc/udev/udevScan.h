#ifndef _UDEV_SCAN_H
#define _UDEV_SCAN_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>

#include <libudev.h>

using namespace std;

#define MAX_NODE_CNT	128

typedef enum _enScanType
{
	ScanBy_SysName,
	ScanBy_SysAttr,
	ScanBy_Property,
	ScanBy_Tag,
	ScanBy_SubSystem,
}enScanType;

class uDevNode
{
private:
	struct udev_device *	mpuDevice;

	typedef pair<string,string>	KVType;
	typedef map<string,string>  KVTable;

	KVTable		mDevLinkLst;
	KVTable		mDevPropLst;
	KVTable		mDevTagLst;
	KVTable		mSysAttrLst;

	void GetDevLinkLst(void);
	void GetDevPropLst(void);
	void GetDevTagLst(void);
	void GetSysAttrLst(void);

	void PrintKVTable(KVTable& tbl);

public:
	uDevNode(void)
		:mpuDevice(NULL)
	{
	};
	uDevNode(struct udev_device * udev)
		:mpuDevice(udev)
	{
	};

	virtual ~uDevNode(void)
	{
		udev_device_unref(mpuDevice);
	}

	void DebugDump(void);

	uDevNode* GetParent(void);
	uDevNode* GetParentWithSubsystemAndDevType(const char* subsystem,const char* devType);

	const char* GetDevPath(void);
	const char* GetSubSystem(void);
	const char* GetDevType(void);
	const char* GetSysPath(void);
	const char* GetSysName(void);
	const char* GetSysNum(void);
	const char* GetDevNode(void);
	const char* GetDriver(void);
	uint32_t GetDevNum(void);
	const char* GetAction(void);
	uint64_t GetSeqNum(void);
	uint64_t GetElapsedTime(void);

	bool HasDevLink(const char* key);
	const char* GetDevLink(const char* key);
	bool HasDevProperty(const char* key);
	const char* GetDevProperty(const char* key);
	bool HasDevTag(const char* key);
	const char* GetDevTag(const char* key);
	bool HasSysAttr(const char* key);
	const char* GetSysAttr(const char* key);
};

class uDevScan
{
private:
	struct udev *			mpuDev;

	uDevNode*				mpDevNodeLst[MAX_NODE_CNT];
	int						mTotalNodeCnt;
	int						mCurIndex;

	void ReleaseAll(void)
	{
		for(int idx=0;idx<mTotalNodeCnt;idx++)
		{
			if(NULL != mpDevNodeLst[idx])
			{
				delete mpDevNodeLst[idx];
				mpDevNodeLst[idx] = NULL;
			}
		}

		mTotalNodeCnt = 0;
		mCurIndex = 0;
	}
public:
	uDevScan(void)
	{
		mpuDev 			= udev_new();
		memset(mpDevNodeLst,0,MAX_NODE_CNT*sizeof(uDevNode*));
		mTotalNodeCnt	= 0;
		mCurIndex		= 0;
	};
	virtual ~uDevScan(void)
	{
		ReleaseAll();

		if(NULL != mpuDev)
		{
			udev_unref(mpuDev);
		}
	}

	int DoScan(enScanType ty,const char* key,const char* val="");

	int GetDevNodeCount(void) {return mTotalNodeCnt;};

	uDevNode* GetDevNode(int idx)
	{
		if(idx >= mTotalNodeCnt)
		{
			return NULL;
		}
		return mpDevNodeLst[idx];
	};

	uDevNode* GetNextDevNode(bool bFromHead=false)
	{
		if(true == bFromHead)
		{
			mCurIndex = 0;
		}

		uDevNode* dev = mpDevNodeLst[mCurIndex];
		mCurIndex = (++mCurIndex >= mTotalNodeCnt)?0:mCurIndex;

		return dev;
	};
};

#endif
