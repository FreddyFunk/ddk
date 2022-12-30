#include "../fsitem.hpp"

namespace FSI::FILTER::COMMON
{
    std::vector<std::vector<FileSystemItem*>> makeClusters(std::vector<FileSystemItem*>& items);
    void sortFSitemsBySize(std::vector<FileSystemItem*>& items);
    void sortFSitemsByHash(std::vector<FileSystemItem*>& items);
    void onlyFiles(std::vector<FileSystemItem*>& items);
    void removeEmptyFiles(std::vector<FileSystemItem*>& items);
}
