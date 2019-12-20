if(AVR_CC)

  message(STATUS "gtest will be configured")

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/session_001)
  add_avr_executable(NAME "session_001/avr_code.atmega32.o"
                     MCU "atmega32"
                     SOURCES "session_001/avr_code.s"
                     CCOPTS -Wa,--gstabs,-D -xassembler-with-cpp)

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check)
  add_avr_executable(NAME "session_irq_check/check.atmega32.o"
                     MCU "atmega32"
                     SOURCES "session_irq_check/check.s"
                     CCOPTS -Wa,--gstabs,-D -xassembler-with-cpp)
  add_avr_executable(NAME "session_irq_check/tc1.atmega32.o"
                     MCU "atmega32"
                     SOURCES "session_irq_check/tc1.s"
                     CCOPTS -Wa,--gstabs,-D -xassembler-with-cpp)
  add_avr_executable(NAME "session_irq_check/tc2.atmega32.o"
                     MCU "atmega32"
                     SOURCES "session_irq_check/tc2.s"
                     CCOPTS -Wa,--gstabs,-D -xassembler-with-cpp)
  add_avr_executable(NAME "session_irq_check/tc3.atmega32.o"
                     MCU "atmega32"
                     SOURCES "session_irq_check/tc3.s"
                     CCOPTS -Wa,--gstabs,-D -xassembler-with-cpp)
  add_avr_executable(NAME "session_irq_check/tc4.atmega32.o"
                     MCU "atmega32"
                     SOURCES "session_irq_check/tc4.s"
                     CCOPTS -Wa,--gstabs,-D -xassembler-with-cpp)
  add_avr_executable(NAME "session_irq_check/tc5.atmega32.o"
                     MCU "atmega32"
                     SOURCES "session_irq_check/tc5.cpp"
                     CCOPTS -O2)
                     
  function(make_session_io_bin)
    set(opts1 MCU OUTVAR)
    set(optsN INVAR)
    cmake_parse_arguments(opts "" "${opts1}" "${optsN}" ${ARGN})

    set(outputs ${opts_INVAR})
    set(obj tc1.${opts_MCU}.o)
    set(bin ${CMAKE_CURRENT_BINARY_DIR}/session_io_pin/${obj})
    add_avr_executable(NAME "session_io_pin/${obj}"
                       MCU ${opts_MCU}
                       SOURCES "session_io_pin/tc1.s"
                       CCOPTS -Wa,--gstabs,-D -xassembler-with-cpp)
    list(APPEND outputs ${bin})
    
    set("${opts_OUTVAR}" ${outputs} PARENT_SCOPE)
    
  endfunction(make_session_io_bin)
  
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/session_io_pin)
  set(session_io_elf "")
  make_session_io_bin(MCU atmega128 INVAR ${session_io_elf} OUTVAR session_io_elf)
  make_session_io_bin(MCU atmega16 INVAR ${session_io_elf} OUTVAR session_io_elf)
  make_session_io_bin(MCU atmega8 INVAR ${session_io_elf} OUTVAR session_io_elf)
  make_session_io_bin(MCU atmega164a INVAR ${session_io_elf} OUTVAR session_io_elf)
  make_session_io_bin(MCU atmega48 INVAR ${session_io_elf} OUTVAR session_io_elf)
  make_session_io_bin(MCU atmega640 INVAR ${session_io_elf} OUTVAR session_io_elf)
  make_session_io_bin(MCU attiny2313 INVAR ${session_io_elf} OUTVAR session_io_elf)
  make_session_io_bin(MCU attiny25 INVAR ${session_io_elf} OUTVAR session_io_elf)
    
  function(make_session_trace)
    set(opts1 MCU OUTVAR)
    set(optsN INVAR)
    cmake_parse_arguments(opts "" "${opts1}" "${optsN}" ${ARGN})

    set(outputs ${opts_INVAR})
    set(obj tc1.${opts_MCU}.o)
    set(bin ${CMAKE_CURRENT_BINARY_DIR}/session_trace/${obj})
    add_avr_executable(NAME "session_trace/${obj}"
                       MCU ${opts_MCU}
                       SOURCES "session_trace/tc1.s"
                       CCOPTS -Wa,--gstabs,-D -xassembler-with-cpp)
    list(APPEND outputs ${bin})
    
    set("${opts_OUTVAR}" ${outputs} PARENT_SCOPE)
    
  endfunction(make_session_trace)
  
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/session_trace)
  set(session_trace_elf "")
  make_session_trace(MCU atmega128 INVAR ${session_trace_elf} OUTVAR session_trace_elf)
  make_session_trace(MCU atmega16 INVAR ${session_trace_elf} OUTVAR session_trace_elf)
  make_session_trace(MCU atmega8 INVAR ${session_trace_elf} OUTVAR session_trace_elf)
  make_session_trace(MCU atmega164a INVAR ${session_trace_elf} OUTVAR session_trace_elf)
  make_session_trace(MCU atmega48 INVAR ${session_trace_elf} OUTVAR session_trace_elf)
  make_session_trace(MCU atmega640 INVAR ${session_trace_elf} OUTVAR session_trace_elf)
  make_session_trace(MCU attiny2313 INVAR ${session_trace_elf} OUTVAR session_trace_elf)
  make_session_trace(MCU attiny25 INVAR ${session_trace_elf} OUTVAR session_trace_elf)

  set(dut_avr ${CMAKE_CURRENT_BINARY_DIR}/session_001/avr_code.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/tc1.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/tc2.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/tc3.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/tc4.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/tc5.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/check.atmega32.o
              ${session_io_elf} ${session_trace_elf})
  
  set(dut_sources gtest-1.10.0/src/gtest-all.cc
                  session_001/unittest001.cpp
                  session_irq_check/unittest_irq.cpp
                  session_io_pin/unittest_io_pin.cpp
                  session_trace/unittest_trace.cpp
                  gtest_main.cpp)
                  
  include_directories(${CMAKE_SOURCE_DIR}/include gtest-1.10.0/include gtest-1.10.0 gtest-1.10.0/include/gtest)

  add_executable(dut ${dut_sources})
  target_link_libraries(dut sim pthread)
  #target_compile_options(dut PRIVATE -Wall -Wextra)
  target_compile_options(dut PRIVATE -Wall)

  set(gtest_timestamp ${CMAKE_CURRENT_BINARY_DIR}/timestamp)

  if(CHECK_VALGRIND AND VALGRIND)
  
    add_custom_command(OUTPUT ${gtest_timestamp}
                       COMMAND ${VALGRIND} --leak-check=full --error-exitcode=42 ${CMAKE_CURRENT_BINARY_DIR}/dut
                       COMMAND ${CMAKE_COMMAND} -E touch ${gtest_timestamp}
                       DEPENDS dut ${dut_avr})

  else()
  
    add_custom_command(OUTPUT ${gtest_timestamp}
                       COMMAND ${CMAKE_CURRENT_BINARY_DIR}/dut
                       COMMAND ${CMAKE_COMMAND} -E touch ${gtest_timestamp}
                       DEPENDS dut ${dut_avr})

  endif()
                                  
  add_custom_target(check-gtest COMMENT "gtest result:"
                                DEPENDS ${gtest_timestamp})
  add_dependencies(check check-gtest)

endif(AVR_CC)