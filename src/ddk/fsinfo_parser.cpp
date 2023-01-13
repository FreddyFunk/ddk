#include "fsinfo_parser.hpp"
#include "filter/common.hpp"
#include <cmath>
#include <sstream>

namespace DDK::FSInfoParser {
std::string humanReadableSize(std::uintmax_t size) {
    double mantissa = size;
    int i = 0;
    for (; mantissa >= 1024.0; ++i) {
        mantissa /= 1024.0;
    }
    mantissa = std::ceil(mantissa * 10.0) / 10.0;
    std::stringstream stream;
    stream << mantissa << " "
           << "BKMGTPE"[i] << (i == 0 ? "" : "B");
    return stream.str();
}

// pass std pair here
std::string getItemInfo(std::pair<DDK::FileSystemItem *, bool> item, bool fullPath) {
    std::string itemInfo = item.second ? "[*] " : "[ ] ";

    switch (item.first->getError()) {
    case DDK::FileSystemError::ACCESS_DENIED:
        itemInfo += "ERROR: ACCESS DENIED! ";
        break;
    case DDK::FileSystemError::PATH_DOES_NOT_EXIST:
        itemInfo += "ERROR: PATH DOES NOT EXIST! ";
        break;
    case DDK::FileSystemError::NO_ERROR:
    default:
        break;
    }

    if (fullPath) {
        itemInfo += item.first->getPathAsString();
    } else {
        itemInfo += item.first->getItemName();
    }

    return itemInfo;
}

std::string getDuplicateGroups(
    const std::vector<std::pair<DDK::FileSystemItem *, bool>> &duplicates) {
    std::string duplicateInfo{};

    duplicateInfo +=
        "Duplicate Group with " + std::to_string(duplicates.size()) + " duplicates detected: " +
        humanReadableSize(duplicates.front().first->getSizeInBytes() * duplicates.size()) + " [" +
        std::to_string(duplicates.size()) + " x " +
        humanReadableSize(duplicates.front().first->getSizeInBytes()) + "]\n";
    for (const auto duplicate : duplicates) {
        duplicateInfo += getItemInfo(duplicate, true) + "\n";
    }

    return duplicateInfo;
}

std::vector<std::pair<DDK::FileSystemItem *, bool>> deletableDuplicates(
    std::vector<DDK::FileSystemItem *> duplicates, const std::filesystem::path &path_compare_root) {
    DDK::FILTER::COMMON::sortFSitemsByPathLexicographically(duplicates);
    std::vector<std::pair<DDK::FileSystemItem *, bool>> duplicatesTagged{};
    for (const auto &duplicate : duplicates) {
        if (!DDK::FILTER::COMMON::is_in_sub_directory(duplicate->getPath(), path_compare_root)) {
            duplicatesTagged.push_back(std::make_pair(duplicate, true));
        } else {
            duplicatesTagged.push_back(std::make_pair(duplicate, false));
        }
    }
    return duplicatesTagged;
}

std::vector<std::pair<DDK::FileSystemItem *, bool>> deletableDuplicates(
    std::vector<DDK::FileSystemItem *> duplicates) {
    DDK::FILTER::COMMON::sortFSitemsByPathLexicographically(duplicates);
    std::vector<std::pair<DDK::FileSystemItem *, bool>> duplicatesTagged{};
    // skip first entry of duplicate list since only the remaining entries are duplicates of the
    // first entry
    duplicatesTagged.push_back(std::make_pair(duplicates.at(0), false));
    for (std::size_t i = 1; i < duplicates.size(); i++) {
        duplicatesTagged.push_back(std::make_pair(duplicates.at(i), true));
    }
    return duplicatesTagged;
}

std::string getDuplicateList(const std::vector<DDK::FileSystemItem *> &duplicates) {
    std::string duplicateInfo{};

    for (const auto &duplicate : deletableDuplicates(duplicates)) {
        if (duplicate.second) {
            duplicateInfo += duplicate.first->getPathAsString() + "\n";
        }
    }
    return duplicateInfo;
}

std::string getDuplicateListFromCompare(const std::vector<DDK::FileSystemItem *> &duplicates,
                                        const std::filesystem::path &path_compare_root) {
    std::string duplicateInfo{};

    for (const auto &duplicate : deletableDuplicates(duplicates, path_compare_root)) {
        if (duplicate.second) {
            duplicateInfo += duplicate.first->getPathAsString() + "\n";
        }
    }
    return duplicateInfo;
}

std::string FSinfoDuplicateList(const DDK::FileSystemInfo *const fsinfo) {
    std::string result{};
    auto duplicates = fsinfo->getDuplicates();

    for (const auto duplicate : duplicates) {
        result += getDuplicateList(duplicate);
    }

    return result;
}

std::string FSinfoDuplicateList(const DDK::FileSystemInfo *const fsinfo,
                                const DDK::FileSystemInfo *const compare) {
    std::string result{};
    auto duplicates = fsinfo->getDuplicatesFromCompare(compare);

    for (const auto duplicate : duplicates) {
        result += getDuplicateListFromCompare(duplicate, compare->getRootPath());
    }

    return result;
}

std::uint64_t redundant_size(const std::vector<std::vector<DDK::FileSystemItem *>> &duplicates,
                             const std::filesystem::path &compare_path) {
    std::uint64_t redundant_data_size = 0;
    for (const auto duplicate : duplicates) {
        for (const auto fsi : duplicate) {
            if (!FILTER::COMMON::is_in_sub_directory(fsi->getPath(), compare_path)) {
                redundant_data_size += fsi->getSizeInBytes();
            }
        }
    }
    return redundant_data_size;
}

std::uint64_t redundant_size(const std::vector<std::vector<DDK::FileSystemItem *>> &duplicates) {
    std::uint64_t redundant_data_size = 0;
    for (const auto duplicate : duplicates) {
        redundant_data_size += (duplicate.size() - 1) * duplicate.front()->getSizeInBytes();
    }
    return redundant_data_size;
}

std::string parseDuplicatesDetailed(
    const std::vector<std::vector<DDK::FileSystemItem *>> &duplicates) {
    std::string result{};

    if (duplicates.empty()) {
        return "No duplicates found!\n";
    } else {
        result += "Duplicate Groups found: " + std::to_string(duplicates.size()) + "\n";
        result += "Redundant data: " + humanReadableSize(redundant_size(duplicates)) + "\n\n";
    }

    for (const auto duplicate : duplicates) {
        result += getDuplicateGroups(deletableDuplicates(duplicate)) + "\n";
    }

    return result;
}

std::string parseDuplicatesDetailedCompare(
    const std::vector<std::vector<DDK::FileSystemItem *>> &duplicates,
    const std::filesystem::path &compare_path) {
    std::string result{};

    if (duplicates.empty()) {
        return "No duplicates found!\n";
    } else {
        result += "Duplicate Groups found: " + std::to_string(duplicates.size()) + "\n";
        result += "Redundant data: " + humanReadableSize(redundant_size(duplicates, compare_path)) +
                  "\n\n";
    }

    for (const auto duplicate : duplicates) {
        result += getDuplicateGroups(deletableDuplicates(duplicate, compare_path)) + "\n";
    }

    return result;
}

std::string FSinfoDuplicateListDetailed(const DDK::FileSystemInfo *const fsinfo) {
    return parseDuplicatesDetailed(fsinfo->getDuplicates());
}

std::string FSinfoDuplicateListDetailed(const DDK::FileSystemInfo *const fsinfo,
                                        const DDK::FileSystemInfo *const compare) {
    return parseDuplicatesDetailedCompare(fsinfo->getDuplicatesFromCompare(compare),
                                          compare->getRootPath());
}

std::string summary(const DDK::FileSystemInfo *const fsinfo) {
    std::string result = "Results for: " + fsinfo->getRootPath().string() + "\n";

    result += "Analyzed files: " + std::to_string(fsinfo->getFilesCount()) + "\n";
    result += "Analyzed subdirectories: " + std::to_string(fsinfo->getDirectoriesCount()) + "\n";

    if (fsinfo->symlinks()) {
        result += "Analyzed symlinks: " + std::to_string(fsinfo->getSymlinksCount()) + "\n";
    }

    result += "Analyzed data: " + humanReadableSize(fsinfo->getTotalSize()) + "\n";

    return result;
}

std::string summary(const DDK::FileSystemInfo *const fsinfo,
                    const DDK::FileSystemInfo *const compare) {
    std::string result = "Searching for duplicates of:\n" + compare->getRootPath().string() + "\n";
    result += "in path:\n" + fsinfo->getRootPath().string() + "\n\n";

    result +=
        "Analyzed files: " + std::to_string(fsinfo->getFilesCount() + compare->getFilesCount()) +
        "\n";
    result += "Analyzed subdirectories: " +
              std::to_string(fsinfo->getDirectoriesCount() + compare->getDirectoriesCount()) + "\n";

    if (fsinfo->symlinks()) {
        result += "Analyzed symlinks: " +
                  std::to_string(fsinfo->getSymlinksCount() + compare->getSymlinksCount()) + "\n";
    }

    result +=
        "Analyzed data: " + humanReadableSize(fsinfo->getTotalSize() + compare->getTotalSize()) +
        "\n";

    return result;
}
} // namespace DDK::FSInfoParser
