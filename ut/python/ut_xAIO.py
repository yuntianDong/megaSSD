import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))), "bin"))

import unittest
import xAIO

TARGET_FILENAME = "/dev/nvme0n1"
TEST_OFFSET     = 0
TEST_SIZE       = 1*1024*1024*1024
TEST_EXECTIME   = 60

class ut_xAIO(unittest.TestCase):
    """
        unittest for xAIO module
    """
    @classmethod
    def setUpClass(self):
        self._aio = xAIO.IAIOPerf(TARGET_FILENAME,True);
        
    def test_seqRead(self):
        rslt = self._aio.SeqRead(TEST_OFFSET,TEST_SIZE,TEST_EXECTIME);
        
        self.assertGreater(rslt.IOPS, 10, "IOPS > 10")
        self.assertGreater(rslt.MBPS, 1,  "MBPS >  1")
    
    def test_seqWrite(self):
        rslt = self._aio.SeqWrite(TEST_OFFSET,TEST_SIZE,TEST_EXECTIME);
        
        self.assertGreater(rslt.IOPS, 10, "IOPS > 10")
        self.assertGreater(rslt.MBPS, 1,  "MBPS >  1")
    
    def test_randRead(self):
        rslt = self._aio.RandRead(TEST_OFFSET,TEST_SIZE,TEST_EXECTIME);
        
        self.assertGreater(rslt.IOPS, 10, "IOPS > 10")
        self.assertGreater(rslt.MBPS, 1,  "MBPS >  1")
    
    def test_randWrite(self):
        rslt = self._aio.RandWrite(TEST_OFFSET,TEST_SIZE,TEST_EXECTIME);
        
        self.assertGreater(rslt.IOPS, 10, "IOPS > 10")
        self.assertGreater(rslt.MBPS, 1,  "MBPS >  1")
        
if __name__ == '__main__':
    unittest.main()