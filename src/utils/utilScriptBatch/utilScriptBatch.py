import os
import sys
import json
import argparse

import xBrain

PROG_NAME       = "utilScriptBatch"
PROG_VER        = "V1.00"
PROG_DATE       = "20200508"
PROG_EMAIL      = "Winter_dong@ymtc.com"
DESC_FORMAT     = "%s %s %s"

DEF_JSONFILE    = "utilScriptBatch.json"
DEF_SERVER_ID   = 0
DEF_BATCH_ID    = 0
DEF_DEV_NAME    = "/dev/nvme0n1"
DEF_SLOT_ID     = 0

OPT_ADD_M_NEW       = "new"
OPT_ADD_M_APPEND    = "append"
OPT_ADD_M_RECOVER   = "recover"

OPT_ADD_M_LST   = [OPT_ADD_M_NEW,OPT_ADD_M_APPEND,OPT_ADD_M_RECOVER]

DEF_REPORT_DUMPFILE = "summary.log"

def AddTaskCond(cond,xt):
    for c in cond:
        key = "" if "key" not in c else c["key"]
        val = [] if "val" not in c else c["val"]
        excluded = False if "excluded" not in c else c["excluded"]
        
        if("" == key or 0 >= len(val)):
            print("Error : Invalid Condition : %s"%str(c))
            continue
        
        bRtn = True
        for v in val:
            if( "sn" == key.lower() ):
                bRtn = xt.AddCondition(xBrain.enCondKey.SN,v,excluded)
            elif( "mn" == key.lower() ):
                bRtn = xt.AddCondition(xBrain.enCondKey.MN,v,excluded)
            elif( "fr" == key.lower() ):
                bRtn = xt.AddCondition(xBrain.enCondKey.FR,v,excluded)
            elif( "gr" == key.lower() ):
                bRtn = xt.AddCondition(xBrain.enCondKey.FR,v,excluded)
            elif( "gb" == key.lower() ):
                bRtn = xt.AddCondition(xBrain.enCondKey.GB,v,excluded)
            elif( "ss" == key.lower() ):
                bRtn = xt.AddCondition(xBrain.enCondKey.SS,v,excluded)
            else:
                print("Error : Unknown Condition Key=%s"%key)
                
            if(False == bRtn):
                print("Error : Invalid Condition %s=%s"%(key,str(v)))
                    
def AddSlots(inst,slots):
    rtn = True
    
    for idx in range(len(slots)):
        if(False == inst.AddSlot(idx)):
            print("Error: Fail to Add Slot %d"%idx)
            rtn = False
        #=======================================================================
        # else:
        #     info = inst.GetSlotInfo(idx)
        #     if(None == info):
        #         print("Fail to GetSlotInfo(%d)"%idx)
        #         rtn = False
        #     elif(info.DevName != slots[idx]):
        #         print("%d DUT Mismatch,exp %s;act %s"%(idx,info.DevName,slots[idx]))
        #         rtn = False
        #=======================================================================
        
    return rtn

def GetDUTSlots(cfg):
    dutLst = []
    if "duts" not in cfg or "slotID" not in cfg["duts"]:
        return dutLst
    
    return cfg["duts"]["slotID"]

def AddTasks(inst,tasks):
    for t in tasks:
        xt = xBrain.TaskDesc()
        
        xt.name    = t["name"]
        xt.cmdline = t["cmdline"]
        xt.loopcnt = t["loopcnt"] if "loopcnt" in t else 0
        
        if("condition"  in t):
            AddTaskCond(t["condition"],xt)
        
        if(False == inst.AddTask(xt)):
            return False
        
    return True

def GetTaskList(cfg):
    taskLst = []
    srtRoot = ""
    
    if("tasks" not in cfg):
        return taskLst
    
    if("global" in cfg and "scriptRoot" in cfg["global"]):
        srtRoot = cfg["global"]["scriptRoot"]
        
    for k in cfg["tasks"].keys():
        v = cfg["tasks"][k]
        
        if("script" not in v):
            print("Error : Invalid Task %s"%k)
            continue
        
        tool    = "" if "tool" not in v else v["tool"]
        
        v["cmdline"]    = v["script"]
        
        if("" != srtRoot):
            cmdline = os.path.join(srtRoot,v["script"])
            v["cmdline"]= cmdline
            
        if("" != tool):
            v["cmdline"]= "%s %s"%(tool,v["cmdline"])
        
        v["name"]   = k
        
        taskLst.append(v)
        
    return taskLst

def GetTaskMngrClient(serverID,batchID,bNeedReset):
    d = xBrain.GetTaskMngrClient(serverID,batchID)
    
    if(True == bNeedReset):
        d.Reset()
    
    return d

def GetTaskMngrServer(serverID):
    d = xBrain.GetTaskMngrServer(0)
    return d

def GetDutMngrServer(devName,slotID):
    d = xBrain.GetDutMngrServer(devName,slotID)
    return d

def ParseJson(filename):
    cfg = None
    try:
        with open(filename,"r") as f:
            cfg = json.load(f)
    except:
        print("Error: %s not found or Json format Error"%filename)
        return None
        
    return cfg

def ParseArgument():
    parser      = argparse.ArgumentParser(prog=PROG_NAME,\
                            description=DESC_FORMAT%(PROG_VER,PROG_DATE,PROG_EMAIL))
    
    subparsers  = parser.add_subparsers(help='supported commands')
    pLst        = {}
    
    pLst["add"]     = subparsers.add_parser("add", help="Add Script into Batch List")
    pLst["report"]  = subparsers.add_parser("report", help="Print/Dump Test Summary Report")
    pLst["result"]  = subparsers.add_parser("result", help="Print/Dump Test Result Report")
    pLst["opensrv"] = subparsers.add_parser("opensrv", help="Open xBrain.TaskMngr Server")
    pLst["closesrv"]= subparsers.add_parser("closesrv", help="Close xBrain.TaskMngr Server")
    pLst["opendut"] = subparsers.add_parser("opendut", help="Open xBrain.DutMngr Server")
    pLst["closedut"]= subparsers.add_parser("closedut", help="Close xBrain.DutMngr Server")
    
    for opt in pLst.keys():
        pLst[opt].set_defaults(command=opt)
    
    for opt in ["add","report","result","opensrv","closesrv","closedut"]:
        pLst[opt].add_argument("-s","--serverID",type=int,
                        default=DEF_SERVER_ID,help="xBrain Server ID")
    
    for opt in ["add"]:
        pLst[opt].add_argument("-f","--jsonfile",type=str,
                            default=DEF_JSONFILE,help="json config file")
     
    for opt in ["add","report","result","closedut","closesrv"]:
        pLst[opt].add_argument("-i","--batchID",type=int,
                        default=DEF_BATCH_ID,help="xBrain Batch Slaver ID")
        
    for opt in ["opendut"]:
        pLst[opt].add_argument("-d","--devName",type=str,
                        default=DEF_DEV_NAME,help="Slot Device Name")
        
    for opt in ["opendut","closedut"]:
        pLst[opt].add_argument("-l","--slotID",type=int,
                        default=DEF_SLOT_ID,help="Slot Identify")
        
    for opt in ["add"]:
        pLst[opt].add_argument("-m","--mode",type=str,choices=OPT_ADD_M_LST,
                        default=OPT_ADD_M_NEW,help="Add mode")
        
    for opt in ["report","result"]:
        pLst[opt].add_argument("-p","--dumpTofile",type=str,
                               help="Dump Test Report Summary to file")
    
    return parser.parse_args()

def opt_add(jsonfile,serverID,batchID,mode):
    cfg  = ParseJson(args.jsonfile)
    if(None == cfg):
        return False
        
    inst  = GetTaskMngrClient(serverID,batchID,OPT_ADD_M_NEW == mode)
    
    if(OPT_ADD_M_RECOVER != mode):
        tasks = GetTaskList(cfg)
        if(0 == len(tasks) or False == AddTasks(inst,tasks)):
            print("Warn: Fail to Add Task(Script) to List")
    
    slots = GetDUTSlots(cfg)
    if(0 == len(slots) or False == AddSlots(inst,slots)):
        print("Warn: Fail to Add DUT Slots %s"%str(slots))
    
    print("Configure Script Batch Successful")
    return True

def opt_report(serverID,batchID,dumpToFile):
    inst  = GetTaskMngrClient(serverID,batchID,False)
    
    if(None == dumpToFile or "none" == dumpToFile):
        return inst.PrintTestReport()
    else:
        return inst.DumpTestReport(dumpToFile)
    
def opt_result(serverID,batchID,dumpToFile):
    inst  = GetTaskMngrClient(serverID,batchID,False)
    
    if(None == dumpToFile or "none" == dumpToFile):
        return inst.PrintTestResult()
    else:
        return inst.DumpTestResult(dumpToFile)

def opt_opensrv(serverID):
    inst = GetTaskMngrServer(serverID)    
    print("TaskMngrServer %d Opened!\n"%(serverID))
    inst.Run()
    
    return True

def opt_closesrv(serverID,batchID):
    inst  = GetTaskMngrClient(serverID,batchID,False)
    print("TaskMngrServer %d Closed!\n"%(serverID))
    
    return inst.CloseService()

def opt_opendut(devName,slotID):
    inst = GetDutMngrServer(devName,slotID)    
    print("Slot (%s)%d Opened!\n"%(devName,slotID))
    inst.Run()
    
    return True

def opt_closedut(serverID,batchID,slotID):
    inst  = GetTaskMngrClient(serverID,batchID,False)
    print("Slot %d Closed!\n"%(slotID))
    
    return inst.DelSlot(slotID)

if __name__ == "__main__":
    args = ParseArgument()
    command  = args.command
    command  = command.lower()

    bRtn = True
    if("add" == command):
        bRtn = opt_add(args.jsonfile,args.serverID,args.batchID,args.mode)
    elif("report" == command):
        bRtn = opt_report(args.serverID,args.batchID,args.dumpTofile)
    elif("result" == command):
        bRtn = opt_result(args.serverID,args.batchID,args.dumpTofile)
    elif("opensrv" == command):
        bRtn = opt_opensrv(args.serverID)
    elif("opendut" == command):
        bRtn = opt_opendut(args.devName,args.slotID)
    elif("closesrv" == command):
        bRtn = opt_closesrv(args.serverID,args.batchID)
    elif("closedut" == command):
        bRtn = opt_closedut(args.serverID,args.batchID,args.slotID)
    else:
        print("Unknown Command %s",command)
        bRtn = False

    if(False == bRtn):
        sys.exit(-1)
        
    sys.exit(0)