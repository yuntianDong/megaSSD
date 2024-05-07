import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))), "bin"))

import unittest
import xDevice
import subprocess,re

class envParam():
    """
        catch Environment Parameters
    """
    def __init__(self) -> None:
        pass

    def runShell(self , args ,getoutput = False):
        if getoutput:
            cmd = subprocess.getoutput(args)
            return cmd
        else:
            cmd = subprocess.Popen(args, stdin=subprocess.PIPE, stderr=sys.stderr, close_fds=True,
                                    stdout=sys.stdout, universal_newlines=True, shell=True, bufsize=1)
            cmd.communicate()
            return cmd.returncode
        
    def getDriverInUse(self,bdf):
        cmdline = "lspci -s {} -v".format(bdf)
        output = self.runShell(cmdline,getoutput=True)
        pattern_driver = re.compile(r"Kernel driver in use:.(\w+)")
        return pattern_driver.findall(output)

class ut_xDevice(unittest.TestCase):
    """
        unitest for moudle xDevice
    """
    @classmethod
    def setUpClass(self):
        self._envparam = envParam()
        self._dev = xDevice.IHostDevice("0000:05:00.0")
        self._devnvme0 = self._dev.GetNVMeDevice(0)
        self._devpcie0 = self._dev.GetPcieDevice(0)

    def test_device(self):
        curdriver = self._envparam.getDriverInUse("0000:05:00.0")
        self.assertEqual(curdriver,self._devnvme0.driver,"Driver is not same!")
        self.assertEqual("0000:05:00.0",self._devnvme0.bdf,"Device bdf have changed!")
        self.assertEqual(0,self._devnvme0.bdfID)

        self.assertEqual("pcie",self._devpcie0.driver,"Driver is not same!")
        self.assertEqual("0000:05:00.0",self._devnvme0.bdf,"Device bdf have changed!")
        self.assertEqual(0,self._devnvme0.bdfID)

        self.assertEqual(1,self._dev.NodeCnt,"Dev count should not over 1")

if __name__ == '__main__':
    unittest.main()