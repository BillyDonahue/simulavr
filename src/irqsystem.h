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

#ifndef HWIRQSYSTEM
#define HWIRQSYSTEM

#include <vector>

#include "hardware.h"
#include "funktor.h"
#include "avrdevice.h"
#include "traceval.h"
#include "irqstatistic.h"

class HWIrqSystem: public TraceValueRegister {
    
    protected:
        int bytesPerVector;
        unsigned int vectorTableSize; ///< number of entries supported by the device, not bytes
        HWSreg *status;
        std::vector<TraceValue*> irqTrace;
        
        /// priority queue of pending interrupts (i.e. waiting to be processed)
        std::map<unsigned int, Hardware *> irqPartnerList;
        AvrDevice *core;
        IrqStatistic irqStatistic;
        std::vector<const Hardware*> debugInterruptTable;

    public:
        HWIrqSystem (AvrDevice* _core, int bytes_per_vector, int number_of_vectors);

        bool IsIrqPending();
        /// returns a new PC pointer if interrupt occurred, -1 otherwise.
        unsigned int GetNewPc(unsigned int &vector_index);
        void SetIrqFlag(Hardware *, unsigned int vector_index);
        void ClearIrqFlag(unsigned int vector_index);
        void IrqHandlerStarted(uint32_t stackPointer, unsigned int vector_index);
        void IrqHandlerFinished(uint32_t stackPointer, unsigned int vector_index);
        /// In datasheets RESET vector is index 1 but we use 0! And not a byte address.
        void DebugVerifyInterruptVector(unsigned int vector_index, const Hardware* source);
        void DebugDumpTable();
};


#endif

