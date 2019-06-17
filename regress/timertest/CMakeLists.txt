if(PYTHONINTERP_FOUND AND AVR_CC AND BUILD_PYTHON)

  #message(STATUS "timertest will be configured")
  
  configure_elf_tests(NAME timer_16_extp
                      SOURCES timer_16bit_ext.c
                      CCOPTS -g -O2
                      MCUS atmega128
                      OUTVAR results)
  configure_elf_tests(NAME timer_16_extn
                      SOURCES timer_16bit_ext.c
                      CCOPTS -g -O2 -DNEGEDGE_T1=1
                      MCUS atmega128
                      INVAR ${results} OUTVAR results)
  configure_elf_tests(NAME timer_16_icap_p
                      SOURCES timer_16bit_icap.c
                      CCOPTS -g -O2 -DPOS_EDGE_NO_C=1
                      MCUS atmega128
                      INVAR ${results} OUTVAR results)
  configure_elf_tests(NAME timer_16_icap_n
                      SOURCES timer_16bit_icap.c
                      CCOPTS -g -O2 -DNEG_EDGE_NO_C=1
                      MCUS atmega128
                      INVAR ${results} OUTVAR results)
  configure_elf_tests(NAME timer_16_icap_pc
                      SOURCES timer_16bit_icap.c
                      CCOPTS -g -O2 -DPOS_EDGE_WITH_C=1
                      MCUS atmega128
                      INVAR ${results} OUTVAR results)
  configure_elf_tests(NAME timer_16_icap_nc
                      SOURCES timer_16bit_icap.c
                      CCOPTS -g -O2 -DNEG_EDGE_WITH_C=1
                      MCUS atmega128
                      INVAR ${results} OUTVAR results)
  configure_elf_tests(NAME timer_tX5_8bit_normal
                      SOURCES timer_tX5_8bit.c
                      CCOPTS -g -O2
                      MCUS attiny25
                      INVAR ${results} OUTVAR results)
  configure_elf_tests(NAME timer_tX5_8bit_ctc
                      SOURCES timer_tX5_8bit.c
                      CCOPTS -g -O2 -DTMODE_CTC=1
                      MCUS attiny25
                      INVAR ${results} OUTVAR results)
  configure_elf_tests(NAME timer_tX5_8bit_pwm
                      SOURCES timer_tX5_8bit.c
                      CCOPTS -g -O2 -DTMODE_PWM=1
                      MCUS attiny25
                      INVAR ${results} OUTVAR results)
                      
  configure_vcd_tests(NAME timer_8bit_normal
                      SOURCES timer_8bit.c
                      CCOPTS -g -O2
                      SIMTIME 5000000
                      SIGNALFILE timer_8bit_normal.sig
                      MCUS atmega128 atmega48
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_vcd_tests(NAME timer_8bit_normal
                      SOURCES timer_8bit.c
                      CCOPTS -g -O2
                      SIMTIME 5000000
                      SIGNALFILE timer_8bit_normal_90.sig
                      MCUS at90s4433 at90s8515 attiny2313 attiny25
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_vcd_tests(NAME timer_16bit_normal
                      SOURCES timer_16bit.c
                      CCOPTS -g -O2
                      SIMTIME 40000000
                      SIGNALFILE timer_16bit_normal.sig
                      MCUS atmega128 atmega48
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_vcd_tests(NAME timer_16bit_normal
                      SOURCES timer_16bit.c
                      CCOPTS -g -O2
                      SIMTIME 40000000
                      SIGNALFILE timer_16bit_normal_90.sig
                      MCUS at90s4433 at90s8515
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_vcd_tests(NAME timer_8bit_ctc
                      SOURCES timer_8bit_ctc.c
                      CCOPTS -g -O2
                      SIMTIME 8000000
                      SIGNALFILE timer_8bit_ctc.sig
                      MCUS atmega128 atmega48
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)
  configure_vcd_tests(NAME timer_16bit_fastpwm
                      SOURCES timer_16bit.c
                      CCOPTS -g -O2 -DT3TEST=1
                      SIMTIME 4000000
                      SIGNALFILE timer_16bit_fastpwm.sig
                      MCUS atmega128
                      INVAR ${results} OUTVAR results
                      PROCDEFINE)

  set(pypath_l ${CMAKE_SOURCE_DIR}/regress/timertest
               ${CMAKE_SOURCE_DIR}/regress/modules
               ${CMAKE_BINARY_DIR}/pysimulavr)
  string(REPLACE ";" ":" pypath "${pypath_l}")

  add_custom_target(check-timertest PYTHONPATH=${pypath} ${PYTHON_EXECUTABLE} -m regress_unittest ${results}
                                    COMMENT "run tests for timer units"
                                    DEPENDS pysimulavr ${results})

endif(PYTHONINTERP_FOUND AND AVR_CC AND BUILD_PYTHON)