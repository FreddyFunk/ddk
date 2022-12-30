add_library(data_types
    image.cpp
    image.hpp
)
add_library(${PROJECT_NAME}::data_types ALIAS data_types)

replicate_directory_structure(${PROJECT_NAME}::data_types)

target_include_directories(data_types INTERFACE ./)
