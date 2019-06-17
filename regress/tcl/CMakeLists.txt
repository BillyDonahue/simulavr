if(BUILD_TCL AND AVR_CC)

  #message(STATUS "tcl test will be configured")

  set(tcl_test_wrapper ${CMAKE_CURRENT_BINARY_DIR}/test-wrapper.sh)

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test-wrapper.sh.in ${tcl_test_wrapper})

  add_avr_executable(NAME "main.elf"
                     MCU "atmega16"
                     SOURCES "main.c"
                     CCOPTS -Os)

  add_custom_target(check-tcltest ${tcl_test_wrapper} ${CMAKE_BINARY_DIR}/libsimulavr/libsimulavr.so
                                                      ${CMAKE_CURRENT_SOURCE_DIR}/tcl-test.tcl
                                  COMMENT "tcl test result:"
                                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/main.elf simtcl)

endif(BUILD_TCL AND AVR_CC)