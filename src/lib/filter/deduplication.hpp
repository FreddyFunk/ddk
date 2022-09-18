#include "duplicate.hpp"
#include "fsitem.hpp"

namespace FSI::FILTER::DEDUPLICATION
{
    void tagDuplicateBinaries(std::vector<FileSystemItem*>& items);
    std::vector<FSI::Duplicate> getUniqueDuplicates(std::vector<FileSystemItem*>& items);
}
