# additional cmake functions

# build avr elf file from sources
####
# example:
# add_avr_executable(NAME "${opts_NAME}_${mcu}.elf"
#                    MCU ${mcu}
#                    SOURCES ${opts_SOURCES}
#                    CCOPTS ${opts_CCOPTS})
#
# NAME executable name, maybe with suffix ".elf"
# MCU the mcu for which the executable is to build
# SOURCES c sources to build executable
# CCOPTS compiler options, but without option -mmcu!
#
function(add_avr_executable)

  set(opts1 NAME MCU)
  set(optsN SOURCES CCOPTS)
  cmake_parse_arguments(opts "" "${opts1}" "${optsN}" ${ARGN})

  set(executable ${CMAKE_CURRENT_BINARY_DIR}/${opts_NAME})

  foreach(_s IN ITEMS ${opts_SOURCES})
    list(APPEND sources "${CMAKE_CURRENT_SOURCE_DIR}/${_s}")
  endforeach(_s)

  set(opts ${opts_CCOPTS} -mmcu=${opts_MCU})
  
  add_custom_command(OUTPUT ${executable}
                     COMMAND ${AVR_CC} ${opts} -o ${executable} ${sources}
                     DEPENDS ${sources})

endfunction(add_avr_executable)

# build avr elf files from sources for different mcu's
####
# example:
# configure_elf_tests(NAME ext_int1
#                     SOURCES ext_int0.c
#                     CCOPTS -g -O2 -DTEST_INT1=1
#                     MCUS atmega128 atmega48 atmega16 attiny2313 at90s4433 at90s8515
#                     INVAR ${results} OUTVAR results)
#
# NAME executable base name, will be extended with mcu and suffix ".elf"
# SOURCES c sources to build executable
# CCOPTS compiler options, but without option -mmcu!
# MCUS the mcu names to build executables for
# INVAR (optional) executable list from other function calls
# OUTVAR returns the list of built executables
# PROCDEFINE if set, add -DPROC_<mcu>=1 to CCOPTS
#
function(configure_elf_tests)

  set(optsB PROCDEFINE)
  set(opts1 NAME OUTVAR)
  set(optsN SOURCES MCUS CCOPTS INVAR)
  cmake_parse_arguments(opts "${optsB}" "${opts1}" "${optsN}" ${ARGN})

  set(outputs ${opts_INVAR})
  
  foreach(mcu IN ITEMS ${opts_MCUS})
  
    set(executable ${CMAKE_CURRENT_BINARY_DIR}/${opts_NAME}_${mcu}.elf)
    
    if(${opts_PROCDEFINE})
      set(procdefine -DPROC_${mcu}=1)
    endif(${opts_PROCDEFINE})
      
    add_avr_executable(NAME "${opts_NAME}_${mcu}.elf"
                       MCU ${mcu}
                       SOURCES ${opts_SOURCES}
                       CCOPTS ${opts_CCOPTS} ${procdefine})
    list(APPEND outputs ${executable})

  endforeach(mcu)

  set("${opts_OUTVAR}" ${outputs} PARENT_SCOPE)
    
endfunction(configure_elf_tests)

# build avr elf files from sources for different mcu's and run simulation with vcd output
####
# example:
# configure_vcd_tests(NAME ext_int1
#                     SOURCES ext_int0.c
#                     CCOPTS -g -O2 -DTEST_INT1=1
#                     SIMTIME 5000000
#                     SIGNALFILE sample.sig
#                     MCUS atmega128 atmega48 atmega16 attiny2313 at90s4433 at90s8515
#                     INVAR ${results} OUTVAR results)
#
# NAME executable base name, will be extended with mcu and suffix ".elf"
# SOURCES c sources to build executable
# CCOPTS compiler options, but without option -mmcu!
# SIMTIME simulation time in ns
# SIGNALFILE signals file for vcd output
# MCUS the mcu names to build executables for
# INVAR (optional) executable list from other function calls
# OUTVAR returns the list of built executables
# PROCDEFINE if set, add -DPROC_<mcu>=1 to CCOPTS
#
function(configure_vcd_tests)

  set(optsB PROCDEFINE)
  set(opts1 NAME OUTVAR SIMTIME SIGNALFILE)
  set(optsN SOURCES MCUS CCOPTS INVAR)
  cmake_parse_arguments(opts "${optsB}" "${opts1}" "${optsN}" ${ARGN})

  set(outputs ${opts_INVAR})
  
  foreach(mcu IN ITEMS ${opts_MCUS})
  
    set(executable ${CMAKE_CURRENT_BINARY_DIR}/${opts_NAME}_${mcu}.elf)
    set(signals ${CMAKE_CURRENT_SOURCE_DIR}/${opts_SIGNALFILE})
    set(vcd ${CMAKE_CURRENT_BINARY_DIR}/${opts_NAME}_${mcu}.vcd)

    if(${opts_PROCDEFINE})
      set(procdefine -DPROC_${mcu}=1)
    endif(${opts_PROCDEFINE})
      
    add_avr_executable(NAME "${opts_NAME}_${mcu}.elf"
                       MCU ${mcu}
                       SOURCES ${opts_SOURCES}
                       CCOPTS ${opts_CCOPTS} ${procdefine})

    add_custom_command(OUTPUT ${vcd}
                       COMMAND $<TARGET_FILE:simulavr> -d${mcu} -m ${opts_SIMTIME}
                                                       -c vcd:${signals}:${vcd}
                                                       -f ${executable}
                       DEPENDS ${executable})

    list(APPEND outputs ${vcd})

  endforeach(mcu)

  set("${opts_OUTVAR}" ${outputs} PARENT_SCOPE)
    
endfunction(configure_vcd_tests)

# EOF