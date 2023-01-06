add_library(
  file_system STATIC
  filter/common.cpp
  filter/common.hpp
  filter/deduplication.cpp
  filter/deduplication.hpp
  fsinfo.cpp
  fsinfo.hpp
  fsitem.cpp
  fsitem.hpp)
add_library(${PROJECT_NAME}::file_system ALIAS file_system)

target_code_coverage(file_system)

set_property(TARGET file_system PROPERTY POSITION_INDEPENDENT_CODE ON)

replicate_directory_structure(${PROJECT_NAME}::file_system)

target_include_directories(file_system INTERFACE ./)

target_link_libraries(file_system PRIVATE xxHash::xxhash
                                          portable-memory-mapping)
