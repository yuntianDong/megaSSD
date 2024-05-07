#ifndef _WRAP_BUFFERS_H
#define _WRAP_BUFFERS_H

#define PYTHON_VER			305			// V3.5
//#define PYTHON_VER			207

#include <stdint.h>
#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/slice.hpp>
#include <boost/python/errors.hpp>
#include <vector>

#include "Logging.h"
#include "buffer/HostBuffer.h"

using namespace std;
using namespace boost::python;

/*
 * Buffers
 */
void buffers_setitem(Buffers & buf,uint32_t index,uint8_t val);
uint8_t buffers_getitem(Buffers & buf,uint32_t index);
uint32_t buffers_len(Buffers & buf);
boost::python::list buffers_Compare(Buffers& buf,Buffers& dstBuf,uint32_t desOffset,uint32_t offset,uint32_t length);
uint64_t buffers_ReadRegVal(Buffers& buf,uint32_t startByte,uint8_t startBits,uint8_t bitsLen);

boost::python::list buffers_getnbytes(Buffers & buf,uint32_t stByte,uint32_t length);
#if(PYTHON_VER >= 300)
void buffers_setitem(Buffers & buf,slice index,boost::python::list vals);
boost::python::list buffers_getitem(Buffers & buf,slice index);
#else
// __getslice__ and __setslice__ is retired from python3.x
boost::python::list buffers_getslice(Buffers & buf,uint32_t from,uint32_t to);
void buffers_setslice(Buffers & buf,uint32_t from,uint32_t to,boost::python::list val);
#endif

#endif
