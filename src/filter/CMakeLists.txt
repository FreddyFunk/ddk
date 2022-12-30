add_library(filter
    common.cpp
    common.hpp
    deduplication.cpp
    deduplication.hpp
)
add_library(${PROJECT_NAME}::filter ALIAS filter)

replicate_directory_structure(${PROJECT_NAME}::filter)

target_include_directories(filter INTERFACE ./)

target_link_libraries(filter PRIVATE file_system io xxHash::xxhash)
