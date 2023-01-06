#include "../fsitem.hpp"

namespace DDK::FILTER::COMMON {
std::vector<std::vector<FileSystemItem *>> makeClusters(std::vector<FileSystemItem *> &items);
void sortFSitemsBySize(std::vector<FileSystemItem *> &items);
void sortFSitemsByHash(std::vector<FileSystemItem *> &items);
void onlyFiles(std::vector<FileSystemItem *> &items);
void removeEmptyFiles(std::vector<FileSystemItem *> &items);
} // namespace DDK::FILTER::COMMON
