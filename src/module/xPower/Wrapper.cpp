/*
 *  * file : Wrapper.cpp
 *   */

#include <string>
#include <stdint.h>
#include <boost/python.hpp>

#include "IPCPwrMngr.h"
#include "Convert.h"
#include "Version.h"

using namespace std;
using namespace boost::python;

BOOST_PYTHON_MODULE(xPower)
{
	namespace python = boost::python;

	WRAPPER_F_LIST

	enum_<_enAsyncFunc>("enAsyncFunc")
			.value("ASF_POW_ON",ASF_POW_ON)
			.value("ASF_POW_OFF",ASF_POW_OFF)
			.value("B2B_POW_CYC",ASF_B2B_POW_CYC);

	enum_<_enPwrCtrlrType>("enPwrCtrlrType")
			.value("PCT_MCU",PCT_MCU)
			.value("PCT_SCPI",PCT_SCPI);

	class_<MCUPwrCtrlr, boost::noncopyable>("MCUPwrCtrlr",
			init<const char*>())
					.add_property("Version",&MCUPwrCtrlr::GetVersion)
					.def("PowerOn",&MCUPwrCtrlr::PowerOn)
					.def("PowerOff",&MCUPwrCtrlr::PowerOff)
					.def("GetVoltage",&MCUPwrCtrlr::GetVoltage)
					.def("GetCurrent",&MCUPwrCtrlr::GetCurrent)
					.def("GetPower",&MCUPwrCtrlr::GetPower)
					.def("GetTemp",&MCUPwrCtrlr::GetTemp)
					.def("GetPowerStatus",&MCUPwrCtrlr::GetPowerStatus)
					.def("DetectDevice",&MCUPwrCtrlr::DetectDevice)
					.def("GetPowerID",&MCUPwrCtrlr_GetPowerID)
					.def("SetPowerID",&MCUPwrCtrlr_SetPowerID)
					.def("WrEEPROM",&MCUPwrCtrlr_WrEEPROM)
					.def("RdEEPROM",&MCUPwrCtrlr_RdEEPROM)
					.def("NVMeMIStart",&MCUPwrCtrlr::NVMeMIStart)
					.def("NVMeMIDXfer",&MCUPwrCtrlr_NVMeMIDXfer)
					.def("NVMeMIReady",&MCUPwrCtrlr_NVMeMIReady)
					.def("NVMeMIDRcvr",&MCUPwrCtrlr_NVMeMIDRcvr)
					.def("UpdateFW",&MCUPwrCtrlr::UpdateFW)
					.def("DownloadFW",&MCUPwrCtrlr_DownloadFW)
					.def("Reset",&MCUPwrCtrlr::Reset);

	class_<SCPIPwrCtrlr, boost::noncopyable>("SCPIPwrCtrlr",
			init<const char*>())
					.add_property("Version",&SCPIPwrCtrlr::GetVersion)
					.def("PowerOn",&SCPIPwrCtrlr::PowerOn)
					.def("PowerOff",&SCPIPwrCtrlr::PowerOff)
					.def("GetPowerStatus",&SCPIPwrCtrlr::GetPowerStatus)
					.def("GetVoltage",&SCPIPwrCtrlr::GetVoltage)
					.def("GetCurrent",&SCPIPwrCtrlr::GetCurrent)
					.def("GetPower",&SCPIPwrCtrlr::GetPower);

	class_<MsgProcCommServer>("MsgProcCommServer",
			init<const char*,uint32_t,uint16_t>())
					.def("Run",&MsgProcCommServer::Run);

	class_<MsgProcCommClient>("MsgProcCommClient",
			init<const char*,uint16_t>())
					.def("CloseService",&MsgProcCommClient::CloseService)
					.def("DoSelfTest",&MsgProcCommClient::DoSelfTest);

	class_<PwrMngrServer,bases<MsgProcCommServer> >("PwrMngrServer",
			init<enPwrCtrlrType,const char*>());

	class_<PwrMngrClient,bases<MsgProcCommClient> >("PwrMngrClient",
			init<uint16_t>())
					.add_property("Voltage",&PwrMngrClient::GetVoltage)
					.add_property("Current",&PwrMngrClient::GetCurrent)
					.add_property("Power",&PwrMngrClient::GetPower)
					.add_property("IsPwrOn",&PwrMngrClient::IsPoweredOn)
					.add_property("Version",&PwrMngrClient::GetVersion)
					.def("AsyncPowerOn",&PwrMngrClient::AsyncPowerOn)
					.def("AsyncPowerOff",&PwrMngrClient::AsyncPowerOff)
					.def("AsyncB2BPowerCycle",&PwrMngrClient::AsyncB2BPowerCycle)
					.def("WaitForDone",&PwrMngrClient::WaitForDone)
					.def("PowerOn",&PwrMngrClient::PowerOn)
					.def("PowerOff",&PwrMngrClient::PowerOff);

	class_<SCPIPwrMngrServer,bases<PwrMngrServer> >("SCPIPwrMngrServer",
			init<>())
					.def(init<const char*>())
					.def("AddPwrCtrlr",&SCPIPwrMngrServer::AddPwrCtrlr);

	class_<SCPIPwrMngrClient,bases<PwrMngrClient> >("SCPIPwrMngrClient",
			init<uint16_t>());

	class_<MCUPwrMngrServer,bases<PwrMngrServer> >("MCUPwrMngrServer",
			init<>())
					.def(init<const char*>())
					.def("AddPwrCtrlr",&MCUPwrMngrServer::AddPwrCtrlr);

	class_<MCUPwrMngrClient,bases<PwrMngrClient> >("MCUPwrMngrClient",
			init<uint16_t>())
					.add_property("Temp",&MCUPwrMngrClient::GetTemp)
					.add_property("bDetected",&MCUPwrMngrClient::DetectDevice)
					.def("GetPowerID",&MCUPwrMngrClient_GetPowerID)
					.def("SetPowerID",&MCUPwrMngrClient_SetPowerID)
					.def("WrEEPROM",&MCUPwrMngrClient_WrEEPROM)
					.def("RdEEPROM",&MCUPwrMngrClient_RdEEPROM)
					.def("NVMeMIStart",&MCUPwrMngrClient::NVMeMIStart)
					.def("NVMeMIDXfer",&MCUPwrMngrClient_NVMeMIDXfer)
					.def("NVMeMIReady",&MCUPwrMngrClient_NVMeMIReady)
					.def("NVMeMIDRcvr",&MCUPwrMngrClient_NVMeMIDRcvr)
					.def("UpdateFW",&MCUPwrMngrClient::UpdateFW)
					.def("DownloadFW",&MCUPwrMngrClient_DownloadFW)
					.def("Reset",&MCUPwrMngrClient::Reset);
}

