# Python Script
import pysimulavr

class XPin(pysimulavr.Pin):
  
  def __init__(self, name):
    pysimulavr.Pin.__init__(self)
    self.name = name
    
  def SetInState(self, pin):
    pysimulavr.Pin.SetInState(self, pin)
    msg = "<pin%s change: in=%s/%0.2fV, out=%s/%0.2fV> "
    print(msg % (self.name,
                 pin.toChar(),
                 pin.GetAnalogValue(vcc),
                 self.toChar(),
                 self.GetAnalogValue(vcc)), end="")
    
def printPin(pid, pin):
  print("  pin%d: (char)pin='%s', (bool)pin=%d, pin.GetAnalogValue(vcc)=%0.2fV" % (pid,
                                                                                   pin.toChar(),
                                                                                   ord(pin.toBool()),
                                                                                   pin.GetAnalogValue(vcc)))

if __name__ == "__main__":

  vcc = 5.0
  print("set vcc=%0.2fV ...\n" % vcc)
  
  print("create 2 pins ...")
  p1 = XPin("1")
  p1.outState = pysimulavr.Pin.LOW
  printPin(1, p1)
  
  p2 = XPin("2")
  p2.outState = pysimulavr.Pin.TRISTATE
  printPin(2, p2)
  
  print("\ncreate net ...")
  n = pysimulavr.Net()
  print("  add pin1 to net:")
  n.Add(p1)
  print("")
  print("  add pin2 to net:")
  n.Add(p2)
  print("")
  
  printPin(1, p1)
  printPin(2, p2)

  print("\nset pin2 output to PULLUP:")
  p2.SetPin("h")
  print("")
  printPin(1, p1)
  printPin(2, p2)

  print("\nset pin1 output to HIGH:")
  p1.SetPin("H")
  print("")
  printPin(1, p1)
  printPin(2, p2)

  print("\nset pin2 output to TRISTATE:")
  p2.SetPin("t")
  print("")
  printPin(1, p1)
  printPin(2, p2)

  print("\nset pin1 output to TRISTATE:")
  p1.SetPin("t")
  print("")
  printPin(1, p1)
  printPin(2, p2)

  print("\nset pin2 output to LOW:")
  p2.SetPin("L")
  print("")
  printPin(1, p1)
  printPin(2, p2)

  del n
 
# EOF
