/*
 * file : Wrapper.cpp
 */

#include <string>
#include <stdint.h>
#include <boost/python.hpp>

#include "udev/udevScan.h"
#include "Version.h"

using namespace std;
using namespace boost::python;

BOOST_PYTHON_MODULE(xUDev)
{
	namespace python = boost::python;

	WRAPPER_F_LIST

	enum_<_enScanType>("enScanType")
			.value("BySysName",ScanBy_SysName)
			.value("BySysAttr",ScanBy_SysAttr)
			.value("ByProperty",ScanBy_Property)
			.value("ByTag",ScanBy_Tag)
			.value("BySubSystem",ScanBy_SubSystem);

	class_<uDevNode>("uDevNode",
					init<>())
					.add_property("DevPath",&uDevNode::GetDevPath)
					.add_property("SubSystem",&uDevNode::GetSubSystem)
					.add_property("DevType",&uDevNode::GetDevType)
					.add_property("SysPath",&uDevNode::GetSysPath)
					.add_property("SysName",&uDevNode::GetSysName)
					.add_property("SysNum",&uDevNode::GetSysNum)
					.add_property("DevNode",&uDevNode::GetDevNode)
					.add_property("Driver",&uDevNode::GetDriver)
					.add_property("DevNum",&uDevNode::GetDevNum)
					.add_property("Action",&uDevNode::GetAction)
					.add_property("SeqNum",&uDevNode::GetSeqNum)
					.add_property("ElapsedTime",&uDevNode::GetElapsedTime)
					.def("DebugDump",&uDevNode::DebugDump)
					.def("HasDevLink",&uDevNode::HasDevLink)
					.def("GetDevLink",&uDevNode::GetDevLink)
					.def("HasDevProperty",&uDevNode::HasDevProperty)
					.def("GetDevProperty",&uDevNode::GetDevProperty)
					.def("HasDevTag",&uDevNode::HasDevTag)
					.def("GetDevTag",&uDevNode::GetDevTag)
					.def("HasSysAttr",&uDevNode::HasSysAttr)
					.def("GetSysAttr",&uDevNode::GetSysAttr)
					.def("GetParent",&uDevNode::GetParent,python::return_value_policy<python::reference_existing_object>())
					.def("GetParentWithSubsystemAndDevType",&uDevNode::GetParentWithSubsystemAndDevType,python::return_value_policy<python::reference_existing_object>());

	class_<uDevScan>("uDevScan",
					init<>())
					.def("DoScan",&uDevScan::DoScan)
					.def("GetDevNodeCount",&uDevScan::GetDevNodeCount)
					.def("GetDevNode",&uDevScan::GetDevNode,python::return_value_policy<python::reference_existing_object>())
					.def("GetNextDevNode",&uDevScan::GetNextDevNode,python::return_value_policy<python::reference_existing_object>());
}
