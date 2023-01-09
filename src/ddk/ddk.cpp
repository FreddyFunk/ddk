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
        fmt::print("Results for: {}\n", fsinfo->getRootPath().string());
        fmt::print("{}\n", DDK::FSInfoParser::summary(fsinfo));
    }

    std::string result = detailed ? DDK::FSInfoParser::FSinfoDuplicateListDetailed(fsinfo)
                                  : DDK::FSInfoParser::FSinfoDuplicateList(fsinfo);
    fmt::print("{}\n", result);
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

    try {
        result = options.parse(argc, argv);
    } catch (const std::exception &e) {
        printInvalidOptions();
        return 1;
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
    std::filesystem::path sanitized_path;

    if (result.count("p") == 0) {
        sanitized_path = std::filesystem::current_path().string();
    } else if (result.count("p") == 1) {
        std::string path = result["p"].as<std::string>();

        const auto tilde = path.find('~');
        if (tilde != std::string::npos) {
            const std::string home = std::getenv("HOME");
            sanitized_path = path.replace(tilde, 1, home);
        }
    } else {
        fmt::print("ERROR: Multiple paths selected with option \"-p\"\n" +
                   std::to_string(result.count("p")));
        printInvalidOptions();
        return 1;
    }

    if (sanitized_path.empty()) {
        sanitized_path = std::filesystem::current_path().string();
    }

    if (!std::filesystem::exists(sanitized_path)) {
        fmt::print("ERROR: Custom path {} does not exists.\n", sanitized_path.string());
        return 1;
    }

    // Scan file system structure
    const DDK::FileSystemInfo fsinfo(std::filesystem::path(sanitized_path), analyze_symLinks);

    printResultsDedup(&fsinfo, detailed);

    return 0;
}
