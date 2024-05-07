#ifndef _I_BASIC_LOCK_H
#define _I_BASIC_LOCK_H

#include <iostream>

#include <boost/thread/mutex.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/externally_locked.hpp>
#include <boost/thread/strict_lock.hpp>
#include <boost/thread/lock_types.hpp>

using namespace boost;

typedef basic_lockable_adapter<boost::mutex> 	lockable_type;
typedef strict_lock<lockable_type>		guard_lock;

class IBasicLock
{
protected:
	basic_lockable_adapter<boost::mutex> *mLock;

	IBasicLock(void)
	{
		mLock	= new basic_lockable_adapter<boost::mutex>();
	}

public:
	virtual ~IBasicLock(void)
	{
		if(NULL != mLock)
		{
			delete mLock;
			mLock = NULL;
		}
	}
};

class IBasicLockWithDutParam : public IBasicLock
{
protected:
	virtual uint32_t GetCurrNSID(guard_lock& guard)=0;
	virtual uint32_t GetCurrSectorSize(guard_lock& guard)=0;
	virtual uint32_t GetCurrNSCap(guard_lock& guard)=0;
	virtual bool CheckFlagAddrOL(guard_lock& guard)=0;
	virtual bool CheckFlagPatRec(guard_lock& guard)=0;
};

#endif
