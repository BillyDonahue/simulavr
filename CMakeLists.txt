# ToDo: what's the oldest possible version?
cmake_minimum_required(VERSION 3.5)

project(Simulavr)

# path to find simulavr specific cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# include additional modules
include(SimulAVRFunctions)
include(FindPythonModule)
include(GetGitInfo)

# get git info
get_git_info()
set(PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message(STATUS "version major ${VERSION_MAJOR} / minor ${VERSION_MINOR} / patch ${VERSION_PATCH}")

# build options
option(BUILD_TCL "build TCL SWIG module" OFF)
option(BUILD_PYTHON "build Python SWIG module" OFF)
option(BUILD_VERILOG "build VERILOG module" OFF)
option(CHECK_VALGRIND "run gtest regression test with valgrind" OFF)

# find tools
# find python executable, for Python3 you need a virtual env, which links python to python3 executable!
find_package(PythonInterp 3.5)
execute_process(COMMAND ${PYTHON_EXECUTABLE}
                        -c "import sys; print('%s.%s' % (sys.version_info.major, sys.version_info.minor))"
                        OUTPUT_VARIABLE PYTHON_VERSION
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX MATCH "[0-9]" PYTHON_MAJOR_VERSION ${PYTHON_VERSION})

# SWIG
if(BUILD_TCL OR BUILD_PYTHON)
  find_package(SWIG REQUIRED)
  include(${SWIG_USE_FILE})
endif(BUILD_TCL OR BUILD_PYTHON)
# TCL
if(BUILD_TCL)
  find_package(TclStub REQUIRED)
endif()
if(BUILD_PYTHON)
  find_package(PythonLibs REQUIRED)
endif(BUILD_PYTHON)
if(BUILD_VERILOG)
  find_program(VVP vvp)
  if(NOT VVP)
    message(SEND_ERROR "verilog runtime vvp not found")
  endif()
  find_program(IVERILOG iverilog)
  if(NOT IVERILOG)
    message(SEND_ERROR "verilog compiler iverilog not found")
  endif()
  #find_program(GTKWAVE gtkwave)
  find_path(VERILOG_HDR NAMES vpi_user.h)
  find_path(VERILOG_IVERILOG_HDR NAMES iverilog/vpi_user.h)
  if(NOT VERILOG_HDR AND NOT VERILOG_IVERILOG_HDR)
    message(SEND_ERROR "verilog header files not found")
  endif()
endif(BUILD_VERILOG)

# avr toolchain
find_program(AVR_CC avr-gcc)
# gzip
find_program(GZIP gzip)

# valgrind tool to detect memory problems
find_program(VALGRIND valgrind)

# c++ standard
set(CMAKE_CXX_STANDARD 11)

# subdirectories
add_subdirectory(libsim)
add_subdirectory(app)
if(BUILD_TCL)
  add_subdirectory(libsimulavr)
endif()
if(BUILD_PYTHON)
  add_subdirectory(pysimulavr)
endif()
add_subdirectory(doc)
add_subdirectory(kbdgentables)

# check targets
add_custom_target(check COMMENT "run all regression tests")
add_subdirectory(regress)
add_dependencies(check check-regress)
if(BUILD_PYTHON)
  add_dependencies(check check-extinttest check-modtest check-timertest)
endif(BUILD_PYTHON)
if(BUILD_TCL)
  add_dependencies(check check-tcltest)
endif(BUILD_TCL)
if(BUILD_VERILOG)
  add_dependencies(check check-verilog)
endif(BUILD_VERILOG)

# debian package target
find_program(DPKG dpkg)
find_program(STRIP strip)
find_program(FAKEROOT fakeroot)

if(DPKG AND GZIP AND STRIP AND FAKEROOT)
  message(STATUS "can build debian package")
  set(BUILD_DEB ON)
else()
  message(STATUS "can't build debian package because of:")
  set(BUILD_DEB OFF)
  if(NOT DPKG)
    message(STATUS "  - dpkg not found")
  endif()
  if(NOT GZIP)
    message(STATUS "  - gzip not found")
  endif()
  if(NOT STRIP)
    message(STATUS "  - strip not found")
  endif()
  if(NOT FAKEROOT)
    message(STATUS "  - fakeroot not found")
  endif()
endif()

if(BUILD_DEB)
  add_subdirectory(debian)
  add_custom_target(predebian COMMENT "install files before build debian packages")
  add_custom_command(TARGET predebian POST_BUILD
                     COMMAND ${CMAKE_COMMAND} --build . --target progdoc --config $<CONFIGURATION>
                     COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIGURATION>)
endif()

# EOF
