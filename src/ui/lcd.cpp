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


/**
  General hint for using Lcds:

  The display controller support 4 and 8 bit port.
  If the display should be used in 4 bit mode the UPPER 4 data
  pins must be connected. This is also the case for the simulated device!
  So use d4..d7 for data port connection in 4 bit mode.

  Before startup, a real hardware can be in 4 bit mode or in 8 bit mode
  and if in 4 bit mode we have no idea if it is reading the upper or lower
  nibble next.
  The device may be not completely initialized or may have wrong initialization
  by getting some spikes on port pins as long the DDR is not set or even
  by setting the DDR and PORT in wrong order.

  As this it is common to send two times 0x3x to the device to switch definitely
  to 8 bit mode, even if only 4 bit mode is desired! This is the only chance
  to get the 4 bit mode into sync, as there is no control line which selects
  upper and lower nibble for data transfer. Setting up 8 bit mode and after that
  to 4 bit mode it is guaranteed after this command that we now read/write the upper
  nibble as next! From that data transfer we send/receive always pairs of upper 
  nibble first followed by lower nibble.

  The simulated device can be set manipulated for simulation to be in any of the 3 states:
  8 bit mode
  4 bit mode, waiting for lower nibble
  4 bit mode, waiting for upper nibble 

  This makes it possible to check if tested code syncs up correctly in every case!
  Use Set4BitMode(), SetLowNibble() for this!
  Default mode is 8 bit at startup. This is the default mode for the real hardware
  but only valid if it was not unintentionally changed on startup before controller
  starts up.
  */


 

#include <iostream>
#include <fstream>

#include "lcd.h"
#include "pinatport.h"
#include "systemclock.h"

void Lcd::Set4BitMode( bool mode4bit_ )
{
    mode4bit = mode4bit_;
}

void Lcd::SetLowNibble( bool lowNibble_ )
{
    lowNibble = lowNibble_;
}

void Lcd::LcdWriteData(unsigned char data) {
    if ( busyUntil )
    {
        // ubs! we are busy and got a write...
        std::cerr << "Lcd got WriteData while still busy!" << std::endl;

        if ( trace_on )
        {
            traceOut << "Lcd got WriteData while still busy! ";
        }

        return; // we ignore DataWrite!
    }

    if ( trace_on )
    {
        traceOut << "WriteData " << std::hex << (unsigned int)data << " " << data << " ";
    }

    std::ostringstream os;
    os << name << " WriteChar " << merke_x+1 << " " << merke_y << " " << (unsigned int)data << std::endl;
    ui->Write(os.str());

    merke_x++;
    SendCursorPosition();

    busyUntil = SystemClock::Now() + standardDelayTime;
}

void Lcd::SendCursorPosition() {
    std::ostringstream os;
    os << name << " MoveCursor " << merke_x << " " << merke_y << " " <<  std::endl;
    ui->Write(os.str());

    if ( trace_on )
    {
        traceOut << "Lcd: " << name << " MoveCursor " << merke_x << " " << merke_y << " ";
    }
}

void Lcd::LcdWriteCommand(unsigned char command) {
    if ( busyUntil )
    {   
        // ubs! we are busy and got a write...
        std::cerr << "Lcd got WriteCommand while still busy!" << std::endl;

        if ( trace_on )
        {
            traceOut << "Lcd got WriteCommand while still busy! ";
        }   

        return; // we ignore DataWrite!
    } 

    if ( trace_on )
    {
        traceOut << "WriteCommand " << std::hex << (unsigned int)command << std::dec << " ";
    }

    if (command >= 0x80 ) { // set ddram address ( goto )

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

        busyUntil = SystemClock::Now() + standardDelayTime;
        return;
    }

    if (command >= 0x40) { //Set Character Generator Address
        if ( trace_on )
        {
            traceOut << "Not supported LCD command: Set Character Generator Address ";
        }

        busyUntil = SystemClock::Now() + standardDelayTime;
        return;
    }

    if (command >= 0x20) { //Function Set
        if ( trace_on )
        {
            traceOut << "Function Set ";
        }
        if ((command & 0x10)) {
            if ( trace_on ) { traceOut << "mode 8 bit "; }
            mode4bit = false;
        } else {
            if ( trace_on ) { traceOut << "mode 4 bit "; }
            mode4bit = true;
            lowNibble = false;
        }


        if ((command & 0x04)) {
            if ( trace_on )
            {
                traceOut << "Not supported LCD command: 5*10 char. size";
            }
        }

        if ( !firstFunctionSetCommandReceived )
        {
            firstFunctionSetCommandReceived = true;
            busyUntil = SystemClock::Now() + clearDisplayTime; 
        }
        else
        {
            busyUntil = SystemClock::Now() + standardDelayTime;
        }
        return;
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
                if ( trace_on )
                {
                    traceOut << "Not supported LCD command: Display shift left or right";
                }
                break;
            default:
                break;
        }

        busyUntil = SystemClock::Now() + standardDelayTime;
        return;
    }

    if (command >= 8) { //Display on / off
        if ( trace_on )
        {
            traceOut << "Not supported LCD command: Display off / Cursor off / Cursor Blink";
        }

        busyUntil = SystemClock::Now() + standardDelayTime;
        return;
    }

    if (command >= 4) { //Set Entry Mode
        if ( trace_on )
        {
            traceOut << "Not supported LCD command: Set Entry Mode";
        }

        busyUntil = SystemClock::Now() + standardDelayTime;
        return;
    }

    if (command >= 2) { //Return Home
        merke_x=0;
        merke_y=0;
        SendCursorPosition();

        busyUntil = SystemClock::Now() + clearDisplayTime;
    }

    if (command==1) { //clear
        if ( trace_on ) { traceOut << "Clear "; }

        for (merke_y=3; merke_y>=0; merke_y--) {
            for ( merke_x=0; merke_x<=19; merke_x++) {
                std::ostringstream os;
                os << name << " WriteChar " << merke_x+1 << " " << merke_y << " " << (unsigned int)' ' << std::endl;
                ui->Write(os.str());
            }
        }
        merke_x=0;
        merke_y=0;
        SendCursorPosition();

        busyUntil = SystemClock::Now() + clearDisplayTime;
    }
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
    if ( busyUntil )
    {
        return 0x80;   // busy flag is transmitted as bit 7 by command read
    }   

    return 0x00;
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
    if ( trace_on )
    {
        traceOut << "Lcd: " << name << " ";
    }

    if ( busyUntil && ( busyUntil < SystemClock::Now() )  )
    {
        busyUntil = 0;
        if ( trace_on )
        {
            traceOut << "active: clear busy flag! ";
        }
    }

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
        if ( trace_on )
        {
            traceOut << "active: ";
        }

        lastEnable = localEnable;

        if ( trace_on )
        {

            if ( controlLines & COMMAND )
            {
                traceOut << "DATA      ";
            } else
            {
                traceOut << "COMMAND   ";
            }

            if ( controlLines & READWRITE )
            {
                traceOut << "READ      ";
            }
            else
            {
                traceOut << "WRITE     ";
            }

            if ( controlLines & ENABLE )
            {
                traceOut << "ENABLE    ";
            }
            else
            {
                traceOut << "--E--     ";
            }

            if ( controlLines & MODE4BIT )
            {
                traceOut << "4 bit     ";
            }
            else
            {
                traceOut << "8 bit     ";
            }

            if ( controlLines & LOWNIBBLE )
            {
                traceOut << " LOW NIBBLE ";
            }
            else
            {
                traceOut << "HIGH NIBBLE ";
            }

        } // trace_on



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
    else    // no change on port
    {
        if ( trace_on )
        {
            traceOut << "no change on enable line, do nothing";
        }
    }

    if ( trace_on )
    {
        traceOut << std::endl;
    }

    if( timeToNextStepIn_ns != 0 ) 
    {
        *timeToNextStepIn_ns = 0; //call as fast as possible
    }

    return 0;
}


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
    busyUntil{ SystemClock::Now() + powerUpTime },   // at power up, we wait a bit longer
    mode4bit{ false },
    lowNibble{ false },
    firstFunctionSetCommandReceived{ false }  // normal init sequence starts with write command 0x30 multiple times, first time results in longer busy time
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

