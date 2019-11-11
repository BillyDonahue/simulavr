# build simulavr

set(appSrcs dumpargs.cpp main.cpp gdbserver.cpp
            ../libsimulavr/ui.cpp ../libsimulavr/mysocket.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(simulavr ${appSrcs})
target_link_libraries(simulavr sim)
#target_compile_options(simulavr PRIVATE -Wall -Wextra)
target_compile_options(simulavr PRIVATE -Wall)

install(TARGETS simulavr DESTINATION "bin")

set(docDir ${CMAKE_SOURCE_DIR}/doc)
set(docSrcs ${docDir}/AUTHORS
            ${docDir}/ChangeLog.old
            ${docDir}/copyright
            ${docDir}/NEWS
            ${docDir}/README
            ${docDir}/README.gdb
            ${docDir}/SUPPORT
            ${docDir}/TODO)
install(FILES ${docSrcs} DESTINATION "share/doc/common")

# EOF