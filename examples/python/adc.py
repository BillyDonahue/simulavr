# Python Script
from sys import argv

import pysimulavr
from ex_utils import SimulavrAdapter

class XPin(pysimulavr.Pin):
  
  def __init__(self, dev, name, state = None):
    pysimulavr.Pin.__init__(self)
    self.dev=dev
    self.name = name
    devpin = dev.GetPin(name)
    if state is not None: self.SetPin(state)
    # hold the connecting net here, it have not be destroyed, if we leave this method
    self.__net = pysimulavr.Net()
    self.__net.Add(self)
    self.__net.Add(devpin)

  def __del__(self):
    del self.__net

if __name__ == "__main__":

  proc, elffile = argv[1].split(":")
  
  sim = SimulavrAdapter()
  sim.dmanSingleDeviceApplication()
  dev = sim.loadDevice(proc, elffile)
  print("before simulation start:")
  print("  value 'adc_value'=%d (before init)" % sim.getWordByName(dev, "adc_value"))
  
  a0 = XPin(dev, "A0", 'a')
  aref = XPin(dev, "AREF", 'a')
  
  aref.SetAnalogValue(2.5)
  print("  aref set to 2.5V")

  a0.SetAnalogValue(0.3)
  e = int((0.3 / 2.5) * 1024) & 0x3ff
  print("  a0 set to 0.3V, this will expect an converted adc int value=%d" % e)
  
  sim.dmanStart()
  print("simulation start: (t=%dns)" % sim.getCurrentTime())
  
  print("run till main function ...")
  bpaddr = dev.Flash.GetAddressAtSymbol("main")
  dev.BP.AddBreakpoint(bpaddr)
  sim.doRun(30000)
  if not dev.PC == bpaddr:
      print(">>> error: main function not arrived!")
  dev.BP.RemoveBreakpoint(bpaddr)
  print("simulation main entrance: (t=%dns)" % sim.getCurrentTime())
  print("  value 'adc_value'=%d (after init)" % sim.getWordByName(dev, "adc_value"))
  
  sim.doRun(sim.getCurrentTime() + 120000)
  print("simulation break: (t=%dns)" % sim.getCurrentTime())
  print("  value 'conversions'=%d" % sim.getWordByName(dev, "conversions"))
  print("  value 'adc_value'=%d (simulation break)" % sim.getWordByName(dev, "adc_value"))
  
  sim.doRun(sim.getCurrentTime() + 330000)
  print("simulation end: (t=%dns)" % sim.getCurrentTime())
  print("  value 'conversions'=%d" % sim.getWordByName(dev, "conversions"))
  print("  value 'adc_value'=%d (simulation end)" % sim.getWordByName(dev, "adc_value"))
  
  sim.dmanStop()
  del dev
  
# EOF
