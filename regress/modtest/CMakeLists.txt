if(PYTHONINTERP_FOUND AND AVR_CC AND BUILD_PYTHON)

  #message(STATUS "modtest will be configured")
  
  set(results "pin.py;portio.py")
  
  configure_elf_tests(NAME port
                      SOURCES port.c
                      CCOPTS -g -O2
                      MCUS at90s4433 at90s8515 at90can32 attiny25 atmega644 atmega16 atmega128 atmega48 attiny2313 atmega8
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  #configure_elf_tests(NAME eeprom
  #                    SOURCES eeprom.c
  #                    CCOPTS -g -O2
  #                    MCUS at90s4433 at90s8515 at90can32 attiny25 atmega644 atmega16 atmega128 atmega48 attiny2313 atmega8
  #                    INVAR ${results} OUTVAR results
  #                    PROCDEFINE)
  configure_elf_tests(NAME eeprom_int
                      SOURCES eeprom_int.c
                      CCOPTS -g -O2
                      MCUS at90s4433 at90can32 attiny25 atmega644 atmega16 atmega128 atmega48 attiny2313 atmega8
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_elf_tests(NAME anacomp
                      SOURCES anacomp.c
                      CCOPTS -g -O2
                      MCUS at90s4433 at90s8515 at90can32 attiny25 atmega644 atmega16 atmega128 atmega48 attiny2313 atmega8
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_elf_tests(NAME anacomp_int
                      SOURCES anacomp_int.c
                      CCOPTS -g -O2
                      MCUS at90s4433 at90s8515 at90can32 attiny25 atmega644 atmega16 atmega128 atmega48 attiny2313 atmega8
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_elf_tests(NAME anacomp_mux
                      SOURCES anacomp_mux.c
                      CCOPTS -g -O2
                      MCUS at90can32 attiny25 atmega644 atmega16 atmega128 atmega48 atmega8
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_elf_tests(NAME adc
                      SOURCES adc.c
                      CCOPTS -g -O2
                      MCUS at90s4433 atmega8 attiny25 at90can32 atmega644 atmega16 atmega128 atmega48
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_elf_tests(NAME adc_int
                      SOURCES adc_int.c
                      CCOPTS -g -O2
                      MCUS at90s4433 atmega8 attiny25 at90can32 atmega644 atmega16 atmega128 atmega48
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_elf_tests(NAME adc_fr
                      SOURCES adc_fr.c
                      CCOPTS -g -O2
                      MCUS at90s4433 atmega8 attiny25 at90can32 atmega644 atmega16 atmega128 atmega48
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_elf_tests(NAME adc_diff
                      SOURCES adc_diff.c
                      CCOPTS -g -O2
                      MCUS atmega16 atmega644 at90can32 atmega128
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_elf_tests(NAME adc_gain
                      SOURCES adc_diff.c
                      CCOPTS -g -O2 -DADC_GAIN200=1
                      MCUS atmega16 atmega644 at90can32 atmega128
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  #configure_elf_tests(NAME adc_diff_t25
  #                    SOURCES adc_diff_t25.c
  #                    CCOPTS -g -O2
  #                    MCUS attiny25
  #                    INVAR ${results} OUTVAR results
  #                    PROCDEFINE)

  set(pypath_l ${CMAKE_SOURCE_DIR}/regress/modtest
               ${CMAKE_SOURCE_DIR}/regress/modules
               ${CMAKE_BINARY_DIR}/pysimulavr)
  string(REPLACE ";" ":" pypath "${pypath_l}")
  add_custom_target(check-modtest PYTHONPATH=${pypath} ${PYTHON_EXECUTABLE} -m regress_unittest ${results}
                                  COMMENT "run tests for peripheral units"
                                  DEPENDS pysimulavr ${results})

endif(PYTHONINTERP_FOUND AND AVR_CC AND BUILD_PYTHON)