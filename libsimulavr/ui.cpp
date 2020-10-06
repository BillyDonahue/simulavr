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

#include <time.h>
#include "externaltype.h"
#include "ui/ui.h"
#include "hardware.h"
#include "pin.h"
#include "systemclock.h"
#include "avrerror.h"
#include <sstream>



UserInterface::UserInterface(int port, bool _withUpdateControl): Socket(port), updateOn(1), pollFreq(100000)  {
    if (_withUpdateControl) {
        waitOnAckFromTclRequest=0;
        waitOnAckFromTclDone=0;
        std::ostringstream os;
        os << "create UpdateControl dummy dummy " << std::endl; 
        Write(os.str());
        AddExternalType("UpdateControl", this);
    }
}

UserInterface::~UserInterface() {
}

void UserInterface::SwitchUpdateOnOff(bool yesNo) {
    updateOn=yesNo;
}


int UserInterface::Step(bool &dummy1, SystemClockOffset *nextStepIn_ns) {
    if (nextStepIn_ns!=0) {
        *nextStepIn_ns=pollFreq;
    }

    static time_t oldTime=0;
    time_t newTime=time(NULL);

    if (updateOn || (newTime!=oldTime)) {
        oldTime=newTime;

        do { 
            if (Poll()!=0) {
                ssize_t len = 0;
                len=Read(dummy);

                //std::string debug=dummy;

                while (len>0) {

                    std::string::size_type pos;

                    pos=dummy.find(" ");

                    std::string net=dummy.substr(0, pos);
                    std::string rest=dummy.substr(pos+1); //vfrom pos+1 to end

                    if (net == "exit" )
                        avr_error("Exiting at external UI request");

                    std::string par;
                    int pos2=rest.find(" ");

                    if (pos2<=0) break;

                    par= rest.substr(0, pos2);
                    dummy=rest.substr(pos2+1);

                    // std::cerr << "UI: net=" << net << "- rest=" << rest << std::endl;
                    if (net == "__ack" ) {
                        waitOnAckFromTclDone++;
                    } else {
                        std::map<std::string, ExternalType*>::iterator ii;
                        ii=extMembers.find(net);
                        if (ii != extMembers.end() ) {
                            (ii->second)->SetNewValueFromUi(par);
                        } else {
                            // std::cerr << "Netz nicht gefunden:" << net << std::endl;
                            // std::cerr << "Start with std::string >>" << net << "<<" << std::endl;
                        }

                        //if (trace_on!=0) traceOut << "Net: " << net << "changed to " << par << std::endl;

                    } //__ack

                    len=dummy.size(); //recalc size from rest of std::string

                } // len > 0
            } //poll
        }while (waitOnAckFromTclRequest > waitOnAckFromTclDone+500); 


        if (waitOnAckFromTclRequest!=waitOnAckFromTclDone) {
            waitOnAckFromTclRequest=waitOnAckFromTclDone=0;
        }

    } //if (update_on  | look for reenable again)

    return 0;
}



void UserInterface::SendUiNewState(const std::string &s, const char &c)  {
    std::ostringstream os;
    //static map<std::string, char> LastState;

    if (LastState[s]==c) {
        return;
    }
    LastState[s]=c;

    os << "set " << s << " " << c << std::endl;
    Write(os.str());

    //    SystemClock::Instance().Rescedule(this, 1000); //read ack back as fast as possible
}

void UserInterface::SetNewValueFromUi(const std::string &value){
    if (value=="0") {
        updateOn=false;
    } else {
        updateOn=true;
    }

}

void UserInterface::Write(const std::string &s) {
    if (updateOn) {

        for (unsigned int tt = 0; tt< s.length() ; tt++) {
            if (s[tt]=='\n') {
                waitOnAckFromTclRequest++;
            }
        }
        Socket::Write(s);
    }
} 

