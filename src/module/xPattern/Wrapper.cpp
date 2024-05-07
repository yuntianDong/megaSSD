/*
 * file : Wrapper.cpp
 */

#include <string>
#include <stdint.h>
#include <boost/python.hpp>

#include "IPatternMngr.h"
#include "Version.h"

using namespace std;
using namespace boost::python;

BOOST_PYTHON_MODULE(xPattern)
{
	namespace python = boost::python;

	WRAPPER_F_LIST

	enum_<_stPatRecType>("stPatRecType")
			.value("PATREC_BITMAP",PATREC_BITMAP)
			.value("PATREC_CRC8",PATREC_CRC8);

	class_<IPatternMngr>("IPatternMngr",init<stPatRecType,bool,IHostBufMngr*,ParentContext*>())
					.def("UptPatRecFromIOWrBuf",&IPatternMngr::UptPatRecFromIOWrBuf)
					.def("CompareWithIORdBuf",&IPatternMngr::CompareWithIORdBuf)
					.def("FillIOWrBufFromPatRec",&IPatternMngr::FillIOWrBufFromPatRec)
					.def("DumpBitmapBuf",&IPatternMngr::DumpBitmapBuf)
					.def("ReleaseAllPattern",&IPatternMngr::ReleaseAllPattern)
					.def("GetPatternByIndex",&IPatternMngr::GetPatternByIndex)
					.def("GetIndexByPattern",&IPatternMngr::GetIndexByPattern)
					.def("IsValidIndex",&IPatternMngr::IsValidIndex)
					.def("SetIdxPattern",&IPatternMngr::SetIdxPattern)
					.def("CalcNVMe16BCRC",&IPatternMngr::CalcNVMe16BCRC)
					.def("CalcNVMe32BCRC",&IPatternMngr::CalcNVMe32BCRC)
					.def("CalcNVMe64BCRC",&IPatternMngr::CalcNVMe64BCRC);
}
