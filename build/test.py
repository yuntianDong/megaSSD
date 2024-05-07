import sys
import xDevice
import xNVMe
import xBuffer


class test :
    def __init__(self):
        self.ssd = xDevice.IHostDevice("0000:c1:00.0")
        self.dev = self.ssd.GetNVMeDevice(0)
        self.admin_cmdset = self.get_admin_cmdset()
        self.buf_manager = self.get_buf_manager()


    # def get_device(self):
    #     ssd = xDevice.IHostDevice("0000:05:00.0")
    #     dev = ssd.GetNVMeDevice(0)
    #     admin_cmd_set = xNVMe.NVMeAdminCmd(dev)
    #     print(admin_cmd_set)
    #     dev.Getengin()
    #     self.dev = dev
    #     self.dev.Getengin()
    #     # return dev

    def get_admin_cmdset(self):
        admin_cmd_set = xNVMe.NVMeAdminCmd(self.dev)

        return admin_cmd_set

    def get_buf_manager(self):
        buf_manager = xBuffer.GetBufMngr()

        return buf_manager

    def test_identify(self):
        identify_buf = self.buf_manager.GetUsrBuf("Identify",4096,0,self.dev.driver)
        resp = self.admin_cmdset.Identify(1,0,0,identify_buf,8000)
        identify_buf.Dump(0,4096)

if __name__ == '__main__':
    unit = test()
    sys.exit(test.test_identify())








