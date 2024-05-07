import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))), "bin"))

import unittest
import xBuffer
import xSM234

TST_SM2_USR_ID  = "12345678abcdefgh"
EXP_SM2_USR_ID  = [0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68]
DEF_PRIVATE_KEY = [0xd7,0xd3,0x37,0x9d,0x36,0xec,0x9f,0xe3,0xc7,0xfc,0x01,0xf2,0x87,0xfc,0x7b,0xef,
                   0xfd,0xff,0x62,0xeb,0xa9,0xd5,0x63,0xb1,0xd2,0xe7,0x71,0x47,0xcb,0xee,0x8f,0x19]
DEF_PUBLIC_KEY  = [0x04,0x66,0xb0,0x2d,0xb4,0x74,0x82,0x4c,0x2a,0xf1,0x2b,0x8c,0x1d,0xb9,0x4d,0xa2,
                   0xa0,0x0f,0x14,0x8c,0x49,0x5c,0x33,0xd2,0x03,0x2b,0x36,0x4a,0x52,0x3b,0x74,0x25,
                   0x2a,0xff,0xfe,0x7c,0x24,0xcd,0x5d,0xbb,0x3a,0x0e,0x65,0xab,0x7b,0x16,0xf0,0x8f,
                   0x4e,0x2f,0x83,0xf9,0x61,0x54,0x00,0x01,0x34,0x45,0x7e,0xa4,0xab,0x79,0xb5,0xa7,0xa7]
EXP_E_KEY       = [0x3a,0x79,0x6a,0x19,0xa0,0xfd,0x28,0x94,0x0e,0xf0,0x59,0xe1,0x7d,0xd2,0x8f,0xfb,
                   0xf9,0x8f,0x50,0x28,0x86,0x4e,0x08,0x59,0x6f,0x3b,0xca,0x06,0x8a,0x0e,0xa1,0x27]
EXP_Z_KEY       = [0x39,0x79,0xeb,0x16,0x57,0xf8,0xbd,0xf9,0x99,0x39,0x36,0x26,0xdb,0xfe,0x21,0xd4,
                   0x46,0x6e,0x74,0x3c,0x3f,0x08,0x69,0x53,0xf5,0xe5,0x26,0x68,0xc4,0xe1,0x13,0x26]
EXP_SIGNATURE   = [0xa1,0x29,0x97,0xce,0x15,0x7f,0x74,0xbf,0x00,0x1b,0xe5,0xff,0x37,0x20,0x32,0x9c,
                   0x08,0xa3,0xdc,0x71,0xe2,0x81,0xda,0x5c,0x9a,0x72,0x14,0x58,0xc5,0x82,0xc6,0x51,
                   0x3f,0x23,0x53,0xe2,0xb6,0x0d,0xa8,0xd1,0x47,0x65,0x1a,0x8e,0x88,0xdb,0x2f,0xd1,
                   0xcd,0xff,0x5a,0x25,0x06,0x5b,0x2a,0x1e,0x7b,0x0a,0x4c,0x9c,0xfc,0xfb,0xfe,0x59]

class ut_OSRSM2(unittest.TestCase):
    """
        unittest for xSM234.OSRSM2 module
    """
    @classmethod
    def setUpClass(self):
        self._sm2 = xSM234.OSRSM2(TST_SM2_USR_ID);
        
    def test_UserID(self):
        kBuf = xBuffer.GetBufMngr().GetUsrBuf("SM2Key",4096,0)
        kBuf.FillZero(0,4096)
        
        self._sm2.GetUsrID(kBuf,0)
        self.assertListEqual(EXP_SM2_USR_ID,kBuf[0:16],"User ID is not correct")
        
    def test_SetGetFunc(self):
        kBuf = xBuffer.GetBufMngr().GetUsrBuf("SM2Key",4096,0)        
        self._sm2.SetPKey(kBuf,0,32)
        self._sm2.SetEKey(kBuf,100)
        self._sm2.SetZKey(kBuf,200)

        kBuf[0:32]      = DEF_PRIVATE_KEY
        kBuf[32:97]     = DEF_PUBLIC_KEY
        kBuf[100:132]   = EXP_E_KEY
        kBuf[200:232]   = EXP_Z_KEY
        
        self._sm2.SetPKey(kBuf,0,32)
        self._sm2.SetEKey(kBuf,100)
        self._sm2.SetZKey(kBuf,200)
        
        kBuf.FillZero(0,4096)
        self._sm2.GetPKey(kBuf,0,32)
        self.assertListEqual(DEF_PRIVATE_KEY, kBuf[0:32], "Private Key is not correct")
        self.assertListEqual(DEF_PUBLIC_KEY, kBuf[32:97], "Public Key is not correct")
        
        kBuf.FillZero(0,4096)
        self._sm2.GetEKey(kBuf,0)
        self._sm2.GetZKey(kBuf,32)
        self.assertListEqual(EXP_E_KEY, kBuf[0:32], "E Key is not correct")
        self.assertListEqual(EXP_Z_KEY, kBuf[32:64], "Z Key is not correct") 
        
        buf = xBuffer.GetBufMngr().GetUsrBuf("SM2MBuf",4096,0)
        buf.FillHexF(0,4096)
        
        self.assertTrue(self._sm2.Sign(buf,4096),"Fail to Sign()")
        self.assertTrue(self._sm2.Verify(),"Fail to Verify()")       
        
    def test_SignAndVerify(self):
        buf = xBuffer.GetBufMngr().GetUsrBuf("SM2MBuf",4096,0)
        buf.FillHexF(0,4096)
        
        self.assertTrue(self._sm2.Sign(buf,4096),"Fail to Sign()")
        self.assertTrue(self._sm2.Verify(),"Fail to Verify()")
        
        kBuf = xBuffer.GetBufMngr().GetUsrBuf("SM2Key",4096,0)
        kBuf.FillZero(0,4096)
        
        self._sm2.GetPKey(kBuf,0,32)
        self.assertListEqual(DEF_PRIVATE_KEY, kBuf[0:32], "Private Key is not correct")
        self.assertListEqual(DEF_PUBLIC_KEY, kBuf[32:97], "Public Key is not correct")
        
        kBuf.FillZero(0,4096)
        self._sm2.GetEKey(kBuf,0)
        self._sm2.GetZKey(kBuf,32)
        self.assertListEqual(EXP_E_KEY, kBuf[0:32], "E Key is not correct")
        self.assertListEqual(EXP_Z_KEY, kBuf[32:64], "Z Key is not correct")
        
        #kBuf.FillZero(0,4096)
        #self._sm2.GetSign(kBuf,0)
        #self.assertListEqual(EXP_SIGNATURE, kBuf[0:64], "Signature is not correct")
        
    def test_EnAndDecrypt(self):
        bufM = xBuffer.GetBufMngr().GetUsrBuf("SM2MBuf",4096,0)
        bufC = xBuffer.GetBufMngr().GetUsrBuf("SM2CBuf",4096,0)
        bufM2 = xBuffer.GetBufMngr().GetUsrBuf("SM2M2Buf",4096,0)
        
        bufM.FillHexF(0,4096)
        bufM2.FillZero(0,4096)
        
        cBytes      = self._sm2.Encrypt(bufM,2048,bufC)
        self.assertEqual(cBytes, 2048 + 97, "Fail to Encrypt()")
        mBytes      = self._sm2.Decrypt(bufC,cBytes,bufM2)
        self.assertEqual(mBytes, 2048, "Fail to Decrypt()")
        
        errPos,_srcVal,_dstVal = bufM.Compare(bufM2,0,0,2048)
        self.assertEqual(errPos,0xFFFFFFFF,"Encrypt() == Decrypt()")


EXP_DIGEST      = [0x99,0x6d,0x9c,0xcd,0x12,0x72,0xa2,0x5d,0x57,0x4e,0xd0,0x5a,0xaa,0x72,0xc6,0xcf,
                   0xd9,0x73,0x6d,0x3c,0xdd,0x0f,0xf7,0x2a,0x45,0x03,0x1f,0x6a,0x1c,0x40,0x92,0xba]

class ut_OSRSM3(unittest.TestCase):
    """
        unittest for xSM234.OSRSM3 module
    """
    @classmethod
    def setUpClass(self):
        self._sm3 = xSM234.OSRSM3();
        
    def test_HashStep(self):
        buf = xBuffer.GetBufMngr().GetUsrBuf("SM3MBuf",4096,0)
        buf.FillZero(0,4096)
        
        self.assertTrue(self._sm3.Step1_Init(),"Fail to Step1_Init()")
        self.assertTrue(self._sm3.Step2_Process(buf,4096),"Fail to Step2_Process()")
        self.assertTrue(self._sm3.Step3_Done(),"Fail to Step3_Done()")
        
        dBuf = xBuffer.GetBufMngr().GetUsrBuf("SM3Dig",4096,0)
        
        self.assertTrue(self._sm3.GetDigest(dBuf,0),"Fail to GetDigest()")
        self.assertListEqual(dBuf[0:32],EXP_DIGEST,"Digest is not expected");
        
    def test_HashOnce(self):
        buf = xBuffer.GetBufMngr().GetUsrBuf("SM3MBuf",4096,0)
        buf.FillZero(0,4096)
        
        self.assertTrue(self._sm3.GetDigestHash(buf,4096),"Fail to GetDigestHash()")
        dBuf = xBuffer.GetBufMngr().GetUsrBuf("SM3Dig",4096,0)
        
        self.assertTrue(self._sm3.GetDigest(dBuf,0),"Fail to GetDigest()")
        self.assertListEqual(dBuf[0:32],EXP_DIGEST,"Digest is not expected");
        
EXP_SM4_KEY = [0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68]
EXP_SM4_IV  = [0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10]
EXP_SM4_CTR = EXP_SM4_IV
        
class ut_OSRSM4(unittest.TestCase):
    """
        unittest for xSM234.OSRSM4 module
    """
    @classmethod
    def setUpClass(self):
        self._sm4 = xSM234.OSRSM4();
        
        buf = xBuffer.GetBufMngr().GetUsrBuf("SM4Key",4096,0)
        buf[0:16]   = EXP_SM4_KEY
        buf[100:116]= EXP_SM4_IV
        buf[200:216]= EXP_SM4_CTR
        
        self._sm4.SetSM4Key(buf,0)
        self._sm4.SetSM4IV(buf,100)
        self._sm4.SetSM4CTR(buf,200)
        
    def test_ECBMode(self):
        self._sm4.SM4Mode = xSM234.enSM4Mode.SM4_M_ECB
        
        bufM1 = xBuffer.GetBufMngr().GetUsrBuf("SM4M1Buf",4096,0)
        bufM2 = xBuffer.GetBufMngr().GetUsrBuf("SM4M2Buf",4096,0)
        bufC  = xBuffer.GetBufMngr().GetUsrBuf("SM4CBuf",4096,0)
        
        self.assertTrue(self._sm4.Encrypt(bufM1,4096,bufC),"Fail to Encrypt()")
        self.assertTrue(self._sm4.Decrypt(bufC,4096,bufM2),"Fail to Decrypt()")
        
        errPos,_srcVal,_dstVal = bufM1.Compare(bufM2,0,0,2048)
        self.assertEqual(errPos,0xFFFFFFFF,"Encrypt() == Decrypt()")
        
    def test_CBCMode(self):
        self._sm4.SM4Mode = xSM234.enSM4Mode.SM4_M_CBC
        
        bufM1 = xBuffer.GetBufMngr().GetUsrBuf("SM4M1Buf",4096,0)
        bufM2 = xBuffer.GetBufMngr().GetUsrBuf("SM4M2Buf",4096,0)
        bufC  = xBuffer.GetBufMngr().GetUsrBuf("SM4CBuf",4096,0)
        
        self.assertTrue(self._sm4.Encrypt(bufM1,4096,bufC),"Fail to Encrypt()")
        self.assertTrue(self._sm4.Decrypt(bufC,4096,bufM2),"Fail to Decrypt()")
        
        errPos,_srcVal,_dstVal = bufM1.Compare(bufM2,0,0,2048)
        self.assertEqual(errPos,0xFFFFFFFF,"Encrypt() == Decrypt()")
        
    def test_CFB128Mode(self):
        self._sm4.SM4Mode = xSM234.enSM4Mode.SM4_M_CFB_128B
        
        bufM1 = xBuffer.GetBufMngr().GetUsrBuf("SM4M1Buf",4096,0)
        bufM2 = xBuffer.GetBufMngr().GetUsrBuf("SM4M2Buf",4096,0)
        bufC  = xBuffer.GetBufMngr().GetUsrBuf("SM4CBuf",4096,0)
        
        self.assertTrue(self._sm4.Encrypt(bufM1,4096,bufC),"Fail to Encrypt()")
        self.assertTrue(self._sm4.Decrypt(bufC,4096,bufM2),"Fail to Decrypt()")
        
        errPos,_srcVal,_dstVal = bufM1.Compare(bufM2,0,0,2048)
        self.assertEqual(errPos,0xFFFFFFFF,"Encrypt() == Decrypt()")
        
    def test_OFB128Mode(self):
        self._sm4.SM4Mode = xSM234.enSM4Mode.SM4_M_OFB_128B
        
        bufM1 = xBuffer.GetBufMngr().GetUsrBuf("SM4M1Buf",4096,0)
        bufM2 = xBuffer.GetBufMngr().GetUsrBuf("SM4M2Buf",4096,0)
        bufC  = xBuffer.GetBufMngr().GetUsrBuf("SM4CBuf",4096,0)
        
        self.assertTrue(self._sm4.Encrypt(bufM1,4096,bufC),"Fail to Encrypt()")
        self.assertTrue(self._sm4.Decrypt(bufC,4096,bufM2),"Fail to Decrypt()")
        
        errPos,_srcVal,_dstVal = bufM1.Compare(bufM2,0,0,2048)
        self.assertEqual(errPos,0xFFFFFFFF,"Encrypt() == Decrypt()")
        
    def test_CTRMode(self):
        self._sm4.SM4Mode = xSM234.enSM4Mode.SM4_M_CTR
        
        bufM1 = xBuffer.GetBufMngr().GetUsrBuf("SM4M1Buf",4096,0)
        bufM2 = xBuffer.GetBufMngr().GetUsrBuf("SM4M2Buf",4096,0)
        bufC  = xBuffer.GetBufMngr().GetUsrBuf("SM4CBuf",4096,0)
        
        self.assertTrue(self._sm4.Encrypt(bufM1,4096,bufC),"Fail to Encrypt()")
        self.assertTrue(self._sm4.Decrypt(bufC,4096,bufM2),"Fail to Decrypt()")
        
        errPos,_srcVal,_dstVal = bufM1.Compare(bufM2,0,0,2048)
        self.assertEqual(errPos,0xFFFFFFFF,"Encrypt() == Decrypt()")
        
    def test_XTSStepMode(self):
        self._sm4.SM4Mode = xSM234.enSM4Mode.SM4_M_XTS_STEP
        
        bufM1 = xBuffer.GetBufMngr().GetUsrBuf("SM4M1Buf",4096,0)
        bufM2 = xBuffer.GetBufMngr().GetUsrBuf("SM4M2Buf",4096,0)
        bufC  = xBuffer.GetBufMngr().GetUsrBuf("SM4CBuf",4096,0)
        
        self.assertTrue(self._sm4.Encrypt(bufM1,4096,bufC),"Fail to Encrypt()")
        self.assertTrue(self._sm4.Decrypt(bufC,4096,bufM2),"Fail to Decrypt()")
        
        errPos,_srcVal,_dstVal = bufM1.Compare(bufM2,0,0,2048)
        self.assertEqual(errPos,0xFFFFFFFF,"Encrypt() == Decrypt()")
        
    def test_XTSOnceMode(self):
        self._sm4.SM4Mode = xSM234.enSM4Mode.SM4_M_XTS_ONCE
        
        bufM1 = xBuffer.GetBufMngr().GetUsrBuf("SM4M1Buf",4096,0)
        bufM2 = xBuffer.GetBufMngr().GetUsrBuf("SM4M2Buf",4096,0)
        bufC  = xBuffer.GetBufMngr().GetUsrBuf("SM4CBuf",4096,0)
        
        self.assertTrue(self._sm4.Encrypt(bufM1,4096,bufC),"Fail to Encrypt()")
        self.assertTrue(self._sm4.Decrypt(bufC,4096,bufM2),"Fail to Decrypt()")
        
        errPos,_srcVal,_dstVal = bufM1.Compare(bufM2,0,0,2048)
        self.assertEqual(errPos,0xFFFFFFFF,"Encrypt() == Decrypt()")
        
    
if __name__ == '__main__':
    unittest.main()