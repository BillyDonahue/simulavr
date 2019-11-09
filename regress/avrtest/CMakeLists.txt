if(AVR_CC)

  # list of tested mcu's
  set(MCU_LIST atmega128 at90s8515 at90s4433 atmega48 at90can64 atmega32 atmega8 attiny2313 attiny25)

  # avr toolchain
  find_program(GNUTIME time)

  # test maxruntime
  set(CHECK_NAME maxruntime)
  set(AVR_SRC test_maxruntime.c avrtest_help.c)

  set(AVR_SIMOPTS -W 0x52,- -m 1000000)
  set(AVR_SHELLOPTS 0,0)

  foreach(AVR_MCU IN ITEMS ${MCU_LIST})

    list(APPEND reports ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}.report)

    add_avr_executable(NAME ${CHECK_NAME}_${AVR_MCU}
                       MCU ${AVR_MCU}
                       SOURCES ${AVR_SRC}
                       CCOPTS -g -O2)
  
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}.report
                       COMMAND GNUTIME=${GNUTIME} ${CMAKE_CURRENT_SOURCE_DIR}/run_test.sh
                                 ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}.report
                                 ${AVR_SHELLOPTS}
                                 ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}
                                 --
                                 $<TARGET_FILE:simulavr> ${AVR_SIMOPTS}
                                                         -d ${AVR_MCU}
                                                         -f ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}
                       DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU})

  endforeach(AVR_MCU)

  # test abort
  set(CHECK_NAME abort)
  set(AVR_SRC test_abort.c avrtest_help.c)

  set(AVR_SIMOPTS -W 0x52,- -a 0x49 -m 10000000)
  set(AVR_SHELLOPTS 134,3)

  foreach(AVR_MCU IN ITEMS ${MCU_LIST})

    list(APPEND reports ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}.report)

    add_avr_executable(NAME ${CHECK_NAME}_${AVR_MCU}
                       MCU ${AVR_MCU}
                       SOURCES ${AVR_SRC}
                       CCOPTS -g -O2)
  
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}.report
                       COMMAND GNUTIME=${GNUTIME} ${CMAKE_CURRENT_SOURCE_DIR}/run_test.sh
                                 ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}.report
                                 ${AVR_SHELLOPTS}
                                 ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}
                                 --
                                 $<TARGET_FILE:simulavr> ${AVR_SIMOPTS}
                                                         -d ${AVR_MCU}
                                                         -f ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}
                       DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU})

  endforeach(AVR_MCU)

  # test exit
  set(CHECK_NAME exit)
  set(AVR_SRC test_exit.c avrtest_help.c)

  set(AVR_SIMOPTS -W 0x52,- -e 0x4F -m 10000000)
  set(AVR_SHELLOPTS 1,1)

  foreach(AVR_MCU IN ITEMS ${MCU_LIST})

    list(APPEND reports ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}.report)

    add_avr_executable(NAME ${CHECK_NAME}_${AVR_MCU}
                       MCU ${AVR_MCU}
                       SOURCES ${AVR_SRC}
                       CCOPTS -g -O2)
  
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}.report
                       COMMAND GNUTIME=${GNUTIME} ${CMAKE_CURRENT_SOURCE_DIR}/run_test.sh
                                 ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}.report
                                 ${AVR_SHELLOPTS}
                                 ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}
                                 --
                                 $<TARGET_FILE:simulavr> ${AVR_SIMOPTS}
                                                         -d ${AVR_MCU}
                                                         -f ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU}
                       DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${CHECK_NAME}_${AVR_MCU})

  endforeach(AVR_MCU)

  add_custom_target(check-avrtest cat ${reports} | sort
                                  COMMENT "avrtest result:"
                                  DEPENDS ${reports})
  add_dependencies(check check-avrtest)

endif(AVR_CC)