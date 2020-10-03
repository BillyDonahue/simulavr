/*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002, 2003 Klaus Rudolph
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

/*
This file should be removed or integrated somewhere in the near future
Actually it is only put to make the compile possible because of 
infinite recursion in header dependencies which I have not fixed until now
*/

#include <cstdint>
#include <iomanip>
#include <limits>

#ifndef SYSTEMCLOCKTYPES
#define SYSTEMCLOCKTYPES
using SystemClockOffset = uint64_t;
static constexpr SystemClockOffset INVALID = std::numeric_limits<SystemClockOffset>::max();



inline constexpr SystemClockOffset operator ""_ns(unsigned long long int val) { return val; }
inline constexpr SystemClockOffset operator ""_us(unsigned long long int val) { return val * 1E3; }
inline constexpr SystemClockOffset operator ""_ms(unsigned long long int val) { return val * 1E6; }
inline constexpr SystemClockOffset operator ""_s (unsigned long long int val) { return val * 1E9; }

inline constexpr SystemClockOffset operator ""_ns(long double val) { return val; }
inline constexpr SystemClockOffset operator ""_us(long double val) { return val * 1E3; }
inline constexpr SystemClockOffset operator ""_ms(long double val) { return val * 1E6; }
inline constexpr SystemClockOffset operator ""_s (long double val) { return val * 1E9; }

inline std::string FormattedTime( SystemClockOffset val )
{
    // split up number to blocks:
    unsigned int seconds;
    unsigned int milliseconds;
    unsigned int microseconds;
    unsigned int nanoseconds;

    nanoseconds = val % 1000;
    val/=1000;
    microseconds = val % 1000;
    val/=1000;
    milliseconds = val % 1000;
    val/=1000;
    seconds = val;

    std::ostringstream os;
    // setw for seconds to 4 means 9999 seconds simulated time as first column. If we see more, it is shifted, no problem :-)
    os << std::setw(4) << std::setfill(' ') << seconds << ":";
    os << std::setw(3) << std::setfill('0') << milliseconds << ".";
    os << std::setw(3) << std::setfill('0') << microseconds << ".";
    os << std::setw(3) << std::setfill('0') << nanoseconds;

    return os.str();
}


#endif
