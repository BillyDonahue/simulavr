# build libsimulavr.so

set(swigSrc simulavr.i)
set(libSrcs ${swigSrc} keyboard.cpp lcd.cpp mysocket.cpp extpin.cpp scope.cpp
            serialrx.cpp serialtx.cpp ui.cpp ../app/gdbserver.cpp)

include_directories(${TCL_INCLUDE_PATH})
include_directories(${CMAKE_SOURCE_DIR}/include)

set_source_files_properties(${swigSrc} PROPERTIES CPLUSPLUS ON)

swig_add_module(simtcl tcl8 ${libSrcs})

set_target_properties(simtcl PROPERTIES OUTPUT_NAME "simulavr")
swig_link_libraries(simtcl sim ${TCL_LIBRARY})

install(TARGETS simtcl DESTINATION "lib")

# EOF
