import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))), "bin"))

import unittest
import xBuffer

class ut_xBuffer(unittest.TestCase):
    """
        unittest for xBuffer module
    """
    @classmethod
    def setUpClass(self):
        self._bufMngr   = xBuffer.GetBufMngr()
    
    def test_CfgBufMngr(self):
        ioBufItemSize   = self._bufMngr.IOBufItemSize
        ioBufMaxSize    = self._bufMngr.IOBufMaxSize
        metaBufItemSize = self._bufMngr.MetaBufItemSize
        sysBufMaxSize   = self._bufMngr.SysBufMaxSize
        frmBufMaxSize   = self._bufMngr.FrmBufMaxSize
        
        self.assertGreater(ioBufItemSize, 0, "IOBufItemSize should not be 0")
        self.assertGreater(ioBufMaxSize, 0, "IOBufMaxSize should not be 0")
        self.assertGreater(metaBufItemSize, 0, "MetaBufItemSize should not be 0")
        self.assertGreater(sysBufMaxSize, 0, "SysBufMaxSize should not be 0")
        self.assertGreater(frmBufMaxSize, 0, "FrmBufMaxSize should not be 0")
        
        tst_ioBufItemSize   = 512*1024
        tst_ioBufMaxSize    = 32*1024*1024
        tst_metaBufItemSize = 512
        tst_sysBufMaxSize   = 16*1024*1024
        tst_frmBufMaxSize   = 8*1024*1024
        
        self._bufMngr.IOBufItemSize     = tst_ioBufItemSize
        self._bufMngr.IOBufMaxSize      = tst_ioBufMaxSize
        self._bufMngr.MetaBufItemSize   = tst_metaBufItemSize
        self._bufMngr.SysBufMaxSize     = tst_sysBufMaxSize
        self._bufMngr.FrmBufMaxSize     = tst_frmBufMaxSize
        
        self.assertEqual(self._bufMngr.IOBufItemSize, tst_ioBufItemSize, "IOBufItemSize is not changed")
        self.assertEqual(self._bufMngr.IOBufMaxSize, tst_ioBufMaxSize, "IOBufMaxSize is not changed")
        self.assertEqual(self._bufMngr.MetaBufItemSize, tst_metaBufItemSize, "MetaBufItemSize is not changed")
        self.assertEqual(self._bufMngr.SysBufMaxSize, tst_sysBufMaxSize, "SysBufMaxSize is not changed")
        self.assertEqual(self._bufMngr.FrmBufMaxSize, tst_frmBufMaxSize, "FrmBufMaxSize is not changed")
        
        self._bufMngr.IOBufItemSize     = ioBufItemSize
        self._bufMngr.IOBufMaxSize      = ioBufMaxSize
        self._bufMngr.MetaBufItemSize   = metaBufItemSize
        self._bufMngr.SysBufMaxSize     = sysBufMaxSize
        self._bufMngr.FrmBufMaxSize     = frmBufMaxSize
        
        self.assertEqual(self._bufMngr.IOBufItemSize, ioBufItemSize, "IOBufItemSize is not changed back")
        self.assertEqual(self._bufMngr.IOBufMaxSize, ioBufMaxSize, "IOBufMaxSize is not changed back")
        self.assertEqual(self._bufMngr.MetaBufItemSize, metaBufItemSize, "MetaBufItemSize is not changed back")
        self.assertEqual(self._bufMngr.SysBufMaxSize, sysBufMaxSize, "SysBufMaxSize is not changed back")
        self.assertEqual(self._bufMngr.FrmBufMaxSize, frmBufMaxSize, "FrmBufMaxSize is not changed back")
        
    def test_ioBuf(self):
        ioBufItemSize   = self._bufMngr.IOBufItemSize
        ioBufMaxSize    = self._bufMngr.IOBufMaxSize
        
        rdBuf0  = self._bufMngr.GetIORdBuf(1,0)
        rdBuf1  = self._bufMngr.GetIORdBuf(1,1)
        rdBuf2  = self._bufMngr.GetIORdBuf(2,0)
        self.assertNotEqual(rdBuf0.GetBufAddr(), rdBuf1.GetBufAddr(), "Different BufIdx Buffer should be different.")
        self.assertNotEqual(rdBuf0.GetBufAddr(), rdBuf2.GetBufAddr(), "Different NSID  Buffer should be different.")
        self.assertEqual(rdBuf0.GetBufSize(),ioBufItemSize,"Buffer Size should be equal to ioBufItemSize")
        self.assertEqual(rdBuf1.GetBufSize(),ioBufItemSize,"Buffer Size should be equal to ioBufItemSize")
        self.assertEqual(rdBuf2.GetBufSize(),ioBufItemSize,"Buffer Size should be equal to ioBufItemSize")
        
        wrBuf0  = self._bufMngr.GetIOWrBuf(1,0)
        self.assertEqual(wrBuf0.GetBufSize(),ioBufItemSize,"Buffer Size should be equal to ioBufItemSize")
        self.assertNotEqual(rdBuf0.GetBufAddr(), wrBuf0.GetBufAddr(), "Read/Write Buffer should be different")
        
        wrBuf1  = self._bufMngr.GetIOWrBuf(1,0)
        self.assertEqual(wrBuf1.GetBufSize(),ioBufItemSize,"Buffer Size should be equal to ioBufItemSize")
        self.assertEqual(wrBuf1.GetBufAddr(), wrBuf0.GetBufAddr(), "Same Parameter Buffer should be the same.")
        
        oldRdBuf    = rdBuf0
        oldWrBuf    = wrBuf0
        for idx in range(1024):
            rdBuf   = self._bufMngr.GetIORdBuf(0,idx)
            wrBuf   = self._bufMngr.GetIOWrBuf(0,idx)
            
            self.assertEqual(rdBuf.GetBufSize(),ioBufItemSize,"Buffer Size should be equal to ioBufItemSize")
            self.assertEqual(wrBuf.GetBufSize(),ioBufItemSize,"Buffer Size should be equal to ioBufItemSize")
            self.assertNotEqual(rdBuf.GetBufAddr(), wrBuf.GetBufAddr(), "Read/Write Buffer should be different")
            self.assertNotEqual(rdBuf.GetBufAddr(), oldRdBuf.GetBufAddr(), "Different bufIdx Read Buffer should be different")  
            self.assertNotEqual(wrBuf.GetBufAddr(), oldWrBuf.GetBufAddr(), "Different bufIdx Read Buffer should be different")  
            
            self.assertLessEqual(self._bufMngr.IOBufAllocBytes, ioBufMaxSize, "Allocated Bytes should be less than IOBufMaxSize")
            
            oldRdBuf    = rdBuf
            oldWrBuf    = wrBuf
            
    def test_metaBuf(self):
        tst_sysBufMaxSize   = 1*1024*1024
        org_sysBufMaxSize   = self._bufMngr.SysBufMaxSize
        
        self._bufMngr.SysBufMaxSize = tst_sysBufMaxSize
        
        metaBufItemSize = self._bufMngr.MetaBufItemSize
        
        rdBuf0  = self._bufMngr.GetMetaRdBuf(1,0)
        rdBuf1  = self._bufMngr.GetMetaRdBuf(1,1)
        rdBuf2  = self._bufMngr.GetMetaRdBuf(2,0)
        self.assertNotEqual(rdBuf0.GetBufAddr(), rdBuf1.GetBufAddr(), "Different BufIdx Buffer should be different.")
        self.assertNotEqual(rdBuf0.GetBufAddr(), rdBuf2.GetBufAddr(), "Different NSID  Buffer should be different.")
        self.assertEqual(rdBuf0.GetBufSize(),metaBufItemSize,"Buffer Size should be equal to ioBufItemSize")
        self.assertEqual(rdBuf1.GetBufSize(),metaBufItemSize,"Buffer Size should be equal to ioBufItemSize")
        self.assertEqual(rdBuf2.GetBufSize(),metaBufItemSize,"Buffer Size should be equal to ioBufItemSize")
        
        wrBuf0  = self._bufMngr.GetMetaWrBuf(1,0)
        self.assertNotEqual(rdBuf0.GetBufAddr(), wrBuf0.GetBufAddr(), "Read/Write Buffer should be different")
        self.assertEqual(rdBuf2.GetBufSize(),metaBufItemSize,"Buffer Size should be equal to ioBufItemSize")
        
        wrBuf1  = self._bufMngr.GetMetaWrBuf(1,0)
        self.assertEqual(wrBuf1.GetBufAddr(), wrBuf0.GetBufAddr(), "Same Parameter Buffer should be the same.")
        self.assertEqual(wrBuf1.GetBufSize(),metaBufItemSize,"Buffer Size should be equal to ioBufItemSize")
        
        loopCnt = (tst_sysBufMaxSize // metaBufItemSize) // 2 + 10

        oldRdBuf    = rdBuf0
        oldWrBuf    = wrBuf0
        for idx in range(loopCnt):
            rdBuf   = self._bufMngr.GetMetaRdBuf(0,idx)
            wrBuf   = self._bufMngr.GetMetaWrBuf(0,idx)
            
            self.assertEqual(rdBuf.GetBufSize(),metaBufItemSize,"Buffer Size should be equal to ioBufItemSize")
            self.assertEqual(wrBuf.GetBufSize(),metaBufItemSize,"Buffer Size should be equal to ioBufItemSize")
            self.assertNotEqual(rdBuf.GetBufAddr(), wrBuf.GetBufAddr(), "Read/Write Buffer should be different")
            self.assertNotEqual(rdBuf.GetBufAddr(), oldRdBuf.GetBufAddr(), "Different bufIdx Read Buffer should be different")  
            self.assertNotEqual(wrBuf.GetBufAddr(), oldWrBuf.GetBufAddr(), "Different bufIdx Read Buffer should be different")  
            
            self.assertLessEqual(self._bufMngr.SysBufAllocBytes, tst_sysBufMaxSize, "Allocated Bytes should be less than SysBufAllocBytes")
            
            oldRdBuf    = rdBuf
            oldWrBuf    = wrBuf
            
        self._bufMngr.SysBufMaxSize = org_sysBufMaxSize
        
    def test_sysBuf(self):
        tst_sysBufMaxSize   = 1*1024*1024
        org_sysBufMaxSize   = self._bufMngr.SysBufMaxSize 
        self._bufMngr.SysBufMaxSize = tst_sysBufMaxSize
        
        baseSize    = 4*1024
        maxIndex    = (tst_sysBufMaxSize // baseSize) // 4
        oldBuf      = self._bufMngr.GetSysBuf("SYSBUF",baseSize,0)
        for idx in range(maxIndex):
            bufSize = (idx+1) * baseSize
            
            buf = self._bufMngr.GetSysBuf("SYSBUF%04d"%idx,bufSize,0)
            
            self.assertEqual(buf.GetBufSize(),bufSize,"Buffer Size should be equal to bufSize")
            self.assertNotEqual(oldBuf.GetBufAddr(), buf.GetBufAddr(), "oldBuf Buffer should be different with buf")
            self.assertLessEqual(self._bufMngr.SysBufAllocBytes, tst_sysBufMaxSize, "Allocated Bytes should be less than SysBufAllocBytes")
        
            oldBuf = buf
        
        self._bufMngr.SysBufMaxSize = org_sysBufMaxSize
        
    def test_frmBuf(self):
        tst_frmBufMaxSize   = 1*1024*1024
        org_frmBufMaxSize   = self._bufMngr.FrmBufMaxSize 
        self._bufMngr.FrmBufMaxSize = tst_frmBufMaxSize
        
        buf = self._bufMngr.GetFrmwkBuf("FRAMEBUF2",tst_frmBufMaxSize,0)
        self.assertEqual(buf.GetBufSize(),tst_frmBufMaxSize,"Buffer Size should be equal to bufSize")
        
        self._bufMngr.FrmBufMaxSize = org_frmBufMaxSize
        
    def test_bufferAPI(self):
        buf0    = self._bufMngr.GetIORdBuf(1,0)
        buf1    = self._bufMngr.GetMetaRdBuf(1,0)
        buf2    = self._bufMngr.GetSysBuf("SYSBUF",4096,0)
        buf3    = self._bufMngr.GetFrmwkBuf("FRAMEBUF",4096,0)
        
        for buf in (buf1,buf2,buf3):
            buf[0]  = 0xCA
            self.assertEqual(buf[0],0xCA,"buf[0] != buf[0]")

            self.assertEqual(len(buf),4096,"len(buf) == 4096")

            buf.SetByte(0,0xCA)
            self.assertEqual(buf.GetByte(0),0xCA,"SetByte() != GetByte()")
            
        for buf in (buf1,buf2):
            buf.SetWord(0,0x57CA)
            self.assertEqual(buf.GetWord(0),0x57CA,"SetWord() != GetWord()")
            buf.SetDWord(0,0xAC7557CA)
            self.assertEqual(buf.GetDWord(0),0xAC7557CA,"SetDWord() != GetDWord()")
            buf.SetQWord(0,0xAC7557CACA5775AC)
            self.assertEqual(buf.GetQWord(0),0xAC7557CACA5775AC,"SetQWord() != GetQWord()")
            
            buf.FillZero(0,4096)
            self.assertEqual(buf.GetDWord(0),0,"FillZero() != 0")
            buf.FillHexF(0,4096)
            self.assertEqual(buf.GetDWord(0),0xFFFFFFFF,"FillHexF() != 0xFFFFFFFF")
            
            buf.FillBytePat(0,4096,0xAC)
            self.assertEqual(buf.GetByte(0),0xAC,"FillBytePat() != GetByte()")
            buf.FillWordPat(0,4096,0xAC57)
            self.assertEqual(buf.GetWord(0),0xAC57,"FillWordPat() != GetWord()")
            buf.FillDWordPat(0,4096,0x57CAAC75)
            self.assertEqual(buf.GetDWord(0),0x57CAAC75,"FillDWordPat() != GetDWord()")
            buf.FillQWordPat(0,4096,0xCA5775ACAC7557CA)
            self.assertEqual(buf.GetQWord(0),0xCA5775ACAC7557CA,"FillQWordPat() != GetQWord()")
            
        for buf in (buf0,):
            buf.FillSector(0,1,0x57CAAC75)
            self.assertEqual(buf.GetDWord(0),0x57CAAC75,"FillSectorWithPattern() != GetDWord()")
            buf.MarkAddrOverlay(0,1,0x12345678)
            self.assertEqual(buf.GetQWord(0),0x12345678,"MarkAddrOverlay() != GetQWord(0)")
            self.assertTrue(buf.IsSectorBuf(),"buf should be SectorBuf")

if __name__ == '__main__':
    unittest.main()