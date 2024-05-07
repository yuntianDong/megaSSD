import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))), "bin"))

import unittest
import xPower

class ut_xPower_Client(unittest.TestCase):
    """
        unittest for xPower module
    """
    @classmethod
    def setUpClass(self):
        self._Client   = xPower.SCPIPwrMngrClient(3)
        
    @classmethod  
    def tearDownClass(self):
        self._Client.CloseService()
        
    def test_SyncFunc(self):
        self.assertTrue(self._Client.PowerOff(),"Fail to Power Off")
        self.assertTrue(False == self._Client.IsPwrOn,"Should be Power Off")
        self.assertGreater(self._Client.Voltage, 4800, "Should be 5V")
        self.assertEqual(self._Client.Current,0,"Should be 0A")
        self.assertEqual(self._Client.Power,0,"Should be 0W")
        
        self.assertTrue(self._Client.PowerOn(),"Fail to Power On")
        self.assertTrue(True == self._Client.IsPwrOn,"Should be Power On")
        self.assertGreater(self._Client.Voltage, 4800, "Should be 5V")
        self.assertGreater(self._Client.Current,0,"Shouldn't be 0A")
        self.assertGreater(self._Client.Power,0,"Shouldn't be 0W")
        
    def test_AsyncFunc(self):
        self.assertTrue(self._Client.AsyncPowerOff(20),"Fail to Async Power Off")
        self.assertTrue(self._Client.WaitForDone(0),"Fail to Wait AsyncPowerOff Done")
        
        self.assertTrue(self._Client.AsyncPowerOn(20),"Fail to Async Power On")
        self.assertTrue(self._Client.WaitForDone(0),"Fail to Wait AsyncPowerOn Done")

        self.assertTrue(self._Client.AsyncB2BPowerCycle(10,0,0),"Fail to B2B Power Cycle")
        self.assertTrue(self._Client.WaitForDone(0),"Fail to Wait B2BPowerCycle Done")

if __name__ == '__main__':
    unittest.main()