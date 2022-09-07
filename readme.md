# FSinfo 

_FileSysteminfo_

FSinfo is a simple GUI and command line tool for file system anlysis. It acomplishes the following goals:

* find large files and directories
* advanced file deduplication
* view file system meta data


## Usage Guide
A complete usage guide can be found here.

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