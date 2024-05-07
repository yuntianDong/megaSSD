import os
import sys
import json
import time
import argparse

PROG_NAME       = "ScriptBatch"
PROG_VER        = "V1.00"
PROG_DATE       = "20200508"
PROG_EMAIL      = "Winter_dong@ymtc.com"
DESC_FORMAT     = "%s %s %s"

DEF_JSONFILE    = "utilScriptBatch.json"
DEF_TOOLPATH    = "./utilScriptBatch"
DEF_SCRIPTROOT  = "./"
DEF_TMSRVID     = 0
DEF_BATCHID     = 0
DEF_SLOTPARAM   = ["/dev/nvme0n1"]

TESTSUMM_DUMP_FILE      = "TestSummary.log"
TESTRSLT_DUMP_FILE      = "TestResult.csv"

ACTION_SETUP    = "setup"
ACTION_RECOVER  = "recover"
ACTION_APPEND   = "append"
ACTION_CLEAN    = "clean"
ACTION_REPORT   = "report"

SLOT_SERVER_LAUNCH      = "%(tool)s opendut -d %(devName)s -l %(slotID)d"
SLOT_SERVER_STOP        = "%(tool)s closedut -s %(srvID)d -i %(batchID)d -l %(slotID)d"
TASKMNGR_SERVER_LAUNCH  = "%(tool)s opensrv -s %(srvID)d"
TASKMNGR_SERVER_STOP    = "%(tool)s closesrv -s %(srvID)d -i %(batchID)d"
SCRIPT_LIST_ADD_POOL    = "%(tool)s add -s %(srvID)d -i %(batchID)d -f %(Jsonfile)s -m %(Mode)s"
TEST_SUMMARY_REPORT     = "%(tool)s report -s %(srvID)d -i %(batchID)d -p %(dumpFile)s"
TEST_RESULT_ERPOTY      = "%(tool)s result -s %(srvID)d -i %(batchID)d -p %(dumpFile)s"

AVAILABLE_ACTION_LIST   = [ACTION_SETUP,ACTION_RECOVER,ACTION_APPEND,ACTION_CLEAN,ACTION_REPORT]

def GetToolPath(cfg):
    if("global" not in cfg):
        return DEF_TOOLPATH
    
    return DEF_TOOLPATH if "toolPath" not in cfg["global"] else cfg["global"]["toolPath"]

def GetScriptRoot(cfg):
    if("global" not in cfg):
        return DEF_SCRIPTROOT
    
    return DEF_SCRIPTROOT if "scriptRoot" not in cfg["global"] else cfg["global"]["scriptRoot"]

def GetTaskMngrSrvParam(cfg):
    if("global" not in cfg):
        return DEF_TMSRVID,DEF_BATCHID
    
    tmsrvID = DEF_TMSRVID if "taskMnrSID" not in cfg["global"] else cfg["global"]["taskMnrSID"]
    batchID = DEF_BATCHID if "batchID" not in cfg["global"] else cfg["global"]["batchID"]
    
    return tmsrvID,batchID

def GetSlotParam(cfg):
    if("duts" not in cfg):
        return DEF_SLOTPARAM
    
    return DEF_SLOTPARAM if "slotID" not in cfg["duts"] else cfg["duts"]["slotID"]

def LaunchSlotServer(toolPath,tmSrvID,batchID,slotParam):
    bRtn = True
    for idx in range(len(slotParam)):
        devName = slotParam[idx]
        
        r = RunShellWithNewTerm(
        SLOT_SERVER_LAUNCH%{
                "tool"  : toolPath,
                "devName" : devName,
                "slotID"    : idx
            }
        )
        
        if(False == r):
            print("Fail to launch Slot %d(%s)"%(idx,devName))
        
        bRtn &= r
        
    return bRtn

def LaunchTaskMngrServer(toolPath,tmSrvID):
    return RunShellWithNewTerm(
        TASKMNGR_SERVER_LAUNCH%{
                "tool"  : toolPath,
                "srvID" : tmSrvID
            }
        )

def AddScriptTaskIntoPool(toolPath,tmSrvID,batchID,jsonFile,mode):
    return RunShellWithNewTerm(
        SCRIPT_LIST_ADD_POOL%{
                "tool"  : toolPath,
                "srvID" : tmSrvID,
                "batchID"   : batchID,
                "Jsonfile"  : jsonFile,
                "Mode"  : mode
            }
        )

def StopSlotServer(toolPath,tmSrvID,batchID,slotParam):
    bRtn = True
    for slotID in range(len(slotParam)):
        r = RunShellWithNewTerm(
            SLOT_SERVER_STOP%{
                    "tool"  : toolPath,
                    "srvID" : tmSrvID,
                    "batchID"   : batchID,
                    "slotID"    : slotID
                }
            )
        
        if(False == r):
            print("Fail to Stop Slot %d(%s)"%(slotID,slotParam[slotID]))
        
        bRtn &= r
        
    return bRtn

def StopTaskMngrServer(toolPath,tmSrvID,batchID):
    return RunShellWithNewTerm(
        TASKMNGR_SERVER_STOP%{
                "tool"  : toolPath,
                "srvID" : tmSrvID,
                "batchID"   : batchID
            }
        )

def SetUpScriptBatchEnv(toolPath,tmSrvID,batchID,slotParam,jsonFile,action):
    if(ACTION_SETUP == action):
        mode = "new"
    elif(ACTION_RECOVER == action):
        mode = "recover"
    elif(ACTION_APPEND == action):
        mode = "append"
    else:
        print("Error : Invalid Parameter: action =%s"%action)
        return False
    
    if(False == LaunchSlotServer(toolPath,tmSrvID,batchID,slotParam)):
        print("Fail to LaunchSlotServer()")
        return False
    
    if(False == LaunchTaskMngrServer(toolPath,tmSrvID)):
        print("Fail to LaunchTaskMngrServer()")
        return False
    
    if(False == AddScriptTaskIntoPool(toolPath,tmSrvID,batchID,jsonFile,mode)):
        print("Fail to AddScriptTaskIntoPool()")
        return False
    
    return True

def CleanupScriptBatchEnv(toolPath,tmSrvID,batchID,slotParam):
    if(False == StopSlotServer(toolPath,tmSrvID,batchID,slotParam)):
        print("Error: Fail to call StopSlotServer()")
        return False
    
    if(False == StopTaskMngrServer(toolPath,tmSrvID,batchID)):
        print("Error: Fail to call StopTaskMngrServer()")
        return False
    
    return True

def ReportTestResult(toolPath,tmSrvID,batchID):
    RunShellWithNewTerm(TEST_SUMMARY_REPORT%
                               {"tool"  : toolPath,
                                "srvID" : tmSrvID,
                                "batchID":batchID,
                                "dumpFile":"none"})
    
    RunShellWithNewTerm(TEST_SUMMARY_REPORT%
                               {"tool"  : toolPath,
                                "srvID" : tmSrvID,
                                "batchID":batchID,
                                "dumpFile":TESTSUMM_DUMP_FILE})
    
    RunShellWithNewTerm(TEST_RESULT_ERPOTY%
                               {"tool"  : toolPath,
                                "srvID" : tmSrvID,
                                "batchID":batchID,
                                "dumpFile":TESTRSLT_DUMP_FILE})
    
    return True
    
def RunShellWithNewTerm(shellCmd):
    time.sleep(3)
    if(True == window):
        cmdline = "gnome-terminal -e \"sudo %s\""%shellCmd
    else:
        cmdline = "sudo %s &"%shellCmd
    
    print(cmdline)
    return 0 == os.system(cmdline)
    
def ParseJson(filename):
    cfg = None
    try:
        with open(filename,"r") as f:
            cfg = json.load(f)
    except Exception as e:
        print("Error: %s not found or Json format Error"%filename)
        print(e)
        return None
        
    return cfg

def ParseArguments():
    parser      = argparse.ArgumentParser(prog=PROG_NAME,\
                            description=DESC_FORMAT%(PROG_VER,PROG_DATE,PROG_EMAIL))
    
    parser.add_argument("-f","--jsonfile",type=str,
                            default=DEF_JSONFILE,help="json config file")
    
    parser.add_argument("-a","--action",choices=AVAILABLE_ACTION_LIST,type=str,help="action name")
    
    parser.add_argument("-w","--window",type=bool,default=False,help="open new terminal or not")
    
    return parser.parse_args()

if __name__ == "__main__":
    global window
    
    args = ParseArguments()
    
    jsonFile    = args.jsonfile
    action      = args.action
    window      = args.window
    
    if(None == action or action.lower() not in AVAILABLE_ACTION_LIST ):
        print("Unknown parameter action=%s"%action)
        sys.exit(-1)
    
    cfg     = ParseJson(jsonFile)
    if(None == cfg):
        print("Fail to ParseJson(%s)"%jsonFile)
        sys.exit(-1)
        
    toolPath        = GetToolPath(cfg)
    srtRoot         = GetScriptRoot(cfg)
    tmSrvID,batchID = GetTaskMngrSrvParam(cfg)
    slotParam       = GetSlotParam(cfg)
    
    if( action.lower() in [ACTION_SETUP,ACTION_RECOVER,ACTION_APPEND]
       and False == SetUpScriptBatchEnv(toolPath,tmSrvID,batchID,slotParam,jsonFile,action.lower())):
        print("Fail to SetUpScriptBatchEnv()")
        print(str(cfg))
        sys.exit(-1)
    elif(ACTION_CLEAN == action.lower()
         and False == CleanupScriptBatchEnv(toolPath,tmSrvID,batchID,slotParam)):
        print("Fail to CleanupScriptBatchEnv()")
        sys.exit(-1)
    elif(ACTION_REPORT == action.lower()
         and False == ReportTestResult(toolPath,tmSrvID,batchID)):
        print("Fail to ReportTestResult()")
        sys.exit(-1)
    else:
        pass
        
    sys.exit(0)
        
        