#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <vector>
#include "context/RunningParam.h"

#define MAX_NS_NUM                  1024 // max num of ns related to spec is 1024
#define NS_ID_DEFAULT               0
#define NS_ID_SPECIAL               0xFFFFFFFF


class ChildContext;

class ParentContext
{
private:
    std::vector<ChildContext*> children;
    ParentParameter parentParameter;

public:
    ParentContext()
    {
        children.clear();

        parentParameter.NsMap.clear();
        parentParameter.lbaMap.clear();
        parentParameter.totalNvmSize = 0;
    };
    ~ParentContext()
    {
        this->ReleaseAllChildContext();

        parentParameter.NsMap.clear();
        parentParameter.lbaMap.clear();
    }

    bool AddNsMap(uint32_t nsid,uint64_t offset,uint64_t nscap,uint32_t lbaf);
    bool AddLbaMap(uint32_t lbaf,uint32_t sectorsize,uint32_t metasize);
    void SetTotalNvmSize(uint64_t tNvmsize);

    void DumpNsMap(void);
    void DumpLbaMap(void);

    uint32_t GetSectorSize(uint32_t lbaf)
    {
        if(!parentParameter.lbaMap.count(lbaf))
        {
            LOGWARN("given lbaf is not exist in current lba map!");
            return 0;
        }
        else
        {
            return parentParameter.lbaMap[lbaf].sectorSize;
        }

        return 0;
    };
    uint32_t GetMetaSize(uint32_t lbaf)
    {
        if(!parentParameter.lbaMap.count(lbaf))
        {
            LOGWARN("given lbaf is not exist in current lba map!");
            return 0;
        }
        else
        {
            return parentParameter.lbaMap[lbaf].metaSize;
        }

        return 0;
    }

    uint64_t GetTotalNvmSize(void);
    ParentParameter* GetParentParameter(void);

    bool RemoveChildContext(uint32_t nsid);
    void ReleaseAllChildContext(void);
    ChildContext* GetChildContext(uint32_t nsid);
    std::vector<ChildContext*>* GetChildren(void);

};

class ChildContext
{
private:
    ParentContext* parent;
    ChildParameter childParameter;

public:
    ChildContext(ParentContext* p) : parent(p)
    {
        if(parent == nullptr)
        {
            LOGERROR("ChildContext has no Parent context, please check!");
        }

        childParameter.cmdTO        = DEF_PARAM_CMD_TIME_OUT;
        childParameter.nsCapacity   = DEF_PARAM_NS_CAPACITY;
        childParameter.nsDataSize   = DEF_PARAM_NS_DATA_SIZE;
        childParameter.nsMetaSize   = DEF_PARAM_NS_META_SIZE;
        childParameter.nsPIGuardSize= DEF_PARAM_NS_PI_GUARD_SIZE;
        childParameter.nsPISTagSize = DEF_PARAM_NS_PI_STORAGE_TAG_SIZE;

    };
    ~ChildContext(){};

    void SetCmdTO(uint32_t timeout)                 {childParameter.cmdTO = timeout;};
    void SetNsID(uint32_t nsid)                     {childParameter.nsID = nsid;};
    void SetNsCap(uint64_t nscap)                   {childParameter.nsCapacity = nscap;};
    void SetNsDataSize(uint32_t nsDataSize)         {childParameter.nsDataSize = nsDataSize;};
    void SetNsMetaSize(uint32_t nsMetaSize)         {childParameter.nsMetaSize = nsMetaSize;};
    void SetNsPIGuardSize(uint32_t nsPIGuardSize)   {childParameter.nsPIGuardSize = nsPIGuardSize;};
    void SetNsPISTagSize(uint32_t nsPISTagSize)     {childParameter.nsPISTagSize = nsPISTagSize;};

    uint32_t GetCmdTO(void)                         {return childParameter.cmdTO;};
    uint32_t GetNsID(void)                          {return childParameter.nsID;};
    uint64_t GetNsCap(void)                         {return childParameter.nsCapacity;};
    uint32_t GetNsDataSize(void)                    {return childParameter.nsDataSize;};
    uint32_t GetNsMetaSize(void)                    {return childParameter.nsMetaSize;};
    uint32_t GetNsPIGuardSize(void)                 {return childParameter.nsPIGuardSize;};
    uint32_t GetNsPISTagSize(void)                  {return childParameter.nsPISTagSize;};

    ParentContext* GetParent(void)
    {
        if(parent != nullptr)
        {
            return parent;
        }

        return nullptr;
    };

    ChildParameter* GetChildParameter(void)         {return &childParameter;};

};

#endif