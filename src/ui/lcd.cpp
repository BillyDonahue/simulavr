/*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002, 2003, 2020   Klaus Rudolph
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
 * Now LCD works with all known displays and we use correct enable edge!
 * Busy and wait time is currently not implemented as we simply
 * did not know how much simulated time we have between step calls
 * LCD works now in 4 and 8 bit mode
 * as this, in 4 bit mode data pins 4..7 must been connected!
 *
 *  $Id$
 */

#include <iostream>
#include <fstream>

#include "lcd.h"
#include "pinatport.h"

int lcdStartLine []={0, 0x40, 0x14, 0x20};
// Exec Times for HD44780 5V version! At 2.7V the first time rises from 15 to 40ms!
//                 Power-on, 1st CMD, 2nd Cmd, 3rd Cmd, typical, Ret-Home
//static int Power_onTimes[]={15000000, 4100000,  100000,   37000, 1520000};
static int Power_onTimes[]={1500000, 410000,  10000,   3700, 152000};

void Lcd::LcdWriteData(unsigned char data) {
    //std::cerr << "Lcd::LcdWriteData " << std::hex << (unsigned int)data << std::dec << std::endl;

    std::ostringstream os;
    os << name << " WriteChar " << merke_x+1 << " " << merke_y << " " << (unsigned int)data << std::endl;
    ui->Write(os.str());

    merke_x++;
    SendCursorPosition();
}

void Lcd::SendCursorPosition() {
    std::ostringstream os;
    os << name << " MoveCursor " << merke_x << " " << merke_y << " " <<  std::endl;
    ui->Write(os.str());
}

unsigned int  Lcd::LcdWriteCommand(unsigned char command) {
    std::cerr << "Lcd::LcdWriteCommand " << std::hex << (unsigned int)command << std::dec << std::endl;

    // TODO: busy flag dependent on timing,
    // currently only a single ReadCommand busy
    busyFlag = true;

    bool lerr = false;
    if (command >= 0x80 ) { //goto

        int line;
        int value=command-0x80;
        if (value >= 0x54) {line=3; value-=0x54;} else {
            if (value >= 0x40) {line=1; value-=0x40;} else {
                if (value >= 0x14) {line=2; value-=0x14;} else {
                    line=0; }}}

        merke_x=value;
        merke_y=line;

        merke_x++;
        SendCursorPosition();

        return Power_onTimes[3];
    }

    if (command >= 0x40) { //Set Character Generator Address
        std::cerr << "Not supported LCD command: Set Character Generator Address " << std::endl;
        return Power_onTimes[3];
    }
    if (command >= 0x20) { //Function Set
        if ((command & 0x10)) {
            mode4bit = false;
        } else {
            mode4bit = true;
        }


        if ((command & 0x04)) {
            std::cerr << "Not supported LCD command: 5*10 char. size";
            lerr = true;
        }
        if (lerr == true) {
            std::cerr << std::endl;
        }
        return Power_onTimes[3];
    }

    if (command >= 0x10) { //Cursor or Display shift
        command &= 0x0c; // Mask S/L, R/L Bits, ignore rest
        switch (command) {
            case 0:
                merke_x--;
                break;
            case 4:
                merke_x++;
                break;
            case 8:
            case 0x0c:
                std::cerr << "Not supported LCD command: Display shift left or right" << std::endl;
                break;
            default:
                break;
        }
        return Power_onTimes[3];
    }

    if (command >= 8) { //Display on / off
        if (command != 0x0e) {// E = Display on, Cursor on, Cursor Blink off
            std::cerr << "Not supported LCD command: Display off / Cursor off / Cursor Blink" << std::endl;
        }
        return Power_onTimes[3];
    }

    if (command >= 4) { //Set Entry Mode
        if (command != 6) {// 6 = Increment, Cursor movement
            std::cerr << "Not supported LCD command: Set Entry Mode" << std::endl;
        }
        return Power_onTimes[3];
    }

    if (command >= 2) { //Return Home
        merke_x=0;
        merke_y=0;
        SendCursorPosition();
        return Power_onTimes[4];
    }

    if (command==1) { //clear
        for (merke_y=3; merke_y>=0; merke_y--) {
            for ( merke_x=0; merke_x<=19; ) {
                LcdWriteData(' ');
            }
        }
        merke_x=0;
        merke_y=0;
        SendCursorPosition();
        return Power_onTimes[4];
    }
    return 0;  // Should not come here! Added to avoid warning
}

void Lcd::SetPort( unsigned char value )
{
    d7 = ( value & 0x80 ) ? 'H' : 'L';
    d6 = ( value & 0x40 ) ? 'H' : 'L';
    d5 = ( value & 0x20 ) ? 'H' : 'L';
    d4 = ( value & 0x10 ) ? 'H' : 'L';
    d3 = ( value & 0x08 ) ? 'H' : 'L';
    d2 = ( value & 0x04 ) ? 'H' : 'L';
    d1 = ( value & 0x02 ) ? 'H' : 'L';
    d0 = ( value & 0x01 ) ? 'H' : 'L';
}

void Lcd::TriStatePort()
{
    d7 = 't';
    d6 = 't';
    d5 = 't';
    d4 = 't';
    d3 = 't';
    d2 = 't';
    d1 = 't';
    d0 = 't';
}

unsigned char Lcd::LcdReadCommand()
{
    // TODO: busy flag is not depending on timing
    unsigned char value = busyFlag << 7;
    busyFlag = false;
    return value;
}

/*
   currently not implemented, but data is present on bus,
   adding functionality to read out real data is now simple!
   */
unsigned char Lcd::LcdReadData()
{
    return 0x23;
}

int Lcd::Step( bool &trueHwStep, SystemClockOffset *timeToNextStepIn_ns ) 
{
    static constexpr unsigned int COMMAND       = 0x01;  // HIGH -> Data, LOW -> Command
    static constexpr unsigned int READWRITE     = 0x02;  // HIGH -> Read, LOW -> Write
    static constexpr unsigned int ENABLE        = 0x04;  // edge to low, take data
    static constexpr unsigned int MODE4BIT      = 0x08;  // TRUE-> we are running 4 bit
    static constexpr unsigned int LOWNIBBLE     = 0x10;  // if true, we read the low nibble of a byte in 4 bit mode

    unsigned int controlLines = ( (lowNibble << 4) | (mode4bit << 3) | ((bool)enable << 2) | ( (bool)readWrite << 1 ) | ( (bool)commandData ) ); 

    bool localEnable = (bool)enable;    // get value from input pin

    /*
       in 4 bit mode high nibble is transfered first
       */


    if ( lastEnable !=  localEnable )
    {
        lastEnable = localEnable;
#ifdef DEBUG_LCD

        if ( controlLines & COMMAND )
        {
            std::cerr << "DATA      ";
        } else
        {
            std::cerr << "COMMAND   ";
        }

        if ( controlLines & READWRITE )
        {
            std::cerr << "READ      ";
        }
        else
        {
            std::cerr << "WRITE     ";
        }

        if ( controlLines & ENABLE )
        {
            std::cerr << "ENABLE    ";
        }
        else
        {
            std::cerr << "--E--     ";
        }

        if ( controlLines & MODE4BIT )
        {
            std::cerr << "4 bit     ";
        }
        else
        {
            std::cerr << "8 bit     ";
        }

        if ( controlLines & LOWNIBBLE )
        {
            std::cerr << " LOW NIBBLE ";
        }
        else
        {
            std::cerr << "HIGH NIBBLE ";
        }

        std::cerr << std::endl;
#endif

        // always we loose enable, we have to remove data from our bus lines
        if ( !((bool)enable))
        {
            TriStatePort();
        }

        switch ( controlLines )
        {
            case 0x00:
                LcdWriteCommand( portValue );
                break;

            case COMMAND:
                LcdWriteData( portValue );
                break;

            case MODE4BIT | LOWNIBBLE: // transfer complete
                lowNibble = false;
                data |= portValue >> 4;
                LcdWriteCommand( data );
                break;

            case MODE4BIT | LOWNIBBLE | COMMAND: // transfer complete
                lowNibble = false;
                data |= portValue >> 4;
                LcdWriteData( data );
                break;

            case MODE4BIT :
            case MODE4BIT | COMMAND:    // read upper nibble first
                lowNibble = true;
                data = portValue & 0xf0;
                break;


                // read command -> busy flag
            case            ENABLE | READWRITE :
                SetPort( LcdReadCommand() );
                break;

            case MODE4BIT | ENABLE | READWRITE :   // high nibble
                lowNibble = true;
                data = LcdReadCommand();
                SetPort( data & 0xf0 );
                break;

            case MODE4BIT | LOWNIBBLE | ENABLE | READWRITE :  // low nibble on upper half of port 
                lowNibble = false;
                SetPort( data << 4 );
                break; 

                // read data 
            case            COMMAND | ENABLE | READWRITE :
                SetPort( LcdReadData() );
                break;


            case MODE4BIT | COMMAND | ENABLE | READWRITE :  // 4 bit high nibble first
                lowNibble = true;
                data = LcdReadData();
                SetPort( data & 0xf0 );
                break;

            case MODE4BIT | LOWNIBBLE | COMMAND | ENABLE | READWRITE :  // 4 bit low nibble
                lowNibble = false;
                SetPort( data << 4 );
                break;

            default:
                ;

        }
    }


    if( timeToNextStepIn_ns != 0 ) 
    {
        *timeToNextStepIn_ns = 0; //call as fast as possible
    }

    return 0;
}


//Lcd::Lcd(UserInterface *_ui, const string &_name, const string &baseWindow):
Lcd::Lcd(UserInterface *ui_, const char *name_, const char *baseWindow):
    ui(ui_), name(name_),
    d0( &portValue, 0x01),
    d1( &portValue, 0x02),
    d2( &portValue, 0x04),
    d3( &portValue, 0x08),
    d4( &portValue, 0x10),
    d5( &portValue, 0x20),
    d6( &portValue, 0x40),
    d7( &portValue, 0x80),
    lastEnable{ false },
    busyFlag{ false },
    mode4bit{ false },
    lowNibble{ false }
{
    allPins["d0"]=&d0;
    allPins["d1"]=&d1;
    allPins["d2"]=&d2;
    allPins["d3"]=&d3;
    allPins["d4"]=&d4;
    allPins["d5"]=&d5;
    allPins["d6"]=&d6;
    allPins["d7"]=&d7;

    allPins["e"]=&enable;
    allPins["r"]=&readWrite;
    allPins["c"]=&commandData;

    myState = POWER_ON;
    CmdExecTime_ns = Power_onTimes[0]; // 15ms = 15.000us = 15.000.000ns

    merke_x=0;
    merke_y=0;

    //setup the corrosponding ui in tcl from here
    std::ostringstream os;
    os << "create Lcd " << name  << " " << baseWindow << " " << " 20 4" << std::endl;
    ui->Write(os.str());
}

Lcd::~Lcd() { }

Pin *Lcd::GetPin(const char *name) {
    return allPins[name];
}

