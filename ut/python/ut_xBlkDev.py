import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))), "bin"))

import unittest
import xBlkDev

class ut_xBlkDev(unittest.TestCase):
    """
        unittest for xBlkDev module
    """
    @classmethod
    def setUpClass(self):
        self._bdMngr   = xBlkDev.GetBlkDevMngr("/dev/nvme0n1")
        
    def test_BlkDiscard(self):
        self.assertTrue(self._bdMngr.BlkDiscard(0,4096,0),"Fail to call BlkDiscard()")
        
    def test_BlkDiscardWithSecu(self):
        self.assertTrue(self._bdMngr.BlkDiscardWithSecu(0,4096,0),"Fail to call BlkDiscardWithSecu()")
        
    def test_ZeroOut(self):
        self.assertTrue(self._bdMngr.ZeroOut(0,4096,0),"Fail to call ZeroOut()")
        
    def test_FlushBuf(self):
        self.assertTrue(self._bdMngr.FlushBuf(0),"Fail to call FlushBuf()")
        
    def test_GetAttr(self):
        self.assertGreater(self._bdMngr.TotalSize, 0, "TotalSize > 0")
        self.assertGreater(self._bdMngr.SoftBlockSize, 0, "SoftBlockSize > 0")
        self.assertGreater(self._bdMngr.LogBlockSize, 0, "LogBlockSize > 0")
        self.assertGreater(self._bdMngr.PhyBlockSize, 0, "PhyBlockSize > 0")
        self.assertGreater(self._bdMngr.IOMin, 0, "IOMin > 0")
        self.assertGreater(self._bdMngr.IOOpt, 0, "IOOpt > 0")
        self.assertGreater(self._bdMngr.AlignOffset, 0, "AlignOffset > 0")
        self.assertGreater(self._bdMngr.MaxSectors, 0, "MaxSectors > 0")
        
        self.assertFalse(self._bdMngr.DiskROFlag, "DiskROFlag == false")
        
if __name__ == '__main__':
    unittest.main()