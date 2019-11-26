.. _intro-simple-ex:

Simple example
==============

Lets look on a simple example to demonstrate the power of simulavr.

Example code
------------

Assume, that we have written a small program for a ATtiny2313 controller. (this
example code is taken from :file:`examples/simple_ex1`) Save it as simple.c:

.. code-block:: c

  /* This port corresponds to the "-W 0x20,-" command line option. */
  #define special_output_port (*((volatile char *)0x20))
  
  /* This port corresponds to the "-R 0x22,-" command line option. */
  #define special_input_port  (*((volatile char *)0x22))
  
  /* Poll the specified string out the debug port. */
  void debug_puts(const char *str) {
    const char *c;
  
    for(c = str; *c; c++)
      special_output_port = *c;
  }
  
  /* Main for test program.  Enter a string and echo it. */
  int main() {
    volatile char in_char;
  
    /* Output the prompt string */
    debug_puts("\nPress any key and enter:\n> ");
  
    /* Input one character but since line buffered, blocks until a CR. */
    in_char = special_input_port;
  
    /* Print the "what you entered:" message. */
    debug_puts("\nYou entered: ");
  
    /* now echo the rest of the characters */
    do {
      special_output_port = in_char;
    } while((in_char = special_input_port) != '\n');
  
    special_output_port = '\n';
    special_output_port = '\n';
  
    return 0;
  }

What does this code do:

.. code-block:: c

  #define special_output_port (*((volatile char *)0x20))
  #define special_input_port  (*((volatile char *)0x22))
  
This two preprocessor lines define 2 *virtual* port register, one for reading a
character, one for writing a character. Think about it as the data in/out
register of a UART unit. But instead to receive/send characters by transmission
line you get it from stdin/pipe or write it to stdout/pipe. This is a feature
of simulavr to have a simple possibility to debug your code
  
.. code-block:: c

  void debug_puts(const char *str) { ... }
  
This defines a function 'debug_puts', which gets a char string and puts it out
to our special "UART"

.. code-block:: c

  /* Input one character but since line buffered, blocks until a CR. */
  in_char = special_input_port;

In this line we wait for the first character from stdin/pipe ...

.. code-block:: c

  /* now echo the rest of the characters */
  do {
    special_output_port = in_char;
  } while((in_char = special_input_port) != '\n');

and then put the received character to stdout/pipe and receive the next
character until we receive a newline. After this we leave main. (not recommended
for production code!)

.. highlight:: none

Now we compile and link this code with avr-gcc::
  
  > avr-gcc -g -O2 -mmcu=attiny2313 -o simple.elf simple.c

Run the example
---------------

.. only:: manual

  We start simulation with::
  
    > simulavr -d attiny2313 -f simple.elf -W 0x20,- -R 0x22,- -T exit
  
    Press any key and enter:
    > abcdef
  
    You entered: abcdef

    >

.. only:: website

  Let's start the simulation inside the docker container, we have used for build. First
  we have to copy the avr program to the container and then start the container. (skip
  this steps if you want to use simular without docker on linux!)::

    > docker cp simple.elf simulavr-build:/root/simulavr/build/app
    > docker start -i simulavr-build

  Now we can start the simulation::

    cd /root/simulavr/build/app # only, if you run it inside docker container!
    ./simulavr -d attiny2313 -f simple.elf -W 0x20,- -R 0x22,- -T exit
  
    Press any key and enter:
    abcdef
  
    You entered: abcdef
   
What's happen:

* we start simulation for a ATtiny2313 with our program 'simple.elf'
* we create a write pipe to stdout at register 0x20
* we create a read pipe from stdin at register 0x22
* we end simulation, if exit label is arrived (exit label will automatically
  inserted by avr-gcc, this is the next address after calling main function and
  means, that we left main function)
* our input is "abcdef" followed by enter
* we got back "abcdef"

Run it with gdb
---------------

Now lets start a debug session.

.. only:: manual

  At first we have to start the simulation::
  
    > simulavr -d attiny2313 -f simple.elf -g
    Going to gdb...
    Waiting on port 1212 for gdb client to connect...

.. only:: website

  Assume, that docker container is already running (see above). Then we can
  start the simulation with gdb interface (option -g) If you run it from
  your linux machine, skip the first command or replace the path! ::
  
    cd /root/simulavr/build/app # only, if you run it inside docker container!
    ./simulavr -d attiny2313 -f simple.elf -g
    
It's quite similar to the call above. We tell simulavr, that we use ATtiny2313,
that our program is simple.elf and - that's new - that we start a gdb session. As
you can see, simulavr opens port 1212 and wait for connection from gdb.

.. only:: manual

  Now we have to open a new shell and start avr-gdb::
  
    > avr-gdb
    GNU gdb 6.4
    Copyright 2005 Free Software Foundation, Inc.
    GDB is free software, covered by the GNU General Public License, and you are
    welcome to change it and/or distribute copies of it under certain conditions.
    Type "show copying" to see the conditions.
    There is absolutely no warranty for GDB.  Type "show warranty" for details.
    This GDB was configured as "--host=i486-linux-gnu --target=avr".
    (gdb)
  
  *(gdb)* is the input prompt and avr-gdb waits now for commands::
  
    (gdb) file simple.elf
    Reading symbols from /home/.../simple.elf...done.
    (gdb) target remote localhost:1212
    Remote debugging using localhost:1212
    0x00000000 in __vectors ()
    (gdb) load
    Loading section .text, size 0xba lma 0x0
    Loading section .data, size 0x58 lma 0xba
    Start address 0x0, load size 274
    Transfer rate: 2192 bits in <1 sec, 137 bytes/write.
    (gdb) step
    Single stepping until exit from function __vectors, 
    which has no line number information.
    0x0000001a in __trampolines_start ()
    (gdb) quit
    The program is running.  Exit anyway? (y or n) y
    >

.. only:: website

  Before we can start with gdb, we have to find out, what's the IP address from our
  docker container::
  
    > docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' simulavr-build
    172.17.0.2
  
  Here we can see, that the IP address is *172.17.0.2* (could be different in your case!)
  
  Now we have to open a new shell (on the host!) and start avr-gdb::
  
    > avr-gdb
    GNU gdb 6.4
    Copyright 2005 Free Software Foundation, Inc.
    GDB is free software, covered by the GNU General Public License, and you are
    welcome to change it and/or distribute copies of it under certain conditions.
    Type "show copying" to see the conditions.
    There is absolutely no warranty for GDB.  Type "show warranty" for details.
    This GDB was configured as "--host=i486-linux-gnu --target=avr".
    (gdb)
  
  *(gdb)* is the input prompt and avr-gdb waits now for commands (watch the IP address
  in target remote command, replace it with that, you've found out for your environment)::
  
    (gdb) file simple.elf
    Reading symbols from /home/.../simple.elf...done.
    (gdb) target remote 172.17.0.2:1212
    Remote debugging using 172.17.0.2:1212
    0x00000000 in __vectors ()
    (gdb) load
    Loading section .text, size 0xba lma 0x0
    Loading section .data, size 0x58 lma 0xba
    Start address 0x0, load size 274
    Transfer rate: 2192 bits in <1 sec, 137 bytes/write.
    (gdb) step
    Single stepping until exit from function __vectors, 
    which has no line number information.
    0x0000001a in __trampolines_start ()
    (gdb) quit
    The program is running.  Exit anyway? (y or n) y
    >
  
`file simple.elf`
  load our program into debugger

.. only:: manual

  `target remote localhost:1212`
    now we connect us to simulavr, in shell with simulavr we can see now, that
    simulavr has connection to gdb: `Connection opened by host 0.0.0.0, port 1212.`

.. only:: website

  `target remote 172.17.0.2:1212`
    now we connect us to simulavr, in shell with simulavr we can see now, that
    simulavr has connection to gdb: `Connection opened by host 0.0.0.0, port 1212.`

`load`
  now we load our program to simulavr
  
`step`
  we make here a single step, but now you're able to debug your code as you like
  
`quit`
  for now we close our debug session
  
After closing our debug session we have to stop simulavr by typing :kbd:`^C` in this
shell with simulavr running. Otherwise simulavr waits for a next gdb session.
