#include "fsitem.hpp"

namespace FSI::FILTER::COMMON
{
    void sortFSitemsBySize(std::vector<FileSystemItem*>& items);
    void sortFSitemsByHash(std::vector<FileSystemItem*>& items);
    void onlyFiles(std::vector<FileSystemItem*>& items);
    void removeEmptyFiles(std::vector<FileSystemItem*>& items);
}
