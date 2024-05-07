#ifndef _CONVERT_NVME_COMMAND_H
#define _CONVERT_NVME_COMMAND_H

#include <boost/python.hpp>

#include "INVMeDevice.h"

namespace py = boost::python;

NVMeCmdResp* Write_Convert(NVMeNVMCmd& obj,uint64_t slba,uint16_t length,uint8_t prinfo,\
                    uint16_t lbat,uint16_t lbatm,uint64_t lbst,uint64_t ilbrf,\
                    Buffers* dBuf,Buffers* mBuf,uint32_t timeout,py::dict kwargs = py::dict());

#endif