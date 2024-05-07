/*
 * file : Wrapper.cpp
 */
#include <boost/python.hpp>
#include "IHostDevice.h"
#include "Version.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(xDevice)
{
    namespace python = boost::python;

    WRAPPER_F_LIST
    
    class_<Device>("Device",init<Operator*>())
    .add_property("bdf", &Device::GetBdf)
    .add_property("bdfID", &Device::GetBdfID)
    .add_property("driver", &Device::GetDriver);

    class_<IHostDevice>("IHostDevice",init<const char*>())
    .def("GetNVMeDevice", &IHostDevice::GetNVMeDevice,return_value_policy<reference_existing_object>())
    .def("GetPcieDevice", &IHostDevice::GetPcieDevice,return_value_policy<reference_existing_object>())
    .def("CloseDev", &IHostDevice::CloseDev)
    .def("RestoreDev", &IHostDevice::RestoreDev,return_value_policy<reference_existing_object>())
    .add_property("NodeCnt",&IHostDevice::GetDevCnt);
}