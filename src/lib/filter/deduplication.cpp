#include "common.hpp"
#include "deduplication.hpp"

namespace FSI::FILTER::DEDUPLICATION
{
    void removeFilesWithUniqueSize(std::vector<FileSystemItem*>& items){
        COMMON::sortFSitemsBySize(items);

        std::size_t itemsCount = items.size();
        for (std::size_t i = 0; i < itemsCount; i++){
            const bool previousDifferent = i == 0 ? true : (items[i]->getSizeInBytes() != items[i - 1]->getSizeInBytes());
            const bool nextDifferent = i == itemsCount - 1 ? true : (items[i]->getSizeInBytes() != items[i + 1]->getSizeInBytes());
            if (previousDifferent && nextDifferent){
                items.erase(items.begin() + i);
                i--;
                itemsCount--;
            }
        }
    }

    void tagDuplicateBinaries(std::vector<FileSystemItem*>& items){
        COMMON::onlyFiles(items);
        removeFilesWithUniqueSize(items);
        
        for (std::size_t i = 1; i < items.size() - 1; i++){
            const bool previousIdentical = items[i]->getSizeInBytes() == items[i - 1]->getSizeInBytes();
            const bool nextIdentical = items[i]->getSizeInBytes() == items[i + 1]->getSizeInBytes();
            
            if (previousIdentical)
            {
                items[i]->addDuplicate(items[i - 1]);
                items[i - 1]->addDuplicate(items[i]);
            }
            if (nextIdentical)
            {
                items[i]->addDuplicate(items[i + 1]);
                items[i + 1]->addDuplicate(items[i]);
            }
        }
    }

    std::vector<FSI::Duplicate> getUniqueDuplicates(std::vector<FileSystemItem*>& items){
        std::vector<FSI::Duplicate> duplicates{};

        // collect all duplicates
		for (FileSystemItem* const item : items)
		{
            for (FileSystemItem* const d : item->getDuplicates())
            {
                duplicates.push_back(Duplicate(item, d));
            }
		}
        
        // remove duplicate duplicates
        std::sort(duplicates.begin(), duplicates.end());
        duplicates.erase(std::unique(duplicates.begin(), duplicates.end()), duplicates.end());

		return duplicates;
    }
}
