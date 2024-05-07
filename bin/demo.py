import sys,os
import argparse
import random,time
import traceback
import xBuffer,xDevice,xNVMe,xPattern,xContext,xLogger

class demo:
    def __init__(self) -> None:
        self.traddr             = self.argSpec().traddr
        # import pdb
        # pdb.set_trace()
        self.LogInfo("step 1 : init ssd device\n")
        self.dev,self.ssd                = self._init_xDevice()
        self.LogInfo("step 2 : init NVMe cmd set\n")
        self.admin_cmd_set,self.io_cmd_set = self._init_xNVMe()
        self.LogInfo("step 3 : init buffer manager\n")
        self.buf_manager        = self._init_xBuffer()
        self.LogInfo("step 4 : init xContext\n")
        self.ctrlr_context      = self._init_xContext()
        self.load_context()
        self.LogInfo("step 5 : init xPattern\n")
        self.pattern_manager    = self._init_xPattern()


    def LogDebug(self,msg):
        # get func caller info
        stack = traceback.extract_stack(None, 2)
        filename, lineno, _, _ = stack[0]
        filename = os.path.basename(filename)

        xLogger.LOGDEBUG(filename,lineno,msg)

    def LogInfo(self,msg):
        # get func caller info
        stack = traceback.extract_stack(None, 2)
        filename, lineno, _, _ = stack[0]
        filename = os.path.basename(filename)

        xLogger.LOGINFO(filename,lineno,msg)

    def LogWarn(self,msg):
        # get func caller info
        stack = traceback.extract_stack(None, 2)
        filename, lineno, _, _ = stack[0]
        filename = os.path.basename(filename)

        xLogger.LOGWARN(filename,lineno,msg)

    def LogError(self,msg):
        # get func caller info
        stack = traceback.extract_stack(None, 2)
        filename, lineno, _, _ = stack[0]
        filename = os.path.basename(filename)

        xLogger.LOGERROR(filename,lineno,msg)
        
    def argSpec(self):
        parser = argparse.ArgumentParser(description="power test on spdk!")
        parser.add_argument('-t', '--traddr', type=str,
                            help="[Needed] device pcie traddr,like -t/--traddr 0000:05:00.0")
        args = parser.parse_args()
        return args
    
    def _init_xDevice(self):
        ssd = xDevice.IHostDevice(self.traddr)

        dev = ssd.GetNVMeDevice(0)
        self.LogDebug("current driver is : {},bdf is {}".format(dev.driver , dev.bdf))

        return dev,ssd
    
    def _init_xNVMe(self,nsid = None):
        nsid = 1 if nsid == None else nsid
        self.cmd_set = xNVMe.INVMeDevice()
        admin_cmd_set = self.cmd_set.GetCtrlr(self.dev)
        io_cmd_set = self.cmd_set.GetNS(self.dev,nsid)

        return admin_cmd_set,io_cmd_set
    
    def _init_xBuffer(self):

        buf_manager = xBuffer.GetBufMngr()

        return buf_manager
    
    def _init_xContext(self):
        ctrlr_context = xContext.ParentContext()

        # get total NVMeSize through Identify
        identify_buf = self.buf_manager.GetUsrBuf("Identify",4096,0,self.dev.driver)

        resp = self.admin_cmd_set.Identify(1,0,0,0,0,identify_buf,8000)
        identify_buf.SetEndianMode(xBuffer.enEndianMode.LittleEndian)
        if resp.Succeeded() != True:
            self.LogError("Identify cmd excute fail!")

        total_nvme_size = identify_buf.GetQWord(280)
        ctrlr_context.totalNvmSize = total_nvme_size
        self.LogInfo("total nvme size is {}".format(ctrlr_context.totalNvmSize))

        ctrlr_context.AddLbaMap(0,512,0)
        ctrlr_context.AddLbaMap(1,512,8)
        ctrlr_context.AddLbaMap(2,4096,0)
        ctrlr_context.AddLbaMap(3,4096,8)

        ctrlr_context.DumpLbaMap()

        # get each ns info
        identify_buf.FillZero(0,4096)
        resp = self.admin_cmd_set.Identify(0x10,0,0,0,0,identify_buf,8000)
        if resp.Succeeded() != True:
            self.LogError("Identify cmd excute fail!")
        
        ns_identify_buf = self.buf_manager.GetUsrBuf("Identify",4096,1,self.dev.driver)
        ns_identify_buf.SetEndianMode(xBuffer.enEndianMode.LittleEndian)
        offset = 0
        for idx in range(0,identify_buf.GetBufSize(),4):
            nsid = identify_buf.GetDWord(idx)
            if nsid == 0 :
                break
            else :
                #get ns data structure through identify
                resp = self.admin_cmd_set.Identify(0x11,0,nsid,0,0,ns_identify_buf,8000)
                if resp.Succeeded() != True:
                    self.LogError("Identify cmd excute fail!")
                lbaf = (ns_identify_buf.GetByte(26)) >> 5
                ctrlr_context.AddNsMap(nsid,offset,ns_identify_buf.GetQWord(8),lbaf)
                offset += ns_identify_buf.GetQWord(8)

                # check ns info
                ns_context = ctrlr_context.GetChildContext(nsid)
                self.LogInfo("nsid:{} using sector size {}".format(nsid,ns_context.nsDataSize))
                self.LogInfo("nsid:{} using meta size {}".format(nsid,ns_context.nsMetaSize))
                #clear ns_identify_buf for next ns use
                ns_identify_buf.FillZero(0,4096)
        
        ns_identify_buf.SetEndianMode(xBuffer.enEndianMode.BigEndian)
        # check ns map
        ctrlr_context.DumpNsMap()

        identify_buf.SetEndianMode(xBuffer.enEndianMode.BigEndian)
        return ctrlr_context
    
    def _init_xPattern(self):
        pattern_manager = xPattern.IPatternMngr(xPattern.stPatRecType.PATREC_BITMAP,True,self.buf_manager,self.ctrlr_context)

        return pattern_manager

    def load_context(self,nsid = None):
        nsid = 1 if nsid == None else nsid

        self.admin_cmd_set.LoadParentContext(self.ctrlr_context)

        self.io_cmd_set.LoadChildContext(self.ctrlr_context.GetChildContext(nsid))

    def io_test(self):
        # prepare write buf
        if self.admin_cmd_set.LoadPatternMngr(self.pattern_manager) is False :
            self.LogError("admin load pattern manager fail")
        if self.io_cmd_set.LoadPatternMngr(self.pattern_manager) is False :
            self.LogError("io load pattern manager fail")

        nsid = 1
        self.buf_manager.SectorSize = self.ctrlr_context.GetChildContext(nsid).nsDataSize
        io_write_buf = self.buf_manager.GetIOWrBuf(nsid,0,self.dev.driver)
        io_read_buf  = self.buf_manager.GetIORdBuf(nsid,0,self.dev.driver)

        length = 4096 // self.buf_manager.SectorSize
        for idx in range(0,length) :
            pattern = self.pattern_manager.GetPatternByIndex(random.randint(1,15))
            
            io_write_buf.FillSector(idx,1,pattern)
            # io_write_buf.MarkAddrOverlay(idx,1,idx*self.buf_manager.SectorSize)

        # self.LogInfo("Dump write buffer")
        # io_write_buf.Dump(0,length * self.buf_manager.SectorSize)

        # ret = self.pattern_manager.UptPatRecFromIOWrBuf(io_write_buf,0,nsid,0,length)

        # if ret is True :
        #     self.LogInfo("update pattern recorder success")
        # else :
        #     self.LogError("update pattern recorder fail")

        self.LogInfo("write")
        self.io_cmd_set.Write(0,length-1,0,0,0,0,0,io_write_buf,None,8000,{})
        self.LogInfo("read")
        self.io_cmd_set.Read(0,length-1,0,False,False,False,0,0,0,0,0,io_read_buf,None,8000)

        self.LogInfo("Dump read buffer")
        io_read_buf.Dump(0,length * self.buf_manager.SectorSize)
        ret = self.pattern_manager.CompareWithIORdBuf(io_read_buf,0,nsid,0,length,self.dev.driver)
        if ret is True :
            self.LogInfo("compare pattern recorder success")
        else :
            self.LogError("compare pattern recorder fail")


    def power_test(self) :
        #close dev
        dev_id = self.dev.bdfID
        dev_driver = self.dev.driver
        self.LogInfo("close dev,dev id is {},driver is {}".format(dev_id,dev_driver))
        self.ssd.CloseDev()

        time.sleep(10)

        #restore dev
        self.LogInfo("restore dev")
        self.dev = self.ssd.RestoreDev(dev_id,dev_driver)
        self.LogDebug("current driver is : {},bdf is {}".format(self.dev.driver , self.dev.bdf))
        

        #regenerate admin cmd set and io cmd set
        self.admin_cmd_set = self.cmd_set.GetCtrlr(self.dev)
        self.io_cmd_set = self.cmd_set.GetNS(self.dev,1)

        #test again
        self.load_context()
        self.io_test()

if __name__ == '__main__' :
    test = demo()
    test.io_test()
