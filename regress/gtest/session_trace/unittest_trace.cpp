#include <iostream>
#include <fstream>
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
#include "avrfactory.h"
#include "avrsignature.h"
#include "traceval.h"

// test function
void do_test(const char* devicename, const char* obj) {
    // dump manager
    DumpManager::Instance()->Reset();
    DumpManager *dman = DumpManager::Instance();
    dman->SetSingleDeviceApp();
    sysConHandler.SetTraceFile("trace.txt", 100);

    // set device and device signature
    AvrDevice *dev = AvrFactory::instance().makeDevice(devicename);
    map<string, unsigned int>::iterator cur  = AvrNameToSignatureMap.find(devicename);
    unsigned int sig;
    if(cur != AvrNameToSignatureMap.end()) {
        // signature found
        sig = cur->second;
    } else {
        //cout << "no signature found for " << devicename << endl;
        sig = -1;
    }
    dev->SetDeviceNameAndSignature(devicename, sig);

    // configure vcd trace output
    TraceSet tset = dman->load("+ STACK.SPH\n+ STACK.SPL\n| CORE.r 16 .. 20");
    Dumper *vcd = new DumpVCD("trace.vcd", "ns", true, true);
    dman->addDumper(vcd, tset);
    
    // set clock freq. and firmware
    dev->SetClockFreq(125); // time base is 1ns!
    dev->Load(obj);
    dev->Reset(); // reset after load data from file to activate fuses and lockbits
    if(sysConHandler.GetTraceState())
        dev->trace_on = 1;

    // run program
    long steps = 0;
    dman->start(); // start dump session
    SystemClock::Instance().ResetClock();
    SystemClock::Instance().Add(dev);
    steps = SystemClock::Instance().Run(5000); // time base is 1ns!
    dman->stopApplication(); // stop dump session. Close dump files, if necessary

    // check lines in output files
    ifstream traceFile("trace.txt");
    long trace_count = count(istreambuf_iterator<char>(traceFile), istreambuf_iterator<char>(), '\n');
    ifstream vcdFile("trace.vcd");
    long vcd_count = count(istreambuf_iterator<char>(vcdFile), istreambuf_iterator<char>(), '\n');
    
    // check values
    EXPECT_GE(steps, 41) << "to less steps processed, expected more than or equal 41, processed " << (int)steps << endl;
    EXPECT_GE(trace_count, 41) << "trace.txt has less than 41 lines, found " << (int)trace_count << endl;
    EXPECT_GE(vcd_count, 170) << "trace.vcd has less than 170 lines, found " << (int)vcd_count << endl;
    EXPECT_EQ(0x40, (unsigned char)(*(dev->rw[16]))) << "wrong value on register R16" << endl;
    EXPECT_EQ(0x41, (unsigned char)(*(dev->rw[17]))) << "wrong value on register R17" << endl;
    EXPECT_EQ(0x51, (unsigned char)(*(dev->rw[18]))) << "wrong value on register R18" << endl;
    EXPECT_EQ(0x10, (unsigned char)(*(dev->rw[19]))) << "wrong value on register R19" << endl;
    EXPECT_GE((unsigned char)(*(dev->rw[20])), 7) << "wrong value register R20" << endl;
        
    // clean up
    delete dev;
}

// test instrumentation
TEST(SESSION_TRACE, TRACE_M128)
{
    do_test("atmega128", "session_trace/tc1.atmega128.o");
}

TEST(SESSION_TRACE, TRACE_M16)
{
    do_test("atmega16", "session_trace/tc1.atmega16.o");
}

TEST(SESSION_TRACE, TRACE_M8)
{
    do_test("atmega8", "session_trace/tc1.atmega8.o");
}

TEST(SESSION_TRACE, TRACE_M164)
{
    do_test("atmega164", "session_trace/tc1.atmega164a.o");
}

TEST(SESSION_TRACE, TRACE_M48)
{
    do_test("atmega48", "session_trace/tc1.atmega48.o");
}

TEST(SESSION_TRACE, TRACE_M640)
{
    do_test("atmega640", "session_trace/tc1.atmega640.o");
}

TEST(SESSION_TRACE, TRACE_T2313)
{
    do_test("attiny2313", "session_trace/tc1.attiny2313.o");
}

TEST(SESSION_TRACE, TRACE_T25 )
{
    do_test("attiny25", "session_trace/tc1.attiny25.o");
}

// EOF