/*
 * file: Context.cpp 
 */

#include "context/Context.h"


/* class ParentContext */
bool ParentContext::AddNsMap(uint32_t nsid,uint64_t offset,uint64_t nscap,uint32_t lbaf)
{
    if(nsid > 1024)
    {
        LOGWARN("Invalid nsid , over 1024 boundry!");
        return false;
    }

    NsMapInfo info = {
        .offset = offset,
        .nsCap  = nscap,
    };

    parentParameter.NsMap[nsid] = info;

    ChildContext* child = this->GetChildContext(nsid);
    child->SetNsCap(nscap);
    if(parentParameter.lbaMap.empty() == false)
    {
        child->SetNsDataSize(parentParameter.lbaMap[lbaf].sectorSize);
        child->SetNsMetaSize(parentParameter.lbaMap[lbaf].metaSize);
    }else{
        LOGWARN("Please initial lbaMap first!");
        return false;
    }


    return true;
}

bool ParentContext::AddLbaMap(uint32_t lbaf,uint32_t sectorsize,uint32_t metasize)
{
    if(metasize > sectorsize)
    {
        LOGERROR("Invalid metaSize!");
        return false;
    }

    LbaMapInfo info = {
        .sectorSize = sectorsize,
        .metaSize   = metasize,
    };
    parentParameter.lbaMap[lbaf] = info;

    return true;
}

void ParentContext::SetTotalNvmSize(uint64_t tNvmsize)
{
    parentParameter.totalNvmSize = tNvmsize;
}

void ParentContext::DumpNsMap(void)
{
    if(parentParameter.NsMap.empty())
    {
        LOGINFO("NsMap is empty!");
        return;
    }
    for(const auto& pair : parentParameter.NsMap)
    {
        LOGINFO("nsid:%4d\tlogical offset:0x%14lx\tnscap:0x%14lx",pair.first,pair.second.offset,pair.second.nsCap);
    }
}

void ParentContext::DumpLbaMap(void)
{
    if(parentParameter.lbaMap.empty())
    {
        LOGINFO("lbaMap is empty!");
        return;
    }
    for(const auto& pair : parentParameter.lbaMap)
    {
        LOGINFO("lbaf:%4d\tsectorSize:%4d\tmetaSize:%4d",pair.first,pair.second.sectorSize,pair.second.metaSize);
    }
}

uint64_t ParentContext::GetTotalNvmSize(void)
{
    return parentParameter.totalNvmSize;
}

ParentParameter* ParentContext::GetParentParameter(void)
{
    return &parentParameter;
}

bool ParentContext::RemoveChildContext(uint32_t nsid)
{
    for(std::vector<ChildContext*>::size_type idx = 0; idx < children.size(); idx++)
    {
        if(children[idx]->GetNsID() == nsid)
        {
            delete children[idx];
            children.erase(children.begin() + idx);
            return true;
        }
    }

    LOGINFO("Given nsid has no context!");
    return false;
}

void ParentContext::ReleaseAllChildContext(void)
{
    for(ChildContext* child : children)
    {
        delete child;
    }

    children.clear();
}

ChildContext* ParentContext::GetChildContext(uint32_t nsid)
{
    if(nsid > MAX_NS_NUM && nsid != NS_ID_SPECIAL)
    {
        LOGERROR("Invalid nsid!");
        return nullptr;
    }

    for(std::vector<ChildContext*>::size_type idx = 0; idx < children.size(); idx++)
    {
        if(children[idx]->GetNsID() == nsid)
        {
            return children[idx];
        }
    }

    ChildContext* child = new ChildContext(this);
    child->SetNsID(nsid);
    children.push_back(child);
    
    return child;
}

std::vector<ChildContext*>* ParentContext::GetChildren(void)
{
    if(children.empty() != true)
    {
        return &children;
    }

    return nullptr;
}