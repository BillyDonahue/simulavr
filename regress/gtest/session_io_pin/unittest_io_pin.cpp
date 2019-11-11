#include <iostream>
using namespace std;

#include "gtest.h"

#include "avrdevice.h"
#include "atmega128.h"
#include "atmega16_32.h"
#include "atmega8.h"
#include "atmega1284abase.h"
#include "atmega668base.h"
#include "atmega2560base.h"
#include "attiny2313.h"
#include "attiny25_45_85.h"
#include "systemclock.h"

#include "pin.h"

// test function
void do_test(AvrDevice* dev, const char* obj, bool t25) {
  dev->Load(obj);
  dev->SetClockFreq(136);    // 7.3728
  dev->RegisterTerminationSymbol("stopsim");
  SystemClock::Instance().Add(dev);

  Net net;

  OpenDrain driveOpenDrain(dev->GetPin("B2"));
  net.Add(&driveOpenDrain);

  net.Add(dev->GetPin("B3"));    // read behind the open drain transistor circuit

  Pin extPullUp(Pin::PULLUP);  //create a pull up
  net.Add(&extPullUp);

  // set all other pins to defined value
  Net net2;
  net2.Add(dev->GetPin("B0"));
  net2.Add(dev->GetPin("B1"));
  net2.Add(dev->GetPin("B4"));
  net2.Add(dev->GetPin("B5"));
  if(!t25) {
    net2.Add(dev->GetPin("B6"));
    net2.Add(dev->GetPin("B7"));
  }
  Pin extPullDown(Pin::PULLDOWN); // create a pull down
  net2.Add(&extPullDown);


  SystemClock::Instance().Endless(); 

  EXPECT_EQ(0x08, (unsigned char)(*(dev->rw[17]))) << "wrong value read back from PORTB R17" << endl;
  EXPECT_EQ(0x04, (unsigned char)(*(dev->rw[18]))) << "wrong value read back from PORTB R18" << endl;
}

// test instrumentation
TEST( SESSION_IO_PIN, OPEN_DRAIN_M128 )
{
    AvrDevice *dev1= new AvrDevice_atmega128;
    do_test(dev1, "session_io_pin/tc1.atmega128.o", false);
    delete dev1;
}

TEST( SESSION_IO_PIN, OPEN_DRAIN_M16 )
{
    AvrDevice *dev1= new AvrDevice_atmega16;
    do_test(dev1, "session_io_pin/tc1.atmega16.o", false);
    delete dev1;
}

TEST( SESSION_IO_PIN, OPEN_DRAIN_M8 )
{
    AvrDevice *dev1= new AvrDevice_atmega8;
    do_test(dev1, "session_io_pin/tc1.atmega8.o", false);
    delete dev1;
}

TEST( SESSION_IO_PIN, OPEN_DRAIN_M164 )
{
    AvrDevice *dev1= new AvrDevice_atmega164A;
    do_test(dev1, "session_io_pin/tc1.atmega164a.o", false);
    delete dev1;
}

TEST( SESSION_IO_PIN, OPEN_DRAIN_M48 )
{
    AvrDevice *dev1= new AvrDevice_atmega48;
    do_test(dev1, "session_io_pin/tc1.atmega48.o", false);
    delete dev1;
}

TEST( SESSION_IO_PIN, OPEN_DRAIN_M640 )
{
    AvrDevice *dev1= new AvrDevice_atmega640;
    do_test(dev1, "session_io_pin/tc1.atmega640.o", false);
    delete dev1;
}

TEST( SESSION_IO_PIN, OPEN_DRAIN_T2313 )
{
    AvrDevice *dev1= new AvrDevice_attiny2313;
    do_test(dev1, "session_io_pin/tc1.attiny2313.o", false);
    delete dev1;
}

TEST( SESSION_IO_PIN, OPEN_DRAIN_T25 )
{
    AvrDevice *dev1= new AvrDevice_attiny25;
    do_test(dev1, "session_io_pin/tc1.attiny25.o", true);
    delete dev1;
}

// EOF