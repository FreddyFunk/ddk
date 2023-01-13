#include "../fsitem.hpp"

namespace DDK::FILTER::DEDUPLICATION {
void tagDuplicateBinaries(std::vector<FileSystemItem *> &items);
void removeDuplicatesNotContainingDuplicatesFromBothPaths(
    std::vector<std::vector<FileSystemItem *>> &duplicates,
    const std::filesystem::path &p1,
    const std::filesystem::path &p2);
std::vector<std::vector<FileSystemItem *>> getDuplicateClusters(
    const std::vector<FileSystemItem *> &items);
std::vector<std::vector<FileSystemItem *>> getDuplicateClustersSorted(
    const std::vector<FileSystemItem *> &items);
} // namespace DDK::FILTER::DEDUPLICATION
