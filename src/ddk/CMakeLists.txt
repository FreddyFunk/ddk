add_executable(
  ddk
  ddk.cpp
  fsinfo_parser.cpp
  fsinfo_parser.hpp
  input_params_parser.cpp
  input_params_parser.hpp)

add_executable(${PROJECT_NAME}::ddk ALIAS ddk)

replicate_directory_structure(${PROJECT_NAME}::ddk)

# link dependencies
target_link_libraries(ddk PRIVATE file_system fmt::fmt)
