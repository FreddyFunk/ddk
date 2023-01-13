#include "../fsitem.hpp"

namespace DDK::FILTER::COMMON {
bool is_in_sub_directory(std::filesystem::path path, const std::filesystem::path &root);
std::vector<std::vector<FileSystemItem *>> makeClusters(std::vector<FileSystemItem *> &items);
void sortFSitemsBySize(std::vector<FileSystemItem *> &items);
void sortFSitemsByHash(std::vector<FileSystemItem *> &items);
void sortFSitemsByPathLexicographically(std::vector<FileSystemItem *> &items);
void onlyFiles(std::vector<FileSystemItem *> &items);
void removeEmptyFiles(std::vector<FileSystemItem *> &items);
void removeFSItemsWithIdenticalPath(std::vector<FileSystemItem *> &items);
} // namespace DDK::FILTER::COMMON
