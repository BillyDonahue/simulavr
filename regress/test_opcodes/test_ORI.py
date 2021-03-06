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
# MERCHANTABILITY or FITNESS FORI A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
###############################################################################
#
# $Id: test_ORI.py,v 1.1 2004/07/31 00:59:11 rivetwa Exp $
#

"""Test the ORI opcode.
"""

import base_test
from registers import Reg, SREG

class ORI_TestFail(base_test.TestFail): pass

class base_ORI(base_test.opcode_test):
	"""Generic test case for testing ORI opcode.

	ORI - Logical OR with Immediate
	opcode is '0110 kkkk dddd kkkk' where 16 <= d <= 31, 0 <= k <= 255

	Only registers PC, Rd and SREG should be changed.
	"""
	def setup(self):
		# Set SREG to have only V set (opcode should clear it)
		self.setup_regs[Reg.SREG] = 1 << SREG.V

		# Set the register values
		self.setup_regs[self.Rd] = self.Vd

		# Return the raw opcode
		return 0x6000 | ((self.Rd - 16) << 4) | ((self.Vk & 0xf0) << 4) | (self.Vk & 0xf)

	def analyze_results(self):
		self.reg_changed.extend( [self.Rd, Reg.SREG] )
		
		# check that result is correct
		expect = ((self.Vd | self.Vk) & 0xff)

		got = self.anal_regs[self.Rd]
		
		if expect != got:
			self.fail('ORI r%02d, 0x%02x: 0x%02x | 0x%02x = (expect=%02x, got=%02x)' % (
				self.Rd, self.Vk, self.Vd, self.Vk, expect, got))

		expect_sreg = 0

		# calculate what we expect sreg to be (I, T, H, V and C should be zero)
		V = 0
		N = ((expect & 0x80) != 0)
		expect_sreg += N             << SREG.N
		expect_sreg += (N ^ V)       << SREG.S
		expect_sreg += (expect == 0) << SREG.Z

		got_sreg = self.anal_regs[Reg.SREG]

		if expect_sreg != got_sreg:
			self.fail('ORI r%02d, 0x%02x: 0x%02x | 0x%02x -> SREG (expect=%02x, got=%02x)' % (
				self.Rd, self.Vk, self.Vd, self.Vk, expect_sreg, got_sreg))

#
# Template code for test case.
# The fail method will raise a test specific exception.
#
template = """
class ORI_r%02d_v%02x_k%02x_TestFail(ORI_TestFail): pass

class test_ORI_r%02d_v%02x_k%02x(base_ORI):
	Rd = %d
	Vd = 0x%x
	Vk = 0x%x
	def fail(self,s):
		raise ORI_r%02d_v%02x_k%02x_TestFail(s)
"""

#
# Define a list of test values such that we test all the cases of SREG bits being set.
#
vals = (
( 0x00, 0x00 ),
( 0xff, 0x00 ),
( 0xfe, 0x01 ),
( 0x0f, 0x00 ),
( 0x0f, 0xf0 ),
( 0x23, 0xff )
)

#
# automagically generate the test_ORI_rNN_vXX_kXX class definitions.
# When K=0xff, this is effectively a SER operation.
#
code = ''
for d in range(16,32):
	for vd,vk in vals:
		args = (d,vd,vk)*4
		code += template % args

exec(code)
