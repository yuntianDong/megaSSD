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

	class_<PerfResult>("PerfResult",
			init<>())
				.add_property("ErrCode",&PerfResult::GetErrorCode)
				.add_property("IOPS",&PerfResult::GetIOPS)
				.add_property("MBps",&PerfResult::GetMBPs)
				.add_property("ioErrCnt",&PerfResult::GetIOErrors);

	class_<IAIOPerf>("IAIOPerf",
			init<const char*>())
				.add_property("bStopWhenError",&IAIOPerf::GetStopWhenError,&IAIOPerf::SetStopWhenError)
				.add_property("bTimeBased",&IAIOPerf::GetTimeBased,&IAIOPerf::SetTimeBased)
				.add_property("bNeedVerify",&IAIOPerf::GetNeedVerify,&IAIOPerf::SetNeedVerify)
				.add_property("QDepth",&IAIOPerf::GetQDepth,&IAIOPerf::SetQDepth)
				.add_property("NumOfQ",&IAIOPerf::GetNumOfQ,&IAIOPerf::SetNumOfQ)
				.add_property("DataPattern",&IAIOPerf::GetDataPattern,&IAIOPerf::SetDataPattern)
				.add_property("XferSize",&IAIOPerf::GetXferSize,&IAIOPerf::SetXferSize)
				.def("SeqWrite",&IAIOPerf::SeqWrite,python::return_value_policy<python::reference_existing_object>())
				.def("SeqRead",&IAIOPerf::SeqRead,python::return_value_policy<python::reference_existing_object>())
				.def("RandWrite",&IAIOPerf::RandWrite,python::return_value_policy<python::reference_existing_object>())
				.def("RandRead",&IAIOPerf::RandRead,python::return_value_policy<python::reference_existing_object>());

}
