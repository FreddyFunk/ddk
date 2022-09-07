# FSinfo 

_FileSysteminfo_

FSinfo is a simple GUI and command line tool for file system anlysis. It acomplishes the following goals:

* find large files and directories
* advanced file deduplication
* view file system meta data


## Usage Guide
```bash
# list all files and directories in your current working directory
fsinfo_cli

# sort entries by size via "-s"
fsinfo_cli -s

# print result as list or as tree view "-v". Default is "list"
fsinfo_cli -v tree

# output content of subdirectories as well with "-o". Default is "current"
fsinfo_cli -o all

# select a specific path via "-p"
fsinfo_cli -p ~/Desktop

# limit number of items in the output via "-l"
fsinfo_cli -l 4

# include symlinks via "-symlinks". This is deactivated by default. Be careful with this option!
fsinfo_cli --include-symlinks

# example 1: show the 5 largest items the Downloads directory
fsinfo_cli -p ~/Downloads -s -l 5
```

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