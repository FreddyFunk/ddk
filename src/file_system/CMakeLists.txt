add_library(ddk_file_system
    fsinfo.cpp
    fsinfo.hpp
    fsitem.cpp
    fsitem.hpp
)
add_library(${PROJECT_NAME}::file_system ALIAS ddk_file_system)

replicate_directory_structure(${PROJECT_NAME}::file_system)

target_include_directories(ddk_file_system INTERFACE ./)
