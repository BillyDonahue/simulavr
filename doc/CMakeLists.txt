# doc tools
find_program(HELP2MAN help2man)
find_program(MAKEINFO makeinfo)
find_program(DOXYGEN doxygen)
find_program(DOT dot)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.texi.in ${CMAKE_CURRENT_BINARY_DIR}/config.texi)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simulavr.texi ${CMAKE_CURRENT_BINARY_DIR}/simulavr.texi COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.texi ${CMAKE_CURRENT_BINARY_DIR}/version.texi COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog ${CMAKE_CURRENT_BINARY_DIR}/changelog COPYONLY)

add_custom_target(progdoc COMMENT "create program documentation files")

if(HELP2MAN AND GZIP)
  message(STATUS "can create man page")

  set(DOC_MAN ${CMAKE_CURRENT_BINARY_DIR}/simulavr.1.gz)
  
  add_custom_command(OUTPUT ${DOC_MAN}
                     COMMAND ${HELP2MAN} -o ${CMAKE_CURRENT_BINARY_DIR}/simulavr.1 "$<TARGET_FILE:simulavr>"
                     COMMAND ${GZIP} -n9 ${CMAKE_CURRENT_BINARY_DIR}/simulavr.1)
  add_custom_target(doc-man DEPENDS ${DOC_MAN})
  add_dependencies(progdoc doc-man)
  
  install(FILES ${DOC_MAN} DESTINATION "share/man/man1")
else()
  message(STATUS "can't create man page because of:")
  if(NOT GZIP)
    message(STATUS "  - gzip not found")
  endif()
  if(NOT HELP2MAN)
    message(STATUS "  - help2man not found")
  endif()
endif()

if(MAKEINFO AND GZIP)
  message(STATUS "can create info page")

  set(DOC_INFO ${CMAKE_CURRENT_BINARY_DIR}/simulavr.info.gz)

  add_custom_command(OUTPUT ${DOC_INFO}
                     COMMAND ${MAKEINFO} ${CMAKE_CURRENT_BINARY_DIR}/simulavr.texi
                     COMMAND ${GZIP} -n9 ${CMAKE_CURRENT_BINARY_DIR}/simulavr.info)
  add_custom_target(doc-info DEPENDS ${DOC_INFO})
  add_dependencies(progdoc doc-info)

  install(FILES ${DOC_INFO} DESTINATION "share/info")
else()
  message(STATUS "can't create info page because of:")
  if(NOT GZIP)
    message(STATUS "  - gzip not found")
  endif()
  if(NOT MAKEINFO)
    message(STATUS "  - makeinfo not found")
  endif()
endif()

if(GZIP)
  set(DOC_CHANGELOG ${CMAKE_CURRENT_BINARY_DIR}/changelog.gz)

  add_custom_command(OUTPUT ${DOC_CHANGELOG}
                     COMMAND ${GZIP} -n9 ${CMAKE_CURRENT_BINARY_DIR}/changelog)
  add_custom_target(doc-changelog DEPENDS ${DOC_CHANGELOG})
  add_dependencies(progdoc doc-changelog)

  install(FILES ${DOC_CHANGELOG} DESTINATION "share/doc/common")
endif()

add_custom_target(doc COMMENT "create simulavr documentation" DEPENDS progdoc)

if(PYTHON_EXECUTABLE)

  find_python_module(sphinx)
  find_python_module(rst2pdf)
  find_python_module(bs4)
  find_python_module(requests)

  if(PY_SPHINX AND PY_RST2PDF AND PY_BS4 AND PY_REQUESTS)

    add_subdirectory(sphinx)

  endif()
  
endif()

if(DOXYGEN AND DOT)

  add_subdirectory(doxygen)

endif()

# EOF