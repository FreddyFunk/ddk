add_library(fsinfo_lib
    fsinfo.cpp
    fsinfo.hpp
    fsitem.cpp
    fsitem.hpp
)

replicate_directory_structure(fsinfo_lib)

target_include_directories(fsinfo_lib INTERFACE ./)

# set C++ language standard to c++17
target_compile_features(fsinfo_lib PRIVATE cxx_std_17)
