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

# select scan mode
fsinfo_cli -scan

# sort entries by size via "--sorted"
fsinfo_cli -scan --sorted

# print result as list or as tree view "-v". Default is "list"
fsinfo_cli -scan -v tree

# output content of subdirectories as well with "-o". Default is "current"
fsinfo_cli -scan -o all

# output only files and no subdirectories. Only works in list view mode.
fsinfo_cli -scan --only-files

# select a specific path via "-p"
fsinfo_cli -scan -p ~/Desktop

# limit number of items in the output via "-l"
fsinfo_cli -scan -l 4

# include symlinks. This is deactivated by default. Be careful with this option!
fsinfo_cli -scan --include-symlinks

# select deduplication mode
fsinfo_cli -dedup

# example 1: show the 5 largest items the Downloads directory
fsinfo_cli -scan -p "~/Downloads" -l 5 --sorted

# example 2: find all definite duplicates
fsinfo_cli -dedup -p "~/Downloads"
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