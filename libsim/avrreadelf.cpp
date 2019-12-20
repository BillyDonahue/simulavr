/*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002 - 2012   Klaus Rudolph & other
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

#include <string>
#include <map>
#include <limits>

#include "elfio/elfio.hpp"

#include "config.h"
#include "flash.h"
#include "hweeprom.h"

#include "avrreadelf.h"

void ELFLoad(const AvrDevice * core) {
    ELFIO::elfio reader;

    if(!reader.load(core->actualFilename))
        avr_error("File '%s' not found or isn't a elf object",
                  core->actualFilename.c_str());

    if(reader.get_machine() != EM_AVR)
        avr_error("ELF file '%s' is not for Atmel AVR architecture (%d)",
                  core->actualFilename.c_str(),
                  reader.get_machine());

    // over all symbols ...
    ELFIO::Elf_Half sec_num = reader.sections.size();

    for(ELFIO::Elf_Half i = 0; i < sec_num; i++) {
        ELFIO::section* psec = reader.sections[i];

        if(psec->get_type() == SHT_SYMTAB) {
            const ELFIO::symbol_section_accessor symbols(reader, psec);

            for(ELFIO::Elf_Xword j = 0; j < symbols.get_symbols_num(); j++) {
                std::string       name;
                ELFIO::Elf64_Addr value = 0;
                ELFIO::Elf_Xword  size = 0;
                unsigned char     bind = 0;
                unsigned char     type = 0;
                ELFIO::Elf_Half   section_index = 0;
                unsigned char     other = 0;
                
                // read symbol properties
                symbols.get_symbol(j, name, value, size, bind,
                                      type, section_index, other);

                // check zero length names
                if(name.length() == 0)
                    continue;

                // don't list section absolute symbols
                if(section_index == SHN_ABS)
                    continue;

                // don't list local symbols
                if((bind == STB_LOCAL) && (type != STT_NOTYPE))
                    continue;

                if(value < 0x800000) {
                    // range of flash space (.text)
                    std::pair<unsigned int, std::string> p(value >> 1, name);

                    core->Flash->AddSymbol(p);
                } else if(value < 0x810000) {
                    // range of ram (.data)
                    ELFIO::Elf64_Addr offset = value - 0x800000;
                    std::pair<unsigned int, std::string> p(offset, name);

                    core->data->AddSymbol(p);
                } else if(value < 0x820000) {
                    // range of eeprom (.eeprom)
                    ELFIO::Elf64_Addr offset = value - 0x810000;
                    std::pair<unsigned int, std::string> p(offset, name);

                    core->eeprom->AddSymbol(p);
                } else if(value < 0x820400) {
                    /* fuses space starting from 0x820000, do nothing */;
                } else if(value >= 0x830000 && value < 0x830400) {
                    /* lock bits starting from 0x830000, do nothing */;
                } else if(value >= 0x840000 && value < 0x840400) {
                    /* signature space starting from 0x840000, do nothing */;
                } else
                    avr_warning("Unknown symbol address range found! (symbol='%s', address=0x%lx)",
                                name.c_str(),
                                value);

            }
        }
    }

    // load program, data and - if available - eeprom, fuses and signature
    ELFIO::Elf_Half seg_num = reader.segments.size();

    for(ELFIO::Elf_Half i = 0; i < seg_num; i++) {
        ELFIO::segment* pseg = reader.segments[i];

        if(pseg->get_type() == PT_LOAD) {
            ELFIO::Elf_Xword  filesize = pseg->get_file_size();
            ELFIO::Elf64_Addr vma = pseg->get_virtual_address();
            ELFIO::Elf64_Addr pma = pseg->get_physical_address();

            if(filesize == 0)
                continue;

            const unsigned char* data = (const unsigned char*)pseg->get_data();

            if(vma < 0x810000) {
                // read program, space below 0x810000 (.text)
                core->Flash->WriteMem(data, pma, filesize);
            } else if(vma >= 0x810000 && vma < 0x820000) {
                // read eeprom content, if available, space from 0x810000 to 0x820000 (.eeprom)
                unsigned int offset = vma - 0x810000;

                core->eeprom->WriteMem(data, offset, filesize);
            } else if(vma >= 0x820000 && vma < 0x820400) {
                // read fuses, if available, space from 0x820000 to 0x820400
                if(!core->fuses->LoadFuses(data, filesize))
                    avr_error("wrong byte size of fuses");
            } else if(vma >= 0x830000 && vma < 0x830400) {
                // read lock bits, if available, space from 0x830000 to 0x830400
                if(!core->lockbits->LoadLockBits(data, filesize))
                    avr_error("wrong byte size of lock bits");
            } else if(vma >= 0x840000 && vma < 0x840400) {
                // read and check signature, if available, space from 0x840000 to 0x840400
                if(filesize != 3)
                    avr_error("wrong device signature size in elf file, expected=3, given=%lu",
                              filesize);
                else {
                    unsigned int sig = (((data[2] << 8) + data[1]) << 8) + data[0];

                    if(core->devSignature != std::numeric_limits<unsigned int>::max() && sig != core->devSignature)
                        avr_error("wrong device signature, expected=0x%x, given=0x%x",
                                  core->devSignature,
                                  sig);
                }
            }
        }
    }
}

unsigned int ELFGetSignature(const char *filename) {
    unsigned int signature = std::numeric_limits<unsigned int>::max();
    ELFIO::elfio reader;

    if(!reader.load(filename))
        avr_error("File '%s' not found or isn't a elf object", filename);

    if(reader.get_machine() != EM_AVR)
        avr_error("ELF file '%s' is not for Atmel AVR architecture (%d)",
                  filename,
                  reader.get_machine());

    ELFIO::Elf_Half seg_num = reader.segments.size();

    for(ELFIO::Elf_Half i = 0; i < seg_num; i++) {
        ELFIO::segment* pseg = reader.segments[i];

        if(pseg->get_type() == PT_LOAD) {
            ELFIO::Elf_Xword  filesize = pseg->get_file_size();
            ELFIO::Elf64_Addr vma = pseg->get_virtual_address();

            if(filesize == 0)
                continue;

            if(vma >= 0x840000 && vma < 0x840400) {
                // read and check signature, if available, space from 0x840000 to 0x840400
                if(filesize != 3)
                    avr_error("wrong device signature size in elf file, expected=3, given=%lu",
                              filesize);
                else {
                    const unsigned char* data = (const unsigned char*)pseg->get_data();

                    signature = (((data[2] << 8) + data[1]) << 8) + data[0];
                    break;
                }
            }
        }
    }

    return signature;
}

// EOF
