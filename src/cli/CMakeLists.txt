add_executable(fsinfo_cli
    cli.cpp
    fsinfo_parser.cpp
    fsinfo_parser.hpp
    input_params_parser.cpp
    input_params_parser.hpp
)

replicate_directory_structure(fsinfo_cli)

# set C++ language standard to c++17
target_compile_features(fsinfo_cli PRIVATE cxx_std_17)

# link dependencies
target_link_libraries(fsinfo_cli PRIVATE fsinfo_lib fmt::fmt)
