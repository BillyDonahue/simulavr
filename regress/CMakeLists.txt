# run build checks

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/regress.py.in ${CMAKE_CURRENT_BINARY_DIR}/regress.py)

if(PYTHONINTERP_FOUND)

  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/regress.out.success
                     COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/regress.py
                                                  -s "$<TARGET_FILE:simulavr>"
                                                  -o ${CMAKE_CURRENT_BINARY_DIR}/regress.out 2> regress.err
                     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/regress.py)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/regress2650.out.success
                     COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/regress.py
                                                  -s "$<TARGET_FILE:simulavr>"
                                                  -d atmega2560
                                                  -o ${CMAKE_CURRENT_BINARY_DIR}/regress2650.out 2> regress2650.err
                     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/regress.py)

  add_custom_target(check-regress COMMENT "simulavr code regression test"
                                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/regress.out.success
                                          ${CMAKE_CURRENT_BINARY_DIR}/regress2650.out.success)
  
else(PYTHONINTERP_FOUND)

  add_custom_target(check-regress COMMAND /bin/bash -c "echo 'no python interpreter found, can't run regression test!'")

endif(PYTHONINTERP_FOUND)

# tests in avrtest, extinttest etc.
add_subdirectory(avrtest)
add_subdirectory(extinttest)
add_subdirectory(modtest)
add_subdirectory(timertest)
add_subdirectory(tcl)
add_subdirectory(gtest)
add_subdirectory(verilog)

# EOF