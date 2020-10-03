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
    // only calc if we have valid entries in both parms for diff
    if ( ( flagCleared     != INVALID ) && ( flagSet        != INVALID ) ) { setClear          = flagCleared -     flagSet; }
    if ( ( handlerStarted  != INVALID ) && ( flagSet        != INVALID ) ) { setStarted        = handlerStarted -  flagSet; }
    if ( ( handlerFinished != INVALID ) && ( flagSet        != INVALID ) ) { setFinished       = handlerFinished - flagSet; }
    if ( ( handlerFinished != INVALID ) && ( handlerStarted != INVALID ) ) { startedFinished   = handlerFinished - handlerStarted; }
}

bool IrqStatisticEntry::CheckComplete() {
    if (
        (flagSet!=INVALID) &&
        (flagCleared!=INVALID) &&
        (handlerStarted!=INVALID) &&
        (handlerFinished!=INVALID)
       ) 
    {
        return true;
    }

    return false;
}


IrqStatisticPerVector::IrqStatisticPerVector():
    dummyInitObject{ std::make_shared<IrqStatisticEntry>() },
    long_SetClear{ dummyInitObject },
    short_SetClear{ dummyInitObject },
    long_SetStarted{ dummyInitObject },
    short_SetStarted{ dummyInitObject },
    long_SetFinished{ dummyInitObject },
    short_SetFinished{ dummyInitObject },
    long_StartedFinished{ dummyInitObject },
    short_StartedFinished{ dummyInitObject },
    last{ dummyInitObject }
{
    dummyInitObject.reset(); // we do not need it any longer
}

void IrqStatisticPerVector::CalculateStatistic() {
    std::vector< decltype( unfinishedEntries.begin() ) > toRemove;

    for ( auto it= unfinishedEntries.begin(); it != unfinishedEntries.end(); it++)
    {
        auto& el=*it;

        if ( 
            ( el->setClear != INVALID ) &&
            (
                ( el->setClear< short_SetClear->setClear ) ||
                ( short_SetClear->setClear == INVALID )
            )
           )
        {
            short_SetClear=el;
        }

        if (
            ( el->setClear != INVALID ) &&
            (
                ( el->setClear> long_SetClear->setClear ) ||
                ( long_SetClear->setClear == INVALID )
            )
           )
        {
            long_SetClear=el;
        }

        if (
            ( el->setStarted != INVALID ) &&
            (
                ( el->setStarted< short_SetStarted->setStarted ) ||
                ( short_SetStarted->setStarted == INVALID )
            )
           )
        {
            short_SetStarted=el;
        }

        if ( 
            ( el->setStarted != INVALID ) &&
            (
                ( el->setStarted> long_SetStarted->setStarted ) ||
                ( long_SetStarted->setStarted == INVALID )
            )
           )
        {
            long_SetStarted=el;
        }

        if (
            ( el->setFinished != INVALID ) &&
            (
                ( el->setFinished< short_SetFinished->setFinished ) ||
                ( short_SetFinished->setFinished == INVALID )
            )
           ) 
        {
            short_SetFinished= el;
        }

        if ( 
            ( el->setFinished != INVALID ) &&
            (
                ( el->setFinished > long_SetFinished->setFinished ) ||
                ( long_SetFinished->setFinished == INVALID )
            )
           )
        {
            long_SetFinished= el;
        }

        if (
            ( el->startedFinished != INVALID ) &&
            (
                ( el->startedFinished < short_StartedFinished->startedFinished ) ||
                ( short_StartedFinished->startedFinished != INVALID )
            )
           )
        {
            short_StartedFinished= el;
        }

        if (
            ( el->startedFinished != INVALID ) &&
            (
                ( el->startedFinished > long_StartedFinished->startedFinished ) ||
                ( long_StartedFinished->startedFinished == INVALID )
            )
           )
        {
            long_StartedFinished= el;
        }

        // if one of the entries has all elements set, we can remove it from the all entries list
        // to print it ones, we leave the entry in the "last" element
        if ( el->CheckComplete() )
        {
            last = el;
            toRemove.push_back( it );
        }
    }

    // now we remove all "completed" entries as they are already part of the statistic summerize table
    for ( auto& rem: toRemove )
    {
        unfinishedEntries.erase( rem );
    }


}

std::ostream& operator<<(std::ostream &os, const IrqStatisticEntry& ise) {
    std::string invalid_string= "               *";
    invalid_string += '\t';

    os << std::dec<<"\t";

    if ( ise.flagSet         != INVALID ) { os << FormattedTime( ise.flagSet ) << "\t"; } else { os << invalid_string ; }
    if ( ise.flagCleared     != INVALID ) { os << FormattedTime( ise.flagCleared ) << "\t";     } else { os << invalid_string ; }
    if ( ise.handlerStarted  != INVALID ) { os << FormattedTime( ise.handlerStarted )<< "\t";   } else { os << invalid_string ; }
    if ( ise.handlerFinished != INVALID ) { os << FormattedTime( ise.handlerFinished ) << "\t"; } else { os << invalid_string ; }

    if ( ise.setClear        != INVALID ) { os << FormattedTime( ise.setClear ) << "\t";    } else { os << invalid_string ; }
    if ( ise.setStarted      != INVALID ) { os << FormattedTime( ise.setStarted ) << "\t";  } else { os << invalid_string ; }
    if ( ise.setFinished     != INVALID ) { os << FormattedTime( ise.setFinished ) << "\t";     } else { os << invalid_string ; }
    if ( ise.startedFinished != INVALID ) { os << FormattedTime( ise.startedFinished )<< "\t";  } else { os << invalid_string ; }

    return os;
}

std::string Cycles( const std::shared_ptr<IrqStatisticEntry>& ise, AvrDevice* core ) {
    // Get time for a single step of cpu core

    std::string invalid_string= "               *";
    invalid_string += '\t';

    std::ostringstream os;
    os << std::dec<<"\t";
    SystemClockOffset ns = core->GetClockFreq();

    if ( ise->flagSet         != INVALID ) { os << FormattedTime( ise->flagSet ) << "\t"; } else { os << invalid_string ; }
    if ( ise->flagCleared     != INVALID ) { os << FormattedTime( ise->flagCleared ) << "\t";     } else { os << invalid_string ; }
    if ( ise->handlerStarted  != INVALID ) { os << FormattedTime( ise->handlerStarted )<< "\t";   } else { os << invalid_string ; }
    if ( ise->handlerFinished != INVALID ) { os << FormattedTime( ise->handlerFinished ) << "\t"; } else { os << invalid_string ; }

    if ( ise->setClear        != INVALID ) { os << std::setw(16) << ( ise->setClear/ns  ) << "\t"; } else { os << invalid_string ; }
    if ( ise->setStarted      != INVALID ) { os << std::setw(16) << ( ise->setStarted/ns ) << "\t"; }else { os << invalid_string ; }
    if ( ise->setFinished     != INVALID ) { os << std::setw(16) << ( ise->setFinished/ns ) << "\t"; }else { os << invalid_string ; }
    if ( ise->startedFinished != INVALID ) { os << std::setw(16) << ( ise->startedFinished/ns )<< "\t"; }  else { os << invalid_string ; }

    return os.str();
}

std::string Print( const IrqStatisticPerVector &ispv, AvrDevice* core ) {
    std::string headline;
    headline += "Entry type              ";

    headline += "          t Flag Set";
    headline += "      t Flag Cleared";
    headline += "   t Handler Started";
    headline += "  t Handler Finished";
    headline += "       dt Set->Clear";
    headline += "       dt Set->Start";
    headline += "      dt Set->Finish";
    headline += "    dt Start->Finish";

    std::string headline2;
    headline2 += "Entry type              ";

    headline2 += "          t Flag Set";
    headline2 += "      t Flag Cleared";
    headline2 += "   t Handler Started";
    headline2 += "  t Handler Finished";
    headline2 += "      clk Set->Clear";
    headline2 += "      clk Set->Start";
    headline2 += "     clk Set->Finish";
    headline2 += "   clk Start->Finish";

    std::ostringstream os;

    if ( ispv.unfinishedEntries.size() )
    {
        os << headline << std::endl;

        for ( auto& v: ispv.unfinishedEntries )
        {
            os << "all active handlers:     " << *v << std::endl;
        }
        os << std::endl;
    }


    os << headline << std::endl;
    os << "last                     " << *ispv.last << std::endl;
    os << "Set->Clear >             " << *ispv.long_SetClear << std::endl; 
    os << "Set->Clear <             " << *ispv.short_SetClear << std::endl;
    os << "Set->HandlerStarted >    " << *ispv.long_SetStarted << std::endl; 
    os << "Set->HandlerStarted <    " << *ispv.short_SetStarted << std::endl;

    os << "Set->HandlerFinished >   " << *ispv.long_SetFinished << std::endl; 
    os << "Set->HandlerFinished <   " << *ispv.short_SetFinished << std::endl;
    os << "Handler Start->Finished >" << *ispv.long_StartedFinished << std::endl; 
    os << "Handler Start->Finished <" << *ispv.short_StartedFinished << std::endl;
    os << std::endl;

    if ( ispv.unfinishedEntries.size() )
    {
        os << headline << std::endl;

        for ( auto& v: ispv.unfinishedEntries )
        {
            os << "all active handlers:     " << Cycles( v, core ) << std::endl;
        }
        os << std::endl;
    }

    os << headline << std::endl;
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
        os << "Statistic for vector: " << ii->first << " " << is.core->GetInterruptVectorName( ii->first ) << std::endl;

        os << Print( ii->second, is.core);
    }

    return os;
}

IrqStatistic::IrqStatistic (AvrDevice *c):Printable(std::cout), core(c) {
    Application::GetInstance()->RegisterPrintable(this);
}

//the standard function object for a printable is printing to "out", so we do this here 
void IrqStatistic::operator()() {
    if(enableIRQStatistic)
        out << *this;
}

void IrqStatistic::SetIrqFlag( unsigned int vector, SystemClockOffset time )
{
    // on getting a new irq flag set, we always start with a new statistic entry
    entries[vector].unfinishedEntries.push_back(std::make_shared<IrqStatisticEntry>());
    entries[vector].unfinishedEntries.back()->flagSet = time;
    if ( core->trace_on ) { traceOut << (*this) << std::endl; }
}

void IrqStatistic::ClearIrqFlag( unsigned int vector, SystemClockOffset time )
{
    auto& el = entries[vector].unfinishedEntries.back();
    if ( el->flagCleared!=INVALID )
    {
        if ( core->trace_on )
        {
            traceOut << "UUUUBS flagCleared was already set######################   E  R  R  O  R  ###############################" << std::endl;
        }
    }

    el->flagCleared = time;
    el->CalcDiffs();
    entries[vector].CalculateStatistic();
    if ( core->trace_on ) { traceOut << (*this) << std::endl; }
}

void IrqStatistic::IrqHandlerStarted( unsigned int vector, SystemClockOffset time, unsigned int stackPointer )
{
    auto& el = entries[vector].unfinishedEntries.back();
    if ( el->handlerStarted!=INVALID )
    {
        if ( core->trace_on )
        {
            traceOut << "UUUUBS handlerStarted was already set######################   E  R  R  O  R  ###############################" << std::endl;
        }
    }

    el->handlerStarted = time;
    el->stackPointer = stackPointer;
    el->CalcDiffs();
    entries[vector].CalculateStatistic();
    if ( core->trace_on ) { traceOut << (*this) << std::endl; }
}

void IrqStatistic::IrqHandlerFinished( unsigned int vector, SystemClockOffset time, unsigned int stackPointer )
{
    // we finish the handler which is on the given stack position as we can have nested irqs for the same
    // vector. This can happen if the handler set sei() and a new irq for the same vector occures and the 
    // same handler is started again.

    auto& vec = entries[vector].unfinishedEntries;

    // search vector from back and find same stackPointer
    for ( auto it = vec.rbegin(); it != vec.rend(); ++it )
    {
        auto& el = *it;
        if ( core->trace_on )
        {
            traceOut << "run over all entries in reverse order: " << std::endl;
            traceOut << el << std::endl;

        }

        if ( el->stackPointer == stackPointer )
        {
            if ( el->handlerFinished!=INVALID )
            {
                if ( core->trace_on )
                {
                    traceOut << "UUUUBS handlerFinished was already set######################   E  R  R  O  R  ###############################" << std::endl;
                }
            }
            el->handlerFinished = time;
            el->CalcDiffs();
            entries[vector].CalculateStatistic();
            break;
        }
    }

    if ( core->trace_on ) { traceOut << (*this) << std::endl; }


}

