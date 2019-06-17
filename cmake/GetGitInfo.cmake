# Git Version
# nach: http://ipenguin.ws/2012/11/cmake-automatically-use-git-tags-as.html
# nach: https://www.mattkeeter.com/blog/2018-01-06-versioning/
# nach: https://bravenewmethod.com/2017/07/14/git-revision-as-compiler-definition-in-build-with-cmake/

function(get_git_info)

  execute_process(COMMAND /bin/sh ${CMAKE_SOURCE_DIR}/cmake/get_git_version.sh
                  OUTPUT_VARIABLE GIT_RELEASE_STRING
                  OUTPUT_STRIP_TRAILING_WHITESPACE
                  ERROR_QUIET)
  list(GET GIT_RELEASE_STRING 0 GIT_HASH)
  list(GET GIT_RELEASE_STRING 1 GIT_TAG)
  list(GET GIT_RELEASE_STRING 2 GIT_VERSION)
  list(GET GIT_RELEASE_STRING 3 GIT_BRANCH)
  
  string(REPLACE "." ";" GIT_VERSION_SPLIT ${GIT_VERSION})
  list(GET GIT_VERSION_SPLIT 0 _VERSION_MAJOR)
  list(GET GIT_VERSION_SPLIT 1 _VERSION_MINOR)
  list(LENGTH GIT_VERSION_SPLIT GIT_VERSION_LEN)
  if(GIT_VERSION_LEN GREATER 2)
    list(GET GIT_VERSION_SPLIT 2 _VERSION_PATCH)
  else()
    set(_VERSION_PATCH "0")
  endif()

  set(VERSION_MAJOR ${_VERSION_MAJOR} PARENT_SCOPE)
  set(VERSION_MINOR ${_VERSION_MINOR} PARENT_SCOPE)
  set(VERSION_PATCH ${_VERSION_PATCH} PARENT_SCOPE)
  
endfunction(get_git_info)

#update_git_info(TARGET sim)

#function(update_git_info)
#
#  set(opts1 TARGET)
#  cmake_parse_arguments(opts "" "${opts1}" "" ${ARGN})
#
#  add_custom_command(TARGET ${opts_TARGET}
#                     PRE_BUILD
#                     COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/UpdateGitInfo.cmake)
#
#endfunction(update_git_info)

# EOF