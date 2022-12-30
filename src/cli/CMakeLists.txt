add_executable(ddk
    cli.cpp
    fsinfo_parser.cpp
    fsinfo_parser.hpp
    input_params_parser.cpp
    input_params_parser.hpp
)
add_executable(${PROJECT_NAME}::cli ALIAS ddk)

replicate_directory_structure(${PROJECT_NAME}::cli)

# link dependencies
target_link_libraries(ddk PRIVATE ddk_filter fmt::fmt)
