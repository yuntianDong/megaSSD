/*
 * file : Wrapper.cpp
 */
#include <boost/python.hpp>
#include "context/Context.h"
#include "Version.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(xContext)
{
    namespace python = boost::python;

    WRAPPER_F_LIST

    class_<ParentContext>("ParentContext")
    .add_property("totalNvmSize",&ParentContext::GetTotalNvmSize,&ParentContext::SetTotalNvmSize)
    .def("AddNsMap",&ParentContext::AddNsMap)
    .def("AddLbaMap",&ParentContext::AddLbaMap)
    .def("DumpNsMap",&ParentContext::DumpNsMap)
    .def("DumpLbaMap",&ParentContext::DumpLbaMap)
    .def("GetSectorSize",&ParentContext::GetSectorSize)
    .def("GetMetaSize",&ParentContext::GetMetaSize)
    .def("RemoveChildContext",&ParentContext::RemoveChildContext)
    .def("GetChildContext",&ParentContext::GetChildContext,return_value_policy<reference_existing_object>());

    class_<ChildContext>("ChildContext",init<ParentContext*>())
    .add_property("cmdTO",&ChildContext::GetCmdTO,&ChildContext::SetCmdTO)
    .add_property("nsID",&ChildContext::GetNsID,&ChildContext::SetNsID)
    .add_property("nsCap",&ChildContext::GetNsCap,&ChildContext::SetNsCap)
    .add_property("nsDataSize",&ChildContext::GetNsDataSize,&ChildContext::SetNsDataSize)
    .add_property("nsMetaSize",&ChildContext::GetNsMetaSize,&ChildContext::SetNsMetaSize)
    .add_property("nsPiGuardSize",&ChildContext::GetNsPIGuardSize,&ChildContext::SetNsPIGuardSize)
    .add_property("nsPiStorageTagSize",&ChildContext::GetNsPISTagSize,&ChildContext::SetNsPISTagSize)
    .def("GetParent",&ChildContext::GetParent,return_value_policy<reference_existing_object>());
}