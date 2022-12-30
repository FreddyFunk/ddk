add_library(ddk_io
    mmap_file_read_only.cpp
    mmap_file_read_only.hpp
)
add_library(${PROJECT_NAME}::io ALIAS ddk_io)

replicate_directory_structure(${PROJECT_NAME}::io)

target_include_directories(ddk_io INTERFACE ./)
