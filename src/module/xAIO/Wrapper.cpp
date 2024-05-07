/*
 * file : Wrapper.cpp
 */

#include <string>
#include <stdint.h>
#include <boost/python.hpp>

#include "IAIOPerf.h"
#include "Version.h"

using namespace std;
using namespace boost::python;

BOOST_PYTHON_MODULE(xAIO)
{
	namespace python = boost::python;

	WRAPPER_F_LIST

	class_<IAIOResult>("IAIOResult",init<>())
			.add_property("CmdErrCnt",&IAIOResult::GetCmdErrCnt)
			.add_property("IOPS",&IAIOResult::GetIOPS)
			.add_property("MBPS",&IAIOResult::GetMBps);

	class_<IAIOPerf>("IAIOPerf",init<const char*,bool>())
			.add_property("XferSize",&IAIOPerf::GetXferSize,&IAIOPerf::SetXferSize)
			.add_property("Pattern",&IAIOPerf::GetDataPattern,&IAIOPerf::SetDataPattern)
			.add_property("QDepth",&IAIOPerf::GetQueueDepth,&IAIOPerf::SetQueueDepth)
			.add_property("NumOfQ",&IAIOPerf::GetNumOfQueue,&IAIOPerf::SetNumOfQueue)
			.def("SeqWrite",&IAIOPerf::SeqWrite,python::return_value_policy<python::reference_existing_object>())
			.def("SeqRead",&IAIOPerf::SeqRead,python::return_value_policy<python::reference_existing_object>())
			.def("RandWrite",&IAIOPerf::RandWrite,python::return_value_policy<python::reference_existing_object>())
			.def("RandRead",&IAIOPerf::RandRead,python::return_value_policy<python::reference_existing_object>());

}
