add_library(fsinfo_lib
    filter/common.cpp
    filter/common.hpp
    filter/deduplication.cpp
    filter/deduplication.hpp
    filter/duplicate.cpp
    filter/duplicate.hpp
    io/mmap_file_read_only.cpp
    io/mmap_file_read_only.hpp
    fsinfo.cpp
    fsinfo.hpp
    fsitem.cpp
    fsitem.hpp
)

replicate_directory_structure(fsinfo_lib)

target_include_directories(fsinfo_lib INTERFACE ./)

target_link_libraries(fsinfo_lib PRIVATE xxHash::xxhash)
