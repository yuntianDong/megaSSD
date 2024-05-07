/*
 * file : Wrapper.cpp
 */

#include <string>
#include <stdint.h>
#include <boost/python.hpp>

#include "IBlkDevMngr.h"
#include "Version.h"

using namespace std;
using namespace boost::python;

IBlkDevMngr * GetInstance(const char* devName)
{
	return new IBlkDevMngr(devName);
}

BOOST_PYTHON_MODULE(xBlkDev)
{
	namespace python = boost::python;

	WRAPPER_F_LIST

	def("GetBlkDevMngr",&GetInstance,python::return_value_policy<python::reference_existing_object>());

	class_<IBlkDevMngr>("IBlkDevMngr",
					init<const char*>())
					.add_property("TotalSize",&IBlkDevMngr::BlkGetTotalSize)
					.add_property("SoftBlockSize",&IBlkDevMngr::BlkGetSoftBlockSize)
					.add_property("LogBlockSize",&IBlkDevMngr::BlkGetLogBlockSize,&IBlkDevMngr::BlkSetLogBlockSize)
					.add_property("PhyBlockSize",&IBlkDevMngr::BlkGetPhyBlockSize)
					.add_property("IOMin",&IBlkDevMngr::BlkGetIOMin)
					.add_property("IOOpt",&IBlkDevMngr::BlkGetIOOpt)
					.add_property("AlignOffset",&IBlkDevMngr::BlkGetAlignOffset)
					.add_property("MaxSectors",&IBlkDevMngr::BlkGetMaxSectors)
					.add_property("DiskROFlag",&IBlkDevMngr::BlkGetDiskROFlag)
					.def("BlkDiscard",&IBlkDevMngr::BlkDiscard)
					.def("BlkDiscardWithSecu",&IBlkDevMngr::BlkDiscardWithSecu)
					.def("ZeroOut",&IBlkDevMngr::BlkZeroOut)
					.def("FlushBuf",&IBlkDevMngr::BlkFlushBuf);
}
