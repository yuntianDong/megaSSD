/*
 * file : Wrapper.cpp
 */

#include <string>
#include <stdint.h>
#include <boost/python.hpp>

#include "Convert.h"
#include "IHostBufMngr.h"
#include "Version.h"

using namespace std;
using namespace boost::python;

BOOST_PYTHON_MODULE(xBuffer)
{
	namespace python = boost::python;

	WRAPPER_F_LIST

	def("GetBufMngr",&IHostBufMngr::GetInstance,python::return_value_policy<python::reference_existing_object>());

	enum_<_enBufType>("enBufType")
			.value("SYSBUF",BUF_T_SYS)
			.value("IOBUF",BUF_T_IO)
			.value("PATIOBUF",BUF_T_PATIO)
			.value("PATRECBUF",BUF_T_PAT_REC);

	enum_<_enEndianMode>("enEndianMode")
			.value("BigEndian",BUFV_BIG_ENDIAN)
			.value("LittleEndian",BUFV_LITTLE_ENDIAN)
			.value("BE",BUFV_BIG_ENDIAN)
			.value("LE",BUFV_LITTLE_ENDIAN);

	class_<Buffers>("Buffers",
			init<uint64_t,uint16_t,const char*>())
					.def("crc8",&Buffers::GenCRC8)
					.def("crc16",&Buffers::GenCRC16)
					.def("crc32",&Buffers::GenCRC32)
					.def("Dump",&Buffers::Dump)
					.def("GetnBytes",&buffers_getnbytes)
					.def("GetBufAddr",&Buffers::GetBufferAddr)
					.def("GetBufSize",&Buffers::GetBufSize)
					.def("GetBufAlignment",&Buffers::GetBufAlignment)
					.def("SetEndianMode",&Buffers::SetEndianMode)
					.def("GetEndianMode",&Buffers::GetEndianMode)
					.def("GetByte",&Buffers::GetByte)
					.def("GetWord",&Buffers::GetWord)
					.def("GetDWord",&Buffers::GetDWord)
					.def("GetQWord",&Buffers::GetQWord)
					.def("SetByte",&Buffers::SetByte)
					.def("SetWord",&Buffers::SetWord)
					.def("SetDWord",&Buffers::SetDWord)
					.def("SetQWord",&Buffers::SetQWord)
					.def("FillZero",&Buffers::FillZero)
					.def("FillHexF",&Buffers::FillHexF)
					.def("FillBytePat",&Buffers::FillWithBytePat)
					.def("FillWordPat",&Buffers::FillWithWordPat)
					.def("FillDWordPat",&Buffers::FillWithDWordPat)
					.def("FillQWordPat",&Buffers::FillWithQWordPat)
					.def("__getitem__",(uint8_t (*)(Buffers &,uint32_t))&buffers_getitem)
					.def("__setitem__",(void (*)(Buffers &,uint32_t,uint8_t))&buffers_setitem)
					.def("__len__",&buffers_len)
#if(PYTHON_VER >= 300)
					.def("__getitem__",(python::list (*)(Buffers &,slice))&buffers_getitem)
					.def("__setitem__",(void (*)(Buffers &,slice,python::list))&buffers_setitem)
#else
					// __getslice__ and __setslice__ is retired from python3.x
					.def("__getslice__",&buffers_getslice)
					.def("__setslice__",&buffers_setslice)
#endif
					.def("WriteRegVal",&Buffers::WriteRegVal)
					.def("CopyFromBuff",&Buffers::CopyFromBuff)
					.def("CopyToBuff",&Buffers::CopyToBuff)
					.def("ToFile",&Buffers::ToFile)
					.def("FromFile",&Buffers::FromFile)
					.def("ReadRegVal",&buffers_ReadRegVal);

	class_<HostBuffer,bases<Buffers> >("HostBuffer",
			init<uint64_t,const char*>())
					.def("GetHostBufSize",&HostBuffer::GetHostBufSize)
					.def("GetHostBufAddr",&HostBuffer::GetHostBufAddr)
					.def("GetHostBufAlign",&HostBuffer::GetHostBufAlign)
					.def("Compare",&buffers_Compare);

	class_<CommandBuf,bases<HostBuffer> >("CommandBuf",init<uint32_t,const char*>());

	class_<SectorBuf,bases<HostBuffer> >("SectorBuf",
			init<uint64_t,uint32_t,const char*>())
					.add_property("SectorSize",&SectorBuf::GetSectorSize)
					.def("FillSector",&SectorBuf::FillSector)
					.def("MarkAddrOverlay",&SectorBuf::MarkAddrOverlay)
					.def("IsSectorBuf",&SectorBuf::IsSectorBuf);

	class_<PatternBuf,bases<SectorBuf> >("PatternBuf",init<uint64_t,uint32_t,const char*>());

	class_<IHostBufMngr>("IHostBufMngr")
					.add_property("SectorSize",&IHostBufMngr::GetSectorSize,&IHostBufMngr::SetSectorSize)
					.add_property("IOBufAllocBytes",&IHostBufMngr::GetIOBufAllocBytes)
					.add_property("SysBufAllocBytes",&IHostBufMngr::GetSysBufAllocBytes)
					.add_property("FrmBufAllocBytes",&IHostBufMngr::GetFrmBufAllocBytes)
					.add_property("IOBufType",&IHostBufMngr::GetIOBufType,&IHostBufMngr::SetIOBufType)
					.add_property("MetaBufType",&IHostBufMngr::GetMetaBufType,&IHostBufMngr::SetMetaBufType)
					.add_property("SysBufType",&IHostBufMngr::GetSysBufType,&IHostBufMngr::SetSysBufType)
					.add_property("FrmBufType",&IHostBufMngr::GetFrmBufType,&IHostBufMngr::SetFrmBufType)
					.add_property("IOBufItemSize",&IHostBufMngr::GetIOBufItemSize,&IHostBufMngr::SetIOBufItemSize)
					.add_property("IOBufMaxSize",&IHostBufMngr::GetIOBufMaxSize,&IHostBufMngr::SetIOBufMaxSize)
					.add_property("MetaBufItemSize",&IHostBufMngr::GetMetaBufItemSize,&IHostBufMngr::SetMetaBufItemSize)
					.add_property("SysBufMaxSize",&IHostBufMngr::GetSysBufMaxSize,&IHostBufMngr::SetSysBufMaxSize)
					.add_property("FrmBufMaxSize",&IHostBufMngr::GetFrmBufMaxSize,&IHostBufMngr::SetFrmBufMaxSize)
					.def("DumpIOBufMngr",&IHostBufMngr::DumpIOBufMngr)
					.def("DumpSysBufMngr",&IHostBufMngr::DumpSysBufMngr)
					.def("DumpFrmkBufMngr",&IHostBufMngr::DumpFrmkBufMngr)
					.def("GetIORdBuf",&IHostBufMngr::GetIORdBuf,python::return_value_policy<python::reference_existing_object>())
					.def("GetIOWrBuf",&IHostBufMngr::GetIOWrBuf,python::return_value_policy<python::reference_existing_object>())
					.def("GetMetaRdBuf",&IHostBufMngr::GetMetaRdBuf,python::return_value_policy<python::reference_existing_object>())
					.def("GetMetaWrBuf",&IHostBufMngr::GetMetaWrBuf,python::return_value_policy<python::reference_existing_object>())
					.def("GetSysBuf",&IHostBufMngr::GetSysBuf,python::return_value_policy<python::reference_existing_object>())
					.def("GetUsrBuf",&IHostBufMngr::GetUsrBuf,python::return_value_policy<python::reference_existing_object>())
					.def("GetFrmwkBuf",&IHostBufMngr::GetFrmwkBuf,python::return_value_policy<python::reference_existing_object>());
}
