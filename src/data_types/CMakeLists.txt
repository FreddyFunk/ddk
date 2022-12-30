add_library(ddk_data_types
    image.cpp
    image.hpp
)
add_library(${PROJECT_NAME}::data_types ALIAS ddk_data_types)

replicate_directory_structure(${PROJECT_NAME}::data_types)

target_include_directories(ddk_data_types INTERFACE ./)
