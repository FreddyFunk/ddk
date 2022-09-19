# fmt
add_subdirectory(fmt)

## xxHash
set(XXHASH_BUILD_XXHSUM OFF)
add_subdirectory(xxHash/cmake_unofficial EXCLUDE_FROM_ALL)
