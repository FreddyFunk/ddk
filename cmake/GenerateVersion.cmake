# Template version.h
configure_file("${PROJECT_SOURCE_DIR}/include/version.h.in"
               "${PROJECT_BINARY_DIR}/include/version.h")

# Make templated version.h available for building
include_directories("${PROJECT_BINARY_DIR}/include")

include_directories("${PROJECT_BINARY_DIR}/include")