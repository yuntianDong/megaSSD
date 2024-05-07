import sys
import argparse

import xPower

PROG_NAME       = "utilPowerCtrlr"
PROG_VER        = "V1.00"
PROG_DATE       = "20200521"
PROG_EMAIL      = "Winter_dong@ymtc.com"
DESC_FORMAT     = "%s %s %s"

BOARD_TYPE_MCU  = "mcu"
BOARD_TYPE_SCPI = "scpi"

ARG_MODE_CS     = "shared"
ARG_MODE_EXC    = "exclusive"
ARG_MODE_DEF    = ARG_MODE_EXC

ARG_DEVICE_DEF  = "/dev/ttyACM0"
ARG_PORT_DEF    = 0

ARG_OPTION_POWON    = "poweron"
ARG_OPTION_POWOFF   = "poweroff"
ARG_OPTION_GETVOL   = "voltage"
ARG_OPTION_GETCUR   = "current"
ARG_OPTION_GETPOW   = "power"
ARG_OPTION_STATUS   = "status"
ARG_OPTION_VERSION  = "version"
ARG_OPTION_DEF      = ARG_OPTION_STATUS
ARG_OPTION_LIST     = [ARG_OPTION_POWON,ARG_OPTION_POWOFF,ARG_OPTION_GETVOL,ARG_OPTION_GETCUR,ARG_OPTION_GETPOW,ARG_OPTION_STATUS,ARG_OPTION_VERSION]

def PowerOn(pwrCtrlr,port):
    if(True == isinstance(pwrCtrlr,xPower.PwrMngrClient)):
        return pwrCtrlr.PowerOn()
    else:
        return pwrCtrlr.PowerOn(port)

def PowerOff(pwrCtrlr,port):
    if(True == isinstance(pwrCtrlr,xPower.PwrMngrClient)):
        return pwrCtrlr.PowerOff()
    else:
        return pwrCtrlr.PowerOff(port)

def GetStatus(pwrCtrlr,port):
    status = False
    if(True == isinstance(pwrCtrlr,xPower.PwrMngrClient)):
        status = pwrCtrlr.IsPwrOn
    else:
        status = pwrCtrlr.GetPowerStatus(port)
        
    print("Status : %s\n"%("ON" if 1 == status else "OFF"))
    return True

def GetVoltage(pwrCtrlr,port):
    vol = 0
    if(True == isinstance(pwrCtrlr,xPower.PwrMngrClient)):
        vol = pwrCtrlr.Voltage
    else:
        vol = pwrCtrlr.GetVoltage(port)
        
    print("Voltage : %dmV\n"%vol)
    return True

def GetCurrent(pwrCtrlr,port):
    current = 0
    if(True == isinstance(pwrCtrlr,xPower.PwrMngrClient)):
        current = pwrCtrlr.Current
    else:
        current = pwrCtrlr.GetCurrent(port)
        
    print("Current : %dmA\n"%current)
    return True

def GetPower(pwrCtrlr,port):
    power = 0
    if(True == isinstance(pwrCtrlr,xPower.PwrMngrClient)):
        power = pwrCtrlr.Power
    else:
        power = pwrCtrlr.GetPower(port)
        
    print("Power : %dmw\n"%power)
    return True

def GetVersion(pwrCtrlr):
    version = pwrCtrlr.Version
        
    print("Version : %s\n"%version)
    return True

def GetPwrCtrlrObject(boardtype,mode,device,port):
    if(ARG_MODE_CS == mode):
        return xPower.PwrMngrClient(port)
    else:
        if(BOARD_TYPE_MCU == boardtype):
            return xPower.MCUPwrCtrlr(device)
        else:
            return xPower.SCPIPwrCtrlr(device)
        
    return None

def ParseArguments():
    parser      = argparse.ArgumentParser(prog=PROG_NAME,\
                            description=DESC_FORMAT%(PROG_VER,PROG_DATE,PROG_EMAIL))
    
    subparsers  = parser.add_subparsers(help='supported commands')
    pLst        = {}
    
    pLst[BOARD_TYPE_MCU]    = subparsers.add_parser(BOARD_TYPE_MCU, help="MCU Power Board API")
    pLst[BOARD_TYPE_SCPI]   = subparsers.add_parser(BOARD_TYPE_SCPI, help="SCPI Power Board API")
    
    for opt in pLst.keys():
        pLst[opt].set_defaults(boardtype=opt)
        
        pLst[opt].add_argument("-m","--mode",type=str,choices=[ARG_MODE_CS,ARG_MODE_EXC],
                        default=ARG_MODE_DEF,help="Work Mode")
        pLst[opt].add_argument("-d","--device",type=str,
                        default=ARG_DEVICE_DEF,help="Device Name,mode==exclusive only")
        pLst[opt].add_argument("-p","--port",type=int,
                        default=ARG_PORT_DEF,help="Power Board Port, 0 based")
        pLst[opt].add_argument("-o","--option",type=str,choices=ARG_OPTION_LIST,
                        default=ARG_OPTION_DEF,help="Operate option")

    return parser.parse_args()

if __name__ == "__main__":
    args = ParseArguments()
    
    boardtype   = args.boardtype
    mode        = args.mode
    device      = args.device
    port        = args.port
    option      = args.option
    
    bResult     = True
    
    pwrCtrlr    = GetPwrCtrlrObject(boardtype,mode,device,port)
    if(ARG_OPTION_POWON == option):
        bResult = PowerOn(pwrCtrlr,port)
    elif(ARG_OPTION_POWOFF == option):
        bResult = PowerOff(pwrCtrlr,port)
    elif(ARG_OPTION_GETVOL == option):
        bResult = GetVoltage(pwrCtrlr,port)
    elif(ARG_OPTION_GETCUR == option):
        bResult = GetCurrent(pwrCtrlr,port)
    elif(ARG_OPTION_GETPOW == option):
        bResult = GetPower(pwrCtrlr,port)
    elif(ARG_OPTION_STATUS == option):
        bResult = GetStatus(pwrCtrlr,port)
    elif(ARG_OPTION_VERSION == option):
        bResult = GetVersion(pwrCtrlr)
    else:
        print("Error: Invalid option : %s\n"%option)
        sys.exit(-1)
        
    if(False == bResult):
        print("Error: Operation Fail\n")
        sys.exit(-1)
        
    sys.exit(0)