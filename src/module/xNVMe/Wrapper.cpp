/*
 * file : Wrapper.cpp
 */

#include <string>
#include <stdint.h>
#include <boost/python.hpp>

#include "INVMeDevice.h"
#include "Convert.h"
#include "device/nvme/NVMeManager.h"
#include "Version.h"

using namespace std;
using namespace boost::python;

BOOST_PYTHON_MODULE(xNVMe)
{
	namespace python = boost::python;

	WRAPPER_F_LIST

	enum_<_emDataXferMode>("emDataXferMode")
			.value("DX_PRP",DataXfer_PRP)
			.value("DX_SGL",DataXfer_SGL_MD_SINGLE)
			.value("DX_SGL_SEGMENT",DataXfer_SGL_MD_SEGMENT);

	enum_<_emFusedMode>("emFusedMode")
			.value("FUSED_NONE",Fused_None)
			.value("FUSED_1STCMD",Fused_1stCmd)
			.value("FUSED_2NDCMD",Fused_2ndCmd);


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

	class_<NVMeCommand>("NVMeCommand",init<Device*>())
					.add_property("DXferMode",&NVMeCommand::GetDataXferMode,&NVMeCommand::SetDataXferMode)
					.def("LoadPatternMngr",&NVMeCommand::LoadPatternMngr)
					.def("PatternMngrSwitch",&NVMeCommand::PatternMngrSwitch)
					.def("IsPatternMngrLoaded",&NVMeCommand::IsPatternMngrLoaded)
					.def("LoadBufferMngr",&NVMeCommand::LoadBufferMngr)
					.def("IsBufferMngrLoaded",&NVMeCommand::IsBufferMngrLoaded);

	class_<NVMeNVMCmd,bases<NVMeCommand> >("NVMeNVMCmd",init<Device*,uint32_t>())
					.add_property("FusedCmdEnabled",&NVMeNVMCmd::ClrFusedFlag,&NVMeNVMCmd::SetFusedFlag)
					.add_property("FusedMode",&NVMeNVMCmd::GetFusedMode)
					.def("LoadChildContext",&NVMeNVMCmd::LoadChildContext)
					.def("IsChildContextLoaded",&NVMeNVMCmd::IsChildContextLoaded)
					.def("UpdateFusedMode",&NVMeNVMCmd::UpdateFusedMode)
					.def("IoPassthrough",&NVMeNVMCmd::IoPassthrough,python::return_value_policy<python::manage_new_object>())
					.def("Compare",&NVMeNVMCmd::Compare,python::return_value_policy<python::manage_new_object>())
					.def("DataSetMngr",&NVMeNVMCmd::DataSetMngr,python::return_value_policy<python::manage_new_object>())
					.def("Flush",&NVMeNVMCmd::Flush,python::return_value_policy<python::manage_new_object>())
					.def("Read",&NVMeNVMCmd::Read,python::return_value_policy<python::manage_new_object>())
					.def("ResvAcquire",&NVMeNVMCmd::ResvAcquire,python::return_value_policy<python::manage_new_object>())
					.def("ResvRegister",&NVMeNVMCmd::ResvRegister,python::return_value_policy<python::manage_new_object>())
					.def("ResvRelease",&NVMeNVMCmd::ResvRelease,python::return_value_policy<python::manage_new_object>())
					.def("ResvReport",&NVMeNVMCmd::ResvReport,python::return_value_policy<python::manage_new_object>())
					.def("Write",&Write_Convert,python::return_value_policy<python::manage_new_object>())
					.def("WriteUNC",&NVMeNVMCmd::WriteUNC,python::return_value_policy<python::manage_new_object>())
					.def("WriteZeroes",&NVMeNVMCmd::WriteZeroes,python::return_value_policy<python::manage_new_object>());

	class_<NVMeAdminCmd,bases<NVMeCommand> >("NVMeAdminCmd",init<Device*>())
					.def("IsNSActive",&NVMeAdminCmd::IsNSActive)
					.def("LoadParentContext",&NVMeAdminCmd::LoadParentContext)
					.def("IsParentContextLoaded",&NVMeAdminCmd::IsParentContextLoaded)
					.def("AdminPassthrough",&NVMeAdminCmd::AdminPassthrough,python::return_value_policy<python::manage_new_object>())
					.def("Abort",&NVMeAdminCmd::Abort,python::return_value_policy<python::manage_new_object>())
					.def("AsyncEventReq",&NVMeAdminCmd::AsyncEventReq,python::return_value_policy<python::manage_new_object>())
					.def("CreateIOCQ",&NVMeAdminCmd::CreateIOCQ,python::return_value_policy<python::manage_new_object>())
					.def("CreateIOSQ",&NVMeAdminCmd::CreateIOSQ,python::return_value_policy<python::manage_new_object>())
					.def("DeleteIOCQ",&NVMeAdminCmd::DeleteIOCQ,python::return_value_policy<python::manage_new_object>())
					.def("DeleteIOSQ",&NVMeAdminCmd::DeleteIOSQ,python::return_value_policy<python::manage_new_object>())
					.def("DoorbellBufCfg",&NVMeAdminCmd::DoorbellBufCfg,python::return_value_policy<python::manage_new_object>())
					.def("DeviceSelfTest",&NVMeAdminCmd::DeviceSelfTest,python::return_value_policy<python::manage_new_object>())
					.def("DirectiveRecv",&NVMeAdminCmd::DirectiveRecv,python::return_value_policy<python::manage_new_object>())
					.def("DirectiveSend",&NVMeAdminCmd::DirectiveSend,python::return_value_policy<python::manage_new_object>())
					.def("FirmwareCommit",&NVMeAdminCmd::FirmwareCommit,python::return_value_policy<python::manage_new_object>())
					.def("FirmwareDownload",&NVMeAdminCmd::FirmwareDownload,python::return_value_policy<python::manage_new_object>())
					.def("GetFeatures",&NVMeAdminCmd::GetFeatures,python::return_value_policy<python::manage_new_object>())
					.def("GetLogPage",&NVMeAdminCmd::GetLogPage,python::return_value_policy<python::manage_new_object>())
					.def("Identify",&NVMeAdminCmd::Identify,python::return_value_policy<python::manage_new_object>())
					.def("KeepAlive",&NVMeAdminCmd::KeepAlive,python::return_value_policy<python::manage_new_object>())
					.def("NVMeMIRecv",&NVMeAdminCmd::NVMeMIRecv,python::return_value_policy<python::manage_new_object>())
					.def("NVMeMISend",&NVMeAdminCmd::NVMeMISend,python::return_value_policy<python::manage_new_object>())
					.def("NSAttach",&NVMeAdminCmd::NSAttach,python::return_value_policy<python::manage_new_object>())
					.def("NSMngmnt",&NVMeAdminCmd::NSMngmnt,python::return_value_policy<python::manage_new_object>())
					.def("SetFeatures",&NVMeAdminCmd::SetFeatures,python::return_value_policy<python::manage_new_object>())
					.def("VirtualMngr",&NVMeAdminCmd::VirtualMngr,python::return_value_policy<python::manage_new_object>())
					.def("FormatNVM",&NVMeAdminCmd::FormatNVM,python::return_value_policy<python::manage_new_object>())
					.def("Sanitize",&NVMeAdminCmd::Sanitize,python::return_value_policy<python::manage_new_object>())
					.def("SecurityRecv",&NVMeAdminCmd::SecurityRecv,python::return_value_policy<python::manage_new_object>())
					.def("SecuritySend",&NVMeAdminCmd::SecuritySend,python::return_value_policy<python::manage_new_object>());


	class_<INVMeDevice>("INVMeDevice")
					.def("GetCtrlr",&INVMeDevice::GetCtrlr,python::return_value_policy<python::manage_new_object>())
					.def("GetNS",&INVMeDevice::GetNS,python::return_value_policy<python::manage_new_object>())
					.def("ReleaseNS",&INVMeDevice::ReleaseNS);

	class_<NVMeManager>("NVMeManager",init<Device*>())
					.def("Reset",&NVMeManager::NVMeReset)
					.def("SubSysReset",&NVMeManager::NVMeSubSysReset);
}
