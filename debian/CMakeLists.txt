# find out, which architecture
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "i686")
  set(PACKAGE_ARCHITECTURE "i386")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
  set(PACKAGE_ARCHITECTURE "amd64")
else()
  message(SEND_ERROR "unknown system architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

message(STATUS "package architecture: ${PACKAGE_ARCHITECTURE}")

set(PACKAGE_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/mk_debian_package.sh)
set(PRE_INSTALL_PATH ${CMAKE_INSTALL_PREFIX})
set(BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(BASE_PATH usr)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mk_debian_package.sh.in ${PACKAGE_SCRIPT} @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libsim.control.in libsim.control)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libsim.triggers.in libsim.triggers)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simulavr.control.in simulavr.control)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simulavr-dev.control.in simulavr-dev.control)

set(PACKAGE_libsim "${BUILD_PATH}/libsim_${PACKAGE_VERSION}_${PACKAGE_ARCHITECTURE}.deb")

add_custom_command(OUTPUT ${PACKAGE_libsim}
                   COMMAND chmod 755 ${PACKAGE_SCRIPT}
                   COMMAND ${PACKAGE_SCRIPT} libsim)

set(PACKAGE_simulavr "${BUILD_PATH}/simulavr_${PACKAGE_VERSION}_${PACKAGE_ARCHITECTURE}.deb")

add_custom_command(OUTPUT ${PACKAGE_simulavr}
                   COMMAND chmod 755 ${PACKAGE_SCRIPT}
                   COMMAND ${PACKAGE_SCRIPT} simulavr)

set(PACKAGE_dev "${BUILD_PATH}/simulavr-dev_${PACKAGE_VERSION}_${PACKAGE_ARCHITECTURE}.deb")

add_custom_command(OUTPUT ${PACKAGE_dev}
                   COMMAND chmod 755 ${PACKAGE_SCRIPT}
                   COMMAND ${PACKAGE_SCRIPT} simulavr-dev)

set(debian_dependencies predebian ${PACKAGE_libsim} ${PACKAGE_simulavr} ${PACKAGE_dev})

if(BUILD_VERILOG)
  set(PACKAGE_vpi "${BUILD_PATH}/simulavr-vpi_${PACKAGE_VERSION}_${PACKAGE_ARCHITECTURE}.deb")

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simulavr-vpi.control.in simulavr-vpi.control)
  
  add_custom_command(OUTPUT ${PACKAGE_vpi}
                     COMMAND chmod 755 ${PACKAGE_SCRIPT}
                     COMMAND ${PACKAGE_SCRIPT} simulavr-vpi)

  list(APPEND debian_dependencies ${PACKAGE_vpi})
endif()

if(BUILD_PYTHON)
  if(${PYTHON_MAJOR_VERSION} EQUAL "3")
    set(PACKAGE_NAME python3-simulavr)
    set(PYTHON_VERSION_NAME "python${PYTHON_MAJOR_VERSION}")
  else()
    set(PACKAGE_NAME python-simulavr)
    set(PYTHON_VERSION_NAME "python${PYTHON_VERSION}")
  endif()
  set(PYTHON_MODULE_PATH "${BASE_PATH}/lib/${PYTHON_VERSION_NAME}/dist-packages")
  
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/python-simulavr.control.in ${PACKAGE_NAME}.control)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/python-simulavr.postinst.in ${PACKAGE_NAME}.postinst @ONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/python-simulavr.prerm.in ${PACKAGE_NAME}.prerm @ONLY)

  set(PACKAGE_python "${BUILD_PATH}/${PACKAGE_NAME}_${PACKAGE_VERSION}_${PACKAGE_ARCHITECTURE}.deb")

  add_custom_command(OUTPUT ${PACKAGE_python}
                     COMMAND chmod 755 ${PACKAGE_SCRIPT}
                     COMMAND ${PACKAGE_SCRIPT} ${PACKAGE_NAME})

  list(APPEND debian_dependencies ${PACKAGE_python})
endif()

if(BUILD_TCL)
  set(PACKAGE_tcl "${BUILD_PATH}/simulavr-tcl_${PACKAGE_VERSION}_${PACKAGE_ARCHITECTURE}.deb")

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simulavr-tcl.control.in simulavr-tcl.control)

  add_custom_command(OUTPUT ${PACKAGE_tcl}
                     COMMAND chmod 755 ${PACKAGE_SCRIPT}
                     COMMAND ${PACKAGE_SCRIPT} simulavr-tcl)

  list(APPEND debian_dependencies ${PACKAGE_tcl})
endif()

add_custom_target(debian COMMENT "build debian packages"
                         DEPENDS ${debian_dependencies})

# EOF