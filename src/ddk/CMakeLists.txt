add_executable(
  ddk
  ddk.cpp
  fsinfo_parser.cpp
  fsinfo_parser.hpp
  input_params_parser.cpp
  input_params_parser.hpp)

add_executable(${PROJECT_NAME}::ddk ALIAS ddk)

set_property(TARGET ddk PROPERTY POSITION_INDEPENDENT_CODE ON)

replicate_directory_structure(${PROJECT_NAME}::ddk)

# link dependencies
target_link_libraries(ddk PRIVATE file_system fmt::fmt-header-only)

set_target_properties(
  ddk
  PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release/ddk"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release/ddk"
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release/ddk")
