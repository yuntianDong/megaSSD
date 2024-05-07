/*
 * udevScan.cpp
 * libudev : https://mirrors.edge.kernel.org/pub/linux/utils/kernel/hotplug/libudev/ch01.html
 */

#include "udev/udevScan.h"
#include "Logging.h"

#define INVALID_STRING_VALUE	""

void uDevNode::GetDevLinkLst(void)
{
	struct udev_list_entry * le = udev_device_get_devlinks_list_entry(mpuDevice);
	struct udev_list_entry * nd = le;

	udev_list_entry_foreach(le,nd)
	{
		const char* name = udev_list_entry_get_name(nd);
		const char* value= udev_list_entry_get_value(nd);

		if(value == NULL)
		{
			mDevLinkLst.insert(KVType(name,""));
		}
		else
		{
			mDevLinkLst.insert(KVType(name,value));
		}

		nd = udev_list_entry_get_next(le);
	}
}

void uDevNode::GetDevPropLst(void)
{
	struct udev_list_entry * le = udev_device_get_properties_list_entry(mpuDevice);
	struct udev_list_entry * nd = le;

	udev_list_entry_foreach(le,nd)
	{
		const char* name = udev_list_entry_get_name(nd);
		const char* value= udev_list_entry_get_value(nd);

		if(value == NULL)
		{
			mDevPropLst.insert(KVType(name,""));
		}
		else
		{
			mDevPropLst.insert(KVType(name,value));
		}

		nd = udev_list_entry_get_next(le);
	}
}

void uDevNode::GetDevTagLst(void)
{
	struct udev_list_entry * le = udev_device_get_tags_list_entry(mpuDevice);
	struct udev_list_entry * nd = le;

	udev_list_entry_foreach(le,nd)
	{
		const char* name = udev_list_entry_get_name(nd);
		const char* value= udev_list_entry_get_value(nd);

		if(value == NULL)
		{
			mDevTagLst.insert(KVType(name,""));
		}
		else
		{
			mDevTagLst.insert(KVType(name,value));
		}

		nd = udev_list_entry_get_next(le);
	}
}

void uDevNode::GetSysAttrLst(void)
{
	struct udev_list_entry * le = udev_device_get_sysattr_list_entry(mpuDevice);
	struct udev_list_entry * nd = le;

	udev_list_entry_foreach(le,nd)
	{
		const char* name = udev_list_entry_get_name(nd);
		const char* value= udev_list_entry_get_value(nd);

		if(value == NULL)
		{
			mSysAttrLst.insert(KVType(name,""));
		}
		else
		{
			mSysAttrLst.insert(KVType(name,value));
		}

		nd = udev_list_entry_get_next(le);
	}
}

uDevNode* uDevNode::GetParent(void)
{
	struct udev_device * dev = udev_device_get_parent(mpuDevice);
	if(NULL == dev)
	{
		return NULL;
	}

	return new uDevNode(dev);
}

uDevNode* uDevNode::GetParentWithSubsystemAndDevType(const char* subsystem,const char* devType)
{
	struct udev_device * dev = udev_device_get_parent_with_subsystem_devtype(mpuDevice,
			subsystem,devType);
	if(NULL == dev)
	{
		return NULL;
	}

	return new uDevNode(dev);
}

const char* uDevNode::GetDevPath(void)
{
	return udev_device_get_devpath(mpuDevice);
}

const char* uDevNode::GetSubSystem(void)
{
	return udev_device_get_subsystem(mpuDevice);
}

const char* uDevNode::GetDevType(void)
{
	return udev_device_get_devtype(mpuDevice);
}

const char* uDevNode::GetSysPath(void)
{
	return udev_device_get_syspath(mpuDevice);
}

const char* uDevNode::GetSysName(void)
{
	return udev_device_get_sysname(mpuDevice);
}

const char* uDevNode::GetSysNum(void)
{
	return udev_device_get_sysnum(mpuDevice);
}

const char* uDevNode::GetDevNode(void)
{
	return udev_device_get_devnode(mpuDevice);
}

const char* uDevNode::GetDriver(void)
{
	return udev_device_get_driver(mpuDevice);
}

uint32_t uDevNode::GetDevNum(void)
{
	return (uint32_t)udev_device_get_devnum(mpuDevice);
}

const char* uDevNode::GetAction(void)
{
	return udev_device_get_action(mpuDevice);
}

uint64_t uDevNode::GetSeqNum(void)
{
	return (uint64_t)udev_device_get_seqnum(mpuDevice);
}

uint64_t uDevNode::GetElapsedTime(void)
{
	return (uint64_t)udev_device_get_usec_since_initialized(mpuDevice);
}

bool uDevNode::HasDevLink(const char* key)
{
	if(0 == mDevLinkLst.size())
	{
		GetDevLinkLst();
	}

	KVTable::iterator iter = mDevLinkLst.find(key);

	return iter != mDevLinkLst.end();
}

const char* uDevNode::GetDevLink(const char* key)
{
	if(0 == mDevLinkLst.size())
	{
		GetDevLinkLst();
	}

	KVTable::iterator iter = mDevLinkLst.find(key);

	if(iter == mDevLinkLst.end())
	{
		return INVALID_STRING_VALUE;
	}

	return iter->second.c_str();
}

bool uDevNode::HasDevProperty(const char* key)
{
	if(0 == mDevPropLst.size())
	{
		GetDevPropLst();
	}

	KVTable::iterator iter = mDevPropLst.find(key);

	return iter != mDevPropLst.end();
}

const char* uDevNode::GetDevProperty(const char* key)
{
	if(0 == mDevPropLst.size())
	{
		GetDevPropLst();
	}

	KVTable::iterator iter = mDevPropLst.find(key);

	if(iter == mDevPropLst.end())
	{
		return INVALID_STRING_VALUE;
	}

	return iter->second.c_str();
}

bool uDevNode::HasDevTag(const char* key)
{
	if(0 == mDevTagLst.size())
	{
		GetDevTagLst();
	}

	KVTable::iterator iter = mDevTagLst.find(key);

	return iter != mDevTagLst.end();
}

const char* uDevNode::GetDevTag(const char* key)
{
	if(0 == mDevTagLst.size())
	{
		GetDevTagLst();
	}

	KVTable::iterator iter = mDevTagLst.find(key);

	if(iter == mDevTagLst.end())
	{
		return INVALID_STRING_VALUE;
	}

	return iter->second.c_str();
}

bool uDevNode::HasSysAttr(const char* key)
{
	if(0 == mSysAttrLst.size())
	{
		GetSysAttrLst();
	}

	KVTable::iterator iter = mSysAttrLst.find(key);

	return iter != mSysAttrLst.end();
}

const char* uDevNode::GetSysAttr(const char* key)
{
	if(0 == mSysAttrLst.size())
	{
		GetSysAttrLst();
	}

	KVTable::iterator iter = mSysAttrLst.find(key);

	if(iter == mSysAttrLst.end())
	{
		return INVALID_STRING_VALUE;
	}

	return iter->second.c_str();
}

int uDevScan::DoScan(enScanType ty,const char* key,const char* val)
{
	ReleaseAll();

	struct udev_enumerate * uEnum = udev_enumerate_new(mpuDev);
	if(NULL == uEnum)
	{
		return false;
	}

	switch(ty)
	{
	case ScanBy_SysName:
		udev_enumerate_add_match_sysname(uEnum,key);
		break;
	case ScanBy_SysAttr:
		udev_enumerate_add_match_sysattr(uEnum,key,val);
		break;
	case ScanBy_Property:
		udev_enumerate_add_match_property(uEnum,key,val);
		break;
	case ScanBy_Tag:
		udev_enumerate_add_match_tag(uEnum,key);
		break;
	case ScanBy_SubSystem:
		udev_enumerate_add_match_subsystem(uEnum,key);
		break;
	default:
		return false;
	}

	if(0 != udev_enumerate_scan_devices(uEnum))
	{
		return false;
	}

	struct udev_list_entry * devLst = udev_enumerate_get_list_entry(uEnum);
	struct udev_list_entry * node   = devLst;

	mTotalNodeCnt = 0;
	udev_list_entry_foreach(devLst,node)
	{
		const char* devPath = udev_list_entry_get_name(node);

		struct udev_device * dev = udev_device_new_from_syspath(mpuDev,devPath);
		if(NULL != dev)
		{
			mpDevNodeLst[mTotalNodeCnt++] = new uDevNode(dev);
		}

		if(mTotalNodeCnt >= MAX_NODE_CNT)
		{
			break;
		}

		node = udev_list_entry_get_next(devLst);
	}

	udev_enumerate_unref(uEnum);

	return GetDevNodeCount();
}

void uDevNode::PrintKVTable(KVTable& tbl)
{
	KVTable::iterator iter = tbl.begin();

	for(;iter !=tbl.end();iter++)
	{
		printf("%s = %s\n",iter->first.c_str(),iter->second.c_str());
	}

	return;
}

void uDevNode::DebugDump(void)
{
	if(0 == mDevLinkLst.size())
	{
		GetDevLinkLst();
	}

	if(0 == mDevPropLst.size())
	{
		GetDevPropLst();
	}

	if(0 == mDevTagLst.size())
	{
		GetDevTagLst();
	}

	if(0 == mSysAttrLst.size())
	{
		GetSysAttrLst();
	}

	printf("###### mDevLinkLst ######\n");
	printf("--------------------------------\n");
	PrintKVTable(mDevLinkLst);
	printf("--------------------------------\n");

	printf("###### mDevPropLst ######\n");
	printf("--------------------------------\n");
	PrintKVTable(mDevPropLst);
	printf("--------------------------------\n");

	printf("###### mDevTagLst  ######\n");
	printf("--------------------------------\n");
	PrintKVTable(mDevTagLst);
	printf("--------------------------------\n");

	printf("###### mSysAttrLst ######\n");
	printf("--------------------------------\n");
	PrintKVTable(mSysAttrLst);
	printf("--------------------------------\n");

	printf("--------------------------------\n");
	printf("DevPath   = %s \n",GetDevPath());
	printf("SubSystem = %s \n",GetSubSystem());
	printf("DevType   = %s \n",GetDevType());
	printf("SysPath   = %s \n",GetSysPath());
	printf("SysName   = %s \n",GetSysName());
	printf("SysNum    = %s \n",GetSysNum());
	printf("DevNode   = %s \n",GetDevNode());
	printf("Driver    = %s \n",GetDriver());
	printf("DevNum    = %d \n",GetDevNum());
	printf("Action    = %s \n",GetAction());
	printf("SeqNum    = %ld \n",GetSeqNum());
	printf("ElapsedTime = %ld \n",GetElapsedTime());
	printf("--------------------------------\n");
}
