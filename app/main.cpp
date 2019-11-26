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

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <limits>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "config.h"

#include "flash.h"
#include "avrdevice.h"
#include "avrfactory.h"
#include "avrsignature.h"
#include "avrreadelf.h"
#include "gdb/gdb.h"
#include "ui/ui.h"
#include "systemclock.h"
#include "traceval.h"
#include "string2.h"
#include "helper.h"
#include "specialmem.h"
#include "irqsystem.h"

#include "cmd/dumpargs.h"

const char *SplitOffsetFile(const char *arg,
                            const char *name,
                            int base,
                            unsigned long *offset)
{
    char *end;
    
    if(!StringToUnsignedLong(arg, offset, &end, base)) {
        cerr << name << ": offset is not a number" << endl;
        exit(1);
    }
    //position behind the "," or any other delimiter for the offset
    if(!*end) {
        cerr << name << ": argument ends before filename" << endl;
        exit(1);
    }
    if(*end != ',') {
        cerr << name << ": argument does not have comma before filename" << endl;
        exit(1);
    }
    ++end;
    if(!*end) {
        cerr << name << ": argument has comma but no filename" << endl;
        exit(1);
    }
    
    return end;
}

typedef pair<string, string> string_pair_t;
vector<string_pair_t> newUsage = {
    {".",
     "Common options"},
    {"-V, --version",
     "print out version and exit immediately"},
    {"-h, --help",
     "print this help"},
    {"-v, --verbose",
     "output some hints to console"},
     
    {".",
     "Simulation options"},
    {"-d <name>, --device <name>",
     "simulate device <name>, see below for simulated devices"},
    {"-f <name>, --file <name>",
     "load elf-file <name> for simulation in simulated target"},
    {"-F <Hz>, --cpufrequency <Hz>",
     "set the cpu frequency to <Hz>"},
    {"-t <file>, --trace <file>",
     "enable trace outputs to <file>"},
    {"-s, --irqstatistic",
     "prints statistic informations about irq usage after simulation is stopped"},
    {"-C <name>, --core-dump <name>",
     "dump a core memory image <name> to file on exit"},
     
    {".",
     "GDB options"},
    {"-g, --gdbserver",
     "listen for GDB connection on TCP port defined by -p"},
    {"-G, --gdb-debug",
     "listen for GDB connection and write debug info"},
    {"--gdb-stdin",
     "for use with GDB as 'target remote | ./simulavr'"},
    {"-p  <port>",
     "use <port> for gdb server (default is port 1212)"},
    {"-n, --nogdbwait",
     "do not wait for gdb connection"},

    {".",
     "Control options"},
    {"-m  <nanoseconds>",
     "maximum run time of <nanoseconds>"},
    {"-W <offset_file>, --writetopipe <offset_file>",
     "add a special pipe register to device at IO-Offset and opens file for writing, write argument as 'offset,file', file can be '-' to write to standard output"},
    {"-R <offset_file>, --readfrompipe <offset_file>",
     "add a special pipe register to device at IO-Offset and opens file for reading, write argument as 'offset,file', file can be '-' to write to standard input"},
    {"-a <offset>, --writetoabort <offset>",
     "add a special register at IO-offset which aborts simulator run"},
    {"-e <offset>, --writetoexit <offset>",
     "add a special register at IO-offset which exits simulator run"},
    {"-T <label>, --terminate <label>",
     "stops simulation if PC runs on <label>, <label> is a text label or a address"},
    {"-B <label>, --breakpoint <label>",
     "same as -T for backward compatibility"},
    {"-M",
     "disable messages for bad I/O and memory references"},
    {"-l <number>, --linestotrace <number>",
     "maximum number of lines in each trace file. 0 means endless. Attention: if you use gdb & trace, please use always 0!"},

    {".",
     "VCD trace options"},
    {"-c <tracing-option>",
     "Enables a tracer with a set of options. The format for <tracing-option> is: <tracer>[:further-options ...]"},
    {"-o <trace-value-file>",
     "Specifies a file into which all available trace value names will be written. Use '-' for standard output"},

    {".",
     "TCL ui option"},
    {"-u",
     "run with user interface for external pin handling at port 7777"},
};

void doCommonUsageItem(const string &first, const string &second) {
   if(first[0] == '.')
       cout << endl << second << ":" << endl;
   else {
       cout << first << endl;
       cout << "\t" << second << endl;
   }
}

void doRSTUsageItem(const string &first, const string &second) {
   if(first[0] == '.') {
       string ul(second.size(), '-');
       cout << second << endl << ul << endl << endl;
   } else {
       cout << ".. option:: " << first << endl << endl;
       cout << "  " << second << endl << endl;
   }
}

void doUsage(void) {
    bool usage = getenv("SIMULAVR_DOC_RST") == NULL;
    
    if(usage) {
        cout << "AVR-Simulator Version " VERSION << endl << endl;

        cout << "simulavr {options}" << endl;
    }
    
    for(vector<string_pair_t>::iterator iter = newUsage.begin(); iter != newUsage.end(); ++iter) {
        if(usage)
            doCommonUsageItem(iter->first, iter->second);
        else
            doRSTUsageItem(iter->first, iter->second);
    }

    vector<string> avrlist = AvrFactory::supportedDevices();
    if(usage) {
        cout << endl << "Supported devices:" << endl;
        for(unsigned int i = 0; i < avrlist.size(); i++) {
            cout << avrlist[i] << endl;
        }
    } else {
        cout << "Supported devices" << endl << "-----------------" << endl << endl;
        cout << ".. hlist::" << endl << "   :columns: 5" << endl << endl;
        for(unsigned int i = 0; i < avrlist.size(); i++) {
            cout << "   * " << avrlist[i] << endl;
        }
    }
}

int main(int argc, char *argv[]) {
    int c;
    bool gdbserver_flag = 0;
    string coredumpfile("unknown");
    string filename("unknown");
    string devicename("unknown");
    string tracefilename("unknown");
    unsigned long global_gdbserver_port = 1212;
    int global_gdb_debug = 0;
    bool globalWaitForGdbConnection = true; //please wait for gdb connection
    int userinterface_flag = 0;
    unsigned long long fcpu = 4000000;
    unsigned long long maxRunTime = 0;
    unsigned long long linestotrace = 1000000;
    UserInterface *ui;
    
    unsigned long writeToPipeOffset = 0x20;
    unsigned long readFromPipeOffset = 0x21;
    unsigned long writeToAbort = 0;
    unsigned long writeToExit = 0;
    string readFromPipeFileName = "";
    string writeToPipeFileName = "";
    
    vector<string> terminationArgs;
    
    vector<string> tracer_opts;
    bool tracer_dump_avail = false;
    string tracer_avail_out;
    
    while (1) {
        //int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"file", 1, 0, 'f'},
            {"device", 1, 0, 'd'},
            {"gdbserver", 0, 0, 'g'},
            {"gdb-debug", 0, 0, 'G'},
            {"debug-gdb", 0, 0, 'G'},
            {"linestotrace", 1, 0, 'l'},
            {"maxruntime", 1, 0, 'm'},
            {"nogdbwait", 0, 0, 'n'},
            {"trace", 1, 0, 't'},
            {"version", 0, 0, 'V'},
            {"cpufrequency", 1, 0, 'F'},
            {"readfrompipe", 1, 0, 'R'},
            {"writetopipe", 1, 0, 'W'},
            {"writetoabort", 1, 0, 'a'},
            {"writetoexit", 1, 0, 'e'},
            {"verbose", 0, 0, 'v'},
            {"terminate", 1, 0, 'T'},
            {"breakpoint", 1, 0, 'B'},
            {"core-dump", 1, 0, 'C'},
            {"irqstatistic", 0, 0, 's'},
            {"help", 0, 0, 'h'},
            {0, 0, 0, 0}
        };
        
        c = getopt_long(argc, argv, "a:e:f:d:gGm:p:t:uxyzhvnisF:R:W:VT:B:c:C:o:l:", long_options, &option_index);
        if(c == -1)
            break;
        
        switch(c) {
            case 'B':
            case 'T':
                terminationArgs.push_back(optarg);
                break;
            
            case 'v':
                global_verbose_on = 1;
                break;
            
            case 'R': //read from pipe 
                readFromPipeFileName = 
                    SplitOffsetFile(optarg, "readFromPipe", 16, &readFromPipeOffset);
                break;
            
            case 'W': //write to pipe
                writeToPipeFileName = 
                   SplitOffsetFile(optarg, "writeToPipe", 16, &writeToPipeOffset);
                break;
            
            case 'a': // write to abort
                if(!StringToUnsignedLong(optarg, &writeToAbort, NULL, 16)) {
                    cerr << "writeToAbort is not a number" << endl;
                    exit(1);
                }
                break;
            
            case 'e': // write to exit
                if(!StringToUnsignedLong(optarg, &writeToExit, NULL, 16)) {
                    cerr << "writeToExit is not a number" << endl;
                    exit(1);
                }
                break;
            
            case 'F':
                if(!StringToUnsignedLongLong(optarg, &fcpu, NULL, 10)) {
                    cerr << "frequency is not a number" << endl;
                    exit(1);
                }
                if(fcpu == 0) {
                    cerr << "frequency is zero" << endl;
                    exit(1);
                }
                if(global_verbose_on)
                    printf("Running with CPU frequency: %1.4f MHz (%lld Hz)\n",
                           fcpu/1000000.0, fcpu);
                break;

            case 'l':
                if(!StringToUnsignedLongLong( optarg, &linestotrace, NULL, 10)) {
                    cerr << "linestotrace is not a number" << endl;
                    exit(1);
                }
                break;

            case 'm':
                if(!StringToUnsignedLongLong( optarg, &maxRunTime, NULL, 10)) {
                    cerr << "maxRunTime is not a number" << endl;
                    exit(1);
                }
                if(maxRunTime == 0) {
                    cerr << "maxRunTime is zero" << endl;
                    exit(1);
                }
                avr_message("Maximum Run Time: %lld", maxRunTime);
                break;
            
            case 'u':
                avr_message("Run with User Interface at Port 7777");
                userinterface_flag = 1;
                break;
            
            case 'f':
                avr_message("File to load: %s", optarg);
                filename = optarg;
                break;
            
            case 'd':
                avr_message("Device to simulate: %s", optarg);
                devicename = optarg;
                break;
            
            case 'g':
                avr_message("Running as gdb-server");
                gdbserver_flag = 1;
                break;
            
            case 'G':
                avr_message("Running with debug information from gdbserver");
                global_gdb_debug = 1;
                gdbserver_flag = 1;
                break;
            
            case 'p':
                if(!StringToUnsignedLong( optarg, &global_gdbserver_port, NULL, 10)) {
                    cerr << "GDB Server Port is not a number" << endl;
                    exit(1);
                }
                avr_message("Running on port: %ld", global_gdbserver_port);
                break;
            
            case 't':
                avr_message("Running in Trace Mode with maximum %lld lines per file",
                            linestotrace);

                sysConHandler.SetTraceFile(optarg, linestotrace);
                break;
            
            case 'V':
                cout << "SimulAVR " << VERSION << endl
                     << "See documentation for copyright and distribution terms" << endl
                     << endl;
                exit(0);
                break;
            
            case 'n':
                cout << "We will NOT wait for a gdb connection, "
                        "simulation starts now!" << endl;
                globalWaitForGdbConnection = false;
                break;
            
            case 'c':
                tracer_opts.push_back(optarg);
                break;
            
            case 'o':
                tracer_dump_avail = true;
                tracer_avail_out = optarg;
                break;
             
            case 's':
                enableIRQStatistic = true;
                break;
            
            case 'C':
                avr_message("Write core dump on exit to file: %s", optarg);
                coredumpfile = optarg;
                break;
            
            default:
                doUsage();
                exit(0);
        }
    }
    
    /* get dump manager and inform it, that we have a single device application */
    DumpManager *dman = DumpManager::Instance();
    dman->SetSingleDeviceApp();
    
    /* check, if devicename is given or get it out from elf file, if given */
    unsigned int sig;
    if(devicename == "unknown") {
        // option -d | --device not given
        if(filename != "unknown") {
            // filename given, try to get signature
            sig = ELFGetSignature(filename.c_str());
            if(sig != numeric_limits<unsigned int>::max()) {
                // signature in elf found, try to get devicename
                std::map<unsigned int, std::string>::iterator cur  = AvrSignatureToNameMap.find(sig);
                if(cur != AvrSignatureToNameMap.end()) {
                    // devicename found
                    devicename = cur->second;
                } else {
                    avr_warning("unknown signature in elf file '%s': 0x%x", filename.c_str(), sig);
                }
            }
        }
    }

    /* now we create the device and set device name and signature */
    AvrDevice *dev1 = AvrFactory::instance().makeDevice(devicename.c_str());
    std::map<std::string, unsigned int>::iterator cur  = AvrNameToSignatureMap.find(devicename);
    if(cur != AvrNameToSignatureMap.end()) {
        // signature found
        sig = cur->second;
    } else {
        avr_warning("signature for device '%s' not found", devicename.c_str());
        sig = -1;
    }
    dev1->SetDeviceNameAndSignature(devicename, sig);
    
    /* We had to wait with dumping the available tracing values
      until the device has been created! */
    if(tracer_dump_avail) {
        ShowRegisteredTraceValues(tracer_avail_out);
        exit(0);
    }
    
    /* handle DumpTrace option */
    SetDumpTraceArgs(tracer_opts, dev1);
    
    if(!gdbserver_flag && filename == "unknown") {
        cerr << "Specify either --file <executable> or --gdbserver (or --gdb-stdin)" << endl;
        exit(1);
    }
    
    //if we want to insert some special "pipe" Registers we could do this here:
    if(readFromPipeFileName != "") {
        avr_message("Add ReadFromPipe-Register at 0x%lx and read from file: %s",
                    readFromPipeOffset, readFromPipeFileName.c_str());
        dev1->ReplaceIoRegister(readFromPipeOffset,
            new RWReadFromFile(dev1, "FREAD", readFromPipeFileName.c_str()));
    }
    
    if(writeToPipeFileName != "") {
        avr_message("Add WriteToPipe-Register at 0x%lx and write to file: %s",
                    writeToPipeOffset, writeToPipeFileName.c_str());
        dev1->ReplaceIoRegister(writeToPipeOffset,
            new RWWriteToFile(dev1, "FWRITE", writeToPipeFileName.c_str()));
    }
    
    if(writeToAbort) {
        avr_message("Add WriteToAbort-Register at 0x%lx", writeToAbort);
        dev1->ReplaceIoRegister(writeToAbort, new RWAbort(dev1, "ABORT"));
    }
    
    if(writeToExit) {
        avr_message("Add WriteToExit-Register at 0x%lx", writeToExit);
        dev1->ReplaceIoRegister(writeToExit, new RWExit(dev1, "EXIT"));
    }
    
    if(filename != "unknown" ) {
        dev1->Load(filename.c_str());
        dev1->Reset(); // reset after load data from file to activate fuses and lockbits
    }
    
    //if we have a file we can check out for termination lines.
    vector<string>::iterator ii;
    for(ii = terminationArgs.begin(); ii != terminationArgs.end(); ii++) {
        avr_message("Termination or Breakpoint Symbol: %s", (*ii).c_str());
        dev1->RegisterTerminationSymbol((*ii).c_str());
    }
    
    //if not gdb, the ui will be master controller :-)
    ui = (userinterface_flag == 1) ? new UserInterface(7777) : NULL;
    
    dev1->SetClockFreq(1000000000 / fcpu); // time base is 1ns!
    
    if(sysConHandler.GetTraceState())
        dev1->trace_on = 1;
    
    dman->start(); // start dump session
    
    long steps = 0;
    if(gdbserver_flag == 0) { // no gdb
        SystemClock::Instance().Add(dev1);
        if(maxRunTime == 0) {
            steps = SystemClock::Instance().Endless();
            cout << "SystemClock::Endless stopped" << endl
                 << "number of cpu cycles simulated: " << dec << steps << endl;
        } else {                                           // limited
            steps = SystemClock::Instance().Run(maxRunTime);
            cout << "Ran too long.  Terminated after " << dec << maxRunTime
                 << " ns (simulated) and " << endl 
                 << dec << steps << " cpu cycles" << endl;
        }
        Application::GetInstance()->PrintResults();
    } else { // gdb should be activated
        avr_message("Waiting for gdb connection ...");
        GdbServer gdb1(dev1, global_gdbserver_port, global_gdb_debug, globalWaitForGdbConnection);
        SystemClock::Instance().Add(&gdb1);
        SystemClock::Instance().Endless();
        if(global_verbose_on) {
            cout << "SystemClock::Endless stopped" << endl
                 << "number of cpu cycles simulated: " << dec << steps << endl;
            Application::GetInstance()->PrintResults();
        }
    }
    
    dman->stopApplication(); // stop dump session. Close dump files, if necessary
    
    if(coredumpfile != "unknown") {
        avr_message("write core dump file ...");
        WriteCoreDump(coredumpfile, dev1);
    }

    // delete ui and device
    delete ui;
    delete dev1;
    
    return 0;
}

