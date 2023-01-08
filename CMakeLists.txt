cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(
  DDK
  VERSION 1.0.0
  DESCRIPTION "DeDuplicationKit (ddk)"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get Git submodule dependencies
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
  execute_process(
    COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE GIT_SUBMOD_RESULT)
  if(NOT GIT_SUBMOD_RESULT EQUAL "0")
    message(
      FATAL_ERROR
        "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT},
        please checkout submodules manually with \"git submodule update --init --recursive\""
    )
  endif()
endif()

# apply project version
include(cmake/GenerateVersion.cmake)

# This function replicates the directory hierarchy of a target, so that the
# layout of the files in IDEs match
function(replicate_directory_structure target_name)
  # Add any files that are not in the source list to a separate group
  source_group("other files" REGULAR_EXPRESSION ".")
  get_target_property(target_sources "${target_name}" SOURCES)

  foreach(file_name IN LISTS target_sources)
    get_filename_component(directory_name "${file_name}" PATH)
    # Group names use '\\' instead of '/'
    string(REPLACE "/" "\\" group_name "${directory_name}")
    source_group("${group_name}" FILES "${file_name}")
  endforeach()
endfunction()

if(CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang" OR CMAKE_CXX_COMPILER_ID
                                                     MATCHES "(Apple)?[Cc]lang")
  set(CMAKE_OSX_ARCHITECTURES
      "arm64;x86_64"
      CACHE STRING "" FORCE)
endif()

# Code Coverage
include(externals/cmake-scripts/code-coverage.cmake)
option(CODE_COVERAGE "Enable coverage reporting" OFF)

add_subdirectory(externals)
add_subdirectory(src)

# Tests
option(DDK_TESTS "Build ddk tests" ON)
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MYPROJECT_BUILD_TESTING)
   AND DDK_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()
