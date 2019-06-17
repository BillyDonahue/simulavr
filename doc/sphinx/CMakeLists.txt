# sphinx documentation
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in ${CMAKE_CURRENT_SOURCE_DIR}/conf.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/web/conf.py.in ${CMAKE_CURRENT_SOURCE_DIR}/web/conf.py)

set(DOC_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(DOC_PDF ${CMAKE_CURRENT_BINARY_DIR}/manual.pdf)
set(DOC_PDF_OPTS -d ${DOC_BUILD_DIR}/doctrees -D latex_paper_size=a4)
add_custom_command(OUTPUT ${DOC_PDF}
                   COMMAND ${PYTHON_EXECUTABLE} loader.py -b pdf ${DOC_PDF_OPTS} . ${DOC_BUILD_DIR}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
                   
set(DOC_HTML ${CMAKE_CURRENT_BINARY_DIR}/html/manual.html)
set(DOC_HTML_OPTS -d ${DOC_BUILD_DIR}/doctrees -D latex_paper_size=a4)
add_custom_command(OUTPUT ${DOC_HTML}
                   COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/html
                   COMMAND ${PYTHON_EXECUTABLE} loader.py -b html ${DOC_PDF_OPTS} . ${DOC_BUILD_DIR}/html
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
                   
set(DOC_WEB ${CMAKE_CURRENT_BINARY_DIR}/web/index.html)
set(DOC_WEB_OPTS -d ${DOC_BUILD_DIR}/doctrees -E -c web)
add_custom_command(OUTPUT ${DOC_WEB}
                   COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/web
                   COMMAND ${PYTHON_EXECUTABLE} loader.py -b html ${DOC_WEB_OPTS} . ${DOC_BUILD_DIR}/web
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
                   
add_custom_target(doc-sphinx DEPENDS ${DOC_PDF} ${DOC_HTML} ${DOC_WEB})

add_dependencies(doc doc-sphinx)

# EOF
