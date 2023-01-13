#include "deduplication.hpp"
#include "MemoryMapped.h"
#include "common.hpp"
#include <algorithm>

#include "xxh3.h"
namespace DDK::FILTER::DEDUPLICATION {
XXH64_hash_t hashMappedMemory(const std::filesystem::path &path) {
    MemoryMapped file(path.string(), MemoryMapped::MapRange::WholeFile,
                      MemoryMapped::CacheHint::SequentialScan);
    XXH64_hash_t hash = XXH64(file.getData(), file.size(), 0);
    return hash;
}

void calculateHashValues(std::vector<FileSystemItem *> &items) {
    for (FileSystemItem *const item : items) {
        item->setHash(hashMappedMemory(item->getPath()));
    }
}

void removeFilesWithUniqueSize(std::vector<FileSystemItem *> &items) {
    COMMON::sortFSitemsBySize(items);

    std::size_t itemsCount = items.size();
    for (std::size_t i = 0; i < itemsCount; i++) {
        const bool previousDifferent =
            i == 0 ? true : (items.at(i)->getSizeInBytes() != items.at(i - 1)->getSizeInBytes());
        const bool nextDifferent =
            i == itemsCount - 1
                ? true
                : (items.at(i)->getSizeInBytes() != items.at(i + 1)->getSizeInBytes());
        if (previousDifferent && nextDifferent) {
            items.erase(items.begin() + i);
            i--;
            itemsCount--;
        }
    }
}

void removeFilesWithUniqueHash(std::vector<FileSystemItem *> &items) {
    COMMON::sortFSitemsByHash(items);

    std::size_t itemsCount = items.size();
    for (std::size_t i = 0; i < itemsCount; i++) {
        const bool previousDifferent =
            i == 0 ? true : (items.at(i)->getHash() != items.at(i - 1)->getHash());
        const bool nextDifferent =
            i == itemsCount - 1 ? true : (items.at(i)->getHash() != items.at(i + 1)->getHash());
        if (previousDifferent && nextDifferent) {
            items.erase(items.begin() + i);
            i--;
            itemsCount--;
        }
    }
}

void tagDuplicateBinaries(std::vector<FileSystemItem *> &items) {
    COMMON::onlyFiles(items);
    COMMON::removeEmptyFiles(items);
    removeFilesWithUniqueSize(items);
    calculateHashValues(items);
    removeFilesWithUniqueHash(items);

    for (std::size_t i = 0; i < items.size(); i++) {
        std::size_t range = i;
        while (range < items.size() - 1 &&
               items.at(i)->getHash() == items.at(range + 1)->getHash()) {
            range++;
        }

        for (std::size_t outer = 0; outer < range - i; outer++) {
            for (std::size_t inner = 0; inner < range - i; inner++) {
                if (inner != outer) {
                    items.at(i + inner)->addDuplicate(items.at(i + outer));
                    items.at(i + outer)->addDuplicate(items.at(i + inner));
                }
            }
        }

        i += range - i;
    }
}

void removeDuplicatesNotContainingDuplicatesFromBothPaths(
    std::vector<std::vector<FileSystemItem *>> &duplicates,
    const std::filesystem::path &p1,
    const std::filesystem::path &p2) {
    // compared to itself
    if (p1 == p2) {
        return;
    }

    const bool is_p2_base_of_p1 = COMMON::is_in_sub_directory(p1, p2);
    const bool is_p1_base_of_p2 = COMMON::is_in_sub_directory(p2, p1);

    // compared to a subdirectory
    if (is_p1_base_of_p2 || is_p2_base_of_p1) {
        const std::filesystem::path base_dir = is_p2_base_of_p1 ? p2 : p1;
        const std::filesystem::path sub_dir = is_p2_base_of_p1 ? p1 : p2;
        duplicates.erase(
            std::remove_if(
                duplicates.begin(), duplicates.end(),
                [&](const auto &duplicate) {
                    bool has_item_in_base_dir = false;
                    bool has_item_in_sub_dir = false;
                    for (const auto fsi : duplicate) {
                        if (COMMON::is_in_sub_directory(fsi->getPath(), sub_dir)) {
                            has_item_in_sub_dir = true;
                        } else if (COMMON::is_in_sub_directory(fsi->getPath(), base_dir) &&
                                   !COMMON::is_in_sub_directory(fsi->getPath(), sub_dir)) {
                            has_item_in_base_dir = true;
                        }
                    }
                    return !(has_item_in_sub_dir && has_item_in_base_dir);
                }),
            duplicates.end());
        return;
    }

    duplicates.erase(std::remove_if(duplicates.begin(), duplicates.end(),
                                    [&](const auto &duplicate) {
                                        bool has_item_in_p1 = false;
                                        bool has_item_in_p2 = false;
                                        for (const auto fsi : duplicate) {
                                            bool is_in_p1 =
                                                COMMON::is_in_sub_directory(fsi->getPath(), p1);
                                            if (is_in_p1) {
                                                has_item_in_p1 = true;
                                            } else {
                                                has_item_in_p2 = true;
                                            }
                                        }
                                        return !(has_item_in_p1 && has_item_in_p2);
                                    }),
                     duplicates.end());
}

std::vector<std::vector<FileSystemItem *>> getDuplicateClusters(
    const std::vector<FileSystemItem *> &items) {
    std::vector<FileSystemItem *> duplicates{};

    // collect all duplicates
    for (auto item : items) {
        duplicates.push_back(item);
        const auto &taggedDuplicates = item->getDuplicates();
        duplicates.insert(duplicates.end(), taggedDuplicates.begin(), taggedDuplicates.end());
    }

    // remove redundant duplicates
    std::sort(duplicates.begin(), duplicates.end());
    duplicates.erase(std::unique(duplicates.begin(), duplicates.end()), duplicates.end());

    COMMON::sortFSitemsByHash(duplicates);

    return COMMON::makeClusters(duplicates);
}

std::vector<std::vector<FileSystemItem *>> getDuplicateClustersSorted(
    const std::vector<FileSystemItem *> &items) {
    auto clusters = getDuplicateClusters(items);

    for (auto &cluster : clusters) {
        std::sort(cluster.begin(), cluster.end(), [](const auto lhs, const auto rhs) {
            if (lhs->getRelativeDirDepth() == rhs->getRelativeDirDepth()) {
                return lhs->getPath().filename().string().size() >
                       rhs->getPath().filename().string().size();
            } else {
                return lhs->getRelativeDirDepth() < rhs->getRelativeDirDepth();
            }
        });
    }

    return clusters;
}
} // namespace DDK::FILTER::DEDUPLICATION
