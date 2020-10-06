 /*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002, 2003   Klaus Rudolph		
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 ****************************************************************************
 *
 *  $Id$
 */

#include <iostream>
#include <sstream>
#include "helper.h"



HexChar::HexChar(unsigned char x) { val=x; }
HexShort::HexShort(unsigned short x) { val=x; }
DecLong::DecLong(unsigned long x) { val=x; }

std::ostream &operator << (std::ostream &os, const HexChar &h) {
    os << "0x";
    os.width(2);
    os.fill('0');
    os << std::hex << (unsigned int) h.val << std::dec ;
    return os;
}

std::ostream &operator << (std::ostream &os, const HexShort &h) {
    os << "0x" ;
    os.width(4);
    os.fill('0');
    os << std::hex << (unsigned int) h.val << std::dec ;
    return os;
}

std::ostream &operator << (std::ostream &os, const DecLong &h) {
    os.width(9);
    os.fill(' ');
    os << std::dec << (unsigned long) h.val << std::dec ;
    return os;
}

std::string int2str(int i) {
    std::stringstream s;
    s << i;
    return s.str();
}

std::string int2hex(int i) {
    std::stringstream s;
    s << std::hex << i;
    return s.str();
}

std::string readline(std::istream &is) {
    std::string out;
    char c=0;
    while (!is.eof() && c!='\n') {
	is.read(&c, 1);
	if (is.gcount())
	    out+=c;
    }
    return out;
}

std::vector<std::string> split(const std::string &inp, std::string splitc) {
    std::vector<std::string> out;
    std::string cur;
    for (size_t i=0; i < inp.size(); i++) {
	char c=inp[i];
	if (splitc.find(c)==splitc.npos)
	    cur+=c;
	else {
	    if (cur.size()) {
		out.push_back(cur);
		cur="";
	    }
	}
    }
    if (cur.size())
	out.push_back(cur);
    return out;
}
