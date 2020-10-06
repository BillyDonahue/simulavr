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

#include <fstream>
#include <sstream>
#include <iomanip>

#include <stdlib.h>

#include "cmd/dumpargs.h"
#include "helper.h"
#include "avrerror.h"
#include "flash.h"
#include "hweeprom.h"


 
void SetDumpTraceArgs(const std::vector<std::string> &traceopts, AvrDevice *dev) {
    DumpManager *dman = DumpManager::Instance();
    for(size_t i = 0; i < traceopts.size(); i++) {
        std::vector<std::string> ls = split(traceopts[i], ":");
        if(ls.size() < 1)
            avr_error("Invalid tracing option '%s'.", traceopts[i].c_str());
        Dumper *d;
        TraceSet ts;
        std::cerr << "Enabling tracer: '";
        if(ls[0] == "warnread") {
            std::cerr << "warnread'." << std::endl;
            if(ls.size() > 1)
                avr_error("Invalid number of options for 'warnread'.");
            ts = dman->all();
            d = new WarnUnknown(dev);
        } else if (ls[0] == "vcd") {
            std::cerr << "vcd'." << std::endl;
            if(ls.size() < 3 || ls.size() > 4)
                avr_error("Invalid number of options for 'vcd'.");
            std::cerr << "Reading values to trace from '" << ls[1] << "'." << std::endl;
        
            std::ifstream is(ls[1].c_str());
            if(is.is_open() == 0)
                avr_error("Can't open '%s'", ls[1].c_str());
        
            std::cerr << "Output VCD file is '" << ls[2] << "'." << std::endl;
            ts = dman->load(is);
        
            bool rs = false, ws = false;
            if(ls.size() == 4) { // ReadStrobe/WriteStrobe display specified?
                if(ls[3] == "rw") {
                    rs = ws = true;
                } else if(ls[3] == "r") {
                    rs = true;
                } else if(ls[3] == "w") {
                    ws = true;
                } else
                    avr_error("Invalid read/write strobe specifier '%s'", ls[3].c_str());
            }
            d = new DumpVCD(ls[2], "ns", rs, ws);
        } else
            avr_error("Unknown tracer '%s'", ls[0].c_str());
        dman->addDumper(d, ts);
    }
}
 
void ShowRegisteredTraceValues(const std::string &outname) {
    std::cerr << "Dumping traceable values to ";
    if(outname != "-")
        std::cerr << "'" << outname << "'." << std::endl;
    else
        std::cerr << "stdout." << std::endl;
    
    std::ostream *outf;
    if(outname != "-")
        outf = new std::ofstream(outname.c_str());
    else
        outf = &std::cout;
   
    DumpManager::Instance()->save(*outf);
    
    if(outf != &std::cout)
        delete outf;
}

static void WriteCoreDumpIO(std::ostream &outf, AvrDevice *dev, int offs, int size) {
    int hsize = (size + 1) / 2;
    const int sp_name = 10, sp_col = 15; // place for IO register name an gap size between columns
    for(int i = 0; i < hsize; i++) {
        // left column
        std::string regname = dev->rw[i + offs]->GetTraceName();
        unsigned char val = 0;
        if(dev->rw[i + offs]->IsInvalid())
            regname = "Reserved";
        else
            val = (unsigned char)*(dev->rw[i + offs]);
        outf << std::hex << std::setw(2) << std::setfill('0') << std::right << (i + offs) << " : "
             << std::setw(sp_name) << std::setfill(' ') << std::left << regname << " : "
             << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::right << (int)val;
        if((i + hsize) >= size)
            outf << std::endl; // odd count of IO registers?
        else {
            // right column
            regname = dev->rw[i + hsize + offs]->GetTraceName();
            val = 0;
            if(dev->rw[i + hsize + offs]->IsInvalid())
                regname = "Reserved";
            else
                val = (unsigned char)*(dev->rw[i + hsize + offs]);
            outf << std::setw(sp_col) <<  std::setfill(' ') << " "
                 << std::hex << std::setw(2) << std::setfill('0') << std::right << (i + hsize + offs) << " : "
                 << std::setw(sp_name) << std::setfill(' ') << std::left << regname << " : "
                 << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::right << (int)val
                 << std::endl;
        }
    }
}

static void WriteCoreDumpRAM(std::ostream &outf, AvrDevice *dev, int offs, int size) {
    const int maxLineByte = 16;
    std::ostringstream buf;
    int start = offs, lastStart = 0, dup = 0, j = 0;
    std::string lastLine("");

    for(int i = 0; i < size; i++) {
        buf << std::hex << std::setw(2) << std::setfill('0') << (int)((unsigned char)*(dev->rw[i + offs])) << " ";
        if(++j == maxLineByte) {
            if(buf.str() == lastLine) // check for duplicate line
              dup++;
            else {
              if(dup > 0) outf << "  -- last line repeats --" << std::endl;
              outf << std::hex << std::setw(4) << std::setfill('0') << std::right << start << " : " << buf.str() << std::endl;
              dup = 0;
              lastLine = buf.str();
            }
            j = 0;
            lastStart = start;
            start += maxLineByte;
            buf.str("");
        }
    }
    if((j > 0) || (dup > 0)) {
        if(dup > 0) outf << "  -- last line repeats --" << std::endl;
        if(j == 0)
          outf << std::hex << std::setw(4) << std::setfill('0') << std::right << lastStart << " : " << lastLine << std::endl;
        else
          outf << std::hex << std::setw(4) << std::setfill('0') << std::right << start << " : " << buf.str() << std::endl;
    }
}

static void WriteCoreDumpEEPROM(std::ostream &outf, AvrDevice *dev, int size) {
    const int maxLineByte = 16;
    std::ostringstream buf;
    int start = 0, lastStart = 0, dup = 0, j = 0;
    std::string lastLine("");

    for(int i = 0; i < size; i++) {
        buf << std::hex << std::setw(2) << std::setfill('0') << (int)(dev->eeprom->ReadFromAddress(i)) << " ";
        if(++j == maxLineByte) {
            if(buf.str() == lastLine) // check for duplicate line
              dup++;
            else {
              if(dup > 0) outf << "  -- last line repeats --" << std::endl;
              outf << std::hex << std::setw(4) << std::setfill('0') << std::right << start << " : " << buf.str() << std::endl;
              dup = 0;
              lastLine = buf.str();
            }
            j = 0;
            lastStart = start;
            start += maxLineByte;
            buf.str("");
        }
    }
    if((j > 0) || (dup > 0)) {
        if(dup > 0) outf << "  -- last line repeats --" << std::endl;
        if(j == 0)
          outf << std::hex << std::setw(4) << std::setfill('0') << std::right << lastStart << " : " << lastLine << std::endl;
        else
          outf << std::hex << std::setw(4) << std::setfill('0') << std::right << start << " : " << buf.str() << std::endl;
    }
}

static void WriteCoreDumpFlash(std::ostream &outf, AvrDevice *dev, int size) {
    const int maxLineWord = 8;
    std::ostringstream buf;
    int start = 0, lastStart = 0, dup = 0, j = 0;
    std::string lastLine("");

    for(int i = 0; i < size; i += 2) {
        buf << std::hex << std::setw(4) << std::setfill('0') << dev->Flash->ReadMemRawWord(i) << " ";
        if(++j == maxLineWord) {
            if(buf.str() == lastLine) // check for duplicate line
              dup++;
            else {
              if(dup > 0) outf << "  -- last line repeats --" << std::endl;
              outf << std::hex << std::setw(4) << std::setfill('0') << std::right << start << " : " << buf.str() << std::endl;
              dup = 0;
              lastLine = buf.str();
            }
            j = 0;
            lastStart = start;
            start += maxLineWord;
            buf.str("");
        }
    }
    if((j > 0) || (dup > 0)) {
        if(dup > 0) outf << "  -- last line repeats --" << std::endl;
        if(j == 0)
          outf << std::hex << std::setw(4) << std::setfill('0') << std::right << lastStart << " : " << lastLine << std::endl;
        else
          outf << std::hex << std::setw(4) << std::setfill('0') << std::right << start << " : " << buf.str() << std::endl;
    }
}

void WriteCoreDump(const std::string &outname, AvrDevice *dev) {
    std::ostream *outf;

    // open dump file
    if(outname != "-")
        outf = new std::ofstream(outname.c_str());
    else
        outf = &std::cout;

    // write out PC
    *outf << "PC = 0x" << std::hex << std::setw(6) << std::setfill('0') << dev->PC
          << " (PC*2 = 0x" << std::hex << std::setw(6) << std::setfill('0') << (dev->PC * 2)
          << ")" << std::endl << std::endl;

    // write out general purpose register
    *outf << "General Purpose Register Dump:" << std::endl;
    for(unsigned int i = 0, j = 0; i < dev->GetMemRegisterSize(); i++) {
        *outf << std::dec << "r" << std::setw(2) << std::setfill('0') << i << "="
              << std::hex << std::setw(2) << std::setfill('0') << (int)((unsigned char)*(dev->rw[i])) << "  ";
        j++;
        if(j == 8) {
            *outf << std::endl;
            j = 0;
        }
    }
    *outf << std::endl;

    // write out IO register
    *outf << "IO Register Dump:" << std::endl;
    WriteCoreDumpIO(*outf, dev, dev->GetMemRegisterSize(), dev->GetMemIOSize());
    *outf << std::endl;

    // write out internal RAM
    *outf << "Internal SRAM Memory Dump:" << std::endl;
    WriteCoreDumpRAM(*outf, dev, dev->GetMemRegisterSize() + dev->GetMemIOSize(), dev->GetMemIRamSize());
    *outf << std::endl;

    // write out external RAM
    if(dev->GetMemERamSize() > 0) {
        *outf << "External SRAM Memory Dump:" << std::endl;
        WriteCoreDumpRAM(*outf, dev, dev->GetMemRegisterSize() + dev->GetMemIOSize() + dev->GetMemIRamSize(), dev->GetMemERamSize());
        *outf << std::endl;
    }

    // write out EEPROM content
    *outf << "EEPROM Memory Dump:" << std::endl;
    WriteCoreDumpEEPROM(*outf, dev, dev->eeprom->GetSize());
    *outf << std::endl;

    // write out flash content
    *outf << "Program Flash Memory Dump:" << std::endl;
    WriteCoreDumpFlash(*outf, dev, dev->Flash->GetSize());
    *outf << std::endl;

    // close file
    if(outf != &std::cout)
        delete outf;
}

// EOF
