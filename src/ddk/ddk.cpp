#include "filter/common.hpp"
#include "fsinfo_parser.hpp"
#include "version.h"
#include <algorithm>
#include <cxxopts.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>

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
    fmt::print("{}", result);
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
    fmt::print("{}", result);
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

    return std::filesystem::canonical(sanitized_path);
}

int main(int argc, char *argv[]) {
    cxxopts::Options options(
        "ddk", "DeDuplicationKit (ddk) is a simple command line tool for finding duplicate files.");

    // clang-format off
    options.add_options()
        ("h,help", "Display this manual and exit", cxxopts::value<bool>()->default_value("false"))
        ("v,version", "Display semantic version of ddk", cxxopts::value<bool>()->default_value("false"))
        ("p,path", "Search for duplicates in specific path \"arg\". When no \"-p arg\" is specified, ddk will default to the current path.", cxxopts::value<std::string>())
        ("c,compare", "Compare content of path \"-p arg\" with content of path \"-c arg\" and lists all duplicates of files within path \"-c arg\" that are found within path \"-p arg\" and are not within path \"-c arg\"", cxxopts::value<std::string>())
        ("d,detailed", "Show detailed information about deduplication scan", cxxopts::value<bool>()->default_value("false"))
        ("l,symlinks", "Follow symbolic links during deduplication scan", cxxopts::value<bool>()->default_value("false"))
        ("r,remove", "Remove duplicates (PERMANENTLY DELETES FILES! USE WITH CAUTION!)", cxxopts::value<bool>()->default_value("false"))
        ("f,force", "Skip user prompt for asking if you really want to delete all duplicates and start deleting files immediately. Can only be used together with option \"-r\".", cxxopts::value<bool>()->default_value("false"))
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
    for (const auto &option : {"h", "v", "p", "c", "d", "l", "r", "f"}) {
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
    const bool compare = result.count("c") == 1;
    const bool remove = result["r"].as<bool>();
    const bool remove_force = result["f"].as<bool>();
    const std::filesystem::path path = getPathFromOption(result, "p");
    const DDK::FileSystemInfo fsinfo(path, analyze_symLinks);
    const DDK::FileSystemInfo *fsinfo_compare;

    if (compare) {
        const std::filesystem::path path_compare = getPathFromOption(result, "c");
        if (path == path_compare) {
            fmt::print(fmt::emphasis::bold | fmt::emphasis::italic | fg(fmt::color::red),
                       "ERROR: If you want to find duplicates in one directory use \"-p\" only.\n");
            return 1;
        }

        if (DDK::FILTER::COMMON::is_in_sub_directory(path, path_compare)) {
            fmt::print(fmt::emphasis::bold | fmt::emphasis::italic | fg(fmt::color::red),
                       "ERROR: Cannot compare \"{}\" to a parent directory \"{}\". This would "
                       "result in 0 duplicates found.\n",
                       path.string(), path_compare.string());
            return 1;
        }

        if (DDK::FILTER::COMMON::is_in_sub_directory(path_compare, path)) {
            fmt::print(
                fmt::emphasis::bold | fmt::emphasis::italic | fg(fmt::color::yellow),
                "WARN: Comparing to a subdirectory will only list duplicates within the path "
                "\"-p\" that are not within the path \"-c\". So here only duplicates found "
                "within \"{}\" that are not located within \"{}\" are listed.",
                path.string(), path_compare.string());
            fmt::print("\n");
        }

        fsinfo_compare = new DDK::FileSystemInfo(path_compare, analyze_symLinks);
        printResultsDedupCompare(&fsinfo, fsinfo_compare, detailed);
    } else {
        printResultsDedup(&fsinfo, detailed);
    }

    // TODO: store duplicates in ddk main to prevent recalculation during delete

    // TODO: Check if any duplicates were found before trying to delete
    if (remove) {
        // safety prompt
        if (!remove_force) {
            std::string proceed = "";
            do {
                fmt::print(
                    fmt::emphasis::bold | fg(fmt::color::red),
                    "Do you want to you want to PERMANENTLY DELETE the listed duplicates? [y/n]: ");
                std::cin >> proceed;
            } while (!std::cin.fail() && proceed != "y" && proceed != "n");
            if (proceed != "y") {
                return 0;
            }
        }

        // remove duplicates
        if (compare) {
            const auto duplicates = fsinfo.getDuplicatesFromCompare(fsinfo_compare);
            for (const auto duplicate : duplicates) {
                for (const auto fsi : duplicate) {
                    if (!DDK::FILTER::COMMON::is_in_sub_directory(fsi->getPath(),
                                                                  fsinfo_compare->getRootPath())) {
                        std::filesystem::remove(fsi->getPath());
                    }
                }
            }
        } else {
            const auto [items, ranges] = fsinfo.getDuplicates();
            std::size_t range_start = 0;
            for (std::size_t range = 0; range < ranges.size(); range++) {
                std::size_t range_end = range_start + ranges.at(range);
                for (std::size_t i = range_start; i < range_end; i++) {
                    std::filesystem::remove(items.at(i)->getPath());
                }
                range_start = range_end;
            }
        }
    }

    if (compare) {
        delete fsinfo_compare;
    }
    return 0;
}
