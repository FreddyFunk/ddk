#include "filter/common.hpp"
#include "fsinfo_parser.hpp"
#include "version.h"
#include <algorithm>
#include <cxxopts.hpp>
#include <fmt/core.h>

static void printVersion() { fmt::print("{}\n", DDK_VERSION); }

static void printInvalidOptions() {
    fmt::print("ERROR: Illegal options. Use \"ddk -h\" for help.\n");
}

static void printResultsDedup(const DDK::FileSystemInfo *const fsinfo, const bool detailed) {
    if (detailed) {
        fmt::print("{}\n", DDK::FSInfoParser::summary(fsinfo));
    }

    const std::string result = detailed ? DDK::FSInfoParser::FSinfoDuplicateListDetailed(fsinfo)
                                        : DDK::FSInfoParser::FSinfoDuplicateList(fsinfo);
    fmt::print("{}\n", result);
}

static void printResultsDedupCompare(const DDK::FileSystemInfo *const fsinfo,
                                     const DDK::FileSystemInfo *const compare,
                                     bool detailed) {
    if (detailed) {
        fmt::print("{}\n", DDK::FSInfoParser::summary(fsinfo, compare));
    }

    const std::string result = detailed
                                   ? DDK::FSInfoParser::FSinfoDuplicateListDetailed(fsinfo, compare)
                                   : DDK::FSInfoParser::FSinfoDuplicateList(fsinfo, compare);
    fmt::print("{}\n", result);
}

static std::filesystem::path getPathFromOption(const cxxopts::ParseResult &result,
                                               const std::string option) {
    std::filesystem::path sanitized_path;

    if (result.count(option) == 0) {
        return std::filesystem::current_path().string();
    }
    std::string path = result[option].as<std::string>();

    const auto tilde = path.find('~');
    if (tilde != std::string::npos) {
        const std::string home = std::getenv("HOME");
        path.replace(tilde, 1, home);
    }

    sanitized_path = std::filesystem::absolute(std::filesystem::path(path)).lexically_normal();

    if (sanitized_path.empty()) {
        sanitized_path = std::filesystem::current_path().string();
    }

    if (!std::filesystem::exists(sanitized_path)) {
        fmt::print("ERROR: Custom path {} does not exists.\n", sanitized_path.string());
        exit(1);
    }

    return sanitized_path;
}

int main(int argc, char *argv[]) {
    cxxopts::Options options(
        "ddk", "DeDuplicationKit (ddk) is a simple command line tool for finding duplicate files.");

    // clang-format off
    options.add_options()
        ("h,help", "Display this manual and exit", cxxopts::value<bool>()->default_value("false"))
        ("v,version", "Display semantic version of ddk", cxxopts::value<bool>()->default_value("false"))
        ("p,path", "Search for duplicates in specific path \"arg\"", cxxopts::value<std::string>())
        ("c,compare", "Compare content of path \"-p arg\" with content of path \"-c arg\" and lists all duplicates found in path \"-p arg\"", cxxopts::value<std::string>())
        ("d,detailed", "Show detailed information about deduplication scan", cxxopts::value<bool>()->default_value("false"))
        ("l,symlinks", "Follow symbolic links during deduplication scan", cxxopts::value<bool>()->default_value("false"))
        ("r,remove", "Remove duplicates (PERMANENTLY DELETES FILES! USE WITH CAUTION!)", cxxopts::value<bool>()->default_value("false"))
        ("i,remove-interactive", "Go through each duplicate one by one and let the user select which files should be deleted", cxxopts::value<bool>()->default_value("false"))
        ;
    // clang-format on

    cxxopts::ParseResult result;

    // Check for invalid options
    try {
        result = options.parse(argc, argv);
    } catch (const std::exception &e) {
        printInvalidOptions();
        return 1;
    }

    // do not allow duplicate options
    for (const auto &option : {"h", "v", "p", "c", "d", "l", "r", "i"}) {
        if (result.count(option) > 1) {
            printInvalidOptions();
            return 1;
        }
    }

    if (result["h"].as<bool>()) {
        fmt::print(options.help());
        return 0;
    }

    if (result["v"].as<bool>()) {
        printVersion();
        return 0;
    }

    const bool analyze_symLinks = result["l"].as<bool>();
    const bool detailed = result["d"].as<bool>();
    const bool remove = result["r"].as<bool>();
    const bool remove_interactive = result["i"].as<bool>();
    const std::filesystem::path path = getPathFromOption(result, "p");
    const DDK::FileSystemInfo fsinfo(path, analyze_symLinks);

    if (result.count("c") == 1) {
        const std::filesystem::path path_compare = getPathFromOption(result, "c");
        if (path == path_compare) {
            fmt::print(
                "If you want to find duplicates in one directory use \"-p\" only. Aborting...\n");
            return 1;
        }

        if (DDK::FILTER::COMMON::is_sub_directory(path, path_compare) ||
            DDK::FILTER::COMMON::is_sub_directory(path_compare, path)) {
            fmt::print("WARN: Comparing to a subdirectory is not recommended due to "
                       "exponentionally increasing resource requirements.\n");
        }

        const DDK::FileSystemInfo compare(path_compare, analyze_symLinks);
        printResultsDedupCompare(&fsinfo, &compare, detailed);
        return 0;
    } else {
        printResultsDedup(&fsinfo, detailed);
        return 0;
    }
}
