/**
 * file:Convert.cpp
*/
#include "Convert.h"

#define get_val_from_kwargs(key,type,variable) \
    if (kwargs.has_key(key)) { \
        py::extract<type> key_##variable(kwargs[key]); \
        if (key_##variable.check()) { \
            variable = key_##variable(); \
        } \
    }

NVMeCmdResp* Write_Convert(NVMeNVMCmd& obj,uint64_t slba,uint16_t length,uint8_t prinfo,\
                    uint16_t lbat,uint16_t lbatm,uint64_t lbst,uint64_t ilbrf,\
                    Buffers* dBuf,Buffers* mBuf,uint32_t timeout,py::dict kwargs)
{
    bool bFua = false,bLR = false,bSTC = false;
    uint8_t dtype = 0,dsm = 0;
    uint16_t dspec = 0;

    int numArgs = py::len(kwargs);
    if(numArgs)
    {
        get_val_from_kwargs("bFua",bool,bFua);
        get_val_from_kwargs("bLR",bool,bLR);
        get_val_from_kwargs("bSTC",bool,bSTC);
        get_val_from_kwargs("dtype",uint8_t,dtype);
        get_val_from_kwargs("dsm",uint8_t,dsm);
        get_val_from_kwargs("dspec",uint16_t,dspec);
    }

    return obj.Write(slba,length,prinfo,bFua,bLR,bSTC,dtype,dspec,dsm,lbat,lbatm,lbst,ilbrf,dBuf,mBuf,timeout);
}