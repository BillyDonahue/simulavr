#! /usr/bin/env python
###############################################################################
#
# simulavr - A simulator for the Atmel AVR family of microcontrollers.
# Copyright (C) 2001, 2002  Theodore A. Roth
# Copyright (C) 2015        Christian Taedcke
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

"""Test the EIJMP opcode.
"""

import base_test
from registers import Reg

class EIJMP_TestFail(base_test.TestFail): pass

class base_EIJMP(base_test.opcode_eind_test):
	"""Generic test case for testing EIJMP opcode.

	The derived class must provide the reg member and the fail method.

	EIJMP - Relative Jump

          PC(15:0) <- Z(15:0)
          PC(21:16) <- EIND(7:0)

	opcode is '1001 0100 0001 1001'
	"""

	def ensure_target_supports_opcode(self):
		if (not self.target.has_eind):
			self.opcode_not_supported()

	def setup(self):
		self.setup_regs[Reg.PC] = 0xff * 2

		# Load the Z register (reg 31:30) with PC to jump to
		self.setup_regs[Reg.R31] = (self.k >> 8) & 0xff
		self.setup_regs[Reg.R30] = (self.k & 0xff)

		#setup EIND register
		self.write_register_eind(self.eind & 0xff)

		return 0x9419

	def analyze_results(self):
		self.is_pc_checked = 1
		
		expect = self.k + (self.eind << 16)

		got = self.anal_regs[Reg.PC] / 2
		
		if expect != got:
			self.fail('EIJMP failed: expect=%x, got=%x' % (expect, got))

#
# Template code for test case.
# The fail method will raise a test specific exception.
#
template = """
class EIJMP_k%06x_ei%02x_TestFail(EIJMP_TestFail): pass

class test_EIJMP_k%06x_ei%02x(base_EIJMP):
	k = 0x%x
	eind = 0x%x
	def fail(self,s):
		raise EIJMP_k%06x_ei%02x_TestFail(s)
"""

#
# automagically generate the test_EIJMP_* class definitions
#
code = ''
for k in (0x36, 0x100, 0x3ff):
        for eind in (0x00, 0x01):
                args = (k, eind) * 4
                code += template % args
exec(code)
