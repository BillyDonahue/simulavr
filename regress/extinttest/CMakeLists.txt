if(PYTHONINTERP_FOUND AND AVR_CC AND BUILD_PYTHON)

  #message(STATUS "exinttest will be configured")
  
  configure_elf_tests(NAME ext_int0
                      SOURCES ext_int0.c
                      CCOPTS -g -O2
                      MCUS atmega128 atmega48 atmega16 attiny2313 at90s4433 at90s8515
                      OUTVAR results)
  configure_elf_tests(NAME ext_int1
                      SOURCES ext_int0.c
                      CCOPTS -g -O2 -DTEST_INT1=1
                      MCUS atmega128 atmega48 atmega16 attiny2313 at90s4433 at90s8515
                      INVAR ${results} OUTVAR results)
  configure_elf_tests(NAME ext_int2
                      SOURCES ext_int0.c
                      CCOPTS -g -O2 -DTEST_INT2_16=1
                      MCUS atmega16
                      INVAR ${results} OUTVAR results)
  configure_elf_tests(NAME ext_pcint
                      SOURCES ext_pcint.c
                      CCOPTS -g -O2
                      MCUS attiny2313 atmega48
                      INVAR ${results} OUTVAR results)

  set(pypath_l ${CMAKE_SOURCE_DIR}/regress/extinttest
               ${CMAKE_SOURCE_DIR}/regress/modules
               ${CMAKE_BINARY_DIR}/pysimulavr)
  string(REPLACE ";" ":" pypath "${pypath_l}")
  add_custom_target(check-extinttest PYTHONPATH=${pypath} ${PYTHON_EXECUTABLE} -m regress_unittest ${results}
                                     COMMENT "run tests for external interrupts"
                                     DEPENDS pysimulavr ${results})

endif(PYTHONINTERP_FOUND AND AVR_CC AND BUILD_PYTHON)