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


// global switch to enable irq statistic (default is disabled)
bool enableIRQStatistic = false;

void IrqStatisticEntry::CalcDiffs() {
    setClear          =flagCleared-flagSet;
    setStarted        =handlerStarted-flagSet;
    setFinished       =handlerFinished-flagSet;
    startedFinished   =handlerFinished-handlerStarted;
}

std::string GetInterruptVectorName( AvrDevice* core, unsigned int vectorNumber )
{
    unsigned int cnt;
    const char*const* names;
    core->GetInterruptVectorNames( names, cnt );

    if ( cnt > vectorNumber )
    {
        return { std::string{"\""} + names[vectorNumber] + "\"" };
    }

    return {};
}

IrqStatisticPerVector::IrqStatisticPerVector() {
    // set the "short" params to the max values so that the first "not dummy" is smaller 
    //  and set to the real statistic
    IrqStatisticEntry longDummy;
    IrqStatisticEntry shortDummy;

    longDummy.setClear=0xffffffffffffll;
    longDummy.setStarted=0xffffffffffffll;
    longDummy.setFinished=0xffffffffffffll;
    longDummy.startedFinished=0xffffffffffffll;

    shortDummy.setClear=0;
    shortDummy.setStarted=0;
    shortDummy.setFinished=0;
    shortDummy.startedFinished=0;

    long_SetClear=shortDummy;
    long_SetStarted=shortDummy;
    long_SetFinished=shortDummy;
    long_StartedFinished=shortDummy;

    short_SetClear=longDummy;
    short_SetStarted=longDummy;
    short_SetFinished=longDummy;
    short_StartedFinished=longDummy;
}

void IrqStatisticPerVector::CalculateStatistic() {
    actual.CalcDiffs();

    if (actual.setClear< short_SetClear.setClear) {
        short_SetClear=actual;
    }

    if (actual.setClear> long_SetClear.setClear) {
        long_SetClear=actual;
    }

    if (actual.setStarted< short_SetStarted.setStarted) {
        short_SetStarted=actual;
    }

    if( actual.setStarted> long_SetStarted.setStarted) {
        long_SetStarted=actual;
    }

    if ( actual.setFinished< short_SetFinished.setFinished) {
        short_SetFinished= actual;
    }

    if ( actual.setFinished > long_SetFinished.setFinished) {
        long_SetFinished= actual;
    }

    if (actual.startedFinished < short_StartedFinished.startedFinished) {
        short_StartedFinished= actual;
    }

    if (actual.startedFinished > long_StartedFinished.startedFinished) {
        long_StartedFinished= actual;
    }
}

void IrqStatisticPerVector::CheckComplete() {
    if ((actual.flagSet!=0) &&
            (actual.flagCleared!=0) &&
            (actual.handlerStarted!=0) &&
            (actual.handlerFinished!=0)) 
    {
        CalculateStatistic();

        last=actual;
        actual=IrqStatisticEntry{};
    }
}

std::ostream& operator<<(std::ostream &os, const IrqStatisticEntry& ise) {
    os << std::dec<<"\t";
     
    os << FormattedTime( ise.flagSet ) << "\t";
    os << FormattedTime( ise.flagCleared ) << "\t"; 
    os << FormattedTime( ise.handlerStarted )<< "\t"; 
    os << FormattedTime( ise.handlerFinished ) << "\t";

    os << FormattedTime( ise.setClear ) << "\t";
    os << FormattedTime( ise.setStarted ) << "\t"; 
    os << FormattedTime( ise.setFinished ) << "\t"; 
    os << FormattedTime( ise.startedFinished )<< "\t"; 

    return os;
}

std::string Cycles( const IrqStatisticEntry& ise, AvrDevice* core ) {
    // Get time for a single step of cpu core

    std::ostringstream os;
    os << std::dec<<"\t";
    SystemClockOffset ns = core->GetClockFreq();

    os << FormattedTime( ise.flagSet ) << "\t";
    os << FormattedTime( ise.flagCleared ) << "\t"; 
    os << FormattedTime( ise.handlerStarted )<< "\t"; 
    os << FormattedTime( ise.handlerFinished ) << "\t";

    os << std::setw(16) << ( ise.setClear/ns  ) << "\t";
    os << std::setw(16) << ( ise.setStarted/ns ) << "\t"; 
    os << std::setw(16) << ( ise.setFinished/ns ) << "\t"; 
    os << std::setw(16) << ( ise.startedFinished/ns )<< "\t"; 

    return os.str();
}

std::string Print( const IrqStatisticPerVector &ispv, AvrDevice* core ) {
    std::ostringstream os;
    os << "Entry type                     t Flag Set          t Flag Cleared      t Handler Started   t Handler Finished  dt Set->Clear       dt Set->Start       dt Set->Finish      dt Start->Finish" << std::endl;
    os << "last                     " << ispv.last << std::endl;
    os << "Set->Clear >             " << ispv.long_SetClear << std::endl; 
    os << "Set->Clear <             " << ispv.short_SetClear << std::endl;
    os << "Set->HandlerStarted >    " << ispv.long_SetStarted << std::endl; 
    os << "Set->HandlerStarted <    " << ispv.short_SetStarted << std::endl;

    os << "Set->HandlerFinished >   " << ispv.long_SetFinished << std::endl; 
    os << "Set->HandlerFinished <   " << ispv.short_SetFinished << std::endl;
    os << "Handler Start->Finished >" << ispv.long_StartedFinished << std::endl; 
    os << "Handler Start->Finished <" << ispv.short_StartedFinished << std::endl;
    os << std::endl;

    os << "Entry type                     t Flag Set          t Flag Cleared      t Handler Started   t Handler Finished  clk Set->Clear      clk Set->Start      clk Set->Finish     clk Start->Finish" << std::endl;
    os << "last                     " << Cycles( ispv.last, core ) << std::endl;
    os << "Set->Clear >             " << Cycles( ispv.long_SetClear, core ) << std::endl; 
    os << "Set->Clear <             " << Cycles( ispv.short_SetClear, core ) << std::endl;
    os << "Set->HandlerStarted >    " << Cycles( ispv.long_SetStarted, core ) << std::endl; 
    os << "Set->HandlerStarted <    " << Cycles( ispv.short_SetStarted, core ) << std::endl;

    os << "Set->HandlerFinished >   " << Cycles( ispv.long_SetFinished, core ) << std::endl; 
    os << "Set->HandlerFinished <   " << Cycles( ispv.short_SetFinished, core ) << std::endl;
    os << "Handler Start->Finished >" << Cycles( ispv.long_StartedFinished, core ) << std::endl; 
    os << "Handler Start->Finished <" << Cycles( ispv.short_StartedFinished, core ) << std::endl;

    return os.str();
}


std::ostream& operator<<(std::ostream &os, const IrqStatistic& is) {
    std::map<unsigned int, IrqStatisticPerVector>::const_iterator ii;

    os << "IRQ STATISTIC" << std::endl;
    for (ii=is.entries.begin(); ii!= is.entries.end(); ii++) {
        os << "####################################################################################################################################################################################" << std::endl;
        os << "Core: "  << is.core->GetFname() << std::endl;
        os << "Statistic for vector: " << ii->first << " " << GetInterruptVectorName( is.core, ii->first ) << std::endl;

        os << Print( ii->second, is.core);
    }

    return os;
}

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
        traceOut << core->GetFname() << " IRQ: " << vector << " " << GetInterruptVectorName( core, vector ) << " is pending" << std::endl;
    }

    if ( irqStatistic.entries[vector].actual.flagSet==0) { //the actual entry was not used before... fine!
        irqStatistic.entries[vector].actual.flagSet=SystemClock::Instance().GetCurrentTime();
    } 
}

void HWIrqSystem::ClearIrqFlag(unsigned int vector) {
    irqPartnerList.erase(vector);
    if (core->trace_on) {
        traceOut << core->GetFname() << " IRQ: " << vector << " " << GetInterruptVectorName( core, vector ) << " flag cleared" << std::endl;
    }

    if (irqStatistic.entries[vector].actual.flagCleared==0) {
        irqStatistic.entries[vector].actual.flagCleared=SystemClock::Instance().GetCurrentTime();
    }

    irqStatistic.entries[vector].CheckComplete();
} 

void HWIrqSystem::IrqHandlerStarted(unsigned int vector) {
    irqTrace[vector]->change(1);
    if (core->trace_on) {
        traceOut << core->GetFname() << " IRQ: " << vector << " " << GetInterruptVectorName( core, vector ) << " handler started" << std::endl;
    }

    if (irqStatistic.entries[vector].actual.handlerStarted==0) {
        irqStatistic.entries[vector].actual.handlerStarted=SystemClock::Instance().GetCurrentTime();
    }
    irqStatistic.entries[vector].CheckComplete();
}

void HWIrqSystem::IrqHandlerFinished(unsigned int vector) {
    irqTrace[vector]->change(0);
    if (core->trace_on) {
        traceOut << core->GetFname() << " IRQ: " << vector << " " << GetInterruptVectorName( core, vector ) << " handler finished" << std::endl;
    }

    if (irqStatistic.entries[vector].actual.handlerFinished==0) {
        irqStatistic.entries[vector].actual.handlerFinished=SystemClock::Instance().GetCurrentTime();
    }

    irqStatistic.entries[vector].CheckComplete();

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

IrqStatistic::IrqStatistic (AvrDevice *c):Printable(std::cout), core(c) {
    Application::GetInstance()->RegisterPrintable(this);
}

//the standard function object for a printable is printing to "out", so we do this here 
void IrqStatistic::operator()() {
    if(enableIRQStatistic)
        out << *this;
}

