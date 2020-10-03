/*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2020   Klaus Rudolph       
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

#pragma once

#include <vector>

#include "hardware.h"
#include "funktor.h"
#include "printable.h"
#include "avrdevice.h"
#include "traceval.h"

//! global switch to enable irq statistic (default is disabled)
extern bool enableIRQStatistic;

#ifndef SWIG

class IrqStatisticEntry {
        
    public:
        SystemClockOffset flagSet;
        SystemClockOffset flagCleared;
        SystemClockOffset handlerStarted;
        SystemClockOffset handlerFinished;

        SystemClockOffset setClear;
        SystemClockOffset setStarted;   
        SystemClockOffset setFinished;  
        SystemClockOffset startedFinished;


        IrqStatisticEntry():
            flagSet(0),
            flagCleared(0),
            handlerStarted(0),
            handlerFinished(0),
            setClear(0),
            setStarted(0),
            setFinished(0),
            startedFinished(0) {}
        void CalcDiffs();
};

class IrqStatisticPerVector {
    
    protected:
        IrqStatisticEntry long_SetClear;
        IrqStatisticEntry short_SetClear;

        IrqStatisticEntry long_SetStarted;
        IrqStatisticEntry short_SetStarted;

        IrqStatisticEntry long_SetFinished;
        IrqStatisticEntry short_SetFinished;

        IrqStatisticEntry long_StartedFinished;
        IrqStatisticEntry short_StartedFinished;

        friend std::string Print( const IrqStatisticPerVector &ispv, AvrDevice* core );

    public:

        IrqStatisticEntry actual;
        IrqStatisticEntry last;

        void CalculateStatistic();
        void CheckComplete();

        IrqStatisticPerVector();
};

std::ostream& operator<<(std::ostream &, const IrqStatisticEntry&);
std::ostream& operator<<(std::ostream &, const IrqStatisticPerVector&);

class IrqStatistic: public Printable {
    
    private:
        AvrDevice *core; // used to get the (file) name and clk speed of the core device
        std::map<unsigned int, IrqStatisticPerVector> entries;
        void operator()();

        friend std::ostream& operator<<(std::ostream &, const IrqStatistic&);

    public:
        IrqStatistic(AvrDevice *);
        virtual ~IrqStatistic() {}
        void SetIrqFlag( unsigned int vector, SystemClockOffset );
        void ClearIrqFlag( unsigned int vector, SystemClockOffset );
        void IrqHandlerStarted( unsigned int vector, SystemClockOffset, unsigned int stackPointer );
        void IrqHandlerFinished( unsigned int vector, SystemClockOffset, unsigned int stackPointer );
};

std::ostream& operator<<(std::ostream &, const IrqStatistic&);

#endif // ifndef SWIG



