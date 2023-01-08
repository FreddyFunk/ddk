#include "fsinfo_parser.hpp"
#include "input_params_parser.hpp"
#include "version.h"
#include <algorithm>
#include <fmt/core.h>

static void printHelpOptions() {
    fmt::print(fmt::format(
        R"(DeDuplicationKit (ddk) is a simple command line tool for finding duplicate files at high speed.

Usage: ddk [OPTION]...

Options:
-h, --help      display this manual and exit

-v, --version   show version

-p PATH         search for duplicates in PATH (default: current working
                directory)

-f PATH         search for duplicates of files passed by "-f PATH" in 
                "-p PATH"

-d              show detailed information about deduplication scan

-l              follow symbolic links during deduplication scan
                (default: deactivated)‚

-r              remove duplicates (PERMANENTLY DELETES FILES! USE WITH
                CAUTION!)

-ri             go through each duplicate one by one and let the user
                select which files should be deleted

Examples:
ddk
ddk -p "~/Documents" -l
ddk -p "/path/to/files/" -d
ddk -f "/path/to/file1.txt"
ddk -p "./path/to/files/" -f "./path/to/files/file1.txt"
ddk -p "./path/to/files/" -r
ddk -p "./path/to/files/" -ri
)"));
}

static void printVersion() { fmt::print("{}\n", DDK_VERSION); }

static bool getPath(const InputParser *const input, std::filesystem::path &selectedPath) {
    if (!input->cmdOptionExists("-p")) {
        selectedPath = std::filesystem::current_path();
        return true;
    }
    std::string path = input->getCmdOption("-p");
    if (path.empty()) {
        return false;
    }
    const auto tilde = path.find('~');
    if (tilde != std::string::npos) {
        const std::string home = std::getenv("HOME");
        path.replace(tilde, 1, home);
    }
    selectedPath = std::filesystem::path(path);

    return true;
}

static void printResultsDedup(const DDK::FileSystemInfo *const fsinfo) {
    std::string result = DDK::FSInfoParser::FSinfoDuplicateList(fsinfo);

    fmt::print("Results for: {}\n", fsinfo->getRootPath().string());
    fmt::print("{}\n", DDK::FSInfoParser::summary(fsinfo));
    fmt::print("{}\n", result);
}

int main(int argc, char *argv[]) {
    const InputParser input(argc, argv);

    if (input.cmdOptionExists("-h") || input.cmdOptionExists("--help")) {
        printHelpOptions();
        return 0;
    }

    if (input.cmdOptionExists("-v") || input.cmdOptionExists("--version")) {
        printVersion();
        return 0;
    }

    std::filesystem::path path;
    if (!getPath(&input, path)) {
        fmt::print("ERROR: Custom path was choosen (\"-p\"), but no path was providen.\n");
        return 1;
    }
    if (!std::filesystem::exists(path)) {
        fmt::print("ERROR: Custom path {} does not exists.\n", path.string());
        return 1;
    }

    // read general parameters
    const bool analyzeSymLinks = input.cmdOptionExists("--symlinks");

    // Scan file system structure
    const DDK::FileSystemInfo fsinfo(path, analyzeSymLinks);

    printResultsDedup(&fsinfo);

    return 0;
}
