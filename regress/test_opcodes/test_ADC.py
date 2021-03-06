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
# $Id: test_ADC.py,v 1.1 2004/07/31 00:59:11 rivetwa Exp $
#

"""Test the ADC opcode.
"""

import base_test
from registers import Reg, SREG

class ADC_TestFail(base_test.TestFail): pass

class base_ADC(base_test.opcode_test):
	"""Generic test case for testing ADC opcode.

	Add with Carry. [Rd <- Rd + Rr + C]
	opcode is '0001 11rd dddd rrrr' where r and d are registers (d is destination).

	Only registers PC, Rd and SREG should be changed.
	"""
	def setup(self):
		# Set SREG to zero or only C flag set
		self.setup_regs[Reg.SREG] = self.C << SREG.C

		# Set the register values
		self.setup_regs[self.Rd] = self.Vd
		self.setup_regs[self.Rr] = self.Vr

		# Return the raw opcode
		return 0x1C00 | (self.Rd << 4) | ((self.Rr & 0x10) << 5) | (self.Rr & 0xf)

	def analyze_results(self):
		self.reg_changed.extend( [self.Rd, Reg.SREG] )
		
		# check that result is correct
		res = (self.Vd + self.Vr + self.C)
		expect = res & 0xff

		got = self.anal_regs[self.Rd]
		
		if expect != got:
			self.fail('ADC r%02d, r%02d: 0x%02x + 0x%02x + %d = (expect=%02x, got=%02x)' % (
				self.Rd, self.Rr, self.Vd, self.Vr, self.C, expect, got))

		expect_sreg = 0

		# calculate what we expect sreg to be (I and T should be zero)
		H = (((self.Vd & self.Vr) | (self.Vr & ~res) | (~res & self.Vd)) >> 3) & 1
		V = (((self.Vd & self.Vr & ~res) | (~self.Vd & ~self.Vr & res)) >> 7) & 1
		N = ((expect & 0x80) != 0)
		expect_sreg += H             << SREG.H
		expect_sreg += V             << SREG.V
		expect_sreg += N             << SREG.N
		expect_sreg += (N ^ V)       << SREG.S
		expect_sreg += (expect == 0) << SREG.Z
		expect_sreg += (res > 0xff)  << SREG.C

		got_sreg = self.anal_regs[Reg.SREG]

		if expect_sreg != got_sreg:
			self.fail('ADC r%02d, r%02d: 0x%02x + 0x%02x + %d -> SREG (expect=%02x, got=%02x)' % (
				self.Rd, self.Rr, self.Vd, self.Vr, self.C, expect_sreg, got_sreg))

#
# Template code for test case.
# The fail method will raise a test specific exception.
#
template = """
class ADC_rd%02d_vd%02x_rr%02d_vr%02x_C%d_TestFail(ADC_TestFail): pass

class test_ADC_rd%02d_vd%02x_rr%02d_vr%02x_C%d(base_ADC):
	Rd = %d
	Vd = 0x%x
	Rr = %d
	Vr = 0x%x
	C  = %d
	def fail(self,s):
		raise ADC_rd%02d_vd%02x_rr%02d_vr%02x_C%d_TestFail(s)
"""

#
# Define a list of test values such that we all the cases of SREG bits being set.
#
vals = (
( 0x00, 0x00 ),
( 0xff, 0x00 ),
( 0xfe, 0x01 ),
( 0x0f, 0x00 ),
( 0x0f, 0xf0 ),
( 0x01, 0x02 ),
( 0x7f, 0x01 )
)

#
# automagically generate the test_ADC_rdNN_vdXX_rrNN_vrXX_C[01] class definitions.
# For these, we don't want Rd=Rr as that is a special case handled below.
#
code = ''
for c in (0,1):
	for d in range(0,32,8):
		for r in range(1,32,8):
			for vd,vr in vals:
				args = (d,vd,r,vr,c)*4
				code += template % args

#
# This is effectively a ROL operation (Rd == Rr)
#
for c in (0,1):
	for d in range(0,32,8):
		for vd,vr in vals:
			args = (d,vd,d,vd,c)*4
			code += template % args
exec(code)
