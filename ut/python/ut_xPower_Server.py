import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))), "bin"))

import xPower
s = xPower.SCPIPwrMngrServer("/dev/ttyS1")
s.Run()