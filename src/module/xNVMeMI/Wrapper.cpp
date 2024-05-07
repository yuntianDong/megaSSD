/*
 *  * file : Wrapper.cpp
 *   */

#include <string>
#include <stdint.h>
#include <boost/python.hpp>

#include "nvme-mi/NVMeMICommand.h"
#include "Version.h"

using namespace std;
using namespace boost::python;

BOOST_PYTHON_MODULE(xNVMeMI)
{
	namespace python = boost::python;

	WRAPPER_F_LIST

	class_<RegisterMap>("RegisterMap",
			init<uint32_t,bool>())
			.def("Debug",&RegisterMap::Debug);

	class_<NVMeCmdResp>("NVMeCmdResp")
					.def("Succeeded",&NVMeCmdResp::Succeeded)
					.def("GetDataBuffer",&NVMeCmdResp::GetDataBuffer,python::return_value_policy<python::reference_existing_object>())
					.def("GetMetaBuffer",&NVMeCmdResp::GetMetaBuffer,python::return_value_policy<python::reference_existing_object>())
					.def("Dump",&NVMeCmdResp::Dump)
					.def("DumpHex",&NVMeCmdResp::DumpHex)
					.def("GetExecTime",&NVMeCmdResp::GetExecTime)
					.def("GetMFlag",&NVMeCmdResp::GetMFlag)
					.def("GetDNRFlag",&NVMeCmdResp::GetDNRFlag)
					.def("GetSC",&NVMeCmdResp::GetSC)
					.def("GetSCT",&NVMeCmdResp::GetSCT)
					.def("GetRespVal",&NVMeCmdResp::GetRespVal)
					.def("GetDW1",&NVMeCmdResp::GetDW1)
					.def("GetSQID",&NVMeCmdResp::GetSQID)
					.def("GetSQHP",&NVMeCmdResp::GetSQHP)
					.def("GetStatus",&NVMeCmdResp::GetStatus)
					.def("GetCID",&NVMeCmdResp::GetCID)
					.def("GetPhaseTag",&NVMeCmdResp::GetPhaseTag);

	class_<MCTPNVMeMIMsgRegMap,bases<RegisterMap> >("MCTPNVMeMIMsgRegMap")
			.add_property("MessageType",&MCTPNVMeMIMsgRegMap::GetMessageType,&MCTPNVMeMIMsgRegMap::SetMessageType)
			.add_property("IC",&MCTPNVMeMIMsgRegMap::GetIC,&MCTPNVMeMIMsgRegMap::SetIC)
			.add_property("CSI",&MCTPNVMeMIMsgRegMap::GetCSI,&MCTPNVMeMIMsgRegMap::SetCSI)
			.add_property("Reserved",&MCTPNVMeMIMsgRegMap::GetReserved,&MCTPNVMeMIMsgRegMap::SetReserved)
			.add_property("NVMeMIMsgType",&MCTPNVMeMIMsgRegMap::GetNVMeMIMsgType,&MCTPNVMeMIMsgRegMap::SetNVMeMIMsgType)
			.add_property("ROR",&MCTPNVMeMIMsgRegMap::GetROR,&MCTPNVMeMIMsgRegMap::SetROR)
			.add_property("Reserved2",&MCTPNVMeMIMsgRegMap::GetReserved2,&MCTPNVMeMIMsgRegMap::SetReserved2)
			.add_property("CRC",&MCTPNVMeMIMsgRegMap::GetCRC,&MCTPNVMeMIMsgRegMap::SetCRC)
			.def("SetMessageData",&MCTPNVMeMIMsgRegMap::SetMessageData)
			.def("GetMessageData",&MCTPNVMeMIMsgRegMap::GetMessageData);

	class_<NVMeMIPrimResp,bases<MCTPNVMeMIMsgRegMap> >("NVMeMIPrimResp")
			.add_property("Status",&NVMeMIPrimResp::GetStatus,&NVMeMIPrimResp::SetStatus)
			.add_property("Tag",&NVMeMIPrimResp::GetTag,&NVMeMIPrimResp::SetTag)
			.add_property("CPSR",&NVMeMIPrimResp::GetCPSR,&NVMeMIPrimResp::SetCPSR);

	class_<NVMeMICmdResp,bases<MCTPNVMeMIMsgRegMap> >("NVMeMICmdResp")
			.add_property("Status",&NVMeMICmdResp::GetStatus,&NVMeMICmdResp::SetStatus)
			.add_property("MR",&NVMeMICmdResp::GetNVMeMR,&NVMeMICmdResp::SetNVMeMR)
			.add_property("MRB1",&NVMeMICmdResp::GetNVMeMRB1,&NVMeMICmdResp::SetNVMeMRB1)
			.add_property("MRB2",&NVMeMICmdResp::GetNVMeMRB2,&NVMeMICmdResp::SetNVMeMRB2)
			.add_property("MRB3",&NVMeMICmdResp::GetNVMeMRB3,&NVMeMICmdResp::SetNVMeMRB3)
			.def("SetResponseData",&NVMeMICmdResp::SetResponseData)
			.def("GetResponseData",&NVMeMICmdResp::GetResponseData);

	class_<NVMeMIBpDriver, boost::noncopyable>("NVMeMIBpDriver");

	class_<DummyBPDriver,bases<NVMeMIBpDriver> >("DummyBPDriver")
			.def("Debug",&DummyBPDriver::Debug);

	class_<MCUPwrBPDriver,bases<NVMeMIBpDriver>,boost::noncopyable >("MCUPwrBPDriver",
			init<const char*,uint8_t>());

	class_<NVMeMiCommand>("NVMeMiCommand",
			init<>())
				.add_property("SrcI2CAddr",&NVMeMiCommand::GetSrcI2CAddr,&NVMeMiCommand::SetSrcI2CAddr)
				.add_property("DstI2CAddr",&NVMeMiCommand::GetDstI2CAddr,&NVMeMiCommand::SetDstI2CAddr)
				.add_property("SrcEPID",&NVMeMiCommand::GetSrcEndPointID,&NVMeMiCommand::SetSrcEndPointID)
				.add_property("DstEPID",&NVMeMiCommand::GetDstEndPointID,&NVMeMiCommand::SetDstEndPointID)
				.add_property("MCTPUnitSize",&NVMeMiCommand::GetMCTPUnitSize,&NVMeMiCommand::SetMCTPUnitSize)
				.add_property("AutoClear",&NVMeMiCommand::GetAutoClear,&NVMeMiCommand::SetAutoClear)
				.add_property("CSI",&NVMeMiCommand::GetCmdSlotID,&NVMeMiCommand::SetCmdSlotID)
				.def("SetBPDriver",&NVMeMiCommand::SetBPDriver)
				.def("InjectError",&NVMeMiCommand::InjectError)
				.def("ClearInjectErr",&NVMeMiCommand::ClearInjectErr)
				.def("MIPrimGetState",&NVMeMiCommand::MIPrimGetStatus,python::return_value_policy<python::reference_existing_object>())
				.def("MIPrimPause",&NVMeMiCommand::MIPrimPause,python::return_value_policy<python::reference_existing_object>())
				.def("MIPrimResume",&NVMeMiCommand::MIPrimResume,python::return_value_policy<python::reference_existing_object>())
				.def("MIPrimAbort",&NVMeMiCommand::MIPrimAbort,python::return_value_policy<python::reference_existing_object>())
				.def("MIPrimReplay",&NVMeMiCommand::MIPrimReplay,python::return_value_policy<python::reference_existing_object>())
				.def("MICmdConfigGet",&NVMeMiCommand::MICmdConfigGet,python::return_value_policy<python::reference_existing_object>())
				.def("MICmdConfigSet",&NVMeMiCommand::MICmdConfigSet,python::return_value_policy<python::reference_existing_object>())
				.def("MICmdCtrlHSP",&NVMeMiCommand::MICmdCtrlHSP,python::return_value_policy<python::reference_existing_object>())
				.def("MICmdNVMSHSP",&NVMeMiCommand::MICmdNVMSHSP,python::return_value_policy<python::reference_existing_object>())
				.def("MICmdReadData",&NVMeMiCommand::MICmdReadData,python::return_value_policy<python::reference_existing_object>())
				.def("MICmdReset",&NVMeMiCommand::MICmdReset,python::return_value_policy<python::reference_existing_object>())
				.def("MICmdVPDRead",&NVMeMiCommand::MICmdVPDRead,python::return_value_policy<python::reference_existing_object>())
				.def("MICmdVPDWrite",&NVMeMiCommand::MICmdVPDWrite,python::return_value_policy<python::reference_existing_object>())
				.def("DeviceSelfTest",&NVMeMiCommand::DeviceSelfTest,python::return_value_policy<python::reference_existing_object>())
				.def("FirmwareCommit",&NVMeMiCommand::FirmwareCommit,python::return_value_policy<python::reference_existing_object>())
				.def("FirmwareDownload",&NVMeMiCommand::FirmwareDownload,python::return_value_policy<python::reference_existing_object>())
				.def("GetFeatures",&NVMeMiCommand::GetFeatures,python::return_value_policy<python::reference_existing_object>())
				.def("GetLogPage",&NVMeMiCommand::GetLogPage,python::return_value_policy<python::reference_existing_object>())
				.def("Identify",&NVMeMiCommand::Identify,python::return_value_policy<python::reference_existing_object>())
				.def("NSAttach",&NVMeMiCommand::NSAttach,python::return_value_policy<python::reference_existing_object>())
				.def("NSMngmnt",&NVMeMiCommand::NSMngmnt,python::return_value_policy<python::reference_existing_object>())
				.def("SetFeatures",&NVMeMiCommand::SetFeatures,python::return_value_policy<python::reference_existing_object>())
				.def("FormatNVM",&NVMeMiCommand::FormatNVM,python::return_value_policy<python::reference_existing_object>())
				.def("Sanitize",&NVMeMiCommand::Sanitize,python::return_value_policy<python::reference_existing_object>())
				.def("SecurityRecv",&NVMeMiCommand::SecurityRecv,python::return_value_policy<python::reference_existing_object>())
				.def("SecuritySend",&NVMeMiCommand::SecuritySend,python::return_value_policy<python::reference_existing_object>());
}

