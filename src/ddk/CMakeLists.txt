add_executable(ddk ddk.cpp fsinfo_parser.cpp fsinfo_parser.hpp
                   ${CMAKE_SOURCE_DIR}/resources/icon.rc)

add_executable(${PROJECT_NAME}::ddk ALIAS ddk)

set_property(TARGET ddk PROPERTY POSITION_INDEPENDENT_CODE ON)

replicate_directory_structure(${PROJECT_NAME}::ddk)

# link dependencies
target_link_libraries(ddk PRIVATE file_system fmt::fmt-header-only
                                  cxxopts::cxxopts)

install(
  TARGETS ddk
  EXPORT DDKTargets
  RUNTIME DESTINATION bin)
