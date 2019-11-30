# Python Script
from sys import argv
from os.path import splitext, basename

import pysimulavr
from ex_utils import SimulavrAdapter

class XPin(pysimulavr.Pin):
  
  def __init__(self, name, pin):
    pysimulavr.Pin.__init__(self)
    self.name = name
    self.__net = pysimulavr.Net()
    self.__net.Add(self)
    self.__net.Add(pin)
    
  def SetInState(self, pin):
    pysimulavr.Pin.SetInState(self, pin)
    print("%s set to '%s' (t=%dns)" % (self.name, pin.toChar(), sim.getCurrentTime()))

  def __del__(self):
    del self.__net

if __name__ == "__main__":

  proc, elffile = argv[1].split(":")
  
  sim = SimulavrAdapter()
  sim.dmanSingleDeviceApplication()
  dev = sim.loadDevice(proc, elffile)
  #dev.SetClockFreq(100)
  
  sigs = ("IRQ.VECTOR9", "PORTA.PORT")
  sim.setVCDDump(splitext(basename(argv[0]))[0] + ".vcd", sigs)
    
  xpin = XPin("port A.0", dev.GetPin("A0"))
    
  sim.dmanStart()
  
  print("simulation start: (t=%dns)" % sim.getCurrentTime())
  sim.doRun(15000000)
  print("simulation end: (t=%dns)" % sim.getCurrentTime())

  print("value 'timer2_ticks'=%d" % sim.getWordByName(dev, "timer2_ticks"))
  
  sim.dmanStop()
  del dev
  
# EOF
