# Python Script
from __future__ import print_function
import pysimulavr

class XPin(pysimulavr.Pin):
  
  def SetInState(self, pin):
    pysimulavr.Pin.SetInState(self, pin)
    print("<pin: id=%s, in=%s/%0.2fV, out=%s, aPin=%0.2fV>" % (id(self),
                                                               pin.toChar(),
                                                               pin.GetAnalogValue(vcc),
                                                               self.toChar(),
                                                               self.GetAnalogValue(vcc)), end=' ')
    
def printPin(pid, pin, withID = False):
  if withID:
    print("  pin %d: id=%s, (char)pin='%s', pin.GetAnalogValue(vcc)=%0.2fV" % (pid,
                                                                               id(pin),
                                                                               pin.toChar(),
                                                                               pin.GetAnalogValue(vcc)))
  else:
    print("  pin %d: (char)pin='%s', pin.GetAnalogValue(vcc)=%0.2fV" % (pid,
                                                                        pin.toChar(),
                                                                        pin.GetAnalogValue(vcc)))

if __name__ == "__main__":

  vcc = 5.0
  print("set vcc=%0.2fV ..." % vcc)
  print("create 2 pins ...")
  p1 = XPin()
  p1.outState = pysimulavr.Pin.LOW
  printPin(1, p1, True)
  
  p2 = XPin()
  p2.outState = pysimulavr.Pin.TRISTATE
  printPin(2, p2, True)
  
  print("\ncreate net ...")
  n = pysimulavr.Net()
  print("  add pin 1:", end=' ')
  n.Add(p1)
  print("")
  print("  add pin 2:", end=' ')
  n.Add(p2)
  print("")
  
  printPin(1, p1)
  printPin(2, p2)

  print("\nset pin 2 output to PULLUP:", end=' ')
  p2.SetPin("h")
  print("")
  printPin(1, p1)
  printPin(2, p2)

  print("\nset pin 1 output to HIGH:", end=' ')
  p1.SetPin("H")
  print("")
  printPin(1, p1)
  printPin(2, p2)

  print("\nset pin 2 output to TRISTATE:", end=' ')
  p2.SetPin("t")
  print("")
  printPin(1, p1)
  printPin(2, p2)

  print("\nset pin 1 output to TRISTATE:", end=' ')
  p1.SetPin("t")
  print("")
  printPin(1, p1)
  printPin(2, p2)

  print("\nset pin 2 output to LOW:", end=' ')
  p2.SetPin("L")
  print("")
  printPin(1, p1)
  printPin(2, p2)
  
# EOF
