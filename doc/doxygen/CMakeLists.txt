# create doxygen documentation
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simulavr.doxy.in ${CMAKE_CURRENT_BINARY_DIR}/simulavr.doxy)

set(DOC_DOXY ${CMAKE_CURRENT_BINARY_DIR}/html/index.html)
add_custom_command(OUTPUT ${DOC_DOXY} COMMAND ${DOXYGEN} simulavr.doxy)

add_custom_target(doxygen COMMENT "create doxygen documentation" DEPENDS ${DOC_DOXY})

# EOF