import sys
import re
import json
import argparse

import xNVMe
import xBuffer

BUFFER_SIZE = 1*1024*1024

PROG_NAME   = "utilNVMeCli"
PROG_VER    = "V1.01"
PROG_DATE   = "20200428"
PROG_EMAIL  = "Winter_dong@ymtc.com"
DESC_FORMAT = "%s %s %s"

DEF_JSONFILE    = "utilNVMeCli.json"

AVAIL_REG_LST   = ("cdw0","cdw1","cdw2","cdw3","cdw10","cdw11","cdw12",\
               "cdw13","cdw14","cdw15")

def printCmdEntry(cmdEntry):
    headLst = AVAIL_REG_LST
    print("\nCommand Entry Dump:")
    for idx in range(len(cmdEntry)):
        print("%5s = 0x%08x"%(headLst[idx],cmdEntry[idx]))
    
def printBuffer(buffer,bufLen):
    print("\nCommand Buffer Dump:")
    if(None == buffer or 0 >= bufLen):
        return
    
    len = 128 if bufLen > 128 else bufLen
    buffer.Dump(0,len)
    print("\n")

def issueCmdByIOPassthru(devName,cmdEntry,buffer,bufLen):
    m = re.search("nvme(\d+)n(\d+)",devName)
    if(None == m):
        print("Error: Invalid Device Name %s"%devName)
        return False
    
    ctrlrID = int(m.group(1))
    nsID    = int(m.group(2))
    
    obj = xNVMe.GetNVMeDev().GetNVMNmSpc(ctrlrID,nsID).GetNVMeNVM()
    res = obj.IoPassthrough(*cmdEntry,bufLen,0,buffer,None,8000)
    
    return res.GetSC()

def issueCmdByAdminPassthru(devName,cmdEntry,buffer,bufLen):
    m = re.search("nvme(\d+)",devName)
    if(None == m):
        print("Error: Invalid Device Name %s"%devName)
        return -1
    
    ctrlrID = int(m.group(1))
    
    obj = xNVMe.GetNVMeDev().GetNVMCtrlr(ctrlrID).GetNVMeAdmin()
    res = obj.AdminPassthrough(*cmdEntry,bufLen,0,buffer,None,8000)
    
    return res.GetSC()

def getCmdRdBuf():
    rdBuf = xBuffer.GetBufMngr().GetSysBuf("PassthruRead",BUFFER_SIZE,0)
    
    return rdBuf

def getCmdWrBuf():
    wrBuf = xBuffer.GetBufMngr().GetSysBuf("PassthruWrite",BUFFER_SIZE,0)
    
    return wrBuf
            
def loadBufFromFile(filename,buffer,bufLen):
    try:
        buffer.FromFile(filename,0,bufLen)
    except:
        print("Error: Fail to load data from %s"%filename)
        return False
    
    return True 

def dumpBufToFile(filename,buffer,bufLen):
    try:
        buffer.ToFile(filename,0,bufLen)
    except:
        print("Error: fail to dump buffer data to file %s"%filename)
        return False
    
    return True

def uptBufFromParams(buffer,params):
    bufChgs    = [] if "buffer" not in params else params["buffer"]
    
    for p in bufChgs:
        offset,nbits,val    = p
        buffer.WriteRegVal(offset//8,offset%8,nbits,val)

def getCmdEntry(args,param):
    pargs   = {} if not "args" in param else param["args"]
    for k,p in pargs.items():
        val = 0 if not hasattr(args,k) else getattr(args,k)
        reg,offset,nbits = p
        
        if( ("buffer"!=reg) and (reg not in AVAIL_REG_LST) ):
            print("Error: Invalid Param Register Name: %s"%reg)
            return None
        
        if( "buffer"!=reg ):
            val = (val & ((1 << nbits) -1)) << offset
            
            if reg in param:
                param[reg]  |= val
            else:
                param[reg]  = val
        else:
            if reg in param:
                param[reg].append([offset,nbits,val])
            else:
                param[reg] = [[offset,nbits,val],]
    
    cdw0    = 0 if not "cdw0" in param else param["cdw0"]
    cdw1    = 0 if not "cdw1" in param else param["cdw1"]
    cdw2    = 0 if not "cdw2" in param else param["cdw2"]
    cdw3    = 0 if not "cdw3" in param else param["cdw3"]
    cdw10   = 0 if not "cdw10" in param else param["cdw10"]
    cdw11   = 0 if not "cdw11" in param else param["cdw11"]
    cdw12   = 0 if not "cdw12" in param else param["cdw12"]
    cdw13   = 0 if not "cdw13" in param else param["cdw13"]
    cdw14   = 0 if not "cdw14" in param else param["cdw14"]
    cdw15   = 0 if not "cdw15" in param else param["cdw15"]
    
    opcode  = 0 if not "opcode" in param else param["opcode"]
    cdw0    |= opcode
    
    bAdmin  = True if not "bAdmin" in param else param["bAdmin"]
    if(False == bAdmin):
        devname = "" if not hasattr(args,"devname") else getattr(args,"devname")
        m = re.search("nvme\d+n(\d+)",devname)
        nsid    = 0 if None==m else int(m.group(1))

        cdw1    |= nsid
    
    return cdw0,cdw1,cdw2,cdw3,cdw10,cdw11,cdw12,cdw13,cdw14,cdw15
        
def getCmdParam(cmd,jsonCfg):
    try:
        return jsonCfg["commands"][cmd]["params"]
    except:
        pass
    
    return {}

def getArgParse():
    parser      = argparse.ArgumentParser(prog=PROG_NAME,\
                            description=DESC_FORMAT%(PROG_VER,PROG_DATE,PROG_EMAIL))
    
    parser.add_argument("--jsonfile",type=str,default=DEF_JSONFILE,help="json config file")
    
    return parser

def getJsonFile(parser):
    argv = list(set(sys.argv) - set(["-h","--help"]))
    args,_ = parser.parse_known_args(argv)
    
    return args.jsonfile

def cfgArgParse(parser,jsonCfg):
    subparsers  = parser.add_subparsers(help='supported commands')
    subpList    = []
    
    if(not "commands" in jsonCfg):
        print("Error : commands keyword not be found in json file")
        return None
    
    for opt,val in jsonCfg["commands"].items():
        if(not "args" in val):
            print("Error : args keyword not be found in %s"%opt)
            return None
        
        helpStr = opt if not "help" in val else val["help"]     
        p = subparsers.add_parser(opt, help=helpStr)
        
        for arg in val["args"]:
            subHelp     = "" if not "help" in arg else arg["help"]
            argType     = "" if not "type" in arg else arg["type"]
            defValue    = "" if not "default" in arg else arg["default"]
            longName    = "" if not "long-name" in arg else "--"+arg["long-name"]
            shortName   = "" if not "short-name" in arg else "-"+arg["short-name"]
            
            if("" == shortName and "" == longName):
                print("Error : [%s] Either long-name or short-name is not defined"%opt)
                return None
            
            argParams   = {}
            
            if("" != subHelp):
                argParams["help"]       = subHelp
                
            if("" != defValue):
                argParams["default"]    = defValue
            
            if("int" == argType):
                argParams["type"]   = int
            elif("str" == argType):
                argParams["type"]   = str
            elif("bool" == argType):
                argParams["type"]   = bool
            else:
                print("Error : [%s] Invalid Argument Type : %s"%(opt,argType))
                return None
            
            if("" == shortName):
                p.add_argument(longName,**argParams)
            else:
                p.add_argument(shortName,longName,**argParams)
                
            p.set_defaults(command=opt)
            
        p.add_argument("--devname",type=str,help="device name",default="/dev/nvme0n1")
        p.add_argument("--input",type=str,help="input filename data from")
        p.add_argument("--output",type=str,help="output filename data to")
        p.add_argument("--debug",type=bool,help="enable debug dump")
        p.add_argument("--dumpOffset",type=int,default=0,help="offset of Buffer Dump")
        p.add_argument("--dumpLength",type=int,default=128,help="length of Buffer Dump")
                
        subpList.append(p)
        
    return parser.parse_args()

def parseJson(filename):
    cfg = None
    try:
        with open(filename,"r") as f:
            cfg = json.load(f)
    except:
        print("Error: %s not found or Json format Error"%filename)
        
    return cfg

if __name__ == "__main__":
    parser  = getArgParse()
    jsonfile= getJsonFile(parser)
    jsonCfg = parseJson(jsonfile)
    if(None == jsonCfg):
        sys.exit(-1)
    
    args = cfgArgParse(parser,jsonCfg)
    if(not hasattr(args,"command")):
        print("Error: please specify command first")
        sys.exit(-1)
    
    devName = args.devname
    command = args.command
    inFile  = args.input
    outFile = args.output
    debug   = args.debug
    dumpOffset  = args.dumpOffset
    dumpLength  = args.dumpLength
    
    params  = getCmdParam(command,jsonCfg)
    cmdEntry= getCmdEntry(args,params)
    
    if(None == cmdEntry):
        print("Error: Fail to getCmdEntry")
        sys.exit(-1)
    
    mode    = "" if not "mode" in params else params["mode"]
    admin   = True if not "bAdmin" in params else params["bAdmin"]
    buffer  = None
    bufLen  = 0
    
    if("out" == mode):
        buffer  = getCmdWrBuf()
        bufLen  = 4096 if not "nBytes" in params else params["nBytes"]
        if(None != inFile):
            loadBufFromFile(inFile,buffer,bufLen)
            
        uptBufFromParams(buffer,params)
    elif("in" == mode):
        bufLen  = 4096 if not "nBytes" in params else params["nBytes"]
        buffer  = getCmdRdBuf()
    else:
        pass
    
    if(True == debug):
        printCmdEntry(cmdEntry)
        printBuffer(buffer,bufLen)
        
        print("Operation Successful!")
        sys.exit(0)
    
    if(True == admin):
        rtn = issueCmdByAdminPassthru(devName,cmdEntry,buffer,bufLen)
    else:
        rtn = issueCmdByIOPassthru(devName,cmdEntry,buffer,bufLen)
        
    if(0 != rtn):
        print("Error : Fail to issue %s , errCode = %d"%(command,rtn))
        sys.exit(rtn)
    else:
        print("Operation Successful!")
        
        if("in" == mode and None != outFile):
            dumpBufToFile(outFile,buffer,bufLen)
            
        if(None!=buffer):
            buffer.Dump(dumpOffset,dumpLength)
        
    sys.exit(0)