/*
 * Convert.cpp
 */

#include "Convert.h"

#define ERROR_RTN_VAL		0x1234567890ABCDEF

void buffers_setitem(Buffers & buf,uint32_t index,uint8_t val)
{
	if(buf.GetBufSize() <= index)
	{
		PyErr_SetString(PyExc_IndexError, "index out of range");
		throw boost::python::error_already_set();
	}

	buf.SetByte(index,val);
}

uint8_t buffers_getitem(Buffers & buf,uint32_t index)
{
	if(buf.GetBufSize() <= index)
	{
		PyErr_SetString(PyExc_IndexError, "index out of range");
		throw boost::python::error_already_set();
	}

	return buf.GetByte(index);
}

uint32_t buffers_len(Buffers & buf)
{
	return buf.GetBufSize();
}

boost::python::list buffers_getnbytes(Buffers & buf,uint32_t stByte,uint32_t length)
{
    boost::python::list list;
    while (0 < length--) list.append(buf.GetByte(stByte++));
    return list;
}

#if(PYTHON_VER >= 300)
boost::python::list buffers_getitem(Buffers & buf,slice index)
{
	slice::range<uint8_t*> bounds;

	try{
		bounds = index.get_indices<>(buf.GetBufferPoint(),
				buf.GetBufferPoint()+buf.GetBufSize());
	}
	catch(std::invalid_argument){
		PyErr_SetString(PyExc_IndexError, "invalid index parameter");
		throw boost::python::error_already_set();
	}

	boost::python::list vals;
	while(bounds.start <= bounds.stop)
	{
		uint8_t val = *bounds.start;
		vals.append(val);
		std::advance( bounds.start, bounds.step);
	}

	return vals;
}

void buffers_setitem(Buffers & buf,slice index,boost::python::list vals)
{
	slice::range<uint8_t*> bounds;

	try{
		bounds = index.get_indices<>(buf.GetBufferPoint(),
				buf.GetBufferPoint()+buf.GetBufSize());
	}
	catch(std::invalid_argument){
		PyErr_SetString(PyExc_IndexError, "invalid index parameter");
		throw boost::python::error_already_set();
	}

	uint8_t idx=0;
	while(bounds.start <= bounds.stop)
	{
		uint8_t val = extract<uint8_t>(vals[idx++]);
		*bounds.start	= val;
		std::advance( bounds.start, bounds.step);
	}
}
#else
// __getslice__ and __setslice__ is retired from python3.x
boost::python::list buffers_getslice(Buffers & buf,uint32_t from,uint32_t to)
{
    if (from > to)
    {
		PyErr_SetString(PyExc_IndexError, "parameter error,from > to");
		throw boost::python::error_already_set();
    }

    // Return copy, as container only references its elements.
    boost::python::list list;
    while (from != to) list.append(buf.GetByte(from++));
    return list;
}

void buffers_setslice(Buffers & buf,uint32_t from,uint32_t to,boost::python::list val)
{
	if(from > to)
	{
		PyErr_SetString(PyExc_IndexError, "parameter error,from > to");
		throw boost::python::error_already_set();
	}

	while(from != to)
	{
		object obj = val.pop(0);
		buf.SetByte(from++,extract<uint8_t>(obj));
	}
}
#endif

boost::python::list buffers_Compare(Buffers& buf,Buffers& dstBuf,uint32_t desOffset,uint32_t offset,uint32_t length)
{
	uint32_t errPos,srcVal,dstVal;
	errPos = srcVal = dstVal = 0;

	if(false == buf.Compare(dstBuf,desOffset,offset,length,errPos,srcVal,dstVal))
	{
		errPos	= (uint32_t)ERROR_RTN_VAL;
	}

	boost::python::list list;
	list.append(errPos);
	list.append(srcVal);
	list.append(dstVal);

	return list;
}

uint64_t buffers_ReadRegVal(Buffers& buf,uint32_t startByte,uint8_t startBits,uint8_t bitsLen)
{
	uint64_t val = 0;

	if(false == buf.ReadRegVal(startByte,startBits,bitsLen,val))
	{
		return (uint64_t)ERROR_RTN_VAL;
	}

	return val;
}
