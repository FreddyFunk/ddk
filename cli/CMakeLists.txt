add_executable(fsinfo_cli
    cli.cpp
)

replicate_directory_structure(fsinfo_cli)

# set C++ language standard to c++17
target_compile_features(fsinfo_cli PRIVATE cxx_std_17)
