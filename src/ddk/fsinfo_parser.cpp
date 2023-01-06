#include "fsinfo_parser.hpp"
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

std::string getItemInfo(const DDK::FileSystemItem *item, bool fullPath) {
    std::string itemInfo{};

    switch (item->getError()) {
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
        itemInfo += item->getPathAsString();
    } else {
        itemInfo += item->getItemName();
    }

    return itemInfo;
}

std::string getDuplicateInfo(const std::vector<DDK::FileSystemItem *> &duplicates) {
    std::string duplicateInfo{};

    duplicateInfo += "Duplicate Group with " + std::to_string(duplicates.size()) +
                     " duplicates detected: " +
                     humanReadableSize(duplicates.front()->getSizeInBytes() * duplicates.size()) +
                     " [" + std::to_string(duplicates.size()) + " x " +
                     humanReadableSize(duplicates.front()->getSizeInBytes()) + "]\n";
    for (const auto duplicate : duplicates) {
        duplicateInfo += getItemInfo(duplicate, true) + "\n";
    }

    return duplicateInfo;
}

std::string FSinfoDuplicateList(const DDK::FileSystemInfo *const fsinfo) {
    std::string result{};
    auto duplicates = fsinfo->getDuplicates();

    if (duplicates.empty()) {
        return "No duplicates found!\n";
    } else {
        result += "Duplicate Groups found: " + std::to_string(duplicates.size()) + "\n";

        std::size_t redundant_data_size = 0;
        for (const auto duplicate : duplicates) {
            redundant_data_size += getDuplicateInfo(duplicate).size() * (duplicate.size() - 1);
        }
        result += "Redundant data: " + humanReadableSize(redundant_data_size) + "\n\n";
    }

    for (const auto duplicate : duplicates) {
        result += getDuplicateInfo(duplicate) + "\n";
    }

    return result;
}

std::string summary(const DDK::FileSystemInfo *const fsinfo) {
    std::string result{};

    result += "Analyzed files: " + std::to_string(fsinfo->getFilesCount()) + "\n";
    result += "Analyzed subdirectories: " + std::to_string(fsinfo->getDirectoriesCount()) + "\n";

    if (fsinfo->symlinks()) {
        result += "Analyzed symlinks: " + std::to_string(fsinfo->getSymlinksCount()) + "\n";
    }

    result += "Analyzed data: " + humanReadableSize(fsinfo->getTotalSize()) + "\n";

    return result;
}
} // namespace DDK::FSInfoParser
