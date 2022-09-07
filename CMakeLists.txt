cmake_minimum_required(VERSION 3.9...3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# CMake versions older than 3.12 do not support version range
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(FSinfo VERSION 1.0.0
                  DESCRIPTION "File System Info"
                  LANGUAGES CXX)

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

add_subdirectory(lib)
add_subdirectory(cli)

target_link_libraries(fsinfo_cli PRIVATE fsinfo_lib)
