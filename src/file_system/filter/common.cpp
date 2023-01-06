#include "common.hpp"
#include <algorithm>

namespace DDK::FILTER::COMMON {
std::vector<std::vector<FileSystemItem *>> makeClusters(std::vector<FileSystemItem *> &items) {
    std::vector<std::vector<FileSystemItem *>> clusters;

    auto clusterBegin = items.begin();
    while (clusterBegin != items.end()) {
        auto element = *clusterBegin;

        auto clusterEnd = std::find_if(clusterBegin, items.end(), [&](FileSystemItem *item) {
            return item->getHash() != element->getHash();
        });
        clusters.emplace_back(std::vector<FileSystemItem *>(clusterBegin, clusterEnd));

        clusterBegin = clusterEnd;
    }

    return clusters;
}
void sortFSitemsBySize(std::vector<FileSystemItem *> &items) {
    std::sort(items.begin(), items.end(), [](const auto lhs, const auto rhs) {
        return lhs->getSizeInBytes() > rhs->getSizeInBytes();
    });
}
void sortFSitemsByHash(std::vector<FileSystemItem *> &items) {
    std::sort(items.begin(), items.end(),
              [](const auto lhs, const auto rhs) { return lhs->getHash() > rhs->getHash(); });
}
void onlyFiles(std::vector<FileSystemItem *> &items) {
    items.erase(std::remove_if(items.begin(), items.end(),
                               [](const FileSystemItem *fsi) {
                                   return fsi->getItemType() != std::filesystem::file_type::regular;
                               }),
                items.end());
}
void removeEmptyFiles(std::vector<FileSystemItem *> &items) {
    items.erase(
        std::remove_if(items.begin(), items.end(),
                       [](const FileSystemItem *fsi) { return fsi->getSizeInBytes() == 0; }),
        items.end());
}
} // namespace DDK::FILTER::COMMON
