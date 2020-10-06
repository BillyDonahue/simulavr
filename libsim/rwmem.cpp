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

/*
 * All here defined types are used to simulate the 
 * read write address space. This means also registers
 * io-data space, internal and external sram
 */

#include <cstdio>

#include "avrerror.h"
#include "traceval.h"
#include "avrdevice.h"
#include "helper.h"
#include "rwmem.h"



RWMemoryMember::RWMemoryMember(TraceValueRegister *_reg,
                               const std::string &_tracename,
                               const int index):
    registry(_reg),
    tracename(_tracename),
    isInvalid(false)
{
    if (_tracename.size()) {
        tv = new TraceValue(8, registry->GetTraceValuePrefix() + _tracename, index);
        if (!registry) {
            avr_error("registry not initialized for RWMemoryMember '%s'.", _tracename.c_str());
        }
        registry->RegisterTraceValue(tv);
    } else {
        tv=0;
    }
}

RWMemoryMember::RWMemoryMember(void):
    tv(NULL),
    registry(NULL),
    tracename(""),
    isInvalid(true) {}

RWMemoryMember::operator unsigned char() const {
    if (tv)
        tv->read();
    return get();
}

unsigned char RWMemoryMember::operator=(unsigned char val) {
    set(val);
    if (tv)
        tv->write(val);
    return val;
}

unsigned char RWMemoryMember::operator=(const RWMemoryMember &mm) {
    if (mm.tv)
        mm.tv->read();
    unsigned char v=mm.get();
    set(v);
    if (tv)
        tv->write(v);
    return v;
}

CLKPRRegister::CLKPRRegister(AvrDevice *core,
                             TraceValueRegister *registry):
        RWMemoryMember(registry, "CLKPR"),
        Hardware(core),
        _core(core) {
    if(_core->fuses->GetFuseBit(AvrFuses::FB_CKDIV8))
        value = 3;
    else
        value = 0;
    activate = 0;

    // connect to core to get core cycles
    core->AddToCycleList(this);
}

void CLKPRRegister::Reset(void) {
    if(_core->fuses->GetFuseBit(AvrFuses::FB_CKDIV8))
        value = 3;
    else
        value = 0;
    activate = 0;
}

unsigned int CLKPRRegister::CpuCycle(void) {
    // control clock set activation
    if(activate > 0) {
        activate--;
        value &= 0x7f; // reset CLKPCE, if set
    }
    return 0;
}

void CLKPRRegister::set(unsigned char v) {
    if(v == 0x80) {
        // set activation period
        if(activate == 0) activate = 4;
    } else if((v & 0x80) == 0) {
        if(activate > 0) {
            std::string buf = "<invalid>";
            unsigned char i = v & 0x0f;
            if(i <= 8)
                buf = "CKx" + int2str(1 << i);
            // set clock prescaler
            avr_warning("CLKPR: change clock prescaler to %s (0x%x)", buf.c_str(), i);
        }
    }
    value = v;
}

XDIVRegister::XDIVRegister(AvrDevice *core,
                             TraceValueRegister *registry):
        RWMemoryMember(registry, "XDIV"), 
        Hardware(core)
{
    Reset();
}

void XDIVRegister::set(unsigned char v) {
    bool old_enbl = (value & 0x80) == 0x80, new_enbl = (v & 0x80) == 0x80;
    if(new_enbl) {
        if(!old_enbl) {
            // enable clock divider
            avr_warning("XDIV: clock divider enabled, CKx%d", 129 - (v & 0x7f));
            // if XDIVEN == 1, XDIV[6:0] are only changeable, if XDIVEN == 0 before!
            value = v;
        }
    } else {
        if(old_enbl)
            // disable clock divider
            avr_warning("XDIV: clock divider disabled, CKx1");
        value = v;
    }
}

OSCCALRegister::OSCCALRegister(AvrDevice *core,
                             TraceValueRegister *registry,
                             int cal):
        RWMemoryMember(registry, "OSCCAL"),
        Hardware(core),
        cal_type(cal)
{
    Reset();
}

void OSCCALRegister::Reset(void) {
    // set factory calibration value, the used value is just a interpolation from datasheet!
    // The real value could differ from device to device.
    if(cal_type == OSCCAL_V3)
        value = 85;
    else
        value = 42;
}

void OSCCALRegister::set(unsigned char v) {
    if(cal_type == OSCCAL_V4)
        v &= 0x7f;
    if(value != v)
        avr_warning("OSCCAL: change oscillator calibration value to 0x%x", v);
    value = v;
}

RAM::RAM(TraceValueCoreRegister *_reg, const std::string &name, const size_t number, const size_t maxsize) {
    corereg = _reg;
    value = 0xaa;
    if(name.size()) {
        tv = new TraceValue(8, corereg->GetTraceValuePrefix() + name, number);
        if(!corereg) {
            avr_error("registry not initialized for RWMemoryMember '%s'.", name.c_str());
        }
        corereg->RegisterTraceSetValue(tv, name, maxsize);
    } else {
        tv = NULL;
    }
}

unsigned char RAM::get() const { return value; }

void RAM::set(unsigned char v) { value=v; }

InvalidMem::InvalidMem(AvrDevice* _c, int _a):
    RWMemoryMember(),
    core(_c),
    addr(_a),
    value(0xAA) {}

unsigned char InvalidMem::get() const {
    std::string s = "Invalid read access from IO[0x" + int2hex(addr) + "], PC=0x" + int2hex(core->PC * 2);
    unsigned int a = addr & core->dataAddressMask;
    unsigned int r = core->GetMemIOSize() + core->GetMemRegisterSize() + core->GetMemIRamSize() + core->GetMemERamSize();
    if(core->abortOnInvalidAccess)
        avr_error("%s", s.c_str());
    if(!global_suppress_memory_warnings)
        avr_warning("%s", s.c_str());
    if(a < r)
        return value;
    return 0;
}

void InvalidMem::set(unsigned char c) {
    std::string s = "Invalid write access to IO[0x" + int2hex(addr) +
        "]=0x" + int2hex(c) + ", PC=0x" + int2hex(core->PC * 2);
    unsigned int a = addr & core->dataAddressMask;
    unsigned int r = core->GetMemIOSize() + core->GetMemRegisterSize() + core->GetMemIRamSize() + core->GetMemERamSize();
    if(core->abortOnInvalidAccess)
        avr_error("%s", s.c_str());
    if(!global_suppress_memory_warnings)
        avr_warning("%s", s.c_str());
    if(a < r)
        value = c;
}

NotSimulatedRegister::NotSimulatedRegister(const char * oname, const char * rname)
    : obj_name(oname),
      reg_name(rname)  {}

unsigned char NotSimulatedRegister::get() const {
    if(!global_suppress_memory_warnings)
        avr_warning("%s register %s not simulated (read from register)", obj_name, reg_name);
    return 0;
}

void NotSimulatedRegister::set(unsigned char c) {
    if(!global_suppress_memory_warnings)
        avr_warning("%s register %s not simulated (write 0x%02x to register)", obj_name, reg_name, (unsigned)c);
}

NotSimulatedRegister NSR_TWI_TWAMR = NotSimulatedRegister("TWI", "TWAMR");
NotSimulatedRegister NSR_TWI_TWCR = NotSimulatedRegister("TWI", "TWCR");
NotSimulatedRegister NSR_TWI_TWDR = NotSimulatedRegister("TWI", "TWDR");
NotSimulatedRegister NSR_TWI_TWAR = NotSimulatedRegister("TWI", "TWAR");
NotSimulatedRegister NSR_TWI_TWSR = NotSimulatedRegister("TWI", "TWSR");
NotSimulatedRegister NSR_TWI_TWBR = NotSimulatedRegister("TWI", "TWBR");
NotSimulatedRegister NSR_ADC_DIDR0 = NotSimulatedRegister("ADC", "DIDR0");
NotSimulatedRegister NSR_ADC_DIDR1 = NotSimulatedRegister("ADC", "DIDR1");
NotSimulatedRegister NSR_ADC_DIDR2 = NotSimulatedRegister("ADC", "DIDR2");
NotSimulatedRegister NSR_MCU_PRR = NotSimulatedRegister("MCU", "PRR");
NotSimulatedRegister NSR_MCU_PRR0 = NotSimulatedRegister("MCU", "PRR0");
NotSimulatedRegister NSR_MCU_PRR1 = NotSimulatedRegister("MCU", "PRR1");
NotSimulatedRegister NSR_MCU_WDTCSR = NotSimulatedRegister("MCU", "WDTCSR");
NotSimulatedRegister NSR_MCU_MCUCR = NotSimulatedRegister("MCU", "MCUCR");
NotSimulatedRegister NSR_MCU_MCUSR = NotSimulatedRegister("MCU", "MCUSR");
NotSimulatedRegister NSR_MCU_SMCR = NotSimulatedRegister("MCU", "SMCR");
NotSimulatedRegister NSR_OCD_OCDR = NotSimulatedRegister("On chip debug", "OCDR");
NotSimulatedRegister NSR_XMC_XMCRA = NotSimulatedRegister("External memory control", "XMCRA");
NotSimulatedRegister NSR_XMC_XMCRB = NotSimulatedRegister("External memory control", "XMCRB");

NotSimulatedRegister* NSR[NotSimulatedRegister::NSR_size] = {
   &NSR_TWI_TWAMR,
   &NSR_TWI_TWCR,
   &NSR_TWI_TWDR,
   &NSR_TWI_TWAR,
   &NSR_TWI_TWSR,
   &NSR_TWI_TWBR,
   &NSR_ADC_DIDR0,
   &NSR_ADC_DIDR1,
   &NSR_ADC_DIDR2,
   &NSR_MCU_PRR,
   &NSR_MCU_PRR0,
   &NSR_MCU_PRR1,
   &NSR_MCU_WDTCSR,
   &NSR_MCU_MCUCR,
   &NSR_MCU_MCUSR,
   &NSR_MCU_SMCR,
   &NSR_OCD_OCDR,
   &NSR_XMC_XMCRA,
   &NSR_XMC_XMCRB,
};

NotSimulatedRegister* NotSimulatedRegister::getRegister(int reg) {
    if(reg >= NotSimulatedRegister::NSR_size)
        avr_error("wrong register id for NotSimulatedRegister");
    return NSR[reg];
}

IOSpecialReg::IOSpecialReg(TraceValueRegister *registry, const std::string &name):
    RWMemoryMember(registry, name)
{
    Reset();
}

unsigned char IOSpecialReg::get() const {
    unsigned char val = value;
    for(size_t i = 0; i < clients.size(); i++)
        val = clients[i]->get_from_client(this, val);
    return val;
}

void IOSpecialReg::set(unsigned char val) {
    for(size_t i = 0; i < clients.size(); i++)
        val = clients[i]->set_from_reg(this, val);
    value = val;
    hardwareChange(value);
}

// EOF
