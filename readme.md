# _`D`e`D`uplication`K`it_ 

![ubuntu x64](https://github.com/FreddyFunk/ddk/actions/workflows/ci-ubuntu-x64.yml/badge.svg?branch=master)
![macOS](https://github.com/FreddyFunk/ddk/actions/workflows/ci-macos.yml/badge.svg?branch=master)


## About
`ddk` is a simple command line tool for finding duplicate files at high speed.

## Long-term plans
`ddk` aims to provide advanced deduplication detection techniques for various file formats like images, video, audio and text in the future.

## Build Instructions
```bash
# BUILD_TYPE can be "Release" or "Debug"
BUILD_TYPE=Release

# Configure CMake
cmake \
-B ./build \
-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
-G Ninja

# Build
cd build
cmake --build . --config $BUILD_TYPE
```