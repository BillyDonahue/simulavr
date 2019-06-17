if(BUILD_VERILOG AND AVR_CC)

  message(STATUS "verilog test will be configured")
  
  add_avr_executable(NAME toggle.elf
                     MCU attiny2313
                     SOURCES toggle.c
                     CCOPTS -Os)
  
  set(verilog_result ${CMAKE_CURRENT_BINARY_DIR}/baretest.vcd)
  set(verilog_src ${CMAKE_CURRENT_SOURCE_DIR}/baretest.v)
  set(verilog_vvp ${CMAKE_CURRENT_BINARY_DIR}/baretest.vvp)
  set(verilog_timestamp ${CMAKE_CURRENT_BINARY_DIR}/timestamp)
                  
  add_custom_command(OUTPUT ${verilog_timestamp}
                     COMMAND ${IVERILOG} ${verilog_src} -s test -v -o ${verilog_vvp}
                     COMMAND ${VVP} -M $<TARGET_FILE_DIR:avr_vpi> -mavr ${verilog_vvp}
                     COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/regress/modules ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/verilog-test.py
                     COMMAND ${CMAKE_COMMAND} -E touch ${verilog_timestamp}
                     DEPENDS ${verilog_src} ${CMAKE_CURRENT_SOURCE_DIR}/verilog-test.py ${CMAKE_CURRENT_BINARY_DIR}/toggle.elf)

  add_custom_target(check-verilog COMMENT "run verilog test"
                                  DEPENDS ${verilog_result} ${verilog_timestamp})

endif(BUILD_VERILOG AND AVR_CC)
