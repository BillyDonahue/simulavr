# build _pysimulavr.so
# https://bloerg.net/2012/11/10/cmake-and-distutils.html

set(setup_py ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
set(pysimulavr_i ${CMAKE_CURRENT_BINARY_DIR}/pysimulavr.i)
set(pysimulavr_output ${CMAKE_CURRENT_BINARY_DIR}/timestamp)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in ${setup_py})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pysimulavr.i.in ${pysimulavr_i} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pysimulationmember.h pysimulationmember.h COPYONLY)

add_custom_command(OUTPUT ${pysimulavr_output}
                   COMMAND rm -rf *.so build
                   COMMAND ${PYTHON_EXECUTABLE} ${setup_py} build_ext
                   COMMAND ln -s build/lib*/*.so
                   COMMAND ${CMAKE_COMMAND} -E touch ${pysimulavr_output}
                   DEPENDS ${pysimulavr_i} ${setup_py} ${setup_cfg} $<TARGET_FILE:sim_static>)

add_custom_target(pysimulavr DEPENDS ${pysimulavr_output})

add_custom_target(target ALL DEPENDS pysimulavr)

# EOF
