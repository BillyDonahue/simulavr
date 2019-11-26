Introduction
============

The **SimulAVR** program is a simulator for the Atmel AVR family of
microcontrollers. Atmel was taken over by Microchip in the year 2016.

SimulAVR can be used either standalone or as a remote target for *avr-gdb*.
When used in gdbserver mode, the simulator is used as a back-end so that
*avr-gdb* can be used as a source level debugger for AVR programs.

SimulAVR started out as a C based project written by Theodore Roth. The hardware
simulation part has since been completely re-written in C++. Only the
instruction decoder and the *avr-gdb* interface are mostly copied from the 
original simulavr sources. This C++ based version was known as simulavrxx until
it became feature compatibile with the old simulavr code, then it renamed back
to simulavr.

The core of SimulAVR is functionally a library. This library is linked together
with a command-line interface to create a command-line program. It is also
linked together with interpreter interfaces to create libraries that can
be used by a interpreter language (currently Python / TCL). In the examples
directory there are examples of simulations with a graphical environment (with
the Tcl/Tk interface) or how to write for example unit tests by using Python
interface. The graphic components in Tcl/Tk examples do not show any
hardware / registers of the simulated CPU. It shows only external components
attached to the IO-pins of the simulated CPU.
