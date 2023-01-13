# _`D`e`D`uplication`K`it_ 

![Build & Test](https://github.com/FreddyFunk/ddk/actions/workflows/build.yml/badge.svg?branch=main)
![CodeQL](https://github.com/FreddyFunk/ddk/actions/workflows/codeql.yml/badge.svg?branch=main)
[![Code Coverage](https://codecov.io/gh/FreddyFunk/ddk/branch/main/graph/badge.svg?token=EE4DVMP2I7)](https://codecov.io/gh/FreddyFunk/ddk)


## About
`ddk` is a simple command line tool for finding duplicate files.

## Long-term plans
`ddk` aims to provide advanced deduplication detection techniques for various file formats like images, video, audio and text in the future.

## Usage
```
ddk [OPTION...]

-h, --help         Display this manual and exit
-v, --version      Display semantic version of ddk
-p, --path arg     Search for duplicates in specific path "arg". When no 
                    "-p arg" is specified, ddk will default to the current 
                    path.
-c, --compare arg  Compare content of path "-p arg" with content of path 
                    "-c arg" and lists all duplicates of files within path 
                    "-c arg" that are found within path "-p arg" and are 
                    not within path "-c arg"
-d, --detailed     Show detailed information about deduplication scan
-l, --symlinks     Follow symbolic links during deduplication scan
-r, --remove       Remove duplicates (PERMANENTLY DELETES FILES! USE WITH 
                    CAUTION!)
-f, --force        Skip user prompt for asking if you really want to 
                    delete all duplicates and start deleting files 
                    immediately. Can only be used together with option 
                    "-r".
```

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