#include "../fsitem.hpp"

namespace DDK::FILTER::DEDUPLICATION
{
    void tagDuplicateBinaries(std::vector<FileSystemItem*>& items);
    std::vector<std::vector<FileSystemItem*>> getDuplicateClusters(const std::vector<FileSystemItem*>& items);
    std::vector<std::vector<FileSystemItem*>> getDuplicateClustersSorted(const std::vector<FileSystemItem*>& items);
}
