#ifndef _XPOWER_CONVERT_H
#define _XPOWER_CONVERT_H

#include <string>
#include <stdint.h>
#include <boost/python.hpp>

#include "buffer/Buffers.h"
#include "IPCPwrMngr.h"

boost::python::list MCUPwrMngrClient_GetPowerID(MCUPwrMngrClient& client,uint8_t maxLen);
bool MCUPwrMngrClient_SetPowerID(MCUPwrMngrClient& client,boost::python::list vals);

bool MCUPwrMngrClient_WrEEPROM(MCUPwrMngrClient& client,uint8_t regAddr,uint16_t offset,Buffers& buf,uint8_t len);
bool MCUPwrMngrClient_RdEEPROM(MCUPwrMngrClient& client,uint8_t regAddr,uint16_t offset,Buffers& buf,uint8_t len);

boost::python::list MCUPwrCtrlr_GetPowerID(MCUPwrCtrlr& pwrCtrlr,uint8_t maxLen);
bool MCUPwrCtrlr_SetPowerID(MCUPwrCtrlr& pwrCtrlr,boost::python::list vals);

bool MCUPwrCtrlr_WrEEPROM(MCUPwrCtrlr& pwrCtrlr,uint8_t port,uint8_t regAddr,uint16_t offset,Buffers& buf,uint8_t len);
bool MCUPwrCtrlr_RdEEPROM(MCUPwrCtrlr& pwrCtrlr,uint8_t port,uint8_t regAddr,uint16_t offset,Buffers& buf,uint8_t len);

bool MCUPwrCtrlr_NVMeMIDXfer(MCUPwrCtrlr& pwrCtrlr,uint8_t ch,Buffers& buf,uint16_t bufOff,uint8_t len,uint8_t offset);
bool MCUPwrMngrClient_NVMeMIDXfer(MCUPwrMngrClient& client,Buffers& buf,uint16_t bufOff,uint8_t len,uint8_t offset);
boost::python::list MCUPwrCtrlr_NVMeMIReady(MCUPwrCtrlr& pwrCtrlr,uint8_t ch);
boost::python::list MCUPwrMngrClient_NVMeMIReady(MCUPwrMngrClient& client);
bool MCUPwrCtrlr_NVMeMIDRcvr(MCUPwrCtrlr& pwrCtrlr,uint8_t ch,Buffers& buf,uint16_t bufOff,uint8_t len,uint8_t offset);
bool MCUPwrMngrClient_NVMeMIDRcvr(MCUPwrMngrClient& client,Buffers& buf,uint16_t bufOff,uint8_t len,uint8_t offset);
bool MCUPwrCtrlr_DownloadFW(MCUPwrCtrlr& pwrCtrlr,Buffers& buf,uint16_t offset,uint8_t len);
bool MCUPwrMngrClient_DownloadFW(MCUPwrMngrClient& client,Buffers& buf,uint16_t offset,uint8_t len);


#endif
