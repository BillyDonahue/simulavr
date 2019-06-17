# maintainance tool for keytrans.h

set(KBDGENTABLES ${CMAKE_CURRENT_BINARY_DIR}/kbdgentables)
set(src ${CMAKE_CURRENT_SOURCE_DIR}/kbdgentables.cpp)
add_custom_command(OUTPUT ${KBDGENTABLES}
                   COMMAND g++ -o ${KBDGENTABLES} -g -O2 -fPIC ${src}
                   DEPENDS ${src})

set(KEYTRANS_H ${CMAKE_CURRENT_BINARY_DIR}/keytrans.h)
set(datfiles ${CMAKE_CURRENT_SOURCE_DIR}/keynumber_to_scancode.dat
             ${CMAKE_CURRENT_SOURCE_DIR}/xcode_to_keynumber.dat)
add_custom_command(OUTPUT ${KEYTRANS_H}
                   COMMAND ${KBDGENTABLES} ${CMAKE_CURRENT_SOURCE_DIR} ${KEYTRANS_H}
                   DEPENDS ${KBDGENTABLES} ${datfiles})

add_custom_target(keytrans COMMENT "maintainance tool to create keytrans.h"
                           DEPENDS ${KEYTRANS_H})

# EOF