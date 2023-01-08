# _`D`e`D`uplication`K`it_ 

![build](https://github.com/FreddyFunk/ddk/actions/workflows/ci.yml/badge.svg?branch=main)
![CodeQL](https://github.com/FreddyFunk/ddk/actions/workflows/codeql.yml/badge.svg?branch=main)
[![codecov](https://codecov.io/gh/FreddyFunk/ddk/branch/main/graph/badge.svg?token=EE4DVMP2I7)](https://codecov.io/gh/FreddyFunk/ddk)


## About
`ddk` is a simple command line tool for finding duplicate files at high speed.

## Long-term plans
`ddk` aims to provide advanced deduplication detection techniques for various file formats like images, video, audio and text in the future.

## Build Instructions
```bash
# Configure CMake
cmake -B ./build -G Ninja

# Build
cd build
cmake --build .

# Run unit tests
ctest
```