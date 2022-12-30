add_library(ddk_filter
    common.cpp
    common.hpp
    deduplication.cpp
    deduplication.hpp
)
add_library(${PROJECT_NAME}::filter ALIAS ddk_filter)

replicate_directory_structure(${PROJECT_NAME}::filter)

target_include_directories(ddk_filter INTERFACE ./)

target_link_libraries(ddk_filter PRIVATE ddk_file_system ddk_io xxHash::xxhash)
