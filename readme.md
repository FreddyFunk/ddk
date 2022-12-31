# _`D`e`D`uplication`K`it_ 

`ddk` is a simple command line tool for finding duplicate files at high speed. `ddk` also provides advanced deduplication detection techniques for various file formats like images, video, audio and text.


## Build Instructions
### Building the command line version
### Building on macOS
```bash
# Install dependencies
brew install cmake ninja

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