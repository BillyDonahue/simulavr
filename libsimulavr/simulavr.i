%module simulavr

%include std_string.i

%{
#include "config.h"
#include "systemclocktypes.h"
#include "traceval.h"
#include "avrdevice.h"
#include "avrfactory.h"
#include "at8515.h"
#include "atmega128.h"
#include "at4433.h"
#include "systemclock.h"
#include "ui/ui.h"
#include "hardware.h"
#include "pin.h"
#include "pinatport.h"
#include "ui/extpin.h"
#include "net.h"
#include "gdb/gdb.h"
#include "ui/keyboard.h"
#include "ui/lcd.h"
#include "ui/serialrx.h"
#include "ui/serialtx.h"
#include "spisrc.h"
#include "spisink.h"
#include "adcpin.h"
#include "pinmon.h"
#include "rwmem.h"
#include "specialmem.h"
#include "ui/scope.h"
#include "avrerror.h"
#include "hwport.h"
#include "hwtimer/timerprescaler.h"
#include "hwtimer/prescalermux.h"
#include "hwtimer/hwtimer.h"
#include "hwtimer/icapturesrc.h"

SystemClock &GetSystemClock() { return SystemClock::Instance(); }
%}

%include "std_vector.i"

namespace std {
   %template(DWordVector) vector<dword>;
};

%immutable HWStack::m_ThreadList;

%include "systemclocktypes.h"
%include "simulationmember.h"
%include "externaltype.h"
%include "ui/mysocket.h"
%include "pinnotify.h"
%include "traceval.h"
%include "avrdevice.h"
%include "avrfactory.h"
%include "at8515.h"
%include "atmega128.h"
%include "at4433.h"
%include "systemclock.h"
%include "ui/ui.h"
%include "hardware.h"
%include "pin.h"
%extend Pin {
    void SetOutState(int s) {
        $self->outState = (Pin::T_Pinstate)s;
    }
}

%include "pinatport.h"
%extend PinAtPort {
  bool GetPinInput(void) {
    return (bool)*$self;
  }
}

%include "config.h"
%include "ui/extpin.h"
%include "net.h"
%include "gdb/gdb.h"
%include "ui/keyboard.h"
%include "ui/lcd.h"
%include "ui/serialrx.h"
%include "ui/serialtx.h"
%include "spisrc.h"
%include "spisink.h"
%include "adcpin.h"
%include "pinmon.h"
%include "rwmem.h"
%include "specialmem.h"
%include "ui/scope.h"
%include "avrerror.h"
%include "hwport.h"
%include "hwtimer/timerprescaler.h"
%include "hwtimer/prescalermux.h"
%include "hwtimer/hwtimer.h"
%include "hwtimer/icapturesrc.h"

SystemClock &GetSystemClock();
