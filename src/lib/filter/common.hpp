#include "fsitem.hpp"

namespace FSI::FILTER::COMMON
{
    void sortFSitemsBySize(std::vector<FileSystemItem*>& items);
    void onlyFiles(std::vector<FileSystemItem*>& items);
}
