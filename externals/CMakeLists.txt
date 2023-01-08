# fmt
add_subdirectory(fmt)

## xxHash
set(XXHASH_BUILD_XXHSUM OFF)
add_subdirectory(xxHash/cmake_unofficial EXCLUDE_FROM_ALL)

# portable-memory-mapping
add_subdirectory(portable-memory-mapping)

# cxxopts command line option parser
add_subdirectory(cxxopts)
