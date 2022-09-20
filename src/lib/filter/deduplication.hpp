#include "duplicate.hpp"
#include "fsitem.hpp"

namespace FSI::FILTER::DEDUPLICATION
{
    void tagDuplicateBinaries(std::vector<FileSystemItem*>& items);
    std::vector<std::vector<FileSystemItem*>> getUniqueDuplicates(std::vector<FileSystemItem*>& items);
}
