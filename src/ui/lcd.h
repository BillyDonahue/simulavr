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
#include "ui.h"
#include "pin.h"

/** 
  Simulates busyFlaga HD44780 character-LCD controller with a 4 and 8 bit interface.
 */
class Lcd : public SimulationMember {
    private:
        void SetPort( unsigned char newValue );
        void TriStatePort(); 

        static constexpr int lcdStartLine []={0, 0x40, 0x14, 0x20};


        // worst case
        // static constexpr SystemClockOffset powerUpTime = 40_ms;
        // static constexpr SystemClockOffset clearDisplayTime = 4.1_ms;
        // static constexpr SystemClockOffset standardDelayTime = 100_us;

        // typical timings 190kHz from TC1602A-01T datasheet
        static constexpr SystemClockOffset powerUpTime = 15_ms;
        static constexpr SystemClockOffset clearDisplayTime = 2.16_ms;
        static constexpr SystemClockOffset standardDelayTime = 53_us;



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

        int merke_x;
        int merke_y;

        void LcdWriteData(unsigned char data);
        void LcdWriteCommand(unsigned char command);
        unsigned char LcdReadData();
        unsigned char LcdReadCommand();

        void SendCursorPosition();
        bool lastEnable;
        SystemClockOffset busyUntil;    //!< lcd is busy until this simulation time has expired, 0 if not busy
        bool mode4bit;                  //!< true if in 4 bit mode
        bool lowNibble;                 //!< in 4 bit mode we first receive/send higher 4 bit data, true if wait for low nibble
        unsigned char data;             //!< if we are running in 4 bit mode, we carry our read values here
        bool firstFunctionSetCommandReceived; //!< init sequence may have multiple SetFunctionMode commands, first takes longer

    public:
        virtual int Step(bool &trueHwStep, SystemClockOffset *timeToNextStepIn_ns=0);
        Lcd(UserInterface *ui, const char *name, const char *baseWindow);
        virtual ~Lcd();
        Pin *GetPin(const char *name);

        /**
          Force Lcd controller to operate in 4 or 8 bit mode. Function
          is only used to control initial startup behaviour to test
          user code for all variants of startup sequences. default for
          hardware startup is 8 bit mode.
          */
        void Set4BitMode( bool mode4bit_ );

        /**
          Forces Lcd to read/write as next the low nibble.
          The function only takes effect if controller is set in 4 bit mode.
          Functuion is only used to set up the controller at startup to test
          user code for correct initilaization of the device
          */
        void SetLowNibble( bool lowNibble_ );
};

#endif
