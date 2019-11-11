# build libsim.so

set(libSrcs at4433.cpp at8515.cpp atmega668base.cpp
            atmega128.cpp at90canbase.cpp atmega8.cpp
            atmega1284abase.cpp atmega2560base.cpp
            attiny25_45_85.cpp atmega16_32.cpp
            attiny2313.cpp adcpin.cpp application.cpp
            externalirq.cpp hwusi.cpp avrdevice.cpp
            avrerror.cpp avrfactory.cpp avrmalloc.cpp
            decoder.cpp decoder_trace.cpp flash.cpp
            flashprog.cpp hardware.cpp helper.cpp
            hwacomp.cpp hwad.cpp hweeprom.cpp avrsignature.cpp
            avrreadelf.cpp hwpinchange.cpp hwport.cpp
            hwspi.cpp hwsreg.cpp hwstack.cpp hwuart.cpp
            hwwado.cpp ioregs.cpp irqsystem.cpp net.cpp
            pin.cpp pinatport.cpp pinmon.cpp rwmem.cpp
            spisrc.cpp spisink.cpp specialmem.cpp string2.cpp
            systemclock.cpp traceval.cpp memory.cpp
            hwtimer/timerprescaler.cpp hwtimer/prescalermux.cpp
            hwtimer/timerirq.cpp hwtimer/icapturesrc.cpp
            hwtimer/hwtimer.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/elfio)

configure_file(${CMAKE_SOURCE_DIR}/include/config.h.in ${CMAKE_SOURCE_DIR}/include/config.h)

add_library(sim_objs OBJECT ${libSrcs})
set_property(TARGET sim_objs PROPERTY POSITION_INDEPENDENT_CODE 1)
#target_compile_options(sim_objs PRIVATE -Wall -Wextra)
target_compile_options(sim_objs PRIVATE -Wall)

add_library(sim SHARED $<TARGET_OBJECTS:sim_objs>)

add_library(sim_static $<TARGET_OBJECTS:sim_objs>)

file(GLOB files_common "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB files_hwtimer "${CMAKE_SOURCE_DIR}/include/hwtimer/*.h")
file(GLOB files_gdb "${CMAKE_SOURCE_DIR}/include/gdb/*.h")
file(GLOB files_ui "${CMAKE_SOURCE_DIR}/include/ui/*.h")
file(GLOB files_cmd "${CMAKE_SOURCE_DIR}/include/cmd/*.h")
file(GLOB files_elfio "${CMAKE_SOURCE_DIR}/include/elfio/elfio/*.hpp")

install(TARGETS sim DESTINATION "lib")
install(FILES ${files_common} DESTINATION "include")
install(FILES ${files_hwtimer} DESTINATION "include/hwtimer")
install(FILES ${files_gdb} DESTINATION "include/gdb")
install(FILES ${files_ui} DESTINATION "include/ui")
install(FILES ${files_cmd} DESTINATION "include/cmd")
install(FILES ${files_elfio} DESTINATION "include/elfio/elfio")

if(BUILD_VERILOG)

  set(vvpSrc vpi.cpp)

  if(VERILOG_IVERILOG_HDR)
    include_directories(${VERILOG_IVERILOG_HDR}/iverilog)
  endif(VERILOG_IVERILOG_HDR)

  add_library(avr_vpi SHARED ${vvpSrc})
  target_link_libraries(avr_vpi sim)
  set_target_properties(avr_vpi PROPERTIES OUTPUT_NAME "avr.vpi")
  set_target_properties(avr_vpi PROPERTIES PREFIX "")
  set_target_properties(avr_vpi PROPERTIES SUFFIX "")
  #target_compile_options(avr_vpi PRIVATE -Wall -Wextra)
  #target_compile_options(avr_vpi PRIVATE -Wall)

  install(TARGETS avr_vpi DESTINATION "lib/simulavr")

endif(BUILD_VERILOG)

# EOF