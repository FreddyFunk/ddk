include(${CMAKE_SOURCE_DIR}/externals/cmake-scripts/code-coverage.cmake)
add_code_coverage()

add_subdirectory(ddk)
add_subdirectory(file_system)
