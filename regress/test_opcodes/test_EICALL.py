#! /usr/bin/env python
###############################################################################
#
# simulavr - A simulator for the Atmel AVR family of microcontrollers.
# Copyright (C) 2001, 2002  Theodore A. Roth
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
###############################################################################
#
#
#

"""Test the EICALL opcode.
"""

import base_test
from registers import Reg

class EICALL_TestFail(base_test.TestFail): pass

class base_EICALL(base_test.opcode_stack_test):
	"""Generic test case for testing EICALL opcode.

	The derived class must provide the reg member and the fail method.

	EICALL - Extended ndirect call to subroutine

	  PC(15:0)  <- Z(15:0)
	  PC(21:16) <- EIND(7:0)
	  STACK     <- PC + 1
	  
	  SP <- SP - 3
	  (3 bytes, 22 bit PC)
	  
	opcode is '1001 0101 0001 1001'
	"""
	def setup(self):
		# setup PC
		self.setup_regs[Reg.PC] = 0xff * 2

		# setup Z register (Z = R31:R30)
		self.setup_regs[Reg.R30] = self.k & 0xff
		self.setup_regs[Reg.R31] = self.k >> 8 & 0xff

                #setup EIND register
                #self.setup_regs[Reg.EIND] = self.eind & 0xff
                
		return 0x9519

	def analyze_results(self):
		self.reg_changed.append( Reg.SP )
		self.is_pc_checked = 1

		expect = self.k

		got = self.anal_regs[Reg.PC] / 2
		
		if expect != got:
			self.fail('EICALL failed: expect=%x, got=%x' % (expect, got))

		expect = self.setup_regs[Reg.SP] - 3 # 22 bit PC
		got    = self.anal_regs[Reg.SP]
		
		if got != expect:
			self.fail('EICALL stack push failed: expect=%04x, got=%04x' % (
				expect, got ))

#
# Template code for test case.
# The fail method will raise a test specific exception.
#
template = """
class EICALL_%04x_TestFail(EICALL_TestFail): pass

class test_EICALL_%04x(base_EICALL):
	k = 0x%x
	eind = 0x00
	def fail(self,s):
		raise EICALL_%04x_TestFail, s
"""

#
# automagically generate the test_EICALL_* class definitions
#
code = ''
for k in (0x100,0x3ff):
	code += template % ((k & 0x3fffff), (k & 0x3fffff), k, (k & 0x3fffff))
exec code
