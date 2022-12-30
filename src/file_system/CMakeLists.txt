add_library(file_system
    filter/common.cpp
    filter/common.hpp
    filter/deduplication.cpp
    filter/deduplication.hpp
    io/mmap_file_read_only.cpp
    io/mmap_file_read_only.hpp
    fsinfo.cpp
    fsinfo.hpp
    fsitem.cpp
    fsitem.hpp
)
add_library(${PROJECT_NAME}::file_system ALIAS file_system)

replicate_directory_structure(${PROJECT_NAME}::file_system)

target_include_directories(file_system INTERFACE ./)

target_link_libraries(file_system PRIVATE xxHash::xxhash)
