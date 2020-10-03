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
#include <memory>

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

        uint32_t stackPointer;


        IrqStatisticEntry():
            flagSet(INVALID),
            flagCleared(INVALID),
            handlerStarted(INVALID),
            handlerFinished(INVALID),
            setClear(INVALID),
            setStarted(INVALID),
            setFinished(INVALID),
            startedFinished(INVALID),
            stackPointer(0)
    {}
        void CalcDiffs();
        bool CheckComplete();
};

class IrqStatisticPerVector {
    std::shared_ptr<IrqStatisticEntry> dummyInitObject;
    
    protected:
        std::shared_ptr<IrqStatisticEntry> long_SetClear;
        std::shared_ptr<IrqStatisticEntry> short_SetClear;

        std::shared_ptr<IrqStatisticEntry> long_SetStarted;
        std::shared_ptr<IrqStatisticEntry> short_SetStarted;

        std::shared_ptr<IrqStatisticEntry> long_SetFinished;
        std::shared_ptr<IrqStatisticEntry> short_SetFinished;

        std::shared_ptr<IrqStatisticEntry> long_StartedFinished;
        std::shared_ptr<IrqStatisticEntry> short_StartedFinished;

        std::shared_ptr<IrqStatisticEntry> last;

        friend std::string Print( const IrqStatisticPerVector &ispv, AvrDevice* core );

    public:
        std::vector< std::shared_ptr<IrqStatisticEntry> > unfinishedEntries;
        std::vector< std::shared_ptr<IrqStatisticEntry> > usedInSummarize;


        void CalculateStatistic();

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



