from __future__ import print_function
from unittest import TextTestRunner, TestSuite
from sys import argv, stderr, exit
import os.path

from vcdtestutil import VCDTestLoader
from simtestutil import SimTestLoader, PyTestLoader

def parseTargetName(name):
  name = os.path.basename(name)
  n = name.split(".")
  l = n[0].split("_")
  if len(l) == 1 and n[1].lower() == "py":
    return l[0] # just a python file
  return "_".join(l[:-1])
  
def parseTargetType(name):
  name = os.path.basename(name)
  return name.split(".")[-1].lower()
  
targetLoader = {
  "vcd": VCDTestLoader,
  "elf": SimTestLoader,
  "py":  PyTestLoader,
}

def getTests(targets):
  l = list()
  for name in targets:
    try:
      m = __import__(parseTargetName(name))
      l.append(targetLoader[parseTargetType(name)](name).loadTestsFromModule(m))
    except Exception as e:
      print("error: %s" % str(e), file=stderr)
  return TestSuite(l)
  
if __name__ == '__main__':

  res = TextTestRunner(verbosity=2).run(getTests(argv[1:]))
  if res.wasSuccessful():
    exit(0)
  else:
    exit(1)
    
# EOF
