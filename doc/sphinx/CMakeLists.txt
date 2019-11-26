# sphinx documentation
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.py.in ${CMAKE_CURRENT_SOURCE_DIR}/config.py)
file(GLOB rst_srcs "*.rst")

set(USAGE_TXT ${CMAKE_CURRENT_SOURCE_DIR}/usage.txt)
add_custom_command(OUTPUT ${USAGE_TXT}
                   COMMAND SIMULAVR_DOC_RST=1 $<TARGET_FILE:simulavr> -h > ${USAGE_TXT}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(DOWNLOAD_TXT ${CMAKE_CURRENT_SOURCE_DIR}/download.txt)
add_custom_command(OUTPUT ${DOWNLOAD_TXT}
                   COMMAND ${PYTHON_EXECUTABLE} getdownload.py --output ${DOWNLOAD_TXT}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(DOC_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(DOC_PDF ${CMAKE_CURRENT_BINARY_DIR}/manual.pdf)
set(DOC_PDF_OPTS -d ${DOC_BUILD_DIR}/doctrees)
add_custom_command(OUTPUT ${DOC_PDF}
                   COMMAND ${PYTHON_EXECUTABLE} loader.py -b pdf ${DOC_PDF_OPTS} . ${DOC_BUILD_DIR}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   DEPENDS ${rst_srcs} ${USAGE_TXT} ${DOWNLOAD_TXT})
                   
set(DOC_HTML ${CMAKE_CURRENT_BINARY_DIR}/html/index.html)
set(DOC_HTML_OPTS -c html -d ${DOC_BUILD_DIR}/doctrees)
add_custom_command(OUTPUT ${DOC_HTML}
                   COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/html
                   COMMAND ${PYTHON_EXECUTABLE} loader.py -b html ${DOC_PDF_OPTS} . ${DOC_BUILD_DIR}/html
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   DEPENDS ${rst_srcs} ${USAGE_TXT} ${DOWNLOAD_TXT})
                   
set(DOC_WEB ${CMAKE_CURRENT_BINARY_DIR}/web/index.html)
set(DOC_WEB_OPTS -d ${DOC_BUILD_DIR}/doctrees -E -c web)
add_custom_command(OUTPUT ${DOC_WEB}
                   COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/web
                   COMMAND ${PYTHON_EXECUTABLE} loader.py -b html ${DOC_WEB_OPTS} . ${DOC_BUILD_DIR}/web
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   DEPENDS ${rst_srcs} ${USAGE_TXT} ${DOWNLOAD_TXT})
                   
add_custom_target(doc-sphinx DEPENDS ${DOC_PDF} ${DOC_HTML} ${DOC_WEB})

add_dependencies(doc doc-sphinx)

# https://stackoverflow.com/questions/57417808/standardizing-a-link-using-a-global-variable-in-sphinx-documentation
# https://stackoverflow.com/questions/15131613/sphinx-docs-variable
# https://github.com/sphinx-doc/sphinx/issues/2173

# https://www.sphinx-doc.org/en/master/usage/configuration.html#conf-tags
# https://www.sphinx-doc.org/en/master/usage/restructuredtext/directives.html

# EOF
