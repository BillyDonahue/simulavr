#from types import IntType, FloatType

from simtestutil import PyTestCase, PyTestLoader
import pysimulavr

#class XPin(pysimulavr.Pin):
  
#  def SetInState(self, pin):
#    pysimulavr.Pin.SetInState(self, pin)
#    self.__inState = pin.toChar()
    
#  @property
#  def inState(self): return self.__inState

class TestCase_1(PyTestCase):
  
  """
  This testcase test a regular port without any outer connections, port has feature pin toggle
  """

  def setUp(self):
    # create a avr core, type isn't important
    self.core = pysimulavr.AvrFactory.instance().makeDevice("atmega16")
    # create port with 4 bit and pin toggle feature
    self.port = pysimulavr.HWPort(self.core, "port", True, 4)

  def tearDown(self):
    del self.port
    del self.core

  def test_00(self):
    """check port initial state after reset"""
    self.assertTrue(self.port.GetPortString() == "tttt", "all output pins in tristate")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xf, "read pin register == 0xf")
    self.assertTrue(self.port.GetDdr() == 0, "read ddr register == 0")
    
  def test_01(self):
    """check, if set ddr work"""
    self.assertTrue(self.port.GetPortString() == "tttt", "all output pins in tristate")
    self.port.SetDdr(0xa) # bit 3 and 1 are output
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0x5, "read pin register == 0x5")
    self.assertTrue(self.port.GetDdr() == 0xa, "read ddr register == 0xa")
    self.assertTrue(self.port.GetPortString() == "LtLt", "output pins 3 and 1 to low")
    self.port.SetPort(0xa) # set bits 3 and 1 to high
    self.assertTrue(self.port.GetPort() == 0xa, "read port register == 0xa")
    self.assertTrue(self.port.GetPin() == 0xf, "read pin register == 0xf")
    self.assertTrue(self.port.GetDdr() == 0xa, "read ddr register == 0xa")
    self.assertTrue(self.port.GetPortString() == "HtHt", "output pins 3 and 1 to high")
    self.port.SetDdr(0) # all bits are inputs
    self.assertTrue(self.port.GetPort() == 0xa, "read port register == 0xa")
    self.assertTrue(self.port.GetPin() == 0xf, "read pin register == 0xf")
    self.assertTrue(self.port.GetDdr() == 0, "read ddr register == 0")
    self.assertTrue(self.port.GetPortString() == "htht", "output pins 3 and 1 have pullup")
    self.port.SetPort(0) # port = 0
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xf, "read pin register == 0xf")
    self.assertTrue(self.port.GetPortString() == "tttt", "all output pins in tristate again")

  def test_02(self):
    """check, if pin toggle works"""
    self.assertTrue(self.port.GetPortString() == "tttt", "all output pins in tristate")
    self.port.SetDdr(0x1) # bit 0 is output
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xe, "read pin register == 0xe")
    self.assertTrue(self.port.GetDdr() == 0x1, "read ddr register == 0x1")
    self.assertTrue(self.port.GetPortString() == "tttL", "bit 0 is low")
    self.port.SetPin(0x1)
    self.assertTrue(self.port.GetPort() == 0x1, "read port register == 0x1")
    self.assertTrue(self.port.GetPin() == 0xf, "read pin register == 0xf")
    self.assertTrue(self.port.GetPortString() == "tttH", "bit 0 is high")
    self.port.SetPin(0x1)
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xe, "read pin register == 0xe")
    self.assertTrue(self.port.GetPortString() == "tttL", "bit 0 is low again")
    self.port.SetPin(0x2) # only port will change!
    self.assertTrue(self.port.GetPort() == 0x2, "read port register == 0x2")
    self.assertTrue(self.port.GetPin() == 0xe, "read pin register == 0xe")
    self.assertTrue(self.port.GetPortString() == "tthL", "bit 0 is low, bit 1 pullup")
    self.port.SetPin(0x2) # only port will change!
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xe, "read pin register == 0xe")
    self.assertTrue(self.port.GetPortString() == "tttL", "bit 0 is low")

class TestCase_2(PyTestCase):
  
  """
  This testcase test a regular port without any outer connections, port has no pin toggle
  """

  def setUp(self):
    # create a avr core, type isn't important
    self.core = pysimulavr.AvrFactory.instance().makeDevice("atmega16")
    # create port with 4 bit and pin toggle feature
    self.port = pysimulavr.HWPort(self.core, "port", False, 4)

  def tearDown(self):
    del self.port
    del self.core

  def test_00(self):
    """check port initial state after reset"""
    self.assertTrue(self.port.GetPortString() == "tttt", "all output pins in tristate")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xf, "read pin register == 0xf")
    self.assertTrue(self.port.GetDdr() == 0, "read ddr register == 0")
    
  def test_01(self):
    """check, if pin toggle doesn't work"""
    self.assertTrue(self.port.GetPortString() == "tttt", "all output pins in tristate")
    self.port.SetDdr(0x1) # bit 0 is output
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xe, "read pin register == 0xe")
    self.assertTrue(self.port.GetDdr() == 0x1, "read ddr register == 0x1")
    self.assertTrue(self.port.GetPortString() == "tttL", "bit 0 is low")
    self.port.SetPin(0x1)
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xe, "read pin register == 0xe")
    self.assertTrue(self.port.GetPortString() == "tttL", "bit 0 is low")

class TestCase_3(PyTestCase):
  
  """
  This testcase test PinAtPort and alternate access to port pins
  """

  def setUp(self):
    # create a avr core, type isn't important
    self.core = pysimulavr.AvrFactory.instance().makeDevice("atmega16")
    # create port with 4 bit and pin toggle feature
    self.port = pysimulavr.HWPort(self.core, "port", False, 4)
    # create 2 special pins for port
    self.pinA = pysimulavr.PinAtPort(self.port, 0)
    self.pinB = pysimulavr.PinAtPort(self.port, 3)

  def tearDown(self):
    del self.pinB
    del self.pinA
    del self.port
    del self.core

  def test_00(self):
    """check pin initial state after reset"""
    self.assertTrue(self.port.GetPortString() == "tttt", "all output pins in tristate")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xf, "read pin register == 0xf")
    self.assertTrue(self.port.GetDdr() == 0, "read ddr register == 0")
    # get methods for pin A
    self.assertFalse(self.pinA.GetPort(), "port pin A is low")
    self.assertFalse(self.pinA.GetDdr(), "ddr pin A is input")
    # get methods for pin B
    self.assertFalse(self.pinB.GetPort(), "port pin B is low")
    self.assertFalse(self.pinB.GetDdr(), "ddr pin B is input")

  def test_01(self):
    """check override ddr functionality"""
    self.port.SetDdr(0xf) # all pins output
    self.assertTrue(self.port.GetPortString() == "LLLL", "all output pins low")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0, "read pin register == 0")
    self.assertTrue(self.port.GetDdr() == 0xf, "read ddr register == 0xf")
    self.pinA.SetAlternateDdr(False) # preset override ddr to input
    self.assertTrue(self.port.GetPin() == 0, "read pin register == 0")
    self.assertTrue(self.port.GetPortString() == "LLLL", "all output pins low")
    self.pinA.SetUseAlternateDdr(True) # override ddr
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0x1, "read pin register == 0x1")
    self.assertTrue(self.port.GetDdr() == 0xf, "read ddr register == 0xf")
    self.assertTrue(self.port.GetPortString() == "LLLt", "pin 0 tristate")
    self.port.SetDdr(0xe) # no reaction expected
    self.assertTrue(self.port.GetPin() == 0x1, "read pin register == 0x1")
    self.assertTrue(self.port.GetDdr() == 0xe, "read ddr register == 0xe")
    self.assertTrue(self.port.GetPortString() == "LLLt", "pin 0 tristate")
    self.pinA.SetAlternateDdr(True) # preset override ddr to output
    self.assertTrue(self.port.GetPin() == 0, "read pin register == 0")
    self.assertTrue(self.port.GetDdr() == 0xe, "read ddr register == 0xe")
    self.assertTrue(self.port.GetPortString() == "LLLL", "all output pins low")
    self.pinA.SetUseAlternateDdr(False) # remove override ddr
    self.assertTrue(self.port.GetPin() == 0x1, "read pin register == 0x1")
    self.assertTrue(self.port.GetDdr() == 0xe, "read ddr register == 0xe")
    self.assertTrue(self.port.GetPortString() == "LLLt", "pin 0 tristate")

  def test_02(self):
    """check override ddr functionality with 2 pins"""
    self.port.SetDdr(0x3) # pin 0,1 as output
    self.assertTrue(self.port.GetPortString() == "ttLL", "2 pins tristate, 2 pins low")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xc, "read pin register == 0xc")
    self.assertTrue(self.port.GetDdr() == 0x3, "read ddr register == 0x3")
    self.pinA.SetAlternateDdr(False) # preset override ddr to input
    self.pinB.SetAlternateDdr(True) # preset override ddr to output
    self.assertTrue(self.port.GetPortString() == "ttLL", "2 pins tristate, 2 pins low")
    self.assertTrue(self.port.GetPin() == 0xc, "read pin register == 0xc")
    self.pinA.SetUseAlternateDdr(True) # override ddr
    self.assertTrue(self.port.GetPortString() == "ttLt", "3 pins tristate, 1 pin low")
    self.assertTrue(self.port.GetPin() == 0xd, "read pin register == 0xd")
    self.pinB.SetUseAlternateDdr(True) # override ddr
    self.assertTrue(self.port.GetPortString() == "LtLt", "2 pins tristate, 2 pins low")
    self.assertTrue(self.port.GetPin() == 0x5, "read pin register == 0x5")

  def test_03(self):
    """check override port functionality with 2 pins"""
    self.port.SetDdr(0x3) # pin 0,1 as output
    self.assertTrue(self.port.GetPortString() == "ttLL", "2 pins tristate, 2 pins low")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xc, "read pin register == 0xc")
    self.assertTrue(self.port.GetDdr() == 0x3, "read ddr register == 0x3")
    self.pinA.SetAlternatePort(True) # preset override port to high
    self.assertTrue(self.port.GetPortString() == "ttLL", "2 pins tristate, 2 pins low")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xc, "read pin register == 0xc")
    self.pinA.SetUseAlternatePort(True) # override port
    self.assertTrue(self.port.GetPortString() == "ttLH", "2 pins tristate, 1 pin low, 1 pin high")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xd, "read pin register == 0xd")
    self.pinB.SetAlternatePort(True) # preset override port to high
    self.pinB.SetUseAlternatePort(True) # override port
    self.assertTrue(self.port.GetPortString() == "ttLH", "2 pins tristate, 1 pin low, 1 pin high")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xd, "read pin register == 0xd")
    self.pinA.SetUseAlternatePort(False) # remove override port
    self.assertTrue(self.port.GetPortString() == "ttLL", "2 pins tristate, 2 pins low")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xc, "read pin register == 0xc")

  def test_04(self):
    """check override port if ddr set functionality with 2 pins"""
    self.port.SetDdr(0x3) # pin 0,1 as output
    self.assertTrue(self.port.GetPortString() == "ttLL", "2 pins tristate, 2 pins low")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xc, "read pin register == 0xc")
    self.assertTrue(self.port.GetDdr() == 0x3, "read ddr register == 0x3")
    self.pinA.SetAlternatePort(True) # preset override port to high
    self.pinA.SetUseAlternatePortIfDdrSet(True) # override port, if ddr is set
    self.assertTrue(self.port.GetPortString() == "ttLH", "2 pins tristate, 1 pin low, 1 pin high")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xd, "read pin register == 0xd")
    self.pinB.SetAlternatePort(True) # preset override port to high
    self.pinB.SetUseAlternatePortIfDdrSet(True) # override port, if ddr is set
    self.assertTrue(self.port.GetPortString() == "ttLH", "2 pins tristate, 1 pin low, 1 pin high")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xd, "read pin register == 0xd")
    self.port.SetDdr(0x2) # pin 1 as output
    self.assertTrue(self.port.GetPortString() == "ttLt", "3 pins tristate, 1 pin low")
    self.assertTrue(self.port.GetPin() == 0xd, "read pin register == 0xd")

class TestCase_4(PyTestCase):
  
  """
  This testcase test PinAtPort and alternate access to one port pin (multiple alternate use)
  """

  def setUp(self):
    # create a avr core, type isn't important
    self.core = pysimulavr.AvrFactory.instance().makeDevice("atmega16")
    # create port with 4 bit and pin toggle feature
    self.port = pysimulavr.HWPort(self.core, "port", False, 4)
    # create 2 special pins for port
    self.pinA = pysimulavr.PinAtPort(self.port, 0)
    self.pinB = pysimulavr.PinAtPort(self.port, 0)

  def tearDown(self):
    del self.pinB
    del self.pinA
    del self.port
    del self.core

  def test_00(self):
    """check pin initial state after reset"""
    self.assertTrue(self.port.GetPortString() == "tttt", "all output pins in tristate")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xf, "read pin register == 0xf")
    self.assertTrue(self.port.GetDdr() == 0, "read ddr register == 0")

  def test_01(self):
    """check override port if ddr set from first PinAtPort"""
    self.port.SetDdr(0x3) # pin 0,1 as output
    self.assertTrue(self.port.GetPortString() == "ttLL", "2 pins tristate, 2 pins low")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xc, "read pin register == 0xc")
    self.assertTrue(self.port.GetDdr() == 0x3, "read ddr register == 0x3")
    self.pinA.SetAlternatePort(True) # preset override port to high for first PinAtPort
    self.pinA.SetUseAlternatePortIfDdrSet(True) # override first PinAtPort, if ddr is set
    self.assertTrue(self.port.GetPortString() == "ttLH", "2 pins tristate, 1 pin low, 1 pin high")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xd, "read pin register == 0xd")
    self.pinA.SetUseAlternatePortIfDdrSet(False) # remove override port
    self.assertTrue(self.port.GetPortString() == "ttLL", "2 pins tristate, 2 pins low")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xc, "read pin register == 0xc")
    self.assertTrue(self.port.GetDdr() == 0x3, "read ddr register == 0x3")
    self.pinB.SetAlternatePort(False) # preset override port to low for second PinAtPort
    self.pinB.SetUseAlternatePortIfDdrSet(True) # override second PinAtPort, if ddr is set
    print ">>>", self.port.GetPortString(), "<<<"
    self.assertTrue(self.port.GetPortString() == "ttLL", "2 pins tristate, 2 pins low")
    self.assertTrue(self.port.GetPort() == 0, "read port register == 0")
    self.assertTrue(self.port.GetPin() == 0xc, "read pin register == 0xc")
    self.assertTrue(self.port.GetDdr() == 0x3, "read ddr register == 0x3")

if __name__ == '__main__':
  
  from unittest import TextTestRunner
  tests = PyTestLoader("portio").loadTestsFromTestCase(TestCase_1)
  TextTestRunner(verbosity = 2).run(tests)

# EOF
