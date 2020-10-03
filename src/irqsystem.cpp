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

#include "irqsystem.h"
#include "avrdevice.h"
#include "funktor.h"
#include "systemclock.h"
#include "helper.h"
#include "avrerror.h"

#include "application.h"

#include <iostream>
#include <assert.h>
#include <typeinfo>


HWIrqSystem::HWIrqSystem(AvrDevice* _core, int bytes, int tblsize):
    TraceValueRegister(_core, "IRQ"),
    bytesPerVector(bytes),
    vectorTableSize(tblsize),
    irqTrace(tblsize),
    core(_core),
    irqStatistic(_core),
    debugInterruptTable(tblsize, (Hardware*)NULL)
{
    for(unsigned int i = 0; i < vectorTableSize; i++) {
        TraceValue* tv = new TraceValue(1, GetTraceValuePrefix() + "VECTOR" + int2str(i));
        tv->set_written(0);
        RegisterTraceValue(tv);
        irqTrace[i] = tv;
    }
}

bool HWIrqSystem::IsIrqPending()
{
    return ( irqPartnerList.size() != 0); // if any interrupt is in the list, return true
}

unsigned int HWIrqSystem::GetNewPc(unsigned int &actualVector) {
    unsigned int newPC = 0xffffffff;

    static std::map<unsigned int, Hardware *>::iterator ii;
    static std::map<unsigned int, Hardware *>::iterator end;
    end = irqPartnerList.end();

    //a map is always sorted, so the priority of the irq vector is known and handled correctly
    for(ii = irqPartnerList.begin(); ii != end; ii++) {
        Hardware* second = ii->second;
        unsigned int index = ii->first;
        assert(index < vectorTableSize);

        if(second->IsLevelInterrupt(index)) {
            second->ClearIrqFlag(index);
            if(second->LevelInterruptPending(index)) {
                actualVector = index;
                newPC = index * (bytesPerVector / 2);
                break;
            }
        } else {
            second->ClearIrqFlag(index);
            actualVector = index;
            newPC = index * (bytesPerVector / 2);
            break;
        }
    }

    return newPC;
}


void HWIrqSystem::SetIrqFlag(Hardware *hwp, unsigned int vector) {
    assert(vector < vectorTableSize);
    irqPartnerList[vector]=hwp;
    if (core->trace_on) {
        traceOut << core->GetFname() << " IRQ: " << vector << " " << core->GetInterruptVectorName( vector ) << " is pending" << std::endl;
    }
    
    irqStatistic.SetIrqFlag( vector, SystemClock::Now() );
}

void HWIrqSystem::ClearIrqFlag(unsigned int vector) {
    irqPartnerList.erase(vector);
    if (core->trace_on) {
        traceOut << core->GetFname() << " IRQ: " << vector << " " << core->GetInterruptVectorName( vector ) << " flag cleared" << std::endl;
    }

    irqStatistic.ClearIrqFlag( vector, SystemClock::Now() );
} 

void HWIrqSystem::IrqHandlerStarted(uint32_t stackPointer, unsigned int vector) {
    irqTrace[vector]->change(1);
    if (core->trace_on) {
        traceOut << core->GetFname() << " IRQ: " << vector << " " << core->GetInterruptVectorName( vector ) << " handler started" << std::endl;
    }

    irqStatistic.IrqHandlerStarted( vector, SystemClock::Now(), stackPointer );
}

void HWIrqSystem::IrqHandlerFinished(unsigned int stackPointer, unsigned int vector) {
    irqTrace[vector]->change(0);
    if (core->trace_on) {
        traceOut << core->GetFname() << " IRQ: " << vector << " " << core->GetInterruptVectorName( vector ) << " handler finished" << std::endl;
    }

    irqStatistic.IrqHandlerFinished(  vector, SystemClock::Now(), stackPointer );

    if ( core->trace_on )
    {
        traceOut << irqStatistic << std::endl;
    }
}

void HWIrqSystem::DebugVerifyInterruptVector(unsigned int vector, const Hardware* source) {
    assert(vector < vectorTableSize);
    const Hardware* existing = debugInterruptTable[vector];
    if(existing == NULL)
        debugInterruptTable[vector] = source;
    else
        assert(existing == source);
    // The same `source' for multiple `vector' values is OK. It would be pain
    // for ExternalIRQSingle class to inherit from Hardware just for this test.
}
void HWIrqSystem::DebugDumpTable()
{
    avr_message("Interrupt vector table (for comparison against a datasheet)\n");
    avr_message("Vector | Address/2 | Source Peripheral (class)\n");
    for(unsigned i = 0; i < debugInterruptTable.size(); i++)
    {
        const Hardware* source = debugInterruptTable[i];
        const char * handler = (i==0) ? "funct AvrDevice::Reset()"
            : source ? typeid(*source).name() : "(unsupported or not registered)";
        avr_message("  %3d  |   $%04x   | %s\n", i+1, i*bytesPerVector/2, handler);
    }
}

