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

#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED


#include <fstream>
#include <string>

#include "systemclocktypes.h"
#include "simulationmember.h"
//#include "hardware.h"
#include "ui.h"
#include "pin.h"

enum t_myState {
        IDLE,
        POWER_ON,     // First State after Reset
        PWR_AFTER_FS1,// After first Function Set Cmd no Busy Flag
        PWR_AFTER_FS2,// After second Function Set Cmd no Busy Flag
        PWR_ON_FINISH,// After third Function Set Cmd no Busy Flag. After the next CMD BF is valid
        CMDEXEC       // Executing any command after init
    } ;


inline void PrintState(t_myState val)
{
    switch( val )
    {
        case IDLE: std::cerr << "IDLE"; break;
        case POWER_ON: std::cerr << "POWER_ON"; break;
        case PWR_AFTER_FS1: std::cerr << "PWR_AFTER_FS1"; break;
        case PWR_AFTER_FS2: std::cerr << "PWR_AFTER_FS2"; break;
        case PWR_ON_FINISH: std::cerr << "PWR_ON_FINISH"; break;
        case CMDEXEC: std::cerr << "CMDEXEC"; break;
    }
}



/** Simulates busyFlaga HD44780 character-LCD controller with a 4 bit interface.
 * This HD-controller is boring slow :-) like some original.
 */
class Lcd : public SimulationMember {
    private:
        void SetPort( unsigned char newValue );
        void TriStatePort(); 

    protected:
        UserInterface *ui;
        std::string name;
        unsigned char portValue;
        std::map<std::string, Pin*> allPins;
        Pin d0;
        Pin d1;
        Pin d2;
        Pin d3;
        Pin d4;
        Pin d5;
        Pin d6;
        Pin d7;

        Pin enable;
        Pin readWrite;
        Pin commandData;

        unsigned int CmdExecTime_ns; // Command Execution Time
        t_myState myState;           // LCD State-Event machine
        char      myd3;              // internal D3


        int merke_x;
        int merke_y;

        void LcdWriteData(unsigned char data);
        unsigned int LcdWriteCommand(unsigned char command);
        unsigned char LcdReadData();
        unsigned char LcdReadCommand();

        void SendCursorPosition();
        bool lastEnable;
        bool busyFlag;
        bool mode4bit;

        bool lowNibble;  // in 4 bit mode we first receive/send higher 4 bit data
        unsigned char data;  // if we are running in 4 bit mode, we carry our read values here

    public:
        virtual int Step(bool &trueHwStep, SystemClockOffset *timeToNextStepIn_ns=0);
        //Lcd(UserInterface *ui, const string &name, const string &baseWindow);
        Lcd(UserInterface *ui, const char *name, const char *baseWindow);
        virtual ~Lcd();
        Pin *GetPin(const char *name);
};

#endif
