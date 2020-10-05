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

#include <stdlib.h> // use atol

#include "extpin.h"
#include "net.h"

#include <sstream>

ExtPin::ExtPin(T_Pinstate ps,
               UserInterface *_ui,
               const char *_extName,
               const char *baseWindow):
    Pin(ps),
    ui(_ui),
    extName(_extName)
{
    std::ostringstream os;
    outState=ps;
    os << "create Net " << _extName << " " << baseWindow << " " << std::endl;
    ui->Write(os.str());

    ui->AddExternalType(extName, this);

    // We also want to set the external GUI to represent the initial value
    // given as first parameter to the ExtPin.

    // Create pin representation as char for gui
    const char defaultState = *this;

    os.str(""); // make it empty
    os.clear(); // reset state flags

    os << "setfrombackdoor " << extName << " " << defaultState << std::endl;
    ui->Write(os.str());
}

void ExtPin::SetInState(const Pin &p) {
    ui->SendUiNewState(extName, p);
}

void ExtPin::SetNewValueFromUi(const std::string& s) {
    Pin tmp;
    tmp= s[0];
    //outState= tmp.GetOutState();
    outState= tmp.outState;

    connectedTo->CalcNet();
}


// #################################################################################


void ExtAnalogPin::SetNewValueFromUi(const std::string& s) {
    outState= ANALOG;

    //analogValue=atol(s.c_str());
    //connectedTo->CalcNet();

    SetAnalogValue(atof(s.c_str()));
}


ExtAnalogPin::ExtAnalogPin(unsigned int value,
                           UserInterface *_ui,
                           const char *_extName,
                           const char* baseWindow):
    Pin(Pin::TRISTATE),
    ui(_ui),
    extName(_extName)
{
    std::ostringstream os;
    os << "create AnalogNet " << _extName << " " << baseWindow << " " << std::endl;
    ui->Write(os.str());

    ui->AddExternalType(extName, this);
}

void ExtAnalogPin::SetInState(const Pin &p) {
    ui->SendUiNewState(extName, p);
}


// #################################################################################


ExtPinButton::ExtPinButton(T_Pinstate psReleased,
                           T_Pinstate psPressed,
                           UserInterface *ui_,
                           const char *extName_,
                           const char *baseWindow):
    Pin(psReleased),
    ui(ui_),
    extName(extName_)
{
    std::ostringstream os;
    outState=psReleased;
    os << "create NetButton " << extName_ << " " << baseWindow << " " << (char)Pin(psReleased) << " " << (char)Pin(psPressed) << std::endl;
    ui->Write(os.str());

    ui->AddExternalType(extName, this);

    // We also want to set the external GUI to represent the initial value
    // given as first parameter to the ExtPin.

    // Create pin representation as char for gui
    const char defaultState = *this;

    os.str(""); // make it empty
    os.clear(); // reset state flags

    os << "setfrombackdoor " << extName << " " << defaultState << std::endl;
    ui->Write(os.str());
}

void ExtPinButton::SetInState(const Pin &p) {
    ui->SendUiNewState(extName, p);
}

void ExtPinButton::SetNewValueFromUi(const std::string& s) {
    Pin tmp;
    tmp= s[0];
    //outState= tmp.GetOutState();
    outState= tmp.outState;

    connectedTo->CalcNet();
}


