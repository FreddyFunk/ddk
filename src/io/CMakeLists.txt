add_library(io
    mmap_file_read_only.cpp
    mmap_file_read_only.hpp
)
add_library(${PROJECT_NAME}::io ALIAS io)

replicate_directory_structure(${PROJECT_NAME}::io)

target_include_directories(io INTERFACE ./)
