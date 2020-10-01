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

#include <string.h> //strcpy()
#include <sstream>
#include <iostream>

#include "memory.h"
#include "avrerror.h"

unsigned int Memory::GetAddressAtSymbol(const std::string &s) {

    // feature: use a number instead of a symbol
    char *dummy;
    char *copy = avr_new(char, s.length() + 1);
    unsigned int retval = 0;
    unsigned int convlen = 0;

    strcpy(copy, s.c_str());
    retval = strtoul(copy, &dummy, 16);
    convlen = (unsigned int)(dummy - copy);
    avr_free(copy);

    if((retval != 0) && ((unsigned int)s.length() == convlen)) {
        // number found, return this
        return retval;
    }

    // isn't a number, try to find symbol ...
    std::multimap<unsigned int, std::string>::iterator ii;

    for(ii = sym.begin(); ii != sym.end(); ii++) {
        if(ii->second == s) {
            return ii->first;
        }
    }

    avr_error("symbol '%s' not found!", s.c_str());

    return 0; // to avoid warnings, avr_error aborts the program
}


std::string Memory::GetSymbolAtAddress( unsigned int add )
{
    std::ostringstream os;

    if ( sym.size() )
    {
        // get value behind our address
        auto it = sym.upper_bound( add );

        // take the symbol where the address is equal or lower than the searched one
        it--;   

        // get all symbols from that address
        auto p = sym.equal_range( it->first ); 

        // get the offset the the address we search for
        unsigned int offset = add- p.first->first;

        // if there is a offset...
        if ( offset ) { os << "("; }

        // try to suppress symbols beginning with '_', but only if others are present
        unsigned int found = 100;
        auto tmp = p.first;
        for ( ; tmp != p.second; tmp++ )
        {
            unsigned int comp = tmp->second.find_first_not_of("_");
            if ( found > comp ) { found = comp; }
        }

        // run over all symbols for the address
        bool isFirst = true;
        for ( ; p.first != p.second; p.first++ )
        {
            if ( found >= p.first->second.find_first_not_of("_") )
            {
                if ( isFirst )
                {
                    isFirst = false;
                }
                else
                { 
                    os << ",";
                }
                os << p.first->second;
            }

        }

        // print the offset if there is one
        if( offset ) 
        {
            os << ")+0x"<< std::hex<< offset;
        }

    }

    return os.str();
}

Memory::Memory(int _size): size(_size) {
    myMemory = avr_new(unsigned char, size);
}

