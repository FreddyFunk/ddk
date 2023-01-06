cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(DDK
    VERSION 0.0.1
    DESCRIPTION "DeDupKit"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get Git submodule dependencies
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, 
        please checkout submodules manually with \"git submodule update --init --recursive\"")
    endif()
endif()

# This function replicates the directory hierarchy of a target, so that the layout of the files in IDEs match
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

# Clang Format
include(externals/cmake-scripts/formatting.cmake)
file(GLOB_RECURSE ALL_CODE_FILES
    ${PROJECT_SOURCE_DIR}/src/*.[ch]pp
    ${PROJECT_SOURCE_DIR}/src/*.[ch]
    ${PROJECT_SOURCE_DIR}/tests/*.[h]pp
    ${PROJECT_SOURCE_DIR}/tests/*.[h]
)
clang_format(format ${ALL_CODE_FILES})

# CMake Format
file(GLOB_RECURSE CMAKE_FILES
    ${PROJECT_SOURCE_DIR}/CMakeLists.txt
)
cmake_format(cmake-format ${CMAKE_FILES})

# Code Coverage
include(externals/cmake-scripts/code-coverage.cmake)
option(CODE_COVERAGE "Enable coverage reporting" OFF)


add_subdirectory(externals)
add_subdirectory(src)

# Tests
option(DDK_TESTS "Build ddk tests" ON)
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MYPROJECT_BUILD_TESTING) AND DDK_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

