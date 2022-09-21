add_library(fsinfo_lib
    data_types/image.cpp
    data_types/image.hpp
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

replicate_directory_structure(fsinfo_lib)

target_include_directories(fsinfo_lib INTERFACE ./)

target_link_libraries(fsinfo_lib PRIVATE xxHash::xxhash)
