/*
 * file : Wrapper.cpp
 */

#include <string>
#include <stdint.h>
#include <boost/python.hpp>

#include "RPCXBrain.h"
#include "Version.h"

using namespace std;
using namespace boost::python;

boost::python::list GetAvailSlotID(IPCTaskMngrClient& client)
{
	boost::python::list list;

	uint8_t sIDLst[MAX_SLOT_NUM];
	uint8_t	cnt	= client.GetAvailSlotID(sIDLst,MAX_SLOT_NUM);

	for(uint8_t idx=0;idx<cnt;idx++)
	{
		list.append(sIDLst[idx]);
	}

	return list;
}

BOOST_PYTHON_MODULE(xBrain)
{
	namespace python = boost::python;

	WRAPPER_F_LIST

	bool (TaskDesc::*ac1)(enCondKey,const char*,bool)	= &TaskDesc::AddCondition;
	bool (TaskDesc::*ac2)(enCondKey,uint32_t,bool)		= &TaskDesc::AddCondition;

	def("GetDutMngrServer",&GetDutMngrServer,python::return_value_policy<python::reference_existing_object>());
	def("GetTaskMngrServer",&IPCTaskMngrServer::GetInstance,python::return_value_policy<python::reference_existing_object>());
	def("GetTaskMngrClient",&IPCTaskMngrClient::GetInstance,python::return_value_policy<python::reference_existing_object>());

	enum_<_enDutStatus>("enDutStatus")
			.value("IDLE",DUT_S_IDLE)
			.value("BUSY",DUT_S_BUSY)
			.value("ERROR",DUT_S_ERROR)
			.value("UNKNOWN",DUT_S_UNKNOWN);

	enum_<_enCondKey>("enCondKey")
			.value("SN",CK_SN)
			.value("MN",CK_MN)
			.value("FR",CK_FR)
			.value("GR",CK_GR)
			.value("GB",CK_GB)
			.value("SS",CK_SS);

	class_<DutInfo>("DutInfo")
			.add_property("DevName",&DutInfo::GetDevName)
			.add_property("PhyAddr",&DutInfo::GetPhyAddr)
			.add_property("SN",&DutInfo::GetDutSN)
			.add_property("MN",&DutInfo::GetDutMN)
			.add_property("FWRev",&DutInfo::GetDutFWRev)
			.add_property("GitRev",&DutInfo::GetDutGitRev)
			.add_property("Capacity",&DutInfo::GetDutCapGB)
			.add_property("SectorSize",&DutInfo::GetDutSectorSize);

	class_<TaskDesc>("TaskDesc")
			.add_property("index",&TaskDesc::GetTaskIndex,&TaskDesc::SetTaskIndex)
			.add_property("name",&TaskDesc::GetTaskName,&TaskDesc::SetTaskName)
			.add_property("cmdline",&TaskDesc::GetCmdLine,&TaskDesc::SetTaskCmdline)
			.add_property("timeout",&TaskDesc::GetTimeOut,&TaskDesc::SetTimeout)
			.add_property("loopcnt",&TaskDesc::GetLoopCount,&TaskDesc::SetLoopCnt)
			.def("Dump",&TaskDesc::Dump)
			.def("Reset",&TaskDesc::Reset)
			.def("AddCondition",ac1)
			.def("AddCondition",ac2)
			.def("IsMatchedDut",&TaskDesc::IsMatchedDut);

	class_<TaskResult>("TaskResult")
			.add_property("valid",&TaskResult::IsValid)
			.add_property("index",&TaskResult::GetRsltIndex,&TaskResult::SetRsltIndex)
			.add_property("result",&TaskResult::GetTestResult,&TaskResult::SetTestResult)
			.add_property("execTime",&TaskResult::GetTestExecTime,&TaskResult::SetTestExecTime)
			.add_property("phyAddr",&TaskResult::GetTargetPhyAddr,&TaskResult::SetTargetPhyAddr)
			.add_property("slotID",&TaskResult::GetSlotID,&TaskResult::SetSlotID);

	class_<MsgProcCommServer>("MsgProcCommServer",
			init<const char*,uint32_t,uint16_t>())
					.def("Run",&MsgProcCommServer::Run);

	class_<MsgProcCommClient>("MsgProcCommClient",
			init<const char*,uint16_t>())
					.add_property("IsGood",&MsgProcCommClient::IsGood)
					.def("CloseService",&MsgProcCommClient::CloseService)
					.def("DoSelfTest",&MsgProcCommClient::DoSelfTest);

	class_< IPCDUTMngrServer,bases<MsgProcCommServer> >("IPCDUTMngrServer",
			init<const char*,uint8_t>());

	class_< IPCDUTMngrClient,bases<MsgProcCommClient> >("IPCDUTMngrClient",
			init<uint8_t,uint16_t>())
					.def("GetDevInfo",&IPCDUTMngrClient::GetDutInfo,python::return_value_policy<python::manage_new_object>())
					.def("GetDevStatus",&IPCDUTMngrClient::GetDutStatus)
					.def("Identify",&IPCDUTMngrClient::Identify)
					.def("ExecTask",&IPCDUTMngrClient::ExecTask)
					.def("WaitTaskDone",&IPCDUTMngrClient::WaitTaskDone);

	class_< IPCTaskMngrServer,bases<MsgProcCommServer> >("IPCTaskMngrServer",
			init<uint8_t>())
					.def("Reset",&IPCTaskMngrServer::Reset);

	class_< IPCTaskMngrClient,bases<MsgProcCommClient> >("IPCTaskMngrClient",
			init<uint8_t,uint8_t>())
					.def("DebugDump",&IPCTaskMngrClient::DebugDump)
					.def("Reset",&IPCTaskMngrClient::Reset)
					.def("AddTask",&IPCTaskMngrClient::AddTask)
					.def("GetPeningTaskCnt",&IPCTaskMngrClient::GetPeningTaskCnt)
					.def("PrintTestReport",&IPCTaskMngrClient::PrintTestReport)
					.def("DumpTestReport",&IPCTaskMngrClient::DumpTestReport)
					.def("PrintTestResult",&IPCTaskMngrClient::PrintTestResult)
					.def("DumpTestResult",&IPCTaskMngrClient::DumpTestResult)
					.def("AddSlot",&IPCTaskMngrClient::AddSlot)
					.def("DelSlot",&IPCTaskMngrClient::DelSlot)
					.def("GetSlotInfo",&IPCTaskMngrClient::GetSlotInfo,python::return_value_policy<python::manage_new_object>())
					.def("GetSlotStatus",&IPCTaskMngrClient::GetSlotStatus)
					.def("IdentifySlot",&IPCTaskMngrClient::IdentifySlot)
					.def("IdfyAllSlots",&IPCTaskMngrClient::IdfyAllSlots)
					.def("CheckSlotAvail",&IPCTaskMngrClient::CheckSlotAvail)
					.def("GetAvailSlotID",&GetAvailSlotID);

	class_< NanoJsonRPCServer >("NanoJsonRPCServer",
			init<const char*>())
					.def("Run",&NanoJsonRPCServer::Run);

	class_< NanoJsonRPCClient >("NanoJsonRPCClient",
			init<const char*>())
					.add_property("Version",&NanoJsonRPCClient::GetVersion)
					.def("CloseService",&NanoJsonRPCClient::CloseService);

	class_< RPCTaskMngrServer,bases<NanoJsonRPCServer> >("RPCTaskMngrServer",
			init<const char*>())
					.def("AddTask",&RPCTaskMngrServer::AddTask)
					.def("AddRslt",&RPCTaskMngrServer::AddRslt)
					.def("Reset",&RPCTaskMngrServer::Reset);

	class_< RPCTaskMngrClient,bases<NanoJsonRPCClient> >("RPCTaskMngrClient",
			init<const char*>())
					.add_property("TotalCount",&RPCTaskMngrClient::GetTotalCount)
					.add_property("LeftCount",&RPCTaskMngrClient::GetLeftCount)
					.def("DebugDump",&RPCTaskMngrClient::DebugDump)
					.def("DumpResult",&RPCTaskMngrClient::DumpTestResult)
					.def("AddTaskDesc",&RPCTaskMngrClient::AddTaskDesc)
					.def("GetTaskDesc",&RPCTaskMngrClient::GetTaskDesc)
					.def("UptTaskRslt",&RPCTaskMngrClient::UptTaskRslt)
					.def("GetTaskRslt",&RPCTaskMngrClient::GetTaskRslt);

	class_< RPCXBrainServer,bases<RPCTaskMngrServer> >("RPCXBrainServer",
			init<const char*>());

	class_< RPCXBrainClient,bases<RPCTaskMngrClient,IPCTaskMngrServer> >("RPCXBrainClient",
			init<const char*,uint8_t>())
					.def("AddSlot",&RPCXBrainClient::AddSlot)
					.def("SetHostID",&RPCXBrainClient::SetHostID);
}

