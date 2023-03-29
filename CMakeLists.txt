cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(
  DDK
  VERSION 2.2.2
  DESCRIPTION "DeDuplicationKit: Advanced File Storage Deduplication"
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Get Git submodule dependencies
include(GitSubmoduleUpdate)

# apply project version
include(GenerateVersion)

# enable IDEs setup pretty project structures
include(ReplicateFolderStructureForIDEs)

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

# Link Time Optimization
include(externals/cmake-scripts/link-time-optimization.cmake)
link_time_optimization()

add_subdirectory(src)

# Tests
option(DDK_TESTS "Build ddk tests" ON)
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MYPROJECT_BUILD_TESTING)
   AND DDK_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()

# Packing
install(FILES ${CMAKE_SOURCE_DIR}/LICENSE.md DESTINATION ./)
install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION ./)
include(Packing)
