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
                     
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/session_io_pin)
  add_avr_executable(NAME "session_io_pin/tc1.atmega128.o"
                     MCU "atmega128"
                     SOURCES "session_io_pin/tc1.s"
                     CCOPTS -Wa,--gstabs,-D -xassembler-with-cpp)

  set(dut_avr ${CMAKE_CURRENT_BINARY_DIR}/session_001/avr_code.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/tc1.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/tc2.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/tc3.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/tc4.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/tc5.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_irq_check/check.atmega32.o
              ${CMAKE_CURRENT_BINARY_DIR}/session_io_pin/tc1.atmega128.o)
  
  set(dut_sources gtest-1.6.0/src/gtest-all.cc
                  session_001/unittest001.cpp
                  session_irq_check/unittest_irq.cpp
                  session_io_pin/unittest_io_pin.cpp
                  gtest_main.cpp)
                  
  include_directories(${CMAKE_SOURCE_DIR}/include gtest-1.6.0/include gtest-1.6.0 gtest-1.6.0/include/gtest)

  add_executable(dut ${dut_sources})
  set_target_properties(dut PROPERTIES COMPILE_FLAGS "-Dprivate=public -Dprotected=public")
  target_link_libraries(dut sim pthread)

  set(gtest_timestamp ${CMAKE_CURRENT_BINARY_DIR}/timestamp)

  add_custom_command(OUTPUT ${gtest_timestamp}
                     COMMAND ${CMAKE_CURRENT_BINARY_DIR}/dut
                     COMMAND ${CMAKE_COMMAND} -E touch ${gtest_timestamp}
                     DEPENDS dut ${dut_avr})
                                        
  add_custom_target(check-gtest COMMENT "gtest result:"
                                DEPENDS ${gtest_timestamp})
  
endif(AVR_CC)